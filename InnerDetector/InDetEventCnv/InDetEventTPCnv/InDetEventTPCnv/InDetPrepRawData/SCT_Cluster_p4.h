/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_P4_TRK_H
#define SCT_CLUSTER_P4_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SCT_Cluster_p4.h
//
//-----------------------------------------------------------------------------
#include "SiWidth_p2.h"

namespace InDet
{
   class SCT_Cluster_p4
    {
   public:
   
 /* #ifndef __IDENTIFIER_64BIT__ */
 /*    typedef  short rdo_diff_type; */
 /* #else */
 /*    typedef Identifier::diff_type rdo_diff_type; */
 /* #endif  */

      // type used for channel id differences
      typedef  short rdo_diff_type;

      SCT_Cluster_p4() {}

      // replace this:
      std::vector<rdo_diff_type >	        m_rdoList;
      float                                     m_localPosX;
      float                                     m_localPosY;
      float 					m_mat00;
      float 					m_mat01;
      float 					m_mat11;
      uint16_t                                  m_hitsInThirdTimeBin;

      //SiWidth_p2
      InDet::SiWidth_p2			m_width;

    };
}

#endif // SCT_CLUSTER_P4_TRK_H
