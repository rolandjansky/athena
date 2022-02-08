/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "IsolationPlots.h"

using CLHEP::GeV;

namespace Egamma{


  IsolationPlots::IsolationPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												 m_sParticleType(std::move(sParticleType)),
												 etcone20(nullptr),       
												 etcone30(nullptr),       
												 etcone40(nullptr),
												 ptcone20(nullptr),       
												 ptcone30(nullptr),       
												 ptcone40(nullptr),       
												 ptvarcone20(nullptr),       
												 ptvarcone30(nullptr),       
												 ptvarcone40(nullptr),       
												 topoetcone20(nullptr),   
												 topoetcone30(nullptr),   
												 topoetcone40(nullptr)
    {}

  void IsolationPlots::initializePlots(){
                     
    etcone20       = Book1D("etcone20", "e_{T}^{cone20} of "+ m_sParticleType+";e_{T}^{cone20};Entries", 65, -5., 60.);
    etcone30       = Book1D("etcone30", "e_{T}^{cone30} of "+ m_sParticleType+";e_{T}^{cone30};Entries", 65, -5., 60.);
    etcone40       = Book1D("etcone40", "e_{T}^{cone40} of "+ m_sParticleType+";e_{T}^{cone40};Entries", 65, -5., 60.); 
    ptcone20       = Book1D("ptcone20", "p_{T}^{cone20} of "+ m_sParticleType+";p_{T}^{cone20};Entries", 65, -5., 60.);
    ptcone30       = Book1D("ptcone30", "p_{T}^{cone30} of "+ m_sParticleType+";p_{T}^{cone30};Entries", 65, -5., 60.);
    ptcone40       = Book1D("ptcone40", "p_{T}^{cone40} of "+ m_sParticleType+";p_{T}^{cone40};Entries", 65, -5., 60.);
    ptvarcone20       = Book1D("ptvarcone20", "p_{T}^{varcone20} of "+ m_sParticleType+";p_{T}^{varcone20};Entries", 65, -5., 60.);
    ptvarcone30       = Book1D("ptvarcone30", "p_{T}^{varcone30} of "+ m_sParticleType+";p_{T}^{varcone30};Entries", 65, -5., 60.);
    ptvarcone40       = Book1D("ptvarcone40", "p_{T}^{varcone40} of "+ m_sParticleType+";p_{T}^{varcone40};Entries", 65, -5., 60.);
    topoetcone20   = Book1D("topoetcone20", "Topo E_{T}^{cone20} of "+ m_sParticleType+";TopoE_{T}^{cone20};Entries", 90, -10., 80.);
    topoetcone30   = Book1D("topoetcone30", "Topo E_{T}^{cone30} of "+ m_sParticleType+";TopoE_{T}^{cone30};Entries", 90, -10., 80.);
    topoetcone40   = Book1D("topoetcone40", "Topo E_{T}^{cone40} of "+ m_sParticleType+";TopoE_{T}^{cone40};Entries", 90, -10., 80.);

  }



  void IsolationPlots::fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo) const{
    FillIsolationPlot(etcone20       ,xAOD::Iso::etcone20, egamma, eventInfo);
    FillIsolationPlot(etcone30       ,xAOD::Iso::etcone30, egamma, eventInfo);
    FillIsolationPlot(etcone40       ,xAOD::Iso::etcone40, egamma, eventInfo);
    FillIsolationPlot(ptcone20       ,xAOD::Iso::ptcone20, egamma, eventInfo);
    FillIsolationPlot(ptcone30       ,xAOD::Iso::ptcone30, egamma, eventInfo);
    FillIsolationPlot(ptcone40       ,xAOD::Iso::ptcone40, egamma, eventInfo);
    FillIsolationPlot(ptvarcone20    ,xAOD::Iso::ptvarcone20, egamma, eventInfo);
    FillIsolationPlot(ptvarcone30    ,xAOD::Iso::ptvarcone30, egamma, eventInfo);
    FillIsolationPlot(ptvarcone40    ,xAOD::Iso::ptvarcone40, egamma, eventInfo);
    FillIsolationPlot(topoetcone20   ,xAOD::Iso::topoetcone20, egamma, eventInfo);
    FillIsolationPlot(topoetcone30   ,xAOD::Iso::topoetcone30, egamma, eventInfo);
    FillIsolationPlot(topoetcone40   ,xAOD::Iso::topoetcone40, egamma, eventInfo);
  }

  void IsolationPlots::FillIsolationPlot(TH1* hist, xAOD::Iso::IsolationType isoType, const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo){ //rel20
//  void IsolationPlots::FillIsolationPlot(TH1* hist, xAOD::EgammaParameters::IsolationType isoType, const xAOD::Egamma& egamma){//rel19
    float val = -10000000;
    float weight = 1.;
    weight = eventInfo.beamSpotWeight();
    
    if(egamma.isolationValue(val, isoType)){
      hist->Fill(val/GeV,weight); 
    }

  }


}
