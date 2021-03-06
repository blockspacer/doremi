// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/ServerStateHandler.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Standard
#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>
#include <time.h>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerServer::NetworkManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "NetworkManagerServer"),
              m_timeoutIntervalConnecting(5.0f),
              m_timeoutIntervalConnected(5.0f),
              m_timeoutIntervalMaster(5.0f),
              m_masterNextUpdateTimer(0.0f),
              m_masterUpdateInterval(0.5f),
              m_maxConnectedMessagesPerFrame(20),
              m_maxConnectingMessagesPerFrame(10),
              m_maxAcceptConnectionsPerFrame(5)
        {
            // Startup network messages and connection, TODOCM could change position of this
            NetworkMessagesServer::StartupNetworkMessagesServer(p_sharedContext);
            NetworkConnectionsServer::StartupNetworkConnectionsServer(p_sharedContext);

            srand(time(NULL));
        }

        NetworkManagerServer::~NetworkManagerServer() {}

        void NetworkManagerServer::Update(double p_dt)
        {
            // Receive Messages
            ReceiveMessages();

            // Send Messages for connected clients
            SendConnectedMessages();

            // Send messges to master
            SendMasterMessages(p_dt);

            // Check for connections TODOCM not sure if I want this if we going for UDP only
            CheckForConnections();

            // Update timeouts
            UpdateTimeouts(p_dt);
        }

        void NetworkManagerServer::ReceiveMessages()
        {
            // For some incomming connecting Received messages we send one
            ReceiveConnectingMessages();

            // Receive connecting messages from connected clients
            ReceiveConnectedMessages();

            // Receive messages from master
            ReceiveMasterMessages();
        }

        void NetworkManagerServer::ReceiveConnectingMessages()
        {
            // Get Nework module
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Get message class
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Create adress we can use, we don't know the incomming adress before we receive message
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Get the socket used for connecting
            SocketHandle t_connectingSocketHandle = NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle();

            // Create buffer NetworkMessage
            NetMessageServerClientConnectingFromClient t_networkMessage = NetMessageServerClientConnectingFromClient();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.ReceiveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), t_connectingSocketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  ++t_NumOfMessagesReceived < m_maxConnectingMessagesPerFrame)
            {
                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageServerClientConnectingFromClient))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageServerClientConnectingFromClient();
                    continue;
                }

                std::cout << "Received unreliable messsage: "; // TODOCM logg instead
                NetMessageServerClientConnectingFromClient& t_netMessageConnecting =
                    *reinterpret_cast<NetMessageServerClientConnectingFromClient*>(&t_networkMessage);
                // Switch on what kind of message
                switch(t_netMessageConnecting.MessageID)
                {
                    case SendMessageIDToServerFromClient::CONNECTION_REQUEST:
                    {
                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveConnectionRequest(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToServerFromClient::VERSION_CHECK:
                    {
                        std::cout << "Version Check" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveVersionCheck(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToServerFromClient::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveDisconnect(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageServerClientConnectingFromClient();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerServer::ReceiveConnectedMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // For each connection
            auto& t_connectedClientConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
            for(auto& t_connection : t_connectedClientConnections)
            {
                // Create message
                NetMessageBuffer t_message = NetMessageBuffer();

                // If we're connected
                if(t_connection.second->ConnectionState >= ClientConnectionStateFromServer::CONNECTED)
                {
                    // Max message counter
                    uint32_t t_messageCounter = 0;

                    // To check how much we received
                    uint32_t t_dataSizeReceived = 0;

                    // While we have something to receive and still less then max messages per frame
                    while(t_networkModule.ReceiveReliableData(&t_message, sizeof(t_message), t_connection.second->ConnectedSocketHandle, t_dataSizeReceived) &&
                          ++t_messageCounter < m_maxConnectedMessagesPerFrame)
                    {
                        // If we received a correct message
                        if(t_dataSizeReceived != sizeof(NetMessageServerClientConnectedFromClient))
                        {
                            t_message = NetMessageBuffer();
                            continue;
                        }

                        // Convert to correct message
                        NetMessageServerClientConnectedFromClient& t_connectedMessage = *reinterpret_cast<NetMessageServerClientConnectedFromClient*>(&t_message);

                        // Interpet message based on type
                        switch(t_connectedMessage.MessageID)
                        {
                            case SendMessageIDToServerFromClient::CONNECTED:
                            {
                                t_netMessages->ReceiveConnected(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDToServerFromClient::LOAD_WORLD:
                            {
                                t_netMessages->ReceiveLoadWorld(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDToServerFromClient::IN_GAME:
                            {
                                t_netMessages->ReceiveInGame(t_connectedMessage, t_connection.second);

                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }

                        // Reset ping timer
                        t_connection.second->LastResponse = 0;

                        // Reset message
                        t_message = NetMessageBuffer();
                    }
                }
            }
        }

        void NetworkManagerServer::ReceiveMasterMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();

            // Get message class
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Get connecting socket
            SocketHandle t_masterConnectingSocketHandle = t_connections->m_masterConnection.ConnectedSocketHandle;

            // Create buffer message
            NetMessageMasterServerFromMaster t_newMessage = NetMessageMasterServerFromMaster();

            // To check how much we received
            uint32_t t_dataSizeReceived = 0;

            // Counter for checking we dont read to much
            uint32_t t_numOfMessages = 0;

            // Receive messages
            // TODOCM not sure if need to send in out adress here
            while(t_networkModule.ReceiveUnreliableData(&t_newMessage, sizeof(t_newMessage), t_masterConnectingSocketHandle,
                                                        t_connections->m_masterConnection.Adress, t_dataSizeReceived) &&
                  ++t_numOfMessages < m_maxConnectingMessagesPerFrame)
            {
                // If wrong size of message
                if(t_dataSizeReceived != sizeof(NetMessageMasterServerFromMaster))
                {
                    t_newMessage = NetMessageMasterServerFromMaster();
                    continue;
                }

                // Check ID and interpet
                switch(t_newMessage.MessageID)
                {
                    case SendMessageIDToServerFromMaster::CONNECTED:
                    {
                        t_netMessages->ReceiveConnectedMaster(t_newMessage);

                        break;
                    }
                    case SendMessageIDToServerFromMaster::DISCONNECT:
                    {
                        t_netMessages->ReceiveDisconnectMaster(t_newMessage);

                        break;
                    }
                    default:
                    {
                        std::cout << "Some error message received" << std::endl; // TODOCM remove deubgg
                        break;
                    }
                }

                // Reset message
                t_newMessage = NetMessageMasterServerFromMaster();
            }
        }

        void NetworkManagerServer::SendConnectedMessages()
        {
            NetworkMessagesServer::GetInstance()->UpdateSequence();
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Get connected clients
            auto& t_connections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();

            // std::cout << t_connections.size() << std::endl;

            // For each connected client
            for(auto& t_connection : t_connections)
            {
                // Send message based on state
                switch(t_connection.second->ConnectionState)
                {
                    case ClientConnectionStateFromServer::CONNECTED:
                    {
                        t_netMessages->SendConnected(t_connection.second);

                        break;
                    }
                    case ClientConnectionStateFromServer::LOAD_WORLD:
                    {
                        t_netMessages->SendLoadWorld(t_connection.second);

                        break;
                    }
                    case ClientConnectionStateFromServer::IN_GAME:
                    {
                        t_netMessages->SendInGame(t_connection.second);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        void NetworkManagerServer::SendMasterMessages(double p_dt)
        {
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Update timer
            m_masterNextUpdateTimer += p_dt;

            // If we're not connected we only send at intervals
            if(m_masterNextUpdateTimer >= m_masterUpdateInterval)
            {
                // Reduce timer
                m_masterNextUpdateTimer -= m_masterUpdateInterval;

                // Based on our state we send different message
                switch(t_connections->m_masterConnection.ConnectionState)
                {
                    case MasterConnectionStateFromServer::CONNECTING:
                    {
                        t_netMessages->SendConnectionRequestMaster();

                        break;
                    }
                    case MasterConnectionStateFromServer::CONNECTED:
                    {
                        t_netMessages->SendConnectedMaster();

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        void NetworkManagerServer::CheckForConnections()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsServer* t_netConnections = NetworkConnectionsServer::GetInstance();
            PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());
            ServerStateHandler* t_serverStateHandler = ServerStateHandler::GetInstance();

            // Get values
            SocketHandle t_connectedSocketHandle = NetworkConnectionsServer::GetInstance()->GetConnectedSocketHandle();
            SocketHandle t_outSocketHandle = 0;
            DoremiEngine::Network::Adress* t_outAdress = t_networkModule.CreateAdress();

            // Holder for the connection we want to fetch
            std::pair<DoremiEngine::Network::Adress*, ClientConnectionFromServer*> t_connection;

            // Counter to not get stuck
            uint32_t t_connectCounter = 0;

            // Check if we can actually accept any connections with lagg condition
            if(TimeHandler::GetInstance()->IsLagging())
            {
                return;
            }

            // Try to accept connections
            while(t_networkModule.AcceptConnection(t_connectedSocketHandle, t_outSocketHandle, t_outAdress) && ++t_connectCounter < m_maxAcceptConnectionsPerFrame)
            {
                // If we have a pending connection
                if(t_netConnections->AdressExistInConnecting(*t_outAdress, t_connection))
                {
                    // If the connection is in connect mode
                    if(t_connection.second->ConnectionState == ClientConnectionStateFromServer::CONNECT)
                    {
                        uint8_t currentPlayers = t_netConnections->GetConnectedClientConnections().size();
                        uint8_t maxPlayers = t_serverStateHandler->GetMaxPlayers();

                        // If server isn't full
                        if(currentPlayers < maxPlayers)
                        {
                            std::cout << "Accepting connection" << std::endl;

                            // Update connection
                            t_connection.second->ConnectionState = ClientConnectionStateFromServer::CONNECTED;

                            // Update last response
                            t_connection.second->LastResponse = 0;

                            // Add socketID
                            t_connection.second->ConnectedSocketHandle = t_outSocketHandle;

                            // New connection
                            t_connection.second->LastSequenceUpdate = SEQUENCE_TIMER_START; // High because we want update

                            // Create new InputHandler
                            InputHandlerServer* t_newInputHandler = new InputHandlerServer(m_sharedContext, DirectX::XMFLOAT3(0, 0, 0));

                            // Create player
                            t_playerHandler->CreateNewPlayer(t_connection.second->MyPlayerID, t_newInputHandler);

                            // Upgrade connection to connected
                            t_netConnections->SetConnecting(t_connection);
                        }
                        else
                        {
                            // If we're not in the connect stage, its bad pattern
                            // TODOXX This will trigger if there are two on same IP joining at same time, will crash hard
                            NetworkMessagesServer::GetInstance()->SendDisconnect(*t_outAdress, "Server is full");
                        }
                    }
                    else
                    {
                        // If we're not in the connect stage, its bad pattern
                        // TODOXX This will trigger if there are two on same IP joining at same time, will crash hard
                        NetworkMessagesServer::GetInstance()->SendDisconnect(*t_outAdress, "Bad pattern: Error in connect");
                    }
                }
                else
                {
                    // If we're not in the connect stage, its bad pattern
                    NetworkMessagesServer::GetInstance()->SendDisconnect(*t_outAdress, "Bad pattern: Multiple connect attempts");
                }
            }

            // Delete the adress
            delete t_outAdress;
        }

        void NetworkManagerServer::UpdateTimeouts(double p_dt)
        {
            // Check all connected connections
            UpdateTimeoutsConnecting(p_dt);

            // Check all connecting connections
            UpdateTimeoutsConnected(p_dt);

            // Check master timeout
            UpdateTimeoutsMaster(p_dt);
        }

        void NetworkManagerServer::UpdateTimeoutsConnecting(double p_dt)
        {
            auto& t_connectedConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
            auto t_connection = t_connectedConnections.begin();

            while(t_connection != t_connectedConnections.end())
            {
                // Update timer
                t_connection->second->LastResponse += p_dt;
                t_connection->second->LastSequenceUpdate += p_dt;

                // If exceed timout
                if(t_connection->second->LastResponse >= m_timeoutIntervalConnected)
                {
                    std::cout << "Timeout client: " << t_connection->second->LastResponse << " seconds." << std::endl;

                    // Send disconnection message
                    NetworkMessagesServer::GetInstance()->SendDisconnect(*t_connection->first, "Timeout");

                    // Remove socket
                    m_sharedContext.GetNetworkModule().DeleteSocket(t_connection->second->ConnectedSocketHandle);

                    // Remove and save player if it exists, it should
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->RemoveAndSavePlayer(t_connection->second->MyPlayerID);

                    // Delete the memory here
                    delete t_connection->first;
                    delete t_connection->second;

                    // Erase from map
                    t_connection = t_connectedConnections.erase(t_connection);
                }
                else
                {
                    // Else just increment
                    t_connection++;
                }
            }
        }

        void NetworkManagerServer::UpdateTimeoutsConnected(double p_dt)
        {
            auto& t_connectingConnections = NetworkConnectionsServer::GetInstance()->GetConnectingClientConnections();
            auto t_connection = t_connectingConnections.begin();

            while(t_connection != t_connectingConnections.end())
            {
                // Update timer
                t_connection->second->LastResponse += p_dt;

                // If exceed timout
                if(t_connection->second->LastResponse >= m_timeoutIntervalConnecting)
                {
                    std::cout << "Timeout client: " << t_connection->second->LastResponse << " seconds." << std::endl;
                    // Send disconnection message
                    NetworkMessagesServer::GetInstance()->SendDisconnect(*t_connection->first, "Timeout");

                    // Remove socket
                    m_sharedContext.GetNetworkModule().DeleteSocket(t_connection->second->ConnectedSocketHandle);

                    // Delete the memory here
                    delete t_connection->first;
                    delete t_connection->second;

                    // Erase from map
                    t_connection = t_connectingConnections.erase(t_connection);
                }
                else
                {
                    // Else just increment
                    t_connection++;
                }
            }
        }

        void NetworkManagerServer::UpdateTimeoutsMaster(double p_dt)
        {
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();

            // Update timer
            t_connections->m_masterConnection.LastResponse += p_dt;

            // If exceed timout
            if(t_connections->m_masterConnection.LastResponse >= m_timeoutIntervalMaster &&
               t_connections->m_masterConnection.ConnectionState > MasterConnectionStateFromServer::DISCONNECTED)
            {
                t_connections->m_masterConnection.ConnectionState = MasterConnectionStateFromServer::CONNECTING;
                t_connections->m_masterConnection.LastResponse = 0;

                // Send disconnection message
                NetworkMessagesServer::GetInstance()->SendDisconnectMaster();
            }
        }
    }
}