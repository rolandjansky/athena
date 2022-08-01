#include "infraL1Calo/GenericCrc.h"

#include <iostream>

uint32_t GenericCrc::crc9fibre(const std::vector<uint32_t>& inwords, size_t num_bits ) const
{
    /**
     *  \brief CRC9 with polynomial 1011111011 over num_bits bits
     *
     *  Uses a more succinct CRC calculation and flexible in terms of digits, checked versus old code
     *  but only supports bit reversal = true
     *
     */

    size_t num_words = inwords.size();
    if ( (num_bits+31)/32 > num_words )
    {
      std::cout << "ERROR: not enough words (" << num_words << ") for " << num_bits << "-bit CRC calculation." << std::endl;
      return 0;
    }
    uint32_t val = 0x1ff;
    for ( size_t i = 0 ; i < num_bits ; ++i )
    {
      if ( (inwords.at(i/32)>>(i%32)) & 1 )
        val ^= 1;
      if ( val&1 )
        val ^= 0x37d;     // 1101111101 = polynomial reversed
      val >>= 1;
    }
    return val;
} 

uint32_t GenericCrc::crc20rod(const std::vector<uint32_t>& inwords, size_t num_bits ) const
{
  int order = 20;
  uint32_t val  = 0xfffff;
  //<<uint32_t poly = 0x18349f;   // Old, wrong, polynomial used until December 2021
  uint32_t poly = 0x18359f;
  size_t num_words = inwords.size();
  if ( (num_bits+31)/32 > num_words )
  {
    std::cout << "ERROR: not enough words (" << num_words << ") for " << num_bits << "-bit CRC calculation." << std::endl;
    return 0;
  }
  for ( int i = 0 ; i < order ; ++i )
  {
    int flip = val&1;
    val >>= 1;
    if ( flip )
      val ^= (poly>>1);
  }
  for ( size_t i = 0 ; i < num_bits ; ++i )
  {
    val <<= 1;
    val |= (inwords[i/32]>>(31-i%32))&1;
    if ((val>>order)&1)
      val ^= poly;
  }
  for ( int i = 0 ; i < order ; ++i )
  {
    val <<= 1;
    if ((val>>order)&1)
      val ^= poly;
  }
  return val;
}

