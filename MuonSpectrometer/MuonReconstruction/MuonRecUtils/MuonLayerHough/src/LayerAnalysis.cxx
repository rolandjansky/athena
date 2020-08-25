/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/LayerAnalysis.h"

#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TROOT.h"
#include "TTree.h"
#include "TString.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TVector3.h"
#include <set>
#include <cmath>

#include <iostream>
#include <cstdlib>
#include <algorithm>

namespace MuonHough {
  
  Plots::Plots(const char* title, int nBinsX, float xMin, float xMax, int nBinsY, float yMin, float yMax){
    Reco           = new TH2F(title, title, nBinsX, xMin, xMax, nBinsY, yMin, yMax);
    Truth          = new TH2F(Form("Truth_%s",title), Form("Truth_%s",title), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
    Matched        = new TH2F(Form("Matched_%s" ,title), Form("Matched_%s" ,title), nBinsX, xMin, xMax, nBinsY, yMin, yMax); 
    Unmatched      = new TH2F(Form("Unmatched_%s" ,title), Form("Unmatched_%s" ,title), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
    Efficiency     = new TH1F(Form("Eff_%s", title), Form("Eff_%s", title), nBinsY, yMin, yMax); 
    FakeEfficiency = new TH1F(Form("FakeEff_%s", title), Form("FakeEff_%s", title), nBinsY, yMin, yMax); 
    Diff           = new TH2F(Form("Diff_%s",title), Form("Diff_%s",title), nBinsX, xMin, xMax, 21, -10.5, 10.5);
  }
  
  void LayerAnalysis::analyse() {
    gROOT->SetBatch(true);
    TH1::AddDirectory(false);
    initialize();//initialize all the plots
    Long64_t nentries = m_tree->GetEntries();
    m_ncalls = 0;
    std::cout << " analysing events " << nentries << std::endl;
    m_ntuple.initForRead(*m_tree);
    if (m_DEBUG_seg){std::cout << " here " << nentries << std::endl; m_ntuple.initForReadseg(*m_tree);}
    std::cout << " after initialize events " << nentries << std::endl;

    m_selectors.resize(Muon::MuonStationIndex::ChIndexMax);//initialzie all the selectors
    m_selectors[Muon::MuonStationIndex::BIS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::BIL] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::BMS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});  
    m_selectors[Muon::MuonStationIndex::BML] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});  
    m_selectors[Muon::MuonStationIndex::BOS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::BOL] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::BEE] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EIS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EIL] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EMS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EML] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EOS] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EOL] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EES] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});
    m_selectors[Muon::MuonStationIndex::EEL] = MuonHough::MuonLayerHoughSelector({std::make_pair(0,1.9)});

    if (m_DEBUG) nentries = 200;//for debugging purpose
    for( Long64_t evt=0; evt<nentries; ++evt ){
      if (evt % 1000 == 0) std::cout << "event: " << evt << std::endl;
      m_tree->LoadTree(evt);
      m_tree->GetEntry(evt);
      m_event.reset();//event data
	    m_muons.clear();
      m_segs.clear();
      if(!m_ntuple.read(m_event,m_muons)) {std::cout << "oops bad event: " << evt << std::endl; continue;}
      if (m_DEBUG_seg && !m_ntuple.readseg(m_segs)) {std::cout << "oops bad segment: " << evt << std::endl; continue;}
      if (m_DEBUG_seg){
        for (auto seg = m_segs.begin(); seg != m_segs.end(); seg++){
          TVector3 globalPos(seg->sposx, seg->sposy, seg->sposz);
          TVector3 globalDir(seg->sdirx, seg->sdiry, seg->sdirz);
          //has to do a transformation here to calcualte! or the phi effect is in
          double globalPos_theta = globalPos.Theta();
          double globalDir_theta = globalDir.Theta();
          std::cout << " @@truth segment " << " sector " << seg->sSector << " chamber " << seg->sChIndex << 
          " r " << globalPos.Perp() <<  " z " <<  globalPos.Z() <<  " postheta " << globalPos_theta << 
          " dirtheta " << M_PI - globalDir_theta << std::endl;
        }
      } 

      if (m_DEBUG) std::cout << "DEBUG: start analyse event evt " << evt << std::endl;
      analysis( m_event.sectors );
      ++m_ncalls;
    }
    finalize();
  }

  void LayerAnalysis::initialize() {
    SetStyle();
    m_h_dtheta = new TH1F("dtheta", "RMS of the #Delta #theta for each clutser; #theta RMS", 315, -0.01, M_PI);
    m_h_dtheta_truth = new TH1F("dtheta_truth", "RMS of the #Delta #theta for each truth clutser; #theta RMS", 315, -M_PI, M_PI);
    m_hMaximaHeightPerChIndex.resize(Muon::MuonStationIndex::ChIndexMax);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BIS] = new Plots("max_BIS", /* 20, 0, 10000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BIL] = new Plots("max_BIL", /* 20, 0, 10000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BMS] = new Plots("max_BMS", /* 24, 0, 12000,*/ 16, 0, 3.2, 20, 0, 20);  
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BML] = new Plots("max_BML", /* 24, 0, 12000,*/ 16, 0, 3.2, 20, 0, 20);  
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BOS] = new Plots("max_BOS", /* 30, 0, 15000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BOL] = new Plots("max_BOL", /* 30, 0, 15000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::BEE] = new Plots("max_BEE", /*100, 0, 40000,*/ 16, 0, 3.2, 25, 0, 25);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EIS] = new Plots("max_EIS", /* 16, 0,  8000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EIL] = new Plots("max_EIL", /* 16, 0,  8000,*/ 16, 0, 3.2, 15, 0, 15);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EMS] = new Plots("max_EMS", /* 30, 0, 15000,*/ 16, 0, 3.2, 30, 0, 30);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EML] = new Plots("max_EML", /* 30, 0, 15000,*/ 16, 0, 3.2, 30, 0, 30);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EOS] = new Plots("max_EOS", /* 30, 0, 15000,*/ 16, 0, 3.2, 10, 0, 10);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EOL] = new Plots("max_EOL", /* 30, 0, 15000,*/ 16, 0, 3.2, 10, 0, 10);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EES] = new Plots("max_EES", /*100, 0, 40000,*/ 16, 0, 3.2, 25, 0, 25);
    m_hMaximaHeightPerChIndex[Muon::MuonStationIndex::EEL] = new Plots("max_EEL", /*100, 0, 40000,*/ 16, 0, 3.2, 25, 0, 25);
    
    float diffbins = 800; float diffxmax = 3000;
    m_h_diff_MI_e = new TH1F("h_diff_MI_endcap", "h_diff_MI_endcap; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MO_e = new TH1F("h_diff_MO_endcap", "h_diff_MO_endcap; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MI_b = new TH1F("h_diff_MI_barrel", "h_diff_MI_barrel; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MO_b = new TH1F("h_diff_MO_barrel", "h_diff_MO_barrel; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MI_e_truth = new TH1F("h_diff_MI_endcap_truth", "h_diff_MI_endcap_truth; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MO_e_truth = new TH1F("h_diff_MO_endcap_truth", "h_diff_MO_endcap_truth; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MI_b_truth = new TH1F("h_diff_MI_barrel_truth", "h_diff_MI_barrel_truth; diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_diff_MO_b_truth = new TH1F("h_diff_MO_barrel_truth", "h_diff_MO_barrel_truth; diff, mm", diffbins, -diffxmax, diffxmax);
    //exptaploated plots
    //diffbins = 100; diffxmax = 0.5;
    m_h_expt_MI_e = new TH1F("h_expt_MI_endcap", "h_expt_MI_endcap; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MO_e = new TH1F("h_expt_MO_endcap", "h_expt_MO_endcap; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MI_b = new TH1F("h_expt_MI_barrel", "h_expt_MI_barrel; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MO_b = new TH1F("h_expt_MO_barrel", "h_expt_MO_barrel; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MI_e_truth = new TH1F("h_expt_MI_endcap_truth", "h_expt_MI_endcap_truth; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MO_e_truth = new TH1F("h_expt_MO_endcap_truth", "h_expt_MO_endcap_truth; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MI_b_truth = new TH1F("h_expt_MI_barrel_truth", "h_expt_MI_barrel_truth; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    m_h_expt_MO_b_truth = new TH1F("h_expt_MO_barrel_truth", "h_expt_MO_barrel_truth; extrapolated diff, mm", diffbins, -diffxmax, diffxmax);
    //compare the two extrapolations
    float compbins = 800; float compxmax = 1500;
    m_h_comp_MI_e = new TH1F("h_comp_MI_endcap", "h_comp_MI_endcap; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MO_e = new TH1F("h_comp_MO_endcap", "h_comp_MO_endcap; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MI_b = new TH1F("h_comp_MI_barrel", "h_comp_MI_barrel; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MO_b = new TH1F("h_comp_MO_barrel", "h_comp_MO_barrel; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MI_e_truth = new TH1F("h_comp_MI_endcap_truth", "h_comp_MI_endcap_truth; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MO_e_truth = new TH1F("h_comp_MO_endcap_truth", "h_comp_MO_endcap_truth; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MI_b_truth = new TH1F("h_comp_MI_barrel_truth", "h_comp_MI_barrel_truth; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    m_h_comp_MO_b_truth = new TH1F("h_comp_MO_barrel_truth", "h_comp_MO_barrel_truth; two extrapolation diff, mm", compbins, -compxmax, compxmax);
    //set basic settings
    m_DEBUG=false;//for understanding output
    m_DEBUG_seg=false;//for more debug output
  }

  void LayerAnalysis::analysis( std::map<int,SectorData>& data ) {
    MuonDetectorHough detectorHough;
    MuonDetectorHough detectorHoughTruth; 
    detectorHough.reset();
    detectorHoughTruth.reset(); 
    std::set<DataIndex> plotRegion;
    std::vector<std::vector<MuonLayerHough::Maximum>> plotMaximum;//save the maximum information here
    std::vector<std::vector<MuonLayerHough::Maximum>> plotMaximum_truth;//save the maximum information here
    std::set<int> maxset;
    if (m_DEBUG) std::cout << "DEBUG: the size of this data event(ROI) " << data.size() << std::endl;
    for(std::map<int,SectorData>::const_iterator it = data.begin() ;it != data.end();++it ){//looping through region?
      if (m_DEBUG) std::cout << "DEBUG: the size of the hit clusters in this ROI is " << it->second.size() << std::endl;
      std::vector<MuonLayerHough::Maximum> plotMaximum_roi;
      std::vector<MuonLayerHough::Maximum> plotMaximum_roi_truth;
      for(SectorData::const_iterator sit = it->second.begin(); sit != it->second.end(); ++sit ){//looping the dataindex--first, hitlist--second
        Muon::MuonStationIndex::DetectorRegionIndex region = sit->first.region();// EndcapA, Barrel, EndcapC,
        Muon::MuonStationIndex::LayerIndex          layer  = sit->first.layer();// Inner, Middle, Outer, Extended, BarrelExtended
		
      	if( sit->first.layer() > Muon::MuonStationIndex::LayerIndexMax ) continue;
      	MuonLayerHough& hough      = detectorHough.hough(sit->first.sector(), region, layer);
      	MuonLayerHough& houghTruth = detectorHoughTruth.hough(sit->first.sector(), region, layer);

        int chIndex = hough.m_descriptor.chIndex;//BIS, BIL, BMS, BML, BOS, BOL, BEE(6), EIS, EIL, EMS, EML, EOS, EOL, EES, EEL, CSS, CSL,

        if (m_DEBUG) std::cout << "DEBUG: start new cluster; position: sector " << sit->first.sector() << " region(A/B/C) " 
        << region << " layer: " << layer  << " chindex: " << chIndex << 
        " && number of the hits in this cluster is " << sit->second.size() << std::endl;
        //start to looping over layers for this sector
        for (int iLayer = 0; iLayer < getMaxLayers(region, sit->first.sector()); iLayer++){// Inner, Middle, Outer, Extended, BarrelExtended
          std::vector<Hit*>  hits; //list of hits: hitlist--second
          std::vector<Hit*>  hitsTruth;
          for(HitList::const_iterator hit = sit->second.begin() ;hit != sit->second.end();++hit ){//start looping the hits in the layer
            if ( (*hit)->debugInfo()->layer != iLayer) continue;//keeping only hits in this layer
            hits.push_back(*hit);
            if( abs((*hit)->debugInfo()->pdgId) == 13 ) {//this is a muon hit!
              hitsTruth.push_back(*hit);
            }
          }//end of lopoing all the hits
          hough.setDebug(false);//to supress output level
          hough.fillLayer2(hits);//fill all the layers with the weight
          houghTruth.fillLayer2(hitsTruth);//fill all the layers with the weight
          MuonLayerHough::Maximum maximum;//this is the maximum of this one layer
          bool recoMaximumFound = hough.findMaximum( maximum, m_selectors[hough.m_descriptor.chIndex] );
          MuonLayerHough::Maximum truthMaximum;//this is the truth maximum of this one layer
		      bool truthMaximumFound = houghTruth.findMaximum( truthMaximum, m_selectors[hough.m_descriptor.chIndex]);
		      
          //fill the max information corresponding histograms
		      if(recoMaximumFound){
            m_hMaximaHeightPerChIndex[chIndex]->Reco->Fill( maximum.theta, maximum.max);
            plotRegion.insert(sit->first.key());
            if(plotMaximum_roi.size() > 0 && plotMaximum_roi.back().refchIndex ==  maximum.refchIndex && plotMaximum_roi.back().max <=  maximum.max) {
              plotMaximum_roi.pop_back();//save only the largest maximum
            }
            plotMaximum_roi.push_back(maximum);
          }
          if(truthMaximumFound ){
	   	      maxset.insert(sit->first.sector());
            m_hMaximaHeightPerChIndex[chIndex]->Truth->Fill( truthMaximum.theta, truthMaximum.max);
            if(plotMaximum_roi_truth.size() > 0 && plotMaximum_roi_truth.back().refchIndex ==  truthMaximum.refchIndex && plotMaximum_roi_truth.back().max <=  truthMaximum.max) {
              plotMaximum_roi_truth.pop_back();
            }
            plotMaximum_roi_truth.push_back(truthMaximum);            
          }
          if(recoMaximumFound && truthMaximumFound){
		          if (fabs(truthMaximum.theta - maximum.theta) < 0.1){
              m_hMaximaHeightPerChIndex[chIndex]->Matched->Fill( truthMaximum.theta, truthMaximum.max);
              m_hMaximaHeightPerChIndex[chIndex]->Diff->Fill(truthMaximum.theta, truthMaximum.max - maximum.max);
	          }
            else{
              m_hMaximaHeightPerChIndex[chIndex]->Unmatched->Fill( maximum.theta, maximum.max);
            }
		      }
        }//end of looping layers
      }//end of looping through sectors; end of sit loop
      plotMaximum.push_back(plotMaximum_roi);
      plotMaximum_truth.push_back(plotMaximum_roi_truth);
    }//end of looping through data event

	  //if (maxset.size() != 2) return;//this is when we didn't find 2 maximum for truth; no ROI is found
    int sectorAlreadyDrawn = -1;
    for(auto sit = plotRegion.begin(); sit != plotRegion.end(); ++sit ){
      DataIndex index(*sit);
      if( index.layer() == 2 ) continue;//just to save output size; could be removed
      if (sectorAlreadyDrawn == index.sector()) continue; 
      sectorAlreadyDrawn = index.sector();
      MuonLayerHough& hough = detectorHough.hough(index.sector(),index.region(),index.layer());
      
      
      TString prefix = "Event_";
      prefix += m_ncalls;
      prefix += "_Sector_";
      prefix += index.sector();
      prefix += "_Region_";
      prefix += index.region();
      prefix += "_Layer_";
      prefix += index.layer();

      if (m_DEBUG){
        std::cout << "DEBUG " << prefix.Data() << std::endl;
        hough.rootHistos(prefix.Data());
        drawSector(index.region(),index.sector(),data[index.sector()],detectorHough,detectorHoughTruth);
      }
    }

    //interate over the maximums
    for(int i=0; i < int(plotMaximum.size()); i++){

      TH1F* h_dtheta_temp = new TH1F("thetavalues", "theta values of this current cluster", 628, -M_PI, M_PI);
      for(int j=0; j < int(plotMaximum[i].size()); j++){
        h_dtheta_temp->Fill(plotMaximum[i][j].theta);
      }

      m_h_dtheta->Fill(h_dtheta_temp->GetRMS());//the rms of dthetas
      delete h_dtheta_temp;//remove the pinter

      //linear extrapolation
      if (int(plotMaximum[i].size()) < 3){continue;}//just add protection

      double diff_MI = 0; 
      double diff_MO = 0; 
      double expt_MI = 0; 
      double expt_MO = 0;

      if (!m_DEBUG){
        diff_MI = linear_extrapolate(plotMaximum[i][1], plotMaximum[i][0]);
        diff_MO = linear_extrapolate(plotMaximum[i][1], plotMaximum[i][2]);
        expt_MI = parab_extrapolate(plotMaximum[i][1], plotMaximum[i][0]);
        expt_MO = parab_extrapolate(plotMaximum[i][1], plotMaximum[i][2]);

        if(plotMaximum[i][1].refregion == 1) { 

          m_h_diff_MI_b->Fill(diff_MI);
          m_h_diff_MO_b->Fill(diff_MO);
          m_h_expt_MI_b->Fill(expt_MI);
          m_h_expt_MO_b->Fill(expt_MO);
          m_h_comp_MI_b->Fill(fabs(diff_MI) - fabs(expt_MI));
          m_h_comp_MO_b->Fill(fabs(diff_MO) - fabs(expt_MO));
        }
        else{ 
          m_h_diff_MI_e->Fill(diff_MI);
          m_h_diff_MO_e->Fill(diff_MO);
          m_h_expt_MI_e->Fill(expt_MI);
          m_h_expt_MO_e->Fill(expt_MO);
          m_h_comp_MI_e->Fill(fabs(diff_MI) - fabs(expt_MI));
          m_h_comp_MO_e->Fill(fabs(diff_MO) - fabs(expt_MO));
        }
      }
    }



    //interate over the truth maximums
    for(int i=0; i < int(plotMaximum_truth.size()); i++){
      TH1F* h_dtheta_temp = new TH1F("thetavalues", "theta values of this current cluster", 628, -M_PI, M_PI);
      if (m_DEBUG) std::cout << "DEBUG: maximum found ! " << std::endl;
      for(int j=0; j < int(plotMaximum_truth[i].size()); j++){
        if (m_DEBUG) std::cout << "DEBUG: maximum " << j << " theta " << plotMaximum_truth[i][j].getGlobalTheta() << " max " << plotMaximum_truth[i][j].max 
            << " pos " << plotMaximum_truth[i][j].pos  << " refpos " << plotMaximum_truth[i][j].refpos  << " refchIndex " << plotMaximum_truth[i][j].refchIndex << " binpos " << plotMaximum_truth[i][j].binpos
            << " binposmin " << plotMaximum_truth[i][j].binposmin  << " binposmax " << plotMaximum_truth[i][j].binposmax
            << std::endl;
        h_dtheta_temp->Fill(plotMaximum_truth[i][j].theta);
      }
      delete h_dtheta_temp;//remove the pinter

      if (int(plotMaximum_truth[i].size()) < 3){continue;}//just add protection
      double diff_MI_truth = 0; 
      double diff_MO_truth = 0; 
      double expt_MI_truth = 0; 
      double expt_MO_truth = 0;
      if (true){//fill the truth informations; always for now
        if (plotMaximum_truth[i][1].hough->m_descriptor.chIndex < 2){continue;}//this is extrapolating from BI outwards, won't work
        diff_MI_truth = linear_extrapolate(plotMaximum_truth[i][1], plotMaximum_truth[i][0]);
        diff_MO_truth = linear_extrapolate(plotMaximum_truth[i][1], plotMaximum_truth[i][2]);
        expt_MI_truth = parab_extrapolate(plotMaximum_truth[i][1], plotMaximum_truth[i][0]);
        expt_MO_truth = parab_extrapolate(plotMaximum_truth[i][1], plotMaximum_truth[i][2]);
        if(plotMaximum_truth[i][1].refregion == 1) { //all in barrel
          
          m_h_diff_MI_b_truth->Fill(diff_MI_truth);
          m_h_diff_MO_b_truth->Fill(diff_MO_truth);
          m_h_expt_MI_b_truth->Fill(expt_MI_truth);
          m_h_expt_MO_b_truth->Fill(expt_MO_truth);
          m_h_comp_MI_b_truth->Fill(fabs(diff_MI_truth) - fabs(expt_MI_truth));
          m_h_comp_MO_b_truth->Fill(fabs(diff_MO_truth) - fabs(expt_MO_truth));
        }
        else{ //all not in barrel
          m_h_diff_MI_e_truth->Fill(diff_MI_truth);
          m_h_diff_MO_e_truth->Fill(diff_MO_truth);
          m_h_expt_MI_e_truth->Fill(expt_MI_truth);
          m_h_expt_MO_e_truth->Fill(expt_MO_truth);
          m_h_comp_MI_e_truth->Fill(fabs(diff_MI_truth) - fabs(expt_MI_truth));
          m_h_comp_MO_e_truth->Fill(fabs(diff_MO_truth) - fabs(expt_MO_truth));
        }
      }
      if (m_DEBUG) std::cout << "**** compare two extrapolations! inner region: " << plotMaximum_truth[i][0].refregion 
      << " parabolic: " << expt_MI_truth
      << " linear: " << diff_MI_truth
      << std::endl;

      if (m_DEBUG) std::cout << "**** compare two extrapolations! outer region: " << plotMaximum_truth[i][2].refregion 
      << " parabolic: " << expt_MO_truth
      << " linear: " << diff_MO_truth
      << std::endl;
     
    }
  }


  void LayerAnalysis::drawSector( int region, int sector, SectorData& data, MuonDetectorHough& detectorHough, MuonDetectorHough& detectorHoughTruth ) {
 
    TString canvasName = Form("event_%i_sector_%i,_display_%i", m_ncalls, sector, region);
    TCanvas canvas0(canvasName,canvasName,1500,1100);
    canvas0.cd();
    TString histName = Form("hist_%s", canvasName.Data());
    TH1F* hist = 0;
    if( region == 1 )       hist = new TH1F(histName,histName,100,-13000,13000);
    else if( region == 0 )  hist = new TH1F(histName,histName,100,-25000,-6500);
    else if( region == 2 )  hist = new TH1F(histName,histName,100,6500,25000);
    hist->SetMinimum(0);
    hist->SetMaximum(12000);
    hist->Draw();
    std::vector< double > min(3,1e9);
    std::vector< double > max(3,-1e9);
    std::vector< double > rmin(3,1e9);
    std::vector< double > rmax(3,-1e9);
    std::vector< std::vector<TObject*> > shapes(3);
    unsigned int ntubes(0);
    unsigned int nstrips(0);

    // loop over layers in sector
    for(SectorData::const_iterator it = data.begin();it!=data.end();++it ){
      // only consider the selected region
      if( it->first.region() != region ) continue;
      // loop over hits
      for(HitList::const_iterator hit_it = it->second.begin(); hit_it != it->second.end();++hit_it ){
        Hit& hit = **hit_it;
        HitDebugInfo* debug = hit.debugInfo();
        if( !debug ) continue;	
        int layer = debug->layer;
        if( layer > 2 ) continue;
        // number of layers per multi layer
        if( abs(debug->pdgId) == 13 ){
          if( hit.ymin < min[layer] ) min[layer] = hit.ymin;
          if( hit.ymax > max[layer] ) max[layer] = hit.ymax;
          if( hit.x < rmin[layer] ) rmin[layer] = hit.x;
          if( hit.x > rmax[layer] ) rmax[layer] = hit.x;
        }
        if( debug->type == 0 ){        
          double y = (hit.ymax+hit.ymin)*0.5;
          double x = hit.x;
          double radius = debug->r;
          if(radius<1 ) radius = 1;
          TEllipse* tube = new TEllipse(x,y,radius); 
          shapes[layer].push_back(tube);
          tube->SetLineColor(1);
          tube->SetLineWidth(3);
          tube->SetFillStyle(1001);
          tube->SetFillColor(1);
          if( abs(debug->pdgId) == 13 ) {
            tube->SetLineColor(2);
            tube->SetFillColor(2);
            tube->SetLineWidth(5);
          }
          tube->Draw("SAME");
          ++ntubes;
        }
        else{
          TLine* box = new TLine(hit.x,hit.ymin-5,hit.x,hit.ymax+5);
          int color = 1;
          box->SetLineColor(color);
          box->SetLineWidth(3);
          box->Draw("SAME");
          if( abs(debug->pdgId) == 13 ) {
            box->SetLineColor(2);
            box->SetLineWidth(5);
          }
          shapes[layer].push_back(box);
          ++nstrips;
        }
      }
    } 

    canvas0.Draw();
    canvas0.Update();
    canvas0.Write();

    canvasName += "_hough";
    TCanvas canvas(canvasName,canvasName,1000,1000);
    Muon::MuonStationIndex::DetectorRegionIndex region_index = static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region);//have to convert type...
    const int iLayers = getMaxLayers(region_index, sector);//be careful here, only loop through the correct layers

    canvas.Divide(1,iLayers);
    for( int i=0;i < iLayers;++i){
      canvas.cd(i+1);
      MuonLayerHough& hough = detectorHough.hough(sector,static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region), static_cast<Muon::MuonStationIndex::LayerIndex>(i));
      std::vector<TH1*> hists = hough.rootHistos(canvasName.Data());
      MuonLayerHough& houghTruth = detectorHoughTruth.hough(sector,static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region), static_cast<Muon::MuonStationIndex::LayerIndex>(i));
      std::vector<TH1*> histsTruth = houghTruth.rootHistos((canvasName+"_T").Data());
      int pos = hists.size()*0.5;
      float maximum = std::max(hists[pos]->GetMaximum(), histsTruth[pos]->GetMaximum())*1.2;
      hists[pos]->SetMaximum(maximum);
      hists[pos]->SetLineColor(1);
      hists[pos]->SetFillStyle(3244);
      hists[pos]->SetFillColor(1);
      hists[pos]->Draw();
      histsTruth[pos]->SetLineColor(2);
      histsTruth[pos]->Draw("SAME");
    }
    canvas.Draw();
    canvas.Update();
    canvas.Write();
  }

  void LayerAnalysis::finalize(){
    for (auto Plot : m_hMaximaHeightPerChIndex){
      if (Plot == 0) continue;
        calculateVariables(Plot);//for calculation efficiencies
        delete Plot->Reco; delete Plot->Truth; delete Plot->Matched;
        delete Plot->Efficiency; delete Plot->FakeEfficiency; delete Plot->Diff;
    }
    //for tony plots
    TCanvas c1("c1","c1",800,700);
    c1.SetLogy();
    c1.cd();
    m_h_dtheta->Draw();
    m_h_dtheta->Write();
    m_h_dtheta_truth->SetLineColor(2);
    m_h_dtheta_truth->Draw("SAME");
    m_h_dtheta_truth->Write();

    finishplot(m_h_diff_MI_e);
    finishplot(m_h_diff_MO_e);
    finishplot(m_h_diff_MI_b);
    finishplot(m_h_diff_MO_b);
    finishplot(m_h_diff_MI_e_truth);
    finishplot(m_h_diff_MO_e_truth);
    finishplot(m_h_diff_MI_b_truth);
    finishplot(m_h_diff_MO_b_truth);
    finishplot(m_h_expt_MI_e);
    finishplot(m_h_expt_MO_e);
    finishplot(m_h_expt_MI_b);
    finishplot(m_h_expt_MO_b);
    finishplot(m_h_expt_MI_e_truth);
    finishplot(m_h_expt_MO_e_truth);
    finishplot(m_h_expt_MI_b_truth);
    finishplot(m_h_expt_MO_b_truth);
    finishplot(m_h_comp_MI_e);
    finishplot(m_h_comp_MO_e);
    finishplot(m_h_comp_MI_b);
    finishplot(m_h_comp_MO_b);
    finishplot(m_h_comp_MI_e_truth);
    finishplot(m_h_comp_MO_e_truth);
    finishplot(m_h_comp_MI_b_truth);
    finishplot(m_h_comp_MO_b_truth);
  }

  void LayerAnalysis::calculateVariables(Plots* Plot){
    TH1D* Reco      = Plot->Reco->ProjectionY();
    TH1D* Truth     = Plot->Truth->ProjectionY(); 
    TH1D* Matched   = Plot->Matched->ProjectionY();
    TH1D* Unmatched = Plot->Unmatched->ProjectionY();
    
    int nBins = Truth->GetNbinsX();
    for (int i = 1; i < nBins; ++i){
      if (Truth->Integral(i,nBins) != 0) Plot->Efficiency->SetBinContent(i, Matched->Integral(i, nBins)/Truth->Integral(1, nBins ));
      if (Reco->Integral(i,nBins)  != 0) Plot->FakeEfficiency->SetBinContent(i, Unmatched->Integral(i, nBins)/Reco->Integral(i,nBins));
    }

    //clean up
    delete Reco; delete Truth; delete Matched; delete Unmatched;
  }

  void LayerAnalysis::SetStyle(){
    // use plain black on white colors
    Int_t icol=0; // WHITE
    gStyle->SetFrameBorderMode(icol);
    gStyle->SetFrameFillColor(icol);
    gStyle->SetCanvasBorderMode(icol);
    gStyle->SetCanvasColor(icol);
    gStyle->SetPadBorderMode(icol);
    gStyle->SetPadColor(icol);
    gStyle->SetStatColor(icol);
    // set the paper & margin sizes
    gStyle->SetPaperSize(20,26);
    // set margin sizes
    gStyle->SetPadTopMargin(0.09);
    gStyle->SetPadRightMargin(0.12);
    gStyle->SetPadBottomMargin(0.20);
    gStyle->SetPadLeftMargin(0.14);
    // set title offsets (for axis label)
    gStyle->SetTitleXOffset(1.4);
    gStyle->SetTitleYOffset(1.6);
    // use large fonts
    Int_t fontT=42; // Helvetica italics
    Int_t font=42; // Helvetica
    Double_t tsize=0.025;
    gStyle->SetTextFont(font);
    gStyle->SetTextSize(tsize);
    gStyle->SetLabelFont(font,"x");
    gStyle->SetTitleFont(fontT,"x");
    gStyle->SetLabelFont(font,"y");
    gStyle->SetTitleFont(fontT,"y");
    gStyle->SetLabelFont(font,"z");
    gStyle->SetTitleFont(fontT,"z");
    gStyle->SetLabelSize(tsize,"x");
    gStyle->SetLabelSize(tsize,"y");
    gStyle->SetLabelSize(tsize,"z");
    // use bold lines and markers
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(1.5);
    gStyle->SetHistLineWidth(2.);
    gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
    gStyle->SetPalette(1);//colz color
    // get rid of X error bars 
    // get rid of error bar caps
    gStyle->SetEndErrorSize(0.);
    // do not display any of the standard histogram decorations
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(1111);
    gStyle->SetOptFit(1111);
    // put tick marks on top and RHS of plots
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
  }

  void LayerAnalysis::finishplot(TH1F* h){
    TCanvas c1("c1","c1",700,600);
    c1.cd();

    h->SetBinContent(h->GetNbinsX(), h->GetBinContent(h->GetNbinsX() + 1) + h->GetBinContent(h->GetNbinsX()));
    h->SetBinContent(h->GetNbinsX() + 1, 0);
    h->SetBinContent(1, h->GetBinContent(0) +  h->GetBinContent(1));

    h->Draw();
    h->Write();
    if(m_DEBUG) gPad->SaveAs("../Output/"+ TString(h->GetName()) + ".png");
    delete h;
  }


  float LayerAnalysis::linear_extrapolate(MuonLayerHough::Maximum ref, MuonLayerHough::Maximum ex){
    //z is always the precision plane. r is the reference plane, simple and robust
    double ref_z = ref.getGlobalZ();
    double ref_r = ref.getGlobalR();
    double ex_z  = ex.getGlobalZ();
    double ex_r  = ex.getGlobalR();
    if (ex.hough->m_descriptor.chIndex <= 6){
      return ex_z - ex_r / ref_r * ref_z;
    }
    else{
      return ex_r - ex_z * ref_r / ref_z;
    }
  }

  float LayerAnalysis::parab_extrapolate(MuonLayerHough::Maximum ref, MuonLayerHough::Maximum ex){
    //z is always the precision plane. r is the reference plane, simple and robust
    int    ex_region = ex.refregion;
    int    ref_region = ref.refregion;
    int    ex_chIndex = ex.hough->m_descriptor.chIndex;
    int    ref_chIndex = ref.hough->m_descriptor.chIndex;

    double z_extrapolated = ex.getGlobalZ();
    double r_extrapolated = ex.getGlobalR();
    double theta_extrapolated = ex.getGlobalTheta();
    double z_segment = ref.getGlobalZ();
    double r_segment = ref.getGlobalR();
    double theta_segment = ref.getGlobalTheta();
    double expected = 0;
    //order: reference position, reference maximum, reference theta, extrapolated reference position, extrapolated region, extrapolated layer
    float extrapolated_diff = 9999;
    //this code now only start from middle
    if (z_segment == 0){ return extrapolated_diff;}//this is out of range
    if (theta_segment == 0){ return extrapolated_diff;}//this is out of range
    //for debug popose
    if (m_DEBUG) std::cout << "DEBUG!!! refregion " << ref_region << " chIndex " << ref_chIndex << " ref_r " << r_segment << " ref_z " << z_segment << " ref_theta " 
      << theta_segment  << " angle diff " << theta_segment - atan2(r_segment, z_segment)
      << " theta binstep " << ref.hough->m_descriptor.thetaStep << " theta bin " << ref.bintheta << std::endl;

    if (m_DEBUG) std::cout << "DEBUG!!! exregion " << ex_region << " chIndex " << ex_chIndex << " ex_r " << r_extrapolated << " ex_z " << z_extrapolated << " ex_theta " 
      << theta_extrapolated  << " angle diff " << theta_extrapolated - atan2(r_extrapolated, z_extrapolated)
      << " theta binstep " << ex.hough->m_descriptor.thetaStep << " theta bin " << ex.bintheta << std::endl;
    //for real extrapolation
    if (ref_chIndex <= 6){//this is starting with barrel chamber; BEE is 6
      double r_start =  5000.;//used to be 4500; should start at 5000; should end at 10500
      double z_SL = z_segment + (r_extrapolated-r_segment) * 1.0 / tan(theta_segment);
      double r_SL = r_segment + (z_extrapolated-z_segment) * tan(theta_segment);

      if (ex_chIndex <= 6){//this ends with barrel chamber; BEE is 6
        if(r_segment < 5000.) {//  Forward direction from BI to BM, BO
          expected = z_SL;
        }
        else { // Extrapolation from BM or BO, to other stations
          double rgeo = (r_segment-r_start)*(r_segment-r_start)/r_segment - 2*(r_segment-r_start);
          double rhoInv = (z_segment/r_segment - 1.0 / tan(theta_segment))/rgeo;
          double cotant0 = 1.0 / tan(theta_segment) - 2*(r_segment-r_start)*rhoInv;//- sign come from taking inverse derivative
          expected = r_extrapolated * cotant0 + (r_extrapolated-r_start)*(r_extrapolated-r_start)*rhoInv;
        }
        extrapolated_diff =  z_extrapolated - expected;
      }//end with barrel to barrel extrapolation
      else{//this is a barrel to endcap extrapolation, mostly in the transition region? no B field, use SL
        expected = r_SL;
        extrapolated_diff =  r_extrapolated - expected;
      }
    }
    else{//this starts with endcap chamber;
        double z_start =  8500.;//used to be 6500; should start at 8500
        double z_end   = 12500.;//used to be 12000; should end at 12500

        if(tan(theta_segment)<0) z_start = - z_start;
        if(tan(theta_segment)<0)   z_end = - z_end;

        double r_SL = r_segment + (z_extrapolated-z_segment)*tan(theta_segment);
        double z_SL = z_segment + (r_extrapolated-r_segment) * 1.0 / tan(theta_segment);

        if (ex_chIndex >= 7){//extrapolate to endcap
          if (std::abs(z_segment) < std::abs(z_end)){//extrapolate from EI or EE, have to use linear
            expected = r_SL;
            extrapolated_diff =  r_extrapolated - expected;
          }
          else{// from EM or EO
            if (std::abs(z_extrapolated) > std::abs(z_end)){//to EM or EO
              //extrapolate to outer layer, just using theta of the middle measurement; only works if the theta measurement is correct
              //can extrapolate with either the outside theta or middle theta; outside theta is better; father away from the B field
              expected = r_SL;
              extrapolated_diff =  r_extrapolated - expected;
            }
            else if (std::abs(z_segment) > std::abs(z_extrapolated)){//to EI or EE
              double r_end = r_segment + (z_end-z_segment)*tan(theta_segment);
              double zgeo = (z_end-z_start)*(z_end-z_start) -2*z_end*(z_end-z_start);
              double rhoInv = (r_end - z_end*tan(theta_segment)) / zgeo;
              double tantheta = tan(theta_segment) - 2*(z_end - z_start)*rhoInv;
              expected = z_extrapolated*tantheta + (z_extrapolated-z_start)*(z_extrapolated-z_start)*rhoInv;//???

              extrapolated_diff = r_extrapolated - expected;
            }
          }
        }
        else{//exrapolate to barrel; again verly likely to be in transition region, just use linear 
          expected = z_SL;
          extrapolated_diff = z_extrapolated - expected;
        }
    }
    float new_parabolic = MuonHough::extrapolate(ref, ex, true);

    return new_parabolic;
  }

  int LayerAnalysis::getMaxLayers(Muon::MuonStationIndex::DetectorRegionIndex region, int sector) {
    bool isSmall = (sector%2==0);
    int maxLayers = 0;
    for( int lay = 0; lay < Muon::MuonStationIndex::LayerIndexMax; ++lay ){
      Muon::MuonStationIndex::LayerIndex layer = static_cast<Muon::MuonStationIndex::LayerIndex>(lay);
      Muon::MuonStationIndex::ChIndex chIndex = Muon::MuonStationIndex::toChamberIndex(region, layer, isSmall);
      if( chIndex == Muon::MuonStationIndex::ChUnknown || chIndex >= Muon::MuonStationIndex::ChIndexMax ) {continue;}
      maxLayers++;
    }
    return maxLayers;
  }

}
