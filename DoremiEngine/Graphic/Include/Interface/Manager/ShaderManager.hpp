#pragma once
#include <string>
struct D3D11_INPUT_ELEMENT_DESC;
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader;
        class VertexShader;
        class GeometryShader;
        class ComputeShader;
        class ShaderManager
        {
        public:
            // TODOKO Add docs
            virtual VertexShader* BuildVertexShader(const std::string& p_fileName, D3D11_INPUT_ELEMENT_DESC p_inputDescription[], int p_arraySize) = 0;
            virtual VertexShader * BuildVertexShaderWithoutInput(const std::string & p_fileName) = 0;
            virtual GeometryShader* BuildGeometryShader(const std::string& p_fileName) = 0;
            virtual PixelShader* BuildPixelShader(const std::string& p_fileName) = 0;
            virtual ComputeShader* BuildComputeShader(const std::string& p_filename) = 0;
            virtual void SetActiveVertexShader(VertexShader* p_shader) = 0;
            virtual void SetActivePixelShader(PixelShader* p_shader) = 0;
            virtual void SetActiveGeometryShader(GeometryShader* p_shader) = 0;
            virtual void SetActiveComputeShader(ComputeShader* p_shader) = 0;
            virtual void RemoveGeometryShader() = 0;
        };
    }
}