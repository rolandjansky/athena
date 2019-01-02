/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTK_RawDataAlgs/FTK_RDO_CreatorAlgo.h"
#include "TrigFTKSim/MultiTruth.h"

#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TBranch.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTK_RDO_CreatorAlgo::FTK_RDO_CreatorAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_StoreGate(nullptr),
  m_detStore(nullptr),
  m_ftktrack_paths_merged(0),
  m_mergedtracks_chain(nullptr),
  m_mergedtracks_tree(nullptr),
  m_mergedtracks_stream(nullptr),
  m_mergedtracks_bname("FTKBankMerged"), /* Old standalone name is FTKBankMerged, prdosys FTKMergedTracksStream */
  m_mergedtracks_iev(0),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_ftk_raw_trackcollection(nullptr), // JWH Added to test FTK RDO
  m_check(true),
  m_NcheckFail(0),
  m_pixelId(nullptr),
  m_sctId(nullptr),
  m_id_helper(nullptr),
  m_trainingBeamspotX(0.),
  m_trainingBeamspotY(0.),
  m_trainingBeamspotTiltX(0.),
  m_trainingBeamspotTiltY(0.),
  m_reverseIBLlocx(true)
{
  declareProperty("mergedTrackPaths",m_ftktrack_paths_merged,
      "Paths of the merged tracks");
  declareProperty("mergeTrackBName",m_mergedtracks_bname,"Branch name for the merged tracks");
  declareProperty("CheckRDO",m_check);
  declareProperty("TrainingBeamspotX", m_trainingBeamspotX);
  declareProperty("TrainingBeamspotY", m_trainingBeamspotY);
  declareProperty("TrainingBeamspotTiltX", m_trainingBeamspotTiltX);
  declareProperty("TrainingBeamspotTiltY", m_trainingBeamspotTiltY);
  declareProperty("ReverseIBLlocX",m_reverseIBLlocx, "reverse the direction of IBL locX from FTK");

  m_FTK_RawTrack_checkFails.reserve(9);
  m_FTK_RawSCT_Cluster_checkFails.reserve(3);
  m_FTK_RawPixelCluster_checkFails.reserve(5);
  for (unsigned int i=0; i < 9; i++)   m_FTK_RawTrack_checkFails[i]=0;
  for (unsigned int i=0; i < 3; i++)   m_FTK_RawSCT_Cluster_checkFails[i]=0;
  for (unsigned int i=0; i < 5; i++)   m_FTK_RawPixelCluster_checkFails[i]=0;
}

FTK_RDO_CreatorAlgo::~FTK_RDO_CreatorAlgo()
{
  if (m_mergedtracks_stream) delete m_mergedtracks_stream;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("FTK_RDO_CreatorAlgo::initialize()");

  ATH_MSG_VERBOSE("mergedTrackPaths: "<<m_ftktrack_paths_merged<<
		  "Paths of the merged tracks: ");
  ATH_MSG_VERBOSE("mergeTrackBName: "<<m_mergedtracks_bname<<" - Branch name for the merged tracks");
  
  ATH_MSG_INFO("Correcting for FTK training beamspot at x " <<  m_trainingBeamspotX <<" y " << 	m_trainingBeamspotY
	       << " z " <<  m_trainingBeamspotZ << " TiltX " << m_trainingBeamspotTiltX << "TiltY " << m_trainingBeamspotTiltY);
  if (m_reverseIBLlocx)ATH_MSG_INFO(" Reversing direction of IBL hit phi position");


  StatusCode scSG = service( "StoreGateSvc", m_StoreGate );
  if (scSG.isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve StoreGate service");
    return scSG;
  }

  StoreGateSvc* detStore;
  ATH_CHECK(service("DetectorStore", detStore));
  ATH_CHECK(detStore->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK(detStore->retrieve(m_sctId, "SCT_ID"));
  ATH_CHECK(detStore->retrieve(m_id_helper, "AtlasID"));

  // prepare the input from the FTK tracks, merged in an external simulation
  m_mergedtracks_chain = new TChain("ftkdata","Merged tracks chain");
  // add the file to the chain
  ATH_MSG_INFO("Loading " << m_ftktrack_paths_merged.size() << " files with FTK merged tracks" );
  vector<string>::const_iterator imtp = m_ftktrack_paths_merged.begin();
  for (;imtp!=m_ftktrack_paths_merged.end();++imtp) {
    Int_t addres = m_mergedtracks_chain->Add((*imtp).c_str());
    ATH_MSG_DEBUG("Added: " << *imtp << '[' << addres << ']');
  }
  m_mergedtracks_stream = new FTKTrackStream();
  TBranch *mergedtracks_branch;
  Int_t res = m_mergedtracks_chain->SetBranchAddress(m_mergedtracks_bname.c_str(),&m_mergedtracks_stream,&mergedtracks_branch);
  if (res<0) {
    ATH_MSG_FATAL("Branch \"" << m_mergedtracks_bname << "\" with merged tracks not found" );
    return StatusCode::FAILURE;
  }

  // reset the counters
  m_mergedtracks_iev = 0;

  // cleanup the map between event number and vector of FTK tracks
  m_trackVectorMap.clear();

  Long64_t nEntries = m_mergedtracks_chain->GetEntries();

  int prevEventNumber(-1); // variable to check repetition of events, special warning condition
  bool hasRepeatedEvents(false); // flag to verify if the repetition of the event condition has been found

  for(Long64_t iEntry=0; iEntry<nEntries; iEntry++) {

    m_mergedtracks_chain->GetEntry(iEntry);
    int eventNumber = m_mergedtracks_stream->eventNumber();

    std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);

    if(mapIt==m_trackVectorMap.end()) {//new event
      if (!hasRepeatedEvents) {
        m_trackVectorMap.insert(std::pair<int, Long64_t >(eventNumber, iEntry ));
        prevEventNumber = eventNumber;
      }
      else {
        ATH_MSG_FATAL("A duplicated event was found before the end of file, the error cannot be recoverd" );
        return StatusCode::FAILURE;
      }
    }
    else if (eventNumber==prevEventNumber) {
      ATH_MSG_WARNING("Event " << eventNumber << " found in the previous event, failure condition masked if this happens at the end of the file" );
      hasRepeatedEvents = true;
    }
    else {
      // Duplicate events are a condition error at the moment, this
      // can be fixed using a runNumber and eventNumber paier as key
      ATH_MSG_FATAL("Tracks for the current event (" << eventNumber << ") already exist. Duplication not allowed" );
      return StatusCode::FAILURE;
    }

  }

  ATH_MSG_DEBUG("Tracks from " << m_trackVectorMap.size() << " events loaded");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::execute() {
  MsgStream log(msgSvc(), name());
  ATH_MSG_DEBUG("FTK_RDO_CreatorAlgo::execute() start" );
  // Get information on the events
  const EventInfo* eventInfo(0);
  if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve event info" );
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );
  int eventNumber =  eventID->event_number();

  ATH_MSG_DEBUG(
		"entered execution for run " << eventID->run_number()
		<< "   event " << eventNumber
		);

  // Extract the vector of tracks found by the FTK for the current event
  std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);
  if(mapIt == m_trackVectorMap.end()) {
    ATH_MSG_ERROR("No FTK tracks for event " << eventNumber << " found, possible mismatch between RDO and FTK files");
    return StatusCode::FAILURE;
  }

  //Have Tracks
  const Long64_t &FTKEntry = (*mapIt).second;
  if (m_mergedtracks_chain->GetEntry(FTKEntry)==-1) {
    ATH_MSG_ERROR("Error reading the FTK entry: " << FTKEntry);
    return StatusCode::FAILURE;
  }

  int ntracks_merged = m_mergedtracks_stream->getNTracks();
  ATH_MSG_DEBUG("Number of FTK tracks to merge in SG: " << ntracks_merged );



  m_ftk_raw_trackcollection = new FTK_RawTrackContainer;

  StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
  if (scJ.isFailure()) {
    ATH_MSG_FATAL("Failure registering FTK_RawTrackContainer" );
    return StatusCode::FAILURE;
  } else{
    ATH_MSG_DEBUG("Setting FTK_RawTrackContainer registered" );
  }

  //
  // Loop on merged tracks
  //
  for(int itrack=0;itrack!=ntracks_merged;++itrack) {

    // get the current track
    FTKTrack *curtrk = m_mergedtracks_stream->getTrack(itrack);

    // check RDO are only created from final tracks
    if (curtrk->getHWRejected()) continue;

    m_ftk_raw_trackcollection->push_back(SimToRaw(*curtrk));

  } // end merged tracks loop

  // prepare to move to the next event
  m_mergedtracks_iev += 1;

  ATH_MSG_DEBUG("FTK_RDO_CreatorAlgo::execute() end" );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::finalize() {
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("finalize()" );

  //
  //  Clean up output and input merged files
  //

  if ( m_NcheckFail== 0 ) {
    ATH_MSG_INFO(" FTK_RDO_Creator: No problems found with FTK RDO");
  }
  if (m_NcheckFail !=0){
    ATH_MSG_INFO(" FTK_RDO_Creator: " << m_NcheckFail << " problems found with FTK RDO:");
    if (m_FTK_RawTrack_checkFails[0]!=0) ATH_MSG_INFO("RoadID " << m_FTK_RawTrack_checkFails[0]);
    if (m_FTK_RawTrack_checkFails[1]!=0) ATH_MSG_INFO("Barcode " << m_FTK_RawTrack_checkFails[1]);
    if (m_FTK_RawTrack_checkFails[2]!=0) ATH_MSG_INFO("z0 " << m_FTK_RawTrack_checkFails[2]);
    if (m_FTK_RawTrack_checkFails[3]!=0) ATH_MSG_INFO("d0 " << m_FTK_RawTrack_checkFails[3]);
    if (m_FTK_RawTrack_checkFails[4]!=0) ATH_MSG_INFO("phi " << m_FTK_RawTrack_checkFails[4]);
    if (m_FTK_RawTrack_checkFails[5]!=0) ATH_MSG_INFO("cosTheta " << m_FTK_RawTrack_checkFails[5]);
    if (m_FTK_RawTrack_checkFails[6]!=0) ATH_MSG_INFO("invPt " << m_FTK_RawTrack_checkFails[6]);
    if (m_FTK_RawTrack_checkFails[7]!=0) ATH_MSG_INFO("SectorID " << m_FTK_RawTrack_checkFails[7]);
    if (m_FTK_RawTrack_checkFails[8]!=0) ATH_MSG_INFO("LayerMap " << m_FTK_RawTrack_checkFails[8]);

    if (m_FTK_RawPixelCluster_checkFails[0]!=0) ATH_MSG_INFO("ModuleID " << m_FTK_RawPixelCluster_checkFails[0]);
    if (m_FTK_RawPixelCluster_checkFails[1]!=0) ATH_MSG_INFO("Row " << m_FTK_RawPixelCluster_checkFails[1]);
    if (m_FTK_RawPixelCluster_checkFails[2]!=0) ATH_MSG_INFO("Column " << m_FTK_RawPixelCluster_checkFails[2]);
    if (m_FTK_RawPixelCluster_checkFails[3]!=0) ATH_MSG_INFO("RowWidth " << m_FTK_RawPixelCluster_checkFails[3]);
    if (m_FTK_RawPixelCluster_checkFails[4]!=0) ATH_MSG_INFO("ColWidth " << m_FTK_RawPixelCluster_checkFails[4]);

    if (m_FTK_RawSCT_Cluster_checkFails[0]!=0) ATH_MSG_INFO("ModuleID " << m_FTK_RawSCT_Cluster_checkFails[0]);
    if (m_FTK_RawSCT_Cluster_checkFails[1]!=0) ATH_MSG_INFO("strip " << m_FTK_RawSCT_Cluster_checkFails[1]);
    if (m_FTK_RawSCT_Cluster_checkFails[2]!=0) ATH_MSG_INFO("cluster width " << m_FTK_RawSCT_Cluster_checkFails[2]);
  }



  // cleanup the map containing the list of FTK tracks found in each event
  ATH_MSG_DEBUG("Clear Track map" );
  m_trackVectorMap.clear();

  ATH_MSG_DEBUG("end Finalize() " );
  return StatusCode::SUCCESS;
}


FTK_RawTrack* FTK_RDO_CreatorAlgo::SimToRaw(const FTKTrack &track)
{
  FTK_RawTrack *raw_track = new FTK_RawTrack();

  const unsigned int sctHashMax = m_sctId->wafer_hash_max();
  const unsigned int pixHashMax = m_pixelId->wafer_hash_max();

  unsigned int layerMap = 0;

  for (unsigned int iPlane = 0, isct=0; iPlane<(unsigned int)track.getNPlanes(); ++iPlane){
    const FTKHit& hit = track.getFTKHit(iPlane);
    if (hit.getDim()== 0) {
      ATH_MSG_DEBUG("No hit for layer " << iPlane);
      continue;
    } 

    layerMap |= (1 << iPlane);

    ATH_MSG_VERBOSE("Information from FTKHit " << iPlane << ": sector " << hit.getSector()<< " plane " << hit.getPlane() << " etaCode " << hit.getEtaCode());
    ATH_MSG_VERBOSE(" isBarrel " << hit.getIsBarrel() << " Aside " << hit.getASide() << " Cside "<< hit.getCSide() );
    ATH_MSG_VERBOSE(" eta module " << hit.getEtaModule() << " phiModule " << hit.getPhiModule() << " section " << hit.getSection());
    ATH_MSG_VERBOSE(" etaWidth " << hit.getEtaWidth() << " phiWidth " << hit.getPhiWidth() << " nStrips " << hit.getPhiWidth() << " Dim " << hit.getDim());
    ATH_MSG_VERBOSE(" coord(0) " << hit.getHwCoord(0) << " localCoord(0) " << hit.getLocalCoord(0));
    if ( hit.getDim()> 1) ATH_MSG_VERBOSE(" coord(1) "<< hit.getHwCoord(1)<< " localCoord(1) " << hit.getLocalCoord(1));

    const IdentifierHash hitHashId = hit.getIdentifierHash();
    ATH_MSG_VERBOSE( "Decoding hit.getIdentifierHash() 0x"<<std::hex<< hit.getIdentifierHash() << " HashId 0x" << hitHashId<<std::dec);

    if (iPlane < 4) {
      ATH_MSG_VERBOSE( m_pixelId->print_to_string(m_pixelId->wafer_id(hitHashId)));
      ATH_MSG_VERBOSE(" phi module " << m_pixelId->phi_module(m_pixelId->wafer_id(hitHashId)) << " eta module " <<  m_pixelId->eta_module(m_pixelId->wafer_id(hitHashId)));
    } else {
      ATH_MSG_VERBOSE( m_sctId->print_to_string(m_sctId->wafer_id(hitHashId)));
      ATH_MSG_VERBOSE(" phi module " << m_sctId->phi_module(m_sctId->wafer_id(hitHashId)) << " eta module " <<  m_sctId->eta_module(m_sctId->wafer_id(hitHashId)));
    }

    const MultiTruth hittruth= hit.getTruth();
    MultiTruth::Barcode hitbarcode;
    MultiTruth::Weight tfrac;
    const bool truth_ok = hittruth.best(hitbarcode,tfrac);

    if ( iPlane<4 ) {
      bool valid = true;
      if (hitHashId >= pixHashMax) {
	ATH_MSG_VERBOSE( " Invalid Pixel hashId " << hitHashId << " pixHashMax " << sctHashMax);
	valid = false;
      }
      ATH_MSG_VERBOSE( " Creating Pixel cluster for layer " << iPlane);
      

      float phi_index= hit.getHwCoord(0);
      float eta_index = hit.getHwCoord(1);

      if (m_reverseIBLlocx) {
	Identifier wafer_id = m_pixelId->wafer_id(hitHashId);
	bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);
	unsigned int layer = m_pixelId->layer_disk(wafer_id);

	if (isBarrel && layer==0) {
	  phi_index = 2680. -   phi_index; //335*8=2680
	} 
      }

      //      if (phi_index > m_pixelId->phi_index_max(wafer_id)){
        //ATH_MSG_INFO( " Invalid Pixel phi_index " << phi_index << " max phi index " <<  m_pixelId->phi_index_max(wafer_id) << " and original value before scaling = " <<  hit.getHwCoord(0));
        //valid = false;
      //      }
      //      if (eta_index > m_pixelId->eta_index_max(wafer_id)){
	// ATH_MSG_INFO( " Invalid Pixel eta_index " << hit.getHwCoord(1) << " max eta index " <<  m_pixelId->eta_index_max(wafer_id));
        //valid = false;
      //      }

      // if (hit.getPhiWidth()==0) {
      //   ATH_MSG_INFO( " Invalid Pixel cluster Phi width=0 setting to 1");
      // }
      // if (hit.getEtaWidth()==0) {
      //   ATH_MSG_INFO( " Invalid Pixel cluster Eta width=0 setting to 1");
      // }
      if (valid) {
        ATH_MSG_VERBOSE( " Creating Pixel cluster for layer " << iPlane << " hit.getDim() " <<hit.getDim());
        FTK_RawPixelCluster pixel_clus;
        pixel_clus.setModuleID(hit.getIdentifierHash());
        pixel_clus.setRowCoord(phi_index);
        pixel_clus.setRowWidth(hit.getPhiWidth());
        pixel_clus.setColCoord(eta_index);
        pixel_clus.setColWidth(hit.getEtaWidth());

        if (truth_ok) pixel_clus.setBarcode(hitbarcode.second);
        ATH_MSG_VERBOSE( " Adding Pixel cluster to track at layer " << iPlane);
        raw_track->setPixelCluster(iPlane,pixel_clus);
      } else {
        ATH_MSG_VERBOSE( " No valid Pixel cluster added for layer " << iPlane);
      } 
    } else { // SCT
      bool valid = true;
      if (hitHashId >= sctHashMax) {
	ATH_MSG_INFO( " Invalid SCT hashId " << hitHashId << " sctHashMax " << sctHashMax);
	valid = false;
      }
      
      Identifier wafer_id = m_sctId->wafer_id(hitHashId);
      // Identifier strip_id = m_sctId->strip_id(wafer_id, strip);


      float sct_strip = hit.getHwCoord(0);

      if (sct_strip > m_sctId->strip_max(wafer_id)){
        //ATH_MSG_INFO( " Invalid SCT strip " << sct_strip << " max strip " <<  m_sctId->strip_max(wafer_id)  << " and raw value of hwcoord = " << hit.getHwCoord(0));
        //valid = false;
      }
      // if (hit.getNStrips()==0) {
      //   ATH_MSG_INFO( " Invalid SCT cluster width=0 setting to 1");
      // }
      if (valid) {
	ATH_MSG_VERBOSE( " Creating SCT cluster for layer " << iPlane);
	FTK_RawSCT_Cluster sct_clus; // iPlane and not iSCT 
        sct_clus.setModuleID(hit.getIdentifierHash());
        sct_clus.setHitCoord(sct_strip); // loc pos
	//        sct_clus.setHitWidth(getNStrips());
        sct_clus.setHitWidth(hit.getPhiWidth());


        if (truth_ok) sct_clus.setBarcode(hitbarcode.second);
        ATH_MSG_VERBOSE( " Adding SCT cluster to track for layer " << iPlane);
        raw_track->setSCTCluster(iPlane,sct_clus);
        isct++;
      }  else {
        ATH_MSG_VERBOSE( " No valid SCT cluster added for layer " << iPlane);
      }
    }
  } // end of hit loop

  // Road ID convert to 32 bit ///
  ATH_MSG_VERBOSE( " Setting track properties ");
  raw_track->setSectorID(track.getSectorID());
  raw_track->setLayerMap(layerMap);
  raw_track->setTower(track.getRegion());
  raw_track->setRoadID((unsigned int)track.getRoadID());


  float track_phi = track.getPhi();
  while (track_phi>TMath::Pi()) track_phi -=TMath::TwoPi(); // get phi in range -pi to pi
  while (track_phi<-TMath::Pi()) track_phi +=TMath::TwoPi(); // get phi in range -pi to pi
  raw_track->setPhi(track_phi);

  double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track.getZ0();//correction for tilt
  double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track.getZ0();//correction for tilt

  double d0 = track.getIP() - dx*sin(track_phi)+dy*cos(track_phi);
  double z0 = track.getZ0() - ((cos(track_phi) *dx + sin(track_phi)*dy))*track.getCotTheta();


  raw_track->setD0(d0);
  raw_track->setZ0(z0);

  raw_track->setCotTh(track.getCotTheta());
  raw_track->setChi2(track.getChi2());
  float invpt=0;
  if (track.getPt()!=0) invpt = 1./track.getPt();
  raw_track->setInvPt(invpt);
  raw_track->setBarcode(track.getBarcode());

  bool RDO_ok = true;
  if (m_check) {
    if (!(this->check_track(track, *raw_track))) {
      ATH_MSG_DEBUG( " RDO check Failed ");
      RDO_ok=false;
      m_NcheckFail+=1;
    } else {
      ATH_MSG_VERBOSE( " RDO check OK ");
    }

  }
  if (!RDO_ok || msgLvl(MSG::VERBOSE)) {
    this->printTrack(track,raw_track);
  }

  return raw_track;
}
	 
void FTK_RDO_CreatorAlgo::printTrack(const FTKTrack& track, const FTK_RawTrack *raw_track) {
  int iHit = 0;
  float invpt=0;
  if (track.getPt()!=0) invpt = 1./track.getPt();
  std::cout << "FTK_RawTrack:" << 
    " SectorID= 0x" << std::hex << raw_track->getSectorID() << 
    " LayerMap= 0x" << raw_track->getLayerMap() <<
    " RoadID= 0x" << raw_track->getRoadID() << std::dec <<
    " d0= " << raw_track->getD0()     <<
    " z0= " << raw_track->getZ0()     <<
    " phi= " << raw_track->getPhi()    <<
    " cot= " << raw_track->getCotTh()  <<
    " invPt= " << raw_track->getInvPt()   <<
    " chi2= " << raw_track->getChi2()   <<
    " barcode= " << raw_track->getBarcode()<< 
    " layermap bits: "<<std::endl;
  this->printBits(raw_track->getLayerMap() ,12);
  std::cout << std::endl;
  std::cout << "FTKTRack:   "<<
    "  SectorID= 0x" << std::hex << track.getSectorID()<<
    "  LayerMap= 0x" << track.getBitmask()<<
    "  RoadID= 0x" << track.getRoadID() << std::dec<<
    " d0= " << track.getIP() <<
    " z0= " << track.getZ0() <<
    " phi= " << track.getPhi() <<
    " cot= " << track.getCotTheta() <<
    " invPt= " << invpt <<
    " chi2= " << track.getChi2() <<
    " barcode= " << track.getBarcode() << std::endl;
  unsigned int iPlane = 0;
  for( unsigned int i = 0; i < raw_track->getPixelClusters().size(); ++i, iHit++,iPlane++){

    const FTKHit& hit = track.getFTKHit(iHit);
    if (hit.getDim() ==0) {
      std::cout << " FTKHit              " << i << " hit.getDim()=0" << std::endl;
    } else {
      const MultiTruth hittruth= hit.getTruth();
      MultiTruth::Barcode mtbarcode;
      MultiTruth::Weight tfrac;
      const bool ok = hittruth.best(mtbarcode,tfrac);
      long int hitbarcode = 0;
      if (ok) hitbarcode = mtbarcode.second;
      std::cout << " FTKHit              " << i <<
        " Col Coord= " << hit.getHwCoord(1)   <<
        " Col Width= " << hit.getEtaWidth() <<
        " Row Coord= " << hit.getHwCoord(0)   <<
        " Row Width= " << hit.getPhiWidth() <<
        " Hash ID= 0x" << hit.getIdentifierHash() <<
        " Barcode= "   << hitbarcode << " Event " << mtbarcode.first << std::endl;
    }
    if (raw_track->isMissingLayer(iPlane))   std::cout <<  " Missing Layer " << iPlane << std::endl;
    if (raw_track->isMissingPixelLayer(i))   std::cout <<  " Missing Pixel Layer " <<  i << std::endl;
    std::cout << " FTK_RawPixelCluster " << i <<
      " Col Coord= " << raw_track->getPixelClusters()[i].getColCoord() <<
      " Col Width= " << raw_track->getPixelClusters()[i].getColWidth() <<
      " Row Coord= " << raw_track->getPixelClusters()[i].getRowCoord() <<
      " Row Width= " << raw_track->getPixelClusters()[i].getRowWidth() <<
      " Hash ID= 0x" << std::hex << raw_track->getPixelClusters()[i].getModuleID() << std::dec <<
      " Barcode= "   << raw_track->getPixelClusters()[i].getBarcode() << std::endl;
    }
  for( unsigned int isct = 0; isct < raw_track->getSCTClusters().size(); ++isct, iHit++){

    const FTKHit& hit = track.getFTKHit(iHit);
    if (hit.getDim() ==0) {
      std::cout << " FTKHit              " << iHit << " hit.getDim()=0" << std::endl;
    } else {

      const MultiTruth hittruth= hit.getTruth();
      MultiTruth::Barcode mtbarcode;
      MultiTruth::Weight tfrac;
      const bool ok = hittruth.best(mtbarcode,tfrac);
      unsigned int hitbarcode = 0;
      if (ok)  hitbarcode = mtbarcode.second;

      std::cout << " FTKHit             " << iHit <<
        " Hit Coord= " << hit.getHwCoord(0)  <<
        " Hit Width= " << hit.getPhiWidth() <<
        " Module ID= " << hit.getIdentifierHash() <<
        " Barcode= "   << hitbarcode << std::endl;
      if (raw_track->isMissingLayer(iPlane))   std::cout <<  " Missing Layer " << iPlane << std::endl;
      if (raw_track->isMissingSCTLayer(isct))   std::cout <<  " Missing SCT Layer" << std::endl;
      std::cout << " FTK_RawSCT_Cluster " << isct <<
        " Hit Coord= " << raw_track->getSCTClusters()[isct].getHitCoord() <<
        " Hit Width= " << raw_track->getSCTClusters()[isct].getHitWidth() <<
        " Module ID= " << raw_track->getSCTClusters()[isct].getModuleID() <<
        " Barcode= "   << raw_track->getSCTClusters()[isct].getBarcode() << std::endl;
    }
  }
  std::cout << std::endl;
}

#define d0Res 0.001
#define z0Res 0.001
#define phiRes 0.001
#define chi2Res 0.001
#define cotThetaRes 0.001
#define invPtRes 0.0015 
#define sctStripRes 0.5
#define pixColRes 0.1
#define pixRowRes 0.1

bool FTK_RDO_CreatorAlgo::check_track(const FTKTrack &track, FTK_RawTrack &rdo) {

  bool check_ok= true;
  if (!(this->checkInt(track.getRoadID(), rdo.getRoadID(), "RoadID"))) {
    m_FTK_RawTrack_checkFails[8]+=1; check_ok=false;
  }
  if (!(this->checkInt(track.getBarcode(), rdo.getBarcode(), "BarCode"))){
    m_FTK_RawTrack_checkFails[1]+=1;check_ok=false;
  }

  float track_phi = track.getPhi();
  while (track_phi>TMath::Pi()) track_phi -=TMath::TwoPi(); // get phi in range -pi to pi
  while (track_phi<-TMath::Pi()) track_phi +=TMath::TwoPi(); // get phi in range -pi to pi

  double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track.getZ0();//correction for tilt
  double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track.getZ0();//correction for tilt

  double d0 = track.getIP() - dx*sin(track_phi)+dy*cos(track_phi);
  double z0 = track.getZ0() - ((cos(track_phi) *dx + sin(track_phi)*dy))*track.getCotTheta();

  if (!(this->checkValue(z0, rdo.getZ0(), z0Res, "z0"))) {
    m_FTK_RawTrack_checkFails[2]+=1;check_ok=false;
  }
  if (!(this->checkValue(d0, rdo.getD0(), d0Res, "d0"))) {
    m_FTK_RawTrack_checkFails[3]+=1;check_ok=false;
  }
  if (!(this->checkValue(track_phi, rdo.getPhi(), phiRes, "phi"))){
    m_FTK_RawTrack_checkFails[4]+=1;check_ok=false;
  }
  if (!(this->checkValue(track.getChi2(), rdo.getChi2(), chi2Res, "chi2"))){
    m_FTK_RawTrack_checkFails[4]+=1;check_ok=false;
  }
  if (!(this->checkValue(track.getCotTheta(), rdo.getCotTh(), cotThetaRes, "cotTheta"))) {
    m_FTK_RawTrack_checkFails[5]+=1;check_ok=false;
  }
  if (fabs(track.getPt()) > 620.) {
    float invpt=0;
    if (track.getPt()!=0) invpt = 1./track.getPt();
    if (!(this->checkValue(invpt, rdo.getInvPt(), invPtRes, "invPt"))){
      m_FTK_RawTrack_checkFails[6]+=1;check_ok=false;
    }
  }
  if (!(this->checkInt((track.getSectorID()&0xffff), rdo.getSectorID(), "SectorID"))) {
    m_FTK_RawTrack_checkFails[7]+=1; check_ok=false;
  }

  unsigned int layerMap = 0;

  for (unsigned int i = 0; i<(unsigned int)track.getNPlanes(); i++) {
    const FTKHit& hit = track.getFTKHit(i); 
    bool hitOK=true;
    if (hit.getDim() == 0) {
      if  (!rdo.isMissingLayer(i)) {
	ATH_MSG_INFO("Error with layerMap: Missing Layer not set for layer "<<i);
      }	 
      if  (i<4) {
	if (!rdo.isMissingPixelLayer(i)) {
	  ATH_MSG_INFO("Error with layerMap: Missing Layer not set for Pixel layer "<<i);
	} 
      } else {
	if (!rdo.isMissingSCTLayer(i-4)) {
	  ATH_MSG_INFO("Error with layerMap: Missing Layer not set for SCT layer "<<(i-4));
	} 
      }
    } else { // hit.getDim() != 0
	layerMap |= 1<<i;
	if (i < 4){
	  if (!(this->checkInt(hit.getIdentifierHash(), rdo.getPixelClusters()[i].getModuleID(), "Pixel moduleID"))){
	    m_FTK_RawPixelCluster_checkFails[0]+=1;hitOK=false;
	  }

	  float phi_index= hit.getHwCoord(0);
	  
	  if (m_reverseIBLlocx) {
	    Identifier wafer_id = m_pixelId->wafer_id(hit.getIdentifierHash());
	    bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);
	    unsigned int layer = m_pixelId->layer_disk(wafer_id);
	    
	    if (isBarrel && layer==0) {
	      phi_index = 2680 -   phi_index; //335*8=2680
	    } 
	  }
	  if (!(this->checkValue(phi_index, rdo.getPixelClusters()[i].getRowCoord(),pixRowRes,"Pixel Row"))){
	    m_FTK_RawPixelCluster_checkFails[1]+=1;hitOK=false;
	  }
	  if (!(this->checkValue(hit.getHwCoord(1), rdo.getPixelClusters()[i].getColCoord(),pixColRes,"Pixel Column"))){
	    m_FTK_RawPixelCluster_checkFails[2]+=1;hitOK=false;
	  }
	  unsigned int wphi=hit.getPhiWidth();
	  if (wphi>8) wphi=8; // expect values 1 to 8
	  if (wphi<1) wphi=1; // expect values 1 to 8
	  if (!(this->checkInt(wphi,rdo.getPixelClusters()[i].getRowWidth(),"Pixel RowWidth"))){ 
	    m_FTK_RawPixelCluster_checkFails[3]+=1;hitOK=false;
	    ATH_MSG_VERBOSE("Pix PhiWidth Check fails hit.getPhiWidth() " << hit.getPhiWidth()  << " expect "  << wphi << " got " << rdo.getPixelClusters()[i].getRowWidth());
	  }
	  unsigned int weta=hit.getEtaWidth(); 
	  if (weta>8) weta=8; // expect values 1 to 8
	  if (weta<1) weta=1; // expect values 1 to 8
	  if (!(this->checkInt(weta, rdo.getPixelClusters()[i].getColWidth(),"Pixel ColWidth"))){ 
	    m_FTK_RawPixelCluster_checkFails[4]+=1;hitOK=false;
	    ATH_MSG_VERBOSE("Pix EtaWidth Check fails hit.getEtaWidth() " << hit.getEtaWidth()  << " expect "  << weta << " got " << rdo.getPixelClusters()[i].getColWidth());
	  }
	} else {
	  if (!(this->checkInt(hit.getIdentifierHash(), rdo.getSCTClusters()[i-4].getModuleID(), "SCT moduleID"))){
	    m_FTK_RawSCT_Cluster_checkFails[0]+=1;hitOK=false;
	  }
	  if (!(this->checkValue(hit.getHwCoord(0), rdo.getSCTClusters()[i-4].getHitCoord(),sctStripRes,"SCT strip"))){
	    m_FTK_RawSCT_Cluster_checkFails[1]+=1;hitOK=false;
	  }
	  unsigned int wstrip=hit.getPhiWidth();
	  if (wstrip>8) wstrip=8; // expect values 1 to 8
	  if (wstrip<1) wstrip=1; // expect values 1 to 8
	  if (!(this->checkInt(wstrip,rdo.getSCTClusters()[i-4].getHitWidth(),"SCT cluster width"))){  
	    m_FTK_RawSCT_Cluster_checkFails[2]+=1;hitOK=false;
	    ATH_MSG_VERBOSE("SCT clusWidth Check fails hit.getPhiWidth() " << hit.getPhiWidth()  << " expect "  << wstrip << " got " << rdo.getSCTClusters()[i-4].getHitWidth());
	  }
	}

	if (!hitOK){
	  check_ok=false;
	  ATH_MSG_INFO(" checks failed for hit at Layer"<<i);
	}
    }
  }

  if (!(this->checkInt(layerMap, rdo.getLayerMap(), "LayerMap"))) {
    m_FTK_RawTrack_checkFails[8]+=1; check_ok=false;
  }

  return check_ok;
}


bool FTK_RDO_CreatorAlgo::checkValue(const float &v1, const float &v2, const float &res, const std::string &what) {
  bool check_ok = true;
  if (fabs(v1+v2)>1e-10) {
    if (fabs(2*(v1-v2)/(v1+v2)) > res) {
      ATH_MSG_INFO( "FTK_RDO_CreatorAlgo::checkValue check failed for "<< what << " difference 2*(FTKTrack-RDO)/(FTKTrack+RDO) = " <<  2*(v1-v2)/(v1+v2) << " values:  FTKTrack " << v1 << " RDO " << v2 << " resolution "  << res);
      check_ok=false;
    }
  }
  return check_ok;
}


bool FTK_RDO_CreatorAlgo::checkInt(const unsigned int &i1, const  unsigned int &i2, const std::string &what) {
  bool check_ok=true;
  if (i1 != i2) {
    ATH_MSG_INFO( "FTK_RDO_CreatorAlgo::checkInt check failed for " << what << " difference FTKTrack-RDO=" <<  i1-i2 << " values:  FTKTrack 0x" << std::hex << i1 << std::dec << " RDO 0x" << std::hex << i2 << std::dec);
    check_ok=false;
  }
  return check_ok;
}


void FTK_RDO_CreatorAlgo::printBits(unsigned int num, unsigned int length){
  unsigned int bit=length;
  for(unsigned int i=0; i<length; i++){
    bit--;
    std::cout << int( (num & (0x1<<bit))>>bit)<<std::endl;
  }
}

