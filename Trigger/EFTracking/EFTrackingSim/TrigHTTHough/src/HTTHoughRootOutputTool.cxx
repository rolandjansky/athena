/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTHough/HTTHoughRootOutputTool.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTObjects/HTTFunctions.h"

/////////////////////////////////////////////////////////////////////////////
HTTHoughRootOutputTool::HTTHoughRootOutputTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
  AthAlgTool(algname, name, ifc)
{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHoughRootOutputTool::initialize()
{
  ATH_CHECK(m_HTTMapping.retrieve());
  ATH_CHECK(m_tHistSvc.retrieve());
  ATH_CHECK(m_EvtSel.retrieve());

  ATH_CHECK(bookTree());
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTHoughRootOutputTool::bookTree()
{
  m_tree = new TTree("HTTHoughRootOutput","HTTHoughRootOutput");
  m_tree->Branch("x",&m_x);
  m_tree->Branch("y",&m_y);
  m_tree->Branch("z",&m_z);
  m_tree->Branch("phi",&m_phi);
  m_tree->Branch("invpt",&m_invpt);
  m_tree->Branch("tracknumber",&m_tracknumber);
  m_tree->Branch("roadnumber",&m_roadnumber);
  m_tree->Branch("barcode",&m_barcode);
  m_tree->Branch("barcodefrac",&m_barcodefrac);
  m_tree->Branch("eventindex",&m_eventindex);
  m_tree->Branch("isPixel",&m_isPixel);
  m_tree->Branch("layer",&m_layer);
  m_tree->Branch("isBarrel",&m_isBarrel);
  m_tree->Branch("etawidth",&m_etawidth);
  m_tree->Branch("phiwidth",&m_phiwidth);
  m_tree->Branch("etamodule",&m_etamodule);
  m_tree->Branch("phimodule",&m_phimodule);
  m_tree->Branch("ID",&m_ID);
  m_tree->Branch("candidate_barcodefrac",&m_candidate_barcodefrac);
  m_tree->Branch("candidate_barcode",&m_candidate_barcode);
  m_tree->Branch("candidate_eventindex",&m_candidate_eventindex);
  m_tree->Branch("treeindex",&m_treeindex);

  m_treeindex = 0;

  m_truthtree = new TTree("HTTTruthTree","HTTTruthTree");
  m_truthtree->Branch("truth_d0",&m_truth_d0);
  m_truthtree->Branch("truth_z0",&m_truth_z0);
  m_truthtree->Branch("truth_pt",&m_truth_pt);
  m_truthtree->Branch("truth_eta",&m_truth_eta);
  m_truthtree->Branch("truth_phi",&m_truth_phi);
  m_truthtree->Branch("truth_q",&m_truth_q);
  m_truthtree->Branch("truth_pdg",&m_truth_pdg);
  m_truthtree->Branch("truth_barcode",&m_truth_barcode);
  m_truthtree->Branch("truth_eventindex",&m_truth_eventindex);


  m_offlinetree = new TTree("HTTOfflineTree","HTTOfflineTree");
  m_offlinetree->Branch("offline_d0",&m_offline_d0);
  m_offlinetree->Branch("offline_z0",&m_offline_z0);
  m_offlinetree->Branch("offline_pt",&m_offline_pt);
  m_offlinetree->Branch("offline_eta",&m_offline_eta);
  m_offlinetree->Branch("offline_phi",&m_offline_phi);
  m_offlinetree->Branch("offline_q",&m_offline_q);
  m_offlinetree->Branch("offline_barcode",&m_offline_barcode);
  m_offlinetree->Branch("offline_barcodefrac",&m_offline_barcodefrac);
  m_offlinetree->Branch("offline_n_holes",&m_offline_n_holes);
  m_offlinetree->Branch("offline_n_inertmaterial",&m_offline_n_inertmaterial);
  m_offlinetree->Branch("offline_n_measurement",&m_offline_n_measurement);
  m_offlinetree->Branch("offline_n_brempoint",&m_offline_n_brempoint);
  m_offlinetree->Branch("offline_n_scatterer",&m_offline_n_scatterer);
  m_offlinetree->Branch("offline_n_perigee",&m_offline_n_perigee);
  m_offlinetree->Branch("offline_n_outlier",&m_offline_n_outlier);
  m_offlinetree->Branch("offline_n_other",&m_offline_n_other);

  ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTHOUGHOUTPUT/%s",m_tree->GetName()), m_tree));
  ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTHOUGHOUTPUT/%s",m_truthtree->GetName()), m_truthtree));
  ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTHOUGHOUTPUT/%s",m_offlinetree->GetName()), m_offlinetree));

  return StatusCode::SUCCESS;
}



StatusCode HTTHoughRootOutputTool::fillTree(std::vector<HTTRoad*> &roads, const std::vector<HTTTruthTrack> &truthTracks, const std::vector<HTTOfflineTrack> &offlineTracks)
{
  m_tracknumber = 0;
  ResetVectors();

  // fill the truth tree, simply once per event! to know which entry here to loop at for a given road or hit
  // combination below, use treeindex from below to find the entry here
  for (auto track : truthTracks) {
    if (!m_EvtSel->passCuts(track)) continue;

    m_truth_d0.push_back(track.getD0());
    m_truth_z0.push_back(track.getVtxZ());
    m_truth_pt.push_back(track.getPt());
    m_truth_eta.push_back(track.getEta());
    m_truth_phi.push_back(track.getPhi());
    m_truth_barcode.push_back(track.getBarcode());
    m_truth_eventindex.push_back(track.getEventIndex());    
    m_truth_q.push_back(track.getQ());
    m_truth_pdg.push_back(track.getPDGCode());
  }
  m_truthtree->Fill();

  // now do the same for offline tree, once per event
  for (auto track : offlineTracks) {
    if (!m_EvtSel->passCuts(track)) continue;

    m_offline_d0.push_back(track.getD0());
    m_offline_z0.push_back(track.getZ0());
    m_offline_pt.push_back(abs(1./track.getQOverPt()));
    m_offline_eta.push_back(track.getEta());
    m_offline_phi.push_back(track.getPhi());
    m_offline_barcode.push_back(track.getBarcode());
    m_offline_barcodefrac.push_back(track.getBarcodeFrac());

    m_offline_q.push_back(track.getQOverPt() > 0 ? 1 : -1);
    int nhole(0), nmeasurement(0), ninert(0), nbrem(0), nscatter(0), nperigee(0), noutlier(0), nother(0);
    for (auto hit : track.getOfflineHits()) {
      if (hit.getHitType() == OfflineHitType::Measurement) nmeasurement++;
      else if (hit.getHitType() == OfflineHitType::InertMaterial) ninert++;
      else if (hit.getHitType() == OfflineHitType::BremPoint) nbrem++;
      else if (hit.getHitType() == OfflineHitType::Scatterer) nscatter++;
      else if (hit.getHitType() == OfflineHitType::Perigee) nperigee++;
      else if (hit.getHitType() == OfflineHitType::Outlier) noutlier++;
      else if (hit.getHitType() == OfflineHitType::Hole) nhole++;
      else nother++;
    }
    m_offline_n_holes.push_back(nhole);
    m_offline_n_measurement.push_back(nmeasurement);
    m_offline_n_inertmaterial.push_back(ninert);
    m_offline_n_brempoint.push_back(nbrem);
    m_offline_n_scatterer.push_back(nscatter);
    m_offline_n_perigee.push_back(nperigee);
    m_offline_n_outlier.push_back(noutlier);
    m_offline_n_other.push_back(nother);
  }
  m_offlinetree->Fill();

  // for calculating the truth for the entire combination, not just an individual hit
  std::vector<HTTMultiTruth> mtv;
  mtv.reserve( m_HTTMapping->PlaneMap_1st()->getNLogiLayers());


  // now we loop over all the roads. The tree is stored per combination, not per event, so there is an tree index
  // that can be used to map to the above
  for (size_t iroad = 0; iroad < roads.size(); iroad++) {
    m_roadnumber = iroad;
    HTTRoad_Hough *road = dynamic_cast<HTTRoad_Hough*>(roads[iroad]);
    if (road == nullptr) continue; // Not Hough roads

    std::vector<std::vector<int>> combs = ::getComboIndices(road->getNHits_layer());
    m_phi = road->getX();
    m_invpt = road->getY();

    // Add the hits from each combination to the tree
    for (size_t icomb = 0; icomb < combs.size(); icomb++)
      {
	mtv.clear();
	
        std::vector<int> const & hit_indices = combs[icomb]; // size nLayers
        for (unsigned layer = 0; layer < m_HTTMapping->PlaneMap_1st()->getNLogiLayers(); layer++)
	  {
	    if (hit_indices[layer] >= 0) {
	      const HTTHit* hit = road->getHits(layer)[hit_indices[layer]];

	      HTTMultiTruth truth = hit->getTruth();

	      truth.assign_equal_normalization();
	      mtv.push_back( truth );

	      HTTMultiTruth::Barcode tbarcode;
	      HTTMultiTruth::Weight tfrac;
	      const bool ok = truth.best(tbarcode,tfrac);
	      if( ok ) {
		m_eventindex.push_back((int)(tbarcode.first));
		m_barcode.push_back((int)(tbarcode.second));
		m_barcodefrac.push_back(tfrac);
	      }
	      else {
		m_eventindex.push_back(-1);
		m_barcode.push_back(-1);
		m_barcodefrac.push_back(-1);
	      }

	      m_x.push_back(hit->getX());
	      m_y.push_back(hit->getY());
	      m_z.push_back(hit->getZ());
	      m_isPixel.push_back(hit->isPixel() ? 1 : 0);
	      m_layer.push_back(hit->getLayer());
	      m_isBarrel.push_back(hit->isBarrel() ? 1 : 0);
	      m_etawidth.push_back(hit->getEtaWidth());
	      m_phiwidth.push_back(hit->getPhiWidth());
	      m_etamodule.push_back(hit->getHTTEtaModule());
	      m_phimodule.push_back(hit->getPhiModule());
	      m_ID.push_back(hit->getHTTIdentifierHash());
            }
	  }
	// done looping over hits, now we do the truth calculation for this track candidate
	// first compute the best geant match, the barcode with the largest number of contributing hits
	// frac is then the fraction of the total number of hits on the candidate attributed to the barcode.
	HTTMultiTruth mt( std::accumulate(mtv.begin(),mtv.end(),HTTMultiTruth(),HTTMultiTruth::AddAccumulator()) );
	// retrieve the best barcode and frac and store it
	HTTMultiTruth::Barcode tbarcode;
	HTTMultiTruth::Weight tfrac;
	const bool ok = mt.best(tbarcode,tfrac);
	if( ok ) {
	  m_candidate_eventindex = (int)(tbarcode.first);
	  m_candidate_barcode = (int)(tbarcode.second);
	  m_candidate_barcodefrac = tfrac;
	}	
	else {
	  m_candidate_eventindex = -1;
	  m_candidate_barcode = -1;
	  m_candidate_barcodefrac = 0;
	}
	m_tree->Fill();
	ResetVectors();
	m_tracknumber++;
      }
  }
  m_treeindex++;
  return StatusCode::SUCCESS;
}

void HTTHoughRootOutputTool::ResetVectors() {
  m_x.clear();
  m_y.clear();
  m_z.clear();
  m_barcode.clear();
  m_barcodefrac.clear();
  m_eventindex.clear();
  m_isPixel.clear();
  m_layer.clear();
  m_isBarrel.clear();
  m_etawidth.clear();
  m_phiwidth.clear();
  m_etamodule.clear();
  m_phimodule.clear();
  m_ID.clear();
  m_truth_d0.clear();
  m_truth_z0.clear();
  m_truth_pt.clear();
  m_truth_eta.clear();
  m_truth_phi.clear();
  m_truth_q.clear();
  m_truth_barcode.clear();
  m_truth_eventindex.clear();
  m_truth_pdg.clear();
  m_offline_n_holes.clear();
  m_offline_n_measurement.clear();
  m_offline_n_inertmaterial.clear();
  m_offline_n_brempoint.clear();
  m_offline_n_scatterer.clear();
  m_offline_n_perigee.clear();
  m_offline_n_outlier.clear();
  m_offline_n_other.clear();
  m_offline_d0.clear();
  m_offline_z0.clear();
  m_offline_pt.clear();
  m_offline_eta.clear();
  m_offline_phi.clear();
  m_offline_q.clear();
  m_offline_barcode.clear();
  m_offline_barcodefrac.clear();
}
