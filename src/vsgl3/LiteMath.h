#pragma once

#include <math.h>
#include <stdlib.h>


struct float2
{
  float2():x(0), y(0){}
  float2(float a,float b): x(a), y(b) {}

  float x,y;
};

struct float3
{
  float3():x(0), y(0), z(0){}
  float3(float a,float b, float c): x(a), y(b), z(c) {}
  float3(const float* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[0]) {}

  float x,y,z;
};

struct float4
{
  float4():x(0), y(0), z(0){}
  float4(float a,float b, float c, float d): x(a), y(b), z(c), w(d) {}

  float x,y,z,w;
};

struct float4x4
{
  float4x4(){identity();}

  float4x4(const float arr[16])
  {
    row[0] = float4(arr[0],arr[1],arr[2],arr[3]);
    row[1] = float4(arr[4],arr[5],arr[6],arr[7]);
    row[2] = float4(arr[8],arr[9],arr[10],arr[11]);
    row[3] = float4(arr[12],arr[13],arr[14],arr[15]);
  }

  void identity()
  {
    row[0] = float4(1,0,0,0);
    row[1] = float4(0,1,0,0);
    row[2] = float4(0,0,1,0);
    row[3] = float4(0,0,0,1);
  }

  float& M(int x, int y) {return ((float*)row)[y*4+x]; }
  float  M(int x, int y) const {return ((float*)row)[y*4+x]; }

  float* L() {return (float*)row;}
  const float* L() const {return (float*)row;}

  float4 row[4];
};

inline float rnd(float s, float e)
{
  float t = (float)(rand())/(32767);
  return s + t*(e-s);
}

template <class T> inline void swap(T& a, T& b)
{
  T temp = a;
  a = b;
  b = temp;
}

// fmax and fmin commented
//static inline float fmax(float a, float b) { return a > b ? a : b; }
//static inline float fmin(float a, float b) { return a < b ? a : b; }

static inline float fmaxf(float a, float b) { return a > b ? a : b; }
static inline float fminf(float a, float b) { return a < b ? a : b; }

static inline float clamp(float u, float a, float b) { float r = fmaxf(a, u); return fminf(r, b); }
static inline int   clamp(int u, int a, int b) { int r = (a > u) ? a : u; return (r < b) ? r : b;}

#define SQR(x) (x)*(x)

static inline float4 make_float4(float a, float b, float c, float d) {return float4(a,b,c,d);}
static inline float3 make_float3(float a, float b, float c) {return float3(a,b,c);}
static inline float2 make_float2(float a, float b) {return float2(a,b);}

//**********************************************************************************
// float4 operators and functions
//**********************************************************************************

static inline float4 operator + (const float4 & u, float v) { return make_float4( u.x + v, u.y + v, u.z + v, u.w + v ); }
static inline float4 operator - (const float4 & u, float v) { return make_float4( u.x - v, u.y - v, u.z - v, u.w - v ); }
static inline float4 operator * (const float4 & u, float v) { return make_float4( u.x * v, u.y * v, u.z * v, u.w * v ); }
static inline float4 operator / (const float4 & u, float v) { return make_float4( u.x / v, u.y / v, u.z / v, u.w / v ); }

static inline float4 operator + (float v, const float4 & u) { return make_float4(  v + u.x, v + u.y, v + u.z, v + u.w ); }
static inline float4 operator - (float v, const float4 & u) { return make_float4(  v - u.x, v - u.y, v - u.z, v - u.w ); }
static inline float4 operator * (float v, const float4 & u) { return make_float4(  v * u.x, v * u.y, v * u.z, v * u.w ); }
static inline float4 operator / (float v, const float4 & u) { return make_float4(  v / u.x, v / u.y, v / u.z, v / u.w ); }

static inline float4 operator + (const float4 & u, const float4 & v) { return make_float4( u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w ); }
static inline float4 operator - (const float4 & u, const float4 & v) { return make_float4( u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w ); }
static inline float4 operator * (const float4 & u, const float4 & v) { return make_float4( u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w ); }
static inline float4 operator / (const float4 & u, const float4 & v) { return make_float4( u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w ); }

static inline float4 & operator += (float4 & u, const float4 & v) { u.x += v.x; u.y += v.y; u.z += v.z; u.w += v.w; return u; }
static inline float4 & operator -= (float4 & u, const float4 & v) { u.x -= v.x; u.y -= v.y; u.z -= v.z; u.w -= v.w; return u; }
static inline float4 & operator *= (float4 & u, const float4 & v) { u.x *= v.x; u.y *= v.y; u.z *= v.z; u.w *= v.w; return u; }
static inline float4 & operator /= (float4 & u, const float4 & v) { u.x /= v.x; u.y /= v.y; u.z /= v.z; u.w /= v.w; return u; }

static inline float4 & operator += (float4 & u, float v) { u.x += v; u.y += v; u.z += v; u.w += v; return u; }
static inline float4 & operator -= (float4 & u, float v) { u.x -= v; u.y -= v; u.z -= v; u.w -= v; return u; }
static inline float4 & operator *= (float4 & u, float v) { u.x *= v; u.y *= v; u.z *= v; u.w *= v; return u; }
static inline float4 & operator /= (float4 & u, float v) { u.x /= v; u.y /= v; u.z /= v; u.w /= v; return u; }

static inline float4   operator - (const float4 & v) { return make_float4( -v.x, -v.y, -v.z, -v.w ); }

static inline float4 catmullrom (const float4 & P0, const float4 & P1, const float4 & P2, const float4 & P3, float t)
{
  const float ts = t * t;
  const float tc = t * ts;

  return (P0 * (-tc + 2.0f * ts - t) + P1 * (3.0f * tc - 5.0f * ts + 2.0f) + P2 * (-3.0f * tc + 4.0f * ts + t) + P3 * ( tc - ts)) * 0.5f;
}

static inline float4 lerp (const float4 & u, const float4 & v, float t ) { return u + t * (v - u); }
static inline float  dot  (const float4 & u, const float4 & v) { return (u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w); }
static inline float  dot3 (const float4 & u, const float4 & v) { return (u.x*v.x + u.y*v.y + u.z*v.z); }
static inline float  dot3 (const float4 & u, const float3 & v) { return (u.x*v.x + u.y*v.y + u.z*v.z); }

static inline float4 clamp(const float4 & u, float a, float b) { return make_float4( clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b), clamp(u.w, a, b) ); }
static inline float4 abs  (const float4 & u) { return make_float4(abs(u.x), abs(u.y), abs(u.z), abs(u.w)); }

static inline float  length3(const float4 & u) { return sqrt(SQR(u.x) + SQR(u.y) + SQR(u.z)); }
static inline float  length (const float4 & u) { return sqrt(SQR(u.x) + SQR(u.y) + SQR(u.z) + SQR(u.w)); }

//inline float4 sqrt   (const float4 & u) { make_float4( sqrt(u.x), sqrt(u.y), sqrt(u.z), sqrt(u.w) ); }

//**********************************************************************************
// float3 operators and functions
//**********************************************************************************

static inline float3 operator + (const float3 & u, float v) { return make_float3( u.x + v, u.y + v, u.z + v ); }
static inline float3 operator - (const float3 & u, float v) { return make_float3( u.x - v, u.y - v, u.z - v ); }
static inline float3 operator * (const float3 & u, float v) { return make_float3( u.x * v, u.y * v, u.z * v ); }
static inline float3 operator / (const float3 & u, float v) { return make_float3( u.x / v, u.y / v, u.z / v ); }

static inline float3 operator + (float v, const float3 & u) { return make_float3( v + u.x, v + u.y, v + u.z ); }
static inline float3 operator - (float v, const float3 & u) { return make_float3( v - u.x, v - u.y, v - u.z ); }
static inline float3 operator * (float v, const float3 & u) { return make_float3( v * u.x, v * u.y, v * u.z ); }
static inline float3 operator / (float v, const float3 & u) { return make_float3( v / u.x, v / u.y, v / u.z ); }

static inline float3 operator + (const float3 & u, const float3 & v) { return make_float3( u.x + v.x, u.y + v.y, u.z + v.z ); }
static inline float3 operator - (const float3 & u, const float3 & v) { return make_float3( u.x - v.x, u.y - v.y, u.z - v.z ); }
static inline float3 operator * (const float3 & u, const float3 & v) { return make_float3( u.x * v.x, u.y * v.y, u.z * v.z ); }
static inline float3 operator / (const float3 & u, const float3 & v) { return make_float3( u.x / v.x, u.y / v.y, u.z / v.z ); }

static inline float3 operator - (const float3 & u) { return make_float3( -u.x, -u.y, -u.z ); }

static inline float3 & operator += (float3 & u, const float3 & v) { u.x += v.x; u.y += v.y; u.z += v.z; return u; }
static inline float3 & operator -= (float3 & u, const float3 & v) { u.x -= v.x; u.y -= v.y; u.z -= v.z; return u; }
static inline float3 & operator *= (float3 & u, const float3 & v) { u.x *= v.x; u.y *= v.y; u.z *= v.z; return u; }
static inline float3 & operator /= (float3 & u, const float3 & v) { u.x /= v.x; u.y /= v.y; u.z /= v.z; return u; }

static inline float3 & operator += (float3 & u, float v) { u.x += v; u.y += v; u.z += v; return u; }
static inline float3 & operator -= (float3 & u, float v) { u.x -= v; u.y -= v; u.z -= v; return u; }
static inline float3 & operator *= (float3 & u, float v) { u.x *= v; u.y *= v; u.z *= v; return u; }
static inline float3 & operator /= (float3 & u, float v) { u.x /= v; u.y /= v; u.z /= v; return u; }


static inline float3 catmullrom (const float3 & P0, const float3 & P1, const float3 & P2, const float3 & P3, float t)
{
  const float ts = t * t;
  const float tc = t * ts;

  return (P0 * (-tc + 2.0f * ts - t) + P1 * (3.0f * tc - 5.0f * ts + 2.0f) + P2 * (-3.0f * tc + 4.0f * ts + t) + P3 * ( tc - ts)) * 0.5f;
}

static inline float3 lerp      (const float3 & u, const float3 & v, float t ) { return u + t * (v - u); }
static inline float  dot       (const float3 & u, const float3 & v) { return (u.x*v.x + u.y*v.y + u.z*v.z); }
static inline float3 cross     (const float3 & u, const float3 & v) { return make_float3( u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x); }
//inline float3 mul       (const float3 & u, const float3 & v) { make_float3( u.x*v.x, u.y*v.y, u.z*v.z} ; return r; }
static inline float3 clamp     (const float3 & u, float a, float b) { return make_float3( clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b) ); }

static inline float  triple    (const float3 & a, const float3 & b, const float3 & c ) { return dot(a, cross(b, c) ); }
static inline float  length    (const float3 & u) { return sqrt(SQR(u.x) + SQR(u.y) + SQR(u.z)); }
static inline float  lengthSquare (const float3 u) { return u.x*u.x + u.y*u.y + u.z*u.z; }
static inline float3 normalize (const float3 & u) { return u / length(u); }
static inline float  coordSumm (const float3 u) { return u.x* + u.y + u.z; }
//static inline float  coordAbsMax (const float3 u) { return max(max(abs(u.x), abs(u.y)), abs(u.z)); }

static inline float  maxcomp(const float3 & u) { return fmax(u.x, fmax(u.y, u.z)); }
static inline float  mincomp(const float3 & u) { return fmin(u.x, fmin(u.y, u.z)); }


//**********************************************************************************
// float2 operators and functions
//**********************************************************************************

static inline float2 operator + (const float2 & u, float v) { return make_float2( u.x + v, u.y + v ); }
static inline float2 operator - (const float2 & u, float v) { return make_float2( u.x - v, u.y - v ); }
static inline float2 operator * (const float2 & u, float v) { return make_float2( u.x * v, u.y * v ); }
static inline float2 operator / (const float2 & u, float v) { return make_float2( u.x / v, u.y / v ); }

static inline float2 operator + (float v, const float2 & u) { return make_float2( v + u.x, v + u.y ); }
static inline float2 operator - (float v, const float2 & u) { return make_float2( v - u.x, v - u.y ); }
static inline float2 operator * (float v, const float2 & u) { return make_float2( v * u.x, v * u.y ); }
static inline float2 operator / (float v, const float2 & u) { return make_float2( v / u.x, v / u.y ); }

static inline float2 operator + (const float2 & u, const float2 & v) { return make_float2( u.x + v.x, u.y + v.y ); }
static inline float2 operator - (const float2 & u, const float2 & v) { return make_float2( u.x - v.x, u.y - v.y ); }
static inline float2 operator * (const float2 & u, const float2 & v) { return make_float2( u.x * v.x, u.y * v.y ); }
static inline float2 operator / (const float2 & u, const float2 & v) { return make_float2( u.x / v.x, u.y / v.y ); }

static inline float2   operator - (const float2 & v) { return make_float2( -v.x, -v.y ); }

static inline float2 & operator += (float2 & u, const float2 & v) { u.x += v.x; u.y += v.y; return u; }
static inline float2 & operator -= (float2 & u, const float2 & v) { u.x -= v.x; u.y -= v.y; return u; }
static inline float2 & operator *= (float2 & u, const float2 & v) { u.x *= v.x; u.y *= v.y; return u; }
static inline float2 & operator /= (float2 & u, const float2 & v) { u.x /= v.x; u.y /= v.y; return u; }

static inline float2 & operator += (float2 & u, float v) { u.x += v; u.y += v; return u; }
static inline float2 & operator -= (float2 & u, float v) { u.x -= v; u.y -= v; return u; }
static inline float2 & operator *= (float2 & u, float v) { u.x *= v; u.y *= v; return u; }
static inline float2 & operator /= (float2 & u, float v) { u.x /= v; u.y /= v; return u; }

static inline float2 catmullrom (const float2 & P0, const float2 & P1, const float2 & P2, const float2 & P3, float t)
{
  const float ts = t * t;
  const float tc = t * ts;

  return (P0 * (-tc + 2.0f * ts - t) + P1 * (3.0f * tc - 5.0f * ts + 2.0f) + P2 * (-3.0f * tc + 4.0f * ts + t) + P3 * ( tc - ts)) * 0.5f;
}

static inline float2 lerp      (const float2 & u, const float2 & v, float t ) { return u + t * (v - u); }
static inline float  dot       (const float2 & u, const float2 & v) { return (u.x*v.x + u.y*v.y); }
static inline float2 clamp     (const float2 & u, float a, float b) { return make_float2( clamp(u.x, a, b), clamp(u.y, a, b) ); }

//inline float2 saturate  (const float2 & u) { return clamp(u, 0.0f, 1.0f); }

//static inline float  sad (const float2 & u, const float2 & v) { return (abs(u.x - v.x) + abs(u.y - v.y)); } // Sum of Absolute Differences for xyzw components
//static inline float  ssd (const float2 & u, const float2 & v) { return (SQR(u.x - v.x) + SQR(u.y - v.y)); } // Sum of Squared Differences for xyzw components

//static inline uchar2 to_uchar2(const float2 & u) { uchar2 r = {u.x*255.0f, u.y*255.0f); }

static inline float  length (const float2 & u)    { return sqrt(SQR(u.x) + SQR(u.y)); }
static inline float2 normalize (const float2 & u) { return u / length(u); }
static inline float4 normalize(const float4& u)   { return u / sqrtf(dot3(u,u)); }
static inline float4 cross(const float4 & u, const float4 & v) { return make_float4(u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x, 0.0f); }

static inline float lerp(float u, float v, float t ) { return u + t * (v - u); }
static inline float3 reflect(const float3& dir, const float3& normal) { return normalize((normal * dot(dir,normal) * (-2)) + dir); }



static inline float3 to_float3(float4 a_data) { return float3(a_data.x, a_data.y, a_data.z); }
static inline float4 to_float4(float3 a_data, float a_w) { return float4(a_data.x, a_data.y, a_data.z, a_w); }

inline float luminance(float3 rgb) { return dot(rgb, float3(0.299f, 0.587f, 0.114f)); } 

static inline float2 fmaxf(float2 a, float2 b) { return float2(fmaxf(a.x,b.x), fmaxf(a.y,b.y)); }
static inline float2 fminf(float2 a, float2 b) { return float2(fminf(a.x,b.x), fminf(a.y,b.y)); }


