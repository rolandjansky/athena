/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File:Herwig.cxx
// Generators/Herwig.cxx Description: Allows the user
// to generate Herwig events and store the result in the
// Transient Store.
//
// AuthorList:
// Ian Hinchliffe:  Initial Code October : 2000
// Modeled after the CDF code by Marge Shapriro and Jeremy Lys
// Modified for Version 6.4 Jan 2002-- Ian Hinchliffe
// AcerMC 1.x interface added by Borut Paul Kersevan (February 2003)
// TAUOLA/PHOTOS interface added by Borut Paul Kersevan (October 2003)
// Jimmy merged in by Jon Butterworth (Feb 2007)
// Removed lots, but not all, of the irrelevant CDF stuff (JMB, Nov 2008)

#include "Herwig_i/Herwig.h"
#include "TruthUtils/GeneratorName.h"
#include "GeneratorUtils/StringParse.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"

#include "Herwig_i/herwig6520.h"
#include "Charybdis_i/CharybdisInterface.h"

#include "HepMC/IO_HERWIG.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "Herwig_i/Lhefinfo.h"
#include "Herwig_i/wgtacp.h"

#include <cstdlib>

using std::string;


#define evtcon evtcon_
extern struct EvtconStruct {
  int istg;
} evtcon;


// AtRndmGenSvc setup
namespace {
  static IAtRndmGenSvc* p_AtRndmGenSvc;
  static string herwig_stream = "HERWIG_INIT";
}
// Provide Fortran-accessible RNG engines
extern "C" double atl_hwrgen_( int* /*idummy*/ )
{
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(herwig_stream);
  return CLHEP::RandFlat::shoot(engine);
}
extern "C" double atl_phoran_( int* /*idummy*/ )
{
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine("PHOTOS");
  return CLHEP::RandFlat::shoot(engine);
}
extern "C" float atl_ranmar_()
{
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine("TAUOLA");
  return (float) CLHEP::RandFlat::shoot(engine);
}


// Declare C++-visible common block init function wrapper
void InitHerwigCommonBlocks();


extern "C" {

  // In Herwig_i
  void herwiginterface_(int* itopd);
  void cdfreadsusy_(const char* ,int * , int);

  // Structure functions in Herwig
  void hwsfun_(const double *, const double *, const int *, const int *, const double *, const int *);

  //  In Herwig
  // SUSY
  void hwissp_(int *syunit);

  // Added for RPV 4-body decays (Sebastian Fleischmann/JMB)
  void hwirpv_();
  // NB: if the RPV code is not linked, a dummy version is called.

  void hwuinc_();
  // void hwusta(int *tstab);
  // void hwabeg_();
  void hweini_();
  void hwuine_();
  void hwupro_();
  void hwepro_();
  void hwbgen_();
  void hwdhob_();
  void hwcfor_();
  void hwcdec_();
  void hwdhad_();
  void hwdhvy_();
  void hwmevt_();
  void hwufne_();
  void hwefin_();
  void hwanal_(int *ihwcod);
  void hwmsct_(int *abort);
  void hwgpdg_(int *mconv);
  void extproc_(int*);

  void upveto_(int*);

  void jminit_();
  void jmefin_();

  // Charybdis fix
  void charybdfix_();

  // PDF information
  void hwpdfinfo_();

  // Alpgen termination
  void alsfin_();

}


////////////////////////////////////////////////////////


Atlas_HEPEVT* Herwig::atlas_HEPEVT = new Atlas_HEPEVT();


//--------------------------------------------------------------------------


Herwig::Herwig(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator)
{
  // printing-out manually set x-section for the MetaData
  m_Default_SetCrossSection=-1.0;
  m_Default_xsscale=-99999;
  //properties
  declareProperty("HerwigCommand", m_herwigCommandVector);
  declareProperty("CrossSectionScaleFactor", m_xsscale=m_Default_xsscale);
  declareProperty("SetCrossSection",m_SetCrossSection=m_Default_SetCrossSection);
  declareProperty("UpdateHadronStatCode", m_updateHadronStatCode=true);

  m_noshower_Parm = false;
  m_nohadroniz_Parm = false;
  m_itopd = 0;
  m_ndetry = 0;
  m_ExternalProcess = 0;
}


//--------------------------------------------------------------------------


Herwig::~Herwig() { }


//-------------------------------------------------------------


AcerMC_acset& Herwig::acermc_acset() {
  return m_acermc_acset;
}


//---------------------------------------------------------------------------


// Initialise the listing output, parameter and decay data input streams
StatusCode Herwig::genInitialize() {
  ATH_MSG_INFO("HERWIG INITIALISING.\n");

  // Set up RNG service
  /// @todo Use ServiceHandle + property method
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc) {
    ATH_MSG_ERROR("Could not initialize Random Number Service");
    return RndmStatus;
  }

  // Default behaviour is to shower and hadronize
  m_noshower_Parm = false;
  m_nohadroniz_Parm = false;

  // Map Herwig common blocks to global C structures
  HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof(int));
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);
  InitHerwigCommonBlocks();

  // Default the top decay, beams and beam energies
  gHwproc->pbeam1 = 7000.;  // beam energies
  gHwproc->pbeam2 = 7000.;  // beam energies
  m_itopd = 0;
  memcpy(gHwbmch->part1, "P       ", 8);
  memcpy(gHwbmch->part2, "P       ", 8);
  // ALL other settings should be given after the call to hwigin (in herwiginterface)

  // Special loop over the input parameters to load iproc, beam type and beam energy
  // before the call to herwiginterface_()
  /// @todo Use a more normal loop!
  CommandVector::const_iterator ic = m_herwigCommandVector.begin();
  do {
    StringParse mystring(*ic);
    string myvar = mystring.piece(1);
    int myint1 = mystring.intpiece(2);
    double  myfl1 = mystring.numpiece(2);
    string myvar2 = mystring.piece(2);
    myvar2.resize(7, ' ');
    if(myvar=="iproc") {
      gHwproc->iproc = myint1;
    }
    else if(myvar=="beam1energy") {
      gHwproc->pbeam1 = myfl1;
    }
    else if(myvar=="beam2energy") {
      gHwproc->pbeam2 = myfl1;
    }
    else if(myvar=="topdec") {
      m_itopd = myint1;
    }
    else if(myvar=="beam1type") {
      strncpy(gHwbmch->part1,myvar2.c_str(),7);
   }
    else if(myvar=="beam2type") {
      strncpy(gHwbmch->part2,myvar2.c_str(),7);
    }
    ic++;
  } while (ic != m_herwigCommandVector.end());


  // Call Fortran routine to initialize Herwig common block
  herwiginterface_(&m_itopd);

  // Set timeout and maximum number of errors to sensibe defaults
  gHwevnt->maxer = 10000000;
  gHwevnt->tlout = -10000000.0;

  m_ExternalProcess = 0;

  // No hardcoded defaults here!!
  //
  // Anything different from generator default should
  // be set in job options.

  // Now read in all the parameter changes from the user
  for (size_t i = 0; i < m_herwigCommandVector.size(); i++) {
    ATH_MSG_INFO("Command is '" << m_herwigCommandVector[i] << "'");
    StringParse mystring(m_herwigCommandVector[i]);
    string myvar = mystring.piece(1);
    string myvar1 = mystring.piece(2);
    int myint1 = mystring.intpiece(2);
    int myint2 = mystring.intpiece(3);
    double myfl1 = mystring.numpiece(2);
    double myfl2 = mystring.numpiece(3);
    if (myvar== "beam1type" || myvar== "beam2type" ||
        myvar== "beam1energy" || myvar== "beam2energy") {
      // Beam types and energy already done.
    }
    // Process number for external processes set here.
    else if (myvar=="iproc") {
      if (myvar1 == "charybdis") {
        gHwproc->iproc = -200;
        m_ExternalProcess = CHARYBDIS;
      } else if (myvar1 == "acermc") {
        gHwproc->iproc = -600;
        m_ExternalProcess = ACERMC;
      } else if(myvar1 == "alpgen") {
        gHwproc->iproc = -1400;
        m_ExternalProcess = ALPGEN;
        if (myint2 > 0) {
          gHwproc->iproc -= myint2;
        }
      } else if (myvar1 == "madgraph") {
        gHwproc->iproc = -950;
        m_ExternalProcess = MADGRAPH;
      } else if (myvar1 == "madcup") {
        gHwproc->iproc = -800;
        m_ExternalProcess = MADCUP;
      } else if (myvar1 == "lhaext") {
        gHwproc->iproc = -900;
        m_ExternalProcess = LHAEXT;
      } else if (myvar1 == "mcatnlo") {
        gHwproc->iproc = -700;
        m_ExternalProcess = MCATNLO;
      } else if (myvar1 == "lhef") {
        gHwproc->iproc = -100;
        m_ExternalProcess = LHEF;
      } else if (myvar1 == "horace") {
        gHwproc->iproc = -300;
        m_ExternalProcess = HORACE;
      } else {
        gHwproc->iproc=myint1;
      }
      extproc_(&m_ExternalProcess);
    }
    // AcerMC tt~ decay mode switching
    else if (myvar=="acset12") {
      if (gHwproc->iproc == -600) {
        this->acermc_acset().acset12() = myint1;
        ATH_MSG_INFO("mamy incjalizacje acera" << myint1);
      }
    }

    // ------- HWHARD common block
    // Kinematic limits
    else if (myvar == "ptmin") { gHwhard->ptmin = myfl1; }
    else if (myvar == "ptmax") { gHwhard->ptmax = myfl1; }
    else if (myvar == "emmin") { gHwhard->emmin = myfl1; }
    else if (myvar == "emmax") { gHwhard->emmax = myfl1; }
    else if (myvar == "q2min") { gHwhard->q2min = myfl1; }
    else if (myvar == "ptpow") { gHwhard->ptpow = myfl1;}

    // ------- HWDIST common block
    else if (myvar == "pltcut") { gHwdist->pltcut = myfl1; }
    else if (myvar == "mixing") { gHwdist->mixing = myint1; }

    // -------  HWPRAM common block
    else if (myvar == "ispac") {
      gHwpram->ispac = myint1;
    } else if (myvar == "qspac") {
      gHwpram->qspac = myfl1;
    }
    else if (myvar=="azsoft") {
      gHwpram->azsoft = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->azsoft ? "true" : "false"));
    }
    else if (myvar == "azspin") {
      gHwpram->azspin = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->azspin ? "true" : "false"));
    }
    else if (myvar == "nospac") {
      gHwpram->nospac = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->nospac ? "true" : "false"));
    }
    else if (myvar == "hardme") {
      gHwpram->hardme = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->hardme ? "true" : "false"));
    }
    else if (myvar == "softme") {
      gHwpram->softme = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->softme ? "true" : "false"));
    }

    // weak parameters and widths
    else if (myvar == "ncolo") { gHwpram->ncolo = myint1; }
    else if (myvar == "nflav") { gHwpram->nflav = myint1; }
    else if (myvar == "swein") { gHwpram->swein = myfl1; }
    else if (myvar == "scabi") { gHwpram->scabi = myfl1; }

    // print options
    else if (myvar == "prvtx") { gHwpram->prvtx = myint1; }
    else if (myvar == "prndef"){ gHwpram->prndef = myint1; }
    else if (myvar == "prntex"){ gHwpram->prntex = myint1; }
    else if (myvar == "prnweb"){ gHwpram->prnweb = myint1; }
    else if (myvar == "nprfmt"){ gHwpram->nprfmt = myint1; }
    else if (myvar == "iprint"){ gHwpram->iprint = myint1; }

    // Widths
    else if (myvar == "gamh") { gHwpram->gamh = myfl1; }
    else if (myvar == "gamw") { gHwpram->gamw = myfl1; }
    else if (myvar == "gamz") { gHwpram->gamz = myfl1; }
    else if (myvar == "gamzp") { gHwpram->gamzp = myfl1; } // Z'
    else if (myvar == "zprime") {
      gHwpram->zprime = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwpram->zprime ? "true" : "false"));
    }

    else if (myvar == "effmin") {
      gHwpram->effmin = myfl1;
    }
    else if (myvar == "psplt") {
      gHwpram->psplt[0] = myfl1;
      gHwpram->psplt[1] = myfl2;
    }
    else if (myvar == "cldir") {
      gHwpram->cldir[0] = myint1;
      gHwpram->cldir[1] = myint2;
    }
    else if (myvar == "clsmr") {
      gHwpram->clsmr[0] = myfl1;
      gHwpram->clsmr[1] = myfl2;
    }
    else if (myvar == "ensof") { gHwpram->ensof = myfl1;}
    else if (myvar == "qcdlam"){ gHwpram->qcdlam = myfl1;}
    else if (myvar == "clpow") { gHwpram->clpow = myfl1;}
    else if (myvar == "prsof") { gHwpram->prsof = myfl1;}
    else if (myvar == "ptrms") { gHwpram->ptrms = myfl1;}
    else if (myvar == "vgcut") { gHwpram->vgcut = myfl1;}
    else if (myvar == "vqcut") { gHwpram->vqcut = myfl1;}
    else if (myvar == "vpcut") { gHwpram->vpcut = myfl1;}
    // Allow user to set different PDF for each beam (this might be needed for
    // non-LHC runs or photoproduction)
    else if (myvar == "modpdf1") { gHwpram->modpdf[0] = myint1; }
    else if (myvar == "modpdf2") { gHwpram->modpdf[1] = myint1; }
    // If beam id not specified, assume both are the same PDF.
    else if (myvar == "modpdf") {
      gHwpram->modpdf[0] = myint1;
      gHwpram->modpdf[1] = myint1;
    }

    // HWPRCH common block
    // allow user to use PDFs from different libraries for each beam
    else if (myvar == "autpdf1") {
      string autlocal = myvar1;
      int nch = (autlocal.size()<20) ? autlocal.size() : 19;
      for (int i=0; i<20; i++) {
        if(i < nch) {
          gHwprch->autpdf[0][i] = autlocal.data()[i];
        } else {
          gHwprch->autpdf[0][i] = ' ';
        }
      }
    }
    else if (myvar == "autpdf2") {
      string autlocal = myvar1;
      int nch = (autlocal.size()<20) ? autlocal.size() : 19;
      for (int i=0; i<20; i++) {
        if(i < nch) {
          gHwprch->autpdf[1][i] = autlocal.data()[i];
        } else {
          gHwprch->autpdf[1][i] = ' ';
        }
      }
    }
    else if (myvar == "autpdf") {
      string autlocal = myvar1;
      int nch = (autlocal.size()<20) ? autlocal.size() : 19;
      for (int i=0; i<20; i++) {
        if(i < nch) {
          gHwprch->autpdf[0][i] = autlocal.data()[i];
          gHwprch->autpdf[1][i] = autlocal.data()[i];
        } else {
          gHwprch->autpdf[0][i] = ' ';
          gHwprch->autpdf[1][i] = ' ';
        }
      }
    }

    // output control and random numbers
    else if (myvar == "maxer") { gHwevnt->maxer = myint1; }
    else if (myvar == "maxpr") { gHwevnt->maxpr = myint1; }

    else if (myvar == "nrn") {
      if (myint1 == 1) {
        gHwevnt->nrn[0] = myint2;
      } else if (myint1 == 2) {
        gHwevnt->nrn[1] = myint2;
      }
    }
    else if (myvar == "nowgt") {
      /// @todo Why handled differently from other '=false' cases?
      //if (myvar1 == "false") { gHwevnt->nowgt = false; }
      gHwevnt->nowgt = (myvar1 != "false");
      ATH_MSG_INFO(" " << myvar << " is set to " << (gHwevnt->nowgt ? "true" : "false"));
    }
    else if (myvar == "wgtmax") { gHwevnt->wgtmax = myfl1; }
    else if (myvar == "tlout") { gHwevnt->tlout = myfl1; }

    // Particle masses
    else if (myvar == "rmass") {
      if (myint1 > 0 && myint1 < 501) {
        gHwprop->rmass[myint1] = myfl2;
      }
    }

    // Particle stability
    else if (myvar == "rstab") {
      if (myint1 > 0 && myint1 < 501) {
        gHwupdt->rstab[myint1] = myint2;
      }
    }

    // Boson decay modes
    else if (myvar == "modbos") {
      if(myint1 > 0 && myint1 < 10) {
        gHwbosc->modbos[myint1-1] = myint2;
      }
    }

    // SUSY filename
    else if (myvar == "susyfile") {
      m_read_Filesusy = myvar1;
    }

    // Graviton stuff
    else if (myvar == "grvlam") { gHwgrav->grvlam = myfl1; }
    else if (myvar == "emgrv")  { gHwgrav ->emgrv = myfl1; }
    else if (myvar == "gamgrv") { gHwgrav->gamgrv = myfl1; }

    // version 6.506 stuff
    else if (myvar == "pdfx0")  { gHw6506->pdfx0  = myfl1; }
    else if (myvar == "pdfpow") { gHw6506->pdfpow = myfl1; }

    // version 6.510 stuff
    else if (myvar == "ndetry") { m_ndetry = myint1; }

    // version 6.520 stuff
    else if (myvar == "trunsh") {
      gHw6520->trunsh = (myvar1 != "false");
      //ATH_MSG_INFO(" " << myvar << " is set to " << (gHw6520->trunsh ? "true" : "false"));
    }

    // Tauola switch
    else if (myvar == "taudec") {
      int nch = (myvar1.size() < 7) ? myvar1.size() : 6;
      for (int i = 0; i < 6; i++) {
        if(i < nch) {
          gHwdspn->taudec[i] = myvar1.data()[i];
        }
      }
    }

    // Spin correlations menu
    else if (myvar=="syspin") { gHwdspn->syspin = myint1; }
    else if (myvar=="spcopt") { gHwspin->spcopt = myint1; }

    // Jimmy common blocks
    else if (myvar == "msflag") { gJmparm->msflag = myint1; }
    else if (myvar == "jmbug") { gJmparm->jmbug = myint1; }
    else if (myvar == "ptjim") { gJmparm->ptjim = (double) myfl1; }
    else if (myvar == "jmueo") { gJmparm->jmueo=myint1;}
    else if (myvar == "jmrad") {
      if (myint1 > 0 && myint1 < 264) {
        gJmparm->jmrad[myint1-1] = myfl2;
      } else {
        ATH_MSG_ERROR("JMRAD ARRAY HAS ONLY 264 elements [1 to 264]");
      }
    }

    // Charybdis parameters - added by nick brett - 04-02-05
    else if (myvar == "charyb") { ::WriteCharybdisParam(myint1, myint2, (double)myfl2); }

    // Colour singlet exchange parameters - added by James Monk 25.Apr.2008
    else if (myvar == "asfixd") { gHwhard->asfixd = myfl1; }
    else if (myvar == "omega0") { gHwhard->omega0 = myfl1; }
    else {
      if (myvar != "topdec") {
        ATH_MSG_WARNING("ERROR in HERWIG PARAMETERS: " << myvar << " is not a known variable name. "
                        << "It could be a typo on your part or a mistake in the Herwig_i code. "
                        << "NB. Not all variables are changeable");
      }
    }
  }

  // Save the HERWIG_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(herwig_stream);
  const long* sip =	engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  // Special settings for Alpgen
  if (m_ExternalProcess == ALPGEN) gHwevnt->maxpr = 4;

  // Special settings for SUSY
  if ((gHwproc->iproc >= 3000 && gHwproc->iproc < 5000) ||
      (gHwproc->iproc >= 13000 && gHwproc->iproc < 15000)) {
    int syunit = 66;
    const string& fileName = m_read_Filesusy;
    cdfreadsusy_(fileName.c_str(),&syunit,fileName.size());
    hwissp_(&syunit);
  }

  // Added for RPV 4-body decays (Sebastian Fleischmann/JMB)
  // NB: if the RPV code is not linked, a dummy version is called.
  hwirpv_();
   ATH_MSG_INFO("mamy dalej");

  hwuinc_(); // calculate herwig constants
 ATH_MSG_INFO("mamy dalej stale");
  hweini_(); // initialise herwig event.
   ATH_MSG_INFO("mamy incjalizacje zd. herwiga");
  jminit_(); // initialise jimmy event.
 ATH_MSG_INFO("mamy incjalizacje jimmy");

  // ... and set them back to the stream for proper save
  p_AtRndmGenSvc->CreateStream(si1, si2, herwig_stream);
  herwig_stream = "HERWIG";

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Herwig::callGenerator() {
  ATH_MSG_DEBUG("HERWIG generating");

  bool goodev = false;
  int abort = 1;

  // Save the random number seeds in the event
  CLHEP::HepRandomEngine* engine = p_AtRndmGenSvc->GetEngine(herwig_stream);
  const long* s = engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

  while (!goodev && abort == 1) {
    hwuine_(); // initialize event
    if (m_ExternalProcess == ALPGEN) {
      ATH_MSG_DEBUG("HERWIG/ALPGEN");
      hwepro_();
      // Store info for PDFInfo
      hwpdfinfo_();

      // Matching-driven veto
      int ipveto = 0;
      upveto_(&ipveto);
      if (ipveto != 0) {
        gHwevnt->ierror = -1;
        ATH_MSG_INFO("EVENT KILLED BY UPVETO. EXECUTION CONTINUES");
      }

      if (evtcon.istg <= 0) {
        hwbgen_();  // Generate parton cascade
        // Do Jimmy underlying event if required.
        abort = 0;
        if (gJmparm->msflag == 1) {
          hwmsct_(&abort);
        }
        if (abort == 0) {
          hwdhob_();  // Do heavy quark decays
          hwcfor_();  // Do cluster formation
          hwcdec_();  // Do cluster decays
          hwdhad_();  // Do unstable particle decays
          hwdhvy_();  // Do heavy flavor decays
          hwmevt_();  // Add soft underlying event
        }
      }
    } else {
      hwepro_();  // Generate hard process
      // Store info for PDFInfo
      hwpdfinfo_();

      if (gHwproc->iproc == -200 ) {
        charybdfix_(); //reset hard cms code to BH=40
      }
      if (evtcon.istg <= 0) {
	abort = 0;
        if (!m_noshower_Parm) {
          hwbgen_();  // Generate parton cascade
          if (gJmparm->msflag == 1) {
            hwmsct_(&abort);
          }
          if (abort == 0) {
            hwdhob_();  // Do heavy quark decays
            if (!m_nohadroniz_Parm) {
              hwcfor_();  // Do cluster formation
              hwcdec_();  // Do cluster decays
              hwdhad_();  // Do unstable particle decays
              hwdhvy_();  // Do heavy flavor decays
              hwmevt_();  // Add soft underlying event
            }
          }
        }
      }
    }

    // User event analysis if wanted
    if (evtcon.istg > 0) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << "\n"
                << "!! " << generator_name(m_ExternalProcess) << " TERMINATES NORMALLY: NO MORE EVENTS IN FILE !!!" << "\n"
                << "!! PLEASE IGNORE ANY ATHENA ERROR MESSAGES LIKE								!!" << "\n"
                << "!! AthenaEventLoopMgr  ERROR Terminating event processing loop due to errors !!" << "\n"
                << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      return StatusCode::FAILURE;
    }

    if (abort == 0) {
      hwufne_();  // Finish event
      int ihwcod = 1;
      hwanal_(&ihwcod);
      if (gHwevnt->ierror == 0) {
        goodev = true;
      } else { //modified by T.Sasaki
        goodev = false;
        abort = 1; //for loop again
      }
    }

  }

  ATH_MSG_DEBUG("HERWIG generating done.");
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Herwig::genFinalize() {
  ATH_MSG_INFO("HERWIG ending.");

  // Don't call this again for AlpGen
  if (m_ExternalProcess != ALPGEN) {
    ATH_MSG_DEBUG("Call HWEFIN and HWAEND at endRun");
    hwefin_();
  } else {
    if (evtcon.istg <= 0) {
      alsfin_(); // when finished gracefully
    }
  }
  if (gJmparm->msflag == 1) {
    jmefin_();
  }

  // User terminal calculations if wanted
  if (m_SetCrossSection==m_Default_SetCrossSection)
    {
      //always print out generator's version
      std::cout << "MetaData: generator = Herwig " << HWVERSION << std::endl;
      //always print out the non-scaled x-section
      std::cout << "MetaData: cross-section (nb)= " << gHwevnt->avwgt << std::endl;

      //print out additional MetaData fields if cross-section scaling is requested
      if (m_xsscale != m_Default_xsscale)
	{
	  std::cout << "MetaData: cross-section*CrossSectionScaleFactor (nb)= " << gHwevnt->avwgt * m_xsscale << std::endl;
	  std::cout << "MetaData: CrossSectionScaleFactor = " << m_xsscale << std::endl;
	}
    }
  else
    {
      std::cout << "Printing cross-section passed by SetCrossSection in JO: "<< std::endl;
      std::cout << "MetaData: cross-section (nb)= " << m_SetCrossSection << std::endl;
    }


  // hwaend_();

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Herwig::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG("HERWIG Atlas_HEPEVT filling.");
  store_Atlas_HEPEVT();

  ATH_MSG_DEBUG("HERWIG filling.");
  // Fill event into HepMC and transient store
  HepMC::IO_HERWIG hepio;
  hepio.fill_next_event(evt);
  int pr_id = HERWIG + m_ExternalProcess + gHwproc->iproc;
  if (gHwproc->iproc < 0) pr_id = HERWIG + m_ExternalProcess;
  string taustring;
  taustring.assign(gHwdspn->taudec,6);
  if (taustring == "TAUOLA") pr_id += TAUOLA_PHOTOS;
  evt->set_signal_process_id(pr_id);
  evt->set_random_states(m_seeds);

  // Set beam particle status = 4
  if (evt->valid_beam_particles()) {
    evt->beam_particles().first->set_status(4);
    evt->beam_particles().second->set_status(4);
  } else {
    ATH_MSG_WARNING("NO VALID BEAM PARTICLES");
  }

  // Set PDF info
  const int id1 = gHrwpdf->id1;
  const int id2 = gHrwpdf->id2;
  const double x1 = gHrwpdf->x1;
  const double x2 = gHrwpdf->x2;
  const double q = gHrwpdf->q;
  // PDF id(s)
  const int idPdf1 = gHrwpdf->idpdf1;
  const int idPdf2 = gHrwpdf->idpdf2;
  // Store x*Pdf(id,x,Q) in PdfInfo structure instead of Pdf ID(s)

  size_t idx1;
  size_t idx2;


  if(id1==21)
    idx1=12;
  else if(id1 >= 1 && id1 <= 6)
    idx1=id1-1;
  else if(id1 <= -1 && id1 >= -6)
    idx1=5-id1;
  else
    idx1=100;

  if(id2==21)
    idx2=25;
  else if(id2 >= 1 && id2 <= 6)
    idx2=id2+12;
  else if(id2 <= -1 && id2 >= -6)
    idx2=18-id2;
  else
    idx2=100;

  double  xpdf1 = -999.,
    xpdf2 = -999.;

  int ld1=1;
  int ld2=2;
  int idh1=gHwevnt->idhw[ ld1-1 ];
  int idh2=gHwevnt->idhw[ ld2-1 ];
  int nset=0;
  double m_disf[26];

  if ( idx1 > 25 || idx2 > 25 ) {
    ATH_MSG_WARNING( "PDF index out of range!" );
  } else {

    hwsfun_(&x1, &q, &idh1, &nset, &(m_disf[0]), &ld1);
    hwsfun_(&x2, &q, &idh2, &nset, &(m_disf[13]), &ld2);

    xpdf1 = m_disf[ idx1 ];
    xpdf2 = m_disf[ idx2 ];
  }


  if ( m_ExternalProcess == LHEF ) {
      // for external (lhe) generators read pdf information from LHEFINFO common block (filled by Lhef_i)
    Lhefinfo linfo;

    ATH_MSG_DEBUG("lhefinfo, id1: "   << linfo.id1());
    ATH_MSG_DEBUG("lhefinfo, id2: "   << linfo.id2());
    ATH_MSG_DEBUG("lhefinfo, x1: "    << linfo.x1());
    ATH_MSG_DEBUG("lhefinfo, x2: "    << linfo.x2());
    ATH_MSG_DEBUG("lhefinfo, scale: " << linfo.scalePdf());
    ATH_MSG_DEBUG("lhefinfo, xpdf1: " << linfo.xPdf1());
    ATH_MSG_DEBUG("lhefinfo, xpdf2: " << linfo.xPdf2());

    HepMC::PdfInfo tmp_pdi( linfo.id1(), linfo.id2(), linfo.x1(), linfo.x2(),
        linfo.scalePdf(), linfo.xPdf1(), linfo.xPdf2() );
    evt->set_pdf_info(tmp_pdi);

  } else {
    HepMC::PdfInfo tmp_pdi( id1, id2, x1, x2, q, xpdf1, xpdf2, idPdf1, idPdf2 );
    evt->set_pdf_info( tmp_pdi );
  }

  ATH_MSG_DEBUG("pdfinfo: "
                << evt->pdf_info()->id1() << ", "
                << evt->pdf_info()->id2() << ", "
                << evt->pdf_info()->x1() << ", "
                << evt->pdf_info()->x2() << ", "
                << evt->pdf_info()->scalePDF() << ", "
                << evt->pdf_info()->pdf1() << ", "
                << evt->pdf_info()->pdf2() << ", "
                << evt->pdf_info()->pdf_id1() << ", "
                << evt->pdf_info()->pdf_id2() );

  // Set weights to +/-1 if we a generating unweighted events.
  //(consider better way to deal with theMCATNLO case in future...)
  if (gHwevnt->nowgt || m_ExternalProcess == MCATNLO){
    if (gHwevnt->evwgt < 0.) evt->weights().push_back(-1.);
    else if (gHwevnt->evwgt > 0.) evt->weights().push_back(1.);
    else if (gHwevnt->evwgt == 0.) ATH_MSG_WARNING("EVENT WEIGHT = 0 !!!!!!");
  }  else {
    evt->weights().push_back(gHwevnt->evwgt);
  }
  // Set user weights for WZ McAtNlo processes
  if(m_ExternalProcess == MCATNLO){
    Wgtacp uwgts;

    if(uwgts.userwgts() != 0){
       // check if we have WW or WZ process (WZ only 10 weights .ne. -999, WW 28)

       //for(int i=0; i<uwgts.nElements(); i++){
       for(int i=0; i<uwgts.userwgts(); i++){
             evt->weights().push_back(uwgts.wgtacp(i));
//             std::cout << "wgh, wgtacp[" << i << "] = " << uwgts.wgtacp(i) << std::endl;
       }
     }

    ATH_MSG_DEBUG("McAtNlo userweghts: " << uwgts.userwgts());

    //    HepMC::WeightContainer tst_wgt;
    //    for(unsigned int i=0; i<evt->weights().size(); i++){
    //    tst_wgt = evt->weights();
    //    std::cout << "evt, weights[" << i << "] = " << tst_wgt[i] << std::endl;
    //      }
  }

  if ( m_updateHadronStatCode ) {
    ATH_MSG_INFO( "Hadron update" );
    updateStatusCode( evt );
  }

  // Convert cm->mm and GeV->MeV
  //cmTomm(evt);
  GeVToMeV(evt);

  return StatusCode::SUCCESS;
}

void Herwig::updateStatusCode( HepMC::GenEvent *evt )
{
    // loop over particles in event (put it to a filter? TODO)
  for ( HepMC::GenEvent::particle_iterator p =  evt->particles_begin(); p != evt->particles_end(); ++p ) {
    HepPDT::ParticleID pid( (*p)->pdg_id() );

    if ( pid.isHadron() ) {
      HepMC::GenVertex *endVertex = (*p)->end_vertex();

      int statusCode = (*p)->status();
      if ( endVertex && statusCode != 1 && statusCode != 4 ) {

        bool sameHadronFound = false;
        for ( HepMC::GenVertex::particle_iterator child = endVertex->particles_begin( HepMC::family );
              child != endVertex->particles_end( HepMC::family ); ++child ) {

          if ( *child != *p ) {   // remove current hadron from the list of children
            if ( (*child)->pdg_id() == (*p)->pdg_id() ) {
              sameHadronFound = true;
              break;
            }
          }
        }

        if ( !sameHadronFound ) {
          (*p)->set_status( 2 );
        }
      } // else - doesn't have end_vertex (status code=1)
    }
  }

}

void Herwig::store_Atlas_HEPEVT() {
  ATH_MSG_DEBUG("atlas_HEPEVT params: "
                << "nhep=" << atlas_HEPEVT->nhep() << ", "
                << "isthep(10)=" << atlas_HEPEVT->isthep(10) << ", "
                << "idhep(10)=" << atlas_HEPEVT->idhep(10) << ", "
                << "jmohep(1,10)=" << atlas_HEPEVT->jmohep(1,10) << ", "
                << "jdahep(2,10)=" << atlas_HEPEVT->jdahep(2,10));
   /// @todo Ugly... and a memory leak?
  atlas_HEPEVT->fill();
  Atlas_HEPEVT* ahep = new Atlas_HEPEVT();
  *(ahep) = *(atlas_HEPEVT);
  string keyid = "Herwig";
  StatusCode sc = evtStore()->record(ahep, keyid);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not record Atlas_HEPEVT");
  }
}
