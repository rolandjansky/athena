/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Generators/Hijing.cxx 
// Description: Allows the user to generate hijing events 
// and store the result in the Transient Store.
//
// AuthorList:
//    Alexandre (Sasha) Lebedev: Initial code, October 2018
//

#include "Ampt_h/Ampt.h"
#include "Ampt_h/VertexShift.h"
#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"

#include "GeneratorUtils/StringParse.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include <stdlib.h>

#include <sys/stat.h> 

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Point3D.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GeneratorObjects/HijingEventParams.h"

#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/HEPEVT_Wrapper.h"


namespace {
  static IAtRndmGenSvc *p_AtRndmGenSvc;
  static std::string ampt_stream = "AMPT_INIT";
}

// FORTRAN subroutines
extern "C"
{

  float atl_ran_( int* )
  {
    CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(ampt_stream);
    return (float) CLHEP::RandFlat::shoot(engine);
  }

  void artset_(void);

  void arini_(void);

  void arini2_(int*);

  void artan1_(void);

  void artmn_(void);

  void artan2_(void);

  void artout_(int*);

  void inizpc_(void);

  void hijset_(float*,
              const char*,
              const char*,
              const char*,
              int*,
              int*,
              int*,
              int*);

  void hijing_(const char*, float*, float*);

  void getnp_(void);

  int invflv_(int*);
}

//------------------------------------------------------------------

Ampt::Ampt(const std::string& name, ISvcLocator* pSvcLocator) : 
  GenModule(name,pSvcLocator)
{
    m_events     = 0; // event counter

  declareProperty("EFRM",   m_efrm = 2560.);
  declareProperty("Frame",  m_frame8 = "CMS     ");
  declareProperty("Proj",   m_proj8 = "A       ");
  declareProperty("Targ",   m_targ8 = "A       ");
  declareProperty("AProj",  m_iap   = 197);
  declareProperty("ZProj",  m_izp   = 79);
  declareProperty("ATarg",  m_iat   = 197);
  declareProperty("ZTarg",  m_izt   = 79);
  declareProperty("Bmin",   m_bmin  = 0.);
  declareProperty("Bmax",   m_bmax  = 14.6);

  declareProperty("writeAmpt", m_writeAmpt=false);
  declareProperty("writeHepMC", m_writeHepMC=false);
  declareProperty("DumpHijing", m_DumpHijing=false);

  // event vertex randomization
  declareProperty("randomizeVertices", m_rand = false); // Randomize for beam gas
  declareProperty("selectVertex", m_sel = false); // Select vertex location (off for random)
  declareProperty("wide", m_wide = false); // Allow randoms off the beamline (out to the pipe)
  declareProperty("Xvtx", m_x = 0.);
  declareProperty("Yvtx", m_y = 0.);
  declareProperty("Zvtx", m_z = 0.);

    m_frame      = m_frame8.substr(0,4);
    m_proj       = m_proj8.substr(0,4);
    m_targ       = m_targ8.substr(0,4);
    declareProperty("stringSwitch",  m_isoft      = 4); // select Default AMPT (1) or String Melting (4)
    m_ntmax      = 150; //  number of timesteps (D=150), see below
    m_dt         = 0.2; //  timestep in fm (hadron cascade time = DT*NTMAX) (D=0.2)
    m_parj41     = 0.55; // parameter a in Lund symmetric splitting function
    m_parj42     = 0.15; // parameter b in Lund symmetric splitting function
    m_ipop       = 1; //(D=1,yes;0,no) flag for popcorn mechanism(netbaryon stopping)
    m_parj5      = 1.0; // to control BMBbar vs BBbar in popcorn (D=1.0)
    m_ihpr2_6    = 1; // shadowing flag in Hijing (Default=1,yes; 0,no)
    m_ihpr2_4    = 0; // quenching flag in Hijing (D=0,no; 1,yes)
    m_hipr1_14   = 2.0; // quenching rate -dE/dx (GeV/fm) in case quenching flag=1
    m_hipr1_8    = 2.0; // p0 cutoff in HIJING for minijet productions (D=2.0)
    m_xmu        = 2.265; // parton screening mass in fm^(-1) (D=2.265d0), see below
    m_izpc       = 0; // IZPC: (D=0 forward-angle parton scatterings; 100,isotropic)
    m_alpha      = 0.33; // alpha in parton cascade (D=0.33d0), see parton screening mass
    m_dpcoal     = 1.0e+06; // dpcoal in GeV
    m_drcoal     = 1.0e+06; // drcoal in fm
    m_ihjsed     = 0; // take HIJING seed from m_hjseed (D=0)or at runtime(11)
    //m_nseed      = 13150909; // ! random seed for HIJING
    m_iseedp     = 8;  // random seed for parton cascade
      declareProperty("decayKs", m_iksdcy = 0); // flag for K0s weak decays (D=0,no; 1,yes)
      declareProperty("decayPhi", m_iphidcy = 1); // flag for phi decays at end of hadron cascade (D=1,yes; 0,no)
      declareProperty("decayPi0", m_ipi0dcy = 0); // flag for pi0 decays at end of hadron cascade (D=0,no; 1,yes)
    m_ioscar     = 0;  //  optional OSCAR output (D=0,no; 1,yes; 2&3,more parton info)
    m_idpert     = 0; // flag for perturbative deuteron calculation (D=0,no; 1or2,yes)
    m_npertd     = 1; // integer factor for perturbative deuterons(>=1 & <=10000)
    m_idxsec     = 1; // choice of cross section assumptions for deuteron reactions
    m_pttrig     = 7.; // Pt in GeV: generate events with >=1 minijet above this value
    m_maxmiss    = 1000; // maxmiss (D=1000): maximum # of tries to repeat a HIJING event
    m_ihpr2_2    = 3; // flag to turn off initial and final state radiation (D=3)
    m_ihpr2_5    = 1; //  flag to turn off Kt kick (D=1)
    m_iembed     = 0; // flag to turn on quark pair embedding (D=0,no; 1,yes)
    m_pxqembd    = 7.; // Initial Px value (GeV) of the embedded quark (u or d)
    m_pyqembd    = 0.; // Initial Py value (GeV) of the embedded quark (u or d)
    m_xembd      = 0.; // Initial x value (fm) of the embedded back-to-back q/qbar
    m_yembd      = 0.; // Initial y value (fm) of the embedded back-to-back q/qbar
    m_nsembd     = 1; // nsembd(D=0 ???) to embed a back-to-back q/qbar pair into each event
    m_psembd     = 5.; // psembd (in GeV) to embed a back-to-back q/qbar pair into each event
    m_tmaxembd   = 0.; // tmaxembd (in radian) to embed a back-to-back q/qbar pair into each event
      declareProperty("iShadow", m_ishadow = 0); // Flag to enable users to modify nuclear shadowing (D=0,no; 1,yes)
      declareProperty("dShadow", m_dshadow = 1.0); // Factor used to modify nuclear shadowing
      declareProperty("iPhiRP", m_iphirp = 0); // Flag for random orientation of reaction plane (D=0,no; 1,yes)

}

//------------------------------------------------------------------

Ampt::~Ampt()
{}

//------------------------------------------------------------------

StatusCode Ampt::genInitialize()
{
  ATH_MSG_INFO( "===> Ampt initialising...  \n" );

  ATH_MSG_INFO("   FRAME = " << m_frame << " " << m_frame8 << endreq );
  ATH_MSG_INFO("   PROJECTILE = " << m_proj << " " << m_proj8 << " " << " " << m_iap << " " << m_izp << endreq );
  ATH_MSG_INFO("   TARGET = " << m_targ << " " << m_targ8 << " " << m_iat << " " << m_izt << endreq );
  ATH_MSG_INFO("   EFRM = " << m_efrm << endreq );
  ATH_MSG_INFO("   Bmin = " << m_bmin << endreq );
  ATH_MSG_INFO("   Bmax = " << m_bmax << endreq );

  if(m_writeHepMC) {
    std::string m_hepmcoutput_file = "ampt_hepmc.dat";
    m_hepmcio.reset( new HepMC::IO_GenEvent(m_hepmcoutput_file.c_str(), std::ios::out) );
  }

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to get pointer to StoreGate Service" );
      return sc;
    }

    static const bool CREATEIFNOTTHERE(true);
    StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
    if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc) {
      ATH_MSG_ERROR( " Could not initialize Random Number Service" );
      return RndmStatus;
    }   

    // Get the AMPT_INIT stream seeds....
    CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(ampt_stream);
    const long*       sip       =       engine->getSeeds();
    long       int       si1       =       sip[0];
    long       int       si2       =       sip[1];

    // ... and set them back to the stream for proper save
    p_AtRndmGenSvc->CreateStream(si1, si2, ampt_stream);

    ATH_MSG_DEBUG( " Initial Random Numbers: " << sip << " " << si1 << " " << si2 << endreq );
 
    // set up COMMON blocks
    m_anim.isoft() = m_isoft;
    m_input2.ntmax() = m_ntmax;
    m_input1.dt() = m_dt;
    m_ludat1.parj(41) = m_parj41;
    m_ludat1.parj(42) = m_parj42;
    m_ludat1.parj(5) = m_parj5;
    if(m_ipop==1) { m_hiparnt.ihpr2(11) = 3; }
    m_hiparnt.ihnt2(6) = m_ihpr2_6;
    m_hiparnt.ihnt2(4) = m_ihpr2_4;
    m_hiparnt.hipr1(14) = m_hipr1_14;
    m_hiparnt.hipr1(8) = m_hipr1_8;
    m_para2.xmu() = m_xmu;
    m_anim.izpc() = m_izpc;
    m_para2.alpha() = m_alpha;
    m_coal.dpcoal() = m_dpcoal;
    m_coal.drcoal() = m_drcoal;
    // m_ihjsed
    //m_rndf77.nseed() = m_nseed; // Fixed random seed for HIJING
    m_rndf77.nseed() = si2;       // Get initial random seed from AMPT_INIT stream in joboptions file
    m_rndm3.iseedp() = m_iseedp;
    m_resdcy.iksdcy() = m_iksdcy;
    m_phidcy.iphidcy() = m_iphidcy;
    m_phidcy.ipi0dcy() = m_ipi0dcy;
    m_para7.ioscar() = m_ioscar;
    m_para8.idpert() = m_idpert;
    m_para8.npertd() = m_npertd;
    m_para8.idxsec() = m_idxsec;
    m_phidcy.pttrig() = m_pttrig;
    m_phidcy.maxmiss() = m_maxmiss;
    m_hiparnt.ihnt2(2) = m_ihpr2_2;
    m_hiparnt.ihnt2(5) = m_ihpr2_5;
    m_embed.iembed() = m_iembed;
    m_embed.nsembd() = m_nsembd;
    m_embed.pxqembd() = m_pxqembd;
    m_embed.pyqembd() = m_pyqembd;
    m_embed.xembd() = m_xembd;
    m_embed.yembd() = m_yembd;
    m_embed.psembd() = m_psembd;
    m_embed.tmaxembd() = m_tmaxembd;
    m_cmsflag.ishadow() = m_ishadow;
    m_cmsflag.dshadow() = m_dshadow;
    m_phihj.iphirp() = m_iphirp;
   
    std::string amptvn;
    if(m_isoft==1)      { amptvn = "  1.26t7 (Default)    "; }
    else if(m_isoft==4) { amptvn = "2.26t7 (StringMelting)"; }
    else                { amptvn = "   Test-Only          "; }
    ATH_MSG_INFO(" ##################################################" << endreq );
    ATH_MSG_INFO(" #      AMPT (A Multi-Phase Transport) model      #" << endreq );
    ATH_MSG_INFO(" #         Version " << amptvn << "         #"       << endreq );
    ATH_MSG_INFO(" #               10/28/2016                       #" << endreq );
    ATH_MSG_INFO(" ##################################################" << endreq );

// turn on warning messages in nohup.out when an event is repeated:
    m_hiparnt.ihpr2(10) = 1;  

// string formation time:
    m_arprnt.arpar1(1) = 0.7;

// smearp is the smearing halfwidth on parton z0, 
// set to 0 for now to avoid overflow in eta.
// smearh is the smearing halfwidth on string production point z0.
    m_smearz.smearp() = 0.0;
    float IAmax = std::max(m_iap,m_iat);
    m_smearz.smearh() = 1.2*pow(IAmax,0.3333)/(m_efrm/2./0.938);

  // If requested, write AMPT output in original txt format
  if(m_writeAmpt) {
    // check if "ana" output directory exists
    // if not, create and open output files
    struct stat st;
    if(stat("ana",&st) == 0) {
      if(st.st_mode & (S_IFDIR != 0)) {
        ATH_MSG_INFO(" ana exists and is a directory.");
      }
    } else {
      const int dir_err = mkdir("ana", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (-1 == dir_err) {
        ATH_MSG_ERROR("Error creating output ana directory!");
        exit(1);
      }
    }
    outfile1.open("ana/ampt.dat");
    outfile2.open("ana/zpc.dat");
  }

    ATH_MSG_INFO(" Calling ARTSET... ");
    artset_();

    const char* frame       = m_frame.c_str();
    const char* proj        = m_proj.c_str();
    const char* targ        = m_targ.c_str();
    ATH_MSG_INFO("   frame, proj, targ = " << frame << " " << proj << " " << targ << endreq );

    ATH_MSG_INFO(" Calling HIJSET... ");
    hijset_(&m_efrm, frame, proj, targ, &m_iap, &m_izp, &m_iat, &m_izt);

    ATH_MSG_INFO( "\n=================================================\n"
               << "  HIJING initialization results: \n"  
               << "    Total sigma     = " << m_hiparnt.hint1(13) << " mb\n"
               << "    Inelastic sigma = " << m_hiparnt.hint1(12) << " mb\n"
               << "    Jet sigma       = " << m_hiparnt.hint1(11) << " mb\n"
               << "===================================================\n" );

    ATH_MSG_INFO(" Calling INIZPC... ");
    inizpc_();

    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------

StatusCode Ampt::callGenerator()
{
  ATH_MSG_DEBUG( " Ampt generating event # " << m_events << endreq );

  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(ampt_stream);
  const long* s = engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);


  ATH_MSG_DEBUG(" Calling HIJING... ");
    const char* frame = m_frame8.c_str();
      hijing_(frame, &m_bmin, &m_bmax);

// The number of particles in the hijing output
  m_arprnt.iaint2(1) = m_himain1.natt();

// evaluate Npart (from primary NN collisions) for both proj and targ:
  ATH_MSG_DEBUG(" Calling GETNP... " << endreq );
    getnp_();

// switch for final parton fragmentation
  if(m_hiparnt.ihpr2(20) != 0 ) {

    ATH_MSG_DEBUG(" Calling ARTINI... " << endreq );
      arini_();
        int K=1; arini2_(&K);

    ATH_MSG_DEBUG(" Calling ARTAN... " << endreq );
      artan1_();
        artmn_();
          artan2_();
  }

  ++m_events;

  int np = m_himain1.np();
  int nt = m_himain1.nt();
  int n0 = m_himain1.n0();
  int n01 = m_himain1.n01();
  int n10 = m_himain1.n10();
  int n11 = m_himain1.n11();
  int natt = m_himain1.natt();
  int jatt = m_himain1.jatt();
  float b = m_hiparnt.hint1(19);
  float bphi = m_hiparnt.hint1(20);
 
  m_event_params =
      new HijingEventParams(np, nt, n0, n01, n10, n11, natt, jatt, b, bphi);

  StatusCode sc =
    m_storeGate->record(m_event_params,"Hijing_event_params");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not record Hijing event params" );
      return  StatusCode::FAILURE;
    }

  ATH_MSG_DEBUG( " Ampt generating done.  \n" );
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------

StatusCode Ampt::genFinalize()
{

// write out diagnostic "histograms"
  if(m_writeAmpt) { artout_(&m_events); }

// close output text files
  outfile1.close();
  outfile2.close();

    ATH_MSG_INFO( "===> Ampt Ending.  \n" );
    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------

StatusCode Ampt::fillEvt(HepMC::GenEvent* evt)
{
  if(m_DumpHijing) { return fillEvtHijing(evt); }
    else { return fillEvtAmpt(evt); }
}

//------------------------------------------------------------------

StatusCode Ampt::fillEvtHijing(HepMC::GenEvent* evt)
{
  ATH_MSG_INFO( " HIJING Filing...  \n" );

  evt->set_event_number( m_events );

  evt->set_random_states(m_seeds);

  evt->set_signal_process_id(HIJING + m_iap);

  int np = m_himain1.np();
  int nt = m_himain1.nt();
  int n0 = m_himain1.n0();
  int n01 = m_himain1.n01();
  int n10 = m_himain1.n10();
  int n11 = m_himain1.n11();
  int jatt = m_himain1.jatt();
  float b = m_hiparnt.hint1(19);
  float bphi = m_hiparnt.hint1(20);

  float sigmainel =  m_hiparnt.hint1(12);

  HepMC::HeavyIon ion
      (
       static_cast<int>(jatt), // Ncoll_hard
       static_cast<int>(np),   // Npart_proj
       static_cast<int>(nt),   // Npart_targ 
       static_cast<int>(n0+n10+n01+n11), // Ncoll
       static_cast<int>(-1),   // spectator_neutrons
       static_cast<int>(-1),   // spectator_protons
       static_cast<int>(n01),  // N_Nwounded_collisions
       static_cast<int>(n10),  // Nwounded_N_collisions
       static_cast<int>(n11),  // Nwounded_Nwounded_collisions
       b,                      // impact_parameter
       bphi,                   // event_plane_angle
       -1,                     // eccentricity
       sigmainel     );        // sigma_inel_NN

  evt->set_heavy_ion(ion);

  std::vector<HepMC::GenVertex*> vertexPtrVec;

  CLHEP::HepLorentzVector newVertex;
  newVertex = CLHEP::HepLorentzVector(0.,0.,0.,0.);

  if( m_rand ) { newVertex = randomizeVertex(); } // Create a random vertex along the pipe
  else if(m_sel) { newVertex = CLHEP::HepLorentzVector(m_x, m_y, m_z, 0.); } // Create vertex at selected point - preempted by m_rand

  HepMC::GenVertex* v1 = new HepMC::GenVertex(newVertex);

  evt->set_signal_process_vertex(v1);
  vertexPtrVec.push_back(v1);

  double eproj = (double) m_efrm;
    if ( m_frame == "CMS     " ) eproj = eproj / 2.;
  int proj_id = 2212;
    if (  m_proj == "PBAR    " ) { proj_id = -2212; }
    else if (  m_proj == "N       " ) { proj_id = 2112; }
    else if (  m_proj == "NBAR    " ) { proj_id = -2112; }
    else if (  m_proj == "PI+     " ) { proj_id = 211; }
    else if (  m_proj == "PI-     " ) { proj_id = -211; }
    else if (  m_proj == "A       " ) { proj_id = 3000000 + m_iap; }
    HepMC::GenParticle* part_p = new HepMC::GenParticle( CLHEP::HepLorentzVector(0., 0., eproj, eproj), proj_id, 101 );
    v1->add_particle_in( part_p );

  double etarg = 0.;
    if ( m_frame == "CMS     " ) etarg = ( (double) m_efrm ) / 2.;
  int       targ_id = 2212;
    if ( m_targ == "PBAR    " ) { targ_id = -2212; }
    else if (  m_targ == "N       " ) { targ_id = 2112; }
    else if (  m_targ == "NBAR    " ) { targ_id = -2112; }
    else if (  m_targ == "PI+     " ) { targ_id = 211; }
    else if (  m_targ == "PI-     " ) { targ_id = -211; }
    else if (  m_targ == "A       " ) { targ_id = 3000000 + m_iat; }
    HepMC::GenParticle* part_t = new HepMC::GenParticle( CLHEP::HepLorentzVector(0., 0., -etarg, etarg), targ_id, 102 );
    v1->add_particle_in( part_t );

   evt->set_beam_particles(part_p,part_t);

//  The number of particles produced by Hijing
    int numHijingPart = m_himain1.natt();
    int particleVertexIndex = 0;

  for (int i = 1; i <= numHijingPart; ++i) {

    int id = m_himain2.katt(i,1);
    float px = m_himain2.patt(i,1);
    float py = m_himain2.patt(i,2);
    float pz = m_himain2.patt(i,3);
    float pt = sqrt(px*px+py*py);
    float mass = m_himain2.patt(i,4);
    float ee = sqrt(mass*mass+pt*pt+pz*pz);
      int partStat = 1;
      CLHEP::HepLorentzVector partP4(px, py, pz, ee);
      HepMC::GenParticle* newParticle_p = new HepMC::GenParticle(partP4, id, partStat);

    CLHEP::HepLorentzVector particleStart(0.,0.,0.,0.);
      HepMC::GenVertex* newVertex_p = new HepMC::GenVertex(particleStart);
          vertexPtrVec.push_back(newVertex_p);
            particleVertexIndex = vertexPtrVec.size() - 1;

        vertexPtrVec[particleVertexIndex]->add_particle_out(newParticle_p);

  } // end loop over Hijing particles

  for (unsigned int iv = 0; iv < vertexPtrVec.size(); iv++) { evt->add_vertex(vertexPtrVec[iv]); }

  GeVToMeV(evt);

  ATH_MSG_INFO( " HIJING Filing Done.  \n" );
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------

StatusCode Ampt::fillEvtAmpt(HepMC::GenEvent* evt)
{
  ATH_MSG_INFO( " AMPT Filing...  \n" );

  evt->set_event_number( m_events );

  evt->set_random_states(m_seeds);

  evt->set_signal_process_id(HIJING + m_iap);

  int np = m_himain1.np();
  int nt = m_himain1.nt();
  int n0 = m_himain1.n0();
  int n01 = m_himain1.n01();
  int n10 = m_himain1.n10();
  int n11 = m_himain1.n11();
  int jatt = m_himain1.jatt();
  float b = m_hiparnt.hint1(19);
  float bphi = m_hiparnt.hint1(20);

  float sigmainel =  m_hiparnt.hint1(12);

  HepMC::HeavyIon ion
      (			
       static_cast<int>(jatt), // Ncoll_hard
       static_cast<int>(np),   // Npart_proj
       static_cast<int>(nt),   // Npart_targ 
       static_cast<int>(n0+n10+n01+n11), // Ncoll
       static_cast<int>(-1),   // spectator_neutrons
       static_cast<int>(-1),   // spectator_protons
       static_cast<int>(n01),  // N_Nwounded_collisions
       static_cast<int>(n10),  // Nwounded_N_collisions
       static_cast<int>(n11),  // Nwounded_Nwounded_collisions
       b,                      // impact_parameter
       bphi,                   // event_plane_angle
       -1,                     // eccentricity
       sigmainel     );        // sigma_inel_NN

  evt->set_heavy_ion(ion); 

// Vector that will keep pointers to generated vertices
  std::vector<HepMC::GenVertex*> vertexPtrVec;

  CLHEP::HepLorentzVector newVertex;
  newVertex = CLHEP::HepLorentzVector(0.,0.,0.,0.);  

  if( m_rand ) { newVertex = randomizeVertex(); } // Create a random vertex along the pipe
  else if(m_sel) { newVertex = CLHEP::HepLorentzVector(m_x, m_y, m_z, 0.); } // Create vertex at selected point - preempted by m_rand

  HepMC::GenVertex* v1 = new HepMC::GenVertex(newVertex);

  evt->set_signal_process_vertex(v1);
  vertexPtrVec.push_back(v1);

  double eproj = (double) m_efrm;
    if ( m_frame == "CMS     " ) eproj = eproj / 2.;
  int proj_id = 2212;
    if (  m_proj == "PBAR    " ) { proj_id = -2212; } 
    else if (  m_proj == "N       " ) { proj_id = 2112; } 
    else if (  m_proj == "NBAR    " ) { proj_id = -2112; } 
    else if (  m_proj == "PI+     " ) { proj_id = 211; } 
    else if (  m_proj == "PI-     " ) { proj_id = -211; } 
    else if (  m_proj == "A       " ) { proj_id = 3000000 + m_iap; }
    HepMC::GenParticle* part_p = new HepMC::GenParticle( CLHEP::HepLorentzVector(0., 0., eproj, eproj), proj_id, 101 );
    v1->add_particle_in( part_p );

  double etarg = 0.;
    if ( m_frame == "CMS     " ) etarg = ( (double) m_efrm ) / 2.;
  int       targ_id = 2212;
    if ( m_targ == "PBAR    " ) { targ_id = -2212; } 
    else if (  m_targ == "N       " ) { targ_id = 2112; } 
    else if (  m_targ == "NBAR    " ) { targ_id = -2112; } 
    else if (  m_targ == "PI+     " ) { targ_id = 211; } 
    else if (  m_targ == "PI-     " ) { targ_id = -211; } 
    else if (  m_targ == "A       " ) { targ_id = 3000000 + m_iat; }
    HepMC::GenParticle* part_t = new HepMC::GenParticle( CLHEP::HepLorentzVector(0., 0., -etarg, etarg), targ_id, 102 );
    v1->add_particle_in( part_t );

   evt->set_beam_particles(part_p,part_t);

//  The number of particles produced by Hijing
  int numHijingPart = m_himain1.natt();
//  The number of particles in the Ampt output
  int numAmptPart = m_hbt.nlast();

  ATH_MSG_DEBUG( " Number of particles HIJING/AMPT = " << numHijingPart << " " << numAmptPart << endreq );

  int particleVertexIndex = 0;

// loop over all particles in event
  int numGoodAmptPart = 0;
  for (int i = 1; i <= numAmptPart; ++i) {

    int id = m_hbt.lblast(i);
    int myid = invflv_(&id);
    float px = m_hbt.plast(1,i);
    float py = m_hbt.plast(2,i);
    float pz = m_hbt.plast(3,i);
    float pt = sqrt(px*px+py*py);
    float mass = m_hbt.plast(4,i);
    float ee = sqrt(mass*mass+pt*pt+pz*pz);
      CLHEP::HepLorentzVector partP4(px, py, pz, ee);

    if((myid==2212 || myid==2112) && px==0. && py==0.) continue; // skip beam particles

    if(abs(myid)==423 || abs(myid)==513 || abs(myid)==42 || abs(myid)==413 || abs(myid)==433) {
       continue; // skip "bad" pdg id which crash reconstruction
    }

    numGoodAmptPart++;

    if( m_rand || m_sel ){ // Shift the particle vertex if it is not at zero
      m_hbt.xlast(1,i) += newVertex(0);
      m_hbt.xlast(2,i) += newVertex(1);
      m_hbt.xlast(3,i) += newVertex(2);
      m_hbt.xlast(4,i) += newVertex(3);
    }

    float vx = m_hbt.xlast(1,i);
    float vy = m_hbt.xlast(2,i);
    float vz = m_hbt.xlast(3,i);
    if(fabs(vx)>999. || fabs(vy)>999. || fabs(vz)>999.) continue; // remove particles outside GEANT World volume

// particle origin vertex
    CLHEP::HepLorentzVector particleStart(m_hbt.xlast(1,i),m_hbt.xlast(2,i),m_hbt.xlast(3,i),m_hbt.xlast(4,i));
      HepMC::GenVertex* newVertex_p = new HepMC::GenVertex(particleStart);
          vertexPtrVec.push_back(newVertex_p);
            particleVertexIndex = vertexPtrVec.size() - 1;

// the particle itself
      int partId = myid;
      int partStat = 1;
      HepMC::GenParticle* newParticle_p = new HepMC::GenParticle(partP4, partId, partStat);

// add particle to its vertex
        vertexPtrVec[particleVertexIndex]->add_particle_out(newParticle_p);

  } // end loop over all particles

// Now loop over all vertices and add them to the event
  for (unsigned int iv = 0; iv < vertexPtrVec.size(); iv++) { evt->add_vertex(vertexPtrVec[iv]); }

// Convert CLHEP::cm->CLHEP::mm and CLHEP::GeV->CLHEP::MeV
   GeVToMeV(evt);
//  evt->print();

  if(m_writeHepMC) { m_hepmcio->write_event(evt); }

  ATH_MSG_INFO( " AMPT Filing Done.  \n" );
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------

CLHEP::HepLorentzVector Ampt::randomizeVertex()
{
  // Check the range in Z for the correct pipe diameter
  // Definitions of constants are in VertexShift.h

  using namespace VertexShift;

  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(ampt_stream);
  
  double ranx, rany, xmax, ymax;
  double ranz = CLHEP::RandFlat::shoot(engine, -Zmax, Zmax);
  if( m_wide ){ // Allow the whole pipe
    if( fabs(ranz) < Start1 ) {
      xmax = Xmin + Delta1;
      ymax = xmax;
    } else if( fabs(ranz) < Start2 ) {
      xmax = Xmin + Delta2;
      ymax = xmax;
    } else if( fabs(ranz) < Start3 ) {
      xmax = Xmin + Delta3;
      ymax = xmax;
    } else if ( fabs(ranz) <= Envelope ){
      xmax = Xmin;
      ymax = xmax;
    } else {    
      ATH_MSG_ERROR( "**** Ampt::randomizeVertex()  " << ranz << " (z) is outside the detector (units of mm). Returning a centered event." );
      return CLHEP::HepLorentzVector(0.,0.,0.,0.);
    }
  } else {
    ATH_MSG_INFO( "New Coordinates: x=0., y=0., z= " << ranz       );
    return CLHEP::HepLorentzVector(0., 0., ranz, 0); // Allow distribution just along the beam (no "width", m_wide is false (default))
  }
  ranx = CLHEP::RandFlat::shoot(engine, -xmax, xmax);
  rany = CLHEP::RandFlat::shoot(engine, -ymax, ymax);
  
  ATH_MSG_INFO( "New Coordinates: x = " << ranx << ", y = " << rany << ", z = " << ranz );
  
  return CLHEP::HepLorentzVector(ranx, rany, ranz, 0);
}





