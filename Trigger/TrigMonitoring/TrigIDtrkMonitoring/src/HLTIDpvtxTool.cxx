/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDpvtxTool.cxx
 *
 *  Contact:
 *  @author Ben.Smart (AT cern DOT ch) 
 *
 */

#include "GaudiKernel/IJobOptionsSvc.h"
//#include "AthenaMonitoring/AthenaMonMaager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigIDtrkMonitoring/HLTIDpvtxTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
//#include "VxVertex/VxContainer.h"
//#include "VxVertex/VxCandidate.h"

#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include <vector>
#include <math.h>
#include <map>

#include "EventInfo/EventInfo.h"

// added by Ben
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

//using namespace HLTIDDump;
using namespace CLHEP;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDpvtxTool::HLTIDpvtxTool(const std::string & type, const std::string & name, const IInterface* parent)
  :IHLTMonTool(type, name, parent),
   m_TDT("Trig::TrigDecisionTool/TrigDecisionTool"),
   m_ChainName(""),
   m_pvtxContainer(""),
   m_effRangeX(0),
   m_effRangeY(0),
   m_effRangeZ(0),
   m_offline_pvtx_x(0),
   m_offline_pvtx_y(0),
   m_offline_pvtx_z(0),
   m_BeamSpot_x(0),
   m_BeamSpot_y(0),
   m_BeamSpot_z(0),
   m_BeamSpot_sigmaX(0),
   m_BeamSpot_sigmaY(0),
   m_BeamSpot_sigmaZ(0),
   m_online_pvtx_x(0),
   m_online_pvtx_y(0),
   m_online_pvtx_z(0),
   m_nTracksTotal(0),
   m_haveOfflinePV(false),
   m_haveOnlinePV(false)
{

  //+++ Trigger
  //declareProperty("TrigDecisionTool",    m_TDT, "The tool to access TrigDecision");
  declareProperty("ChainName",                m_ChainName); 
  declareProperty("OnlinePrimaryVertexContainerName",                m_pvtxContainer); 
  declareProperty("OnlineEfficiancyRangeCutX",                m_effRangeX); 
  declareProperty("OnlineEfficiancyRangeCutY",                m_effRangeY); 
  declareProperty("OnlineEfficiancyRangeCutZ",                m_effRangeZ); 

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDpvtxTool::~HLTIDpvtxTool() {
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDpvtxTool::init() {

  StatusCode sc = m_TDT.retrieve();
  if ( sc.isFailure()   ) {  
    ATH_MSG_FATAL("HLTIDpvtxTool FATAL: Could not retrieve TrigDecisionTool!");
    return sc;
  }

  m_online_efficiency_xyz.push_back(std::vector<int>(2,0));
  m_online_efficiency_xyz.push_back(std::vector<int>(2,0));
  m_online_efficiency_xyz.push_back(std::vector<int>(2,0));

  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTIDpvtxTool::book() {
#else
StatusCode HLTIDpvtxTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) { 
#endif


  ATH_MSG_VERBOSE(" ====== Begin book() ====== ");

  //+++ Standard histograms
  MonGroup * monGroup = new MonGroup(this, "HLT/IDpvtx", run);
  addMonGroup(monGroup);

  //  if (newRun || newLowStatInterval) {
  if (newRun) {

    //+++ Diagnostic Histograms
    addHistogram(new TH1F("L2Result_Checker", "Checks events that failed due to L2 truncated result", 2, -0.5, 1.5), "HLT/IDpvtx");
    addHistogram(new TH1F("EFResult_Checker", "Checks events that failed due to EF truncated result", 2, -0.5, 1.5), "HLT/IDpvtx");

    // Main Histograms
    addHistogram(new TH1F("Offline_n","Number of offline primary verticies per event",51,-0.5,50.05), "HLT/IDpvtx");

    addHistogram(new TH1F("Offline_x","Offline primary vertex x coordinate",25,-2,2), "HLT/IDpvtx");
    addHistogram(new TH1F("Offline_y","Offline primary vertex y coordinate",25,-2,2), "HLT/IDpvtx");
    addHistogram(new TH1F("Offline_z","Offline primary vertex z coordinate",25,-200,200), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_n","Number of online primary verticies per event",51,-0.5,50.05), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_x","Online primary vertex x coordinate",25,-2,2), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_y","Online primary vertex y coordinate",25,-2,2), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_z","Online primary vertex z coordinate",25,-200,200), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_nTracks","Number of online tracks per event",51,-0.5,50.05), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_x_residual","Online primary vertex x residual",25,-0.02,0.02), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_y_residual","Online primary vertex y residual",25,-0.02,0.02), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_z_residual","Online primary vertex z residual",25,-2,2), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_r_residual","Online primary vertex r residual",25,0,0.04), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_efficiency","Online primary vertex efficiency", 2,-0.5,1.5), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_efficiency_x","Online primary vertex efficiency in x", 2,-0.5,1.5), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_efficiency_y","Online primary vertex efficiency in y", 2,-0.5,1.5), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_efficiency_z","Online primary vertex efficiency in z", 2,-0.5,1.5), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_efficiency_nTracks","Online primary vertex efficiency against number of online tracks per event", 51,-0.5,50.05), "HLT/IDpvtx");

    addHistogram(new TH1F("Online_x_resolution","Online primary vertex x resolution against number of online tracks per event",51,-0.5,50.05), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_y_resolution","Online primary vertex y resolution against number of online tracks per event",51,-0.5,50.05), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_z_resolution","Online primary vertex z resolution against number of online tracks per event",51,-0.5,50.05), "HLT/IDpvtx");
    addHistogram(new TH1F("Online_r_resolution","Online primary vertex r resolution against number of online tracks per event",51,-0.5,50.05), "HLT/IDpvtx");

  }
  
  ATH_MSG_VERBOSE(" ====== End book() ====== ");
  
  return StatusCode::SUCCESS;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDpvtxTool::fill( ) {
  ATH_MSG_VERBOSE(" ====== Begin fill() ====== ");
  
  m_haveOfflinePV = false;
  m_haveOnlinePV = false;

  StatusCode sc = StatusCode::SUCCESS;

  /// Code to remove events with Truncated L2 results
  
  const DataHandle<TrigDec::TrigDecision> td;
  StatusCode sc2 = StatusCode::SUCCESS;
  sc2 = evtStore()->retrieve(td);
  if (sc2.isFailure()) {

  }
  
  bool Truncated_Result_L2 = 0;
  bool Truncated_Result_EF = 0;
  
  const HLT::HLTResult* r = (&(td->getL2Result()));
  if (r->isHLTResultTruncated()){
    hist("L2Result_Checker","HLT/IDpvtx")->Fill(1.0);
    Truncated_Result_L2 = 1;
  }
  else hist("L2Result_Checker","HLT/IDpvtx")->Fill(0.0);

  const HLT::HLTResult* rd = (&(td->getEFResult()));
  if (rd->isHLTResultTruncated()){
    hist("EFResult_Checker","HLT/IDpvtx")->Fill(1.0);
    Truncated_Result_EF = 1;
  }
  else hist("EFResult_Checker","HLT/IDpvtx")->Fill(0.0);


  ATH_MSG_DEBUG("Passed L2_b10_IDTrkNoCut = " << m_TDT->isPassed("L2_b10_IDTrkNoCut"));
  ATH_MSG_DEBUG("Passed L2_b10_j30_a4tc_EFFS_IDTrkNoCut = "
                << m_TDT->isPassed("L2_b10_j30_a4tc_EFFS_IDTrkNoCut"));
  ATH_MSG_DEBUG("Passed L2_b50_NoCut_j50_c4cchad = " << m_TDT->isPassed("L2_b50_NoCut_j50_c4cchad"));

  ATH_MSG_DEBUG("Passed EF_b10_IDTrkNoCut = " << m_TDT->isPassed("EF_b10_IDTrkNoCut"));
  ATH_MSG_DEBUG("Passed EF_b10_j30_a4tc_EFFS_IDTrkNoCut = "
                << m_TDT->isPassed("EF_b10_j30_a4tc_EFFS_IDTrkNoCut"));
  ATH_MSG_DEBUG("Passed EF_b55_NoCut_j55_a4tchad = "
                << m_TDT->isPassed("EF_b55_NoCut_j55_a4tchad"));


  if(!Truncated_Result_L2 && !Truncated_Result_EF && m_TDT->isPassed(m_ChainName) ) {

    // Offline

    std::string VxPrimaryCandidate = "VxPrimaryCandidate";
    const VxContainer* VertexCollection;
    StatusCode sc_offvtx = m_storeGate->retrieve(VertexCollection, VxPrimaryCandidate);

    if (sc_offvtx.isFailure()) {
      ATH_MSG_WARNING("Offline primary vertex collection was not retrieved from storegate!");
    }
    else {
      int n_offline_pvtx = 0;
      for(int i=0 ; i<(int)VertexCollection->size() ; i++){
	// must check type!
	int type = (VertexCollection->at(i))->vertexType();
	if(type==1){
	  m_offline_pvtx_x = (VertexCollection->at(i))->recVertex().position().x();
	  m_offline_pvtx_y = (VertexCollection->at(i))->recVertex().position().y();
	  m_offline_pvtx_z = (VertexCollection->at(i))->recVertex().position().z();

	  hist("Offline_x","HLT/IDpvtx")->Fill(m_offline_pvtx_x);
	  hist("Offline_y","HLT/IDpvtx")->Fill(m_offline_pvtx_y);
	  hist("Offline_z","HLT/IDpvtx")->Fill(m_offline_pvtx_z);

	  n_offline_pvtx++;

	  m_haveOfflinePV = true;
	}
      }
      hist("Offline_n","HLT/IDpvtx")->Fill(n_offline_pvtx);
    }


    // Offline Beam spot

    IBeamCondSvc* m_iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc); // ???
    //StatusCode sc = m_storeGate->retrieve(m_iBeamCondSvc, "BeamCondSvc"); // ???

    if (sc.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      ATH_MSG_WARNING("Could not retrieve Beam Conditions Service.");
    }
    else {
 
      //HepPoint3D m_beamSpot = m_iBeamCondSvc->beamPos();
      // HepGeom::Point3D<double> m_beamSpot = m_iBeamCondSvc->beamPos();

#if 0      
      m_BeamSpot_x = m_beamSpot.x();
      m_BeamSpot_y = m_beamSpot.y();
      m_BeamSpot_z = m_beamSpot.z();

      m_BeamSpot_sigmaX = m_iBeamCondSvc->beamSigma(0);
      m_BeamSpot_sigmaY = m_iBeamCondSvc->beamSigma(1);
      m_BeamSpot_sigmaZ = m_iBeamCondSvc->beamSigma(2);
#else
      m_BeamSpot_x = m_iBeamCondSvc->beamPos()[0];
      m_BeamSpot_y = m_iBeamCondSvc->beamPos()[1];
      m_BeamSpot_z = m_iBeamCondSvc->beamPos()[2];
#endif 

    }


    // Online

    // Define Expert methods so that you can navigate to the collection that you need
    Trig::ExpertMethods* em = m_TDT->ExperimentalAndExpertMethods();
    em->enable();
    HLT::NavigationCore* nc = const_cast<HLT::NavigationCore*>(em->getNavigation());
    //const HLT::NavigationCore* nc = em->getNavigation();

    Trig::FeatureContainer featCont = m_TDT->features(m_ChainName);

    //const std::vector< Trig::Combination > comb_container = featCont.getCombinations();
    //for( Trig::FeatureContainer::combination_const_iterator c = comb_container.begin(); c != comb_container.end(); ++c){
    //std::vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = c->get<TrigRoiDescriptor>("initialRoI");

    m_nTracksTotal = 0;

    if(m_ChainName.find("L2_", 0) != std::string::npos){

      //if (SLICE.name == "Mu" || SLICE.name == "MuComb") initRois = featCont.get<TrigRoiDescriptor>("forID");
      std::vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = featCont.get<TrigRoiDescriptor>("initialRoI");

      for ( std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin(); roiItr != vecRoi.end(); ++roiItr) {

	const HLT::TriggerElement* a = (*roiItr).te();

	// Tracks
	std::vector< const TrigInDetTrackCollection*> collectionVector;
	if(m_ChainName == "L2_b10_IDTrkNoCut") nc->getFeaturesInRoI( a, collectionVector, "TrigSiTrack_Jet");
	if(m_ChainName == "L2_b50_NoCut_j50_c4cchad") nc->getFeaturesInRoI( a, collectionVector, "TrigL2SiTrackFinder_Jet");
	int nTracks = 0;
	if(collectionVector.size()){
	  nTracks = (collectionVector[0])->size();
	}
	m_nTracksTotal = m_nTracksTotal + nTracks;

	if(m_pvtxContainer == "L2BjetFex"){

	  // Primary vertex
	  std::vector< const TrigL2BjetContainer*> bjetcoll;
	  nc->getFeaturesInRoI( a, bjetcoll, "L2BjetFex");
	  int n_online_pvtx = 0;
	  int nCol2bj = bjetcoll.size();
	  for(int i = 0 ; i<nCol2bj ; i++){
	    int nPVX = bjetcoll[i]->size();
	    for(int j = 0 ; j<nPVX ; j++){
	      m_online_pvtx_x = 0;
	      m_online_pvtx_y = 0;
	      m_online_pvtx_z = (bjetcoll[i])->at(j)->prmVtx(); // primary vertex z coord

	      hist("Online_x","HLT/IDpvtx")->Fill(m_online_pvtx_x);
	      hist("Online_y","HLT/IDpvtx")->Fill(m_online_pvtx_y);
	      hist("Online_z","HLT/IDpvtx")->Fill(m_online_pvtx_z);

	      n_online_pvtx++;

	      m_haveOnlinePV = true;
	      calculate(nTracks);
	    }
	  }
	  hist("Online_n","HLT/IDpvtx")->Fill(n_online_pvtx);

	}
  else {
	  ATH_MSG_WARNING("HLTIDpvtxTool does not know how to handle primary vertex container name "
                    << m_pvtxContainer << " in chain " << m_ChainName);
	}

      }

    }else if(m_ChainName.find("EF_", 0) != std::string::npos){

      if(m_ChainName == "EF_b55_NoCut_j55_a4tchad"){

	std::vector< Trig::Feature<TrigEFBjetContainer> > vecbjetcoll = featCont.get<TrigEFBjetContainer>("EFBjetFex");

	for ( std::vector< Trig::Feature<TrigEFBjetContainer> >::const_iterator bjItr = vecbjetcoll.begin(); bjItr != vecbjetcoll.end(); ++bjItr) {

	  const HLT::TriggerElement* a = (*bjItr).te();

	  // Tracks
	  std::vector< const Rec::TrackParticleContainer*> collectionVectort;
	  nc->getFeaturesInRoI( a, collectionVectort, "InDetTrigParticleCreation_Bjet_EFID");
	  int nCol1t = collectionVectort.size();
	  int nTracks = 0;
	  for(int i = 0 ; i<nCol1t ; i++){
	    nTracks = (collectionVectort[i])->size(); // number of tracks?
	  }
	  m_nTracksTotal = m_nTracksTotal + nTracks;

	  if(m_pvtxContainer == "EFBjetFex"){

	    // Primary Vertex
	    std::vector< const TrigEFBjetContainer*> bjetcoll;
	    nc->getFeaturesInRoI( a, bjetcoll, "EFBjetFex");
	    int n_online_pvtx = 0;
	    int nCol2bj = bjetcoll.size();
	    for(int i = 0 ; i<nCol2bj ; i++){
	      int nPVX = bjetcoll[i]->size();
	      for(int j = 0 ; j<nPVX ; j++){
		m_online_pvtx_x = 0;
		m_online_pvtx_y = 0;
		m_online_pvtx_z = (bjetcoll[i])->at(j)->prmVtx(); // primary vertex z coord

		hist("Online_x","HLT/IDpvtx")->Fill(m_online_pvtx_x);
		hist("Online_y","HLT/IDpvtx")->Fill(m_online_pvtx_y);
		hist("Online_z","HLT/IDpvtx")->Fill(m_online_pvtx_z);

		n_online_pvtx++;

		m_haveOnlinePV = true;
		calculate(nTracks);
	      }
	    }
	    hist("Online_n","HLT/IDpvtx")->Fill(n_online_pvtx);

	  }else if(m_pvtxContainer == "PrimVx"){

	    // Primary Vertex (OCPEF?)
	    std::vector< const VxContainer*> vxcollpv;
	    nc->getFeaturesInRoI( a, vxcollpv, "PrimVx");
	    int n_online_pvtx = 0;
	    int nCol2pv = vxcollpv.size();
	    for(int i = 0 ; i<nCol2pv ; i++){
	      int nPVX = vxcollpv[i]->size();
	      for(int j = 0 ; j<nPVX ; j++){
		// must check type!
		int type = (vxcollpv[i])->at(j)->vertexType();
		if(type==1){
		  m_online_pvtx_x = (vxcollpv[i])->at(j)->recVertex().position().x();
		  m_online_pvtx_y = (vxcollpv[i])->at(j)->recVertex().position().y();
		  m_online_pvtx_z = (vxcollpv[i])->at(j)->recVertex().position().z(); // primary vertex z coord

		  hist("Online_x","HLT/IDpvtx")->Fill(m_online_pvtx_x);
		  hist("Online_y","HLT/IDpvtx")->Fill(m_online_pvtx_y);
		  hist("Online_z","HLT/IDpvtx")->Fill(m_online_pvtx_z);

		  n_online_pvtx++;

		  m_haveOnlinePV = true;
		  calculate(nTracks);
		}
	      }
	    }
	    hist("Online_n","HLT/IDpvtx")->Fill(n_online_pvtx);

	  }
    else {
	    ATH_MSG_WARNING("HLTIDpvtxTool does not know how to handle primary vertex container name "
                      << m_pvtxContainer << " in chain " << m_ChainName);
	  }

	}
      }


      if(m_ChainName == "EF_b10_IDTrkNoCut"){

	//if (SLICE.name == "Mu" || SLICE.name == "MuComb") initRois = featCont.get<TrigRoiDescriptor>("forID");
	std::vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = featCont.get<TrigRoiDescriptor>("initialRoI");

	for ( std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin(); roiItr != vecRoi.end(); ++roiItr) {

	  const HLT::TriggerElement* a = (*roiItr).te();

	  // Tracks
	  std::vector< const Rec::TrackParticleContainer*> collectionVectort;
	  nc->getFeaturesInRoI( a, collectionVectort, "InDetTrigParticleCreation_Bjet_EFID");
	  int nCol1t = collectionVectort.size();
	  int nTracks = 0;
	  for(int i = 0 ; i<nCol1t ; i++){
	    nTracks = (collectionVectort[i])->size(); // number of tracks?
	  }
	  m_nTracksTotal = m_nTracksTotal + nTracks;

	  if(m_pvtxContainer == "EFBjetFex"){

	    // Primary Vertex
	    std::vector< const TrigEFBjetContainer*> bjetcoll;
	    nc->getFeaturesInRoI( a, bjetcoll, "EFBjetFex");
	    int n_online_pvtx = 0;
	    int nCol2bj = bjetcoll.size();
	    for(int i = 0 ; i<nCol2bj ; i++){
	      int nPVX = bjetcoll[i]->size();
	      for(int j = 0 ; j<nPVX ; j++){
		m_online_pvtx_x = 0;
		m_online_pvtx_y = 0;
		m_online_pvtx_z = (bjetcoll[i])->at(j)->prmVtx(); // primary vertex z coord

		hist("Online_x","HLT/IDpvtx")->Fill(m_online_pvtx_x);
		hist("Online_y","HLT/IDpvtx")->Fill(m_online_pvtx_y);
		hist("Online_z","HLT/IDpvtx")->Fill(m_online_pvtx_z);

		n_online_pvtx++;

		m_haveOnlinePV = true;
		calculate(nTracks);
	      }
	    }
	    hist("Online_n","HLT/IDpvtx")->Fill(n_online_pvtx);

	  }else if(m_pvtxContainer == "PrimVx"){

	    // Primary Vertex (OCPEF?)
	    std::vector< const VxContainer*> vxcollpv;
	    nc->getFeaturesInRoI( a, vxcollpv, "PrimVx");
	    int n_online_pvtx = 0;
	    int nCol2pv = vxcollpv.size();
	    for(int i = 0 ; i<nCol2pv ; i++){
	      int nPVX = vxcollpv[i]->size();
	      for(int j = 0 ; j<nPVX ; j++){
		// must check type!
		int type = (vxcollpv[i])->at(j)->vertexType();
		if(type==1){
		  m_online_pvtx_x = (vxcollpv[i])->at(j)->recVertex().position().x();
		  m_online_pvtx_y = (vxcollpv[i])->at(j)->recVertex().position().y();
		  m_online_pvtx_z = (vxcollpv[i])->at(j)->recVertex().position().z(); // primary vertex z coord

		  hist("Online_x","HLT/IDpvtx")->Fill(m_online_pvtx_x);
		  hist("Online_y","HLT/IDpvtx")->Fill(m_online_pvtx_y);
		  hist("Online_z","HLT/IDpvtx")->Fill(m_online_pvtx_z);

		  n_online_pvtx++;

		  m_haveOnlinePV = true;
		  calculate(nTracks);
		}
	      }
	    }
	    hist("Online_n","HLT/IDpvtx")->Fill(n_online_pvtx);

	  }
    else {
	    ATH_MSG_WARNING("HLTIDpvtxTool does not know how to handle primary vertex container name "
                      << m_pvtxContainer << " in chain " << m_ChainName);
	  }

	  //std::string art ="";
	  //nc->printASCIIArt(art,a);
	  //ATH_MSG_VERBOSE(art);

	}
      }

    }
    else {
      ATH_MSG_WARNING("HLTIDpvtxTool does not know how to handle chain name " << m_ChainName);
    }

    // 
    hist("Online_nTracks","HLT/IDpvtx")->Fill(m_nTracksTotal);
    //calculate(m_nTrackTotal);

  }
  
  ATH_MSG_VERBOSE(" ====== End fill() ====== ");

  return sc;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode  HLTIDpvtxTool::proc() { 
#else
StatusCode  HLTIDpvtxTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun ) {
#endif

  ATH_MSG_VERBOSE(" ====== Begin proc() ====== ");

  if (endOfRun) {

    int allIn = 0;
    int allOut = 0;

    for(int i=0 ; i<(int)m_online_efficiency.size() ; i++){
      int in = m_online_efficiency[i][0];
      int out = m_online_efficiency[i][1];
      allIn = allIn + in;
      allOut = allOut + out;
      float ratio = 0;
      if( in != 0 || out != 0 ){
	ratio = ((float)in)/((float)(in+out));
      }
      hist("Online_efficiency_nTracks","HLT/IDpvtx")->Fill(m_online_efficiency[i][2],ratio);
    }
    if( m_online_efficiency_xyz[0][0] != 0 || m_online_efficiency_xyz[0][1] != 0 ) hist("Online_efficiency_x","HLT/IDpvtx")->Fill(0.0,((float)m_online_efficiency_xyz[0][0])/((float)(m_online_efficiency_xyz[0][0]+m_online_efficiency_xyz[0][1])));
    if( m_online_efficiency_xyz[1][0] != 0 || m_online_efficiency_xyz[1][1] != 0 ) hist("Online_efficiency_y","HLT/IDpvtx")->Fill(0.0,((float)m_online_efficiency_xyz[1][0])/((float)(m_online_efficiency_xyz[1][0]+m_online_efficiency_xyz[1][1])));
    if( m_online_efficiency_xyz[2][0] != 0 || m_online_efficiency_xyz[2][1] != 0 ) hist("Online_efficiency_z","HLT/IDpvtx")->Fill(0.0,((float)m_online_efficiency_xyz[2][0])/((float)(m_online_efficiency_xyz[2][0]+m_online_efficiency_xyz[2][1])));

    if( allIn != 0 || allOut != 0 ){
      float allRatio = ((float)allIn)/((float)(allIn+allOut));
      hist("Online_efficiency","HLT/IDpvtx")->Fill(0.0,allRatio);
    }

    for(int i=0 ; i<(int)m_online_efficiency.size() ; i++){
      if(m_online_x_resolution[i][0] > 1.0){
	float sigma = sqrt(m_online_x_resolution[i][2]/m_online_x_resolution[i][0]);
	hist("Online_x_resolution","HLT/IDpvtx")->Fill(m_online_x_resolution[i][3],sigma);
      }
      if(m_online_y_resolution[i][0] > 1.0){
	float sigma = sqrt(m_online_y_resolution[i][2]/m_online_y_resolution[i][0]);
	hist("Online_y_resolution","HLT/IDpvtx")->Fill(m_online_y_resolution[i][3],sigma);
      }
      if(m_online_z_resolution[i][0] > 1.0){
	float sigma = sqrt(m_online_z_resolution[i][2]/m_online_z_resolution[i][0]);
	hist("Online_z_resolution","HLT/IDpvtx")->Fill(m_online_z_resolution[i][3],sigma);
      }
      if(m_online_r_resolution[i][0] > 1.0){
	float sigma = sqrt(m_online_r_resolution[i][2]/m_online_r_resolution[i][0]);
	hist("Online_r_resolution","HLT/IDpvtx")->Fill(m_online_r_resolution[i][3],sigma);
      }
    }

  }
  ATH_MSG_VERBOSE(" ====== End proc() ====== ");
  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void HLTIDpvtxTool::calculate(int nTracks){

  if(!m_haveOfflinePV || !m_haveOnlinePV) return;

  float residual_x = m_online_pvtx_x - m_offline_pvtx_x;
  float residual_y = m_online_pvtx_y - m_offline_pvtx_y;
  float residual_z = m_online_pvtx_z - m_offline_pvtx_z;
  float residual_r = sqrt((residual_x*residual_x) + (residual_y*residual_y) + (residual_z*residual_z));

  hist("Online_x_residual","HLT/IDpvtx")->Fill(residual_x);
  hist("Online_y_residual","HLT/IDpvtx")->Fill(residual_y);
  hist("Online_z_residual","HLT/IDpvtx")->Fill(residual_z);
  //hist("Online_r_residual","HLT/IDpvtx")->Fill(residual_r);
  hist("Online_r_residual","HLT/IDpvtx")->Fill(residual_r,1.0/(4.0*M_PI*residual_r*residual_r));

  bool passEffCut = (residual_x < m_effRangeX && residual_y < m_effRangeY && residual_z < m_effRangeZ);
  if(residual_x < m_effRangeX) m_online_efficiency_xyz[0][0]++;
  else m_online_efficiency_xyz[0][1]++;
  if(residual_y < m_effRangeY) m_online_efficiency_xyz[1][0]++;
  else m_online_efficiency_xyz[1][1]++;
  if(residual_z < m_effRangeZ) m_online_efficiency_xyz[2][0]++;
  else m_online_efficiency_xyz[2][1]++;

  bool doWeHaveThisNumberOfTracksAlready = false;
  int positionOfNTracksInVectors = -1;

  for(int i=0 ; i<(int)m_online_efficiency.size() ; i++){
    if(m_online_efficiency[i][2] == nTracks){
      doWeHaveThisNumberOfTracksAlready = true;
      positionOfNTracksInVectors = i;
      break;
    }
  }

  if(!doWeHaveThisNumberOfTracksAlready){
    positionOfNTracksInVectors = m_online_efficiency.size();
    m_online_efficiency.push_back(std::vector<int>(3,0));
    m_online_efficiency[positionOfNTracksInVectors][2] = nTracks;
    m_online_x_resolution.push_back(std::vector<double>(4,0.0));
    m_online_x_resolution[positionOfNTracksInVectors][3] = nTracks;
    m_online_y_resolution.push_back(std::vector<double>(4,0.0));
    m_online_y_resolution[positionOfNTracksInVectors][3] = nTracks;
    m_online_z_resolution.push_back(std::vector<double>(4,0.0));
    m_online_z_resolution[positionOfNTracksInVectors][3] = nTracks;
    m_online_r_resolution.push_back(std::vector<double>(4,0.0));
    m_online_r_resolution[positionOfNTracksInVectors][3] = nTracks;
  }

  if(passEffCut){
    m_online_efficiency[positionOfNTracksInVectors][0]++;
  }else{
    m_online_efficiency[positionOfNTracksInVectors][1]++;
  }

  double k;
  double Aold;
  double Qold;
  double Anew;

  m_online_x_resolution[positionOfNTracksInVectors][0]++;
  k = m_online_x_resolution[positionOfNTracksInVectors][0];
  Aold = m_online_x_resolution[positionOfNTracksInVectors][1];
  m_online_x_resolution[positionOfNTracksInVectors][1] = Aold + ((residual_x - Aold)/k);
  Anew = m_online_x_resolution[positionOfNTracksInVectors][1];
  Qold = m_online_x_resolution[positionOfNTracksInVectors][2];
  m_online_x_resolution[positionOfNTracksInVectors][2] = Qold + ((residual_x - Aold)*(residual_x - Anew));

  m_online_y_resolution[positionOfNTracksInVectors][0]++;
  k = m_online_y_resolution[positionOfNTracksInVectors][0];
  Aold = m_online_y_resolution[positionOfNTracksInVectors][1];
  m_online_y_resolution[positionOfNTracksInVectors][1] = Aold + ((residual_y - Aold)/k);
  Anew = m_online_y_resolution[positionOfNTracksInVectors][1];
  Qold = m_online_y_resolution[positionOfNTracksInVectors][2];
  m_online_y_resolution[positionOfNTracksInVectors][2] = Qold + ((residual_y - Aold)*(residual_y - Anew));

  m_online_z_resolution[positionOfNTracksInVectors][0]++;
  k = m_online_z_resolution[positionOfNTracksInVectors][0];
  Aold = m_online_z_resolution[positionOfNTracksInVectors][1];
  m_online_z_resolution[positionOfNTracksInVectors][1] = Aold + ((residual_z - Aold)/k);
  Anew = m_online_z_resolution[positionOfNTracksInVectors][1];
  Qold = m_online_z_resolution[positionOfNTracksInVectors][2];
  m_online_z_resolution[positionOfNTracksInVectors][2] = Qold + ((residual_z - Aold)*(residual_z - Anew));

  m_online_r_resolution[positionOfNTracksInVectors][0]++;
  k = m_online_r_resolution[positionOfNTracksInVectors][0];
  Aold = m_online_r_resolution[positionOfNTracksInVectors][1];
  m_online_r_resolution[positionOfNTracksInVectors][1] = Aold + ((residual_r - Aold)/k);
  Anew = m_online_r_resolution[positionOfNTracksInVectors][1];
  Qold = m_online_r_resolution[positionOfNTracksInVectors][2];
  m_online_r_resolution[positionOfNTracksInVectors][2] = Qold + ((residual_r - Aold)*(residual_r - Anew));

}
