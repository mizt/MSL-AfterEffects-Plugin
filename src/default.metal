#import <metal_stdlib>
using namespace metal;

constexpr sampler linear(filter::linear,coord::pixel);

kernel void processimage(
    texture2d<float,access::sample> src[[texture(0)]],
    texture2d<float,access::write> dst[[texture(1)]],
    constant float2 &resolution[[buffer(0)]],
    uint2 gid[[thread_position_in_grid]]) {
    
    float2 uv = (float2(gid)+float2(0.5,0.5))/resolution;
    float3 rgb = src.sample(linear,uv*resolution).yzw;
    dst.write(float4(1.0,rgb),gid);
}
