/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthTestTool.h"

#include "AtlasHepMC/GenEvent.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GeneratorObjects/McEventCollection.h"
#include <TH1.h>
#include <TH2D.h>

#include <fstream>


TruthTestTool::TruthTestTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SimTestToolBase(type, name, parent),
    m_n_vert(0), m_n_part(0),
    m_n_vert_prim(0), m_n_vert_sec(0),
    m_n_part_prim(0), m_n_part_sec(0),
    m_x_vert(0), m_y_vert(0), m_z_vert(0),
    m_n_generations(0),
    m_vtx_r(0), m_vtx_z(0),
    m_vtx_zr_indet(0), m_vtx_xy_indet(0),
    m_meanx_vert(0), m_meany_vert(0), m_meanz_vert(0),
    m_px_truth(0), m_py_truth(0), m_pz_truth(0), m_pt_truth(0), m_log_pt_truth(0),
    m_theta_truth(0), m_eta_truth(0), m_phi_truth(0),
    m_barcode_small(0),m_barcode_large(0), m_log_barcode_large(0), m_particle_status(0),m_particle_type(0),
    m_p_gen(0), m_log_p_gen(0), m_eta_gen(0), m_phi_gen(0),m_pion_mass(0)
{
  declareProperty("DumpMcEvent", m_mcEventDump=10);
}


StatusCode TruthTestTool::initialize() 
{

  // initialize standard TruthTestTool
  std::string histdirname("Truth");
  if (m_key!="TruthEvent") {
    // initialize alternative TruthTestTool e.g. for GEN_EVENT
    histdirname = m_key;
  }
  m_path+=histdirname+"/";

  _TH1D(m_n_vert,"N_vert_all",50,0.,1000.);
  _SET_TITLE(m_n_vert,"total number of verticies","n","dN/dn");
  m_n_vert -> StatOverflows();
  _TH1D(m_n_part,"N_part_all",50,0.,5000.);
  _SET_TITLE(m_n_part,"total number of particles","n","dN/dn");
  m_n_part -> StatOverflows();

  _TH1D(m_n_vert_prim,"N_vert_prim",25,0.,750.);
  _SET_TITLE(m_n_vert_prim,"number of primary verticies","n","dN/dn");
  m_n_vert_prim -> StatOverflows();
  _TH1D(m_n_part_prim,"N_part_prim",25,0.,1500.);
  _SET_TITLE(m_n_part_prim,"number of primary particles","n","dN/dn");
  m_n_part_prim -> StatOverflows();
  _TH1D(m_n_vert_sec,"N_vert_sec",25,0.,750.);
  _SET_TITLE(m_n_vert_sec,"number of secondary verticies","n","dN/dn");
  m_n_vert_sec -> StatOverflows();
  _TH1D(m_n_part_sec,"N_part_sec",25,0.,1500.);
  _SET_TITLE(m_n_part_sec,"number of secondary particles","n","dN/dn");
  m_n_part_sec -> StatOverflows();

  _TH1D(m_x_vert,"X_vert_gen",100,-0.5,0.5);
  _SET_TITLE(m_x_vert,"x vertex distribution","x [mm]","dN/dx [1/mm]");
  _TH1D(m_y_vert,"Y_vert_gen",100,0.5,1.5);
  _SET_TITLE(m_y_vert,"y vertex distribution","y [mm]","dN/dy [1/mm]");
  _TH1D(m_z_vert,"Z_vert_gen",100,-300.,300.);
  _SET_TITLE(m_z_vert,"z vertex distribution","z [mm]","dN/dz [1/mm]");

  _TH1D(m_n_generations,"part_generation",26,-0.5,25.5);
  _SET_TITLE(m_n_generations,"number of particles per generation (prim,sec,...)","n","dN/dn");
  m_n_generations -> StatOverflows();

  _TH1D(m_vtx_r,"vertex_r",150,0.,4500.);
  _SET_TITLE(m_vtx_r, "GenVertex r positions","r [mm]","dN/dr");
  _TH1D(m_vtx_z,"vertex_z",150,-25000.,25000.);
  _SET_TITLE(m_vtx_z, "GenVertex z positions","z [mm]", "dN/dz");

  _TH2D(m_vtx_zr_indet,"vertex_zr_indet",100,-3200.,3200.,100,0.,1200.);
  _SET_TITLE(m_vtx_zr_indet, "GenVertex zr positions","z [mm]","r [mm]");
  _TH2D(m_vtx_xy_indet,"vertex_xy_indet",100,-1200.,1200.,100,-1200.,1200.);
  _SET_TITLE(m_vtx_xy_indet, "GenVertex xy positions","x [mm]","y [mm]");


  _TH1D(m_meanx_vert,"MeanX_vert_gen",100,-0.5,0.5);
  _SET_TITLE(m_meanx_vert,"mean x vertex distribution","x [mm]","dN/dx [1/mm]");
  _TH1D(m_meany_vert,"MeanY_vert_gen",100,0.5,1.5);
  _SET_TITLE(m_meany_vert,"mean y vertex distribution","y [mm]","dN/dy [1/mm]");
  _TH1D(m_meanz_vert,"MeanZ_vert_gen",100,-300.,300.);
  _SET_TITLE(m_meanz_vert,"mean z vertex distribution","z [mm]","dN/dz [1/mm]");

  // MC truth (incl. G4 particles created in the inner detector
  _TH1D(m_pt_truth,"Pt_truth",25,0.,50000.);
  _SET_TITLE(m_pt_truth,"pt distribution of MC truth","pt [MeV]","dN/dpt [1/MeV]");
  _TH1D(m_log_pt_truth,"log_Pt_truth",100.,0.,16.);
  _SET_TITLE(m_log_pt_truth,"log(pt) distribution of MC truth","log(pt [MeV])","dN/dlog(pt) [1/log(MeV)]");


  _TH1D(m_px_truth,"PX_truth",25,-50000.,50000.);
  _SET_TITLE(m_px_truth,"px distribution of MC truth","px [MeV]","dN/dp [1/MeV]");
  _TH1D(m_py_truth,"PY_truth",25,-50000.,50000.);
  _SET_TITLE(m_py_truth,"py distribution of MC truth","py [MeV]","dN/dp [1/MeV]");
  _TH1D(m_pz_truth,"PZ_truth",25,-50000.,50000.);
  _SET_TITLE(m_pz_truth,"pz distribution of MC truth","pz [MeV]","dN/dp [1/MeV]");

  _TH1D(m_theta_truth,"theta_truth",25,0,M_PI);
  _SET_TITLE(m_theta_truth,"theta distribution of MC truth","theta","dN/dtheta");
  _TH1D(m_eta_truth,"eta_truth",25,-5.,5.);
  _SET_TITLE(m_eta_truth,"eta distribution of MC truth","eta","dN/deta");
  _TH1D(m_phi_truth,"phi_truth",25,-M_PI,M_PI);
  _SET_TITLE(m_phi_truth,"phi distribution of MC truth","phi","dN/dphi");

  _TH1D(m_barcode_large,(histdirname+"_barcode1").c_str(),100,0.,1500000.);
  m_barcode_large -> StatOverflows();
  _TH1D(m_log_barcode_large,(histdirname+"_log_barcode1").c_str(),50,0.,16.);
  _TH1D(m_barcode_small,(histdirname+"_barcode2").c_str(),100,10000.,10100.);

  _TH1D(m_particle_status,"particle_status",5,-0.5,4.5);
  _SET_TITLE(m_particle_status,"Particle Status (1==undecayed, 2==decayed, 4==beam particle)","GenParticle Status","N GenParticles");
  _TH1D(m_particle_type,"particle_type",15,-7.5,7.5);
  _SET_TITLE(m_particle_type,"Type (Type 0 = PDG 22, 1=11, 2=13,15, 3=111,211, 4=130,310,311,321, 5=2122, 6=2212, 7=else)","Particle Type (negative for anti-particles)","N Particles");

 
  // generator particles
  _TH1D(m_eta_gen,"eta_gen",25,-5.,5.);
  _SET_TITLE(m_eta_gen,"eta distribution of generator particles","eta","dN/deta");
  _TH1D(m_phi_gen,"phi_gen",25,-M_PI,M_PI);
  _SET_TITLE(m_phi_gen,"phi distribution of generator particles","phi","dN/dphi");
  _TH1D(m_p_gen,"p_gen",50,0.,250000.);
  _SET_TITLE(m_p_gen,"momentum distribution of generator particles","p","dN/dp [1/MeV]");
  _TH1D(m_log_p_gen,"log_p_gen",50,0.,16.);
  _SET_TITLE(m_log_p_gen,"momentum distribution of generator particles","log(p)","dN/dlog(p) [1/log(MeV)]");

  
  _TH1D(m_pion_mass,"p_pion_mass",50,114.,164.);
  _SET_TITLE(m_pion_mass,"pion mass distribution (generator particles)","m [MeV]","dN/dm [1/MeV]");
  
  return StatusCode::SUCCESS;
}


StatusCode TruthTestTool::processEvent() 
{
  const DataHandle<EventInfo> event;
  if (!evtStore()->retrieve(event,"McEventInfo").isSuccess())
    return StatusCode::FAILURE;

  const int evtnum(event->event_ID()->event_number());

  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,m_key).isSuccess()) {
    McEventCollection::const_iterator currentGenEventIter = mcCollection->begin();

    if (currentGenEventIter!=mcCollection->end() ) {
      {
        auto  vtx = HepMC::signal_process_vertex(*currentGenEventIter);
        if (!vtx && (*currentGenEventIter)->vertices_size()>0) vtx=*((*currentGenEventIter)->vertices_begin());
        if ( vtx )
        {  
          m_x_vert->Fill( vtx->position().x() );
          m_y_vert->Fill( vtx->position().y() );
          m_z_vert->Fill( vtx->position().z() );
        }
      }

      int nvtx=0;
      int nvtx_sec=0;
      float mx=0.,my=0.,mz=0.;
      for (HepMC::GenEvent::vertex_const_iterator vtx=(*currentGenEventIter)->vertices_begin(); 
           vtx!=(*currentGenEventIter)->vertices_end();++vtx) {

        double x = (*vtx)->position().x();
        double y = (*vtx)->position().y();
        double z = (*vtx)->position().z();
        double r = sqrt(x*x+y*y);
        m_vtx_r->Fill(r);
        m_vtx_z->Fill(z);
        m_vtx_zr_indet->Fill(z, r);
        m_vtx_xy_indet->Fill(x, y);
        if ( HepMC::barcode(*vtx)>-20000 ) {
          mx+=x;
          my+=y;
          mz+=z;
          ++nvtx;
        }
        else {
          // TODO: add 2D distribution of secondary verticies (and primary ?)
          ++nvtx_sec;
        }
      }
      if (nvtx>0) {
        m_meanx_vert->Fill( mx/nvtx );
        m_meany_vert->Fill( my/nvtx );
        m_meanz_vert->Fill( mz/nvtx );  
      }
    
      m_n_vert->Fill( nvtx + nvtx_sec );
      m_n_vert_prim->Fill( nvtx);
      m_n_vert_sec->Fill( nvtx_sec );      
           
    }

    for (;currentGenEventIter!=mcCollection->end(); ++currentGenEventIter) {
      if (evtnum<m_mcEventDump) {
        char fname[80];
        sprintf(fname,"%s.event%d.txt",m_key.c_str(),evtnum);
        std::ofstream of(fname);
        (*currentGenEventIter)->print(of); // verbose output
        of.close();
      }

      int npart_prim=0, npart_sec=0;
      HepMC::GenEvent::particle_const_iterator currentGenParticleIter;
      for (currentGenParticleIter= (*currentGenEventIter)->particles_begin(); 
           currentGenParticleIter!= (*currentGenEventIter)->particles_end(); ++currentGenParticleIter) {

        const HepMC::FourVector mom = (*currentGenParticleIter)->momentum();
        m_px_truth->Fill( mom.x() );
        m_py_truth->Fill( mom.y() );
        m_pz_truth->Fill( mom.z() );
        m_pt_truth->Fill( mom.perp() );
        m_log_pt_truth->Fill( mom.perp() > 0 ? log(mom.perp()) : -1 );

        m_theta_truth->Fill( mom.theta() );
        m_eta_truth->Fill( mom.eta() );
        m_phi_truth->Fill( mom.phi() );

        if(std::abs((*currentGenParticleIter)->pdg_id())==211) {
          m_pion_mass->Fill(mom.m());
        }
        m_barcode_small->Fill(HepMC::barcode(*currentGenParticleIter));
        m_barcode_large->Fill(HepMC::barcode(*currentGenParticleIter));
        int barcode = HepMC::barcode(*currentGenParticleIter);
        m_log_barcode_large->Fill( barcode > 0 ? log(barcode) : -1);
        m_particle_status->Fill((*currentGenParticleIter)->status());

        int pdg = (*currentGenParticleIter)->pdg_id();
        int particleType = 100;
        switch ( abs(pdg) ) {
          case 22:
            particleType = 0;
            break;
          case 11:
            particleType = 1;
            break;
          case 13:
          case 15:
            particleType = 2;
            break;
          case 111:
          case 211:
            particleType = 3;
            break;
          case 130:
          case 310:
          case 311:
          case 321:
            particleType = 4;
            break;
          case 2112:
            particleType = 5;
            break;
          case 2212:
            particleType = 6;
            break;
          default:
            particleType = 7;
            break;
        }
        particleType = (pdg<0) ? -particleType : particleType;
        m_particle_type->Fill( particleType );

        if ( HepMC::barcode(*currentGenParticleIter)<200000 ) {
          m_p_gen->Fill( mom.rho() );
          m_log_p_gen->Fill( log(mom.rho()) );
          m_eta_gen->Fill( mom.eta() );
          m_phi_gen->Fill( mom.phi() );
          ++npart_prim;
          if ( HepMC::barcode(*currentGenParticleIter)<10000 ) {
            m_n_generations ->Fill(0);
          }
          else {
            m_n_generations ->Fill(1);
          }
        }
        else {
          ++npart_sec;
          const int gen = HepMC::barcode(*currentGenParticleIter)/ 1000000 +2;
          m_n_generations ->Fill(gen);
        }
      }
      m_n_part_prim->Fill(npart_prim);
      m_n_part_sec->Fill(npart_sec);
      m_n_part->Fill(npart_prim+npart_sec);
    }
  }

  return StatusCode::SUCCESS;
}
