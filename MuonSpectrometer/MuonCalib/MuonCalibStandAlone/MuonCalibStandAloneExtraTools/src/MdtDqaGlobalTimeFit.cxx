/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.09.2008, AUTHOR: MAURO IODICE
// 20.03.2009, MODIFICATIONS: DAN LEVIN  ==> allow endcap
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS MdtDqaGlobalTimeFit      ::
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
// #include "GaudiKernel/MsgStream.h"
// #include "StoreGate/StoreGateSvc.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"

#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

// Rt stuff   ------
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibUtils/GlobalTimeFitter.h"

// #include "MdtCalibInterfaces/IMdtSegmentFitter.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtFullInfo.h"
#include "MdtCalibData/MdtCalibrationFactory.h"

// #include "MuonCalibStl/ToString.h"
#include "MdtCalibFitters/LocalSegmentResolver.h"

//MdtCalibFitters
// #include "MdtCalibFitters/MuCCaFitter.h"  // OBSOLETE
#include "MdtCalibFitters/DCSLFitter.h"


// -------


// MdtDqaGlobalTimeFit      //
#include "MuonCalibStandAloneExtraTools/MdtDqaGlobalTimeFit.h"

//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

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

MdtDqaGlobalTimeFit::MdtDqaGlobalTimeFit(int nbHitsMin, int nbHitsMax, float chi2cut, int BfieldON,  bool debug) :
  m_mdtIdHelper(NULL), m_detMgr(NULL), m_id_tool(NULL), p_reg_sel_svc(NULL), m_histoManager(NULL),
  //m_tfile(NULL),
  //m_tfile_debug(NULL), m_hit_ntuple(NULL), m_nb_hits(-1.), m_cal_region(NULL),
  m_rtRel(NULL),
  //m_qfitter(NULL),
  m_muFitter(NULL), m_GTFitter(NULL)
  //m_nb_trigger(-1), m_nb_stations(-1),
  //m_htoffset(NULL)
{
  m_minNumHits = nbHitsMin;
  m_maxNumHits = nbHitsMax;
  m_chi2_cut = chi2cut;
  m_BfieldON = BfieldON;
  m_debug = debug;
}
  
//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
  
StatusCode MdtDqaGlobalTimeFit::initialize(const MdtIdHelper* mdtIdHelper, const MuonGM::MuonDetectorManager *detMgr, 
					   const MuonCalib::IIdToFixedIdTool *id_tool, RegionSelectionSvc *reg_sel_svc,
					   HistogramManager *histoManager) {


  // m_BfieldON = 0;   // THIS PARAMETER MUST BE SET VIA jobOptions 
  //  cout << " MdtDqaGlobalTimeFit::initialize ... " <<endl;
  m_mdtIdHelper = mdtIdHelper;
  m_detMgr = detMgr; 
  m_id_tool = id_tool;
  p_reg_sel_svc = reg_sel_svc;
  m_histoManager = histoManager;

  // m_muFitter = new MuCCaFitter();   // OBSOLETE
  m_muFitter = new DCSLFitter();

  m_rtRel = getDefaultRtRelation(m_BfieldON);
  m_GTFitter = new GlobalTimeFitter(m_muFitter, m_rtRel);

  /*
    ToString ts;

    string RegionName = p_reg_sel_svc->GetRegionSelection();
    cout << " MdtDqaGlobalTimeFit::initialize - RegionName: "<<RegionName<< endl;

    const std::vector<MuonCalib::NtupleStationId> stationsInRegion = p_reg_sel_svc->GetStationsInRegions();

    //----------------------------------//
    //-- Create Root Files and Histos --//
    //----------------------------------//

    m_qfitter = NULL;
    
    m_nb_trigger = 0;

    // loop over stations in region
    m_nb_stations = stationsInRegion.size();
    for (int istation=0;istation<m_nb_stations;istation++) {
    for (int k=0;k<4;k++) m_nb_layers_tubes[istation][k] = -1;
    }
    cout << " end of first for loop " << endl;

    //
    std::vector<MuonCalib::NtupleStationId>::const_iterator itstation;
    for (itstation = stationsInRegion.begin();
    itstation!=stationsInRegion.end(); itstation++) {
    cout << "in the loop. itest = "<<itest++ << endl;
    string stationNameString = (itstation)->regionId();
    cout << " stationName : "<< stationNameString << endl; 
    }
   

    for (int istation=0;istation<m_nb_stations;istation++) {
    string stationNameString = stationsInRegion.at(istation).regionId();
    // cout << " initializing vector m_nb_layers_tubes. istation : "<< istation
    //     << " stationName: "<< stationNameString << endl; 
    string chamberType = stationNameString.substr(0,3);
    int phi_id = stationsInRegion.at(istation).GetPhi();
    int eta_id = stationsInRegion.at(istation).GetEta();

    // string fullStationName = chamberType+"_"+ts(phi_id)+"_"+ts(eta_id);
    Identifier station_id = m_mdtIdHelper->elementID(chamberType, eta_id, phi_id);
    int stationIntId = (int) station_id;
    int numberOfML = m_mdtIdHelper->numberOfMultilayers(station_id);

    for (int multilayer=1;multilayer<=numberOfML; multilayer++) {
    Identifier MdtML = m_mdtIdHelper->multilayerID(station_id, multilayer);
    int layerMin = m_mdtIdHelper->tubeLayerMin(MdtML);
    int layerMax = m_mdtIdHelper->tubeLayerMax(MdtML);
    int tubeMin = m_mdtIdHelper->tubeMin(MdtML);
    int tubeMax = m_mdtIdHelper->tubeMax(MdtML);
    m_nb_layers_tubes[istation][0] = stationIntId;
    m_nb_layers_tubes[istation][1] = layerMax-layerMin+1;
    m_nb_layers_tubes[istation][1+multilayer] = tubeMax-tubeMin+1;
    }
    } //end loop on stations
  */
  return StatusCode::SUCCESS;
}  //end MdtDqaGlobalTimeFit::initialize

//*****************************************************************************

  //:::::::::::::::::::::
  //:: METHOD finalize ::
  //:::::::::::::::::::::  
StatusCode MdtDqaGlobalTimeFit::finalize() {
    
  /****
       Here I have removed everything : 
       I don't know why the m_mdtIdHelper here gets corrupted!!!
  ****/
  
  return StatusCode::SUCCESS;
}

//*****************************************************************************

  //::::::::::::::::::::::::
  //:: METHOD handleEvent ::
  //::::::::::::::::::::::::

StatusCode MdtDqaGlobalTimeFit::handleEvent( const MuonCalibEvent & /*event*/,
					     int /*eventnumber*/,
					     const std::vector<MuonCalibSegment *> &segments, 
					     unsigned int position) {

  //  if ( m_debug ) cout << " MdtDqaGlobalTimeFit::handleEvent ... " <<endl;
  //  if ( m_debug ) cout << " MdtDqaGlobalTimeFit:: segments.size() and position: "<<segments.size()<<" "<<position <<endl;
  
  if (segments.size()<=position) return StatusCode::SUCCESS;
  // MuonCalibSegment segment(*segments[position]);
  for (unsigned int k=position; k<segments.size(); k++) {    // LOOP OVER SEGMENTS 
    
    MuonCalibSegment segment(*segments[k]);
 
    //---------------//
    //-- Variables --//
    //---------------//
    
    // int segAuthor = segment.author();
    // cout << "DEBUG DEBUG - segment.author() = " <<segAuthor;
    int nhits = (int) segment.mdtHitsOnTrack();
    bool REFIT_GTFIT = true;    // THIS SHOULD BE PASSED VIA JOBOPTIONS !!!!!!!!!!!!!    
    // station identifiers //
  
    MuonFixedId Mid((segment.mdtHOT()[0])->identify());
     
    //this has to be set in order to get the m_detMgr->getMdtReadoutElement() method working correctly
    //otherwise also for the first multilayer, the second is returned
    Mid.setMdtMultilayer(1);

    //----------//
    //-- Hits --//
    //----------//
    
    //    if ( m_debug ) cout << " MdtDqaGlobalTimeFit:: segments num of hits: "<<nhits <<endl;
    if(nhits<m_minNumHits || nhits>m_maxNumHits) continue;

    //
    // Get REGION and STATION of the segment :
    // 
    // and access to MdtDqa histograms 
    //
    // TFile * mdtDqaRoot =  m_histoManager->rootFile();
    ToString ts;
    int stationNameId = Mid.stationName();
    int phi  = Mid.phi();
    int eta  = Mid.eta();
    int absEta = eta;
    if (eta<0) absEta=-eta;
    string stationNameStr = Mid.stationNameString();

    //
    // HERE WE LIMIT THE ANALYSIS TO BM(L,S) and EM(L,S) CHAMBERS : 
    //
    if ( !(stationNameStr.substr(0,2)=="BM" || stationNameStr.substr(0,2)=="EM") ) continue;

    //    if ( m_debug ) cout << " MdtDqaGlobalTimeFit:: stationNameStr: "<<stationNameStr <<endl;

    //int stationIntId = (int) m_mdtIdHelper->elementID(stationNameStr,eta,phi);

    string region = "Barrel";
    if ( stationNameStr.substr(0,1) == "E" ) region = "Endcap";
    string side = "A";
    if (eta<0) side = "C";

    PhiEtaNameConverter phiEtaConverter;
    string chamberType = stationNameStr;

    string fullStationName = chamberType+"_"+ts(phi)+"_"+ts(eta);
    //    if (m_debug) cout << " MdtDqaGlobalTimeFit on " << fullStationName << endl;

    int sector = phiEtaConverter.phi_8to16(stationNameId,phi);

    // DEBUG DEBUG DEBUG 
    // DEBUG DEBUG DEBUG 
    // DEBUG DEBUG DEBUG 
    // double r;
    // double dr;
    // double t;
    
    /* MARCH 27, 2009 I comment this ! M.I.
       for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
       it!=segment.mdtHOTEnd();++it) {
        
       t=(*it)->driftTime();
       r = (*it)->driftRadius(); // THIS IS A PATCH !!! ONE HAS TO PUT THE rt !!!!
       //float rTEST = m_rtRel->radius(t);
       dr=(*it)->sigmaDriftRadius();
       (*it)->setDriftRadius(r,dr);
       // std::cout << "DEBUGDEBUG HIT  RawTDC "<< (*it)->tdcCount() 
       //          <<" RawTime "<<(*it)->tdcCount()*(25./32.)
       //          <<" Time "<<t<<" Radius "<<r<< "resol "
       //          <<dr <<" rTEST = "<< rTEST << std::endl;
       }
    */ // Commented March 27, 2009

    //----------------------//
    //-- GLOBAL TIME FIT  --//
    //----------------------//
    // What does actually the resolver do ?
    // from Dan Levin email of March 20, 2009 we can probably remove it!
    // 
    /* MARCH 27, 2009 I comment this ! M.I.
       LocalSegmentResolver resolver;
       resolver.resolve(&segment);
    */ // Commented March 27, 2009

    TH1F *h1;
    TH2F *h2;

    // DEBUG DIFFERENCES BETWEEN OLD AND NEW SEGMENTS :
    // 
    /*
      double chi2_old = segment.chi2();
      double dirY_old = segment.direction().y();
      double dirZ_old = segment.direction().z();
      double posY_old = segment.position().y();
      double posZ_old = segment.position().z();
      double aseg_old = -999.;
      double bseg_old = -999.;
      if(dirZ_old !=0. ) {
      aseg_old = dirY_old/dirZ_old; 
      bseg_old = posY_old - aseg_old*posZ_old;
      }
    */
    // 
    // DEBUG DIFFERENCES BETWEEN OLD AND NEW SEGMENTS :

    float toffset(999.);
    if ( REFIT_GTFIT ) toffset =m_GTFitter->GTFit(&segment);
    
    float SegChi2DoF = segment.chi2();
    int DoF = segment.mdtHitsOnTrack() - 2;
    if ( REFIT_GTFIT ) DoF = DoF-1;
    float SegChi2 = SegChi2DoF*((float)DoF);
    
    // DEBUG DIFFERENCES BETWEEN OLD AND NEW SEGMENTS :
    // 
    /*
      if ( REFIT_GTFIT ) {
      double chi2_new = segment.chi2();
      double dirY_new = segment.direction().y();
      double dirZ_new = segment.direction().z();
      double posY_new = segment.position().y();
      double posZ_new = segment.position().z();
      double aseg_new = -999.;
      double bseg_new = -999.;
      if(dirZ_new !=0. ) {
      aseg_new = dirY_new/dirZ_new; 
      bseg_new = posY_new - aseg_new*posZ_new;
      }

      float abs_a_old = aseg_old;
      if (aseg_old<0) abs_a_old = -aseg_old;
      float abs_a_new = aseg_new;
      if (aseg_new<0) abs_a_new = -aseg_new;
      if ( abs_a_old<0.05 && abs_a_new>0.3 ) {
      std::cout << " Event with 0 slope: " <<event.eventInfo().eventNumber()<<std::endl;
      std::cout << " slope_old , slope_new : " << aseg_old << " "<< aseg_new <<std::endl;
      }

      string debugHisto;
      debugHisto = "a_old_Vs_a_new";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(debugHisto,region,side,sector);
      if (h2) h2->Fill(aseg_new,aseg_old);

      debugHisto = "b_old_Vs_b_new";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(debugHisto,region,side,sector);
      if (h2) h2->Fill(bseg_new,bseg_old);

      debugHisto = "a_diff";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(debugHisto,region,side,sector);
      if (h1) h1->Fill(aseg_new-aseg_old);

      debugHisto = "b_diff";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(debugHisto,region,side,sector);
      if (h1) h1->Fill(bseg_new-bseg_old);
      } 
    */
    // 
    // DEBUG DIFFERENCES BETWEEN OLD AND NEW SEGMENTS :
    
    //    if ( m_debug ) cout << " MdtDqaGlobalTimeFit:: segmentChi2perDoF: "<<SegChi2DoF <<endl;

    if (SegChi2DoF<=m_chi2_cut){ // chi2DoF cut
      //        if (m_debug) cout << "MdtDqaGlobalTimeFit handling segment with chi2, DoF and chi2/Dof : "
      //                          << SegChi2 <<" "<< DoF <<" "<< SegChi2DoF << endl;
      // get MDT coordinates from first hit 
      // (this can be changed in future by averaging over the hit for example)

      // std::vector<MdtCalibHitBase*>::iterator firstHit = segment.mdtHOTBegin();
 
      // TRY THE FOLLOWING 
      float distRO = (segment.mdtHOT()[0])->distanceToReadout();  // along the tube
      float tube   = (float) ((segment.mdtHOT()[0])->identify()).mdtTube();
      //const HepGeom::Vector3D<double>& localPosHit=(segment.mdtHOT()[0])->localPosition();
      //float localX = localPosHit.x(); // along the tube
      //float localZ = localPosHit.z(); // follows tube numbering


      // DEBUG DEBUG :
      // std::cout << " DEBUGDEBUG firstHit coordinates : distRO "<<distRO
      //           << " tube "<<tube<<" localX "<<localX<<" localZ "<<localZ<<std::endl;
      //
      // FILLING Histograms 
      //
      string histoType;

      string station = "undefined";
      if ( region == "Barrel" )station = "BM";
      if ( region == "Endcap" )station = "EM";

      histoType="TrigTime_"+station+"_eta_"+ts(absEta);
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h1) {
	h1->Fill(toffset);
      } 
      // else {
      //   cout << "DEBUGDEBUG histo not found in dir "
      //       <<m_histoManager->GetTDaqDirectoryName(region, side,sector) <<endl;}

      //       cout << " TEST TEST HISTTITLE = " << histoType << endl;

      histoType="TrigTimeVsROdistance_"+station+"_eta_"+ts(absEta);

      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(distRO,toffset);

      histoType="TriggerCoverage_"+station+"_eta_"+ts(absEta);
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(distRO,tube);

      histoType="Segment_chi2";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(SegChi2DoF,absEta);

      histoType="Segment_ProbChi2";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h1) h1->Fill(TMath::Prob(SegChi2,DoF));

      histoType="HitsOnSegment";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      if (h2) h2->Fill(segment.mdtHitsOnTrack(),absEta);

      histoType="HitRadius";
      h1 = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {

	// const HepGeom::Point3D<double>& localClosestApproach = (*it)->localPointOfClosestApproach();
	// float localCloApZ = localClosestApproach.z(); // follows tube numbering
	// const HepGeom::Point3D<double>& localTubeWirePosition = (*it)->localPosition();
	// float localTubeWirePositionZ = localTubeWirePosition.z(); // follows tube numbering
	float radius=(*it)->driftRadius();
	//cout << " DEBUG closestApproach " <<localCloApZ<<" tubePosition "<<localTubeWirePositionZ<<endl;
	//cout << " DEBUG signedDistanceToTrack "<<(*it)->signedDistanceToTrack() 
	//     << " radius "<<radius<<endl;

	if ((*it)->signedDistanceToTrack()<0.) radius=-radius;
	if (h1) h1->Fill(radius);
      }
      
      histoType="Residuals";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {
	float resid=(*it)->radialResidual();
	if (h2) h2->Fill(resid,absEta);
      }

      histoType="ResidualsVsRadius";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) {
	float resid=(*it)->radialResidual();
	float radius=(*it)->driftRadius();
	if ((*it)->signedDistanceToTrack()<0.) radius=-radius;
	if (h2) h2->Fill(radius,resid);
      }
/* IMPLEMENTED BY NIR but REMOVED by himself afterward AT the moment we leave it commented
      // Do the hit residuals calculation:
      histoType="Hit_Residuals";
      h2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);
      histoType="Hit_Res_Chi2DoF";
      TH2F* h2_2 = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,sector);

      IMdtSegmentFitter::HitSelection ref_hitsel(segment.mdtHitsOnTrack(), 0); // Reference hit selection, containing 0 for all hits in segment
      for (std::vector<MdtCalibHitBase*>::iterator it =segment.mdtHOTBegin();
	   it!=segment.mdtHOTEnd();++it) { // Loop on all hits on tracks
	unsigned int                    iHit   = it-segment.mdtHOTBegin();
	IMdtSegmentFitter::HitSelection hitsel = ref_hitsel;
	// Ignore hit iHit;
	hitsel[iHit]                           = 1; 
	// Clone segment:
	MuonCalibSegment segmentClone(segment);
	// Printing hitSelections:
	//  	 cout<<"Fitting segment with hitSelection: [ ";
	//  	 for (IMdtSegmentFitter::HitSelection::const_iterator selIt = hitsel.begin(); 
	//  	      selIt != hitsel.end(); selIt++) { // printing the hit selection
	//  	   cout << *selIt << " ";
	//  	 }
	//  	 cout<<"]\n";
	// Setment fit:
	m_GTFitter->fit(segmentClone, hitsel);

	//Get the residual of the ignored hit:
	float orig_resid    = segment.mdtHOT()[iHit]->radialResidual(); // unsigned residual
	float resid         = segmentClone.mdtHOT()[iHit]->radialResidual(); // unsigned residual
	float orig_dToTrack = segment.mdtHOT()[iHit]->signedDistanceToTrack(); // distance to track
	float dToTrack      = segmentClone.mdtHOT()[iHit]->signedDistanceToTrack(); // distance to track
	float orig_driftR   = segment.mdtHOT()[iHit]->driftRadius(); // drift Radius
	float driftR        = segmentClone.mdtHOT()[iHit]->driftRadius(); // drift Radius
	float chi2DoF       = segmentClone.chi2(); // segment chi2/dof

	//	  printf("orig_dToTrack = %6.2f dToTrack = %6.2f orig_driftR = %6.2f driftR = %6.2f orig_resid = %6.2f resid = %6.2f   orig_chi2 = %6.2f new_chi2 = %6.2f   n = %d\n",
	// orig_dToTrack, dToTrack, orig_driftR, driftR, orig_resid, resid, segment.chi2(), segmentClone.chi2(), (int) segment.mdtHitsOnTrack());

	if (h2) h2->Fill(resid,absEta);
	if (h2_2) h2_2->Fill(chi2DoF, absEta);
      } // End loop over hits
*/

    } // End if over chi2DoF
  } // end LOOP OVER SEGMENTS

  return StatusCode::SUCCESS;
}  //end MdtDqaGlobalTimeFit::handleEvent

//*****************************************************************************

  //::::::::::::::::::::::::::::
  //:: METHOD analyseSegments ::
  //::::::::::::::::::::::::::::
StatusCode MdtDqaGlobalTimeFit::analyseSegments(const std::vector<MuonCalibSegment *> & /*segments*/) {
  return StatusCode::SUCCESS;
}

IRtRelation *MdtDqaGlobalTimeFit::getDefaultRtRelation(int BfieldON ) {
  MuonCalib::CalibFunc::ParVec rtPars;
  rtPars.push_back( -50. );   // t_Start 
  rtPars.push_back( 8. );     // t_binSize

   // B field is OFF :
  if ( BfieldON == 0) {       
   /*
   rtPars.push_back(0.2 );    // r(-50)
   rtPars.push_back(0.2 );    // r(-50 + 8)
   rtPars.push_back(0.2 );    // ....
   rtPars.push_back(0.2 );
   rtPars.push_back(0.22);
   rtPars.push_back(0.25);
   rtPars.push_back(0.3 );
   rtPars.push_back(0.4 );
   rtPars.push_back(0.7 );
   rtPars.push_back(1.106 );
   rtPars.push_back(1.506 );
   rtPars.push_back(1.898 );
   rtPars.push_back(2.283 );
   rtPars.push_back(2.658 );
   rtPars.push_back(3.024 );
   rtPars.push_back(3.38  );
   rtPars.push_back(3.725 );
   rtPars.push_back(4.058 );
   rtPars.push_back(4.381 );
   rtPars.push_back(4.693 );
   rtPars.push_back(4.993 );
   rtPars.push_back(5.281 );
   rtPars.push_back(5.559 );
   rtPars.push_back(5.826 );
   rtPars.push_back(6.082 );
   rtPars.push_back(6.327 );
   rtPars.push_back(6.563 );
   rtPars.push_back(6.788 );
   rtPars.push_back(7.005 );
   rtPars.push_back(7.213 );
   rtPars.push_back(7.412 );
   rtPars.push_back(7.603 );
   rtPars.push_back(7.787 );
   rtPars.push_back(7.964 );
   rtPars.push_back(8.135 );
   rtPars.push_back(8.299 );
   rtPars.push_back(8.458 );
   rtPars.push_back(8.613 );
   rtPars.push_back(8.762 );
   rtPars.push_back(8.908 );
   rtPars.push_back(9.05  );
   rtPars.push_back(9.189 );
   rtPars.push_back(9.325 );
   rtPars.push_back(9.458 );
   rtPars.push_back(9.59  );
   rtPars.push_back(9.719 );
   rtPars.push_back(9.847 );
   rtPars.push_back(9.974 );
   rtPars.push_back(10.099 );
   rtPars.push_back(10.224 );
   rtPars.push_back(10.348 );
   rtPars.push_back(10.471 );
   rtPars.push_back(10.593 );
   rtPars.push_back(10.715 );
   rtPars.push_back(10.837 );
   rtPars.push_back(10.958 );
   rtPars.push_back(11.078 );
   rtPars.push_back(11.198 );
   rtPars.push_back(11.317 );
   rtPars.push_back(11.435 );
   rtPars.push_back(11.552 );
   rtPars.push_back(11.668 );
   rtPars.push_back(11.783 );
   rtPars.push_back(11.896 );
   rtPars.push_back(12.008 );
   rtPars.push_back(12.118 );
   rtPars.push_back(12.226 );
   rtPars.push_back(12.332 );
   rtPars.push_back(12.435 );
   rtPars.push_back(12.536 );
   rtPars.push_back(12.635 );
   rtPars.push_back(12.731 );
   rtPars.push_back(12.824 );
   rtPars.push_back(12.915 );
   rtPars.push_back(13.003 );
   rtPars.push_back(13.088 );
   rtPars.push_back(13.17  );
   rtPars.push_back(13.251 );
   rtPars.push_back(13.329 );
   rtPars.push_back(13.406 );
   rtPars.push_back(13.481 );
   rtPars.push_back(13.556 );
   rtPars.push_back(13.64  );
   rtPars.push_back(13.74  );
   rtPars.push_back(13.83  );
   rtPars.push_back(13.9   );
   rtPars.push_back(14.0  );
   rtPars.push_back(14.08  );
   rtPars.push_back(14.18  );
   rtPars.push_back(14.27  );
   rtPars.push_back(14.37  );
   rtPars.push_back(14.45  );
   rtPars.push_back(14.55  );
   rtPars.push_back(14.62  );
   rtPars.push_back(14.7   );
   rtPars.push_back(14.75  );
   rtPars.push_back(14.8   );
   rtPars.push_back(14.82  );
   rtPars.push_back(14.84  );
   rtPars.push_back(14.86  );
   */
    rtPars.push_back(0.2);        // r(-50)
    rtPars.push_back(0.2);          // r(-50+8)
    rtPars.push_back(0.2);          // ...
    rtPars.push_back(0.25);
    rtPars.push_back(0.294403);
    rtPars.push_back(0.311703);
    rtPars.push_back(0.485873);
    rtPars.push_back(0.804235);
    rtPars.push_back(1.19624);
    rtPars.push_back(1.5562);
    rtPars.push_back(1.91466);
    rtPars.push_back(2.3147);
    rtPars.push_back(2.66996);
    rtPars.push_back(3.05436);
    rtPars.push_back(3.40371);
    rtPars.push_back(3.80574);
    rtPars.push_back(4.13506);
    rtPars.push_back(4.45473);
    rtPars.push_back(4.77338);
    rtPars.push_back(5.05294);
    rtPars.push_back(5.33755);
    rtPars.push_back(5.58459);
    rtPars.push_back(5.8718);
    rtPars.push_back(6.09389);
    rtPars.push_back(6.37129);
    rtPars.push_back(6.56824);
    rtPars.push_back(6.73359);
    rtPars.push_back(6.96923);
    rtPars.push_back(7.16944);
    rtPars.push_back(7.38872);
    rtPars.push_back(7.55627);
    rtPars.push_back(7.73885);
    rtPars.push_back(7.86403);
    rtPars.push_back(8.07223);
    rtPars.push_back(8.26818);
    rtPars.push_back(8.39531);
    rtPars.push_back(8.54069);
    rtPars.push_back(8.69377);
    rtPars.push_back(8.86487);
    rtPars.push_back(9.01724);
    rtPars.push_back(9.16516);
    rtPars.push_back(9.24791);
    rtPars.push_back(9.45596);
    rtPars.push_back(9.56237);
    rtPars.push_back(9.69553);
    rtPars.push_back(9.80647);
    rtPars.push_back(9.9432);
    rtPars.push_back(10.039);
    rtPars.push_back(10.207);
    rtPars.push_back(10.2884);
    rtPars.push_back(10.3977);
    rtPars.push_back(10.6045);
    rtPars.push_back(10.6181);
    rtPars.push_back(10.7883);
    rtPars.push_back(10.9057);
    rtPars.push_back(10.9908);
    rtPars.push_back(11.1615);
    rtPars.push_back(11.2359);
    rtPars.push_back(11.3409);
    rtPars.push_back(11.4451);
    rtPars.push_back(11.5412);
    rtPars.push_back(11.6896);
    rtPars.push_back(11.7487);
    rtPars.push_back(11.8853);
    rtPars.push_back(11.9571);
    rtPars.push_back(12.0381);
    rtPars.push_back(12.1739);
    rtPars.push_back(12.2601);
    rtPars.push_back(12.3892);
    rtPars.push_back(12.4406);
    rtPars.push_back(12.5809);
    rtPars.push_back(12.6297);
    rtPars.push_back(12.7495);
    rtPars.push_back(12.8314);
    rtPars.push_back(12.9463);
    rtPars.push_back(13.0487);
    rtPars.push_back(13.1337);
    rtPars.push_back(13.2417);
    rtPars.push_back(13.2706);
    rtPars.push_back(13.3917);
    rtPars.push_back(13.5107);
    rtPars.push_back(13.60);
    rtPars.push_back(13.69);
    rtPars.push_back(13.71);
    rtPars.push_back(13.8088);
    rtPars.push_back(13.8577);
    rtPars.push_back(14.0212);
    rtPars.push_back(14.0671);
    rtPars.push_back(14.1225);
    rtPars.push_back(14.2112);
    rtPars.push_back(14.3113);
    rtPars.push_back(14.3747);
    rtPars.push_back(14.3531);
    rtPars.push_back(14.4799);
    rtPars.push_back(14.458);
    rtPars.push_back(14.516);
    rtPars.push_back(14.8312);
    rtPars.push_back(14.85);
    rtPars.push_back(14.86);
    rtPars.push_back(14.87);
    
  }
  // B field is ON :
  if ( BfieldON == 1) {   
    rtPars.push_back(0.2);
    rtPars.push_back(0.2);
    rtPars.push_back(0.2);
    rtPars.push_back(0.22);
    rtPars.push_back(0.27);
    rtPars.push_back(0.33);
    rtPars.push_back(0.37);
    rtPars.push_back(0.460902);
    rtPars.push_back(0.805728);
    rtPars.push_back(1.10767);
    rtPars.push_back(1.53996);
    rtPars.push_back(1.9101);
    rtPars.push_back(2.36668);
    rtPars.push_back(2.71068);
    rtPars.push_back(3.06689);
    rtPars.push_back(3.41867);
    rtPars.push_back(3.78364);
    rtPars.push_back(4.11741);
    rtPars.push_back(4.41084);
    rtPars.push_back(4.73491);
    rtPars.push_back(5.02635);
    rtPars.push_back(5.24373);
    rtPars.push_back(5.5519);
    rtPars.push_back(5.80729);
    rtPars.push_back(5.98834);
    rtPars.push_back(6.26835);
    rtPars.push_back(6.42993);
    rtPars.push_back(6.62648);
    rtPars.push_back(6.85403);
    rtPars.push_back(6.99663);
    rtPars.push_back(7.21787);
    rtPars.push_back(7.32455);
    rtPars.push_back(7.64958);
    rtPars.push_back(7.73685);
    rtPars.push_back(7.90371);
    rtPars.push_back(8.0533);
    rtPars.push_back(8.18414);
    rtPars.push_back(8.44938);
    rtPars.push_back(8.60877);
    rtPars.push_back(8.65394);
    rtPars.push_back(8.84461);
    rtPars.push_back(8.95214);
    rtPars.push_back(9.14024);
    rtPars.push_back(9.24901);
    rtPars.push_back(9.37823);
    rtPars.push_back(9.5724);
    rtPars.push_back(9.60793);
    rtPars.push_back(9.74432);
    rtPars.push_back(9.89992);
    rtPars.push_back(10.0009);
    rtPars.push_back(10.1321);
    rtPars.push_back(10.187);
    rtPars.push_back(10.3244);
    rtPars.push_back(10.4858);
    rtPars.push_back(10.6038);
    rtPars.push_back(10.7109);
    rtPars.push_back(10.8385);
    rtPars.push_back(10.8563);
    rtPars.push_back(10.977);
    rtPars.push_back(11.0865);
    rtPars.push_back(11.2767);
    rtPars.push_back(11.3279);
    rtPars.push_back(11.4734);
    rtPars.push_back(11.5325);
    rtPars.push_back(11.6352);
    rtPars.push_back(11.793);
    rtPars.push_back(11.8545);
    rtPars.push_back(11.9321);
    rtPars.push_back(12.0367);
    rtPars.push_back(12.091);
    rtPars.push_back(12.1884);
    rtPars.push_back(12.3246);
    rtPars.push_back(12.3793);
    rtPars.push_back(12.5348);
    rtPars.push_back(12.6328);
    rtPars.push_back(12.7038);
    rtPars.push_back(12.7699);
    rtPars.push_back(12.9211);
    rtPars.push_back(12.9866);
    rtPars.push_back(13.0501);
    rtPars.push_back(13.1832);
    rtPars.push_back(13.2441);
    rtPars.push_back(13.2971);
    rtPars.push_back(13.4107);
    rtPars.push_back(13.562);
    rtPars.push_back(13.5098);
    rtPars.push_back(13.6794);
    rtPars.push_back(13.7281);
    rtPars.push_back(13.8953);
    rtPars.push_back(13.9238);
    rtPars.push_back(14.0338);
    rtPars.push_back(14.0898);
    rtPars.push_back(14.1931);
    rtPars.push_back(14.2 );
    rtPars.push_back(14.4053);
    rtPars.push_back(14.41 );
    rtPars.push_back(14.4481);
    rtPars.push_back(14.4606);
    rtPars.push_back(14.49);
    rtPars.push_back(14.5283);
  }    

  IRtRelation *rt = MuonCalib::MdtCalibrationFactory::createRtRelation( "RtRelationLookUp", rtPars );

  return rt;
}  //end MdtDqaGlobalTimeFit::getDefaultRtRelation

} // namespace MuonCalib 
