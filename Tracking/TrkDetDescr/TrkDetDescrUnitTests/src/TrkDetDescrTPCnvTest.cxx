/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkDetDescrTPCnvTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDetDescrUnitTests/TrkDetDescrTPCnvTest.h"
// general Trk & TrkGeometry includes
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkGeometry/CompoundLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"
#include <climits>

Trk::TrkDetDescrTPCnvTest::TrkDetDescrTPCnvTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
 m_writeMode(true),
 m_materialStepCollectionName("RandomMaterialSteps"),
 m_layerMaterialCollectionName("RandomLayerMaterialMap"),
 m_elementTableName("RandomElementTable")
{
    
    declareProperty("WriteMode",                    m_writeMode);
    // collection names
    declareProperty("MaterialStepCollection",       m_materialStepCollectionName);
    declareProperty("LayerMaterialMap",             m_layerMaterialCollectionName);
    declareProperty("ElementTable",                 m_elementTableName);
}

StatusCode Trk::TrkDetDescrTPCnvTest::runTest()
{
    ATH_MSG_VERBOSE("Running the TrkDetDescrTPCnvTest Test");
    
    // ----------------- WRITE MODE ----------------- 
    if (m_writeMode){  
        
        ATH_MSG_VERBOSE("Test is running in write mode - writing out a file.");
        
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
                
        if (!m_materialStepCollectionName.empty() && (evtStore()->record(msCollection, m_materialStepCollectionName)).isFailure() ) {
            ATH_MSG_WARNING("Could not record the material collection " << m_materialStepCollectionName);
            return StatusCode::FAILURE;
        } 
        
        ATH_MSG_VERBOSE("Recording LayerMaterialMap '" << m_layerMaterialCollectionName << "'.");        
        
        // The LayerMaterialMap
        std::unique_ptr<Trk::LayerMaterialMap> lmMap(new Trk::LayerMaterialMap);
        
        // (e) element Table
        Trk::ElementTable* eTable = new Trk::ElementTable();
        eTable->addElement(Trk::Material(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                         TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                         TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                         1,
                                         TrkDetDescrUnitTestBase::m_flatDist->shoot()),"TestMaterial1");
         eTable->addElement(Trk::Material(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          2,
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot()),"TestMaterial2");
         eTable->addElement(Trk::Material(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                          3,
                                          TrkDetDescrUnitTestBase::m_flatDist->shoot()),"TestMaterial3");
        
        if ( !m_elementTableName.empty() &&  (evtStore()->record(eTable, m_elementTableName)).isFailure() ) {
             ATH_MSG_WARNING("Could not record the element table " << m_elementTableName);
             return StatusCode::FAILURE;
        }  
		
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
			    std::vector< std::vector< const Trk::MaterialProperties* > > materialMatrix(300, std::vector<const Trk::MaterialProperties*>(200, (Trk::MaterialProperties*)nullptr) );
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
	    Trk::BinUtility zUtility(200,-100,100, Trk::open, Trk::binZ);
		
		std::vector< const Trk::MaterialProperties*> materialVector; 
		std::vector< unsigned short int >            materialIndices;
		materialVector.reserve(200);
		materialIndices.reserve(200);
        for (size_t iib = 0; iib < 200; ++iib){
			 materialIndices.push_back(int(TrkDetDescrUnitTestBase::m_flatDist->shoot()*200));
             materialVector.push_back(new Trk::MaterialProperties(TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                  TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                  TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                  TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                  TrkDetDescrUnitTestBase::m_flatDist->shoot(),
                                                                  TrkDetDescrUnitTestBase::m_flatDist->shoot()));
																   
		}														   
        (*lmMap)[Trk::LayerIndex(3)] = new Trk::CompressedLayerMaterial(zUtility, materialVector, materialIndices);
		
		// (d) compound material
	    Trk::BinUtility zUtilityC(200,-100,100, Trk::open, Trk::binZ);
	    
		
		Trk::ValueMatrix thicknessMatrix;
		Trk::ValueMatrix x0Matrix;
		Trk::ValueMatrix l0Matrix;
		Trk::ValueMatrix aMatrix;
		Trk::ValueMatrix zMatrix;
		Trk::ValueMatrix rhoMatrix;
		thicknessMatrix.reserve(25);
		x0Matrix.reserve(25);
		l0Matrix.reserve(25);
		aMatrix.reserve(25);
		zMatrix.reserve(25);
		rhoMatrix.reserve(25);

		std::vector< std::vector< Trk::MaterialComposition > > materialCompositionMatrix;
		
		for (size_t iob = 0; iob < 200; ++iob){
			// the vectors
			std::vector<unsigned char> thicknessVector; thicknessVector.reserve(25);
			std::vector<unsigned char> x0Vector; x0Vector.reserve(25);
			std::vector<unsigned char> l0Vector; l0Vector.reserve(25);
			std::vector<unsigned char> aVector; aVector.reserve(25);
			std::vector<unsigned char> zVector; zVector.reserve(25);
			std::vector<unsigned char> rhoVector; rhoVector.reserve(25);
			std::vector< Trk::MaterialComposition > materialCompositionVector;
			materialCompositionVector.reserve(25);
			for (size_t iib = 0; iib < 25; ++iib){
				x0Vector.push_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				l0Vector.push_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				aVector.push_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				zVector.push_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				rhoVector.push_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				std::vector< Trk::ElementFraction > elements;
				for (size_t iic = 0; iic < 5; ++iic){
					elements.emplace_back((unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX),
															(unsigned char)(TrkDetDescrUnitTestBase::m_flatDist->shoot()*UCHAR_MAX));
				}
				materialCompositionVector.emplace_back(elements);
			}
			thicknessMatrix.push_back(thicknessVector);
			x0Matrix.push_back(x0Vector);
			l0Matrix.push_back(l0Vector);
			aMatrix.push_back(aVector);
			zMatrix.push_back(zVector);
			rhoMatrix.push_back(rhoVector);
			materialCompositionMatrix.push_back(materialCompositionVector);
		}
		
		Trk::ValueStore thicknessBins;
		thicknessBins.valueMin  = 0.;
		thicknessBins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		thicknessBins.valueBinMatrix = thicknessMatrix;
					
		Trk::ValueStore x0Bins;
		x0Bins.valueMin  = 0.;
		x0Bins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		x0Bins.valueBinMatrix = x0Matrix;

		
		Trk::ValueStore l0Bins;
		l0Bins.valueMin  = 0.;
		l0Bins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		l0Bins.valueBinMatrix = l0Matrix;
		
		Trk::ValueStore aBins;
		aBins.valueMin  = 0.;
		aBins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		aBins.valueBinMatrix = aMatrix;
		
		Trk::ValueStore zBins;
		zBins.valueMin  = 0.;
		zBins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		zBins.valueBinMatrix = zMatrix;
		
		Trk::ValueStore rhoBins;
		rhoBins.valueMin  = 0.;
		rhoBins.valueStep = TrkDetDescrUnitTestBase::m_flatDist->shoot();
		rhoBins.valueBinMatrix = rhoMatrix;
		
        (*lmMap)[Trk::LayerIndex(3)] = new Trk::CompoundLayerMaterial(zUtilityC,
                              										  thicknessBins,
                              										  x0Bins,
                              										  l0Bins,
                              										  aBins,
                              										  zBins,
                              										  rhoBins,
                              										  materialCompositionMatrix);
		
        Trk::SharedObject<const Trk::ElementTable> sharedElementTable(new Trk::ElementTable(*eTable));
        lmMap->updateElementTable(sharedElementTable);
        
        if ( (evtStore()->record(lmMap.release(), m_layerMaterialCollectionName)).isFailure() ) {
             ATH_MSG_WARNING("Could not record the material collection " << m_layerMaterialCollectionName);
             return StatusCode::FAILURE;
        } 
    } else {
        //
        ATH_MSG_VERBOSE("Test is running in READ mode, reading in a file.");
        
        // ----------------- READ MODE  -----------------
        const Trk::LayerMaterialMap* lmMap = nullptr;
        if ( (evtStore()->retrieve(lmMap, m_layerMaterialCollectionName)).isFailure() ) {
             ATH_MSG_WARNING("Could not read the material collection " << m_layerMaterialCollectionName);
             return StatusCode::FAILURE;
        } else 
            ATH_MSG_INFO("Successfully read a LayerMaterialMap with size " << lmMap->size());
        
    }
                                                                                            
    return StatusCode::SUCCESS;
}
