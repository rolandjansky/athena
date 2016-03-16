/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTER_P2_TRK_H
#define PIXELCLUSTER_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   PixelCluster_p2.h
//
//-----------------------------------------------------------------------------
#include "SiWidth_p2.h"
#include "Identifier/Identifier.h"
namespace InDet
{
    class PixelCluster_p2 {
    public:
   
        typedef Identifier::diff_type rdo_diff_type;
	 
        PixelCluster_p2() {}

        // replace this:
        Identifier::value_type                    m_clusId; 
        std::vector<rdo_diff_type>                m_rdoList;
        float                                     m_localPosX;
        float                                     m_localPosY;
        float 					m_mat00;
        float 					m_mat01;
        float 					m_mat11;
        float                                     m_omX;
        float                                     m_omY;

        //SiWidth_p2
        InDet::SiWidth_p2			m_width;
      
    };
}

#endif // PIXELCLUSTER_P2_TRK_H
