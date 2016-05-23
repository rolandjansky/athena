/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_fakes.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_fakes.h"
#include <math.h> //for M_PI




InDetPerfPlot_fakes::InDetPerfPlot_fakes(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_fakes::initializePlots() {
  book(m_fakepT, "fakepT");
  book(m_fakePtLow, "fakepTlow");
  book(m_fakephi, "fakephi");
  book(m_fakeeta, "fakeeta");
  book(m_faked0,"faked0");
  book(m_fakez0, "fakez0");
  //
  book(m_track_fakerate_vs_eta, "track_fakerate_vs_eta");
  book(m_track_fakerate_vs_pt, "track_fakerate_vs_pt");
  book(m_track_fakerate_vs_phi, "track_fakerate_vs_phi");
  book(m_track_fakerate_vs_d0, "track_fakerate_vs_d0");
  book(m_track_fakerate_vs_z0, "track_fakerate_vs_z0"); 
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
		m_track_fakerate_vs_eta->Fill(eta,fakeNum);
		m_track_fakerate_vs_pt->Fill(pt,fakeNum);
		m_track_fakerate_vs_phi->Fill(phi_degrees,fakeNum);
		m_track_fakerate_vs_d0->Fill(d0,fakeNum);
		m_track_fakerate_vs_z0->Fill(z0,fakeNum);
}
