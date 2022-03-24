/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompoundLayerMaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "TrkGeometry/CompoundLayerMaterial.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/Material.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"


void CompoundLayerMaterialCnv_p1::persToTrans( const Trk::CompoundLayerMaterial_p1 *persObj,
                                             Trk::CompoundLayerMaterial    *transObj,
                                             MsgStream &mlog )
{

    // create the transient BinUtility
  transObj->updateBinning (new Trk::BinUtility);
    m_gBinUtilityCnv.persToTrans(&persObj->binUtility, transObj->m_binUtility, mlog);
    
    // the minimum values    
    transObj->m_thicknessBins.valueMin       = persObj->valueMins[0];
    transObj->m_x0Bins.valueMin              = persObj->valueMins[1];
    transObj->m_l0Bins.valueMin              = persObj->valueMins[2];
    transObj->m_aBins.valueMin               = persObj->valueMins[3];
    transObj->m_zBins.valueMin               = persObj->valueMins[4];
    transObj->m_rhoBins.valueMin             = persObj->valueMins[5];
                                             
    // the step values                       
    transObj->m_thicknessBins.valueStep      = persObj->valueSteps[0];
    transObj->m_x0Bins.valueStep             = persObj->valueSteps[1];
    transObj->m_l0Bins.valueStep             = persObj->valueSteps[2];
    transObj->m_aBins.valueStep              = persObj->valueSteps[3];
    transObj->m_zBins.valueStep              = persObj->valueSteps[4];
    transObj->m_rhoBins.valueStep            = persObj->valueSteps[5];
    
    // the matrix values    
    transObj->m_thicknessBins.valueBinMatrix = persObj->valueMatrices[0];
    transObj->m_x0Bins.valueBinMatrix        = persObj->valueMatrices[1];
    transObj->m_l0Bins.valueBinMatrix        = persObj->valueMatrices[2];
    transObj->m_aBins.valueBinMatrix         = persObj->valueMatrices[3];
    transObj->m_zBins.valueBinMatrix         = persObj->valueMatrices[4];
    transObj->m_rhoBins.valueBinMatrix       = persObj->valueMatrices[5];

    // composition matrix
    size_t ocBins = persObj->composition.size();
    if (ocBins){
        size_t icBins = persObj->composition[0].size();
        // reserve the right amount
        transObj->m_composition.reserve(ocBins);
        for (size_t ioc = 0; ioc < ocBins; ++ioc){
            transObj->m_composition.emplace_back( );
            transObj->m_composition[ioc].reserve(icBins);
            for (size_t iic = 0; iic < icBins; ++iic){
                const std::vector< std::pair<unsigned char, unsigned char> >& cEntry = persObj->composition[ioc][iic];
                transObj->m_composition[ioc].push_back( cEntry );
            }
        }                 
    }

    transObj->resizeMaterialProperties();
}

void CompoundLayerMaterialCnv_p1::transToPers( const Trk::CompoundLayerMaterial    *transObj,
                                             Trk::CompoundLayerMaterial_p1 *persObj,
                                             MsgStream  &mlog )
{
    
    // create the persistent BinUtility
    m_gBinUtilityCnv.transToPers(transObj->m_binUtility, &persObj->binUtility, mlog);
    
    // the minimum values 
    persObj->valueMins.push_back(transObj->m_thicknessBins.valueMin);
    persObj->valueMins.push_back(transObj->m_x0Bins.valueMin);
    persObj->valueMins.push_back(transObj->m_l0Bins.valueMin);
    persObj->valueMins.push_back(transObj->m_aBins.valueMin);
    persObj->valueMins.push_back(transObj->m_zBins.valueMin);         
    persObj->valueMins.push_back(transObj->m_rhoBins.valueMin);

    // the step values 
    persObj->valueSteps.push_back(transObj->m_thicknessBins.valueStep);
    persObj->valueSteps.push_back(transObj->m_x0Bins.valueStep);
    persObj->valueSteps.push_back(transObj->m_l0Bins.valueStep);
    persObj->valueSteps.push_back(transObj->m_aBins.valueStep);
    persObj->valueSteps.push_back(transObj->m_zBins.valueStep);         
    persObj->valueSteps.push_back(transObj->m_rhoBins.valueStep);
    
    // the matrix values 
    persObj->valueMatrices.push_back(transObj->m_thicknessBins.valueBinMatrix);
    persObj->valueMatrices.push_back(transObj->m_x0Bins.valueBinMatrix);
    persObj->valueMatrices.push_back(transObj->m_l0Bins.valueBinMatrix);
    persObj->valueMatrices.push_back(transObj->m_aBins.valueBinMatrix);
    persObj->valueMatrices.push_back(transObj->m_zBins.valueBinMatrix);         
    persObj->valueMatrices.push_back(transObj->m_rhoBins.valueBinMatrix);
    
    typedef std::vector< std::pair< unsigned char, unsigned char> > CVector;
    // composition matrix
    size_t ocBins = transObj->m_composition.size();
    if (ocBins){
        size_t icBins = transObj->m_composition[0].size();
        // reserve the right amoung
        persObj->composition = std::vector< std::vector < CVector > >(ocBins, std::vector < CVector >(icBins, CVector()) );
        for (size_t ioc = 0; ioc < ocBins; ++ioc)
            for (size_t iic=0; iic < icBins; ++iic){
                persObj->composition[ioc][iic] = CVector();
                persObj->composition[ioc][iic].reserve(transObj->m_composition[ioc][iic].size());
                for (auto& ic : transObj->m_composition[ioc][iic]){
                     persObj->composition[ioc][iic].push_back(ic);
                }
        
        }
    }
}
