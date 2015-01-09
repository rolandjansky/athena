/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// SiResidualValidationNtupleHelper.cxx
//   Source file for class SiResidualValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

//Gaudi
//#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TTree.h"

// Trk
#include "InDetTrackValidation/SiResidualValidationNtupleHelper.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

//#include "TrkEventPrimitives/LocalParameters.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "InDetPrepRawData/SiWidth.h"
#include "TrkSurfaces/Surface.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
//#include "TrkEventPrimitives/LocalPosition.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "CLHEP/Vector/TwoVector.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "xAODEventInfo/EventInfo.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <cmath>

//using HepGeom::Vector3D;
using CLHEP::deg;

// constructor
InDet::SiResidualValidationNtupleHelper::SiResidualValidationNtupleHelper(
		const std::string& t, const std::string& n, const IInterface*  p):
    AthAlgTool(t,n,p),
    m_pixelid(0),
    m_WriteDetailedPixelInformation(false),
    m_rdocontainername("PixelRDOs"),
    m_rdocontainer(0),
    m_PixelConditionsSvc("PixelSiliconConditionsSvc",n), 
    m_PixDetType(0),
    m_PixHasGanged(0),
    m_PixIsFake(0),
    m_PixIsSplit(0),
    m_PixSplitProb1(0),
    m_PixSplitProb2(0),
    m_PixLVL1A(0),
    m_PixToT(0),
    m_PixCharge(0),
    m_PixTLorPhi(0),
    m_PixTLorEta(0),
    m_PixBiasVolt(0),
    m_PixTemp(0),
    m_PixDepVolt(0),
    m_PixDigResPhi(0),
    m_PixDigResEta(0),
    m_PixErrPhi(0),
    m_PixErrEta(0),
    m_PixEta(0),
    m_PixGloX(0),
    m_PixGloY(0),
    m_PixGloZ(0),
    m_PixLocX(0),
    m_PixLocY(0),
    m_PixEtaModule(0),
    m_PixPhiModule(0),
    m_PixFirstRow(0),
    m_PixFirstCol(0),
    m_PixDeltaPhi(0),
    m_PixDeltaEta(0),
    m_PixDeltaRow(0),
    m_PixDeltaCol(0),
    m_PixOmegaPhi(0),
    m_PixOmegaEta(0),
    m_PixTrkAngle(0),
    m_PixTrkThetaI(0),
    m_PixTrkEta(0),
    m_PixTrkPt(0),
    m_PixTrkQ(0),
    m_PixTrkClusGroupSize(0),
    m_PixChargeList(0),	
    m_PixToTList(0),
    m_PixLVL1AList(0),
    m_PixEtaIndex(0),
    m_PixPhiIndex(0),
    m_PixHoleGloX(0),
    m_PixHoleGloY(0),
    m_PixHoleGloZ(0),
    m_PixHoleLocX(0),
    m_PixHoleLocY(0),
    m_PixHoleEtaModule(0),
    m_PixHolePhiModule(0),
    m_PixHoleLayer(0),
    m_PixHoleECBarrel(0),
    m_PixHoleClNearstSize(0),
    m_PixHoleLocXClNearst(0),
    m_PixHoleLocYClNearst(0),
    m_PixHoleClRowClNearst(0),
    m_PixHoleClColClNearst(0),
    m_PixHoleGloZNearst(0),
    m_PixHoleDeltaRowNearst(0),
    m_PixHoleDeltaColNearst(0),
    m_SctDetType(0),
    m_SctLayerDisk(0),
    m_SctEtaModule(0),
    m_SctPhiModule(0),
    m_SctSide(0),
    m_SctTLorPhi(0),
    m_SctTLorEta(0),
    m_SctEta(0),
    m_SctDeltaPhi(0),
    m_SctLocX(0),
    m_SctLocY(0),
    m_SctIncAng(0),
    m_SctTrkAngle(0),
    m_SctTrkQ(0),
    m_SctErr(0),
    m_SctTrkClusGroupSize(0),
    m_lvl1TriggerType(0)
 {
   declareInterface<IValidationNtupleHelperTool>(this);
   declareProperty("WriteDetailedPixelInformation",m_WriteDetailedPixelInformation);
   declareProperty("PixelRDO_Container",m_rdocontainername);
}

// destructor
InDet::SiResidualValidationNtupleHelper::~SiResidualValidationNtupleHelper() {}

////////////////////////
/// initialize
////////////////////////
StatusCode InDet::SiResidualValidationNtupleHelper::initialize() {

	
    // Get the Pixel and SCT helpers
    if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
      ATH_MSG_FATAL ("Could not get Pixel ID helper");
      return StatusCode::FAILURE;
    }
    if (detStore()->retrieve(m_sctid, "SCT_ID").isFailure()) {
      ATH_MSG_FATAL ("Could not get SCT ID helper");
      return StatusCode::FAILURE;
    }

    //Get Pixel conditions
    StatusCode sc= m_PixelConditionsSvc.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL( "PixelSiliconConditionsSvc service not found !" );
      return sc;
    }
	
    m_PixDetType    = new std::vector<int>();
    m_PixHasGanged  = new std::vector<int>();
    m_PixIsFake     = new std::vector<int>();
    m_PixIsSplit    = new std::vector<int>();
    m_PixSplitProb1 = new std::vector<float>();
    m_PixSplitProb2 = new std::vector<float>();
    m_PixLVL1A      = new std::vector<int>();
    m_PixToT        = new std::vector<int>();
    m_PixCharge     = new std::vector<float>();
    m_PixTLorPhi    = new std::vector<float>();
    m_PixTLorEta    = new std::vector<float>();
    m_PixBiasVolt   = new std::vector<float>();
    m_PixTemp       = new std::vector<float>();
    m_PixDepVolt    = new std::vector<float>();
    m_PixLocX       = new std::vector<float>();
    m_PixLocY       = new std::vector<float>();
    m_PixEtaModule  = new std::vector<int>();
    m_PixPhiModule  = new std::vector<int>();
    m_PixGloX       = new std::vector<float>();
    m_PixGloY       = new std::vector<float>();
    m_PixGloZ       = new std::vector<float>();
    m_PixEta        = new std::vector<float>();
    m_PixFirstRow   = new std::vector<float>();
    m_PixFirstCol   = new std::vector<float>();
    m_PixDigResPhi  = new std::vector<float>();
    m_PixDigResEta  = new std::vector<float>();
    m_PixErrPhi     = new std::vector<float>();
    m_PixErrEta     = new std::vector<float>();
    m_PixDeltaRow   = new std::vector<int>();
    m_PixDeltaCol   = new std::vector<int>();
    m_PixDeltaPhi   = new std::vector<float>();
    m_PixDeltaEta   = new std::vector<float>();
    m_PixOmegaPhi   = new std::vector<float>();
    m_PixOmegaEta   = new std::vector<float>();
    m_PixChargeList = new std::vector<std::vector<float> >();
    m_PixToTList    = new std::vector<std::vector<int> >();
    m_PixLVL1AList  = new std::vector<std::vector<int> >();
    m_PixEtaIndex   = new std::vector<std::vector<int> >();
    m_PixPhiIndex   = new std::vector<std::vector<int> >();
    m_PixTrkAngle   = new std::vector<float>();
    m_PixTrkThetaI  = new std::vector<float>();
    m_PixTrkEta     = new std::vector<float>();
    m_PixTrkPt      = new std::vector<float>();
    m_PixTrkQ       = new std::vector<float>();
    m_PixTrkClusGroupSize = new std::vector<float>();
    m_PixHoleGloX        = new std::vector<float>();
    m_PixHoleGloY        = new std::vector<float>();
    m_PixHoleGloZ        = new std::vector<float>();
    m_PixHoleLocX        = new std::vector<float>();    
    m_PixHoleLocY        = new std::vector<float>(); 
    m_PixHoleEtaModule   = new std::vector<int>(); 
    m_PixHolePhiModule   = new std::vector<int>(); 
    m_PixHoleLayer       = new std::vector<float>();
    m_PixHoleECBarrel    = new std::vector<float>();
    m_PixHoleClNearstSize    = new std::vector<int>();
    m_PixHoleLocXClNearst    = new std::vector<float>();
    m_PixHoleLocYClNearst    = new std::vector<float>();
    m_PixHoleGloZNearst      = new std::vector<float>();
    m_PixHoleClRowClNearst   = new std::vector<float>();
    m_PixHoleClColClNearst   = new std::vector<float>();
    m_PixHoleDeltaRowNearst  = new std::vector<float>();
    m_PixHoleDeltaColNearst  = new std::vector<float>();
    m_SctDetType    = new std::vector<int>();
    m_SctLayerDisk  = new std::vector<int>();
    m_SctEtaModule  = new std::vector<int>();
    m_SctPhiModule  = new std::vector<int>(); 
    m_SctSide       = new std::vector<int>();
    m_SctTLorPhi    = new std::vector<float>();
    m_SctTLorEta    = new std::vector<float>();
    m_SctEta        = new std::vector<float>();
    m_SctDeltaPhi   = new std::vector<float>();
    m_SctLocX   = new std::vector<float>();
    m_SctLocY   = new std::vector<float>();
    m_SctIncAng   = new std::vector<float>();
    m_SctTrkAngle   = new std::vector<float>();
    m_SctTrkQ       = new std::vector<float>();
    m_SctErr        = new std::vector<float>();
    m_SctTrkClusGroupSize = new std::vector<float>();

    return sc;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode InDet::SiResidualValidationNtupleHelper::finalize() {
    delete m_PixDetType;
    delete m_PixHasGanged;
    delete m_PixIsFake;
    delete m_PixIsSplit;
    delete m_PixSplitProb1;
    delete m_PixSplitProb2;
    delete m_PixLVL1A;
    delete m_PixToT;
    delete m_PixCharge;
    delete m_PixTLorPhi;
    delete m_PixTLorEta;
    delete m_PixBiasVolt;
    delete m_PixTemp;
    delete m_PixDepVolt;
    delete m_PixDigResPhi;
    delete m_PixDigResEta;
    delete m_PixErrPhi;
    delete m_PixErrEta;
    delete m_PixEta;
    delete m_PixGloX;
    delete m_PixGloY;
    delete m_PixGloZ;
    delete m_PixLocX;
    delete m_PixLocY;
    delete m_PixEtaModule;
    delete m_PixPhiModule;
    delete m_PixFirstRow;
    delete m_PixFirstCol;
    delete m_PixDeltaPhi;
    delete m_PixDeltaEta;
    delete m_PixDeltaRow;
    delete m_PixDeltaCol;
    delete m_PixOmegaPhi;
    delete m_PixOmegaEta;
    delete m_PixChargeList;
    delete m_PixToTList;
    delete m_PixLVL1AList;
    delete m_PixEtaIndex;
    delete m_PixPhiIndex;
    delete m_PixTrkAngle;
    delete m_PixTrkThetaI;
    delete m_PixTrkEta;
    delete m_PixTrkPt;
    delete m_PixTrkQ;
    delete m_PixTrkClusGroupSize;
    delete m_PixHoleGloX;
    delete m_PixHoleGloY;
    delete m_PixHoleGloZ;
    delete m_PixHoleLocX;
    delete m_PixHoleLocY;
    delete m_PixHoleEtaModule;
    delete m_PixHolePhiModule;
    delete m_PixHoleLayer;
    delete m_PixHoleECBarrel;
    delete m_PixHoleClNearstSize;
    delete m_PixHoleLocXClNearst;
    delete m_PixHoleLocYClNearst;
    delete m_PixHoleClRowClNearst;
    delete m_PixHoleClColClNearst;
    delete m_PixHoleGloZNearst;
    delete m_PixHoleDeltaRowNearst;
    delete m_PixHoleDeltaColNearst;
    delete m_SctDetType;
    delete m_SctLayerDisk;
    delete m_SctEtaModule;
    delete m_SctPhiModule;
    delete m_SctSide;
    delete m_SctTLorPhi;
    delete m_SctTLorEta;
    delete m_SctEta;
    delete m_SctDeltaPhi;
    delete m_SctLocX;
    delete m_SctLocY;
    delete m_SctIncAng;
    delete m_SctTrkAngle;
    delete m_SctTrkQ;
    delete m_SctErr;
    delete m_SctTrkClusGroupSize;
    m_PixDetType=0;
    m_PixHasGanged=0;
    m_PixIsFake=0;
    m_PixIsSplit=0;
    m_PixSplitProb1=0;
    m_PixSplitProb2=0;
    m_PixLVL1A=0;
    m_PixToT=0;
    m_PixCharge=0;
    m_PixTLorPhi=0;
    m_PixTLorEta=0;
    m_PixBiasVolt=0;
    m_PixTemp=0;
    m_PixDepVolt=0;
    m_PixDigResPhi=0;
    m_PixDigResEta=0;
    m_PixErrPhi=0;
    m_PixErrEta=0;
    m_PixEta=0;
    m_PixGloX=0;
    m_PixGloY=0;
    m_PixGloZ=0;
    m_PixLocX=0;
    m_PixLocY=0;
    m_PixEtaModule=0;
    m_PixPhiModule=0;
    m_PixFirstRow=0;
    m_PixFirstCol=0;
    m_PixDeltaPhi=0;
    m_PixDeltaEta=0;
    m_PixDeltaRow=0;
    m_PixDeltaCol=0;
    m_PixOmegaPhi=0;
    m_PixOmegaEta=0;
    m_PixChargeList=0;
    m_PixToTList=0;
    m_PixLVL1AList=0;
    m_PixEtaIndex=0;
    m_PixPhiIndex=0;
    m_PixTrkAngle=0;
    m_PixTrkThetaI=0;
    m_PixTrkEta=0;
    m_PixTrkPt=0;
    m_PixTrkQ=0;
    m_PixTrkClusGroupSize=0;
    m_PixHoleGloX=0;
    m_PixHoleGloY=0;
    m_PixHoleGloZ=0;
    m_PixHoleLocX=0;
    m_PixHoleLocY=0;
    m_PixHoleEtaModule=0;
    m_PixHolePhiModule=0;
    m_PixHoleLayer=0;
    m_PixHoleECBarrel=0;
    m_PixHoleClNearstSize=0;
    m_PixHoleLocXClNearst=0;
    m_PixHoleLocYClNearst=0;
    m_PixHoleClRowClNearst=0;
    m_PixHoleClColClNearst=0;
    m_PixHoleGloZNearst=0;
    m_PixHoleDeltaRowNearst=0;
    m_PixHoleDeltaColNearst=0;
    m_SctDetType=0;
    m_SctTLorPhi=0;
    m_SctTLorEta=0;
    m_SctEta=0;
    m_SctDeltaPhi=0;
    m_SctLocX=0;
    m_SctLocY=0;
    m_SctIncAng=0;
    m_SctTrkAngle=0;
    m_SctTrkQ=0;
    m_SctLayerDisk=0;
    m_SctEtaModule=0;
    m_SctPhiModule=0;    
    m_SctSide=0;
    m_SctErr=0;
    m_SctTrkClusGroupSize=0;
    ATH_MSG_INFO( "finalize() successful in " << name() );
	return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode InDet::SiResidualValidationNtupleHelper::addNtupleItems(
        TTree* tree, const int& detectorType) const{

    // -------------------------------------------
    // add items to the ntuple
    if (detectorType==Trk::TrackState::Pixel) {
        
	    
	// add items for pixel
	
	// Layer information (0, 1, 2 for barrel /
	// 100, 101, 102 for endcap A side, -100,
	// -101, -102 for endcap C side) it is 0 by default
        tree->Branch("PixDetType",      &m_PixDetType);
	// 1 if the cluster contains ganged pixels 
        tree->Branch("PixHasGanged",    &m_PixHasGanged);
	// 1 if the cluster is marked as fake
        tree->Branch("PixIsFake",       &m_PixIsFake);
	// 1 if the cluster is marked as split
        tree->Branch("PixIsSplit",      &m_PixIsSplit);
	// probability to be a 1-particle cluster
        tree->Branch("PixSplitProb1",   &m_PixSplitProb1);
	// probability to be a 2-particle cluster
        tree->Branch("PixSplitProb2",   &m_PixSplitProb2);
	// lower level1 accept among the pixel of the cluster
        tree->Branch("PixLVL1A",        &m_PixLVL1A);
	// total ToT of the cluster
        tree->Branch("PixToT",          &m_PixToT);
	// total charge of the cluster
        tree->Branch("PixCharge",       &m_PixCharge);
	// tangent of the lorentz angle (local x direction)
        tree->Branch("PixTLorPhi",      &m_PixTLorPhi);
	// tangent of the lorentz angle (local y direction - should be 0!)
        tree->Branch("PixTLorEta",      &m_PixTLorEta);
	// Bias voltage (from conditions service)
        tree->Branch("PixBiasVolt",      &m_PixBiasVolt);
	// Temperature (from conditions service)
        tree->Branch("PixTemp",      &m_PixTemp);
	// Depletion voltage (from conditions service)
        tree->Branch("PixDepVolt",      &m_PixDepVolt);
        // Residuals obtained using the following cluster coordinates:
        // [x(rowmin)+x(rowmax)]/2.
        // [y(colmin)+y(colmax)]/2
        // they are useful for the tuning of charge interpolation algorithms
        tree->Branch("PixDigResPhi",    &m_PixDigResPhi);
        tree->Branch("PixDigResEta",    &m_PixDigResEta);
        // Pixel Cluster position error (local x)
        tree->Branch("PixErrPhi",       &m_PixErrPhi);
        // Pixel Cluster position error (local y)
        tree->Branch("PixErrEta",       &m_PixErrEta);
        // Cluster local coordinates
        tree->Branch("PixLocX",         &m_PixLocX);
        tree->Branch("PixLocY",         &m_PixLocY);
	// Eta and phi indexes of the module
        tree->Branch("PixEtaModule",    &m_PixEtaModule);
        tree->Branch("PixPhiModule",    &m_PixPhiModule);
	// Global position of the cluster
        tree->Branch("PixGloX",         &m_PixGloX);
        tree->Branch("PixGloY",         &m_PixGloY);
        tree->Branch("PixGloZ",         &m_PixGloZ);
        tree->Branch("PixEta",          &m_PixEta);
	// lower row of the pixels contained in the cluster
        tree->Branch("PixFirstRow",     &m_PixFirstRow);
	// lower column of the pixels contained in the cluster
        tree->Branch("PixFirstCol",     &m_PixFirstCol);
        // Pixel Cluster Size, in cell units
        tree->Branch("PixDeltaRow",     &m_PixDeltaRow);
        tree->Branch("PixDeltaCol",     &m_PixDeltaCol);
        // Pixel cluster size, in mm
        tree->Branch("PixDeltaPhi",     &m_PixDeltaPhi);
        tree->Branch("PixDeltaEta",     &m_PixDeltaEta);
        // Pixel charge sharing variable. It is defined as
        // omega_phi = Q1/(Q+Q2)
        // where Q1 is the sum of the charges of the pixel cells of the
        // first row of the cluster, Q2 is the sum of the charges of
        // pixel cells of the last row of the cluster
        // For omega_eta the definition is similar, changing row with
        // columns
        // It is a data member of PixelCluster.cxx and it is used in
        // charge interpolation algorithms
        tree->Branch("PixOmegaPhi",     &m_PixOmegaPhi);
        tree->Branch("PixOmegaEta",     &m_PixOmegaEta);
	/// detailed information for Pixel in the Cluster
	if(m_WriteDetailedPixelInformation==true){
	  tree->Branch("PixChargeList",&m_PixChargeList) ;
	  tree->Branch("PixToTList",&m_PixToTList) ;
	  tree->Branch("PixLVL1AList",&m_PixLVL1AList) ;
	  tree->Branch("PixEtaIndex", &m_PixEtaIndex);
	  tree->Branch("PixPhiIndex", &m_PixPhiIndex);
	}
        // Track incidence angle on the module (rphi plane) in local reference frame
        tree->Branch("PixTrkAngle",     &m_PixTrkAngle);
        // Track incidence angle on the module (eta-normal plane) in local reference frame
        tree->Branch("PixTrkThetaI",     &m_PixTrkThetaI);
        // Track pseudorapidity incidence in local module reference frame
        // i.e. m_log(atan(theta/2)) where theta is the local polar angle
        tree->Branch("PixTrkEta",       &m_PixTrkEta);
	// Track momentum
        tree->Branch("PixTrkPt",        &m_PixTrkPt);
        // charge of the associated track (this allows to easily
        // make plots for positive and negative particles using the same
        // ntuple, for example)
        tree->Branch("PixTrkQ",         &m_PixTrkQ);
	// total size of the cluster
        tree->Branch("PixTrkClusGroupSize", &m_PixTrkClusGroupSize);
        
	
	// informations about holes on tracks
	// Global position of the hole
        tree->Branch("PixHoleGloX",       &m_PixHoleGloX);
        tree->Branch("PixHoleGloY",       &m_PixHoleGloY);
        tree->Branch("PixHoleGloZ",       &m_PixHoleGloZ);
	// Local position of the hole
        tree->Branch("PixHoleLocX",       &m_PixHoleLocX);
        tree->Branch("PixHoleLocY",       &m_PixHoleLocY);
	// Eta and phi indexes of the module that contains the hole	
        tree->Branch("PixHoleEtaModule",  &m_PixHoleEtaModule);
        tree->Branch("PixHolePhiModule",  &m_PixHolePhiModule);
	// Layer (0, 1, 2) of the hole
        tree->Branch("PixHoleLayer",      &m_PixHoleLayer);
	// -2 for EC, 2 for EA, 0 for barrel
        tree->Branch("PixHoleECBarrel",   &m_PixHoleECBarrel);
        // informations about nearest cluster to hole
	// total size of the nearest cluster
        tree->Branch("PixHoleClNearstSize",  &m_PixHoleClNearstSize);
	// nearest cluster local X
        tree->Branch("PixHoleLocXClNearst",  &m_PixHoleLocXClNearst);
	// nearest cluster local Y
        tree->Branch("PixHoleLocYClNearst",  &m_PixHoleLocYClNearst);
	// Column and row of the nearest cluster
        tree->Branch("PixHoleClRowClNearst", &m_PixHoleClRowClNearst);
        tree->Branch("PixHoleClColClNearst", &m_PixHoleClColClNearst);
	// global z of the nearest cluster
        tree->Branch("PixHoleGloZNearst",    &m_PixHoleGloZNearst);
	// nearest Cluster Size, in cell units
        tree->Branch("PixHoleDeltaRowNearst",&m_PixHoleDeltaRowNearst);
        tree->Branch("PixHoleDeltaColNearst",&m_PixHoleDeltaColNearst);



	// LVL1 trigger type
        tree->Branch("LVL1TriggerType",&m_lvl1TriggerType, "LVL1TriggerType/I");
    
    
    } else if (detectorType==Trk::TrackState::SCT) {

        // 1 for barrel, 2 for disks
        tree->Branch("SctDetType",      &m_SctDetType);
        tree->Branch("SctLayerDisk",    &m_SctLayerDisk);
        tree->Branch("SctEtaModule",    &m_SctEtaModule);
        tree->Branch("SctPhiModule",    &m_SctPhiModule);        
	tree->Branch("SctSide",         &m_SctSide);
        // Lorentz angle in the two projection
        tree->Branch("SctTLorPhi",      &m_SctTLorPhi);
        tree->Branch("SctTLorEta",      &m_SctTLorEta);
        // psudorapidity, cluster size
        tree->Branch("SctEta",          &m_SctEta);
        // cluster size
        tree->Branch("SctDeltaPhi",     &m_SctDeltaPhi);
	// incidence angle in plane perp. to strip direction
	tree->Branch("SctIncAng",       &m_SctIncAng); 
	//Local X and Y
        tree->Branch("SctLocX",       &m_SctLocX);
        tree->Branch("SctLocY",       &m_SctLocY);
        // SCT angle (in rphi plane) in local module reference frame
        tree->Branch("SctTrkAngle",     &m_SctTrkAngle);
        // charge of the associated track (convenience)
        tree->Branch("SctTrkQ",         &m_SctTrkQ);
        tree->Branch("SctErr",          &m_SctErr);
        tree->Branch("SctTrkClusGroupSize", &m_SctTrkClusGroupSize);
    
    }

    ATH_MSG_DEBUG( "added all items to ntuple" );
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill silicon data
////////////////////////////////////////
StatusCode InDet::SiResidualValidationNtupleHelper::fillMeasurementData (
		const Trk::MeasurementBase* measurement,
		const Trk::TrackParameters* trkParameters,
		const int& detectorType, const bool& isOutlier) const{


        const xAOD::EventInfo* m_eventInfo;
	StatusCode status = evtStore()->retrieve(m_eventInfo);
	if (status.isFailure())
		ATH_MSG_ERROR( "Could not retrieve event info" );

	m_lvl1TriggerType = m_eventInfo->level1TriggerType();
	ATH_MSG_DEBUG(m_lvl1TriggerType );


	if (isOutlier) {
		// outliers usually have no track parameters
		// and we do not want to have the outliers
		// in the pull distributions
		return StatusCode::SUCCESS;
	}
	const Trk::RIO_OnTrack *ROT = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
	if (ROT && trkParameters){

				
        if (detectorType==Trk::TrackState::Pixel) {
			m_PixEta->push_back( measurement->globalPosition().eta() );
			m_PixGloX->push_back( measurement->globalPosition().x() );
			m_PixGloY->push_back( measurement->globalPosition().y() );
			m_PixGloZ->push_back( measurement->globalPosition().z() );
			m_PixLocX->push_back( measurement->localParameters()[Trk::locX] );
			m_PixLocY->push_back( measurement->localParameters()[Trk::locY] );
			    
			//m_PixErrPhi->push_back(measurement->localErrorMatrix().error(Trk::locX) );
			//m_PixErrEta->push_back(measurement->localErrorMatrix().error(Trk::locY) );

		       	m_PixErrPhi->push_back(Amg::error(measurement->localCovariance(),Trk::locX) );
			m_PixErrEta->push_back(Amg::error(measurement->localCovariance(),Trk::locY) );

			// cluster sizes
			ATH_MSG_DEBUG( "cluster sizes" );

			const Trk::PrepRawData* prd = ROT->prepRawData();
			float PixDigResPhi  = -1000.;
			float PixDigResEta  = -1000.;
			int   PixDeltaRow   = 0;
			int   PixDeltaCol   = 0;
			float PixDeltaPhi   = -1000.;
			float PixDeltaEta   = -1000.;
			float PixOmegaPhi   = -1000.;
			float PixOmegaEta   = -1000.;
			float PixTLorPhi    = -1000.;
			float PixTLorEta    = -1000.;
			float PixBiasVolt    = -1000.;
			float PixTemp    = -1000.;
			float PixDepVolt    = -1000.;
			int   PixHasGanged  = 0;
			int   PixIsFake     = 0;
			int   PixIsSplit    = 0;
			int   PixSplitProb1 = 0;
			int   PixSplitProb2 = 0;
			int   PixLVL1A      = 0;
			int   PixToT        = 0;
			float PixCharge     = 0;
			int   PixDetType    = 0;
			float PixTrkAngle   = -1000.;
			float PixTrkThetaI   = -1000.;
			float PixTrkEta     = -1000.;
			float PixTrkPt      = -1000.;
			float PixTrkQ       = -1000.;
            		float PixTrkClusGroupSize = -1000.;
			int PixEtaModule = -1000;
			int PixPhiModule = -1000;
			std::vector<float>* PixChargeList = new std::vector<float>();
			std::vector<int>* PixToTList  = new std::vector<int>();
			std::vector<int>* PixLVL1AList= new std::vector<int>();
			std::vector<int>* PixEtaIndex = new std::vector<int>();
			std::vector<int>* PixPhiIndex = new std::vector<int>();

			if(prd) {
				const InDet::PixelCluster* sc =
					dynamic_cast<const InDet::PixelCluster*>(prd);
				if(sc) {
					const InDetDD::SiDetectorElement* element =
						sc->detectorElement(); 

					// digital resolution
					ATH_MSG_DEBUG( "digital resolution..." );
					Amg::Vector2D digLocPar = GetDigitalPosition(sc,element);
					ATH_MSG_DEBUG( "done" );
					
					PixDigResPhi = digLocPar[Trk::distPhi] - trkParameters->parameters()[Trk::distPhi];
					PixDigResEta = digLocPar[Trk::distEta] - trkParameters->parameters()[Trk::distEta];
					if(sc->gangedPixel()) PixHasGanged = 1;
					if(sc->isFake()) PixIsFake = 1;
					if(sc->isSplit()) PixIsSplit = 1;
					PixSplitProb1=sc->splitProbability1();
					PixSplitProb2=sc->splitProbability2();
					PixLVL1A = sc->LVL1A();
					PixToT = sc->totalToT();
					PixCharge = sc->totalCharge();
					PixOmegaPhi = sc->omegax();
					PixOmegaEta = sc->omegay();
					
					InDet::SiWidth width = sc->width();
					PixDeltaRow = int(width.colRow().x());
					PixDeltaCol = int(width.colRow().y());
					PixDeltaPhi = width.phiR();
					PixDeltaEta = width.z();

					PixTLorPhi  = element->design().readoutSide() * element->getTanLorentzAnglePhi();
					PixTLorEta  = element->getTanLorentzAngleEta();

					const Identifier element_id = element->identify();
				
					PixBiasVolt = m_PixelConditionsSvc->biasVoltage(element_id);
					PixTemp = m_PixelConditionsSvc->temperature(element_id);
					PixDepVolt = m_PixelConditionsSvc->depletionVoltage(element_id);
					
					PixEtaModule = m_pixelid->eta_module(element_id);
					PixPhiModule = m_pixelid->phi_module(element_id);

					// use 0, 1, 2 for barrel / 100, 101, 102 for EA / -100, -101, -102 for EC
					if(m_pixelid->barrel_ec(element_id)==0) PixDetType = m_pixelid->layer_disk(element_id);
					else if(m_pixelid->barrel_ec(element_id)==-2) PixDetType =-1* (100 + m_pixelid->layer_disk(element_id));
                        		else if(m_pixelid->barrel_ec(element_id)==2) PixDetType =(100 + m_pixelid->layer_disk(element_id));

//					HepVector3D my_track = trkParameters->momentum();
//					HepVector3D my_normal = element->normal();
//					HepVector3D my_phiax = element->phiAxis();
//					HepVector3D my_etaax = element->etaAxis();
					
					Amg::Vector3D my_track = trkParameters->momentum();
					Amg::Vector3D my_normal = element->normal();
					Amg::Vector3D my_phiax = element->phiAxis();
					Amg::Vector3D my_etaax = element->etaAxis();

                                        // track component on etaAxis:
					float trketacomp = my_track.dot(my_etaax);
					// track component on phiAxis:
					float trkphicomp = my_track.dot(my_phiax);
					// track component on the normal to the module
					float trknormcomp = my_track.dot(my_normal);
					// Track angle
					PixTrkAngle = atan2(trkphicomp,trknormcomp);
					PixTrkThetaI = atan2(trketacomp,trknormcomp);
					// reducing the angle in the right quadrant
					float Theta = PixTrkThetaI;
					if(Theta > M_PI / 2 ) Theta -= M_PI;
					else if(Theta < - M_PI / 2 ) Theta += M_PI;
					Theta = ( M_PI / 2 ) - Theta;
					PixTrkEta   = - log(tan(Theta/2));
					PixTrkPt    = trkParameters->pT();
					PixTrkQ     = trkParameters->charge();
                    			PixTrkClusGroupSize = sc->rdoList().size(); 

					if(m_WriteDetailedPixelInformation==true){
					  PixelRDO_Container::const_iterator rdo_collection; 
					  if ( m_rdocontainer ) 
					    rdo_collection = 
					      m_rdocontainer->indexFind(m_pixelid->wafer_hash(element_id));

					  const std::vector<Identifier>& rdoID = sc->rdoList();
					  for(int w=0; w<PixTrkClusGroupSize; w++) {
					    PixEtaIndex->push_back(m_pixelid->eta_index((rdoID)[w]));
					    PixPhiIndex->push_back(m_pixelid->phi_index((rdoID)[w]));
					    if ( m_rdocontainer && rdo_collection!=m_rdocontainer->end() ) {
					      int LVL1L = -1, ToTL=-1;
					      PixelRDO_Collection::const_iterator pRDO    = (*rdo_collection)->begin();
					      PixelRDO_Collection::const_iterator pRDOend = (*rdo_collection)->end();
					      // take into account ganged pixels are not in the RDO collection
					      Identifier rdoID1 = rdoID[w], rdoID2 = rdoID[w]; 
					      if ( sc->gangedPixel() ) {
						InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID[w]);
						if ( element->numberOfConnectedCells (cellID) > 1) {
						  InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,0);
						  if ( gangedCellID==cellID ) gangedCellID = element->connectedCell (cellID,1);
						  rdoID2 = element->identifierFromCellId (gangedCellID);
						} 
					      }
					      while (pRDO!=pRDOend) {
						Identifier myID = (*pRDO)->identify(); 
						if ( myID==rdoID1 || myID==rdoID2 ) {
						  LVL1L = (*pRDO)->getLVL1A();
						  ToTL  = (*pRDO)->getToT();
						  break;
						}
						pRDO++;
					      }
					      PixToTList->push_back(ToTL);
					      PixLVL1AList->push_back(LVL1L);
					      if ( (sc->chargeList()).size()==PixTrkClusGroupSize )
						PixChargeList->push_back((sc->chargeList())[w]);
					      else 
						PixChargeList->push_back(-1.);
					    }
					  }					
					}
					
				}// end if(pixel cluster)
			}else{
				m_PixFirstRow->push_back(0);
				m_PixFirstCol->push_back(0);
			}// end if (prd)
			m_PixEtaModule->push_back(PixEtaModule);
			m_PixPhiModule->push_back(PixPhiModule);
			m_PixDigResPhi->push_back(PixDigResPhi);
			m_PixDigResEta->push_back(PixDigResEta);
			m_PixDeltaRow->push_back(PixDeltaRow);
			m_PixDeltaCol->push_back(PixDeltaCol);
			m_PixDeltaPhi->push_back(PixDeltaPhi);
			m_PixDeltaEta->push_back(PixDeltaEta);
			m_PixOmegaPhi->push_back(PixOmegaPhi);
			m_PixOmegaEta->push_back(PixOmegaEta);
			m_PixTLorPhi->push_back(PixTLorPhi);
			m_PixTLorEta->push_back(PixTLorEta);
			m_PixBiasVolt->push_back(PixBiasVolt);
			m_PixTemp->push_back(PixTemp);
			m_PixDepVolt->push_back(PixDepVolt);
			m_PixHasGanged->push_back(PixHasGanged);
			m_PixIsFake->push_back(PixIsFake);
			m_PixIsSplit->push_back(PixIsSplit);
			m_PixSplitProb1->push_back(PixSplitProb1);
			m_PixSplitProb2->push_back(PixSplitProb2);
			m_PixLVL1A->push_back(PixLVL1A);
			m_PixToT->push_back(PixToT);
			m_PixCharge->push_back(PixCharge);
			m_PixDetType->push_back(PixDetType);
			m_PixTrkAngle->push_back(PixTrkAngle);
			m_PixTrkThetaI->push_back(PixTrkThetaI);
			m_PixTrkEta->push_back(PixTrkEta);
			m_PixTrkPt->push_back(PixTrkPt);
			m_PixTrkQ->push_back(PixTrkQ);
            		m_PixTrkClusGroupSize->push_back(PixTrkClusGroupSize);
			m_PixChargeList->push_back(*PixChargeList);
			m_PixToTList->push_back(*PixToTList);
			m_PixLVL1AList->push_back(*PixLVL1AList);
			m_PixEtaIndex->push_back(*PixEtaIndex);
			m_PixPhiIndex->push_back(*PixPhiIndex);
                        delete PixChargeList;
                        delete PixToTList;
                        delete PixLVL1AList;
                        delete PixEtaIndex;
                        delete PixPhiIndex;

		}else if (detectorType==Trk::TrackState::SCT) {
			
			m_SctEta->push_back( measurement->globalPosition().eta() );
			//m_SctErr->push_back( measurement->localErrorMatrix().error(Trk::locX) );

			m_SctErr->push_back( Amg::error(measurement->localCovariance(),Trk::locX) );

			float SctDeltaPhi   = -1000.;
			float SctLocX       = -1000.;
			float SctLocY       = -1000.;
			float SctIncAng     = -1000.; 
			float SctTLorPhi    = -1000.;
			float SctTLorEta    = -1000.;
			int SctDetType      = 0;
			float SctTrkAngle   = -1000.;
			float SctTrkQ       = -1000.;
			float SctTrkClusGroupSize = -1000.;
			int SctPhiModule    = 0;
			int SctSide         = 0;
			int SctEtaModule    = 0;
			int SctLayerdisk    = 0;
            
			const Trk::PrepRawData* prd = ROT->prepRawData();
			if(prd) {
				const InDet::SiCluster* sc = dynamic_cast<const InDet::SiCluster*>(prd);
				if(sc) {
					const InDetDD::SiDetectorElement* element = sc->detectorElement();
                    			const Identifier clId = sc->identify();

					InDet::SiWidth width = sc->width();
					SctDeltaPhi = width.phiR();
					SctPhiModule = m_sctid->phi_module(clId);
					SctSide      = m_sctid->side(clId);
                    			SctEtaModule = m_sctid->eta_module(clId);
                    			SctLayerdisk = m_sctid->layer_disk(clId);
                    			SctDetType  = m_sctid->barrel_ec(clId);

					SctTLorPhi  = element->getTanLorentzAnglePhi();
					SctTLorEta  = element->getTanLorentzAngleEta();

					// Track angle
					ATH_MSG_DEBUG( "track angle" );
					float phi = trkParameters->parameters()[Trk::phi];
					float trkq = trkParameters->charge();
					float phis = element->normal().phi();
					double PI = acos(-1.);
					if(fabs(phi-phis) < PI) SctTrkAngle = phi-phis;
					else if(phi-phis>PI) SctTrkAngle = phi-phis-2*PI;
					else if(phi-phis<-1*PI) SctTrkAngle = 2*PI+phi-phis;
					
					//Local X and Y
					SctLocX=trkParameters->parameters()[Trk::locX];
					SctLocY=trkParameters->parameters()[Trk::locY];
					//Incidence angle (for Lorentz angle calculation)
					double sinAlpha = element->sinStereoLocal(sc->localPosition());
					double phiToWafer(90.),thetaToWafer(90.);
					double pTrack[3];
					pTrack[0] = trkParameters->momentum().x();
					pTrack[1] = trkParameters->momentum().y();
					pTrack[2] = trkParameters->momentum().z();
					int iflag = findAnglesToWaferSurface (pTrack, sinAlpha,  element, thetaToWafer, phiToWafer );
					if ( iflag < 0) {
					  ATH_MSG_ERROR(  "Error in finding track angles to wafer surface");
					  return StatusCode::FAILURE;
					}
					SctIncAng = phiToWafer;

					SctTrkQ = trkq;
                    			SctTrkClusGroupSize=sc->rdoList().size();

				}// end if (silicon cluster)
			}// end if (prd)
			m_SctDeltaPhi->push_back( SctDeltaPhi );
			m_SctLocX->push_back(SctLocX);
			m_SctLocY->push_back(SctLocY);
                        m_SctIncAng->push_back( SctIncAng );
			m_SctTLorPhi->push_back( SctTLorPhi );
			m_SctTLorEta->push_back( SctTLorEta );
			m_SctDetType->push_back( SctDetType );
			m_SctTrkAngle->push_back( SctTrkAngle );
			m_SctTrkQ->push_back( SctTrkQ );
		    	m_SctTrkClusGroupSize->push_back( SctTrkClusGroupSize );
		    	m_SctPhiModule->push_back( SctPhiModule );
		    	m_SctSide->push_back( SctSide );
		    	m_SctEtaModule->push_back( SctEtaModule );
		    	m_SctLayerDisk->push_back( SctLayerdisk );

		} // end detector type
	}else return StatusCode::FAILURE; // end of if(ROT && trkParameters)
	
	ATH_MSG_DEBUG( "that's it folks" );
	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

Amg::Vector2D InDet::SiResidualValidationNtupleHelper::GetDigitalPosition(
		const InDet::PixelCluster*& pix,
		const InDetDD::SiDetectorElement*& EL) const {
	
	const InDetDD::PixelModuleDesign* design =
		dynamic_cast<const InDetDD::PixelModuleDesign*>(&EL->design());
	std::vector<Identifier> rdos = pix->rdoList();
	std::vector<Identifier>::const_iterator oneRDO = rdos.begin();
	int rowmin=9999;
	int rowmax=-9999;
	int colmin=9999;
	int colmax=-9999;
	for(; oneRDO != rdos.end(); oneRDO++) {
		int row = m_pixelid->phi_index(*oneRDO);
		int col = m_pixelid->eta_index(*oneRDO);
		if(rowmin > row) rowmin = row;
		if(rowmax < row) rowmax = row;
		if(colmin > col) colmin = col;
		if(colmax < col) colmax = col;
	}
	m_PixFirstRow->push_back(rowmin);
	m_PixFirstCol->push_back(colmin);
	InDetDD::SiLocalPosition pos1 = design->positionFromColumnRow(colmin,rowmin);
	InDetDD::SiLocalPosition pos2 = design->positionFromColumnRow(colmax,rowmin);
	InDetDD::SiLocalPosition pos3 = design->positionFromColumnRow(colmin,rowmax);
	InDetDD::SiLocalPosition pos4 = design->positionFromColumnRow(colmax,rowmax);
	InDetDD::SiLocalPosition centroid = 0.25*(pos1+pos2+pos3+pos4);
	double shift = EL->getLorentzCorrection();
	Amg::Vector2D position =
	  Amg::Vector2D(centroid.xPhi() + shift, centroid.xEta());
	return position;

}


////////////////////////////////////////
/// fill holes data
////////////////////////////////////////

StatusCode InDet::SiResidualValidationNtupleHelper::fillHoleData (
		const Trk::TrackStateOnSurface& tsos,
		const int& detectorType ) const{

	if (detectorType==Trk::TrackState::Pixel) {
		m_PixHoleGloX->push_back(tsos.trackParameters()->position().x());
		m_PixHoleGloY->push_back(tsos.trackParameters()->position().y());
		m_PixHoleGloZ->push_back(tsos.trackParameters()->position().z()); 
		const Trk::TrackParameters *tpmt = tsos.trackParameters();
		//const Trk::Surface aS = tpmt->associatedSurface();
		const Amg::Vector2D locPos= tpmt->localPosition();
		m_PixHoleLocX->push_back(locPos[Trk::locX]);	
		m_PixHoleLocY->push_back(locPos[Trk::locY]);

		Identifier detHoleId = tpmt->associatedSurface().associatedDetectorElementIdentifier();
		m_PixHoleEtaModule->push_back(int(m_pixelid->eta_module(detHoleId))); 
		m_PixHolePhiModule->push_back(int(m_pixelid->phi_module(detHoleId)));
		m_PixHoleLayer->push_back(float(m_pixelid->layer_disk(detHoleId))); 
		m_PixHoleECBarrel->push_back(float(m_pixelid->barrel_ec(detHoleId)));
	
		//	===   search for nearest cluster   ===	
		float dist=100, dist2=100;
		Identifier clNearstId;
		// default values if nearest cluster not found 
		int PixDeltaRowNearst = -1;
		int PixClRowClNearst  = -10;
		int PixClColClNearst  = -10;
		int PixDeltaColNearst = -1;
		int  GroupSizeNearst  = -1; 
		float   LocXClNearst  = -10;
		float   LocYClNearst  = -40;
		float   GloZNearst  = -1000;
	
		const InDet::PixelClusterContainer *m_riocontainer=0;
		StatusCode sc = evtStore()->retrieve(m_riocontainer, "PixelClusters");
		if(sc.isFailure()) {
			ATH_MSG_WARNING("Could not get PrepRawDataContainer");
			return sc;
		}
		

		InDet::PixelClusterContainer::const_iterator rioItr = m_riocontainer->begin();
		for( ; rioItr != m_riocontainer->end(); rioItr++){ // loop1
			const PixelClusterCollection *colNext=(*rioItr);
			if (!colNext) continue;
			if (detHoleId == colNext->identify()){ // if H==C
				DataVector<PixelCluster>::const_iterator p_clus = colNext->begin();
				for(; p_clus!=colNext->end(); ++p_clus) { // loop2
					const PixelCluster& cluster = **p_clus;
					const Amg::Vector2D locPos2 = cluster.localPosition();
					dist2=sqrt( (locPos2[Trk::locX]-locPos[Trk::locX]) *
							( locPos2[Trk::locX] - locPos[Trk::locX]) +
							(locPos2[Trk::locY] - locPos[Trk::locY]) *
							(locPos2[Trk::locY] - locPos[Trk::locY]) );
					if ( dist2<dist ){
						dist              = dist2; 
						InDet::SiWidth width = cluster.width(); 	
						PixDeltaRowNearst = int(width.colRow().x());
						PixDeltaColNearst = int(width.colRow().y());
						GroupSizeNearst   = cluster.rdoList().size();  
						GloZNearst        = cluster.globalPosition().z(); 
						clNearstId        = (*p_clus)->identify();
						PixClRowClNearst  = m_pixelid->phi_index(clNearstId);
						PixClColClNearst  = m_pixelid->eta_index(clNearstId);
						LocXClNearst      = locPos2[Trk::locX];
						LocYClNearst      = locPos2[Trk::locY];
					}// end if dist2<dist
				} // end loop2
			}  // end if H==C
		}  // end loop1 
		
		m_PixHoleClNearstSize   ->push_back(GroupSizeNearst); // nearest cluster size
		m_PixHoleLocXClNearst   ->push_back(LocXClNearst); // local x of cluster
		m_PixHoleLocYClNearst   ->push_back(LocYClNearst);
		m_PixHoleGloZNearst     ->push_back(GloZNearst); 
		m_PixHoleDeltaRowNearst ->push_back(PixDeltaRowNearst);
		m_PixHoleDeltaColNearst ->push_back(PixDeltaColNearst);
		m_PixHoleClRowClNearst  ->push_back(PixClRowClNearst); 
		m_PixHoleClColClNearst  ->push_back(PixClColClNearst); 
    }  // end if detectorType==Pixel
    
    return StatusCode::SUCCESS;
} // end fillHoleData ()

////////////////////////////////////////
/// calculate angle to SCT wafer
////////////////////////////////////////
int InDet::SiResidualValidationNtupleHelper::findAnglesToWaferSurface (double (&vec)[3], double &sinAlpha, const InDetDD::SiDetectorElement*& element, double &theta, double &phi ) const {

  int iflag(-1);

  phi   = 90.;
  theta = 90.;

  double cosAlpha = sqrt(1.-sinAlpha*sinAlpha);
  double phix =  cosAlpha*element->phiAxis().x() + sinAlpha*element->phiAxis().y();
  double phiy = -sinAlpha*element->phiAxis().x() + cosAlpha*element->phiAxis().y();

  double pNormal = vec[0]*element->normal().x()  + vec[1]*element->normal().y()  + vec[2]*element->normal().z();
  double pEta    = vec[0]*element->etaAxis().x() + vec[1]*element->etaAxis().y() + vec[2]*element->etaAxis().z();
  //  double pPhi    = vec[0]*element->phiAxis().x() + vec[1]*element->phiAxis().y() + vec[2]*element->phiAxis().z();
  double pPhi    = vec[0]*phix                   + vec[1]*phiy                   + vec[2]*element->phiAxis().z();

  if (pPhi<0.) phi   = -90.;
  if (pEta<0.) theta = -90.;
  if (pNormal!=0.) {
    phi   = atan(pPhi/pNormal)/deg;
    theta = atan(pEta/pNormal)/deg;
  }

  iflag = 1;

  return iflag;
}





////////////////////////////////////////
/// clear variables
////////////////////////////////////////

StatusCode InDet::SiResidualValidationNtupleHelper::resetVariables (
		const int& detectorType ) const {

  if (detectorType==Trk::TrackState::Pixel) {
    if( m_WriteDetailedPixelInformation ){
      StatusCode sc=evtStore()->retrieve(m_rdocontainer, m_rdocontainername);
      if ( sc.isFailure() ) {
	m_rdocontainer=0;
	ATH_MSG_DEBUG("Container " << m_rdocontainername << " not found");
      }
    }
    m_PixDetType->clear();
    m_PixHasGanged->clear();
    m_PixIsFake->clear();
    m_PixIsSplit->clear();
    m_PixSplitProb1->clear();
    m_PixSplitProb2->clear();
    m_PixLVL1A->clear();
    m_PixToT->clear();
    m_PixCharge->clear();
    m_PixTLorPhi->clear();
    m_PixTLorEta->clear();
    m_PixBiasVolt->clear();
    m_PixTemp->clear();
    m_PixDepVolt->clear();
    m_PixDigResPhi->clear();
    m_PixDigResEta->clear();
    m_PixErrPhi->clear();
    m_PixErrEta->clear();
    m_PixEta->clear();
    m_PixGloX->clear();
    m_PixGloY->clear();
    m_PixGloZ->clear();
    m_PixLocX->clear();
    m_PixLocY->clear();
    m_PixEtaModule->clear();
    m_PixPhiModule->clear();
    m_PixFirstRow->clear();
    m_PixFirstCol->clear();
    m_PixDeltaPhi->clear();
    m_PixDeltaEta->clear();
    m_PixDeltaRow->clear();
    m_PixDeltaCol->clear();
    m_PixOmegaPhi->clear();
    m_PixOmegaEta->clear();
    m_PixChargeList->clear();
    m_PixToTList->clear();
    m_PixLVL1AList->clear();
    m_PixEtaIndex->clear();
    m_PixPhiIndex->clear();
    m_PixTrkAngle->clear();
    m_PixTrkThetaI->clear();
    m_PixTrkEta->clear();
    m_PixTrkPt->clear();
    m_PixTrkQ->clear();
    m_PixTrkClusGroupSize->clear();
    m_PixHoleGloX->clear();
    m_PixHoleGloY->clear();
    m_PixHoleGloZ->clear();
    m_PixHoleLocX->clear();
    m_PixHoleLocY->clear();
    m_PixHoleEtaModule->clear();
    m_PixHolePhiModule->clear();
    m_PixHoleLayer->clear();
    m_PixHoleECBarrel->clear();
    m_PixHoleClNearstSize->clear();
    m_PixHoleLocXClNearst->clear();
    m_PixHoleLocYClNearst->clear();
    m_PixHoleClRowClNearst->clear();
    m_PixHoleClColClNearst->clear();
    m_PixHoleGloZNearst->clear();
    m_PixHoleDeltaRowNearst->clear();
    m_PixHoleDeltaColNearst->clear();
  }else if (detectorType==Trk::TrackState::SCT){
    m_SctDetType->clear();
    m_SctLayerDisk->clear();
    m_SctEtaModule->clear();
    m_SctPhiModule->clear();
    m_SctTLorPhi->clear();
    m_SctTLorEta->clear();
    m_SctEta->clear();
    m_SctDeltaPhi->clear();
    m_SctLocX->clear();
    m_SctLocY->clear();
    m_SctIncAng->clear();
    m_SctTrkAngle->clear();
    m_SctTrkQ->clear();
    m_SctErr->clear();
    m_SctTrkClusGroupSize->clear();
    m_SctSide->clear();
  }
	
  return StatusCode::SUCCESS;
}

