/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/LayerAnalysis.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include "TTree.h"
#include "TString.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include <set>
#include <cmath>

#include <iostream>
#include <cstdlib>

namespace MuonHough {
  void LayerAnalysis::analyse() {



    Long64_t nentries = m_tree->GetEntries();
    //nentries = 10;
    m_ncalls = 0;
    std::cout << " analysing events " << nentries << std::endl;
    for( Long64_t evt=0;evt<nentries;++evt ){
      m_tree->LoadTree(evt);
      m_tree->GetEntry(evt);
      m_event.reset();
      if(!m_ntuple.read(m_event,m_muons)) break;
      
      analysis( m_event.sectors );
      ++m_ncalls;
    }
  }

  void LayerAnalysis::analysis( std::map<int,SectorData>& data ) {

    bool fillPerLayer = true;
    MuonDetectorHough detectorHough;
    MuonDetectorHough detectorHoughTruth;

    std::map<int,int> hitsPerSector;
    std::map<int,int> muonHitsPerSector;
    std::set<int> plotRegion;
    std::map<int,SectorData>::const_iterator it = data.begin();
    std::map<int,SectorData>::const_iterator it_end = data.end();
    for( ;it!=it_end;++it ){

      SectorData::const_iterator sit = it->second.begin();
      SectorData::const_iterator sit_end = it->second.end();
      for( ;sit!=sit_end;++sit ){
	if( sit->first.layer() > 2 ) continue;

	MuonLayerHough& hough = detectorHough.hough(sit->first.sector(),sit->first.region(),sit->first.layer());
	hough.setDebug(true);
	MuonLayerHough& houghTruth = detectorHoughTruth.hough(sit->first.sector(),sit->first.region(),sit->first.layer());
	if( fillPerLayer ){
	  std::vector<Hit*>  hitsInSubLayer;
	  std::vector<Hit*>  hitsInSubLayerTruth;
	  int currentSubLayer = -1;
	  std::cout << " sector " << sit->first.sector() << " layer " << sit->first.layer() << " type " << sit->first.type()  
		    << " hits  " << sit->second.size() << std::endl;
	  HitList::const_iterator hit = sit->second.begin();
	  HitList::const_iterator hit_end = sit->second.end();
	  for( ;hit!=hit_end;++hit ){
	    if( !(*hit)->debugInfo() ) continue;
	    int hitLayer = 1000*(*hit)->debugInfo()->layer + 100*(*hit)->layer;
	    if( currentSubLayer != hitLayer && currentSubLayer != -1 ){
	      //std::cout << " filling layer " << currentSubLayer << " hits " << hitsInSubLayer.size() << " truth " << hitsInSubLayerTruth.size() << std::endl;
	      hough.fillLayer(hitsInSubLayer);
	      houghTruth.fillLayer(hitsInSubLayerTruth);
	      hitsInSubLayer.clear();
	    }
// 	    std::cout << " adding hit: layer " << (*hit)->debugInfo()->layer << " sublay " << (*hit)->layer
// 		      << " x " << (*hit)->x << " ymin " << (*hit)->ymin << " ymax " << (*hit)->ymax
// 		      << " pdg " << (*hit)->debugInfo()->pdgId << std::endl;
	    currentSubLayer = hitLayer;
	    hitsInSubLayer.push_back(*hit);
	    ++hitsPerSector[sit->first.key()];
	    if( abs((*hit)->debugInfo()->pdgId) == 13 ) {
	      ++muonHitsPerSector[10*sit->first.sector()+sit->first.region()];
	      hitsInSubLayerTruth.push_back(*hit);
	    }
	  }
	  //std::cout << " filling layer " << currentSubLayer << " hits " << hitsInSubLayer.size() << " truth " << hitsInSubLayerTruth.size() << std::endl;
	  hough.fillLayer(hitsInSubLayer);
	  houghTruth.fillLayer(hitsInSubLayerTruth);
	}else{
	  HitList::const_iterator hit = sit->second.begin();
	  HitList::const_iterator hit_end = sit->second.end();
	  for( ;hit!=hit_end;++hit ){
	    hough.fill(**hit);
	    ++hitsPerSector[sit->first.key()];
	  }
	}
      }
      for( int ir=0;ir<3;ir+=2 ){
	MuonLayerHough& hough = detectorHough.hough(it->first,static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(ir),Muon::MuonStationIndex::Middle);
	MuonLayerHough::Maximum maximum;
	if( hough.findMaximum( maximum, 14 ) ){
	  std::cout << " found maximum:region  " << ir << " sector " << it->first << " layer 1 " << maximum.max << std::endl;
	  int secreg = 10*it->first+ir;
	  plotRegion.insert(secreg);
	}
      }
    }
    return;
    std::set<int> wasDrawn;
    std::map<int,int>::iterator sit = hitsPerSector.begin();
    std::map<int,int>::iterator sit_end = hitsPerSector.end();
    for( ;sit!=sit_end;++sit ){
      
      DataIndex index(sit->first);
      int secreg = 10*index.sector()+index.region();

      if( index.layer() == 2 ) continue;
      MuonLayerHough& hough = detectorHough.hough(index.sector(),index.region(),index.layer());
    
      
      //if(  < 16 ) continue;
      if( plotRegion.count(secreg) || muonHitsPerSector[secreg] < 16) continue;
      TString prefix = "Event_";
      prefix += m_ncalls;
      prefix += "_Sector_";
      prefix += index.sector();
      prefix += "_Region_";
      prefix += index.region();
      prefix += "_Layer_";
      prefix += index.layer();
      std::cout << " event " << m_ncalls << " ploting key " << sit->first <<  " hits " << sit->second;
      hough.rootHistos(prefix.Data());

      if( !wasDrawn.count(secreg) ) {
	wasDrawn.insert(secreg);
	drawSector(index.region(),index.sector(),data[index.sector()],detectorHough,detectorHoughTruth);
      }
    }

  }


  void LayerAnalysis::drawSector( int region, int sector, SectorData& data, MuonDetectorHough& detectorHough, MuonDetectorHough& detectorHoughTruth ) {

    //double refpos[6] = { 4560, 8096, 10630, 4950, 7153, 9500, };
 
    TString cname = "event_";
    cname += m_ncalls;
    cname += "_sector_";
    cname += sector;
    cname += "_region_";
    cname += region;
    cname += "_display_";

    TCanvas* c = new TCanvas(cname,cname,1500,1100);
    c->cd();
    TString hname = "hist_";
    hname += cname;
    TH1F* h = 0;
    if( region == 1 )       h = new TH1F(hname,hname,100,-13000,13000);
    else if( region == 0 )  h = new TH1F(hname,hname,100,-25000,-6500);
    else if( region == 2 )  h = new TH1F(hname,hname,100,6500,25000);
    h->SetMinimum(0);
    h->SetMaximum(12000);
    h->Draw();
    std::vector< double > min(3,1e9);
    std::vector< double > max(3,-1e9);
    std::vector< double > rmin(3,1e9);
    std::vector< double > rmax(3,-1e9);
    std::vector< std::vector<TObject*> > shapes(3);
    std::cout << " drawing sector " << cname << " detector region " << region << std::endl;
    unsigned int ntubes(0);
    unsigned int nstrips(0);

    // loop over layers in sector
    SectorData::const_iterator it = data.begin();
    SectorData::const_iterator it_end = data.end();
    for( ;it!=it_end;++it ){

      // only consider the selected region
      if( it->first.region() != region ) continue;

      // loop over hits
      HitList::const_iterator hit = it->second.begin();
      HitList::const_iterator hit_end = it->second.end();
      for( ;hit!=hit_end;++hit ){
	
	Hit& h = **hit;
	HitDebugInfo* debug = h.debugInfo();
	if( !debug ) continue;
	
	int layer = debug->layer;
	if( layer > 2 ) continue;
	// number of layers per multi layer
	if( abs(debug->pdgId) == 13 ){
	  if( h.ymin < min[layer] ) min[layer] = h.ymin;
	  if( h.ymax > max[layer] ) max[layer] = h.ymax;
	  if( h.x < rmin[layer] ) rmin[layer] = h.x;
	  if( h.x > rmax[layer] ) rmax[layer] = h.x;
	}
	if( debug->type == 0 ){

	  double y = (h.ymax+h.ymin)/2.;
	  double x = h.x;
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
	}else{
	  TLine* box = new TLine(h.x,h.ymin-5,h.x,h.ymax+5);
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

    std::cout << " updating display: MDTs " << ntubes << " rpcs " << nstrips
	      << " lay0 " << shapes[0].size() << " lay1 " << shapes[1].size() << " lay2 " << shapes[2].size()
	      << std::endl;
    c->Draw();
    c->Update();
    c->Draw();
    c->Update();
    c->Write();

    cname += "_zoom";
    c = new TCanvas(cname,cname,1500,1100);
    c->Divide(1,3);
    for( int i=0;i<3;++i){
      if( min[i] < 1e9 ){
	double scale = ((rmax[i]-rmin[i])/12)  / ((max[i]-min[i])/3.);
	if( scale < 1 ) {
	  max[i] += 0.5*(max[i]-min[i])*scale;
	  min[i] -= 0.5*(max[i]-min[i])*scale;
	}else{
	  rmax[i] += 0.5*(rmax[i]-rmin[i])/scale;
	  rmin[i] -= 0.5*(rmax[i]-rmin[i])/scale;
	}
	std::cout << " drawing " << i << " scale " << scale 
		  << " range z " << min[i] << " " << max[i] << "  r  " << rmin[i] << " " << rmax[i] << std::endl;
	hname = "hist_";
	hname += cname;
	hname += i;
	c->cd(3-i);
	double rangez = 120;
	double ranger = 30;
	if( region != 1 && i == 1 ){
	  rangez = 100;
	  ranger = 10;
	}

	h = new TH1F(hname,hname,100,rmin[i]-ranger,rmax[i]+ranger);
	h->SetMinimum(min[i]-rangez);
	h->SetMaximum(max[i]+rangez);
	h->Draw();
	for( unsigned int s=0;s<shapes[i].size();++s){
	  shapes[i][s]->Draw("SAME");
	}
      }
    }
    c->Draw();
    c->Update();
    c->Draw();
    c->Update();
    c->Write();

    cname += "_hough";
    c = new TCanvas(cname,cname,1000,700);
    c->Divide(1,3);
    for( int i=0;i<3;++i){
      c->cd(i+1);
      MuonLayerHough& hough = detectorHough.hough(sector,static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region),
                                                  static_cast<Muon::MuonStationIndex::LayerIndex>(i));
      std::vector<TH1*> hists = hough.rootHistos(cname.Data());
      MuonLayerHough& houghTruth = detectorHoughTruth.hough(sector,static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region),
                                                            static_cast<Muon::MuonStationIndex::LayerIndex>(i));
      std::vector<TH1*> histsTruth = houghTruth.rootHistos((cname+"_T").Data());
//       std::vector<TH1*>::iterator it=hists.begin();
//       std::vector<TH1*>::iterator it_end=hists.end();
//       int max = 0;
//       for( ;it!=it_end;++it ){
// 	if( (**it).GetMaximum() > max ) max = (**it).GetMaximum();
//       }
//       it=hists.begin();
//       int col = 1;
//       for( ;it!=it_end;++it ){
// 	(*it)->SetLineColor(col++);
// 	if( it == hists.begin() ){
// 	  (*it)->SetMaximum(1.2*max);
// 	  (*it)->Draw();
// 	}else{
// 	  (*it)->Draw("SAME");
// 	}
//       }
      int pos = hists.size()/2;
      hists[pos]->Draw();
      histsTruth[pos]->SetLineColor(2);
      histsTruth[pos]->Draw("SAME");
    }
    c->Draw();
    c->Update();
    c->Draw();
    c->Update();
    c->Write();
    //c->Print(cname+".pdf","pdf");
    std::cout << " type input to continue " << std::endl;
    std::string dummy = "";
    std::cin >> dummy;
    std::cout << " continuing " << std::endl;

  }


}
