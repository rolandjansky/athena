/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/Root/crc64.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief A crc-64 implementation, using pclmul where possible.
 *
 * This is an implementation of the crc64 calculation, as used in Athena,
 * that takes advantage of the x86_64 pclmul instruction where possible.
 *
 * This implements the algorithm described in this reference:
 *
 *   Gopal, et al., ``Fast CRC Computation for Generic Polynomials
 *   Using PCLMULQDQ Instruction,'' Intel White Paper 323102 (2009).
 *   <http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf>
 *   <https://github.com/intel/soft-crc>
 *
 * An alterate implementation of this is here:
 *   <https://github.com/rawrunprotected/crc>
 *
 * and a useful general reference for CRC calculations is:
 *   <https://zlib.net/crc_v3.txt>.
 *
 * In general, given an arbitrarily long message M expressed as a single
 * binary number, the CRC is calculated as
 *
 *   CRC = 2^w M mod P
 *
 * where the arithmetic is done in a `carryless' fashion; that is,
 * `addition' becomes the xor operation (and subtraction is thus
 * the same as addition).  Here, w is the number of bits in the desired
 * result; the most-significant bit of P should be 2^w.  (This is like
 * appending w 0's to the end of M.)  One often uses language in which M and P
 * are considered to be polynomials, one term per bit in the binary
 * expansion; and hence P is usually called the `polynomial'.
 * This comes from considering the arithmetic as an instance of a Galois
 * field.  However, if one is simply trying to understand a CRC algorithm,
 * this language often obscures more that it clarifies, so we will usually
 * not use it here (except for calling P the polynomial).
 *
 * As a concrete example, the CRC of 0x96 with a polynomal of 0x16 may
 * be calculated like this
 *
 *    100101100000
 *    10110
 *    -----
 *     010011
 *      10110
 *      -----
 *      0010100
 *        10110
 *        -----
 *        00010000
 *           10110
 *           -----
 *            0110
 *
 * So the remainder, and the CRC, is 6.  Note that this is nothing more
 * than binary long division with xor taking the place of addition and
 * subtraction.
 *
 * One can process more than one bit at a time in the division using
 * a lookup table with a number of entries equal to 2 to the number
 * of bits (see crc_v3.txt for details).  Implementations commonly do
 * a byte at a time, and that is what is implemented here as the
 * non-vectorized version.  One can use larger sizes than 8 bits, but
 * memory requirements soon become prohibitive.
 *
 * The implementation used here speeds up the CRC calculation by using
 * the pclmul instruction, which does a carryless multiplication of two
 * 64-bit quantities, producing a 127-bit result.  This allows processing
 * 128 bits at a time.
 *
 * A few other subtleties are worth mentioning.  First, when the value
 * of p is written, the initial 1 bit is often dropped.  So for the example
 * above, we were calculating a 4-bit CRC with a p of 0x16, but in practice
 * one might drop the initial 1 on p and  write it as 0x6 (with the bit
 * width understood).  Second, as stated here, the CRC value is insensitive
 * to the number of leading 0's on M.  This is usually not a desirable
 * feature, so the CRC calculation is very often initialized with something
 * other than 0.  A value of all bits set is common, and that is what
 * is used as a default here.  Third, the bits in each byte are sometimes
 * reversed before the CRC calculation.  This originated due to the way
 * a UART serial interface works, but has spread to many cases that don't
 * have anything to do with serial ports.  The implementation here is such
 * a reversed CRC (simply because that's the way the original bytewise
 * implementation we're trying to match worked).
 *
 * The way a CRC calculation works is by iteratively reducing the length
 * of the message in such a way that the CRC does not change.  For example,
 * suppose we have a message M that is split into two parts, A of T bits,
 * and B of U bits.  Thus, M = 2^U A + B.  Then suppose we find a shorter
 * string A' with T' bits, such that A mod P = A' mod P.  It is then easy
 * to show that the string M' = 2^U A' + B has the same CRC:
 *
 *   M  mod P = [(x^U mod P)(A  mod P)] mod P + B mod P
 *   M' mod P = [(x^U mod P)(A' mod P)] mod P + B mod P
 *
 * But A mod P = A' mod P, so therefore M mod P = M' mod P.
 *
 * The algorithm here consists of two parts: a `folding' step that is applied
 * iteratively, and a final `reduction' step.
 *
 * For a folding step, we take a block of 256 bits (A in the above example)
 * and reduce it to a block of 128 bits (A' in the above).  Suppose
 * we have a 256-bit A that's divided into a 64-bit H, a 64-bit L,
 * and a 128-bit G:
 *
 *   A = 2^192 H + 2^128 L + G
 *
 * Then C = A mod P =
 *    [(2^192 mod P) (H mod P)] mod P + [(2^128 mod P) (L mod P)] mod P+ G mod P
 *
 * Since H and L are 64-bits and P has the 2^64 bit implicitly set,
 * H mod P = H, and similarly for L.  So
 *
 * A mod P = A' mod P, where
 * A' = (2^192 mod P) H + (2^128 mod P) L + G mod P
 *
 * (where again, addition here means xor).  Note that the coefficients
 * 2^192 mod P and 2^128 mod P are constants that can be precomputed.
 *
 * So the folding algorithm works by keeping a 128-bit accumulator.
 * At each step, we divide it up into high and low 64-bit halves,
 * do a clmul of each half with the corresponding constant, and xor these
 * two results together with the next 128-bit block.
 *
 * For the reversed case, nearly everything is the same, since bit ordering
 * doesn't matter for carryless multiplication.  The only thing to watch out
 * for is that the results need to be shifted one place to the left, since
 * pclmul produces a result in the 127 least-significant bits.  If * is
 * carryless multiplication, then
 *
 *  reflect(a)*reflect(b) << 1 = reflect(a*b)
 *
 * The shift can be applied to the pre-computed constants (they also need
 * to be bit-reflected).
 *
 * Once the entire message has been reduced to 128 bits (see below for
 * cases where it doesn't come out evenly), we need to fold in the
 * 64 trailing 0 bits in the CRC definition.  Let M be divided into
 * 64-bit halves H and L:
 *
 *   M = 2^64 H + L
 *
 * Then
 *
 *   2^64 M mod P = [(2^128 mod P) H] mod P + (2^64 L) mod P
 *                = [(2^128 mod P) H + 2^64 L] mod P
 *
 * where 2^128 mod P is again one of the constants that was used for folding.
 *
 * Finally, the 128-bit folded value is reduced to a 64-bit CRC using
 * the technique of Barrett reduction:
 *
 *  <https://en.m.wikipedia.org/wiki/Barrett_reduction>
 *  <https://www.nayuki.io/page/barett-reduction-algorithm>
 *
 * If we want to compute C = M mod P, where C < M^2, then we can precompute
 * s = 1/P and then C = M - floor(M s)P.  In the case of fixed-point arithmetic,
 * it is convenient to scale s by a power of 2 so that it is not less than one.
 * We precompute mu = floor(2^128 / P).  Then
 *
 *  CRC = M - floor(M mu / 2^128) P
 *
 * which we implement as (remember all operations are carryless, hence
 * addition and subtraction are both xor):
 *
 *  T1 = floor(M / 2^64) mu
 *  T2 = floor(T1 / x^64) * P
 *  CRC = (M + T2) mod 64
 *
 * For the reflected case, we again need to reflect/shift the constants.
 *
 * In the cases where the size of the message is not divisible by 128 bits,
 * we need one more folding round for the remainder.
 *
 * Performance: On an intel i7-4600M CPU, the version using
 * pclmul is about ten times faster than the bytewise version
 * for long strings (50-60k bytes).  For short strings (20-40 bytes),
 * it is about five times faster.  On a Celeron C2840 CPU, the speedups
 * are more modest, four and 1.5 times, respectively.
 *
 * We currently support compiling this only with gcc.
 * clang doesn't implement funciton multiversioning, and some of the
 * intrinsics are missing/renamed.  We haven't tried it on icc, but it's
 * unlikely to work without adjustment.
 */


#include "CxxUtils/crc64.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include <stdio.h>


#if ATH_CRC64_VEC
/// Vector types.  All these make one SSE register.

// Two 64-bit integers.
typedef long long int  v2di __attribute__ ((vector_size (16)));


// Two unsigned 64-bit integers.
typedef uint64_t v2du __attribute__ ((vector_size (16)));


// 16 signed characters.
typedef char v16qi __attribute__ ((vector_size (16)));


#endif


namespace {


//***************************************************************************
// Primitive functions and utilities.
//


#if ATH_CRC64_VEC
/**
 * @brief Load a 128-bit chunk into a vector register (unaligned).
 * @param x Address from which to load.
 *          The pointer need not be aligned.
 */
inline
v2di load_unaligned (const char* x)
{
  return (v2di)__builtin_ia32_loaddqu (x);
}


/**
 * @brief Load a 128-bit chunk into a vector register (aligned).
 * @param x Address from which to load.
 *          The pointer must be aligned to a 16-byte boundary.
 */
inline
v2di load_aligned (const char* x)
{
  return *(v2di*)x;
}


/**
 * @brief Shift a 128-bit vector register left by @c n bytes.
 * @param x Value to shift.
 * @param n Number of bytes to shift.
 */
// A macro, not a function, because the second argument is constrained
// to be an 8-bit constant.  If this were a function, compilation might fail
// if it is not inlined.
#define byteshift_l(X, N) (__builtin_ia32_pslldqi128 ((X), (N)*8))


/**
 * @brief Shift a 128-bit vector register right by @c n bytes.
 * @param x Value to shift.
 * @param n Number of bytes to shift.
 */
// A macro, not a function; see above.
#define byteshift_r(X, N) (__builtin_ia32_psrldqi128 ((X), (N)*8))


/**
 * @brief Carryless multiplication of two 64-bit values, yielding 128 bits.
 * @param a First factor.
 * @param b Second factor.
 * @param which Selects which halves of the inputs get multiplied.
 *                0x00: Low  half of a, low  half of b.
 *                0x01: High half of a, low  half of b.
 *                0x10: Low  half of a, high half of b.
 *                0x11  High half of a, high half of b.
 */
// A macro, not a function; see above.
#define clmul(A, B, WHICH) (__builtin_ia32_pclmulqdq128 ((A), (B), (WHICH)))


/**
 * @brief Left byteshift with carry.
 * @param in Value to shift.
 * @param n Number of bytes to shift.
 * @param outLow Low part of output.
 * @param outHigh High part of output.
 *
 * For example, if IN is 0x1f1e1d1c1b1a1918 1716151413121110 (high low),
 * and we shift 6 places, then we'll have:
 *   outHigh: 0x0000000000000000 00001f1e1d1c1b1a
 *   outLow:  0x1918171615141312 1110000000000000
 */
__attribute__ ((target ("sse4")))
inline
void byteshift_l256 (v2di in, size_t n, v2di& outHigh, v2di& outLow)
{
  static const uint8_t shuffleMasks[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x8f, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80,
  };
  
  const v16qi mask = (v16qi)load_unaligned ((const char*)shuffleMasks + (16-n));
  outLow  = (v2di)__builtin_ia32_pshufb128 ((v16qi)in, ~mask);
  outHigh = (v2di)__builtin_ia32_pshufb128 ((v16qi)in,  mask);
}


/**
 * @brief Conditional on the high bit of x.
 * @param x Value in which to test the high bit.
 * @param y Value to return if the high bit is set.
 *
 * Returns: The equivalent of  (x & (1<<63)) ? y : 0
 */
inline
uint64_t hightest (uint64_t x, uint64_t y)
{
  // Relies on sign-extension of right-shift of a signed int.
  // This is strictly speakign implementation-defined behavior.
  // Since this code is anyway enabled only on x86_64, that's ok.
  // cppcheck-suppress shiftTooManyBitsSigned
  return y & (static_cast<int64_t>(x)>>63);
}


/**
 * @brief Calculate 2^exp mod p, using caryless multiplication.
 * @param exp The leading exponent.
 * @param p The modulus (polynomial).
 *          The leading 2^64 is implicitly taken to be SET.
 *          (So @p is actually the modulus ^ (1<<64). )
 *
 * In polynomial language, this is calculating x^exp mod p(x).
 */
uint64_t exp_mod (unsigned exp, uint64_t p)
{
  // This is basically just doing binary long division without carry
  // (so subtraction becomes xor).
  uint64_t d = p;
  for (unsigned i=0; i < exp-64; i++) {
    d = (d<<1) ^ hightest (d, p);
  }
  return d;
}


/**
 * @brief Calculate 2^129 mod p, using carryless multiplication.
 * @param p The modulus (polynomial).
 *          The leading 2^64 is implicitly taken to be SET.
 *          (So @p is actually the modulus ^ (1<<64). )
 *
 * The leading 2^64 bit of the result is implicitly SET.
 */
uint64_t exp129_div (uint64_t p)
{
  // Again, just binary long division without carry.
  uint64_t q = 0;
  uint64_t h = p;
  for (unsigned i=0; i < 64; i++) {
    q |= (h & (1ull << 63)) >> i;
    h = (h << 1) ^ hightest (h, p);
  }
  return q;
}


#endif // ATH_CRC64_VEC


/*
 * @brief Reflect the bits in a 64-bit word around the center.
 * @param v Value to reflect.
 *
 * So, for example,  0x0120034005600780 becomes
 *                   0x01e006a002c00480
 *
 * Reference: https://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
 * Originally credited to E. Freed, Dr. Dobbs's Journal 8 no. 4, p. 24 (1983).
 */
uint64_t bit_reflect (uint64_t v)
{
  v = ((v >> 1) & 0x5555555555555555) | ((v & 0x5555555555555555) << 1);
  v = ((v >> 2) & 0x3333333333333333) | ((v & 0x3333333333333333) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0F) | ((v & 0x0F0F0F0F0F0F0F0F) << 4);
  v = ((v >> 8) & 0x00FF00FF00FF00FF) | ((v & 0x00FF00FF00FF00FF) << 8);
  v = ((v >> 16) & 0x0000FFFF0000FFFF) | ((v & 0x0000FFFF0000FFFF) << 16);
  v = (v >> 32) | (v << 32);
  return v;
}


//****************************************************************************
// CRC helpers.
//


#if ATH_CRC64_VEC
/**
 * @brief Perform one round of CRC folding.
 * @param fold Current folded buffer.
 * @param data Next 128-bit data words.
 * @param k Precomputed constants (k1, k2; see above).
 *
 * Folds together two 128-bit blocks so that the result has the same CRC.
 */
__attribute__ ((target ("pclmul")))
inline
v2di folding_round (v2di fold, v2di data, v2di k)
{
  return data
    ^ clmul (fold, k, 0x00)
    ^ clmul (fold, k, 0x11);
}


/**
 * @brief Fold in the trailing 64 bits of zeros.
 * @param fold Folding buffer.
 * @param k Precomputed constants (k1, k2; see above).
 */
__attribute__ ((target ("pclmul")))
inline
v2di fold_trailing_zeros (v2di data, v2di k)
{
  return clmul (data, k, 0x10) ^  byteshift_r (data, 8);
}


/**
 * @brief Perform final Barrett reduction step.
 * @param R Buffer to reduce.
 * @param k Precomputed constants (mu, p; see above).
 */
__attribute__ ((target ("pclmul")))
inline
v2di barrett_reduce (v2di R, v2di k)
{
  v2di T1  = clmul (R,  k, 0x00);
  return R
    ^ clmul (T1, k, 0x10)
    ^ byteshift_l (T1, 8);
}
#endif


} // anonymous namespace


namespace CxxUtils {


//***************************************************************************
// Public entry points.
//

/**
 * @brief Precomputed tables and constants for the CRC calculation.
 */
class CRCTable
{
public:
  /**
   * @brief Initialize the CRC tables and constants.
   * @param p Polynomial for the CRC.
   *          A 1 in the 2^64 bit is implied.
   */
  CRCTable (uint64_t p, uint64_t initial = 0xffffffffffffffff);

  /// Initial CRC value.
  uint64_t m_initial;

  /// Lookup table for bytewise CRC calculation.
  uint64_t m_table[256];

#if ATH_CRC64_VEC
  /// Constants used for the folding step.
  v2di m_fold_constants;

  /// Constants used for the final Barrett reduction step.
  v2di m_barrett_constants;
#endif
};


/**
 * @brief Initialize the CRC tables and constants.
 * @param p Polynomial for the CRC.
 *          A 1 in the 2^64 bit is implied.
 */
CRCTable::CRCTable (uint64_t p, uint64_t initial /* = 0xffffffffffffffff*/)
{
  m_initial = initial;

  uint64_t prev = bit_reflect (p);
  for (int i = 0; i < 256; i++)
  {
    uint64_t r = i;
    for (int j = 0; j < 8; j++)
    {
      if (r & 1)
        r = (r >> 1) ^ prev;
      else
        r >>= 1;
    }
    m_table[i] = r;
  }

#if ATH_CRC64_VEC
  const uint64_t k1 = bit_reflect (exp_mod (128+64, p)) << 1;
  const uint64_t k2 = bit_reflect (exp_mod (128, p)) << 1;
  const uint64_t mu = (bit_reflect (exp129_div (p)) << 1) | 1;
  const uint64_t prev65 =  (bit_reflect (p) << 1) | 1;
  v2du a = {k1, k2};
  m_fold_constants = reinterpret_cast<v2di>(a);
  v2du b = { mu, prev65 };
  m_barrett_constants = reinterpret_cast<v2di>(b);
#endif
}


// Polynomial taken from code from David T. Jones (dtj@cs.ucl.ac.uk).
// (The form given here is reversed.)
// http://www0.cs.ucl.ac.uk/staff/D.Jones/crcnote.pdf
const CRCTable defaultCRCTable (0xad93d23594c935a9);


/**
 * @brief Delete a CRC table object.
 */
void deleteCRCTable (CxxUtils::CRCTable* table)
{
  delete table;
}


/**
 * @brief Initialize CRC tables and constants.
 * @param p Polynomial for the CRC.
 *          A 1 in the 2^64 bit is implied.
 * @param initial Initial CRC value.
 */
std::unique_ptr<CRCTable> makeCRCTable (uint64_t p,
                                        uint64_t initial /*= 0xffffffffffffffff*/)
{
  return std::make_unique<CRCTable> (p, initial);
}


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm.
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64_bytewise (const CRCTable& table,
                         const char* data,
                         size_t data_len)
{
  uint64_t crc = table.m_initial;
  const char* seq = data;
  const char* end = seq + data_len;
  while (seq < end)
    crc = table.m_table[(crc ^ *seq++) & 0xff] ^ (crc >> 8);
  return crc;
}


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm,
 *        with the default CRC.
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64_bytewise (const char* data,
                         size_t data_len)
{
  return crc64_bytewise (defaultCRCTable, data, data_len);
}


/**
 * @brief Find the CRC-64 of a string, using a byte-by-byte algorithm,
 *        with the default CRC.
 * @param table Precomputed CRC tables and constants.
 * @param s String to hash.
 */
uint64_t crc64_bytewise (const std::string& s)
{
  return crc64_bytewise (defaultCRCTable, s.data(), s.size());
}


#if ATH_CRC64_VEC
/**
 * @brief Find the CRC-64 of a string, using a vectorized algorithm.
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
__attribute__ ((target ("pclmul")))
uint64_t crc64 (const CRCTable& table,
                const char* data,
                size_t data_len)
{
  uint64_t crc = table.m_initial;

  // Early exit if the string is null.
  if (ATH_UNLIKELY(!data_len)) return crc;

  // The main body assumes that the data are aligned to 128 bits.
  // This should almost always be the case.  But just in case the input
  // string is not, consume the initial unaligned portion byte-by-byte
  // until it is.
  if (ATH_UNLIKELY (reinterpret_cast<unsigned long>(data) & 15)) {
    // Number of unaligned bytes we need to read from the start of the string.
    size_t leadin = std::min (16 - (reinterpret_cast<unsigned long>(data) & 15), data_len);
    crc = crc64_bytewise (table, data, leadin);
    data += leadin;
    data_len -= leadin;

    if (ATH_UNLIKELY(!data_len)) return crc;
  }

  // Accumulator for CRC value.
  v2di fold = {static_cast<int64_t>(crc), 0};

  // Constants for the folding step.
  v2di k = table.m_fold_constants;

  if (ATH_UNLIKELY (data_len < 16)) {
    // Special case for less than 128 bits.
    v2di temp2 = load_aligned (data);
    v2di crc0, crc1;
    byteshift_l256 (fold, 16-data_len, crc1, crc0);
    v2di A, B;
    byteshift_l256 (temp2, 16-data_len, B, A);

    fold = A ^ crc0;
    fold = fold_trailing_zeros (fold, k);
    fold ^= byteshift_l (crc1, 8);
  }
  else {
    // We have 128 bits or more.

    // Load the first 128 bits.
    fold ^= load_aligned (data);

    // Main folding loop.  Fold in 128 bits at a time until there
    // are fewer than 128 left.
    size_t n = 16;
    for (; n+16 <= data_len; n += 16) {
      v2di temp2 = load_aligned (data + n);
      fold = folding_round (fold, temp2, k);
    }

    // Handle a partial block at the end of less than 128 bits.
    if (ATH_LIKELY (n < data_len)) {
      v2di remainder = load_aligned (data + n);
      // Number of remaining bytes.
      size_t nrem = data_len - n;
      v2di A, B, C, D;
      byteshift_l256 (fold, 16-nrem, B, A);
      byteshift_l256 (remainder, 16-nrem, D, C);
      fold = folding_round (A, B|C, k);
    }
    fold = fold_trailing_zeros (fold, k);
  }

  /// Do final Barrett reduction step.
  fold = barrett_reduce (fold, table.m_barrett_constants);

  return fold[1];
}


#endif // ATH_CRC64_VEC


/**
 * @brief Find the CRC-64 of a string,
 * @param table Precomputed CRC tables and constants.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 *
 * This is the default implementation, used on platforms without pclmul.
 */
#if ATH_CRC64_VEC
__attribute__ ((target ("default")))
#endif
uint64_t crc64 (const CRCTable& table,
                const char* data,
                size_t data_len)
{
  return crc64_bytewise (table, data, data_len);
}


/**
 * @brief Find the CRC-64 of a string, with the default CRC.
 * @param data Pointer to the string to hash.
 * @param data_len Length of the string to hash, in bytes.
 */
uint64_t crc64 (const char* data,
                size_t data_len)
{
  return crc64 (defaultCRCTable, data, data_len);
}


/**
 * @brief Find the CRC-64 of a string, using the default polynomial.
 * @param str The string to hash.
 */
uint64_t crc64 (const std::string& s)
{
  return crc64 (defaultCRCTable, s.data(), s.size());
}


/**
 * @brief Extend a previously-calculated CRC to include an int.
 * @param crc The previously-calculated CRC.
 * @param x The integer to add.
 * @return The new CRC.
 */
uint64_t crc64addint (uint64_t crc, unsigned int x)
{
  while (x > 0) {
    crc = defaultCRCTable.m_table[(crc ^ x) & 0xff] ^ (crc >> 8);
    x >>= 8;
  }
  return crc;
}


/**
 * @brief Format a CRC-64 as a string.
 * @param crc The CRC to format.
 */
std::string crc64format (uint64_t crc)
{
  char buf[64];
  sprintf (buf, "%08X%08X",
           (unsigned)((crc>>32)&0xffffffff), (unsigned)(crc&0xffffffff));
  return buf;
}


/**
 * @brief Return a CRC-64 digest of a string.
 * @param str The string to hash.
 * @return The CRC-64 digest of the string.
 *         This is the hash code, expressed as hex as a string.
 */
std::string crc64digest (const std::string& str)
{
  return crc64format (crc64 (str));
}


} // namespace CxxUtils


