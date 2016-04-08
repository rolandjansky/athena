/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationPlots.h"

namespace Egamma{


  IsolationPlots::IsolationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												 m_sParticleType(sParticleType),
												 etcone20 (0),
												 etcone30 (0),
												 etcone40 (0),
												 etcone20corr(0),   
												 etcone30corr(0),   
												 etcone40corr(0),   
												 ptcone20(0),       
												 ptcone30(0),       
												 ptcone40(0),       
												 nucone20(0),       
												 nucone30(0),       
												 nucone40(0),       
												 topoetcone20(0),   
												 topoetcone30(0),   
												 topoetcone40(0)   
    {}

  void IsolationPlots::initializePlots(){
                     
    etcone20       = Book1D("etcone20", "E_{T}^{cone20} of "+ m_sParticleType+";E_{T}^{cone20};Entries", 60, -5., 25.);
    etcone30       = Book1D("etcone30", "E_{T}^{cone30} of "+ m_sParticleType+";E_{T}^{cone30};Entries", 60, -5., 25.);
    etcone40       = Book1D("etcone40", "E_{T}^{cone40} of "+ m_sParticleType+";E_{T}^{cone40};Entries", 60, -5., 25.);
    etcone20corr   = Book1D("etcone20corr", "E_{T}^{cone20corr} of "+ m_sParticleType+";E_{T}^{cone20corr};Entries", 60, -5., 25.);
    etcone30corr   = Book1D("etcone30corr", "E_{T}^{cone30corr} of "+ m_sParticleType+";E_{T}^{cone30corr};Entries", 60, -5., 25.);
    etcone40corr   = Book1D("etcone40corr", "E_{T}^{cone40corr} of "+ m_sParticleType+";E_{T}^{cone40corr};Entries", 60, -5., 25.);
    ptcone20       = Book1D("ptcone20", "p_{T}^{cone20} of "+ m_sParticleType+";p_{T}^{cone20};Entries", 60, 0., 15.);
    ptcone30       = Book1D("ptcone30", "p_{T}^{cone30} of "+ m_sParticleType+";p_{T}^{cone30};Entries", 60, 0., 15.);
    ptcone40       = Book1D("ptcone40", "p_{T}^{cone40} of "+ m_sParticleType+";p_{T}^{cone40};Entries", 60, 0., 15.);
    nucone20       = Book1D("nucone20", "N_{Trk}^{cone20} of "+ m_sParticleType+";N_{Trk}^{cone20};Entries", 15, 0., 15.);
    nucone30       = Book1D("nucone30", "N_{Trk}^{cone30} of "+ m_sParticleType+";N_{Trk}^{cone30};Entries", 15, 0., 15.);
    nucone40       = Book1D("nucone40", "N_{Trk}^{cone40} of "+ m_sParticleType+";N_{Trk}^{cone40};Entries", 15, 0., 15.);
    topoetcone20   = Book1D("topoetcone20", "Topo E_{T}^{cone20} of "+ m_sParticleType+";TopoE_{T}^{cone20};Entries", 60, -5., 25.);
    topoetcone30   = Book1D("topoetcone30", "Topo E_{T}^{cone30} of "+ m_sParticleType+";TopoE_{T}^{cone30};Entries", 60, -5., 25.);
    topoetcone40   = Book1D("topoetcone40", "Topo E_{T}^{cone40} of "+ m_sParticleType+";TopoE_{T}^{cone40};Entries", 60, -5., 25.);
  }



  void IsolationPlots::fill(const xAOD::Egamma& egamma){
    FillIsolationPlot(etcone20    ,xAOD::Iso::etcone20, egamma);
    FillIsolationPlot(etcone30    ,xAOD::Iso::etcone30, egamma);
    FillIsolationPlot(etcone40    ,xAOD::Iso::etcone40, egamma);
    FillIsolationPlot(ptcone20    ,xAOD::Iso::ptcone20, egamma);
    FillIsolationPlot(ptcone30    ,xAOD::Iso::ptcone30, egamma);
    FillIsolationPlot(ptcone40    ,xAOD::Iso::ptcone40, egamma);
    FillIsolationPlot(topoetcone20,xAOD::Iso::topoetcone20, egamma);
    FillIsolationPlot(topoetcone30,xAOD::Iso::topoetcone30, egamma);
    FillIsolationPlot(topoetcone40,xAOD::Iso::topoetcone40, egamma);
/*
    FillIsolationPlot(etcone20    ,xAOD::EgammaParameters::etcone20, egamma);
    FillIsolationPlot(etcone30    ,xAOD::EgammaParameters::etcone30, egamma);
    FillIsolationPlot(etcone40    ,xAOD::EgammaParameters::etcone40, egamma);
    FillIsolationPlot(ptcone20    ,xAOD::EgammaParameters::ptcone20, egamma);
    FillIsolationPlot(ptcone30    ,xAOD::EgammaParameters::ptcone30, egamma);
    FillIsolationPlot(ptcone40    ,xAOD::EgammaParameters::ptcone40, egamma);
    FillIsolationPlot(topoetcone20,xAOD::EgammaParameters::topoetcone20, egamma);
    FillIsolationPlot(topoetcone30,xAOD::EgammaParameters::topoetcone30, egamma);
    FillIsolationPlot(topoetcone40,xAOD::EgammaParameters::topoetcone40, egamma);
*/
  }

  void IsolationPlots::FillIsolationPlot(TH1* hist, xAOD::Iso::IsolationType isoType, const xAOD::Egamma& egamma){ //rel20
//  void IsolationPlots::FillIsolationPlot(TH1* hist, xAOD::EgammaParameters::IsolationType isoType, const xAOD::Egamma& egamma){//rel19
    float val = -10000000;
    if(egamma.isolationValue(val, isoType)){
      hist->Fill(val*0.001); //MeV to GeV conversion, make prettier
    }

  }


}
