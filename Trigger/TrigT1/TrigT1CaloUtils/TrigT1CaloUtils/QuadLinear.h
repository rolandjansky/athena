/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          QuadLinear.h  -  description
                             -------------------
    begin                : 31-05-2006 
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

                                                                                
 #ifndef QUADLINEAR_H
 #define QUADLINEAR_H
                                                                                
#include <stdlib.h>

namespace LVL1 {

/**
QuadLinear encoding is used for transmission of ET/Ex/Ey sums from JEM to CMM.
This class compresses/uncompresses 8 bit ET (JEM energy sums) to/from quad linear scale
	\todo this should probably be a static class.
  */
class QuadLinear {

public: 
  /** Compress data */
  static unsigned int Compress(int Et);
  /** Uncompress data */
  static unsigned int Expand(int Code);
 
private: 
	/** Mask to select 6-bit field */
	static const unsigned int m_mask = 0x3F;
	/** Number of ET ranges to encode in */
	static const int m_nRanges = 4;
	/** Number of bits to shift by in each step */
	static const int m_nShift = 2;  
	/** Offset for compression code */
	static const int m_Offset = 6;
};

}//end of ns

#endif
