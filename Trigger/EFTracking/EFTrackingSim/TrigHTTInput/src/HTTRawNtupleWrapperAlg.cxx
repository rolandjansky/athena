#include "HTTRawNtupleWrapperAlg.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTOfflineHit.h"



#include "TFile.h"
#include "TTree.h"

HTTRawNtupleWrapperAlg::HTTRawNtupleWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{}


StatusCode HTTRawNtupleWrapperAlg::initialize()
{

  ATH_CHECK(m_hitInputTool.retrieve());
  ATH_MSG_INFO ("Creating output file: " );
  m_outfile = TFile::Open(m_outpath.value().c_str(),"recreate");
  m_eventHeader = new HTTEventInputHeader();

 // create a TTree to store the truth tracks
  m_trackstree = new TTree("truthtracks","Truth tracks");
  // add the branch related to the truth tracks
  m_trackstree->Branch("TruthTracks",&m_truth_tracks);

  // create a TTree to store event information
  m_evtinfo = new TTree("evtinfo","Events info");
  m_evtinfo->Branch("RunNumber",&m_run_number,"RunNumber/I");
  m_evtinfo->Branch("EventNumber",&m_event_number,"EventNumber/I");
  m_evtinfo->Branch("LB",&m_LB,"LB/I");
  m_evtinfo->Branch("BCID",&m_BCID,"BCID/I");
  m_evtinfo->Branch("ExtendedLevel1ID",&m_extendedLevel1ID,"ExtendedLevel1ID/i");
  m_evtinfo->Branch("Level1TriggerType",&m_level1TriggerType,"Level1TriggerType/i");
  m_evtinfo->Branch("Level1TriggerInfo",&m_level1TriggerInfo);
  m_evtinfo->Branch("AverageInteractionsPerCrossing",&m_averageInteractionsPerCrossing,"AverageInteractionsPerCrossing/F");
  m_evtinfo->Branch("ActualInteractionsPerCrossing",&m_actualInteractionsPerCrossing,"ActualInteractionsPerCrossing/F");

  // create and populate the TTree
  m_hittree = new TTree("htthits","Raw hits for the HTT simulation");
    // prepare a branch for each tower
  m_original_hits = new std::vector<HTTHit>[m_ntowers];
  for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
    m_hittree->Branch(Form("RawHits%d.",ireg),&m_original_hits[ireg], 32000, 1);
  } // end towers loop


  if (m_getOffline) {

    m_offline_locX = new std::vector<float>;
    m_offline_locY = new std::vector<float>;
    m_offline_isPixel = new std::vector<int>;
    m_offline_isBarrel = new std::vector<int>;
    m_offline_layer = new std::vector<int>;
    m_offline_clustID = new std::vector<int>;
    m_offline_trackNumber = new std::vector<int>;

    m_offline_qoverpt = new std::vector<float>;
    m_offline_eta = new std::vector<float>;
    m_offline_phi = new std::vector<float>;
    m_offline_d0 = new std::vector<float>;
    m_offline_z0 = new std::vector<float>;

    m_offline_barcode = new std::vector<int>;
    m_offline_barcode_frac = new std::vector<float>;


    m_offline_cluster_tree = new TTree("offline_cluster_tree","offline_cluster_tree");

    m_offline_cluster_tree->Branch("offline_locX",&m_offline_locX);
    m_offline_cluster_tree->Branch("offline_locY",&m_offline_locY);
    m_offline_cluster_tree->Branch("offline_is_Pixel",&m_offline_isPixel);
    m_offline_cluster_tree->Branch("offline_is_Barrel",&m_offline_isBarrel);
    m_offline_cluster_tree->Branch("offline_layer",&m_offline_layer);
    m_offline_cluster_tree->Branch("offline_clustID",&m_offline_clustID);
    m_offline_cluster_tree->Branch("offline_trackNumber",&m_offline_trackNumber);

    m_offline_cluster_tree->Branch("offline_qoverpt",&m_offline_qoverpt);
    m_offline_cluster_tree->Branch("offline_eta",&m_offline_eta);
    m_offline_cluster_tree->Branch("offline_phi",&m_offline_phi);
    m_offline_cluster_tree->Branch("offline_d0",&m_offline_d0);
    m_offline_cluster_tree->Branch("offline_z0",&m_offline_z0);

    m_offline_cluster_tree->Branch("offline_barcode",&m_offline_barcode);
    m_offline_cluster_tree->Branch("offline_barcode_frac",&m_offline_barcode_frac);
  }

 // eventually add some histograms for monitoring?  
  return StatusCode::SUCCESS;
}



StatusCode HTTRawNtupleWrapperAlg::execute() {
  ATH_MSG_DEBUG (  "Start execute");
  bool last=false;
  ATH_CHECK (m_hitInputTool->readData(m_eventHeader, last));
   if (last) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG (*m_eventHeader);


 // reset the branches
  for (int ireg=0;ireg!=m_ntowers;++ireg) {
    m_original_hits[ireg].clear();
  }

  const HTTEventInfo event=m_eventHeader->event();
  m_run_number = event.runNumber(); // event's run number
  m_event_number = event.eventNumber(); // event number
  m_LB =  event.LB();
  m_BCID =  event.BCID();
  m_averageInteractionsPerCrossing =  event.averageInteractionsPerCrossing();
  m_actualInteractionsPerCrossing = event.actualInteractionsPerCrossing();
  m_level1TriggerType =  event.level1TriggerType();
  m_level1TriggerInfo =  event.level1TriggerInfo();
  m_extendedLevel1ID =  event.extendedLevel1ID();
  m_evtinfo->Fill();

  std::vector<HTTHit> fulllist = m_eventHeader->hits();
  ATH_MSG_VERBOSE("Going to run  on "<< fulllist.size()<<" hits");

  std::vector<HTTHit>::const_iterator ihit = fulllist.begin();
  std::vector<HTTHit>::const_iterator ihitE = fulllist.end();
  for (;ihit!=ihitE;++ihit) { // hit loop
    const HTTHit &currawhit = *ihit;
    for (int ireg=0;ireg!=m_ntowers;++ireg) {    
      // if the equivalent hit is compatible with this tower the hit is saved
      m_original_hits[ireg].push_back(currawhit);
    }
  }

   m_hittree->Fill();

   //truth tracks
    m_truth_tracks.clear();
    const std::vector<HTTTruthTrack> &truthtracks = m_eventHeader->optional().getTruthTracks();
    m_truth_tracks.insert(m_truth_tracks.end(),truthtracks.begin(),truthtracks.end());
  // Write the tracks
    m_trackstree->Fill();


  if (m_getOffline) {

    m_offline_locX->clear();
    m_offline_locY->clear();
    m_offline_isPixel->clear();
    m_offline_isBarrel->clear();
    m_offline_layer->clear();
    m_offline_clustID->clear();
    m_offline_trackNumber->clear();

    m_offline_qoverpt->clear();
    m_offline_eta->clear();
    m_offline_phi->clear();
    m_offline_d0->clear();
    m_offline_z0->clear();

    m_offline_barcode->clear();
    m_offline_barcode_frac->clear();

    const std::vector<HTTOfflineTrack> &offlinetracks = m_eventHeader->optional().getOfflineTracks();

    for (auto &offline_t: offlinetracks){

      m_offline_qoverpt->push_back(offline_t.getQOverPt());
      m_offline_eta->push_back(offline_t.getEta());
      m_offline_phi->push_back(offline_t.getPhi());
      m_offline_d0->push_back(offline_t.getD0());
      m_offline_z0->push_back(offline_t.getZ0());
      m_offline_barcode->push_back(offline_t.getBarcode());
      m_offline_barcode_frac->push_back(offline_t.getBarcodeFrac());

      for (auto &hit: offline_t.getOfflineHits()){
        m_offline_isPixel->push_back(hit.isPixel());
        m_offline_isBarrel->push_back(hit.isBarrel());
        m_offline_clustID->push_back(hit.getClusterID());
        m_offline_trackNumber->push_back(hit.getTrackNumber());
        m_offline_layer->push_back(hit.getLayer());
        m_offline_locX->push_back(hit.getLocX());
        m_offline_locY->push_back(hit.getLocY());
      }

    }

    m_offline_cluster_tree->Fill();
  }


  return StatusCode::SUCCESS;
}


StatusCode HTTRawNtupleWrapperAlg::finalize()
{
  // close the output files, but check that it exists (for athenaMT)
  if (m_outfile) {
    m_outfile->Write();
    m_outfile->Close();
  }

  return StatusCode::SUCCESS;
}
