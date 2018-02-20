/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// ---------------------------------------------------------------------- 
// Generators/QGSJet.cxx
//  
// Description: Allows the user to generate crmc (model qgsjet) events and store the result 
// in the Transient Store.
//
// AuthorList: 
//   Sami tyKama:       Initial code.
//   Sebastian Piec:  Adaptation for Epos 1.99.crmc.r2790.
// ---------------------------------------------------------------------- 

#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_GenEvent.h"

#include "QGSJet_i/QGSJet.h"
//#include "Epos_i/EposFort.h"

// #include "boost/format.hpp"


namespace{
  static std::string qgsjet_rndm_stream = "QGSJET_INIT";
  static IAtRndmGenSvc* p_AtRndmGenSvcQGSJet = 0;
}

extern "C" double atl_qgsjet_rndm_( int* ) 
{
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvcQGSJet->GetEngine(qgsjet_rndm_stream);
  return CLHEP::RandFlat::shoot(engine);
}

// ---------------------------------------------------------------------- 
// QGSJet Fortran bindings.
// ---------------------------------------------------------------------- 
extern "C"   
{
    // generator initialization
 void crmc_set_f_(int &nEvents,int &iSeed,double &beamMomentum, double &targetMomentum, int &primaryParticle, int &targetParticle, int &model, int &itab, int &ilheout, const char *paramFile);
  void crmc_init_f_();
  //  void crmc_init_f_( int &iSeed, double &beamMomentum, double &targetMomentum, int &primaryParticle, int &targetParticle, int &model, const char *paramFile );
    // event generation
  void crmc_f_( int &iout, int &ievent, int &nParticles, double &impactParam, int &partPdg, 
		double &partPx, double &partPy, double &partPz, double &partEnergy, double &partMass, int &outstat );
 
    // cross section info 
  void crmc_xsection_f_(double &xsigtot, double &xsigine, double &xsigela, double &xsigdd, 
      double &xsigsd, double &xsloela, double &xsigtotaa, double &xsigineaa, double &xsigelaaa);
} 
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
  } cevt_; //qgsjet.inc
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
  } c2evt_; //qgsjet.inc
}




using HepMC::IO_HEPEVT;

// ----------------------------------------------------------------------
QGSJet::QGSJet( const std::string &name, ISvcLocator *pSvcLocator ): 
  GenModule( name, pSvcLocator )
{
  qgsjet_rndm_stream = "QGSJET_INIT";
  
  declareProperty( "BeamMomentum",    m_beamMomentum    = -3500.0 );      // GeV
  declareProperty( "TargetMomentum",  m_targetMomentum  = 3500.0 );
  declareProperty( "Model",           m_model           = 7 );            // 0=EPOS 1.99 LHC, 1=EPOS 1.99, 7=QGSJETII04
  declareProperty( "PrimaryParticle", m_primaryParticle = 1 );            // 1=p, 12=C, 120=pi+, 207=Pb 
  declareProperty( "TargetParticle",  m_targetParticle  = 1 );
  declareProperty( "ParamFile",       m_paramFile       = "crmc.param" );
  declareProperty( "LheOutput",       m_ilheout       = 0 );
  declareProperty( "LheFile",         m_lheout       = "qgsjet.lhe" );
  declareProperty( "TabCreate",       m_itab       = 0 );
  declareProperty( "nEvents",         m_nEvents    = 5500 );
  
  m_events = 0; // current event number (counted by interface)
  m_ievent = 0;  // current event number counted by QGSJet
  m_iout = 0; // output type (output)

  // initialize internally used arrays
  for (size_t i = 0; i < kMaxParticles; ++i) {
    m_partID[i] = 0;
    m_partPx[i] = m_partPy[i] = m_partPz[i] = m_partEnergy[i] = m_partMass[i] = 0.0;
    m_partStat[i] = 0;
  }

}

// ----------------------------------------------------------------------
QGSJet::~QGSJet()
{}

// ----------------------------------------------------------------------
StatusCode QGSJet::genInitialize() 
{
  ATH_MSG_INFO( " CRMC INITIALISING.\n" );

  static const bool CREATEIFNOTTHERE = true;
  
  StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvcQGSJet, CREATEIFNOTTHERE);
  if ( !RndmStatus.isSuccess() || 0 == p_AtRndmGenSvcQGSJet ) {
    ATH_MSG_ERROR( " Could not initialize Random Number Service!" );
    return RndmStatus;
  }
   
  CLHEP::HepRandomEngine *engine = p_AtRndmGenSvcQGSJet->GetEngine(qgsjet_rndm_stream);
  const long *sip = engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  // eA

  std::cout << "eA seed: " << si1 << " " << si2 << std::endl; 

  int iSeed = si1%1000000000;     // FIXME ?

  // set up initial values

  std::cout << "parameters " << m_nEvents << " " << iSeed << " " << m_beamMomentum << " " << m_targetMomentum << " " << m_primaryParticle << " " << m_targetParticle << " " << m_model << " " << m_itab << " " << m_ilheout << " " <<  m_lheout.c_str()<< " " <<  m_paramFile.c_str() << std::endl;

  crmc_set_f_(m_nEvents, iSeed, m_beamMomentum, m_targetMomentum, m_primaryParticle, m_targetParticle, m_model, m_itab, m_ilheout,  m_paramFile.c_str() ); 

    // initialize QGSJet
  //  crmc_init_f_( iSeed, m_beamMomentum, m_targetMomentum, m_primaryParticle, m_targetParticle, m_model, m_paramFile.c_str() );
  crmc_init_f_();

    // ... and set them back to the stream for proper save
  p_AtRndmGenSvcQGSJet->CreateStream( si1, si2, qgsjet_rndm_stream );

  qgsjet_rndm_stream = "QGSJet";

    // setup HepMC
    HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof( int ));
    HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 );
    HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);    // as used in crmc-aaa.f!!!

  m_events = 0;

  //  m_ascii_out = new HepMC::IO_GenEvent(m_qgsjetEventInfo);
 
 return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode QGSJet::callGenerator() 
{
  // ATH_MSG_INFO( " QGSJet Generating." );

    // save the random number seeds in the event
    CLHEP::HepRandomEngine* engine = p_AtRndmGenSvcQGSJet->GetEngine( qgsjet_rndm_stream );
   const long *s = engine->getSeeds();

   std:: cout << "eA seed s : " << s[0] << " " << s[1] << std::endl;
  
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

//  std::cout << "events " << m_events << " " << m_ievent << std::endl;
  //  HepMC::HEPEVT_Wrapper::print_hepevt();

  /* for (int i=1;i<=50;++i){
   std::cout << "wrapper gen " << i <<  " " << HepMC::HEPEVT_Wrapper::number_entries() << " " << HepMC::HEPEVT_Wrapper::px(i)<<" " <<
     HepMC::HEPEVT_Wrapper::py(i) << " " << HepMC::HEPEVT_Wrapper::pz(i) << " " << HepMC::HEPEVT_Wrapper::e(i) << " " << HepMC::HEPEVT_Wrapper::m(i) << " " << HepMC::HEPEVT_Wrapper::id(i) << " " << HepMC::HEPEVT_Wrapper::status(i) << std::endl;
     }*/


    // debug printout
  /* std::cout << "parameters "<< m_iout << " " << m_ievent << " " << impactParameter << std::endl;  
 std::cout << "n particles " << nParticles << std::endl;
 for (int i=0; i<nParticles; i++){
   std::cout << "part " << i << " " << m_partID[i] << " " << m_partStat[i] << std::endl;
   std::cout << "part x " << m_partPx[i]<< " " << m_partPy[i] << " " << m_partPz[i] << " " << m_partEnergy[i] <<" " <<  m_partMass[i] << std::endl;
   }*/
 


  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode QGSJet::genFinalize() 
{
  using namespace std;
  ATH_MSG_INFO("QGSJet finalizing.");

  cout << "MetaData: generator = QGSJet " << endl;

    // retrieve information about the total cross-section from QGSJet
  double xsigtot, xsigine, xsigela, xsigdd, xsigsd, xsloela, xsigtotaa, xsigineaa, xsigelaaa;
  xsigtot = xsigine = xsigela = xsigdd = xsigsd = xsloela = xsigtotaa = xsigineaa = xsigelaaa = 0.0;
   
  crmc_xsection_f_(xsigtot, xsigine, xsigela, xsigdd, xsigsd, xsloela, xsigtotaa, xsigineaa, xsigelaaa);
  
  xsigtot *= 1000000;         // [mb] to [nb] conversion
  cout << "MetaData: cross-section (nb) = " << xsigtot << endl;
  xsigine *= 1000000;        //[mb] to [nb] conversion
  cout << "MetaData: cross-section inelastic (cut + projectile diffraction)[nb] = " << xsigine << endl;
     xsigela *= 1000000;         // [mb] to [nb] conversion
  cout << "MetaData: cross-section elastic (includes target diffraction)[nb] = " << xsigela << endl;
  xsigdd *= 1000000;         // [mb] to [nb] conversion
  cout << "MetaData: cross-section dd (nb) = " << xsigdd << endl;
  xsigsd *= 1000000;         // [mb] to [nb] conversion
  cout << "MetaData: cross-section sd (nb) = " << xsigsd << endl;

  //  m_qgsjetEventInfo.close();

  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------- 
StatusCode QGSJet::fillEvt( HepMC::GenEvent* evt ) 
{
  //  ATH_MSG_INFO( " QGSJet Filling.\n" );

    // debug printout


  HepMC::HEPEVT_Wrapper::set_event_number(m_events);
  HepMC::IO_HEPEVT hepio;

 
  hepio.set_trust_mothers_before_daughters(0);
  hepio.set_print_inconsistency_errors(0);
  hepio.fill_next_event(evt);
  // evt->print();
 
  evt->set_random_states( m_seeds );

  evt->weights().push_back(1.0); 
  GeVToMeV(evt);
  
  std::vector<HepMC::GenParticle*> beams;

  for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
    if ((*p)->status() == 4) {
      beams.push_back(*p);
    }
  } 

  evt->set_beam_particles(beams[0], beams[1]); 
  /*   std::cout << "beam particles" << std::endl;
  beams[0]->print();
  beams[1]->print();
  std::cout << "general print out " << std::endl;
    // debug printout
 for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p)
 {
   (*p)->print();
   }*/

  // Heavy Ion and Signal ID from QGSJet to HepMC

/*     HepMC::HeavyIon ion(cevt_.kohevt,
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

		      evt->set_heavy_ion(ion);  
   */
  //an integer ID uniquely specifying the signal process (i.e. MSUB in Pythia)
  // std::cout<<"obecny sig_proc_id " << evt->signal_process_id() << std::endl;
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

  evt->set_signal_process_id(sig_id);
   
  //ATH_MSG_INFO( "Event Information : \n");
  //std::cout << " signal_process_id "<<evt->signal_process_id() << std::endl;
  /* std::cout << " heavy ion " << evt->heavy_ion() << std::endl;
  evt->print();
  std::cout << " print-out done " << std::endl;*/

  //  HeavyIon* hi = evt.heavy_ion();
  // std::cout << " HI " << hi.spectator_protons() << std::endl;

  /*  boost::format fmt_e ("%1$-5.3e %2%\n");
  boost::format fmt_d ("%1$-5d %2%\n");

  std::string s;
  s = boost::str (fmt_d % "Event Parameters:" % "") +
	  boost::str (fmt_d % cevt_.nevt    % "... error code. 1=valid event, 0=invalid event") +
	  boost::str (fmt_e % cevt_.bimevt  % "... absolute value of impact parameter") +
	  boost::str (fmt_e % cevt_.phievt  % "... angle of impact parameter") +
	  boost::str (fmt_d % cevt_.kolevt  % "... number of collisions") +
	  boost::str (fmt_d % cevt_.koievt  % "... number of inelastic collisions") +
	  boost::str (fmt_e % cevt_.pmxevt  % "... reference momentum") +
	  boost::str (fmt_e % cevt_.egyevt  % "... pp cm energy (hadron) or string energy (lepton)") +
	  boost::str (fmt_d % cevt_.npjevt  % "... number of primary projectile participants") +
	  boost::str (fmt_d % cevt_.ntgevt  % "... number of primary target participants") +
	  boost::str (fmt_d % cevt_.npnevt  % "... number of primary projectile neutron spectators") +
	  boost::str (fmt_d % cevt_.nppevt  % "... number of primary projectile proton spectators") +
	  boost::str (fmt_d % cevt_.ntnevt  % "... number of primary target neutron spectators") +
	  boost::str (fmt_d % cevt_.ntpevt  % "... number of primary target proton spectators") +
	  boost::str (fmt_d % cevt_.jpnevt  % "... number of absolute projectile neutron spectators") +
	  boost::str (fmt_d % cevt_.jppevt  % "... number of absolute projectile proton spectators") +
	  boost::str (fmt_d % cevt_.jtnevt  % "... number of absolute target neutron spectators") +
	  boost::str (fmt_d % cevt_.jtpevt  % "... number of absolute target proton spectators" ) +
	  boost::str (fmt_e % cevt_.xbjevt  % "... bjorken x for dis") +
	  boost::str (fmt_e % cevt_.qsqevt  % "... q**2 for dis") +
	  boost::str (fmt_d % cevt_.nglevt  % "... number of collisions acc to  Glauber") +
	  boost::str (fmt_e % cevt_.zppevt  % "... average Z-parton-proj") +
	  boost::str (fmt_e % cevt_.zptevt  % "... average Z-parton-targ") +
	  boost::str (fmt_d % c2evt_.ng1evt % "... number of Glauber participants with at least one IAs") +
	  boost::str (fmt_d % c2evt_.ng2evt % "... number of Glauber participants with at least two IAs") +
	  boost::str (fmt_d % c2evt_.ikoevt % "... number of elementary parton-parton scatterings") +
	  boost::str (fmt_d % c2evt_.typevt % "... type of event (1=Non Diff, 2=Double Diff, 3=Single Diff");

  ATH_MSG_INFO(s);
  */
    
  //Make this output from HepMC 
  //  (*m_ascii_out) << evt; 

 return StatusCode::SUCCESS;
}

