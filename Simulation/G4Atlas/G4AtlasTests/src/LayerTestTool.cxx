/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LayerTestTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"

#include <TH1.h>
#include <TH2.h>

LayerTestTool::LayerTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent), m_collection("CaloEntry"), m_n_energy(0),m_n_energy_e(0),m_n_energy_ep(0),
    m_n_energy_gamma(0),m_n_energy_neutron(0),m_n_energy_muon(0),m_n_energy_muonp(0),
    m_n_pos(0),m_n_px(0),m_n_py(0),m_n_pz(0),m_n_rho(0),m_n_eta(0),m_n_phi(0), 
    m_energy(0),m_pos_x(0),m_pos_y(0),m_pos_z(0),m_pos_r(0),m_p_x(0),m_p_y(0),m_p_z(0),m_p_rho(0),m_p_phi(0),m_p_eta(0)
{
    declareProperty("CollectionName",  m_collection="CaloEntry");
}


StatusCode LayerTestTool::initialize() 
{
  m_path+="Layer/";
  m_path+=m_collection+"/";

  _TH1D(m_n_energy,(m_collection+"_energy").c_str(),31,0.,2000000.);
  _SET_TITLE(m_n_energy,"energy","E MeV","n");

  _TH1D(m_n_energy_e,(m_collection+"_energy_e").c_str(),31,0.,20000.);
  _SET_TITLE(m_n_energy_e,"energy","E MeV","n");

  _TH1D(m_n_energy_ep,(m_collection+"_energy_ep").c_str(),31,0.,20000.);
  _SET_TITLE(m_n_energy_ep,"energy","E MeV","n");

  _TH1D(m_n_energy_gamma,(m_collection+"_energy_gamma").c_str(),31,0.,20000.);
  _SET_TITLE(m_n_energy_gamma,"energy","E MeV","n");

  _TH1D(m_n_energy_neutron,(m_collection+"_energy_neutron").c_str(),31,0.,20000.);
  _SET_TITLE(m_n_energy_neutron,"energy","E MeV","n");

  _TH1D(m_n_energy_muon,(m_collection+"_energy_muon").c_str(),31,0.,2000000.);
  _SET_TITLE(m_n_energy_muon,"energy","E MeV","n");

  _TH1D(m_n_energy_muonp,(m_collection+"_energy_muonp").c_str(),31,0.,2000000.);
  _SET_TITLE(m_n_energy_muonp,"energy","E MeV","n");

  _TH1D(m_n_px,(m_collection+"_px").c_str(),31,-500000.,500000.);
  _SET_TITLE(m_n_px,"px distribution","px MeV","n");
  _TH1D(m_n_py,(m_collection+"_py").c_str(),31,-500000.,500000.);
  _SET_TITLE(m_n_py,"py distribution","py MeV","n");
  _TH1D(m_n_pz,(m_collection+"_pz").c_str(),31,-500000.,500000.);
  _SET_TITLE(m_n_pz,"pz distribution","pz MeV","n");

  _TH1D(m_n_rho,(m_collection+"_rho").c_str(),31,0,50000.);
  _SET_TITLE(m_n_rho,"rho distribution","rho","n");
  _TH1D(m_n_eta,(m_collection+"_eta").c_str(),31,-5.,5.);
  _SET_TITLE(m_n_eta,"eta distribution","eta","n");
  _TH1D(m_n_phi,(m_collection+"_phi").c_str(),31,-5.,5.);
  _SET_TITLE(m_n_phi,"phi distribution","phi","n");

  _TH2D(m_n_pos,(m_collection+"_pos").c_str(),31,-7500.,7500.,31,0.,4500.);
  _SET_TITLE(m_n_pos,"pos","z","r");
  return StatusCode::SUCCESS;
}


StatusCode LayerTestTool::processEvent() 
{
  //std::cout<<"LayerTestTool::processEvent()"<<std::endl;
  const TrackRecordCollection * trCollection = 0;
  m_key=m_collection+"Layer";

  if (evtStore()->retrieve(trCollection,m_key).isSuccess()) {
   
    for(const auto& e : *trCollection){

      m_energy = e.GetEnergy();
      m_n_energy->Fill(m_energy);

      if(e.GetPDGCode() == 11){
        m_n_energy_e->Fill(m_energy);
      }

      if(e.GetPDGCode() == -11){
        m_n_energy_ep->Fill(m_energy);
      }

      if(e.GetPDGCode() == 22){
        m_n_energy_gamma->Fill(m_energy);
      }

      if(e.GetPDGCode() == 2112){
        m_n_energy_neutron->Fill(m_energy);
      }

      if(e.GetPDGCode() == 13){
        m_n_energy_muon->Fill(m_energy);
      }

      if(e.GetPDGCode() == -13){
        m_n_energy_muonp->Fill(m_energy);
      }

      m_pos_x = e.GetPosition().x();
      m_pos_y = e.GetPosition().y();
      m_pos_z = e.GetPosition().z();
      m_pos_r= std::sqrt(m_pos_x*m_pos_x+m_pos_y*m_pos_y);
      m_n_pos->Fill(m_pos_z,m_pos_r);

      m_p_x=e.GetMomentum().x();
      m_p_y=e.GetMomentum().y();
      m_p_z=e.GetMomentum().z();
      //    std::cout<<"px="<<m_p_x<<";py="<<m_p_y<<";pz="<<m_p_z<<std::endl;
      m_n_px->Fill(m_p_x);
      m_n_py->Fill(m_p_y);
      m_n_pz->Fill(m_p_z);

      m_p_rho=e.GetMomentum().rho();
      m_p_phi=e.GetMomentum().phi();
      m_p_eta=e.GetMomentum().eta();
      //    std::cout<<"rho="<<m_p_rho<<";phi="<<m_p_phi<<";eta="<<m_p_eta<<std::endl;
      m_n_rho->Fill(m_p_rho);
      m_n_eta->Fill(m_p_eta);
      m_n_phi->Fill(m_p_phi);
    }

  }

  return StatusCode::SUCCESS;
}
