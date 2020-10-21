/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileRawChannel2Bytes5.h" 
#include "TileEvent/TileRawChannel.h" 
#include <algorithm> 
#include <cmath>

#define UINT32 uint32_t
#define UINT16 uint16_t
#define INT32  int32_t
#define INT16  int16_t

#define verb false

#define code_ped4 TileRawChannel2Bytes5::code_ped4
#define code_ped5 TileRawChannel2Bytes5::code_ped5
#define code_amp5 TileRawChannel2Bytes5::code_amp5
#define code_amp6 TileRawChannel2Bytes5::code_amp6
#define code_raws TileRawChannel2Bytes5::code_raws
#define code_rawf TileRawChannel2Bytes5::code_rawf
#define code_full TileRawChannel2Bytes5::code_full
#define code_dump TileRawChannel2Bytes5::code_dump
#define code_null TileRawChannel2Bytes5::code_null


namespace {


// 0 - ADC, 1 - pC, 2 - CspC, 3 - MeV
const int OF_E_FRACBITS[4] = { 4, 5, 5, -5 };

// ampcorr lookup table, step = 0.5 ns, scale = 2^14,
// time = [-32 .. 31], index = time + 32
const unsigned short ampcorr_lookup[64] = {
  0x505A, 0x4F62, 0x4E71, 0x4D87, 0x4CA4, 0x4BC8, 0x4AF4, 0x4A26,
  0x4960, 0x48A1, 0x47EA, 0x473A, 0x4692, 0x45F1, 0x4558, 0x44C7,
  0x443D, 0x43BC, 0x4342, 0x42D0, 0x4267, 0x4205, 0x41AC, 0x415B,
  0x4112, 0x40D2, 0x409A, 0x406B, 0x4044, 0x4026, 0x4011, 0x4004,
  0x4000, 0x4005, 0x4013, 0x402A, 0x404A, 0x4073, 0x40A6, 0x40E2,
  0x4127, 0x4175, 0x41CD, 0x422F, 0x429A, 0x430E, 0x438D, 0x4415,
  0x44A7, 0x4543, 0x45E9, 0x4699, 0x4753, 0x4817, 0x48E6, 0x49BF,
  0x4AA2, 0x4B8F, 0x4C87, 0x4D8A, 0x4E97, 0x4FAF, 0x50D2, 0x51FF
};

// ========== DSP Intrinstic Functions ========== //

int32_t _ext(int32_t d, uint32_t a, uint32_t b) {
  a = a & 0x1F; b = b & 0x1F;
  return ((int32_t)((d << a) & 0xFFFFFFFF)) >> b;
}
uint32_t _extu(uint32_t d, uint32_t a, uint32_t b) {
  a = a & 0x1F; b = b & 0x1F;
  return ((d << a) & 0xFFFFFFFF) >> b;
}
int32_t _extr(int32_t d, uint32_t ab) {
  return _ext(d, ab >> 5, ab);
}

#if 0
uint32_t _hi(uint32_t d) { return (d >> 16) & 0xFFFF; }
uint32_t _lo(uint32_t d) { return d & 0xFFFF; }
#endif
uint32_t _pack2(uint32_t a, uint32_t b) { return (a & 0xFFFF) << 16 | (b & 0xFFFF); }
#if 0
uint32_t _packh2(uint32_t a, uint32_t b) { return _pack2(a >> 16, b >> 16); }
uint32_t _sub2(uint32_t a, uint32_t b) { return _pack2(_hi(a) - _hi(b), _lo(a) - _lo(b)); }
uint32_t _set(uint32_t d, int a, int b) { return d |  _extu(0xFFFFFFFF, a, b); }
uint32_t _clr(uint32_t d, int a, int b) { return d & ~_extu(0xFFFFFFFF, a, b); }
uint32_t _setr(int a, int b) { return _set(0, a, b); }
#endif

unsigned char getbyte(const unsigned char* ptr) {
  //uint64_t u = (uint64_t) ptr;
  uintptr_t u = reinterpret_cast<uintptr_t> (ptr);
  u = u + 3 - 2*(u % 4);
  ptr = reinterpret_cast<const unsigned char*> (u);
  return *ptr;
}

#if 0
uint8_t _mem1(const unsigned char* ptr) {
  unsigned char b0;
  b0 = getbyte(ptr);
  return b0;
}
uint8_t _mem1(const char* ptr)     { return _mem1((const unsigned char*) ptr); }
#endif


#if 0
uint16_t _mem2(const unsigned char* ptr) {
  unsigned char b0, b1;
  b0 = getbyte(ptr++);
  b1 = getbyte(ptr++);
  return (b0 << 8) | b1;
}
uint16_t _mem2(const char* ptr)     { return _mem2((const unsigned char*) ptr); }
#endif

uint32_t _mem4(const unsigned char* ptr) {
  unsigned char b0, b1, b2, b3;
  b0 = getbyte(ptr++);
  b1 = getbyte(ptr++);
  b2 = getbyte(ptr++);
  b3 = getbyte(ptr++);
  return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

#if 0
uint32_t _mem4(const char* ptr)     { return _mem4((const unsigned char*) ptr); }
#endif

uint32_t _packhl2(uint32_t a, uint32_t b) { return (a & 0xFFFF0000) | (b & 0x0000FFFF); }


#if 0
uint32_t _norm(uint32_t d) {
  uint32_t u = d & 0x80000000;
  int n = 0;
  d = d << 1;
  while ((u == (d & 0x80000000)) && (n < 31)) {
    d = d << 1;
    n++;
  }
  return n;
}
#endif


int32_t _sadd(int32_t a, int32_t b) {
  int32_t u = a + b;
  if (a > 0 && b > 0 && u < 0) u = 0x7FFFFFFF; else
  if (a < 0 && b < 0 && u > 0) u = 0x80000000;
  return u;
}

int32_t _sshl(int32_t a, uint32_t b) {
  b = b & 0x1F;
  int32_t u = a << b;
  if ((u >> b) != a) {
    if (a > 0) u = 0x7FFFFFFF; else u = 0x80000000;
  }
  return u;
}

int32_t _mpy(uint32_t a, uint32_t b) {
  int16_t a16 = (a & 0xFFFF), b16 = (b & 0xFFFF);
  int32_t a32 = a16, b32 = b16; 
  return a32*b32;
}
int32_t _mpyh(uint32_t a, uint32_t b) { return _mpy(a >> 16, b >> 16); }
int32_t _mpyhl(uint32_t a, uint32_t b) { return _mpy(a >> 16, b); }

int32_t _dotp2(uint32_t a, uint32_t b) {
  return _mpy(a, b) + _mpyh(a, b);
}
uint32_t _ssub(int32_t a, int32_t b) {
  int64_t a64 = a, b64 = b;
  int64_t u = 0x100000000LL;
  if (a64 - b64 > u - 1) return (u - 1) & 0xFFFFFFFF;
  if (a64 - b64 < -u) return -u & 0xFFFFFFFF;
  return (a64 - b64) & 0xFFFFFFFF;
}

uint32_t _deal(uint32_t u) {
  uint32_t u1, u0, k, k1, k0;
  u1 = 0; u0 = 0;
  k = 1; k0 = 1; k1 = 1 << 16;

  for (int i = 0; i < 16; ++i) {
    if (u & k) u0 |= k0;
    k0 = k0 << 1;
    k  = k  << 1;

    if (u & k) u1 |= k1;
    k1 = k1 << 1;
    k  = k  << 1;
  }
  return u1 | u0;
}

/// Intrinsic functions which do not work correctly
/// due to differences in byte ordering

//uint32_t _hi(double d) { return (uint64_t)d >> 32; }
//uint32_t _lo(double d) { return (uint64_t)d & 0xFFFFFFFF; }
//
//double _memd8(uint32_t* ptr) {
//  uint64_t u = *ptr++;
//  u = u << 32;
//  u |= *ptr++;
//  return (double)u;
//}

// ========== Some definitions from Compress.h (DSP code) ========== //

#define sbits(u, n) (32 - 1 - (n) < _norm(u))
#define mvbits(u, n, i) _extu(u, 32 - (n), 32 - (n + i))
#define mvbits_ex(u, n, i, i0) mvbits(mvbits(u, n + i0, -(i0)), n, i)
#define nonsat_ene(ene) ((0 < ene) && (ene < 0x7FFF))

//#define _round(u, k) (_ext(u + _rotl(0x1, k - 1), 0, k))
#define _round(u, k) (_ext(u + _sshl(0x1, k - 1), 0, k))

#define ssatbits(u, n) _ext(_sshl(u, 32 - (n)), 0, 32 - (n))

// does not work for abs(u) >= 0x20000000
#define saturate(u, n) _ext(_sshl(u + _abs(u), 30 - (n)), 0, 31 - (n))

int get_code(UINT32 reco) { return TileRawChannel2Bytes5::get_code(reco); }

// ========== RarCompress.c (DSP code) ========== //

inline int _roundf(float f) {
  if (f > 0) return (int)(f + 0.5);
  else return (int)(f - 0.5);
}

UINT32 get_uncalib(int unit, UINT32 calib)
{
  // uncalib = 1/calib
  // 1.0 <=  calib  < 2.0
  // 0.5 < uncalib <= 1.0
  // 2^14 <=  calib_coeff  < 2^15, calib_scale   = 14
  // 2^13 < uncalib_coeff <= 2^14, uncalib_scale = 14

  int FRACBITS_MINUS_11 = _sadd(OF_E_FRACBITS[unit], 11);
  UINT16 calib_coeff   = _extu(calib,  0, 16);
  UINT16 calib_scale   = _extu(calib, 16, 16) - FRACBITS_MINUS_11;
  UINT16 uncalib_coeff = (int)((1.0/calib_coeff)*(1 << 14)*(1 << 14) + 0.5);
  UINT16 uncalib_scale = 17 - calib_scale;
  //UINT32 calib   = _pack2(calib_coeff,   calib_scale);
  UINT32 uncalib = _pack2(uncalib_coeff, uncalib_scale);
  return uncalib;
}

UINT32 repack_hg(UINT32 hg, UINT16 shift)
{
  return _packhl2(_extr(hg, shift), hg);
}

#if 0
INT16 get_ampcorr_coeff(int time)
{
  // Time with 0.5 ns step
  // time = t*2
  // time [-32..31] <==> t [-16.0 .. 15.5]
  double ampcorr = ((0.0000006467*time + 0.0002713687)*time + 0.0000365732)*time + 1;
  return _roundf(ampcorr*(0x1 << 14));
}


void print_ampcorr_lookup() {
  int time;
  int k = 0;
  printf("\n");
  printf("// ampcorr lookup table, step = 0.5 ns\n");
  printf("unsigned short ampcorr_lookup[64] = {"); 
  for (time = -32; time < 32; time++) {
    UINT16 ampcorr_coeff = get_ampcorr_coeff(time);
    if (k % 8 == 0) printf("\n ");
    k++;
    printf(" 0x%04X,", ampcorr_coeff);
  }
  printf("}\n");
}
#endif


#define unpack_diff(diff, nbits, d2, d3, d5, d6, d7)    \
{                                                       \
  UINT32 u = _deal(_deal((diff >> nbits)));             \
  d5 = _ext(diff, 32 - nbits, 32 - nbits);              \
  d7 = _ext(u,  8 - nbits, 32 - nbits);                 \
  d6 = _ext(u, 16 - nbits, 32 - nbits);                 \
  d3 = _ext(u, 24 - nbits, 32 - nbits);                 \
  d2 = _ext(u, 32 - nbits, 32 - nbits);                 \
}

#define fill_samp(s1, s2, s3, s4, s5, s6, s7, s)        \
{ int i = 0;                                            \
  s[i++] = s1;                                          \
  s[i++] = s2;                                          \
  s[i++] = s3;                                          \
  s[i++] = s4;                                          \
  s[i++] = s5;                                          \
  s[i++] = s6;                                          \
  s[i++] = s7;                                          \
}

int get_amp_reco(const UINT32* ofw, int unit, int chan, int gain, int ene0)
{
  const UINT32* ofc = (ofw + chan*NOFWORDS_WEIGHTS_7S + 
                       gain*NOFWORDS_WEIGHTS_7S_1GAIN);

  UINT32 calib    = ofc[4];
  UINT32 uncalib  = get_uncalib(unit, calib); // ofc[RECALIB_OFFSET + 1];
  INT32  amp_reco = _extr(_mpyhl(uncalib, ene0), uncalib);
  return amp_reco;
}

int get_s4(const UINT32* ofw, int unit, int chan, int gain, int ene,
  int s1, int s2, int s3, int s5, int s6, int s7)
{
  const UINT32* ofc = (ofw + chan*NOFWORDS_WEIGHTS_7S + 
                       gain*NOFWORDS_WEIGHTS_7S_1GAIN);

  UINT32 s1x, s32, s5x, s76;
  UINT32 a1x, a32, a54, a76;
  int amp, amp_reco, a4, s4;
  float a4s4;

  s1x = _pack2(s1,  0);
  s32 = _pack2(s3, s2);
  s5x = _pack2(s5,  0);
  s76 = _pack2(s7, s6);

  { // amp coeff
    //double a1x32 = _memd8(ofc);
    //double a5476 = _memd8(ofc + 2);
    a1x = ofc[0]; //a1x = _hi(a1x32);
    a32 = ofc[1]; //a32 = _lo(a1x32);
    a54 = ofc[2]; //a54 = _hi(a5476);
    a76 = ofc[3]; //a76 = _lo(a5476);
  }
  amp_reco = get_amp_reco(ofw, unit, chan, gain, ene);

  amp = _mpyh (a1x, s1x)
      + _dotp2(a32, s32)
      + _dotp2(a54, s5x)
      + _dotp2(a76, s76);

  a4s4 = amp_reco - amp;
  a4 = _ext(a54, 16, 16);
  s4 = _roundf(a4s4/a4);

  return s4;
}

inline UINT32 pop_buf(const unsigned char** pptr_buff, int size)
{
  UINT32 data = _mem4(*pptr_buff);
  data = _extu(data, 0, 32 - size);
  *pptr_buff += (size) >> 3;
  return data;
}

int calc_ped_s4(const UINT32* ofw, int unit, int chan, int gain, int ene,
  int s1, int s2, int s3, int s5, int s6, int s7)
{
  return get_s4(ofw, unit, chan, gain, ene, s1, s2, s3, s5, s6, s7);
}

void calc_amp(const UINT32* ofw, int unit, int chan, int gain, int ene, int time,
  int s1, int d2, int d3, int d5, int d6, int d7, int s[])
{
  int s2, s3, s4, s5, s6, s7;
  INT16 amp2, ampcorr, a_scale;
  int amptime, amp_reco;
  UINT32 wamptime;

  const UINT32* ofc = (ofw + chan*NOFWORDS_WEIGHTS_7S + 
                       gain*NOFWORDS_WEIGHTS_7S_1GAIN);

  a_scale = _extu(ofc[0], 16, 16); // a1x
  amp_reco = get_amp_reco(ofw, unit, chan, gain, ene);
  amp2 = (INT16) (amp_reco >> a_scale);

  ampcorr = ampcorr_lookup[time + 32];
  amp2 = _mpy(amp2, ampcorr) >> 14;
  amptime  = _sshl(_round(_mpy(amp2, time), 3), 16);
  wamptime = _packhl2(-amptime, amp2);
  {
    // g_scale, h_scale
    UINT16 g_scale = _extu(ofc[13], 16, 16);
    UINT16 h_scale = _extu(ofc[13], 0, 16) - 1;
        UINT16 shift   = h_scale - g_scale;

    // shape, derivative coeff
    //UINT32 hg1 = ofc[14];
    UINT32 hg2 = ofc[15];
    UINT32 hg3 = ofc[16];
    //UINT32 hg4 = ofc[17];
    UINT32 hg5 = ofc[18];
    UINT32 hg6 = ofc[19];
    UINT32 hg7 = ofc[20];

    int q2 = _extr(_dotp2(repack_hg(hg2, shift), wamptime), g_scale);
    int q3 = _extr(_dotp2(repack_hg(hg3, shift), wamptime), g_scale);
    int q5 = _extr(_dotp2(repack_hg(hg5, shift), wamptime), g_scale);
    int q6 = _extr(_dotp2(repack_hg(hg6, shift), wamptime), g_scale);
    int q7 = _extr(_dotp2(repack_hg(hg7, shift), wamptime), g_scale);

    s2 = q2 + d2 + s1;
    s3 = q3 + d3 + s1;
    s5 = q5 + d5 + s1;
    s6 = q6 + d6 + s1;
    s7 = q7 + d7 + s1;
  }
  s4 = get_s4(ofw, unit, chan, gain, ene, s1, s2, s3, s5, s6, s7);
  fill_samp(s1, s2, s3, s4, s5, s6, s7, s);
}

void calc_raw(const UINT32* ofw, int unit, int chan, int gain, int ene,
  int s1, int s2, int s3, int s5, int s6, int s7, int s[])
{
  int s4 = get_s4(ofw, unit, chan, gain, ene, s1, s2, s3, s5, s6, s7);
  fill_samp(s1, s2, s3, s4, s5, s6, s7, s);
}

int unpack_null(const UINT32* /* ofw */, int /* chan */, const UINT32** pptr_reco,
  int* gain, int* ene, int* time, int s[])
{
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  int i;
  if (code != code_null) return 0;
  (*pptr_reco)++;

  *gain = reco & 0x1;
  *ene = 0;
  *time = 0;
  for (i = 0; i < 7; i++)
    s[i] = (reco >> 1) & 0x3FF;

  return 32;
}

int unpack_ped(const UINT32* ofw, int unit, int chan,
  const UINT32** pptr_reco, const unsigned char** pptr_buff,
  int* gain, int* ene, int* time, int s[])
{
  int nbits;
  int size = 0;
  int s1, s2, s3, s4, s5, s6, s7, d2, d3, d5, d6, d7;
  UINT32 diff, data;
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  if (code != code_ped4 && code != code_ped5) return 0;
  (*pptr_reco)++;

  *gain = 1;
  *ene  = mvbits_ex(reco, 9, 0,  0) - 256;
  *time = 0;

  if (code == code_ped4) {
    s1 = mvbits_ex(reco, 7, 0, 9);
    nbits = 4;
    diff = mvbits_ex(reco, 20 - 8, 0, 7 + 9);
    data = pop_buf(pptr_buff, 8);
    diff |= _extu(data, 20 - 8, 0);
    size = 40;
  } else {
    s1 = mvbits_ex(reco, 10, 0, 9);
    nbits = 5;
    diff = mvbits_ex(reco, 25 - 16, 0, 10 + 9);
    data = pop_buf(pptr_buff, 16);
    diff |= _extu(data, 25 - 16, 0);
    size = 48;
  }

  unpack_diff(diff, nbits, d2, d3, d5, d6, d7);
  s2 = s1 + d2;
  s3 = s1 + d3;
  s5 = s1 + d5;
  s6 = s1 + d6;
  s7 = s1 + d7;

  s4 = calc_ped_s4(ofw, unit, chan, *gain, *ene, s1, s2, s3, s5, s6, s7);
  fill_samp(s1, s2, s3, s4, s5, s6, s7, s);
  return size;
}

int unpack_amp(const UINT32* ofw, int unit, int chan, 
  const UINT32** pptr_reco, const unsigned char** pptr_buff,
  int* gain, int* ene, int* time, int s[])
{
  int nbits = 0;
  int size = 0;
  int s1 = 0, d2, d3, d5, d6, d7;
  UINT32 diff = 0;
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  if (code != code_amp5 && code != code_amp6) return 0;
  (*pptr_reco)++;

  *gain = mvbits_ex(reco,  1, 0, 31);
  *ene  = mvbits_ex(reco, 15, 0,  0) - ((*gain == 0) ? 512 : 2048);
  *time = _ext(reco, 32 - 15 - 6,  32 - 6);

  if (code == code_amp5) {
    s1 = mvbits_ex(reco, 7, 0, 15 + 6);
    diff = mvbits_ex(reco, 25 - 24, 0, 15 + 6 + 7);
    diff = _extu(pop_buf(pptr_buff, 24), 1, 0) | diff;
    nbits = 5;
    size = 56;
  } else 
  if (code == code_amp6) {
    s1 = mvbits_ex(reco, 5, 0, 15 + 6);
    diff = pop_buf(pptr_buff, 32);
    s1 = _extu(diff, 32 - 2, 32 - 2 - 5) | s1;
    diff = _extu(diff, 0, 2);
    nbits = 6;
    size = 64;
  }
  unpack_diff(diff, nbits, d2, d3, d5, d6, d7);
  calc_amp(ofw, unit, chan, *gain, *ene, *time, s1, d2, d3, d5, d6, d7, s);
  return size;
}

int unpack_raw(const UINT32* ofw, int unit, int chan,
  const UINT32** pptr_reco, const unsigned char** pptr_buff,
  int* gain, int* ene, int* time, int s[])
{
  int s1, s2, s3, s5, s6, s7;
  UINT32 diff1, diff2;
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  int size = 0;
  if (code != code_raws && code != code_rawf) return 0;
  (*pptr_reco)++;

  *gain = mvbits_ex(reco,  1, 0, 31);
  *ene  = mvbits_ex(reco, 15, 0,  0) - ((*gain == 0) ? 512 : 2048);
  *time = 0;

  if (code == code_raws) {
    // code = code_raws
    diff1 = pop_buf(pptr_buff, 32);
    diff2 = pop_buf(pptr_buff,  8);
    s1 = mvbits_ex(reco, 8, 0, 15);
    s2 = mvbits_ex(diff1, 3, 6, 0) | mvbits_ex(reco, 6, 0, 15 + 8);
    s3 = mvbits_ex(diff1, 10, 0, 3);
    s5 = mvbits_ex(diff1, 10, 0, 10 + 3);
    s6 = mvbits_ex(diff1, 9, 0, 10 + 10 + 3);
    s7 = diff2;
    size = 72;
  } else {
    // code = code_rawf
    diff1 = pop_buf(pptr_buff, 32);
    diff2 = pop_buf(pptr_buff, 16);
    s1 = mvbits_ex(reco, 10, 0, 15);
    s2 = mvbits_ex(diff1,  8, 2, 0) | mvbits_ex(reco, 2, 0, 15 + 10);
    s3 = mvbits_ex(diff1, 10, 0, 8);
    s5 = mvbits_ex(diff1, 10, 0, 8 + 10);
    s6 = mvbits_ex(diff2,  6, 4, 0) | mvbits_ex(diff1, 4, 0, 8 + 10 + 10);
    s7 = mvbits_ex(diff2, 10, 0, 6);
    size = 80;
  }

  calc_raw(ofw, unit, chan, *gain, *ene, s1, s2, s3, s5, s6, s7, s);
  return size;
}

int unpack_full(const UINT32* /* ofw */, int /* chan */,
  const UINT32** pptr_reco, const unsigned char** pptr_buff,
  int* gain, int* ene, int* time, int s[])
{
  UINT32 diff1, diff2;
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  int overflow;
  if (code != code_full) return 0;
  (*pptr_reco)++;

  diff1 = pop_buf(pptr_buff, 32);
  diff2 = pop_buf(pptr_buff, 16);

  *gain = mvbits_ex(reco, 1, 0, 31);
  overflow = mvbits_ex(reco, 1, 0, 23);
  *ene  = (overflow ? 0x7FFF : 0) - ((*gain == 0) ? 512 : 2048);
  *time = 0;

  s[1-1] = mvbits_ex(reco, 10, 0,  0);
  s[2-1] = mvbits_ex(reco, 10, 0,  0 + 10);
  s[3-1] = mvbits_ex(reco,  2, 0,  0 + 10 + 10) | mvbits_ex(diff1, 8, 2, 0);
  s[4-1] = mvbits_ex(diff1, 10, 0, 8);
  s[5-1] = mvbits_ex(diff1, 10, 0, 8 + 10);
  s[6-1] = mvbits_ex(diff1,  4, 0, 8 + 10 + 10) | mvbits_ex(diff2, 6, 4, 0);
  s[7-1] = mvbits_ex(diff2, 10, 0, 6);

  return 80;
}

int unpack_dump(const UINT32* /* ofw */, int /* chan */,
  const UINT32** pptr_reco, const unsigned char** pptr_buff,
  int* gain, int* ene, int* time, int s[])
{
  UINT32 diff1, diff2;
  UINT32 reco = **pptr_reco;
  UINT32 code = get_code(reco);
  if (code != code_dump) return 0;
  (*pptr_reco)++;

  diff1 = pop_buf(pptr_buff, 32);
  diff2 = pop_buf(pptr_buff, 24);

  *gain = mvbits_ex(reco, 1, 0, 31);
  *ene  = mvbits_ex(reco, 15, 0,  0) - ((*gain == 0) ? 512 : 2048);
  *time = 0;

  s[1-1] = mvbits_ex(reco,  10, 0, 15);
  s[2-1] = mvbits_ex(diff1,  6, 4, 0) | mvbits_ex(reco,  4, 0, 15 + 10);
  s[3-1] = mvbits_ex(diff1, 10, 0, 6);
  s[4-1] = mvbits_ex(diff1, 10, 0, 6 + 10);
  s[5-1] = mvbits_ex(diff2,  4, 6, 0) | mvbits_ex(diff1,  6, 0, 6 + 10 + 10);
  s[6-1] = mvbits_ex(diff2, 10, 0, 4);
  s[7-1] = mvbits_ex(diff2, 10, 0, 4 + 10);

  return 88;
}

} // anonymous namespace


// ========== DSP reconstruction of Amplitude (DSP code) ========== //


int TileRawChannel2Bytes5::amplitude(const UINT32* ofw, int unit, int chan, int gain, int s[]) const
{

  UINT32 calib, calib_coeff;
  INT16 calib_scale;
  int amp, ene, ene0, ene_shift;
  //INT16 amp2;
  const UINT32* ofc;

  int FRACBITS_MINUS_11 = _sadd(OF_E_FRACBITS[unit], 11);

  int s1 = s[0], s2 = s[1], s3 = s[2], s4 = s[3], s5 = s[4], s6 = s[5], s7 = s[6];

  // samples
  UINT32 s1x = _pack2(s1, gain << 15);
  UINT32 s32 = _pack2(s3, s2);
  UINT32 s54 = _pack2(s5, s4);
  UINT32 s76 = _pack2(s7, s6);

  ene_shift = 512 + _mpy(gain, 2048 - 512);

  ofc = ofw + chan*NOFWORDS_WEIGHTS_7S + gain*NOFWORDS_WEIGHTS_7S_1GAIN;

  // amp coeff
  UINT32 a1x = ofc[0];
  UINT32 a32 = ofc[1];
  UINT32 a54 = ofc[2];
  UINT32 a76 = ofc[3];
  //UINT16 a_scale = _extu(a1x, 16, 16);

  amp = _mpyh (a1x, s1x)
      + _dotp2(a32, s32)
      + _dotp2(a54, s54)
      + _dotp2(a76, s76);

  // calib
  calib = ofc[4];

  calib_coeff = _ext(calib, 0, 16);
  calib_scale = _ssub(_extu(calib, 16, 16) , FRACBITS_MINUS_11);
  ene0 = _round(amp, 11);
  ene0 = _round(ene0*calib_coeff, calib_scale);
  ene = ene0 + ene_shift;

  return ene;
}


void TileRawChannel2Bytes5::unpack(const UINT32* ofw, const UINT32* ptr_frag, TileChanData* ChanData) const
{
  int unit    = _extu(ptr_frag[2], 0, 32 - 2); // frag_info = xx......
  int size_L2 = _extu(ptr_frag[2], 2, 32 - 3); // frag_info = ..xxx...
  int i, chan, code, size;
  int gain(0), bad, ene(0), time(0), u[7];
  const UINT32* ptr_reco = (const UINT32*) (ptr_frag + 3); // + Header
  const unsigned char* ptr_buff = (const unsigned char*) (ptr_frag + 3 + 48 + size_L2); // + Header + Reco + Size_L2
  const UINT32* ptr = ptr_reco;
  UINT16 bad_bits[3];
  UINT16 bad16 = 0;

  const unsigned char* ptr_ped4;
  const unsigned char* ptr_ped5;
  const unsigned char* ptr_amp5;
  const unsigned char* ptr_amp6;
  const unsigned char* ptr_raws;
  const unsigned char* ptr_rawf;
  const unsigned char* ptr_full;
  const unsigned char* ptr_dump;

  int cnt_ped4, cnt_ped5, cnt_amp5, cnt_amp6, 
      cnt_raws, cnt_rawf, cnt_full, cnt_null, cnt_dump;
  cnt_ped4 = cnt_ped5 = cnt_amp5 = cnt_amp6 = 
  cnt_raws = cnt_rawf = cnt_full = cnt_null = cnt_dump = 0;
  
  bad_bits[0] = pop_buf(&ptr_buff, 16);
  bad_bits[1] = pop_buf(&ptr_buff, 16);
  bad_bits[2] = pop_buf(&ptr_buff, 16);

  for (chan = 0; chan < 48; ++chan) {
    for (i = 0; i < 7; ++i) u[i] = 0;
    code = get_code(ptr_reco[chan]);
    if (code == code_ped4) cnt_ped4++; else
    if (code == code_ped5) cnt_ped5++; else
    if (code == code_amp5) cnt_amp5++; else
    if (code == code_amp6) cnt_amp6++; else
    if (code == code_raws) cnt_raws++; else
    if (code == code_rawf) cnt_rawf++; else
    if (code == code_full) cnt_full++; else
    if (code == code_dump) cnt_dump++; else
    if (code == code_null) cnt_null++;
  }

  ptr_ped4 = ptr_buff;
  ptr_ped5 = ptr_ped4 + cnt_ped4*1; // 40 - ped4
  ptr_amp5 = ptr_ped5 + cnt_ped5*2; // 48 - ped5
  ptr_amp6 = ptr_amp5 + cnt_amp5*3; // 56 - amp5
  ptr_raws = ptr_amp6 + cnt_amp6*4; // 64 - amp6
  ptr_rawf = ptr_raws + cnt_raws*5; // 72 - raws
  ptr_full = ptr_rawf + cnt_rawf*6; // 80 - rawf
  ptr_dump = ptr_full + cnt_full*6; // 80 - full

  for (chan = 0; chan < 48; ++chan) {
    if (chan % 16 == 0) bad16 = ~bad_bits[chan/16]; // note inversion here
    bad = (bad16 & 0x1); bad16 >>= 1;
    code = get_code(ptr_reco[chan]);

    size = 0;
    if (code == code_ped4) size = unpack_ped (ofw, unit, chan, &ptr, &ptr_ped4, &gain, &ene, &time, u); else
    if (code == code_ped5) size = unpack_ped (ofw, unit, chan, &ptr, &ptr_ped5, &gain, &ene, &time, u); else
    if (code == code_amp5) size = unpack_amp (ofw, unit, chan, &ptr, &ptr_amp5, &gain, &ene, &time, u); else
    if (code == code_amp6) size = unpack_amp (ofw, unit, chan, &ptr, &ptr_amp6, &gain, &ene, &time, u); else
    if (code == code_raws) size = unpack_raw (ofw, unit, chan, &ptr, &ptr_raws, &gain, &ene, &time, u); else
    if (code == code_rawf) size = unpack_raw (ofw, unit, chan, &ptr, &ptr_rawf, &gain, &ene, &time, u); else
    if (code == code_full) size = unpack_full(ofw,       chan, &ptr, &ptr_full, &gain, &ene, &time, u); else
    if (code == code_dump) size = unpack_dump(ofw,       chan, &ptr, &ptr_dump, &gain, &ene, &time, u); else
    if (code == code_null) size = unpack_null(ofw,       chan, &ptr,            &gain, &ene, &time, u);
    if (size == 0) printf("\nTileRawChannel2Bytes5::unpack ERROR size = 0\n");

    // Fill ChanData
    ChanData[chan].code    = code;
    ChanData[chan].gain    = gain;
    ChanData[chan].bad     = bad;
    ChanData[chan].ene_bin = ene;
    ChanData[chan].time_bin = time;

    ChanData[chan].ene = (gain) ? ene/AMPLITUDE_FACTOR5_HG[unit]
                                : ene/AMPLITUDE_FACTOR5_LG[unit];
    ChanData[chan].time = time/2.0;

    for (i = 0; i < 7; ++i) ChanData[chan].s[i] = u[i];
  }
}

bool TileRawChannel2Bytes5::check_raw(const uint32_t* feb, int of_energy[], TileChanData* Data) const
{
  bool OK = true;
  int chan, i;
  int gain, ene, u[7], s[7];
  const UINT32* data = feb + 3;
  for (chan = 0; chan < 48; ++chan) {
    bool chOK = true;
    //code = Data[chan].code;
    //bad  = Data[chan].bad;
    gain = Data[chan].gain;
    ene  = Data[chan].ene_bin;
    //time = Data[chan].time_bin;

    for (i = 0; i < 7; ++i) u[i] = Data[chan].s[i];
        {
      //double s1x32 = _memd8(data + chan*4);
      //double s5476 = _memd8(data + chan*4 + 2);
      UINT32 s1x = data[chan*4 + 0]; //_hi(s1x32);
      UINT32 s32 = data[chan*4 + 1]; //_lo(s1x32);
      UINT32 s54 = data[chan*4 + 2]; //_hi(s5476);
      UINT32 s76 = data[chan*4 + 3]; //_lo(s5476);
      s[1-1] = _extu(s1x,  0, 16);
      s[2-1] = _extu(s32, 16, 16);
      s[3-1] = _extu(s32,  0, 16);
      s[4-1] = _extu(s54, 16, 16);
      s[5-1] = _extu(s54,  0, 16);
      s[6-1] = _extu(s76, 16, 16);
      s[7-1] = _extu(s76,  0, 16);
    }

    for (i = 0; i < 7; ++i) {
      if (s[i] != u[i]) chOK = false;
    }
    if (!chOK) { printf("\n %d: Raw Data ERROR", chan); }

    ene += (gain == 0 ? 512 : 2048);

    if (of_energy[chan] != ene) {
      OK = false;
      printf("\n %d : amp error ene: %d  e_t_amp: %d\n", chan, ene, of_energy[chan]);
    }

    if (!chOK) {
      OK = false;
      printf("\n %2d ERROR", chan);
    }
  }
  printf("\nunpack_data");
  if (OK) printf(" OK"); else printf(" ERROR");
  printf("  ");
  return OK;
}

bool TileRawChannel2Bytes5::check_reco(const UINT32* frag, int of_energy[]) const
{
  int chan;
  bool OK = true;
  int bad, gain, amp, ene, time;
  const UINT32* reco = (frag + 3); // + Header
  UINT32 w;

  for (chan = 0; chan < 48; ++chan) {
    w = reco[chan];

    if (!unpack_reco_bin(w, bad, gain, amp, time)) {
      printf("\nTileRawChannel2Bytes5::unpack_reco_bin ERROR\n");
      OK = false;  
    }

    ene = amp + AMPLITUDE_OFFSET5[gain];

    if (of_energy[chan] != ene) {
      OK = false;
      //printf("\n  "); print_code(code);
      //printf(" amp error ene: %d  e_t_amp: %d", amp, of_energy[chan]);
    }
  } // for

  return OK;
}
