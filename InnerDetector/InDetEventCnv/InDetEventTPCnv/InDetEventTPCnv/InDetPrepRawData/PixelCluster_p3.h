/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTER_P3_TRK_H
#define PIXELCLUSTER_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   PixelCluster_p3.h
//
//-----------------------------------------------------------------------------
#include "SiWidth_p2.h"
#include "Identifier/Identifier.h"
namespace InDet
{
   class PixelCluster_p3
    {
   public:
   
      // type used for channel id differences
      typedef int rdo_diff_type;

      PixelCluster_p3() { m_splitInfo = 0; }

      // m_clustId - stores the pixel cluster id only for 32-bit ids,
      // otherwise the cluster id is stored in the m_rdoList
      unsigned int                          m_clusId; 
      std::vector<rdo_diff_type>            m_rdoList;
      float                                 m_localPosX;
      float                                 m_localPosY;
      float                                 m_mat00;
      float                                 m_mat01; 
      float                                 m_mat11;
      float                                 m_omegax;
      float                                 m_omegay;
      unsigned int                          m_properties;
      int                                   m_splitInfo;
      std::vector<float>                    m_chargeList;
      //SiWidth_p2                          
      InDet::SiWidth_p2                     m_width;
      
    };
}

#endif // PIXELCLUSTER_P3_TRK_H
