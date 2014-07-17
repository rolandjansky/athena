/***************************************************************************
                          QuadLinear.cxx  -  description
                             -------------------
    begin                : 31-05-2006 
    copyright            : (C) 2006 Alan Watson
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "TrigT1CaloUtils/QuadLinear.h"

namespace LVL1 {


unsigned int LVL1::QuadLinear::Compress(int Et) {
 
  // Compressed codes are unsigned. Calling function should keep track of sign   
  unsigned int code = abs(Et);
  unsigned int quadEt = m_mask + ((m_nRanges-1)<<m_Offset);
   
  for (int i = 0; i < m_nRanges; i++) {
    if ( code <= m_mask ) {
      quadEt = code + (i<<m_Offset);
      break;
    }
    code >>= m_nShift;
  }
  return quadEt;
}

unsigned int LVL1::QuadLinear::Expand(int Code) {

  // Compressed codes are unsigned. Calling function should keep track of sign   
  int range = Code >> m_Offset;
  int Et = Code&m_mask;
  
  return (Et<<= (range*m_nShift));
}

} // end of namespace bracket
