/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LucidHitsTestTool.h"

#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
//#include "LUCID_SimEvent/LUCID_SimHit.h"
#include "AtlasHepMC/GenParticle.h"

#include <cmath>
#include <string>
#include <cmath>
#include <TH2D.h>
#include <TH1D.h>

LucidHitsTestTool::LucidHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent)
  , m_x_v_y_edep(0), m_x_v_y_hits(0), m_R_v_z_edep(0), m_R_v_z_hits(0)
  , m_x_v_y_post(0), m_R_v_z_post(0), m_time_edep(0)
  , m_pdgid(0), m_tubeid(0), m_gen_volume(0), m_wavelength(0), m_hit_edep(0), m_total_e(0)
{;}

StatusCode LucidHitsTestTool::initialize()
{
  m_path+="Lucid/";
  // ----------------------------------------
  _TH2D_WEIGHTED(m_x_v_y_edep,"x_v_y_edep",100,-500.,500.,100,-500.,500.);
  _SET_TITLE(m_x_v_y_edep, "Energy distribution [MeV]","x [mm]","y [mm]");
  _TH2D(m_x_v_y_hits,"x_v_y_hits",100,-500.,500.,100,-500.,500.);
  _SET_TITLE(m_x_v_y_hits, "Hit distribution","x [mm]","y [mm]");

  _TH2D_WEIGHTED(m_R_v_z_edep,"R_v_z_edep",100,0.,500.,100,5500.,8500.);
  _SET_TITLE(m_R_v_z_edep, "Energy distribution [MeV]","R [mm]","z [mm]");
  _TH2D(m_R_v_z_hits,"R_v_z_hits",100,0.,500.,100,5500.,8500.);
  _SET_TITLE(m_R_v_z_hits, "Hit distribution","R [mm]","z [mm]");

  _TH2D(m_x_v_y_post,"x_v_y_post",100,-500.,500.,100,-500.,500.);
  _SET_TITLE(m_x_v_y_post, "Post hit distribution","x [mm]","y [mm]");
  _TH2D(m_R_v_z_post,"R_v_z_post",100,0.,500.,100,5500.,8500.);
  _SET_TITLE(m_R_v_z_post, "Post hit distribution","R [mm]","z [mm]");

  _TH1D(m_pdgid,"pdg_id",1000,-3000,3000.);
  _SET_TITLE(m_pdgid, "PDG ID","PDG ID","Hits");

  _TH2D_WEIGHTED(m_time_edep,"Time_edep",100,-10.,50.,100,-10.,50.);
  _SET_TITLE(m_time_edep, "Energy distribution [MeV]","Time 1 [ns]","Time 2 [ns]");

  _TH1D(m_tubeid,"tube_id",1000,0,10000.);
  _SET_TITLE(m_tubeid, "Tube ID","Tube ID","Hits");

  _TH1D(m_gen_volume,"gen_volume",1000,0,100000.);
  _SET_TITLE(m_gen_volume, "Volume","Volume","Hits");

  _TH1D(m_wavelength,"wavelength",1000,0,3000.);
  _SET_TITLE(m_wavelength, "Wavelength","Wavelength","Hits");

  _TH1D(m_hit_edep,"hit_edep",100,0,10000.);
  _SET_TITLE(m_hit_edep, "Energy deposited per hit","Energy [MeV]","Hits");

  _TH1D(m_total_e,"total_e",1000,0,100000.);
  _SET_TITLE(m_total_e, "Total energy deposited","Energy [MeV]","Events");

  return StatusCode::SUCCESS;
}

StatusCode LucidHitsTestTool::processEvent() {

  double etot = 0;
  const DataHandle<LUCID_SimHitCollection> iter;
  CHECK(evtStore()->retrieve(iter));
  for (LUCID_SimHitCollection::const_iterator hi=(*iter).begin(); hi != (*iter).end(); ++hi) {
    m_x_v_y_edep->Fill( hi->GetX() , hi->GetY() , hi->GetEnergy() );
    m_x_v_y_hits->Fill( hi->GetX() , hi->GetY() );
    m_R_v_z_edep->Fill( std::sqrt( hi->GetX()*hi->GetX() + hi->GetY()*hi->GetY() ) , std::abs( hi->GetZ() ) , hi->GetEnergy() );
    m_R_v_z_hits->Fill( std::sqrt( hi->GetX()*hi->GetX() + hi->GetY()*hi->GetY() ) , std::abs( hi->GetZ() ) );
    m_x_v_y_post->Fill( hi->GetEPX() , hi->GetEPY() );
    m_R_v_z_post->Fill( std::sqrt( hi->GetEPX()*hi->GetEPX() + hi->GetEPY()*hi->GetEPY() ) , std::abs( hi->GetEPZ() ) );

    m_pdgid->Fill( hi->GetPdgCode() );
    m_time_edep->Fill( hi->GetPreStepTime() , hi->GetPostStepTime() , hi->GetEnergy() );
    m_tubeid->Fill( hi->GetTubeID() );
    m_gen_volume->Fill( hi->GetGenVolume() );
    m_wavelength->Fill( hi->GetWavelength() );
    m_hit_edep->Fill( hi->GetEnergy() );

    etot+=hi->GetEnergy();
  }
  m_total_e->Fill( etot );

  return StatusCode::SUCCESS;
}
