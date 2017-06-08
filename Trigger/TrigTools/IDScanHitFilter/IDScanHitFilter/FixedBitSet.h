/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: FixedBitSet.h
// 
// author:           Nikos Konstantinidis
//                   nk@hep.ucl.ac.uk
//
// athena migration: Malte Muller
//                   mm@hep.ucl.ac.uk
//		 
// Description: defines the FixedBitSet class, used to store layer-information
// 
// date: 29/10/2002
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_FIXEDBITSET_H )
#define IDSCAN_FIXEDBITSET_H


  // Create the masks for each bit.
  static const int setMask[] = { 
    ((int) 1<< 0), ((int) 1<< 1), ((int) 1<< 2), ((int) 1<< 3), 
    ((int) 1<< 4), ((int) 1<< 5), ((int) 1<< 6), ((int) 1<< 7), 
    ((int) 1<< 8), ((int) 1<< 9), ((int) 1<<10), ((int) 1<<11), 
    ((int) 1<<12), ((int) 1<<13), ((int) 1<<14), ((int) 1<<15), 
    ((int) 1<<16), ((int) 1<<17), ((int) 1<<18), ((int) 1<<19), 
    ((int) 1<<20), ((int) 1<<21), ((int) 1<<22), ((int) 1<<23), 
    ((int) 1<<24), ((int) 1<<25), ((int) 1<<26), ((int) 1<<27), 
    ((int) 1<<28), ((int) 1<<29), ((int) 1<<30), ((int) 1<<31)};
  
  static const int clearMask[] = { 
    ~((int) 1<< 0),~((int) 1<< 1),~((int) 1<< 2),~((int) 1<< 3), 
    ~((int) 1<< 4),~((int) 1<< 5),~((int) 1<< 6),~((int) 1<< 7), 
    ~((int) 1<< 8),~((int) 1<< 9),~((int) 1<<10),~((int) 1<<11), 
    ~((int) 1<<12),~((int) 1<<13),~((int) 1<<14),~((int) 1<<15), 
    ~((int) 1<<16),~((int) 1<<17),~((int) 1<<18),~((int) 1<<19), 
    ~((int) 1<<20),~((int) 1<<21),~((int) 1<<22),~((int) 1<<23), 
    ~((int) 1<<24),~((int) 1<<25),~((int) 1<<26),~((int) 1<<27), 
    ~((int) 1<<28),~((int) 1<<29),~((int) 1<<30),~((int) 1<<31)};

  static const int countLUT0[] = 
  {0,2,1,3,1,3,2,4,1,3,2,4,2,4,3,5,1,3,2,4,2,4,3,5,2,4,3,5,3,5,4,6,
   1,3,2,4,2,4,3,5,2,4,3,5,3,5,4,6,2,4,3,5,3,5,4,6,3,5,4,6,4,6,5,7,
   2,4,3,5,3,5,4,6,3,5,4,6,4,6,5,7,3,5,4,6,4,6,5,7,4,6,5,7,5,7,6,8,
   3,5,4,6,4,6,5,7,4,6,5,7,5,7,6,8,4,6,5,7,5,7,6,8,5,7,6,8,6,8,7,9,
   1,3,2,4,2,4,3,5,2,4,3,5,3,5,4,6,2,4,3,5,3,5,4,6,3,5,4,6,4,6,5,7,
   2,4,3,5,3,5,4,6,3,5,4,6,4,6,5,7,3,5,4,6,4,6,5,7,4,6,5,7,5,7,6,8,
   3,5,4,6,4,6,5,7,4,6,5,7,5,7,6,8,4,6,5,7,5,7,6,8,5,7,6,8,6,8,7,9,
   4,6,5,7,5,7,6,8,5,7,6,8,6,8,7,9,5,7,6,8,6,8,7,9,6,8,7,9,7,9,8,10};

  static const int countLUT1[] = 
  {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

  
class FixedBitSet {

	public:

		FixedBitSet()                       { m_bits = s_clearAllMask; };
		FixedBitSet(const FixedBitSet& set) { m_bits = set.m_bits; };

		bool get(const int bitIndex) const   { return ((m_bits&setMask[bitIndex]) != 0); };
		int length() const                   { return 32; };
		int size() const                     { return 32; };
		int count() const
		{
			return ( countLUT1[m_bits & s_maskByte]+
			         countLUT1[(m_bits>>8) & s_maskByte]+
			         countLUT1[(m_bits>>16) & s_maskByte]+
			         countLUT1[(m_bits>>24) & s_maskByte] );
		};

		void set(const int bitIndex)     { m_bits |= setMask[bitIndex]; };
		void setAll()                    { m_bits = s_setAllMask; };
		void clear(const int bitIndex)   { m_bits &= clearMask[bitIndex]; };
		void clearAll()                  { m_bits = s_clearAllMask; };

		void And  (FixedBitSet& set)   { m_bits &= set.m_bits; };
		void NAnd (FixedBitSet& set)   { m_bits &= ~set.m_bits; };
		void Or   (FixedBitSet& set)   { m_bits |= set.m_bits; };
		void XOr  (FixedBitSet& set)   { m_bits ^= set.m_bits; };

		bool operator==(const FixedBitSet& set) const   { return (m_bits == set.m_bits); };

	private:

		static const int s_clearAllMask = 0;
		static const int s_setAllMask = ~((int) 0);
		static const int s_maskByte = 255;

		// The m_bits used to store this FixedBitSet.  
		int m_bits;

};


#endif
