/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTestToolBase.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"


#include "TH1D.h"
#include "TH2D.h"
#include <math.h>


MuonHitTestToolBase::MuonHitTestToolBase(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_detname("MDT"),
    m_pMuonMgr(0),
    m_muonevnt(0),m_muonrun(0),
    m_muoneta(0), m_muontheta(0), m_muonphi(0),
    m_muonzResid(0), m_muonphiResid(0),
    m_muondetBarrel(0), m_muonlongView(0),
    m_eta(0), m_theta(0), m_phi(0),
    m_zResid(0),m_phiResid(0),
    m_detBarrel(0), m_longView(0)

{
  declareProperty("BarrelEtaCut", m_BarrelEtaCut=99.);
  declareProperty("DetectorName", m_detname="MDT");

}

StatusCode MuonHitTestToolBase::executeCheckEventInfo()
{
  const EventInfo* pevt;
  CHECK(evtStore()->retrieve(pevt));
  int evt = pevt->event_ID()->event_number();
  int numrun = pevt->event_ID()->run_number();
  ATH_MSG_VERBOSE("Processing EventInfo event #"<< evt<< " run: " << numrun);
  m_muonevnt->Fill(evt);
  m_muonrun->Fill(numrun);
  const DataHandle<McEventCollection> mcEvent;
  CHECK(evtStore()->retrieve(mcEvent,m_key));

  // *AS* Why only if mcEvent ==1? when would there be more than one event?
  if (mcEvent->size()!=1) {
    m_direction=Amg::Vector3D(0.,0.,0.);
    return StatusCode::SUCCESS;
  }

  // *AS* Why this (double) loop, if only the last entry is preserved?
  //    changed it to take the gen particle
  DataVector<HepMC::GenEvent>::const_iterator e;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    HepMC::GenEvent::particle_const_iterator p;
    for (p= (**e).particles_begin(); p!= (**e).particles_end(); p++) {
      if ( HepMC::barcode(*p)<200000) {
        Amg::Vector3D temp_momentum((**p).momentum().px(),
                                    (**p).momentum().py(),
                                    (**p).momentum().pz());
        m_direction = temp_momentum.unit();
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode  MuonHitTestToolBase::executeFillHistos(const Amg::Vector3D & u) {
  ///for MDTs that have barrel + endcap section, take only the barrel when plotting the xy-2d plot of the detector

  if (m_direction.perp() > 0 && fabs(m_direction.eta())<m_BarrelEtaCut){
    //mdtdet->Fill(u.x(),u.y());
    m_muondetBarrel->Fill(u.x(),u.y());
    m_detBarrel->Fill(u.x(),u.y());
  }

  double rad=sqrt(u.x()*u.x()+u.y()*u.y());
  m_muonlongView->Fill(u.z(),rad);
  m_longView->Fill(u.z(),rad);

  // //m_direction vector is filled with truth above, so here it is wrong (no eta, theta, phi of the hit!!!)
  // *AS* why not use "u"?
  // theta->Fill(m_direction.theta());
  // theta->Fill(m_direction.theta());
  // eta->Fill(m_direction.eta());
  // eta->Fill(m_direction.eta());
  // phi->Fill(m_direction.phi());
  // phi->Fill(m_direction.phi());

  if (m_direction.perp() > 0) {
    m_muonzResid->Fill(u.cross(m_direction).dot(m_direction.cross(Amg::Vector3D(0,0,1)).unit()));
    m_muonphiResid->Fill(u.cross(m_direction).z());

    m_zResid->Fill(u.cross(m_direction).dot(m_direction.cross(Amg::Vector3D(0,0,1)).unit()));
    m_phiResid->Fill(u.cross(m_direction).z());
  }
  else {
    m_muonzResid->Fill(0);
    m_muonphiResid->Fill(0);
    m_zResid->Fill(0);
    m_phiResid->Fill(0);
  }

  return StatusCode::SUCCESS;
}



StatusCode MuonHitTestToolBase::initialize() {

  CHECK(detStore()->retrieve(m_pMuonMgr));

  //MuonSpectrometer
  /** Generic Muon part, these histograms are filled from
      all subalgorithms (MDT,RPC,CSC,TGC)
      For detailed description look in the .cxx code
  */

  m_path+="Muon/";
  // will only create new if not already registered (uses m_path)
  _TH1D(m_muonevnt,"event_num",100,0.,1000.);
  _TH1D(m_muonrun,"run_num",100,-300.,300.);

  /*
    TH1D *etamuon=new TH1D("muonhitpos_eta","muonhitpos_eta",50,-5.,5.);
    registerHistogram("/truth/muonhitpos_eta",etamuon);

    TH1D *theta=new TH1D("muonhitpos_theta","muonhitpos_theta",50,-10.,10.);
    registerHistogram("/truth/muonhitpos_theta",theta);

    TH1D *phimuon=new TH1D("muonhitpos_phi","muonhitpos_phi",50,-5.,5.);
    registerHistogram("/truth/muonhitpos_phi",phimuon);
  */

  _TH1D(m_muonzResid,"muonhitpos_zResid",50,-300.,300.);
  _TH1D(m_muonphiResid,"muonhitpos_phiResid",50,-300.,300.);

  _TH2D(m_muondetBarrel,"muondet_barrel",200,-11000.,11000.,200,-11000.,11000.);
  _TH2D(m_muonlongView,"muonlong_view",200,-24000.,24000.,200,0.,13000.);
  //  ================================================================================

  m_path+=m_detname+"/";
  // book Histograms
  /** specific part, these histograms are filled from  subalgorithm
      For detailed description look, e.g, in the CSCHitsTestAlg.cxx code
  */
  /*
    TH1D *eta=new TH1D("hitpos_eta","hitpos_eta",50,-5.,5.);
    registerHistogram("/truth/hitpos_eta",eta);

    TH1D *theta=new TH1D("hitpos_theta","hitpos_theta",50,-10.,10.);
    registerHistogram("/truth/hitpos_theta",theta);

    TH1D *phi=new TH1D("hitpos_phi","hitpos_phi",50,-5.,5.);
    registerHistogram("/truth/hitpos_phi",phi);
  */
  _TH1D(m_zResid,(m_detname+"_hitpos_zResid").c_str(),50,-300.,300.);
  _TH1D(m_phiResid,(m_detname+"_hitpos_phiResid").c_str(),50,-300.,300.);

  _TH2D(m_detBarrel,(m_detname+"_det_barrel").c_str(),200,-11000.,11000.,200,-11000.,11000.);
  _TH2D(m_longView,(m_detname+"_long_view").c_str(),200,-24000.,24000.,200,0.,14000.);

  return StatusCode::SUCCESS;

}
