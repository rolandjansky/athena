/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_fakes.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_fakes.h"
#include <math.h> //for Pi

InDetPerfPlot_fakes::InDetPerfPlot_fakes(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_fakes::initializePlots() {
    m_fakepT  = Book1D("fakepT","p_{T} of selected fake tracks (in GeV);p_{T}(GeV/c)",200,0.,200, false);
    m_fakePtLow  = Book1D("fakepTlow","p_{T} of selected fake tracks (in GeV);p_{T}(GeV/c)",200,0,20, false);
    m_fakephi = Book1D("fakephi","#phi of selected fake tracks;#phi",100,0,2*M_PI, false);
    m_fakeeta = Book1D("fakeeta","#eta of selected fake tracks;#eta",100,-5,5, false);
    m_faked0 = Book1D("faked0","d_{0} of selected fake tracks (in mm);d_{0}(mm)",200,-5,5, false);
    m_fakez0 = Book1D("fakez0","z_{0} of selected fake tracks (in mm);z_{0}(mm)",120,-300,300, false);
    //
    m_track_fakerate_vs_eta= BookTProfile( "track_fakerate_vs_eta","Fraction of tracks with <50% truth match probability ;#eta;Fake Rate",20,-2.5,2.5,0,1, false);
    m_track_fakerate_vs_pt= BookTProfile( "track_fakerate_vs_pt","Fraction of tracks with <50% truth match probability ;p_{T}(GeV/c);Fake Rate",25,0,50,0,1,false);
    m_track_fakerate_vs_phi= BookTProfile( "track_fakerate_vs_phi","Fraction of tracks with <50% truth match probability ;#phi;Fake Rate",24,-180,180,0,100,false);
    m_track_fakerate_vs_d0= BookTProfile( "track_fakerate_vs_d0","Fraction of tracks with <50% truth match probability ;Fake Rate",20,-6,6,0,100,false);
    m_track_fakerate_vs_z0= BookTProfile( "track_fakerate_vs_z0","Fraction of tracks with <50% truth match probability ;Fake Rate",20,-300,300,0,100,false);

}

void 
InDetPerfPlot_fakes::fill(const xAOD::TrackParticle& trkprt,const bool isFake, const Category /*f*/){
		const unsigned int fakeNum=(unsigned int)isFake;
		static const double degreesPerRadian(180./M_PI);
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
