/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialAnalyser.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkDetDescrTestTools/LayerMaterialAnalyser.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/LayerMaterialRecord.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/CompoundLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
// Event & GeoPrimitives
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"     
// ROOT
#include "TTree.h"


// constructor
Trk::LayerMaterialAnalyser::LayerMaterialAnalyser(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_layerMaterialName("UnspecifiedLayerMaterialMap"),
  m_validationTree(nullptr),
  m_validationTreeName("LayerMaterialAnalyser"),
  m_validationTreeDescription("LayerMaterialAnalyser information"),
  m_validationTreeFolder("/val/LayerMaterialAnalyser"),
  m_layerIndex(0),        
  m_layerType(0),
  m_layerTranslation(nullptr),
  m_layerRotation(nullptr),
  m_layerDimension0(0.),   
  m_layerDimension1(0.),   
  m_layerBins(0),         
  m_layerBins0(0),         
  m_layerBins1(0),
  m_bin0(nullptr),
  m_bin1(nullptr),
  m_thickness(nullptr),
  m_X0(nullptr),
  m_L0(nullptr),
  m_A(nullptr),
  m_Z(nullptr),
  m_Rho(nullptr),
  m_elements(nullptr),
  m_binCounter(nullptr)
{
    declareInterface<Trk::ILayerMaterialAnalyser>(this);
    // give the map a name
    declareProperty("LayerMaterialName",          m_layerMaterialName);
    declareProperty("ValidationTreeName",         m_validationTreeName);
    declareProperty("ValidationTreeDescription",  m_validationTreeDescription);
    declareProperty("ValidationTreeFolder",       m_validationTreeFolder);
}

// destructor
Trk::LayerMaterialAnalyser::~LayerMaterialAnalyser()
{}


// initialize
StatusCode Trk::LayerMaterialAnalyser::initialize() 
{
  
    m_layerTranslation = new std::vector<float>(3, 0.);
    m_layerRotation    = new std::vector<float>(9, 0.);
    m_bin0             = new std::vector<int>(LAYERMAXBINS, 0);
    m_bin1             = new std::vector<int>(LAYERMAXBINS, 0);
    m_thickness        = new std::vector<float>(LAYERMAXBINS, 0.);
    m_X0               = new std::vector<float>(LAYERMAXBINS, 0.);
    m_L0               = new std::vector<float>(LAYERMAXBINS, 0.);
    m_A                = new std::vector<float>(LAYERMAXBINS, 0.);
    m_Z                = new std::vector<float>(LAYERMAXBINS, 0.);
    m_Rho              = new std::vector<float>(LAYERMAXBINS, 0.);
    m_elements         = new std::vector<int>(LAYERMAXBINS, 0);
    m_binCounter       = new std::vector<int>(LAYERMAXBINS, 0);
    
    // now register the Tree
    ITHistSvc* tHistSvc = nullptr;
    
    // ------------- validation section ------------------------------------------
    m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

    // position coordinates of the update
    m_validationTree->Branch("LayerIndex",            &m_layerIndex         );
    m_validationTree->Branch("LayerType",             &m_layerType          );
    m_validationTree->Branch("LayerTranslation",      &m_layerTranslation   );
    m_validationTree->Branch("LayerRotation",         &m_layerRotation      );
    m_validationTree->Branch("LayerDimension0",       &m_layerDimension0    );
    m_validationTree->Branch("LayerDimension1",       &m_layerDimension1    );
    m_validationTree->Branch("LayerBins",             &m_layerBins          );
    m_validationTree->Branch("LayerBins0",            &m_layerBins0         );
    m_validationTree->Branch("LayerBins1",            &m_layerBins1         );
    m_validationTree->Branch("LayerBin0",             &m_bin0               );
    m_validationTree->Branch("LayerBin1",             &m_bin1               );
    m_validationTree->Branch("LayerBinCounter",       &m_binCounter         );
    m_validationTree->Branch("LayerThickness",        &m_thickness          );
    m_validationTree->Branch("LayerX0",               &m_X0                 );
    m_validationTree->Branch("LayerL0",               &m_L0                 );
    m_validationTree->Branch("LayerA",                &m_A                  );
    m_validationTree->Branch("LayerZ",                &m_Z                  );
    m_validationTree->Branch("LayerRo",               &m_Rho                );
    m_validationTree->Branch("LayerElements",         &m_elements           );
    
    // now register the Tree
    if (service("THistSvc",tHistSvc).isFailure()) {
        ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !" );
        delete m_validationTree; m_validationTree = nullptr;
        return StatusCode::SUCCESS;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder.c_str(), m_validationTree)).isFailure()) {
        ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
        delete m_validationTree; m_validationTree = nullptr;
        return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
}
    
// finalize
StatusCode Trk::LayerMaterialAnalyser::finalize() 
{
    delete m_layerTranslation ;
    delete m_layerRotation    ;
    delete m_bin0             ;
    delete m_bin1             ;
    delete m_thickness        ;
    delete m_X0               ;
    delete m_L0               ;
    delete m_A                ;
    delete m_Z                ;
    delete m_Rho              ;
    delete m_elements         ;
    delete m_binCounter       ;
    return StatusCode::SUCCESS;
}



StatusCode Trk::LayerMaterialAnalyser::analyseLayerMaterial(const Trk::Layer& layer) const
{
    
    // get the LayerMaterial 
    const Trk::LayerMaterialProperties* lMaterial = layer.layerMaterialProperties();
    if (!lMaterial) return StatusCode::SUCCESS;

    return analyseLayerMaterial(layer, *lMaterial);
}


StatusCode Trk::LayerMaterialAnalyser::analyseLayerMaterial(const Trk::Layer& layer, const Trk::LayerMaterialProperties& lMaterial) const
{
    
    // binned material can directly call the filling method
    const Trk::BinnedLayerMaterial* blMaterial = dynamic_cast<const Trk::BinnedLayerMaterial*>(&lMaterial);
    if (blMaterial) {
        ATH_MSG_DEBUG( "Recieved BinnedLayerMaterial - analyzing it." );
        return analyseLayerMaterial(layer, blMaterial->fullMaterial());
    }
    
    // we need to create a MaterialProperties matrix for this
    const Trk::BinUtility* bUtility = lMaterial.binUtility();
    size_t mBins0 = bUtility ? bUtility->max(0)+1 : 1;
    size_t mBins1 = bUtility ? bUtility->max(1)+1 : 1;
    
    Trk::MaterialPropertiesMatrix mpMatrix(mBins1, std::vector< const Trk::MaterialProperties*>(mBins0, nullptr));    
    for (size_t ibin1 = 0; ibin1 < mBins1; ++ ibin1){
        for (size_t ibin0 = 0; ibin0 < mBins0; ++ibin0)
            mpMatrix[ibin1][ibin0] = lMaterial.material(ibin0, ibin1);
    }    
    // now send it to the analyser
    return analyse(layer, mpMatrix);
}


StatusCode Trk::LayerMaterialAnalyser::analyseLayerMaterial(const Trk::Layer& layer, const Trk::MaterialPropertiesMatrix& mpMatrix) const
{
    ATH_MSG_DEBUG( "Recieved MaterialPropertyMatrix  - analyzing it." );
    return analyse(layer, mpMatrix);
}


StatusCode Trk::LayerMaterialAnalyser::analyseLayerMaterial(const Trk::Layer& layer, const Trk::LayerMaterialRecord& lmRecord) const
{
    ATH_MSG_DEBUG( "Recieved LayerMaterialRecord  - analyzing it." );
    return analyse(layer, lmRecord.associatedLayerMaterial(), (const std::vector< std::vector< unsigned int > >*)&lmRecord.binCounts());
}

StatusCode Trk::LayerMaterialAnalyser::analyse(const Trk::Layer& layer,
                                               const Trk::MaterialPropertiesMatrix& mpMatrix,
                                               const std::vector< std::vector< unsigned int > >* bCounter ) const
{
  
    // general layer information
    m_layerIndex = layer.layerIndex().value();
    const Trk::Surface& lSurface = layer.surfaceRepresentation();
    m_layerTranslation->at(0) = lSurface.center().x();
    m_layerTranslation->at(1) = lSurface.center().y();
    m_layerTranslation->at(2) = lSurface.center().z();
    
    AmgMatrix(3,3) rMatrix = lSurface.transform().rotation();
    m_layerRotation->at(0) = rMatrix(0,0);
    m_layerRotation->at(1) = rMatrix(1,0);
    m_layerRotation->at(2) = rMatrix(2,0);
    m_layerRotation->at(3) = rMatrix(0,1);
    m_layerRotation->at(4) = rMatrix(1,1);
    m_layerRotation->at(5) = rMatrix(2,1);
    m_layerRotation->at(6) = rMatrix(0,2);
    m_layerRotation->at(7) = rMatrix(1,2);
    m_layerRotation->at(8) = rMatrix(2,2);

    // cylinder bounds 
    if ( lSurface.type() == Trk::SurfaceType::Cylinder ){
        m_layerType = 1;
        // cylinder bounds
        const Trk::CylinderBounds* cb = dynamic_cast<const Trk::CylinderBounds*>(&(lSurface.bounds()));
        if (cb){
            m_layerDimension0 = cb->r();
            m_layerDimension1 = cb->halflengthZ();
        }
    } else if ( lSurface.type() == Trk::SurfaceType::Disc ) {
        m_layerType = 2;
        // disc bounds
        const Trk::DiscBounds* db = dynamic_cast<const Trk::DiscBounds*>(&(lSurface.bounds()));
        if (db){
            m_layerDimension0 = db->rMin();
            m_layerDimension1 = db->rMax();
        }
    }

    // now get the material matrix and record all single bins;
    m_layerBins0  = 0;
    m_layerBins1  = 0;
    m_layerBins   = 0; 
    int bin1      = 0;
    for (const auto & outerIter : mpMatrix){
        int bin0 = 0;
        for (const auto & innerIter : outerIter ){
            m_bin0->at(m_layerBins) = bin0;
            m_bin1->at(m_layerBins) = bin1;
            // get the material
            const Trk::MaterialProperties* mProperties = innerIter;
            if (mProperties){
                m_thickness->at(m_layerBins) = mProperties->thickness();
                m_X0->at(m_layerBins)        = mProperties->x0();
                m_L0->at(m_layerBins)        = mProperties->l0();
                m_A->at(m_layerBins)         = mProperties->averageA();
                m_Z->at(m_layerBins)         = mProperties->averageZ();
                m_Rho->at(m_layerBins)       = mProperties->averageRho();
                m_elements->at(m_layerBins)  = mProperties->material().composition ? mProperties->material().composition->size() : 0;    
            } else  {
                m_thickness->at(m_layerBins) = 0.;
                m_X0->at(m_layerBins)        = 0.;
                m_L0->at(m_layerBins)        = 0.;
                m_A->at(m_layerBins)         = 0.;
                m_Z->at(m_layerBins)         = 0.;
                m_Rho->at(m_layerBins)       = 0.;
                m_elements->at(m_layerBins)  = 0.;                
            }
            // set the bin Counter 
            m_binCounter->at(m_layerBins)    = bCounter ? (*bCounter)[bin1][bin0] : 1;
            //     
            ++bin0;
            if (!bin1) ++m_layerBins0;
            ++m_layerBins;    
        }
        ++bin1;
        ++m_layerBins1;
    }
    m_validationTree->Fill();
    
    // return 0 - since this is only an analyser
    return StatusCode::SUCCESS;
}    
    
