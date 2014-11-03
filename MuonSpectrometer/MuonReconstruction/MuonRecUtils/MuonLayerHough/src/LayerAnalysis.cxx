/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/LayerAnalysis.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "MuonIdHelpers/MuonStationIndex.h"

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

    Long64_t nentries = m_tree->GetEntries();
//    nentries = 10;
    m_ncalls = 0;
    std::cout << " analysing events " << nentries << std::endl;
    m_ntuple.initForRead(*m_tree);

    m_selectors.resize(Muon::MuonStationIndex::ChIndexMax);
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

    initialize();
    for( Long64_t evt=0;evt<nentries;++evt ){
      if (evt % 100 == 0) std::cout << "event: " << evt << std::endl;
      m_tree->LoadTree(evt);
      m_tree->GetEntry(evt);
      m_event.reset();
	  m_muons.clear();
      if(!m_ntuple.read(m_event,m_muons)) continue;
      analysis( m_event.sectors );
      ++m_ncalls;
    }
    finalize();
  }

  void LayerAnalysis::initialize(){
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
        
  }

  void LayerAnalysis::analysis( std::map<int,SectorData>& data ) {
    MuonDetectorHough detectorHough;
    MuonDetectorHough detectorHoughTruth; 
    std::set<DataIndex> plotRegion;
    std::set<int> maxset;
    for(std::map<int,SectorData>::const_iterator it = data.begin() ;it != data.end();++it ){
      for(SectorData::const_iterator sit = it->second.begin(); sit != it->second.end(); ++sit ){
        Muon::MuonStationIndex::DetectorRegionIndex region = sit->first.region();
        Muon::MuonStationIndex::LayerIndex          layer  = sit->first.layer();
		
      	if( sit->first.layer() > Muon::MuonStationIndex::LayerIndexMax ) continue;
      	MuonLayerHough& hough      = detectorHough.hough(sit->first.sector(), region, layer);
      	MuonLayerHough& houghTruth = detectorHoughTruth.hough(sit->first.sector(), region, layer);

        int chIndex = hough.m_descriptor.chIndex;
        for (int iLayer = 0; iLayer < Muon::MuonStationIndex::LayerIndexMax; iLayer++){
          std::vector<Hit*>  hits; 
          std::vector<Hit*>  hitsTruth;
          for(HitList::const_iterator hit = sit->second.begin() ;hit != sit->second.end();++hit ){
             if ( (*hit)->debugInfo()->layer != iLayer) continue;
             hits.push_back(*hit);
             if( abs((*hit)->debugInfo()->pdgId) == 13 ) {
               hitsTruth.push_back(*hit);
             }
           }
        
          hough.setDebug(false);
          hough.fillLayer2(hits);
          houghTruth.fillLayer2(hitsTruth);
	      MuonLayerHough::Maximum maximum;
          bool recoMaximumFound = hough.findMaximum( maximum, m_selectors[hough.m_descriptor.chIndex] );
          MuonLayerHough::Maximum truthMaximum;
		  bool truthMaximumFound = houghTruth.findMaximum( truthMaximum, m_selectors[hough.m_descriptor.chIndex]);
		  
		  if(recoMaximumFound){
            m_hMaximaHeightPerChIndex[chIndex]->Reco->Fill( maximum.theta, maximum.max);
            plotRegion.insert(sit->first.key());
          }
          if(truthMaximumFound ){
	   	    maxset.insert(sit->first.sector());
            m_hMaximaHeightPerChIndex[chIndex]->Truth->Fill( truthMaximum.theta, truthMaximum.max);             
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
        }
      }
    }
	if (maxset.size() != 2) return;
    int sectorAlreadyDrawn = -1;
    for(auto sit = plotRegion.begin(); sit != plotRegion.end(); ++sit ){
      DataIndex index(*sit);
      if( index.layer() == 2 ) continue;
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

      hough.rootHistos(prefix.Data());
      drawSector(index.region(),index.sector(),data[index.sector()],detectorHough,detectorHoughTruth);
    }
  }


  void LayerAnalysis::drawSector( int region, int sector, SectorData& data, MuonDetectorHough& detectorHough, MuonDetectorHough& detectorHoughTruth ) {
 
    TString canvasName = Form("event_%i_sector_%i,_display_%i", m_ncalls, sector, region);
    // TCanvas canvas(canvasName,canvasName,1500,1100);
    // canvas.cd();
    // TString histName = Form("hist_%s", canvasName.Data());
    //  TH1F* hist = 0;
    //  if( region == 1 )       hist = new TH1F(histName,histName,100,-13000,13000);
    //  else if( region == 0 )  hist = new TH1F(histName,histName,100,-25000,-6500);
    //  else if( region == 2 )  hist = new TH1F(histName,histName,100,6500,25000);
//    hist->SetMinimum(0);
//    hist->SetMaximum(12000);
//    hist->Draw();
//    std::vector< double > min(3,1e9);
//    std::vector< double > max(3,-1e9);
//    std::vector< double > rmin(3,1e9);
//    std::vector< double > rmax(3,-1e9);
//    std::vector< std::vector<TObject*> > shapes(3);
//    //std::cout << " drawing sector " << canvasName << " detector region " << region << std::endl;
//    unsigned int ntubes(0);
//    unsigned int nstrips(0);
//
//    // loop over layers in sector
//    for(SectorData::const_iterator it = data.begin();it!=data.end();++it ){
//
//      // only consider the selected region
//      if( it->first.region() != region ) continue;
//
//      // loop over hits
//      for(HitList::const_iterator hit_it = it->second.begin(); hit_it != it->second.end();++hit_it ){
//	      Hit& hit = **hit_it;
//	      HitDebugInfo* debug = hit.debugInfo();
//	      if( !debug ) continue;	
//	      int layer = debug->layer;
//	      if( layer > 2 ) continue;
//	      // number of layers per multi layer
//	      if( abs(debug->pdgId) == 13 ){
//	        if( hit.ymin < min[layer] ) min[layer] = hit.ymin;
//	        if( hit.ymax > max[layer] ) max[layer] = hit.ymax;
//	        if( hit.x < rmin[layer] ) rmin[layer] = hit.x;
//	        if( hit.x > rmax[layer] ) rmax[layer] = hit.x;
//	      }
//	      if( debug->type == 0 ){        
//	        double y = (hit.ymax+hit.ymin)*0.5;
//	        double x = hit.x;
//	        double radius = debug->r;
//	        if(radius<1 ) radius = 1;
//	        TEllipse* tube = new TEllipse(x,y,radius); 
//	        shapes[layer].push_back(tube);
//	        tube->SetLineColor(1);
//	        tube->SetLineWidth(3);
//	        tube->SetFillStyle(1001);
//	        tube->SetFillColor(1);
//	        if( abs(debug->pdgId) == 13 ) {
//	          tube->SetLineColor(2);
//	          tube->SetFillColor(2);
//	          tube->SetLineWidth(5);
//	        }
//	        tube->Draw("SAME");
//	        ++ntubes;
//	      }
//        else{
//	        TLine* box = new TLine(hit.x,hit.ymin-5,hit.x,hit.ymax+5);
//	        int color = 1;
//	        box->SetLineColor(color);
//	        box->SetLineWidth(3);
//	        box->Draw("SAME");
//	        if( abs(debug->pdgId) == 13 ) {
//	          box->SetLineColor(2);
//	          box->SetLineWidth(5);
//	        }
//	        shapes[layer].push_back(box);
//	        ++nstrips;
//	      }
//      }
//    } 
//
//    //std::cout << " updating display: MDTs " << ntubes << " rpcs " << nstrips
//	  //    << " lay0 " << shapes[0].size() << " lay1 " << shapes[1].size() << " lay2 " << shapes[2].size()
//	  //    << std::endl;
//    canvas->Draw();
//    canvas->Update();
//    canvas->Draw();
//    canvas->Update();
////    canvas->SaveAs(canvasName + ".pdf");
//    canvas->Write();
//
//
//
////--------------------------------------------------------------------------------------------------------------
//
//
//    canvasName += "_zoom";
//    canvas = new TCanvas(canvasName,canvasName,1500,1100);
//    canvas->Divide(1,3);
//    for( int i=0;i<3;++i){
//      if( min[i] < 1e9 ){
//	double scale = ((rmax[i]-rmin[i])/12)  / ((max[i]-min[i])/3.);
//	if( scale < 1 ) {
//	  max[i] += 0.5*(max[i]-min[i])*scale;
//	  min[i] -= 0.5*(max[i]-min[i])*scale;
//	}else{
//	  rmax[i] += 0.5*(rmax[i]-rmin[i])/scale;
//	  rmin[i] -= 0.5*(rmax[i]-rmin[i])/scale;
//	}
//	//std::cout << " drawing " << i << " scale " << scale 
//  // << " range z " << min[i] << " " << max[i] << "  r  " << rmin[i] << " " << rmax[i] << std::endl;
//	histName = "hist_";
//	histName += canvasName;
//	histName += i;
//	canvas->cd(3-i);
//	double rangez = 120;
//	double ranger = 30;
//	if( region != 1 && i == 1 ){
//	  rangez = 100;
//	  ranger = 10;
//	}
//
//	hist = new TH1F(histName,histName,100,rmin[i]-ranger,rmax[i]+ranger);
//	hist->SetMinimum(min[i]-rangez);
//	hist->SetMaximum(max[i]+rangez);
//	hist->Draw();
//	for( unsigned int s=0;s<shapes[i].size();++s){
//	  shapes[i][s]->Draw("SAME");
//	}
//      }
//    }
//    canvas->Draw();
//    canvas->Update();
//    canvas->Draw();
//    canvas->Update();
////    canvas->SaveAs(canvasName + ".pdf");
//    canvas->Write();
//


//--------------------------------------------------------------------------------------------------------------



    canvasName += "_hough";
    TCanvas canvas(canvasName,canvasName,1000,700);
    const int iLayers = Muon::MuonStationIndex::LayerIndexMax;
    canvas.Divide(1,iLayers);
    for( int i=0;i<iLayers;++i){
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
      int chIndex = hough.m_descriptor.chIndex;
      histsTruth[pos]->SetLineColor(2);
      histsTruth[pos]->Draw("SAME");
      // float maxPos = fabs(hists[pos]->GetXaxis()->GetBinCenter(hists[pos]->GetMaximumBin()));
      // float max  = hists[pos]->GetBinContent(hists[pos]->GetMaximumBin());
      //
      // if (max > 1) m_hMaximaHeightPerChIndex[chIndex]->Fill( maxPos, max);
      // float truthMaxPos = fabs(histsTruth[pos]->GetXaxis()->GetBinCenter(histsTruth[pos]->GetMaximumBin()));
      // float truthMax  = histsTruth[pos]->GetBinContent(histsTruth[pos]->GetMaximumBin());
      //
      // if (truthMax > 1) m_hMaximaHeightPerChIndexTruth[chIndex]->Fill( truthMaxPos, truthMax);
    }
    canvas.Draw();
    canvas.Update();
//    canvas.SaveAs(canvasName + ".pdf");
    canvas.Write();
  }

  void LayerAnalysis::finalize(){
    for (auto Plot : m_hMaximaHeightPerChIndex){
      if (Plot == 0) continue;
        calculateVariables(Plot);
        Plot->Reco->Draw("COLZTEXT");
        gPad->SaveAs(TString(Plot->Reco->GetName()) + ".pdf");
        Plot->Truth->Draw("COLZTEXT");
        gPad->SaveAs(TString(Plot->Truth->GetName()) + ".pdf");
        // Plot->Matched->Draw("COLZ");
        // gPad->SaveAs(TString(Plot->Matched->GetName()) + ".pdf");
        // Plot->Efficiency->Draw();
        // gPad->SaveAs(TString(Plot->Efficiency->GetName()) + ".pdf");
        // Plot->FakeEfficiency->Draw();
        // gPad->SaveAs(TString(Plot->FakeEfficiency->GetName()) + ".pdf");
        Plot->Diff->Draw("COLZTEXT");
        gPad->SaveAs(TString(Plot->Diff->GetName()) + ".pdf");
			
    }
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
  }

}


