/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialInspector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//// Trk include
#include "TrkDetDescrTestTools/LayerMaterialInspector.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkDetDescrUtils/BinUtility.h"
// Root
#include "TH2F.h"
#include "TTree.h"
#include "TString.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// constructor
Trk::LayerMaterialInspector::LayerMaterialInspector(const std::string& t, const std::string& n, const IInterface* p) : 
  Trk::RecursiveGeometryProcessor(t,n,p),
  m_treeFolder("/val/")
{}

// destructor
Trk::LayerMaterialInspector::~LayerMaterialInspector()
{}

StatusCode Trk::LayerMaterialInspector::processNode(const Trk::TrackingVolume& , size_t) const
{
  return StatusCode::SUCCESS;
}  

StatusCode Trk::LayerMaterialInspector::processNode(const Trk::Layer& lay, size_t) const
{
    
    const Trk::TrackingVolume* tvol = lay.enclosingTrackingVolume();
    TString folderId   = tvol ? tvol->volumeName() : std::string("Unknown");
    TString folderName = m_treeFolder+folderId;
    folderName.ReplaceAll("::","_");
    
    // prepare the information
    int lIndex                                    = lay.layerIndex().value();
    const Amg::Vector3D& lCenter                  = lay.surfaceRepresentation().center();
    const Trk::LayerMaterialProperties* lMaterial = lay.layerMaterialProperties();
    
    // skip navigation layers
    if (!lMaterial) return StatusCode::SUCCESS;
    
    int   lType   = 0; // 1 - cylinder, 2 - disc 
    float dim0    = 0.;
    float dim1    = 0.;
                  
    int   bins0   = 1;
    int   bins1   = 1;
                  
    float centerX = lCenter.x();
    float centerY = lCenter.y();
    float centerZ = lCenter.z();
    
    // cylinder bounds 
    if ( lay.surfaceRepresentation().type() == Trk::Surface::Cylinder ){
        lType = 1;
        // cylinder bounds
        const Trk::CylinderBounds* cb = dynamic_cast<const Trk::CylinderBounds*>(&(lay.surfaceRepresentation().bounds()));
        if (cb){
            dim0 = cb->r();
            dim1 = cb->halflengthZ();
        }
    } else if ( lay.surfaceRepresentation().type() == Trk::Surface::Disc ) {
        lType = 2;
        // disc bounds
        const Trk::DiscBounds* db = dynamic_cast<const Trk::DiscBounds*>(&(lay.surfaceRepresentation().bounds()));
        if (db){
            dim0 = db->rMin();
            dim1 = db->rMax();
        }
    }
    
    if (lMaterial){
        const Trk::BinUtility*  lBinUtility  = lMaterial->binUtility();
        bins0 = lBinUtility ? lBinUtility->max(0)+1 : 1;
        bins1 = lBinUtility ? lBinUtility->max(1)+1 : 1;
    }


    // -------------------------------------------------------------------------------------
    TString hName  = ( lType == 1 ) ? "CylinderLayer_" : "DiscLayer_";
            hName += int(lIndex);
    
    TString pXo  = "_pX0";
    TString info = "_Information";
    
    TH2F* lMaterialHist = lType == 1 ?
            new TH2F(hName+pXo, hName, bins0, -M_PI*dim0, M_PI*dim0, bins1, -dim1, dim1) :
            new TH2F(hName+pXo, hName, bins0, dim0, dim1, bins1, -M_PI, M_PI);
   
    TTree* lTreeInformation = new TTree(hName+info,hName);
    lTreeInformation->Branch("LayerCenterX",        &centerX,   "cX/F");
    lTreeInformation->Branch("LayerCenterY",        &centerY,   "cY/F");
    lTreeInformation->Branch("LayerCenterZ",        &centerZ,   "cZ/F");
    lTreeInformation->Branch("LayerIndex",          &lIndex,    "idx/I");
    if (lType == 1){                                            
        lTreeInformation->Branch("Radius",          &dim0,      "r/F");
        lTreeInformation->Branch("HalflengthZ",     &dim1,      "hz/F");
        lTreeInformation->Branch("BinsRPhi",        &bins0,     "bin0/I");
        lTreeInformation->Branch("BinsHalfZ",       &bins1,     "bin1/I");
    } else if (lType == 2){                                     
        lTreeInformation->Branch("InnerRadius",     &dim0,      "rmin/F");
        lTreeInformation->Branch("OuterRadius",     &dim1,      "rmax/F");
        lTreeInformation->Branch("BinsR",           &bins0,     "bin0/I");
        lTreeInformation->Branch("BinsPhi",         &bins1,     "bin1/I");        
    }
   
    // --------------- registration for histogram and tree ---------------------------------
    TString regHistName = folderName+"/"+hName+"/"+hName+pXo;
    TString regTreeName = folderName+"/"+hName+"/"+hName+info;
    
    ITHistSvc* tHistSvc = 0;
    if (service("THistSvc",tHistSvc).isFailure()) {
       ATH_MSG_FATAL( "initialize() Could not find Hist Service! Aborting." );
       return StatusCode::FAILURE;
    }
    
    if ( (tHistSvc->regTree(std::string(regTreeName.Data()),lTreeInformation)).isFailure() ){
        ATH_MSG_WARNING("Could not register TTree with name " << hName);
    }
    lTreeInformation->Fill();

    if ( (tHistSvc->regHist(std::string(regHistName.Data()),lMaterialHist)).isFailure() ){
        ATH_MSG_WARNING("Could not register THist with name " << hName);
    }

    if (lMaterial){
        // fill the material into the bins
        for ( int ib0 = 0; ib0 < bins0; ++ib0  )
            for ( int ib1 = 0; ib1 < bins1; ++ib1  ){
                // get the material
                const Trk::MaterialProperties* mps = lMaterial->material(size_t(ib0),size_t(ib1));
                if (mps)
                    lMaterialHist->SetBinContent(ib0+1,ib1+1,mps->thicknessInX0());
        }
    }
   
    return StatusCode::SUCCESS;    
}

StatusCode Trk::LayerMaterialInspector::processNode(const Trk::Surface& , size_t) const
{
  return StatusCode::SUCCESS;
}  




