/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHitsTestTool.h"

#include "GeoAdaptors/GeoLArHit.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "AtlasHepMC/GenParticle.h"

#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

LArHitsTestTool::LArHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_detname("EMB"),
    m_lar_eta(0), m_lar_phi(0),
    m_lar_zr(0), m_lar_etaphi(0),
    m_lar_edep_zr(0),
    m_eta(0), m_phi(0),
    m_zr(0), m_etaphi(0),
    m_time(0), m_edep(0), m_log_edep(0),
    m_edep_zr(0),
    m_eta_cut1(0),m_phi_cut1(0),
    m_zr_cut1(0),m_etaphi_cut1(0),
    m_time_cut1(0),m_edep_cut1(0),
    m_edep_zr_cut1(0),
    m_etot(0),
    m_edep_eta(0), m_edep_phi(0),
    m_edep_z(0), m_edep_r(0),
    m_etot_eta(0), m_etot_phi(0)
{  
  declareProperty("EDepCut", m_edep_cut=0.1);
  declareProperty("DetectorName", m_detname="EMB");
}

StatusCode LArHitsTestTool::initialize()
{
  m_path+="LAr/";
  // all LAr detectors
  _TH1D(m_lar_eta,"lar_eta",25,-5.,5.);
  _SET_TITLE(m_lar_eta, "LAr hit distribution","eta","dN/deta");
  _TH1D(m_lar_phi,"lar_phi",25,-M_PI,M_PI);
  _SET_TITLE(m_lar_phi, "LAr hit distribution","phi","dN/dphi");

  _TH2D(m_lar_zr,"lar_zr",100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_lar_zr, "LAr hit distribution","z [mm]","r [mm]");
  _TH2D(m_lar_etaphi,"lar_etaphi",100,-5.,5.,100,-M_PI,M_PI);
  _SET_TITLE(m_lar_etaphi, "LAr hit distribution","eta", "phi");
  _TH2D(m_lar_edep_zr,"lar_edep_zr",100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_lar_edep_zr, "LAr energy weighted hit distribution","z [mm]", "r [mm]");
  
  // variables specific to sub detector (root histo names have to be unique!)
  m_path+=m_detname+"/";
  _TH1D(m_eta,(m_detname+"_eta").c_str(),25,-5.,5.);
  _SET_TITLE(m_eta, "hit distribution","eta","dN/deta");
  _TH1D(m_phi,(m_detname+"_phi").c_str(),25,-M_PI,M_PI);
  _SET_TITLE(m_phi, "hit distribution","phi","dN/dphi");

  _TH2D(m_zr,(m_detname+"_zr").c_str(),100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_zr, "hit distribution","z [mm]","r [mm]");
  _TH2D(m_etaphi,(m_detname+"_etaphi").c_str(),100,-5.,5.,100,-M_PI,M_PI);
  _SET_TITLE(m_etaphi, "hit distribution","eta", "phi");
  _TH1D_WEIGHTED(m_time,(m_detname+"_time").c_str(),100,0,25);
  _SET_TITLE(m_time, "energy weighted hit distribution","t [ns]","dE/dt");

  _TH1D(m_edep,(m_detname+"_edep").c_str(),100,0,10.);
  _SET_TITLE(m_edep, "hit distribution","edep [MeV]","dN/dE [1/MeV]");
  _TH1D(m_log_edep,(m_detname+"_log_edep").c_str(),100,-15.,8.);
  _SET_TITLE(m_log_edep, "logarithm of energy deposits","log(edep [MeV])","dN/dlog(E) [1/log(MeV)]");
  _TH2D_WEIGHTED(m_edep_zr,(m_detname+"_edep_zr").c_str(),100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_edep_zr, "energy weighted hit distribution","z [mm]", "r [mm]");  

  // ----------------------------------------
  _TH1D(m_eta_cut1,(m_detname+"_eta_cut").c_str(),25,-5.,5.);
  _SET_TITLE(m_eta, "hit distribution","eta","dN/deta");
  _TH1D(m_phi_cut1,(m_detname+"_phi_cut").c_str(),25,-M_PI,M_PI);
  _SET_TITLE(m_phi, "hit distribution","phi","dN/dphi");

  _TH2D(m_zr_cut1,(m_detname+"_zr_cut").c_str(),100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_zr, "hit distribution","z [mm]","r [mm]");
  _TH2D(m_etaphi_cut1,(m_detname+"_etaphi_cut").c_str(),100,-5.,5.,100,-M_PI,M_PI);
  _SET_TITLE(m_etaphi, "hit distribution","eta", "phi");

  _TH1D_WEIGHTED(m_time_cut1,(m_detname+"_time_cut").c_str(),100,0,25);
  _SET_TITLE(m_time, "energy weighted hit distribution","t [ns]","dE/dt");
  _TH1D(m_edep_cut1,(m_detname+"_edep_cut").c_str(),100,0,10.);
  _SET_TITLE(m_edep_cut1, "hit distribution","edep [MeV]","dN/dE [1/MeV]");
  _TH2D_WEIGHTED(m_edep_zr_cut1,(m_detname+"_edep_zr_cut").c_str(),100,-5200.,5200.,100,0.,3000.);
  _SET_TITLE(m_edep_zr, "energy weighted hit distribution","z [mm]", "r [mm]");  
  
  // ----------------------------------------
  _TH1D(m_etot,(m_detname+"_etot").c_str(),100,0.,500.);
  _SET_TITLE(m_etot, "distribution total energy deposit per event","E_tot [MeV]","1/dE");
  _TH1D_WEIGHTED(m_edep_eta,(m_detname+"_edep_eta").c_str(),25,-5.,5.);
  _SET_TITLE(m_edep_eta, "energy weighted hit distribution","eta","dE/deta");
  _TH1D_WEIGHTED(m_edep_phi,(m_detname+"_edep_phi").c_str(),25,-M_PI,M_PI);
  _SET_TITLE(m_edep_phi, "energy weighted hit distribution","phi","dE/deta");
  _TH1D_WEIGHTED(m_edep_z,(m_detname+"_edep_z").c_str(),100,-5200.,5200.); 
  _SET_TITLE(m_edep_z, "energy weighted hit distribution","z [mm]","dE/dz");
  _TH1D_WEIGHTED(m_edep_r,(m_detname+"_edep_r").c_str(),100,0.,3000.);
  _SET_TITLE(m_edep_r, "energy weighted hit distribution","r [mm]","dE/dr");
  _TPROFILE(m_etot_eta,(m_detname+"_etot_eta").c_str(),25,-5.,5.); 
  _SET_TITLE(m_etot_eta, "tot energy deposited per Event vs generator truth","eta","1/N dE/deta [MeV]");
  _TPROFILE(m_etot_phi,(m_detname+"_etot_phi").c_str(),25,-M_PI,M_PI);
  _SET_TITLE(m_etot_phi, "tot energy deposited per Event vs generator truth","phi","1/N dE/dphi [MeV]");  

  return StatusCode::SUCCESS;
}

StatusCode LArHitsTestTool::processEvent() {

  LArHitContainer::const_iterator hi;
  std::string lArkey = "LArHit"+m_detname;

  double etot=0;
  const DataHandle<LArHitContainer> iter;
  CHECK(evtStore()->retrieve(iter,lArkey));
  for (hi=(*iter).begin(); hi != (*iter).end(); ++hi) {
    GeoLArHit ghit(**hi);
    if (!ghit) continue;
    const CaloDetDescrElement* ddElement = ghit.getDetDescrElement();

    double eta = ddElement->eta();
    double phi = ddElement->phi();
    double radius = ddElement->r();
    double z  = ddElement->z();

    // double deta = ddElement->deta();
    // double dphi = ddElement->dphi();
    // double dradius = ddElement->dr();
    double energy = ghit.Energy();
    // double t2 = 2*atan(exp(-(eta-deta/2.)));
    // double t1 = 2*atan(exp(-(eta+deta/2.)));
    // double p1 = phi-dphi/2.;
    // double p2 = phi+dphi/2.;
    // double rf = radius;
    // double rb = radius+ dradius;
    // double x  = ddElement->x();
    // double dx = ddElement->dx();
    // double y  = ddElement->y();
    // double dy = ddElement->dy();
    // double z  = ddElement->z();
    // double dz = ddElement->dz();
    // int samplingLayer = ghit.SamplingLayer();

    m_lar_eta->Fill(eta);
    m_lar_phi->Fill(phi);
    m_lar_zr->Fill(z,radius);
    m_lar_etaphi->Fill(eta,phi);
    m_lar_edep_zr->Fill(z,radius,energy);


    m_eta->Fill(eta);
    m_phi->Fill(phi);
    m_time->Fill( (*hi)->time(),energy);
    m_edep->Fill( energy);
    m_log_edep->Fill( energy > 0 ? log(energy) : -1 );
		 
    m_zr->Fill(z,radius);
    m_etaphi->Fill(eta,phi);
    m_edep_zr->Fill(z,radius,energy);

    m_edep_eta->Fill(eta,energy);
    m_edep_phi->Fill(phi,energy);

    m_edep_z->Fill(z,energy);
    m_edep_r->Fill(radius,energy);

    etot+=energy;

    if ((*hi)->energy()>m_edep_cut) {
      m_eta_cut1->Fill(eta);
      m_phi_cut1->Fill(phi);
      m_time_cut1->Fill( (*hi)->time(), (*hi)->energy());
      m_edep_cut1->Fill( (*hi)->energy());
      
      m_zr_cut1->Fill(z,radius);
      m_etaphi_cut1->Fill(eta,phi);
      m_edep_zr_cut1->Fill(z,radius,(*hi)->energy());
    }
  }
  
 
  //For FastCaloSim Container with _Fast postfix, just try to retrieve, if exist, collect information from this container, it not just skip it.
  
  LArHitContainer::const_iterator hi_fast;
  std::string lArkey_fast="LArHit"+m_detname+"_Fast";
  
  const DataHandle<LArHitContainer> iter_fast;
  if( evtStore()->contains(iter_fast, lArkey_fast) &&
      (evtStore()->retrieve(iter_fast,lArkey_fast)).isSuccess())
  {
    ATH_MSG_DEBUG ( "Read hit info from FastCaloSim Container" );
    for(hi_fast=(*iter_fast).begin();hi_fast!=(*iter_fast).end();++hi_fast) 
    {
      GeoLArHit ghit(**hi_fast); 
      if(!ghit) continue;
      const CaloDetDescrElement *ddElement=ghit.getDetDescrElement();
      double eta=ddElement->eta();
      double phi=ddElement->phi();
      double radius=ddElement->r();
      double z=ddElement->z();
      double energy=ghit.Energy();

      m_lar_eta->Fill(eta);
      m_lar_phi->Fill(phi);
      m_lar_zr->Fill(z,radius);
      m_lar_etaphi->Fill(eta,phi);
      m_lar_edep_zr->Fill(z,radius,energy);

      m_eta->Fill(eta);
      m_phi->Fill(phi);
      m_time->Fill((*hi_fast)->time(),energy);
      m_edep->Fill(energy);
      m_log_edep->Fill( log(energy));

      m_zr->Fill(z,radius);
      m_etaphi->Fill(eta,phi);
      m_edep_zr->Fill(z,radius,energy);

      m_edep_eta->Fill(eta,energy);
      m_edep_phi->Fill(phi,energy);

      m_edep_z->Fill(z,energy);
      m_edep_r->Fill(radius,energy);

      etot+=energy;

      if((*hi_fast)->energy() > m_edep_cut)
      {
        m_eta_cut1->Fill(eta); 
        m_phi_cut1->Fill(phi);
        m_time_cut1->Fill((*hi_fast)->time(),(*hi_fast)->energy());
        m_edep_cut1->Fill((*hi_fast)->energy());

        m_zr_cut1->Fill(z,radius);
        m_etaphi_cut1->Fill(eta,phi);
        m_edep_zr_cut1->Fill(z,radius,(*hi_fast)->energy());
      }
    }
  }
  auto primary = getPrimary();
  if (primary) {
    m_etot_eta->Fill(primary->momentum().eta(),etot);
    m_etot_phi->Fill(primary->momentum().phi(),etot);
  }
  m_etot->Fill(etot);
 
  return StatusCode::SUCCESS;
}
