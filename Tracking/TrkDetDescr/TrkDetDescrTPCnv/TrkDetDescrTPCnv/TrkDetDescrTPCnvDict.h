/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKDETDESCRTPCNVDICT_H
#define TRKDETDESCRTPCNVDICT_H


//-----------------------------------------------------------------------------
// Top level collections
//-----------------------------------------------------------------------------
#include "TrkDetDescrTPCnv/LayerMaterialMap_tlp1.h"

//-----------------------------------------------------------------------------
// TrkGeometry
//-----------------------------------------------------------------------------
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCollection_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStep_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTable_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/Material_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialProperties_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMap_p1.h"

//-----------------------------------------------------------------------------
// TrkDetDescrUtils
//-----------------------------------------------------------------------------
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"


struct TrackDetDescrTPCnvDict
{
 
    std::vector< unsigned char >                                                            m_v_uchar;
    std::pair< unsigned char, unsigned char >                                               m_p_uchar;
    std::vector< std::vector < unsigned char > >                                            m_vv_uchar;
    std::vector< std::vector < std::vector < unsigned char > > >                            m_vvv_uchar;
    std::vector< std::pair< unsigned char, unsigned char > >                                m_vp_uchar;
    std::vector < std::vector< std::pair< unsigned char, unsigned char > > >                m_vvp_uchar;
    std::vector< std::vector < std::vector< std::pair< unsigned char, unsigned char > > > > m_vvvp_uchar;
    
    Trk::MaterialStepCollection_p1                                                          m_msc_p1;
    Trk::MaterialStep_p1                                                                    m_ms_p1;
    Trk::MaterialProperties_p1                                                              m_mp_p1;
    Trk::Material_p1                                                                        m_mat_p1;
    Trk::ElementTable_p1                                                                    m_et_p1;
    std::vector< Trk::ElementTable_p1 >                                                     m_v_et_p1;
    std::vector< Trk::MaterialStepCollection_p1 >                                           m_v_msc_p1;
    std::vector< Trk::MaterialStep_p1 >                                                     m_v_ms_p1;
    std::vector< Trk::MaterialProperties_p1 >                                               m_v_mp_p1;
                                                                                                                                                                                      
    Trk::LayerMaterialMap_p1                                                                m_lmm_p1;
    Trk::LayerMaterialMap_tlp1                                                              m_lmm_tlp1;     
    std::vector< Trk::LayerMaterialMap_p1 >                                                 m_v_lmm_p1;
    std::vector< Trk::LayerMaterialMap_tlp1 >                                               m_v_lmm_tlp1;
                                                                                            
    Trk::BinnedLayerMaterial_p1                                                             m_blm_p1;
    Trk::CompressedLayerMaterial_p1                                                         m_klm_p1;
    Trk::CompoundLayerMaterial_p1                                                           m_clm_p1;
    Trk::HomogeneousLayerMaterial_p1                                                        m_hlm_p1;
    std::vector< Trk::BinnedLayerMaterial_p1 >                                              m_v_blm_p1;
    std::vector< Trk::CompressedLayerMaterial_p1 >                                          m_v_clm_p1;
    std::vector< Trk::CompoundLayerMaterial_p1 >                                            m_v_klm_p1;    
    std::vector< Trk::HomogeneousLayerMaterial_p1 >                                         m_v_hlm_p1;
                                                                                            
    Trk::BinUtility_p1                                                                      m_pbu_p1;    
    std::vector< Trk::BinUtility_p1 >                                                       m_v_pbu_p1;
    std::vector<std::vector<std::pair<int,float> > >                                        m_vvp_if;    
   
};

#endif // TRKDETDESCRTPCNVDICT_H
