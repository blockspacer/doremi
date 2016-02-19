#include "CommonInclude.hlsl"



// View space frustums for the grid cells.
RWStructuredBuffer<Frustum> out_Frustums : register(u0);

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    //TODORK send as parameter
    uint3 numThreadGroups = uint3(80, 45, 1);
    uint3 numThreads = uint3(SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    // View space eye position is always at the origin.
    const float3 eyePos = float3(0, 0, 0);

    float4 screenSpace[4];
    // Top left point
    //DEBUG groupID = dispatchThreadID
    screenSpace[0] = float4(input.groupID.xy * BLOCK_SIZE, -1.0f, 1.0f);
    // Top right point
    screenSpace[1] = float4(float2(input.groupID.x + 1, input.groupID.y) * BLOCK_SIZE, -1.0f, 1.0f);
    // Bottom left point
    screenSpace[2] = float4(float2(input.groupID.x, input.groupID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);
    // Bottom right point
    screenSpace[3] = float4(float2(input.groupID.x + 1, input.groupID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);

    float3 viewSpace[4];
    Frustum frustum;

    for (int i = 0; i < 4; i++)
    {
        viewSpace[i] = ScreenToView(screenSpace[i]).xyz;
    }


    // Left plane
    frustum.plane[0] = ComputePlane(eyePos, viewSpace[2], viewSpace[0]);
    // Right plane
    frustum.plane[1] = ComputePlane(eyePos, viewSpace[1], viewSpace[3]);
    // Top plane
    frustum.plane[2] = ComputePlane(eyePos, viewSpace[0], viewSpace[1]);
    // Bottom plane
    frustum.plane[3] = ComputePlane(eyePos, viewSpace[3], viewSpace[2]);

    // Store the computed frustum in global memory.

    if (input.groupThreadID.x == 0 && input.groupThreadID.y == 0)
    {
        uint index = input.groupID.x + (input.groupID.y * numThreadGroups.x);
        out_Frustums[index] = frustum;
    }


}