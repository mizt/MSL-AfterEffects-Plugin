#import <metal_stdlib>
using namespace metal;

constexpr sampler _sampler(filter::linear,coord::normalized);

kernel void processimage(
    texture2d<float,access::sample> src[[texture(0)]],
    texture2d<float,access::write> dst[[texture(1)]],
    constant float2 &resolution[[buffer(0)]],
    uint2 gid[[thread_position_in_grid]]) {
    
    float2 uv = float2(gid)/resolution;
    float3 rgb = src.sample(_sampler,uv).yzw;
    dst.write(float4(1.0,rgb),gid);
}
