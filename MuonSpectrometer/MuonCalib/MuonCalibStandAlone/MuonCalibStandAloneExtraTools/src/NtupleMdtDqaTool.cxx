/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS NtupleMdtDqaTool ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

// Athena //
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"


// NtupleMdtDqaTool //
#include "MuonCalibStandAloneExtraTools/NtupleMdtDqaTool.h"
#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"


//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//root
#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"
#include "TString.h"
#include "TDirectory.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
namespace MuonCalib {

//*****************************************************************************

// constructor
//
// a parte le declareProperty ....come deve essere fatto questo costruttore? che sono qui "t" "n" e "p" ???
//
//

NtupleMdtDqaTool::NtupleMdtDqaTool( const std::string &t, const std::string &n, 
                                    const IInterface *p) : AthAlgTool(t, n, p) {
    
  declareInterface< NtupleCalibrationTool >(this);	

  m_MdtDqa_file_name = string("MdtDqa");
  declareProperty("MdtDqaFileName", m_MdtDqa_file_name);

  m_verbose = false;
  declareProperty("Verbose", m_verbose);

  m_doHitResids = true;
  declareProperty("doSegHitResidsVsRadius", m_doHitResids);

  m_fillHistos = true;
  declareProperty("FillHistos", m_fillHistos);

  m_doFinalize = true;
  declareProperty("doFinalize", m_doFinalize);

  m_doEfficiency = true;
  declareProperty("doEfficiency", m_doEfficiency);

  m_ADCCUT=70.;
  declareProperty("ADCCUT", m_ADCCUT);

  m_EffiNSigma = -1.;  // set a negative value for Hardware Eficiency
  declareProperty("EffiNSigma", m_EffiNSigma);

  m_EffiChi2Cut = 5.;  // cut on chi2/dof
  declareProperty("EffiChi2Cut", m_EffiChi2Cut);

  m_EffiHitADCCut = 0.;
  declareProperty("EffiHitADCCut", m_EffiHitADCCut);

  m_EffiUseDefaultResolution = false;
  declareProperty("EffiUseDefaultResolution", m_EffiUseDefaultResolution);

  m_EffiGTFitON = false;
  declareProperty("EffiGTFitON", m_EffiGTFitON);

  m_EffiUseNewCalibConstants = false;
  declareProperty("EffiUseNewCalibConstants", m_EffiUseNewCalibConstants);

  m_EffiUseTimeCorrections = false;
  declareProperty("EffiUseTimeCorrections", m_EffiUseTimeCorrections);

  //
  // MdtDqaGlobalTimeFit jobOptions :
  //
  m_doGlobalTimeFit = true;
  declareProperty("doGlobalTimeFit", m_doGlobalTimeFit);

  m_rtDefaultBfieldON = 0;
  declareProperty("rtDefaultBfieldON",m_rtDefaultBfieldON);

  m_GTFitSeg_chi2Cut = 15.;
  declareProperty("GTFitSeg_chi2Cut",m_GTFitSeg_chi2Cut);

  m_GTFitSeg_minNumHits = 4;
  declareProperty("GTFitSeg_minNumHits",m_GTFitSeg_minNumHits);

  m_GTFitSeg_maxNumHits = 10;
  declareProperty("GTFitSeg_maxNumHits",m_GTFitSeg_maxNumHits);

  m_GTFitDebug = false;
  declareProperty("GTFitDebug",m_GTFitDebug);

  // DO TRACK ANALYSIS 
  m_doTracks = false;
  declareProperty("doTracks",m_doTracks);

  m_TrkAuthor = -1;
  declareProperty("TrkAuthor",m_TrkAuthor);

  m_DeadElementsAlgorithm= 1; //1 = analysis by Marco, 0 = analysis by Toni
  declareProperty("DeadElementsAlgorithm",m_DeadElementsAlgorithm);
}
 
//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
StatusCode NtupleMdtDqaTool::initialize() {
	
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing NtupleMdtDqaTool" << endmsg;

  //-----------------------------//
  //-- Get the StoreGate Stuff --//
  //-----------------------------//

  StoreGateSvc *detStore;
  StatusCode sc=service("DetectorStore", detStore);
  if(!sc.isSuccess()){
    log << MSG::FATAL << "Cannot retrieve Store Gate!" << endmsg;
    return sc;
  }
  
  sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (!sc.isSuccess()) {
    log << MSG::FATAL << "Can't retrieve MdtIdHelper" << endmsg;
    return sc;
  }
  
  sc = detStore->retrieve( m_detMgr );
  if (!sc.isSuccess()) {
    log << MSG::FATAL << "Can't retrieve MuonDetectorManager" << endmsg;
    return sc;
  }

  //retrieve fixed id tool   
  std::string idToFixedIdToolType("MuonCalib::IdToFixedIdTool");
  std::string idToFixedIdToolName("MuonCalib_IdToFixedIdTool");
    
  sc = toolSvc()->retrieveTool(idToFixedIdToolType, idToFixedIdToolName, m_id_tool); 
  if(!sc.isSuccess()) {
    log << MSG::FATAL << "Can't retrieve IdToFixedIdTool" << endmsg;
    return sc;
  }

  //get region selection service
  sc=service("RegionSelectionSvc", p_reg_sel_svc);
  if(!sc.isSuccess()) {
    log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endmsg;
    return sc;
  }

  //get pointer to Calibration input service 
  sc=service("MdtCalibInputSvc", p_calib_input_svc);
  if(!sc.isSuccess()) {
    log << MSG::ERROR <<"Cannot retrieve MdtCalibInputSvc!" <<endmsg;
    return sc;
  }

  //------------------------------------------------------//
  //-- Check Region and book histograms                 --//
  //------------------------------------------------------//

  PhiEtaNameConverter phiEtaConverter;

  // loop on stations to find SectorMin and SectorMax
  int barrelEndCap;  // 1 = Barrel ;  2 = EndCap
  int iside;   // iside = 1 => "A"  ;  iside = 2  =>  "C"

  int sectorMin = 20;
  int sectorMax = 0;
  int SectorMin[2][2] ;
  int SectorMax[2][2] ;
  int SectorMinMax[2][2][2];
  string spectrometerRegion[2];
  string spectrometerSide[2];
  spectrometerRegion[0] = "Barrel";
  spectrometerRegion[1] = "Endcap";
  spectrometerSide[0] = "A";
  spectrometerSide[1] = "C";
  for (int i=0;i<=1;i++) {
    for (int j=0;j<=1;j++) {
      SectorMin[i][j] = 20;
      SectorMax[i][j] = 0;
      SectorMinMax[i][j][0] = 20;
      SectorMinMax[i][j][1] = 0;
    }
  }

  const std::vector<MuonCalib::NtupleStationId> stationsInRegion = p_reg_sel_svc->GetStationsInRegions();
  std::vector<MuonCalib::NtupleStationId>::const_iterator itstation;
  for (itstation = stationsInRegion.begin(); itstation!=stationsInRegion.end(); itstation++) {
//        int stationNameId = itstation->GetStation();
    string stationName = itstation->regionId();
    string chamberType = stationName.substr(0,3);
    int phi = itstation->GetPhi();
    int eta = itstation->GetEta();
    
    MDTName chamb(chamberType,phi,eta);
    int sector=chamb.getOnlineSector();
    iside = 1;
    if (chamb.isBackward()) iside = 2;
    barrelEndCap = 1;
    if ( chamb.isEndcap() ) barrelEndCap = 2;
    if(sector>SectorMax[barrelEndCap-1][iside-1]) SectorMax[barrelEndCap-1][iside-1] = sector;
    if(sector<SectorMin[barrelEndCap-1][iside-1]) SectorMin[barrelEndCap-1][iside-1] = sector;
    if(sector>sectorMax) sectorMax = sector;
    if(sector<sectorMin) sectorMin = sector;
    
    if(sector<SectorMinMax[barrelEndCap-1][iside-1][0]) SectorMinMax[barrelEndCap-1][iside-1][0] = sector;
    if(sector>SectorMinMax[barrelEndCap-1][iside-1][1]) SectorMinMax[barrelEndCap-1][iside-1][1] = sector;
  }


  //------------------------------------------------------------//
  //   Attach chamber name to output file name,                 //
  //   only if 1 chamber is analysed and filling is requested    //
  //------------------------------------------------------------//
  if (stationsInRegion.size() ==1 && m_fillHistos ) 
    m_MdtDqa_file_name +="_"+stationsInRegion.at(0).regionId(); 

  //-----------------------------------------------------------//
  //-- Create Histogram Manager,  Root Files and Book Histos --//
  //-----------------------------------------------------------//

  // m_histoManager = new HistogramManager();

  // use HistogramManager constructor with MdtIdHelper to retrieve the chamber geometry
  m_histoManager = new HistogramManager(m_mdtIdHelper);
  m_histoManager->SetDoTracks(m_doTracks);

  string mdtDqaRootFileName = m_MdtDqa_file_name+".root";
  if (m_fillHistos || m_doTracks) {
    if(!m_histoManager->openOutputFile(mdtDqaRootFileName)) return StatusCode::FAILURE;
  }

  if(m_doTracks) m_histoManager->buildTrackHistos();

  if (m_fillHistos) {
    // 
    // loop over REGIONS and Book TopLevel histograms :
    // 
    // here below we should replace the use of SectorMin[i][j] with : SectorMinMax[i][j][0,1]
    //
    m_histoManager->buildGlobalHistos();
    for (int i=0;i<=1;i++) {
      for (int j=0;j<=1;j++) {
	int secMin = SectorMin[i][j];
	int secMax = SectorMax[i][j];
	if (secMin>0&&secMin<=16 && secMax>0&&secMax<=16 && secMin<=secMax ) {
	  m_histoManager->buildTopLevel(spectrometerRegion[i],spectrometerSide[j], secMin, secMax);
	  //	  std::cout<<" Top Level Histograms histogram built for region :"
	  //		   <<spectrometerRegion[i] <<" Side : " << spectrometerSide[j]
	  //		   <<"Sector min : "<< secMin<< "Sector max : "<< secMax<< std::endl;
	}
      }
    }
    // 
    // loop over stations and Book Chamber  histograms :
    // 
    for (itstation = stationsInRegion.begin(); itstation!=stationsInRegion.end(); itstation++) {
      //             int stationNameId = itstation->GetStation();
      string stationName = itstation->regionId();
      string chamberType = stationName.substr(0,3);
      int phi = itstation->GetPhi();
      int eta = itstation->GetEta();
      
      MDTName chamb(chamberType,phi,eta);
      string region = chamb.getRegion();
      string side = chamb.getSide();
      //             int sector = chamb.getOnlineSector(); 
    
      //      std::cout<<" Chamber Histograms building : "<<chamb.getOnlineName()<<std::endl;
      m_histoManager->buildChamberHistos(chamb);
    }
  }

  // put a protection if file is not open!
  if (!(m_fillHistos || m_doTracks)) {
    if (!m_histoManager->openUpdateFile(mdtDqaRootFileName)) return StatusCode::FAILURE;
  }

  //------------------------------------------------------//
  //-- Create Analysis Tools                            --//
  //------------------------------------------------------//

  m_ntupleAna = new MdtDqaNtupleAnalysis(m_verbose, m_MdtDqa_file_name);
  if(!m_ntupleAna->initialize(p_reg_sel_svc, m_histoManager, m_DeadElementsAlgorithm, m_ADCCUT).isSuccess()) {
    return StatusCode::FAILURE;
  }

  m_segmentAnalyzer=new SegmentAnalysis(p_reg_sel_svc,m_histoManager,m_verbose,m_doHitResids);

  if (m_doEfficiency) {
    m_tubeEffi = new MdtDqaTubeEfficiency(m_EffiNSigma, m_EffiChi2Cut, 
					  m_EffiUseDefaultResolution, m_EffiHitADCCut, m_EffiGTFitON,
					  m_EffiUseNewCalibConstants, m_EffiUseTimeCorrections);
    if(!m_tubeEffi->initialize(m_mdtIdHelper, m_detMgr, m_id_tool, p_reg_sel_svc, p_calib_input_svc, m_histoManager).isSuccess()) return StatusCode::FAILURE;
  }

  if (m_doGlobalTimeFit) {
    // m_globalTimeFit = new MdtDqaGlobalTimeFit(4,10,50.,false);
    m_globalTimeFit = new MdtDqaGlobalTimeFit(m_GTFitSeg_minNumHits,m_GTFitSeg_maxNumHits,m_GTFitSeg_chi2Cut,
					      m_rtDefaultBfieldON, m_GTFitDebug);
    if(!m_globalTimeFit->initialize(m_mdtIdHelper, m_detMgr, m_id_tool, p_reg_sel_svc, m_histoManager).isSuccess()) return StatusCode::FAILURE;
  }
  //FOR THE TRACK ANALYSIS
  if (m_doTracks) {
    m_trackAnalyzer=new MdtDqaTrackAnalysis(p_reg_sel_svc,m_TrkAuthor, m_histoManager,m_verbose);
  }
  return StatusCode::SUCCESS;
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode NtupleMdtDqaTool::finalize(void) {

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Finalizing NtupleMdtDqaTool" << endmsg;

     if (m_doFinalize) {
        log << MSG::INFO << "Executing MdtDqaNtupleAnalysis::histogramAnalysis " << endmsg;
        m_ntupleAna->histogramAnalysis(m_histoManager->rootFile());
     } 

    m_histoManager->WriteAndCloseFile();
    return StatusCode::SUCCESS;
}  //end NtupleMdtDqaTool::initialize

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD handleEvent ::
//::::::::::::::::::::::::
StatusCode NtupleMdtDqaTool::handleEvent( const MuonCalibEvent &event, 
					  int eventnumber,
					  const std::vector<MuonCalibSegment *> &segments, 
					  unsigned int position) {
     
  if ( m_fillHistos)    m_ntupleAna->handleEvent(event, eventnumber,segments,position);
  if ( m_fillHistos)    m_segmentAnalyzer->handleEvent(event, eventnumber, segments, position);
  if ( m_doEfficiency)  
    if(!m_tubeEffi->handleEvent(event, eventnumber, segments, position).isSuccess()) return StatusCode::FAILURE;
  if ( m_doGlobalTimeFit) 
    if(!m_globalTimeFit->handleEvent(event, eventnumber, segments, position).isSuccess()) return StatusCode::FAILURE;
  if (m_doTracks) { 
    m_trackAnalyzer->handleEvent(event, eventnumber, segments, position);
  }
  return StatusCode::SUCCESS;
}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD analyseSegments ::
//::::::::::::::::::::::::::::
StatusCode NtupleMdtDqaTool::analyseSegments(const std::vector<MuonCalibSegment *> &/*segments*/) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "NtupleMdtDqaTool::analyseSegments ...it does nothing analysis moved elsewhere" 
      << endmsg;
  // if (m_doFinalize && m_doEfficiency ) {
  //    log << MSG::INFO << "Executing MdtDqaTubeEfficiency::analyseSegments" << endmsg;
  //    m_tubeEffi->analyseSegments(segments);
  // }
  return StatusCode::SUCCESS;
}

}  //namespace MuonCalib
