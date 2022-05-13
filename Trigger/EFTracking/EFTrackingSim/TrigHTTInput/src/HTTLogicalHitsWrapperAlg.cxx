#include "HTTLogicalHitsWrapperAlg.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"

#include "TH1F.h"
#include "TH2F.h"

HTTLogicalHitsWrapperAlg::HTTLogicalHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{}


StatusCode HTTLogicalHitsWrapperAlg::initialize()
{
  ATH_CHECK( m_hitInputTool.retrieve());
  ATH_CHECK( m_writeOutputTool.retrieve());
  ATH_CHECK( m_hitMapTool.retrieve());
  
  if (m_Clustering ) {
    ATH_CHECK( m_clusteringTool.retrieve());
    ATH_MSG_INFO ("Clustering is enabled");
  }

  if (m_Monitor)  ATH_CHECK( BookHistograms());
  return StatusCode::SUCCESS;
}

StatusCode HTTLogicalHitsWrapperAlg::BookHistograms(){

  m_hits_r_vs_z = new TH2F("h_hits_r_vs_z", "r/z ITK hit map; z[mm];r[mm]", 3500, 0., 3500., 1400, 0., 1400.);
  m_hits_barrelEC = new TH1F("h_hits_barrelEC", "barrelEC of hits", 10, 0., 10.);

  m_nMissedHit = new TH1F("h_nMissedHit", "n. of missing hits/event", 100, 0.,1000000.);
  m_nMissedHitFrac = new TH1F("h_nMissedHitFrac", "Fraction of missing hits/event", 100, 0.,1.);
  m_MissedHit_eta = new TH1F("h_MissedHit_eta", "eta of missing hits", 100, -10., 10.);
  m_MissedHit_phi = new TH1F("h_MissedHit_phi", "phi of missing hits", 100, 0., 100.);
  m_MissedHit_type = new TH1F("h_MissedHit_type", "type of missing hits", 10, 0., 10.);
  m_MissedHit_barrelEC = new TH1F("h_MissedHit_barrelEC", "barrelEC of missing hits", 10, 0., 10.);
  m_MissedHit_layer = new TH1F("h_MissedHit_layer", "layer of missing hits", 20, 0., 20.);

  return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsWrapperAlg::execute()
{
  HTTEventInputHeader         eventHeader;
  
  bool last = false;
  ATH_CHECK (m_hitInputTool->readData(&eventHeader, last));
  if (last) return StatusCode::SUCCESS;

  ATH_MSG_DEBUG (eventHeader);
  
  if (m_Monitor) {
    // fill histograms
    for (const auto& hit: eventHeader.hits()) {
      float r= std::sqrt(hit.getX()*hit.getX() + hit.getY()*hit.getY());
      m_hits_barrelEC->  Fill(static_cast<int>(hit.getDetectorZone()));
      m_hits_r_vs_z->    Fill(hit.getZ(), r);
    }
  }

  // Map hits:
  HTTLogicalEventInputHeader  logicEventHeader_1st;
  ATH_CHECK(m_hitMapTool->convert(1, eventHeader, logicEventHeader_1st));
  ATH_MSG_DEBUG (logicEventHeader_1st);

  // clustering:
  if (m_Clustering) {
    std::vector<HTTCluster> clusters;
    ATH_CHECK(m_clusteringTool->DoClustering(logicEventHeader_1st, clusters));
    ATH_MSG_INFO ("Ending with " << clusters.size() << " clusters");
  }
  // get unmapped hits for debuggin purpose
  if (m_Monitor) {
    ATH_MSG_DEBUG ("Filling histormas");
    std::vector<HTTHit> missing_hits;
    ATH_CHECK(m_hitMapTool->getUnmapped(missing_hits));

    std::vector<HTTHit> fulllist= eventHeader.hits();
    m_nMissedHit->Fill(missing_hits.size());
    m_nMissedHitFrac->Fill(float(missing_hits.size())/ float(fulllist.size()));
    for (const auto& hit: missing_hits){
      m_MissedHit_eta->     Fill(hit.getHTTEtaModule()) ;
      m_MissedHit_phi->     Fill(hit.getPhiModule());
      m_MissedHit_type->    Fill(static_cast<int>(hit.getDetType()));
      m_MissedHit_barrelEC->Fill(static_cast<int>(hit.getDetectorZone()));
      m_MissedHit_layer->   Fill(hit.getLayer());
    }
  }

  
  HTTLogicalEventInputHeader  logicEventHeader_2nd;//fake empty
  HTTLogicalEventOutputHeader logicEventOutputHeader;
  ATH_CHECK (m_writeOutputTool->writeData(&logicEventHeader_1st, &logicEventHeader_2nd, &logicEventOutputHeader));

  return StatusCode::SUCCESS;
}

