#include <Utilities/Memory/CircleBuffer/ArbitrarySizeCirclebufferTest.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <memory>

TEST_F(ArbitrarySizeCirclebufferTest, basicInit)
{
    m_circleBuffer->Initialize(104);
    SUCCEED();
}

/*
TEST_F(ArbitrarySizeCirclebufferTest, emptyConsume)
{
    m_circleBuffer->Initialize(104);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();
    const bool res = m_circleBuffer->Consume(returnHeader, returnData);

    ASSERT_FALSE(res);
    delete returnHeader;
    delete returnData;
}

TEST_F(ArbitrarySizeCirclebufferTest, toManyProduce)
{
    m_circleBuffer->Initialize(104);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    TestStruct64 sendData = TestStruct64();
    bool res = m_circleBuffer->Produce(sendHeader, &sendData);
    ASSERT_TRUE(res);
    res = m_circleBuffer->Produce(sendHeader, &sendData);
    ASSERT_FALSE(res);
}

TEST_F(ArbitrarySizeCirclebufferTest, basicProduceAndConsume)
{
    m_circleBuffer->Initialize(300);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    sendData.f1 = 4;
    m_circleBuffer->Produce(sendHeader, &sendData);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();
    m_circleBuffer->Consume(returnHeader, returnData);

    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::TEXT).typeValue, CircleBufferType(returnHeader->packageType).typeValue);
    delete returnHeader;
    delete returnData;
}

TEST_F(ArbitrarySizeCirclebufferTest, twoProduceAndConsume)
{
    m_circleBuffer->Initialize(300);

    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);

    TestStruct64 sendData = TestStruct64();
    sendData.f1 = 4;

    // Produce
    m_circleBuffer->Produce(sendHeader, &sendData);
    sendData.f2 = 19;
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::COMMAND);
    m_circleBuffer->Produce(sendHeader, &sendData);

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::TEXT).typeValue, CircleBufferType(returnHeader->packageType).typeValue);

    m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_EQ(4, returnData->f1);
    ASSERT_EQ(19, returnData->f2);
    ASSERT_EQ(64, returnHeader->packageSize);
    ASSERT_EQ(CircleBufferType(CircleBufferTypeEnum::COMMAND).typeValue, CircleBufferType(returnHeader->packageType).typeValue);
    delete returnHeader;
    delete returnData;
}


TEST_F(ArbitrarySizeCirclebufferTest, twoBuffersUsingDifferentPartOfSameExternalMemory)
{
    using namespace Doremi::Utilities;
    void* memory = malloc(400);
    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(PointerArithmetic::Addition(memory, 200), 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    //// Produce
    // Produce first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, &sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 19;
    res = m_circleBuffer->Produce(sendHeader, &sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 78;
    res = m_circleBuffer->Produce(sendHeader, &sendData);
    ASSERT_FALSE(res);

    // Produce second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, &sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 775;
    res = otherBuffer->Produce(sendHeader, &sendData);
    ASSERT_TRUE(res);

    sendData.f1 = 95;
    res = otherBuffer->Produce(sendHeader, &sendData);
    ASSERT_FALSE(res);


    //// Consume
    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    // Consume first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(4, returnData->f1);

    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(19, returnData->f1);

    res = m_circleBuffer->Consume(returnHeader, returnData);
    ASSERT_FALSE(res);

    // Consume second buffer
    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_TRUE(res);
    ASSERT_EQ(775, returnData->f1);

    res = otherBuffer->Consume(returnHeader, returnData);
    ASSERT_FALSE(res);

    free(memory);
    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}


TEST_F(ArbitrarySizeCirclebufferTest, twoBuffersPointingOnSameMemory)
{
    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    using namespace Doremi::Utilities;
    void* memory = malloc(200);
    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(memory, 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    // Produce with first buffer
    sendData.f1 = 98;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 1
    ASSERT_TRUE(res);

    // Produce with second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_TRUE(res);

    // Produce with first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_FALSE(res);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(98, returnData->f1);

    // Produce with first buffer
    sendData.f1 = 33;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_TRUE(res);

    // Consume with second  buffer
    res = otherBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_TRUE(res);
    ASSERT_EQ(33, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_FALSE(res);

    free(memory);
    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}

TEST_F(ArbitrarySizeCirclebufferTest, twoBuffersUsingOneFileMapToMemory)
{
    using namespace Doremi::Utilities::IO;
    FileMap m_fileMap;
    void* memory = m_fileMap.Initialize("test", 200);
    ASSERT_NE(nullptr, memory); // Assert not null

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(memory, 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    // Produce with first buffer
    sendData.f1 = 98;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 1
    ASSERT_TRUE(res);

    // Produce with second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_TRUE(res);

    // Produce with first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_FALSE(res);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(98, returnData->f1);

    // Produce with first buffer
    sendData.f1 = 33;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_TRUE(res);

    // Consume with second  buffer
    res = otherBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_TRUE(res);
    ASSERT_EQ(33, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_FALSE(res);

    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}

TEST_F(ArbitrarySizeCirclebufferTest, twoBuffersUsingTwoFileMapsTMemory)
{
    using namespace Doremi::Utilities::IO;

    // Allocate memory via filemap
    FileMap m_fileMap;
    void* memory = m_fileMap.Initialize("test", 200);
    ASSERT_NE(nullptr, memory); // Assert not null

    // Allocate another filemap
    FileMap m_fileMap2;
    void* memory2 = m_fileMap.Initialize("test", 200);
    ASSERT_NE(nullptr, memory2); // Assert not null

    CircleBufferHeader* returnHeader = new CircleBufferHeader();
    TestStruct64* returnData = new TestStruct64();

    m_circleBuffer->Initialize(memory, 200);

    CircleBuffer<TestStruct64>* otherBuffer = new CircleBuffer<TestStruct64>();
    otherBuffer->Initialize(memory2, 200);

    // Build package
    CircleBufferHeader sendHeader = CircleBufferHeader();
    sendHeader.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
    sendHeader.packageSize = sizeof(TestStruct64);
    TestStruct64 sendData = TestStruct64();
    bool res;

    // Produce with first buffer
    sendData.f1 = 98;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // should succeed
    // Memory now contains 1 object
    ASSERT_TRUE(res);

    // Produce with second buffer
    sendData.f1 = 62;
    res = otherBuffer->Produce(sendHeader, &sendData); // should succeed
    // Memory now contains 2 object
    ASSERT_TRUE(res);

    // Produce with first buffer
    sendData.f1 = 4;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // should fail
    // Memory now contains 2 object
    ASSERT_FALSE(res);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(98, returnData->f1);

    // Produce with first buffer
    sendData.f1 = 33;
    res = m_circleBuffer->Produce(sendHeader, &sendData); // 2
    ASSERT_TRUE(res);

    // Consume with second  buffer
    res = otherBuffer->Consume(returnHeader, returnData); // 1
    ASSERT_TRUE(res);
    ASSERT_EQ(62, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_TRUE(res);
    ASSERT_EQ(33, returnData->f1);

    // Consume with first buffer
    res = m_circleBuffer->Consume(returnHeader, returnData); // 0
    ASSERT_FALSE(res);

    delete otherBuffer;
    delete returnHeader;
    delete returnData;
}
*/