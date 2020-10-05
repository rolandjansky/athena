/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------- 
// Generators/Epos.cxx
//  
// Description: Allows the user to generate epos events and store the result 
// in the Transient Store.
//
// AuthorList: 
//   Sami Kama:       Initial code.
//   Sebastian Piec:  Adaptation for Epos 1.99.crmc.r2790.
// ---------------------------------------------------------------------- 

#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "AtlasHepMC/HEPEVT_Wrapper.h"
#include "AtlasHepMC/IO_HEPEVT.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/HeavyIon.h"


#include "Epos_i/Epos.h"
#include "Epos_i/EposFort.h"



namespace{
  static std::string epos_rndm_stream = "EPOS_INIT";
  static IAtRndmGenSvc* p_AtRndmGenSvcEpos = 0;
}

extern "C" double atl_epos_rndm_( int* ) 
{
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvcEpos->GetEngine(epos_rndm_stream);
  return CLHEP::RandFlat::shoot(engine);
}

// ---------------------------------------------------------------------- 
// Epos Fortran bindings.
// ---------------------------------------------------------------------- 
extern "C" 
{
    // generator initialization
  void crmc_set_f_(int &nEvents,int &iSeed,double &beamMomentum, double &targetMomentum, int &primaryParticle, int &targetParticle, int &model, int &itab, int &itypout, const char *paramFile);

  void crmc_init_f_();
  //  void crmc_init_f_( int &iSeed, double &beamMomentum, double &targetMomentum, int &primaryParticle, int &targetParticle, int &model, const char *paramFile );
    // event generation
  void crmc_f_( int &iout, int &ievent, int &nParticles, double &impactParam, int &partPdg, 
		double &partPx, double &partPy, double &partPz, double &partEnergy, double &partMass, int &outstat );

    // cross section info 
  void crmc_xsection_f_(double &xsigtot, double &xsigine, double &xsigela, double &xsigdd, 
      double &xsigsd, double &xsloela, double &xsigtotaa, double &xsigineaa, double &xsigelaaa);
}
/*
extern "C"
{
  extern struct
  {
    float sigtot;
    float sigcut;
    float sigela;
    float sloela;
    float sigsd;
    float sigine;
    float sigdif;
    float sigineaa;
    float sigtotaa;
    float sigelaaa;
    float sigcutaa;
  } hadr5_; //crmc-aaa.f
}

extern "C"
{
  extern struct
  {
    // nevt .......... error code. 1=valid event, 0=invalid event
    // bimevt ........ absolute value of impact parameter
    // phievt ........ angle of impact parameter
    // kolevt ........ number of collisions
    // koievt ........ number of inelastic collisions
    // pmxevt ........ reference momentum
    // egyevt ........ pp cm energy (hadron) or string energy (lepton)
    // npjevt ........ number of primary projectile participants
    // ntgevt ........ number of primary target participants
    // npnevt ........ number of primary projectile neutron spectators
    // nppevt ........ number of primary projectile proton spectators
    // ntnevt ........ number of primary target neutron spectators
    // ntpevt ........ number of primary target proton spectators
    // jpnevt ........ number of absolute projectile neutron spectators
    // jppevt ........ number of absolute projectile proton spectators
    // jtnevt ........ number of absolute target neutron spectators
    // jtpevt ........ number of absolute target proton spectators
    // xbjevt ........ bjorken x for dis
    // qsqevt ........ q**2 for dis
    // sigtot ........ total cross section
    // nglevt ........ number of collisions acc to  Glauber
    // zppevt ........ average Z-parton-proj
    // zptevt ........ average Z-parton-targ
    // ng1evt ........ number of Glauber participants with at least one IAs
    // ng2evt ........ number of Glauber participants with at least two IAs
    // ikoevt ........ number of elementary parton-parton scatterings
    // typevt ........ type of event (1=Non Diff, 2=Double Diff, 3=Single Diff
    float phievt;
    int nevt;
    float bimevt;
    int kolevt;
    int koievt;
    float pmxevt;
    float egyevt;
    int npjevt;
    int ntgevt;
    int npnevt;
    int nppevt;
    int ntnevt;
    int ntpevt;
    int jpnevt;
    int jppevt;
    int jtnevt;
    int jtpevt;
    float xbjevt;
    float qsqevt;
    int nglevt;
    float zppevt;
    float zptevt;
    int minfra;
    int maxfra;
    int kohevt;
  } cevt_; //epos.inc
}

extern "C"
{
  extern struct
  {
    int ng1evt;
    int ng2evt;
    float rglevt;
    float sglevt;
    float eglevt;
    float fglevt;
    int ikoevt;
    float typevt;
  } c2evt_; //epos.inc
}

*/


// ----------------------------------------------------------------------
Epos::Epos( const std::string &name, ISvcLocator *pSvcLocator ): 
  GenModule( name, pSvcLocator )
{
  epos_rndm_stream = "EPOS_INIT";
  
  declareProperty( "BeamMomentum",    m_beamMomentum    = -3500.0 );      // GeV
  declareProperty( "TargetMomentum",  m_targetMomentum  = 3500.0 );
  declareProperty( "Model",           m_model           = 7 );            // 0=EPOS 1.99 LHC, 1=EPOS 1.99
  declareProperty( "PrimaryParticle", m_primaryParticle = 1 );            // 1=p, 12=C, 120=pi+, 207=Pb 
  declareProperty( "TargetParticle",  m_targetParticle  = 1 );
  declareProperty( "ParamFile",       m_paramFile       = "crmc.param" );
  declareProperty( "LheOutput",       m_ilheout       = 0 );
  declareProperty( "LheFile",         m_lheout       = "epos.lhe" );
  declareProperty( "TabCreate",       m_itab       = 0 );
  declareProperty( "nEvents",         m_nEvents    = 5500 );
  
  m_events = 0; // current event number (counted by interface)
  m_ievent = 0;  // current event number counted by Epos
  m_iout = 0; // output type (output)

  // initialize internally used arrays
  for (size_t i = 0; i < kMaxParticles; ++i) {
    m_partID[i] = 0;
    m_partPx[i] = m_partPy[i] = m_partPz[i] = m_partEnergy[i] = m_partMass[i] = 0.0;
    m_partStat[i] = 0;
  }

}

// ----------------------------------------------------------------------
Epos::~Epos()
{}

// ----------------------------------------------------------------------
StatusCode Epos::genInitialize() 
{
  ATH_MSG_INFO( " CRMC INITIALISING.\n" );
  ATH_MSG_INFO( "signal_rocess_id 101-ND, 105-DD, 102-CD, 103 AB->XB, 104 AB->AX \n");

  static const bool CREATEIFNOTTHERE = true;
  
  StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvcEpos, CREATEIFNOTTHERE);
  if ( !RndmStatus.isSuccess() || 0 == p_AtRndmGenSvcEpos ) {
    ATH_MSG_ERROR( " Could not initialize Random Number Service!" );
    return RndmStatus;
  }
   
  CLHEP::HepRandomEngine *engine = p_AtRndmGenSvcEpos->GetEngine(epos_rndm_stream);
  const long *sip = engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  int iSeed = si1%1000000000;     // FIXME ?

  // set up initial values

  //   std::cout << "parameters " << m_nEvents << " " << iSeed << " " << m_beamMomentum << " " << m_targetMomentum << " " << m_primaryParticle << " " << m_targetParticle << " " << m_model << " " << m_itab << " " << m_ilheout << " " <<  m_lheout.c_str()<< " " <<  m_paramFile.c_str() << std::endl;

    crmc_set_f_(m_nEvents, iSeed, m_beamMomentum, m_targetMomentum, m_primaryParticle, m_targetParticle, m_model, m_itab, m_ilheout, m_paramFile.c_str() ); 

    // initialize Epos
  //  crmc_init_f_( iSeed, m_beamMomentum, m_targetMomentum, m_primaryParticle, m_targetParticle, m_model, m_paramFile.c_str() );
  crmc_init_f_();

    // ... and set them back to the stream for proper save
  p_AtRndmGenSvcEpos->CreateStream( si1, si2, epos_rndm_stream );

  epos_rndm_stream = "EPOS";

    // setup HepMC
#ifdef HEPMC3
     /* This ifdef is used for consistency */
     /* HepMC3 does not need this setup */
#else    
    HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof( int ));
    HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 );
    HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);    // as used in crmc-aaa.f!!!
#endif

  m_events = 0;

  //  m_ascii_out = new HepMC::IO_GenEvent(m_eposEventInfo);
 
 return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode Epos::callGenerator() 
{
  // ATH_MSG_INFO( " EPOS Generating." );

    // save the random number seeds in the event
    CLHEP::HepRandomEngine* engine = p_AtRndmGenSvcEpos->GetEngine( epos_rndm_stream );
   const long *s = engine->getSeeds();
  
   m_seeds.clear();
   m_seeds.push_back(s[0]);
   m_seeds.push_back(s[1]);

   ++m_events;
  
    // as in crmchep.h
  int nParticles = 0;
  double impactParameter = -1.0;
 
    // generate event 
  crmc_f_( m_iout, m_ievent ,nParticles, impactParameter, m_partID[0], m_partPx[0], m_partPy[0], m_partPz[0], 
	   m_partEnergy[0], m_partMass[0], m_partStat[0]  );




  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode Epos::genFinalize() 
{
  ATH_MSG_INFO("EPOS finalizing.");

  std::cout << "MetaData: generator = Epos " << std::endl;

    // retrieve information about the total cross-section from Epos
  double xsigtot, xsigine, xsigela, xsigdd, xsigsd, xsloela, xsigtotaa, xsigineaa, xsigelaaa;
  xsigtot = xsigine = xsigela = xsigdd = xsigsd = xsloela = xsigtotaa = xsigineaa = xsigelaaa = 0.0;
   
  crmc_xsection_f_(xsigtot, xsigine, xsigela, xsigdd, xsigsd, xsloela, xsigtotaa, xsigineaa, xsigelaaa);
  
  xsigtot *= 1000000;         // [mb] to [nb] conversion
  std::cout << "MetaData: cross-section (nb) = " << xsigtot << std::endl;
  xsigine *= 1000000;        //[mb] to [nb] conversion
  std::cout << "MetaData: cross-section inelastic (cut + projectile diffraction)[nb] = " << xsigine << std::endl;
     xsigela *= 1000000;         // [mb] to [nb] conversion
  std::cout << "MetaData: cross-section elastic (includes target diffraction)[nb] = " << xsigela << std::endl;
  xsigdd *= 1000000;         // [mb] to [nb] conversion
  std::cout << "MetaData: cross-section dd (nb) = " << xsigdd << std::endl;
  xsigsd *= 1000000;         // [mb] to [nb] conversion
  std::cout << "MetaData: cross-section sd (nb) = " << xsigsd << std::endl;

  //  m_eposEventInfo.close();

  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode Epos::fillEvt( HepMC::GenEvent* evt ) 
{
  //  ATH_MSG_INFO( " EPOS Filling.\n" );

    // debug printout


  HepMC::HEPEVT_Wrapper::set_event_number(m_events);
#ifdef HEPMC3
  HepMC::HEPEVT_Wrapper::HEPEVT_to_GenEvent(evt);
#else  
  HepMC::IO_HEPEVT hepio;

 
  hepio.set_trust_mothers_before_daughters(0);
  hepio.set_print_inconsistency_errors(0);
  hepio.fill_next_event(evt);
#endif
  // evt->print();
 

  HepMC::set_random_states(evt, m_seeds );

  evt->weights().push_back(1.0); 
  GeVToMeV(evt);
  
  std::vector<HepMC::GenParticlePtr> beams;

  for (auto p: *evt) {
    if (p->status() == 4) {
      beams.push_back(p);
   }
  }

  if (beams.size()>=2) {
  evt->set_beam_particles(beams[0], beams[1]); 
  }

  // Heavy Ion and Signal ID from Epos to HepMC

#ifdef HEPMC3
     HepMC::GenHeavyIonPtr ion= std::make_shared<HepMC::GenHeavyIon>();
                      ion->Ncoll_hard=cevt_.kohevt;
                      ion->Npart_proj=cevt_.npjevt;
                      ion->Npart_targ=cevt_.ntgevt;
                      ion->Ncoll=cevt_.kolevt;
                      ion->spectator_neutrons=cevt_.npnevt + cevt_.ntnevt;
                      ion->spectator_protons=cevt_.nppevt + cevt_.ntpevt;
                      ion->N_Nwounded_collisions=-1;
                      ion->Nwounded_N_collisions=-1;
                      ion->Nwounded_Nwounded_collisions=-1;
                      ion->impact_parameter= cevt_.bimevt;
                      ion->event_plane_angle=cevt_.phievt;
                      ion->eccentricity=-1;  //c2evt_.fglevt,  //correct name but not defined
                      ion->sigma_inel_NN=1e9*hadr5_.sigine;
#else
     HepMC::HeavyIon ion(cevt_.kohevt,
                      cevt_.npjevt,
                      cevt_.ntgevt,
                      cevt_.kolevt,
                      cevt_.npnevt + cevt_.ntnevt,
                      cevt_.nppevt + cevt_.ntpevt,
                      -1,
                      -1,
                      -1,
                      cevt_.bimevt,
                      cevt_.phievt,
                      -1,  //c2evt_.fglevt,  //correct name but not defined
                      1e9*hadr5_.sigine);
#endif

		      evt->set_heavy_ion(ion);  

  //an integer ID uniquely specifying the signal process (i.e. MSUB in Pythia)
  // Epos 1-ND, 2-DD, 3-CD, 4 AB->XB, -4 AB->AX translated into signal_proces_id 101-ND, 105 - DD, 102 - CD, 103 -AB->XB, 104 AB->AX
   int sig_id = -1;
   switch (int(c2evt_.typevt))
    {
    case  1: sig_id = 101; break;
    case -1: sig_id = 101; break;
    case  2: sig_id = 105; break;
    case -2: sig_id = 105; break;
    case  3: sig_id = 102; break;
    case -3: sig_id = 102; break;
    case  4: sig_id = 103; break;
    case -4: sig_id = 104; break;
    default: ATH_MSG_INFO( "Signal ID not recognised for setting HEPEVT \n");
    }

  HepMC::set_signal_process_id(evt,sig_id);


 return StatusCode::SUCCESS;
}

