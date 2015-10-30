/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_fakes.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_fakes.h"
#include <math.h> //for Pi




InDetPerfPlot_fakes::InDetPerfPlot_fakes(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_fakes::initializePlots() {
  const bool prependDirectory(false);
  
  //
  SingleHistogramDefinition hd= retrieveDefinition("fakepT", "default");
  m_fakepT  = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd= retrieveDefinition("fakepTlow", "default");
  m_fakePtLow = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("fakephi", "default");
  m_fakephi = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("fakeeta", "default");
  m_fakeeta = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("faked0", "default");
  m_faked0 = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("fakez0", "default");
  m_fakez0 = Book1D(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second, prependDirectory);
  //
  hd = retrieveDefinition("track_fakerate_vs_eta", "default");
  m_track_fakerate_vs_eta= BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second,hd.yAxis.first,hd.yAxis.second, prependDirectory); 
  hd = retrieveDefinition("track_fakerate_vs_pt", "default");
  m_track_fakerate_vs_pt= BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second,hd.yAxis.first,hd.yAxis.second, prependDirectory); 
  hd = retrieveDefinition("track_fakerate_vs_phi", "default");
  m_track_fakerate_vs_phi= BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second,hd.yAxis.first,hd.yAxis.second, prependDirectory); 
  hd = retrieveDefinition("track_fakerate_vs_d0", "default");
  m_track_fakerate_vs_d0= BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second,hd.yAxis.first,hd.yAxis.second, prependDirectory); 
  hd = retrieveDefinition("track_fakerate_vs_z0", "default");
  m_track_fakerate_vs_z0= BookTProfile(hd.name,hd.allTitles,hd.nBinsX,hd.xAxis.first,hd.xAxis.second,hd.yAxis.first,hd.yAxis.second, prependDirectory); 
 
}

void 
InDetPerfPlot_fakes::fill(const xAOD::TrackParticle& trkprt,const bool isFake, const Category /*f*/){
		const unsigned int fakeNum=(unsigned int)isFake;
		constexpr double degreesPerRadian(180./M_PI);
		double pt = trkprt.pt()/1000.;
		double eta(trkprt.eta());
		double phi(trkprt.phi());
		double phi_degrees(phi * degreesPerRadian);
		double d0(trkprt.d0());
		double z0(trkprt.z0());
		m_fakepT->Fill(pt);
		if (isFake){
			m_fakePtLow->Fill(pt);
			m_fakeeta->Fill(eta);
			m_fakephi->Fill(phi);
			m_faked0->Fill(d0);
			m_fakez0->Fill(z0);
		}
		m_track_fakerate_vs_eta->Fill(pt,fakeNum);
		m_track_fakerate_vs_pt->Fill(eta,fakeNum);
		m_track_fakerate_vs_phi->Fill(phi_degrees,fakeNum);
		m_track_fakerate_vs_d0->Fill(d0,fakeNum);
		m_track_fakerate_vs_z0->Fill(z0,fakeNum);
}
