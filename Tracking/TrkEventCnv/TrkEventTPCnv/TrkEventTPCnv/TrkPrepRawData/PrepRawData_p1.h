/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PREP_RAW_DATA_P1_TRK_H
#define PREP_RAW_DATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   PrepRawData_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class PrepRawData_p1
   {
  public:
     PrepRawData_p1() : m_clusId(0), m_indexAndHash(0){}

     Identifier::value_type                 m_clusId; 
     std::vector< Identifier::value_type >  m_rdoList;
     
     // Trk::LocalPosition_p1*
     TPObjRef                               m_localPos;
     // Trk::ErrorMatrix_p1*
     TPObjRef                               m_localErrMat;
     
     unsigned int                           m_indexAndHash;
   };
}

#endif // PREP_RAW_DATA_P1_TRK_H
