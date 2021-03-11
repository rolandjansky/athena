/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// -------------------------------------------------------------
// Generators/Superchic_i.cxx Description: Allows the user
// to generate Superchic_v4.02 events and store the result in the
// Transient Store.
//
// AuthorList:
//    Prabhakar Palni, 29 August 2018 (v3.03)
//    Junichi Tanaka, 8 March 2021 (v4.02)
//
// Random seed set via jo
// Random numbers not saved by atlas engine mechanism event by event.

#include "Superchic_i/Superchic_i.h"

#include "GeneratorFortranCommon/Gen_HEPEVT.h"
#include "TruthUtils/GeneratorName.h"
#include "GaudiKernel/MsgStream.h"
#include "GeneratorUtils/StringParse.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "GeneratorUtils/StringParse.h"
#include <cstdlib>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/StoreGateSvc.h"

namespace{
  // Pointer On AtRndmGenSvc
  static IAtRndmGenSvc*  p_AtRndmGenSvc;
  static std::string     superchic_stream = "SUPERCHIC_INIT";
}

// store the HEPEVT common information from the hepevt output.
Atlas_HEPEVT_modified* Superchic_i::s_atlas_HEPEVT = new Atlas_HEPEVT_modified();

// call fortran subroutines to calculate parameters and then generate events
extern"C" {
  void calcparam_();
  void superchicrun_();
  void superchicrun_init_();
}

Superchic_i::Superchic_i(const std::string& name, ISvcLocator* pSvcLocator):
GenModule(name,pSvcLocator),
m_events(0),
m_configFileName(""),
m_rts("5.02d3"),
m_isurv(4),
m_intag("'in5'"),
m_PDFname("'MMHT2014lo68cl'"),
m_PDFmember(0),
m_proc(59),
m_beam("'prot'"),
m_outtg("'out'"),
m_sfaci(".false."),
m_diff("el"),
m_an("208d0"),
m_az("82d0"),
m_rz("6.68d0"),
m_dz("0.447d0"),
m_rn("6.7d0"),
m_dn("0.55d0"),
m_ionqcd("'coh'"),
m_ncall(10000),
m_itmx(10),
m_prec("1.0d0"),
m_ncall1(10000),
m_inccall(10000),
m_itend(1000),
m_iseed(17),
m_genunw(".true."),
m_nev(1),
m_erec("'hepevt'"),
m_readwt(".false."),
m_wtmax("0d0"),
m_ymin("-2.5d0"),
m_ymax("2.5d0"),
m_mmin("5d0"),
m_mmax("50d0"),
m_gencuts(".true."),
m_scorr(".true."),
m_fwidth(".true."),
m_ptxmax("100d0"),
m_acoabmax("10d0"),
m_ptamin("0d0"),
m_ptbmin("0d0"),
m_ptcmin("0d0"),
m_ptdmin("0d0"),
m_ptemin("0d0"),
m_ptfmin("0d0"),
m_etaamin("-2.5d0"),
m_etabmin("-2.5d0"),
m_etacmin("-2.5d0"),
m_etadmin("-2.5d0"),
m_etaemin("-2.5d0"),
m_etafmin("-2.5d0"),
m_etaamax("2.5d0"),
m_etabmax("2.5d0"),
m_etacmax("2.5d0"),
m_etadmax("2.5d0"),
m_etaemax("2.5d0"),
m_etafmax("2.5d0"),
m_rjet("0.6d0"),
m_jalg("'antikt'"),
m_m2b("0.133d0"),
m_pdgid1(211),
m_pdgid2(-211),
m_malp("5d-2"),
m_gax("1d-4"),
m_alpt("'ps'"),
m_mpol("500d0"),
m_mmon("933d0"),
m_gamm("10d0"),
m_mcharg("100d0"),
m_mneut("80d0")
{
  declareProperty("Initialize",	m_InitializeVector);
  declareProperty("ConfigFileName", m_configFileName);
}

Superchic_i::~Superchic_i()
{}

StatusCode Superchic_i::genInitialize()
{
  // Initialisation of input parameters
  //
  //ATH_MSG_INFO( "===> August 27 2019 SUPERCHIC INTERFACE VERSION. \n"   );
  ATH_MSG_INFO( "===> Feb 18 2021 SUPERCHIC INTERFACE VERSION. \n"   );
  ATH_MSG_INFO( "===> SUPERCHIC INITIALISING. \n"   );
  
  //  Initialize the random number generator stream
  static const bool CREATEIFNOTTHERE(true);
  ATH_CHECK( service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE) );
  
  // Save the SUPERCHIC_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(superchic_stream);
  const long* sip = engine->getSeeds();
  //m_randomSeed = sip[0];
  
  long int si1 = sip[0];
  long int si2 = sip[1];
  
  p_AtRndmGenSvc->CreateStream(si1,si2,superchic_stream);
  
  // set up to the use "SUPERCHIC" generation stream
  superchic_stream = "SUPERCHIC";
  
  m_events = 0;
  // set input parameters ....
  set_user_params();
  
  // read input parametrs from the input.DAT file and calculate the initial parameters
  calcparam_();
  
  // 
  superchicrun_init_();

  ATH_MSG_INFO( "\n=================================================\n"
                << "  Superchic  initialization results: \n"
                << "=================================================\n" );
  
  return StatusCode::SUCCESS;
}

StatusCode Superchic_i::callGenerator()
{
  ATH_MSG_DEBUG( " SUPERCHIC generating. \n"   );

  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(superchic_stream);
  const long* s	= engine->getSeeds();

  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);
  
  // Generate 1 event in hepevt format, event format and other parameters set in the input.DAT
  superchicrun_();
  
  ++m_events;

  ATH_MSG_INFO( " Superchic generating done.  \n"   );
  
  return StatusCode::SUCCESS;
}

StatusCode
Superchic_i::genFinalize()
{
  ATH_MSG_INFO( "  SUPERCHIC Ending.  \n"   );
  
  return StatusCode::SUCCESS;
}

StatusCode
Superchic_i::fillEvt(HepMC::GenEvent* evt)
{
  ATH_MSG_DEBUG( "  SUPERCHIC Filing.  \n"   );

  // get the event record information
  if (m_events <= 5) evt->print();

  // Set the event number
  evt->set_event_number( m_events );
  
  // Set the random generator seeds
  evt->set_random_states(m_seeds);
  
  // Set the generator process id .. 
  evt->set_signal_process_id(m_proc);
  
  // Create the event vertex
  HepMC::GenVertex* v1 = new HepMC::GenVertex();
  evt->add_vertex( v1 );
  
  // use the s_atlas_HEPEVT  ponter to obtain the information from the common block
  int npart = s_atlas_HEPEVT->nhep();
  
  for (int i=1; i<= npart; ++i) {
    // exclude incoming and outgoing beam particles
    //if (s_atlas_HEPEVT->isthep(i) == 1 && !((s_atlas_HEPEVT->idhep(i) == 2212) ||(s_atlas_HEPEVT->idhep(i) == 1000822080) )) {
      v1->add_particle_out(new HepMC::GenParticle(CLHEP::HepLorentzVector(s_atlas_HEPEVT->phep(1,i), s_atlas_HEPEVT->phep(2,i), s_atlas_HEPEVT->phep(3,i), s_atlas_HEPEVT->phep(4,i)), s_atlas_HEPEVT->idhep(i), s_atlas_HEPEVT->isthep(i)) );
      //}
  }
  
  // Convert cm->mm and GeV->MeV
  GeVToMeV(evt);

  return StatusCode::SUCCESS;
}

bool Superchic_i::set_user_params()
{
  // if external config file not specified
  if (m_configFileName.empty()) {
    m_configFileName = "input.DAT";
    if (!prepare_params_file()) {
      ATH_MSG_WARNING("problems initializing input parameters. cannot initialize superchic.");
      return false;
    }
  }
  
  return true;
}

bool Superchic_i::prepare_params_file()
{
  // Write initialization parameters to tmp file input.DAT
  
  for(CommandVector::iterator i = m_InitializeVector.begin(); i != m_InitializeVector.end(); i++ )
    {
      ATH_MSG_INFO( "  Command is: " << *i  );
      
      StringParse mystring(*i);
      std::string myparam = mystring.piece(1);
      if (myparam == "rts")
      {
        m_rts  = mystring.piece(2);
      }
      else if (myparam == "isurv")
      {
        m_isurv  = mystring.numpiece(2);
      }
      else if (myparam == "intag")
      {
        m_intag  = mystring.piece(2);
      }
      else if (myparam == "PDFname")
      {
        m_PDFname  = mystring.piece(2);
      }
      else if (myparam == "PDFmember")
      {
        m_PDFmember  = mystring.numpiece(2);
      }
      else if (myparam == "proc")
      {
        m_proc  = mystring.numpiece(2);
      }
      else if (myparam == "beam")
      {
        m_beam  = mystring.piece(2);
      }
      else if (myparam == "outtg")
      {
        m_outtg  = mystring.piece(2);
      }
      else if (myparam == "sfaci")
      {
        m_sfaci  = mystring.piece(2);
      }
      else if (myparam == "diff")
      {
        m_diff  = mystring.piece(2);
      }
      else if (myparam == "an")
      {
        m_an  = mystring.piece(2);
      }
      else if (myparam == "az")
      {
        m_az = mystring.piece(2);
      }
      else if (myparam == "rz")
      {
        m_rz = mystring.piece(2);
      }
      else if (myparam == "dz")
      {
        m_dz = mystring.piece(2);
      }
      else if (myparam == "rn")
      {
        m_rn = mystring.piece(2);
      }
      else if (myparam == "dn")
      {
        m_dn = mystring.piece(2);
      }
      else if (myparam == "ionqcd")
      {
        m_ionqcd = mystring.piece(2);
      }
      else if (myparam == "ncall")
      {
        m_ncall  = mystring.numpiece(2);
      }
      else if (myparam == "itmx")
      {
        m_itmx  = mystring.numpiece(2);
      }
      else if (myparam == "prec")
      {
        m_prec  = mystring.piece(2);
      }
      else if (myparam == "ncall1")
      {
        m_ncall1  = mystring.numpiece(2);
      }
      else if (myparam == "inccall")
      {
        m_inccall  = mystring.numpiece(2);
      }
      else if (myparam == "itend")
      {
        m_itend  = mystring.numpiece(2);
      }
      else if (myparam == "iseed")
      {
        m_iseed  = mystring.numpiece(2);
      }
      else if (myparam == "genunw")
      {
        m_genunw  = mystring.piece(2);
      }
      else if (myparam == "nev")
      {
        m_nev  = mystring.numpiece(2);
      }
      else if (myparam == "erec")
      {
        m_erec  = mystring.piece(2);
      }
      else if (myparam == "readwt")
      {
        m_readwt = mystring.piece(2);
      }
      else if (myparam == "wtmax")
      {
        m_wtmax = mystring.piece(2);
      }
      else if (myparam == "ymin")
      {
        m_ymin = mystring.piece(2);
      }
      else if (myparam == "ymax")
      {
        m_ymax = mystring.piece(2);
      }
      else if (myparam == "mmin")
      {
        m_mmin = mystring.piece(2);
      }
      else if (myparam == "mmax")
      {
        m_mmax = mystring.piece(2);
      }
      else if (myparam == "gencuts")
      {
        m_gencuts = mystring.piece(2);
      }
      else if (myparam == "scorr")
      {
        m_scorr = mystring.piece(2);
      }
      else if (myparam == "fwidth")
      {
        m_fwidth = mystring.piece(2);
      }
      else if (myparam == "ptxmax")
      {
        m_ptxmax = mystring.piece(2);
      }
      else if (myparam == "ptamin")
      {
        m_ptamin = mystring.piece(2);
      }
      else if (myparam == "ptbmin")
      {
        m_ptbmin = mystring.piece(2);
      }
      else if (myparam == "acoabmax")
      {
        m_acoabmax = mystring.piece(2);
      }
      else if (myparam == "ptcmin")
      {
        m_ptcmin = mystring.piece(2);
      }
      else if (myparam == "ptdmin")
      {
        m_ptdmin = mystring.piece(2);
      }
      else if (myparam == "ptemin")
      {
        m_ptemin = mystring.piece(2);
      }
      else if (myparam == "ptfmin")
      {
        m_ptfmin = mystring.piece(2);
      }
      else if (myparam == "etaamin")
      {
        m_etaamin = mystring.piece(2);
      }
      else if (myparam == "etabmin")
      {
        m_etabmin = mystring.piece(2);
      }
      else if (myparam == "etacmin")
      {
        m_etacmin = mystring.piece(2);
      }
      else if (myparam == "etadmin")
      {
        m_etadmin = mystring.piece(2);
      }
      else if (myparam == "etaemin")
      {
        m_etaemin = mystring.piece(2);
      }
      else if (myparam == "etafmin")
      {
        m_etafmin = mystring.piece(2);
      }
      else if (myparam == "etaamax")
      {
        m_etaamax = mystring.piece(2);
      }
      else if (myparam == "etabmax")
      {
        m_etabmax = mystring.piece(2);
      }
      else if (myparam == "etacmax")
      {
        m_etacmax = mystring.piece(2);
      }
      else if (myparam == "etadmax")
      {
        m_etadmax = mystring.piece(2);
      }
      else if (myparam == "etaemax")
      {
        m_etaemax = mystring.piece(2);
      }
      else if (myparam == "etafmax")
      {
        m_etafmax = mystring.piece(2);
      }
      else if (myparam == "rjet")
      {
        m_rjet = mystring.piece(2);
      }
      else if (myparam == "jalg")
      {
        m_jalg = mystring.piece(2);
      }
      else if (myparam == "m2b")
      {
        m_m2b = mystring.piece(2);
      }
      else if (myparam == "pdgid1")
      {
        m_pdgid1 = mystring.numpiece(2);
      }
      else if (myparam == "pdgid2")
      {
        m_pdgid2 = mystring.numpiece(2);
      }
      else if (myparam == "malp")
      {
        m_malp = mystring.piece(2);
      }
      else if (myparam == "gax")
      {
        m_gax = mystring.piece(2);
      }
      else if (myparam == "alpt")
      {
        m_alpt = mystring.piece(2);
      }
      else if (myparam == "mpol")
      {
        m_mpol = mystring.piece(2);
      }
      else if (myparam == "mmon")
      {
        m_mmon = mystring.piece(2);
      }
      else if (myparam == "gamm")
      {
        m_gamm = mystring.piece(2);
      }
      else if (myparam == "mcharg")
      {
        m_mcharg = mystring.piece(2);
      }
      else if (myparam == "mneut")
      {
        m_mneut = mystring.piece(2);
      }
      else
      {
        ATH_MSG_ERROR( " ERROR in Superchic4.02 INITIALIZATION PARAMETERS  "
                       << myparam << " is an invalid parameter !"  );
        return false;
      }
    }
  
  std::ofstream configFile;
  configFile.open(m_configFileName.c_str());
  
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "****** Initialize afain if FIRST FIVE PARAMETERS ARE CHANGED (and beam = 'prot'):******"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_rts        << "                     ! [rts] : CMS collision energy (GeV) " << std::endl;
  configFile << m_isurv      << "                         ! [isurv] : Model of soft survival (from 1 -> 4)" << std::endl;
  configFile << m_intag      << "                     ! [intag] for input files  " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_PDFname    << "                ! [PDFname] : PDF set " <<  std::endl;
  configFile << m_PDFmember  << "                       ! [PDFmember] : PDF member " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_proc       << "                             ! [proc] : Process number " << std::endl;
  configFile << m_beam       << "                           ! [beam] : Beam type ('prot', 'ion', 'ionp' or 'el') " << std::endl;
  configFile << m_outtg      << "                           ! [outtg] : for output file " << std::endl;
  configFile << m_sfaci      << "                          ! [sfaci] : Include soft survival effects " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_diff       << "                          ! [diff] : dissociation flag " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_an << "                                  ! [an] : Ion mass number " << std::endl;
  configFile << m_az << "                                   ! [az] : Ion atomic number " << std::endl;
  configFile << m_rz << "                                 ! [rz] : Ion proton density - radius "  << std::endl;
  configFile << m_dz << "                                ! [dz] : Ion proton density - skin thickness " << std::endl;
  configFile << m_rn << "                                  ! [rn] : Ion neutron density - radius " << std::endl;
  configFile << m_dn << "                                  ! [dn] : Ion neutron density - skin thickness " << std::endl;
  configFile << m_ionqcd << "                               ! [ionqcd] : Coherent ('coh') or incoherent ('incoh') for QCD-induced processes "  << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "*************Integration parameters************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_ncall << "                              ! [ncall] : Number of calls for preconditioning " << std::endl;
  configFile << m_itmx << "                                  ! [itmx] : Number of iterations for preconditioning " << std::endl;
  configFile << m_prec << "                                 ! [prec] :  Relative accuracy (in %) in main run " << std::endl;
  configFile << m_ncall1 << "                               ! [ncall1] : Number of calls in first iteration " << std::endl;
  configFile << m_inccall << "                              ! [inccall] : Number of increase calls per iteration "  << std::endl;
  configFile << m_itend << "                                ! [itend] : Maximum number of iterations " << std::endl;
  configFile << m_iseed << "                                 ! [iseed] : Random number seed (integer > 0) " << std::endl;
  //configFile << "***********************************************************************************"<< std::endl;
  //configFile <<  m_s2int << "                                ! [s2int] : Survival factor integration " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "********************Unweighted events**********************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_genunw << "                              ! [genunw] : Generate unweighted events " << std::endl;
  configFile << m_nev << "                                 ! [nev] : Number of events (1 or a small value is recommended) ( the number of produced total events are controlled by athena ) " << std::endl;
  configFile << m_erec << "                               ! [erec] : Event record format ('hepmc','lhe','hepevt') " << std::endl;
  configFile << m_readwt << "                             ! [readwt] : Set to true to read in pre-calculated maxium weight below " << std::endl;
  configFile << m_wtmax <<  "                               ! [wtmax] : Maximum weight "<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "*******************   general cuts ************************************************"<< std::endl;
  configFile << m_ymin << "                               ! [ymin] : Minimum object rapidity Y_X " << std::endl;
  configFile << m_ymax << "                               ! [ymax] : Maximum object rapidity Y_X "  << std::endl;
  configFile << m_mmin << "                               ! [mmin] : Minimum object mass M_X (redundant for resonance production) " << std::endl;
  configFile << m_mmax << "                               ! [mmax] : Maximum object mass M_X (redundant for resonance production) " << std::endl;
  configFile << m_gencuts << "                             ! [gencuts] : Generate cuts below " << std::endl;
  configFile << m_scorr << "                             ! [scorr] : Include spin correlations (for chi_c/b decays) " << std::endl;
  configFile << m_fwidth << "                             ! [fwidth] : Include finite width (for chi_c decays)  " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "************ See manual for momentum assignments***********************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "******************* Proton Cuts ***************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_ptxmax << "                               ! [ptxmax] : max pT of the system  " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "**********2 body final states : p(a) + p(b) ***************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile <<  m_ptamin << "                               ! [ptamin] "  << std::endl;
  configFile <<  m_ptbmin << "                               ! [ptbmin] "  << std::endl;
  configFile <<  m_etaamin << "                              ! [etaamin] " << std::endl;
  configFile <<  m_etaamax << "                              ! [etaamax] " << std::endl;
  configFile <<  m_etabmin << "                              ! [etabmin] " << std::endl;
  configFile <<  m_etabmax << "                              ! [etabmax] " << std::endl;
  configFile <<  m_acoabmax << "                             ! [acoabmax] " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "****** 3 body final states : p(a) + p(b) + p(c) ***********************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_ptamin << "                                ! [ptamin] " << std::endl;
  configFile << m_ptbmin  << "                               ! [ptbmin] " << std::endl;
  configFile << m_ptcmin << "                                ! [ptcmin] " << std::endl;
  configFile << m_etaamin << "                               ! [etaamin] " << std::endl;
  configFile << m_etaamax << "                               ! [etaamax] " << std::endl;
  configFile << m_etabmin  << "                              ! [etabmin] " << std::endl;
  configFile << m_etabmax << "                               ! [etabmax]	 "  << std::endl;
  configFile << m_etacmin << "                               ! [etacmin] " << std::endl;
  configFile << m_etacmax << "                               ! [etacmax]	 " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "****** 4 body final states : p(a) + p(b) + p(c) + p(d) ****************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_ptamin << "                                ! [ptamin] "  << std::endl;
  configFile << m_ptbmin << "                                ! [ptbmin] " << std::endl;
  configFile << m_ptcmin << "                                ! [ptcmin] " << std::endl;
  configFile << m_ptdmin << "                                ! [ptdmin] " << std::endl;
  configFile << m_etaamin << "                               ! [etaamin] " << std::endl;
  configFile << m_etaamax << "                               ! [etaamax] " << std::endl;
  configFile << m_etabmin << "                               ! [etabmin] " << std::endl;
  configFile << m_etabmax << "                               ! [etabmax] " << std::endl;
  configFile << m_etacmin << "                               ! [etacmin] " << std::endl;
  configFile << m_etacmax << "                               ! [etacmax] " << std::endl;
  configFile << m_etadmin << "                               ! [etacmin] " << std::endl;
  configFile << m_etadmax << "                               ! [etadmax] " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "****** 6 body final states : p(a) + p(b) + p(c) + p(d) + p(e) + p(f) **************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_ptamin << "                                ! [ptamin] " << std::endl;
  configFile << m_ptbmin << "                                ! [ptbmin] " << std::endl;
  configFile << m_ptcmin << "                                ! [ptcmin]  " << std::endl;
  configFile << m_ptdmin << "                                ! [ptdmin] " << std::endl;
  configFile << m_ptemin << "                                ! [ptemin] " << std::endl;
  configFile << m_ptfmin << "                                ! [ptfmin]  " << std::endl;
  configFile << m_etaamin << "                               ! [etaamin] " << std::endl;
  configFile << m_etaamax << "                               ! [etaamax] " << std::endl;
  configFile << m_etabmin << "                               ! [etabmin] " << std::endl;
  configFile << m_etabmax << "                               ! [etabmax]	" << std::endl;
  configFile << m_etacmin << "                               ! [etacmin] " << std::endl;
  configFile << m_etacmax << "                               ! [etacmax] " << std::endl;
  configFile << m_etadmin << "                               ! [etadmin]  " << std::endl;
  configFile << m_etadmax << "                               ! [etadmax]  " << std::endl;
  configFile << m_etaemin << "                               ! [etaemin]  " << std::endl;
  configFile << m_etaemax << "                               ! [etaemax] " << std::endl;
  configFile << m_etaemin << "                               ! [etafmin] " << std::endl;
  configFile << m_etafmax << "                               ! [etafmax]	" << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "******************* jet algorithm parameters **************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_rjet << "                                  ! [rjet] : Jet Radius " << std::endl;
  configFile << m_jalg << "                                  ! [jalg] : Jet algorithm ('antikt','kt') " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***** chi_c/b two-body decays *****************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_m2b << "                                   ! [m2b] : mass of decay particles " << std::endl;
  configFile << m_pdgid1 << "                                ! [pdgid1] : PDG number particle 1 " << std::endl;
  configFile << m_pdgid2 << "                                ! [pdgid2] : PDG number particle 2 "  << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "*******  ALP Parameters ***********************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_malp << "                                  ! [malp] : ALP mass (GeV) " << std::endl;
  configFile << m_gax << "                                   ! [gax] : ALP coupling (GeV^-1) " << std::endl;
  configFile << m_alpt << "                                  ! [alpt] : AlP type (scalar - 'sc', pseudoscalar - 'ps') "  << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "**** Monopole Parameters **********************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_mpol << "                                  ! [mpol] : Monopole mass " << std::endl;
  configFile << m_mmon << "                                  ! [mmon] : Monopolium mass " << std::endl;
  configFile << m_gamm << "                                  ! [gamm] : Monopolium width " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "****   SUSY Parameters ************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << m_mcharg << "                                ! [mcharg] : Chargino/Slepton mass " << std::endl;
  configFile << m_mneut  << "                                ! [mneut]  : Neutralino mass " << std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  configFile << "***********************************************************************************"<< std::endl;
  
  configFile.close();
  return true;
}
