/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SiHitAnalysis.h"

#include "GeoAdaptors/GeoSiHit.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

SiHitAnalysis::SiHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode SiHitAnalysis::initialize()
{
  ATH_MSG_DEBUG("Initializing SiHitAnalysis");

  std::string detName("Pixel");
  std::string ntupName("SiPixel");

  if (m_hitsContainerKey.key()=="PixelHits") {
    detName = "Pixel";
    ntupName = "SiPixel";
  }
  else if (m_hitsContainerKey.key()=="ITkPixelHits") {
    detName = "ITkPixel";
    ntupName = "SiITkPixel";
  }
  else if (m_hitsContainerKey.key()=="PLRHits") {
    detName = "PLR";
    ntupName = "SiPLR";
  }
  else if (m_hitsContainerKey.key()=="SCT_Hits") {
    detName = "SCT";
    ntupName = "SiSCT";
  }
  else if (m_hitsContainerKey.key()=="ITkStripHits") {
    detName = "ITkStrip";
    ntupName = "SiITkStrip";
  }
  else if (m_hitsContainerKey.key()=="BCMHits") {
    detName = "BCM";
    ntupName = "SiBCM";
  }
  else if (m_hitsContainerKey.key()=="BLMHits") {
    detName = "BLM";
    ntupName = "SiBLM";
  }
  else if (m_hitsContainerKey.key()=="HGTD_Hits") {
    detName = "HGTD";
    ntupName = "SiHGTD";
  }
  else {
    ATH_MSG_ERROR("SiHitsAnalysis for " << m_hitsContainerKey.key() << " not supported!!!");
    return StatusCode::FAILURE;
  }

  // Grab the Ntuple and histogramming service for the tree
  ATH_CHECK(m_thistSvc.retrieve());

  /** Histograms**/
  float bin_down = -600;
  float bin_up = 600;
  float radius_up = 600;
  float radius_down = 200;
  float z_max = 1200;
  if (detName == "Pixel") {
    bin_down = -170;
    bin_up = 170;
    radius_up = 170;
    radius_down = 0;
  } else if (detName == "ITkPixel") {
    bin_down = -325;
    bin_up = 325;
    radius_up = 325;
    radius_down = 0;
    z_max = 3000;
  } else if (detName == "ITkStrip") {
    bin_down = -1000;
    bin_up = 1000;
    radius_up = 1000;
    radius_down = 0;
    z_max = 3000;
  } else if (detName == "HGTD") {
    bin_down = -1000;
    bin_up = 1000;
    radius_up = 700;
    radius_down = 0;
    z_max = 4000;
  } else if (detName == "PLR") {
    bin_down = -125;
    bin_up = 125;
    radius_up = 125;
    radius_down = 0;
    z_max = 3000;
  }
  m_h_hits_x = new TH1D(("h_"+detName+"_x").c_str(),("h_"+detName+"_x").c_str(), 100,bin_down, bin_up);
  m_h_hits_x->StatOverflows();

  m_h_hits_y = new TH1D(("h_"+detName+"_y").c_str(), ("h_"+detName+"_y").c_str(), 100,bin_down,bin_up);
  m_h_hits_y->StatOverflows();

  m_h_hits_z = new TH1D(("h_"+detName+"_z").c_str(), ("h_"+detName+"_z").c_str(), 200,-z_max,z_max);
  m_h_hits_z->StatOverflows();

  m_h_hits_r = new TH1D(("h_"+detName+"_r").c_str(), ("h_"+detName+"_r").c_str(), 100,radius_down,radius_up);
  m_h_hits_r->StatOverflows();

  m_h_xy = new TH2D(("h_"+detName+"_xy").c_str(), ("h_"+detName+"_xy").c_str(), 100,bin_down,bin_up,100, bin_down, bin_up);
  m_h_xy->StatOverflows();

  m_h_zr = new TH2D(("h_"+detName+"_zr").c_str(), ("h_"+detName+"_zr").c_str(), 100,-z_max, z_max, 100, radius_down, radius_up);
  m_h_zr->StatOverflows();

  m_h_hits_time = new TH1D(("h_"+detName+"_time").c_str(), ("h_"+detName+"_time").c_str(), 100,0,500);
  m_h_hits_time->StatOverflows();

  m_h_hits_eloss = new TH1D(("h_"+detName+"_eloss").c_str(), ("h_"+detName+"_eloss").c_str(), 100,0,50);
  m_h_hits_eloss->StatOverflows();

  m_h_hits_step = new TH1D(("h_"+detName+"_step").c_str(), ("h_"+detName+"_step").c_str(), 100,0,50);
  m_h_hits_step->StatOverflows();

  m_h_hits_barcode = new TH1D(("h_"+detName+"_barcode").c_str(), ("h_"+detName+"_barcode").c_str(), 200,0,250000);
  m_h_hits_barcode->StatOverflows();

  m_h_time_eloss = new TH2D(("h_"+detName+"_time_eloss").c_str(), ("h_"+detName+" Eloss vs. time").c_str(),100, 0,500,100,0,50);
  m_h_time_eloss->StatOverflows();

  m_h_z_eloss = new TH2D(("h_"+detName+"_z_eloss").c_str(), ("h_"+detName+" Eloss vs. z").c_str(),100, -z_max,z_max,100,0,50);
  m_h_z_eloss->StatOverflows();

  m_h_r_eloss = new TH2D(("h_"+detName+"_r_eloss").c_str(), ("h_"+detName+ " Eloss vs. r").c_str(),100, radius_down,radius_down,100,0,50);
  m_h_r_eloss->StatOverflows();

  m_h_barrel_endcap = new TH1D(("h_"+detName+"_barrel_endcap").c_str(), ("h_"+detName+ " barrel/endcap").c_str(), 10, -5, 5);
  m_h_barrel_endcap->StatOverflows();

  m_h_layer_disk = new TH1D(("h_"+detName+"_layer_disk").c_str(), ("h_"+detName+ " layer/disk").c_str(), 10, 0, 10);
  m_h_layer_disk->StatOverflows();

  m_h_module_eta = new TH1D(("h_"+detName+"_module_eta").c_str(), ("h_"+detName+ " module in #eta").c_str(), 100, 0, 100);
  m_h_module_eta->StatOverflows();

  m_h_module_phi = new TH1D(("h_"+detName+"_module_phi").c_str(), ("h_"+detName+ " module in #phi").c_str(), 100, 0, 100);
  m_h_module_phi->StatOverflows();

  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_x->GetName(), m_h_hits_x));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_y->GetName(), m_h_hits_y));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_z->GetName(), m_h_hits_z));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_r->GetName(), m_h_hits_r));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_xy->GetName(), m_h_xy));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_zr->GetName(), m_h_zr));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_time->GetName(), m_h_hits_time));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_eloss->GetName(), m_h_hits_eloss));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_step->GetName(), m_h_hits_step));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_hits_barcode->GetName(), m_h_hits_barcode));

  //To be filled only when the expert mode is on.
  if (m_expert.value()) {
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_time_eloss->GetName(), m_h_time_eloss));
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_z_eloss->GetName(), m_h_z_eloss));
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_r_eloss->GetName(), m_h_r_eloss));
  }

  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_barrel_endcap->GetName(), m_h_barrel_endcap));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_layer_disk->GetName(), m_h_layer_disk));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_module_eta->GetName(), m_h_module_eta));
  ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_module_phi->GetName(), m_h_module_phi));

  ATH_CHECK(m_thistSvc.retrieve());

  /** now add branches and leaves to the tree */
  m_tree = new TTree(ntupName.c_str(), ntupName.c_str());
  std::string fullNtupleName =  "/" + m_ntuplePath + "/" + detName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));

  if (m_tree){
    m_tree->Branch((detName+"_x").c_str(), &m_hits_x);
    m_tree->Branch((detName+"_y").c_str(), &m_hits_y);
    m_tree->Branch((detName+"_z").c_str(), &m_hits_z);
    m_tree->Branch((detName+"_r").c_str(), &m_hits_r);
    m_tree->Branch((detName+"_time").c_str(), &m_hits_time);
    m_tree->Branch((detName+"_eloss").c_str(), &m_hits_eloss);
    m_tree->Branch((detName+"_step").c_str(), &m_hits_step);
    m_tree->Branch((detName+"_barcode").c_str(), &m_hits_barcode);
    if (m_extraTruthBranches) {
      m_tree->Branch((detName+"_pdgId").c_str(), &m_hits_pdgId);
      m_tree->Branch((detName+"_pT").c_str(), &m_hits_pT);
      m_tree->Branch((detName+"_eta").c_str(), &m_hits_eta);
      m_tree->Branch((detName+"_phi").c_str(), &m_hits_phi);
      m_tree->Branch((detName+"_prodVtx_x").c_str(), &m_hits_prodVtx_x);
      m_tree->Branch((detName+"_prodVtx_y").c_str(), &m_hits_prodVtx_y);
      m_tree->Branch((detName+"_prodVtx_z").c_str(), &m_hits_prodVtx_z);
    }

    m_tree->Branch((detName+"_barrel_endcap").c_str(), &m_barrel_endcap);
    m_tree->Branch((detName+"_layer_disk").c_str(), &m_layer_disk);
    m_tree->Branch((detName+"_module_eta").c_str(), &m_module_eta);
    m_tree->Branch((detName+"_module_phi").c_str(), &m_module_phi);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // Initialize keys
  ATH_CHECK(m_hitsContainerKey.initialize());

  return StatusCode::SUCCESS;
}


StatusCode SiHitAnalysis::execute()
{
  ATH_MSG_DEBUG("In SiHitAnalysis::execute()");

  m_hits_x->clear();
  m_hits_y->clear();
  m_hits_z->clear();
  m_hits_r->clear();
  m_hits_time->clear();
  m_hits_eloss->clear();
  m_hits_step->clear();
  m_hits_barcode->clear();
  if (m_extraTruthBranches) {
    m_hits_pdgId->clear();
    m_hits_pT->clear();
    m_hits_eta->clear();
    m_hits_phi->clear();
    m_hits_prodVtx_x->clear();
    m_hits_prodVtx_y->clear();
    m_hits_prodVtx_z->clear();
  }

  m_barrel_endcap->clear();
  m_layer_disk->clear();
  m_module_eta->clear();
  m_module_phi->clear();

  SG::ReadHandle<SiHitCollection> hitCollection(m_hitsContainerKey);
  if (hitCollection.isValid()) {
    ATH_MSG_INFO("Event contains " << hitCollection->size() << " entries in " << m_hitsContainerKey.key());
    for (const SiHit &hit : *hitCollection) {
      GeoSiHit ghit(hit);
      HepGeom::Point3D<double> p = ghit.getGlobalPosition();
      m_h_hits_x->Fill(p.x());
      m_h_hits_y->Fill(p.y());
      m_h_hits_z->Fill(p.z());
      m_h_hits_r->Fill(p.perp());
      m_h_xy->Fill(p.x(), p.y());
      m_h_zr->Fill(p.z(), p.perp());
      m_h_hits_eloss->Fill(hit.energyLoss());
      m_h_hits_time->Fill(hit.meanTime());
      double step_length = (hit.localStartPosition() - hit.localEndPosition()).mag();
      m_h_hits_step->Fill(step_length);
      m_h_hits_barcode->Fill(hit.particleLink().barcode());

      if (m_expert.value()) {
        m_h_time_eloss->Fill(hit.meanTime(), hit.energyLoss());
        if (hit.getBarrelEndcap() == 0) {
          m_h_z_eloss->Fill(p.z(), hit.energyLoss());
        }
        else {
          m_h_r_eloss->Fill(p.perp(), hit.energyLoss());
        }
      }

      m_h_barrel_endcap->Fill(hit.getBarrelEndcap());
      m_h_layer_disk->Fill(hit.getLayerDisk());
      m_h_module_eta->Fill(hit.getEtaModule());
      m_h_module_phi->Fill(hit.getPhiModule());

      m_hits_x->push_back(p.x());
      m_hits_y->push_back(p.y());
      m_hits_z->push_back(p.z());
      m_hits_r->push_back(p.perp());
      m_hits_eloss->push_back(hit.energyLoss());
      m_hits_time->push_back(hit.meanTime());
      m_hits_step->push_back(step_length);
      m_hits_barcode->push_back(hit.particleLink().barcode());
      if (m_extraTruthBranches) {
        auto tpl = hit.particleLink();
        if (tpl.isValid()) {
          m_hits_pdgId->push_back(tpl->pdg_id());
          m_hits_pT->push_back(tpl->momentum().perp());
          m_hits_eta->push_back(tpl->momentum().eta());
          m_hits_phi->push_back(tpl->momentum().phi());
          m_hits_prodVtx_x->push_back(tpl->production_vertex()->position().x());
          m_hits_prodVtx_y->push_back(tpl->production_vertex()->position().y());
          m_hits_prodVtx_z->push_back(tpl->production_vertex()->position().z());
        }
        else {
          m_hits_pdgId->push_back(-9999);
          m_hits_pT->push_back(-9999);
          m_hits_eta->push_back(-9999);
          m_hits_phi->push_back(-9999);
          m_hits_prodVtx_x->push_back(-9999);
          m_hits_prodVtx_y->push_back(-9999);
          m_hits_prodVtx_z->push_back(-9999);
        }
      }

      m_barrel_endcap->push_back(hit.getBarrelEndcap());
      m_layer_disk->push_back(hit.getLayerDisk());
      m_module_eta->push_back(hit.getEtaModule());
      m_module_phi->push_back(hit.getPhiModule());
    } // End while hits
  } else { // End statuscode success upon retrieval of hits
    ATH_MSG_ERROR("Invalid collection");
    return StatusCode::FAILURE;
  }

  if (m_tree != nullptr) m_tree->Fill();

  return StatusCode::SUCCESS;
}
