/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkDetDescrTPCnvTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDetDescrUnitTests/TrkDetDescrTPCnvTest.h"
// general Trk & TrkGeometry includes
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"

Trk::TrkDetDescrTPCnvTest::TrkDetDescrTPCnvTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
 m_writeMode(true),
 m_materialStepCollectionName("RandomMaterialSteps"),
 m_layerMaterialCollectionName("RandomLayerMaterialMap")
{
    
    declareProperty("WriteMode",                    m_writeMode);
    // collection names
    declareProperty("MaterialStepCollection",       m_materialStepCollectionName);
    declareProperty("LayerMaterialMap",             m_layerMaterialCollectionName);
}

StatusCode Trk::TrkDetDescrTPCnvTest::runTest()
{
    ATH_MSG_VERBOSE("Running the TrkDetDescrTPCnvTest Test");
     
    if (m_writeMode){    
        // The Material STEP collection
        Trk::MaterialStepCollection* msCollection = new Trk::MaterialStepCollection;
        for (size_t it = 0; it < TrkDetDescrUnitTestBase::m_numTests; ++it ){
            msCollection->push_back(new Trk::MaterialStep(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),  
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),      
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),      
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                          TrkDetDescrUnitTestBase::m_flatDist->shoot()));
        }    
        
        ATH_MSG_VERBOSE("Recording MaterialStepCollection '" << m_materialStepCollectionName << "'.");
                
        if ( (evtStore()->record(msCollection, m_materialStepCollectionName)).isFailure() ) {
            ATH_MSG_WARNING("Could not record the material collection " << m_materialStepCollectionName);
            return StatusCode::FAILURE;
        } 
        
        ATH_MSG_VERBOSE("Recording LayerMaterialMap '" << m_layerMaterialCollectionName << "'.");        
        
        // The LayerMaterialMap
        Trk::LayerMaterialMap* lmMap = new Trk::LayerMaterialMap;
        
        // (a) homogeneous material
        Trk::HomogeneousLayerMaterial* hml = new Trk::HomogeneousLayerMaterial(Trk::MaterialProperties(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot()),
                                                                                                       TrkDetDescrUnitTestBase::m_flatDist->shoot());
        (*lmMap)[Trk::LayerIndex(1)] = hml;

        // (b) binned material
	    Trk::BinUtility rPhiZUtility(200,-M_PI, M_PI, Trk::closed, Trk::binRPhi);
                            rPhiZUtility += Trk::BinUtility(300,-250.,250., Trk::open, Trk::binZ);
			    std::vector< std::vector< const Trk::MaterialProperties* > > materialMatrix(300, std::vector<const Trk::MaterialProperties*>(200, (Trk::MaterialProperties*)0) );
	    for (size_t iob = 0; iob < 300; ++iob)
                for (size_t iib = 0; iib < 200; ++iib)
                    materialMatrix[iob][iib] = new Trk::MaterialProperties(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                           TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                           TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                           TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                           TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                           TrkDetDescrUnitTestBase::m_flatDist->shoot());
	    (*lmMap)[Trk::LayerIndex(2)] =  new Trk::BinnedLayerMaterial(rPhiZUtility, materialMatrix, TrkDetDescrUnitTestBase::m_flatDist->shoot()) ;
        
        
            // (c) compressed material 
        
        
        if ( (evtStore()->record(lmMap, m_layerMaterialCollectionName)).isFailure() ) {
             ATH_MSG_WARNING("Could not record the material collection " << m_layerMaterialCollectionName);
             return StatusCode::FAILURE;
        } 
    }

                                                                                               
                                                                                            
    return StatusCode::SUCCESS;
}
