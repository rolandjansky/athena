/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// An interface to the Pyquen(ch) generator written by I.P. Lokhtin,
// A.M. Snigirev to model jet quenching on the output of Pythia. Largely
// modelled after the Pythia_i interface written by Ian Hinchcliffe.
//
// AuthorList: Brian Cole, Nathan Grau

#include "Pyquench_i/Pyquench.h"
#include "TruthUtils/GeneratorName.h"
#include "GeneratorUtils/StringParse.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"
#include "CLHEP/Random/RandFlat.h"
#include <cstdlib>
#include <iomanip>
#include <sstream>

// The following code provides an interface to the Pythia random number
// generator.  It is implemented in Pythia_i but since we are replacing that
// code here we need to duplicate definition.
extern "C" double atl_pyr_( int* /* idummy */ ) {
  CLHEP::HepRandomEngine* engine = Pyquench::m_AtRndmGenSvc_p->GetEngine(Pyquench::m_pythia_stream);
  return CLHEP::RandFlat::shoot(engine);
}

//  Declare C interface to Pythia entry points
extern "C" {
  void initpyblock_(int*, const char*);
  int  openrandom_(int*, int*, const char*);
  void pyinit_(const char*, const char*,
               const char*, double* ,
               int, // lengths of character strings
               int, // (should be value, not reference)
               int);
  void pyevnt_();
  void pyevnw_();
  void pyexec_();
  void pystat_(int*);
  void pylist_(int*);
  void pyupda_(int*, int*);
  void pyrget_(int*, int*);
  void pyrset_(int*, int*);
  int  pycomp_(int*);
  void pyhepc_(int*);

  //  pyquen routine
  void pyquen_(double*, int*, double*);
}

// Instantiation of static data members
IAtRndmGenSvc* Pyquench::m_AtRndmGenSvc_p;
std::string    Pyquench::m_pythia_stream = "PYTHIA_INIT";
Atlas_HEPEVT*  Pyquench::s_atlas_HEPEVT = new Atlas_HEPEVT();


//--------------------------------------------------------------------------


//  Constructor
Pyquench::Pyquench(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator), m_events(0)
{
  m_pythia_stream = "PYTHIA_INIT";
  declareProperty("nuclearA", m_nuclearA = 208);
  declareProperty("minBiasYN",m_minBias = true );
  declareProperty("bimpact",  m_bimpact = 0.0 );
  declareProperty("sqrtSNN",  m_rootsNN = 2760.);
  declareProperty("seed",     m_randomseed = 19780503);
  declareProperty("PythiaCommand",    m_pythiaCommandVector);
  declareProperty("TuneIndex", m_tune_index=129);

}


//--------------------------------------------------------------------------


Pyquench::~Pyquench()
{   }


//---------------------------------------------------------------------------


StatusCode Pyquench::genInitialize() {
  ATH_MSG_INFO("Generators/Pyquench ===> PYTHIA INITIALISING.\n");

  //  Initialize the random number generator stream
  /// @todo Remove: this is already done by the GenModule base class
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc", m_AtRndmGenSvc_p, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == Pyquench::m_AtRndmGenSvc_p) {
    ATH_MSG_ERROR(" Could not initialize Random Number Service");
    return RndmStatus;
  }
  m_pythia_stream = "PYTHIA_INIT";

  //  Now setup up Pythia taking into account user input
  setPythiaParameters();
  std::stringstream ss;
  ss << std::setw(10) << "PYQPAR"
     << std::setw(10) << this->pyqpar().T0u()
     << std::setw(10) << this->pyqpar().tau0u()
     << std::setw(10) << this->pyqpar().nfu()
     << std::setw(10) << this->pyqpar().ienglu()
     << std::setw(10) << this->pyqpar().ianglu()
     << std::endl;
  ATH_MSG_INFO(ss.str());
  std::cout << ss.str() << std::endl;
  // Save the PYTHIA_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = Pyquench::m_AtRndmGenSvc_p->GetEngine(m_pythia_stream);
  const long*	sip = engine->getSeeds();
  long	int	si1 = sip[0];
  long	int	si2 = sip[1];

  // set up the input parameters to pyinit: these cannot yet be changed by the user
  const char* frame = "CMS   ";
  const char* proj  = "P     ";
  const char* targ  = "P     ";

  //  Initialize Pythia
  pyinit_(frame, proj, targ, &m_rootsNN,
          strlen(frame), strlen(proj), strlen(targ));

  ATH_MSG_INFO("Generators/Pyquench ===> pyinit_ was called \n");

  // ... and set them back to the stream for proper save
  m_AtRndmGenSvc_p->CreateStream(si1, si2, m_pythia_stream);

  //  And now set up to the use "Pythia" generation stream
  m_pythia_stream = "PYTHIA";

  //  pylist_(&minlist);
  m_events = 0;

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}

StatusCode Pyquench::callGenerator()
{
  ATH_MSG_INFO(" PYQUEN generating.");

  // Write/Read the random numbers to/from file if requested
  //
  //  if (m_RndmSwitch > 0)	RandomNumberIO();

  // Save the random number seeds in the event
  //
  CLHEP::HepRandomEngine* engine = m_AtRndmGenSvc_p->GetEngine(m_pythia_stream);
  const long* s	= engine->getSeeds();

  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

  
  //eml 29.03.10 change mstj(1)=1 to mstj(1)=0 (fix from Nathan Grau)
  this->pydat1().mstj(1)=0;
  pyevnt_();
  int ifb=0;
  if (m_minBias) ifb = 1;

  // Convert the nucleus A to a double
  //
  double inputA = m_nuclearA;

  pyquen_(&inputA, &ifb, &m_bimpact);

  // Now call pythia again with hadronization on
  //
  this->pydat1().mstj(1)=1;
  pyexec_();


  // update event counter
  ++m_events;

  // Convert from PYJETS to HEPEVT
  //
  int mconv = 1;
  pyhepc_(&mconv);
  HepMC::HEPEVT_Wrapper::set_event_number(m_events);

  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------


StatusCode Pyquench::genFinalize() {
  ATH_MSG_INFO(" PYTHIA Ending.");
  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------



StatusCode Pyquench::fillEvt(HepMC::GenEvent* evt)
{
  ATH_MSG_DEBUG(" PYTHIA Atlas_HEPEVT Filling.");
  store_Atlas_HEPEVT();

  ATH_MSG_DEBUG(" PYTHIA Filling." );
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);

  hepio.fill_next_event(evt);
  evt->set_signal_process_id(this->pyint1().mint(1));
  evt->set_random_states(m_seeds);
  evt->weights().push_back(this->pypars().pari(7));
  evt->weights().push_back(this->pypars().pari(9));
  evt->weights().push_back(this->pypars().pari(10));

  // Set beam particle status = 4
  if (evt->valid_beam_particles()) {
    evt->beam_particles().first->set_status(4);
    evt->beam_particles().second->set_status(4);
  }

  // PDF weights, using HepMC::PdfInfo object: pdf_id, x_1, x_1, Q2, f_1, f_2
  const int id1=this->pypars().msti(15);
  const int id2=this->pypars().msti(16);
  const double x1=this->pypars().pari(33);
  const double x2=this->pypars().pari(34);
  const double q=this->pypars().pari(23);
  const double pdf1=(double)this->pypars().pari(29); // x*pdf1
  const double pdf2=(double)this->pypars().pari(30); // x*pdf2 
  const int pdfId1 = (int)this->pypars().mstp( 51 );    // LHAPDF set id of first parton
  const int pdfId2 = (int)this->pypars().mstp( 51 );    // LHAPDF set id of second --

  HepMC::PdfInfo tmp_pdi(id1, id2, x1, x2, q, pdf1, pdf2, pdfId1, pdfId2);
  evt->set_pdf_info(tmp_pdi);

  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->id1());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->id2());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->x1());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->x2());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->scalePDF());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->pdf1());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->pdf2());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->pdf_id1());
  ATH_MSG_DEBUG("pdfinfo: "<<  evt->pdf_info()->pdf_id2());

  // Convert cm->mm and GeV->MeV
  // cmTomm(evt);
  GeVToMeV(evt);

  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------


void Pyquench::store_Atlas_HEPEVT(void)
{
    ATH_MSG_DEBUG("atlas_HEPEVT------" << s_atlas_HEPEVT->nhep() );
    ATH_MSG_DEBUG("atlas_HEPEVT------" << s_atlas_HEPEVT->isthep(10) );
    ATH_MSG_DEBUG("atlas_HEPEVT------" << s_atlas_HEPEVT->idhep(10) );
    ATH_MSG_DEBUG("atlas_HEPEVT------" << s_atlas_HEPEVT->jmohep(1,10) );
    ATH_MSG_DEBUG("atlas_HEPEVT------" << s_atlas_HEPEVT->jdahep(2,10) );

    s_atlas_HEPEVT->fill();

    Atlas_HEPEVT* ahep = new Atlas_HEPEVT();
    *(ahep) = *(s_atlas_HEPEVT);
    static const std::string keyid = "Pythia";
    StatusCode sc = evtStore()->record(ahep, keyid);
    if (!sc.isSuccess())  ATH_MSG_WARNING("Could not record Atlas_HEPEVT");
    }

//--------------------------------------------------------------------------


void Pyquench::setPythiaParameters()
{

  this->pydatr().mrpy(1) = m_randomseed;
  this->pypars().mstp(5) = m_tune_index;

  //set what is necessary for correct use of Pythia within Athena:
  //-----------------------------------------------------------------------------
  this->pypars().mstp(128)  =1;      // fix junk output for documentary particles 	 
  this->pydat1().mstu(21)   =1;      // error handling switch 
  this->pydat1().mstj(22)=2;         // (Make K_S, Lambda stable)
  //-----------------------------------------------------------------------------
  ATH_MSG_INFO( "setPythiaTune: using Pythia internal tune via PYTUNE(" << m_tune_index <<") call.");
  ATH_MSG_INFO( "setting parameters needed for desired functioning of Pythia within Athena        ");
  ATH_MSG_INFO( " mstp(128)  =1;      fix junk output for documentary particles ");
  ATH_MSG_INFO( " mstu(21)   =1;      error handling switch                     ");
  ATH_MSG_INFO( " mstj(22)   =2;      (Make K_S, Lambda stable)                 ");
  ATH_MSG_INFO( " Please note: ");
  ATH_MSG_INFO( " 1. in the jO overrides of the parameters set by PYTUNE # "<< m_tune_index <<" will have no effect " );
  ATH_MSG_INFO( " - this correspods to (standalone) Pythia behavior "  );
  ATH_MSG_INFO( " 2. handling of non-existing PYTUNE tune is same as for (default) Pythia standalone." );

  for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++)
  {
    ATH_MSG_INFO(" Command is: " << m_pythiaCommandVector[i]);
    StringParse mystring(m_pythiaCommandVector[i]);
    std::string myblock = mystring.piece(1);
    std::string myentry = mystring.piece(2);
    std::string mystr = mystring.piece(3);
    std::string mystr2 = mystring.piece(4);
    std::string mystr3 = mystring.piece(5);
    int myint1 = mystring.intpiece(3);
    int myint2 = mystring.intpiece(4);
    int myint3 = mystring.intpiece(5);
    int myint4 = mystring.intpiece(6);
    //int myint5 = mystring.intpiece(7);
    double myfl0 = mystring.numpiece(3);
    double myfl1 = mystring.numpiece(4);
    double myfl2 = mystring.numpiece(5);
    double myfl3 = mystring.numpiece(6);
    // For ExoGraviton_i and PythiaBc_i user param. input
    //int myint6 = mystring.intpiece(2);

    // Note that Pythia needs doubles hence the convert here
    if (myentry!="use_PYINIT") ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);
    else ATH_MSG_INFO(myblock << " block  " << myentry << " set to " << mystr << " " <<mystr2<<" " <<mystr3<<" "<<myfl3);


    if (myblock == "pysubs")
    {
      if(myentry == "msel") this->pysubs().msel()=myint1;
      else if (myentry == "msub") this->pysubs().msub(myint1)=myint2;
      else if (myentry == "ckin") this->pysubs().ckin(myint1)=myfl1;
      else if (myentry == "kfin") this->pysubs().kfin(myint1,myint2)=myint3;
      else  ATH_MSG_ERROR("PYTHIA ERROR, block PYSUBS has MSEL, MSUB, KFIN AND CKIN: YOU HAVE SPECIFIED " << myentry);
    }
    else if (myblock == "pypars")
    {
      if(myentry == "mstp") this->pypars().mstp(myint1)=myint2;
      else if(myentry == "msti") this->pypars().msti(myint1)=myint2;
      else if(myentry == "parp") this->pypars().parp(myint1)=myfl1;
      else if(myentry == "pari") this->pypars().pari(myint1)=myfl1;
      else ATH_MSG_ERROR("PYTHIA ERROR, block PYPARS has MSTP,PARP, MSTI AND PARI: YOU HAVE SPECIFIED " << myentry);
    }
    else if (myblock == "pydat1")
    {
      if(myentry == "mstu") this->pydat1().mstu(myint1)=myint2;
      else if(myentry == "mstj") this->pydat1().mstj(myint1)=myint2;
      else if(myentry == "paru") this->pydat1().paru(myint1)=myfl1;
      else if(myentry == "parj") this->pydat1().parj(myint1)=myfl1;
      else ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT1  HAS MSTU, MSTJ, PARU AND PARJ: YOU HAVE SPECIFIED " << myentry);
    }
    else if (myblock == "pydat2")
    {
      if(myentry == "kchg") this->pydat2().kchg(myint1,myint2)=myint3;
      else if(myentry == "pmas") this->pydat2().pmas(pycomp_(&myint1),myint2)=myfl2;
      else if(myentry == "parf") this->pydat2().parf(myint1)=myfl1;
      else if(myentry == "vckm") this->pydat2().vckm(myint1,myint2)=myfl2;
      else  ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT2  HAS KCHG, PMAS, PARF AND VCKM: YOU HAVE SPECIFIED " << myentry);

    }
    else if (myblock == "pydat3")
    {
      if(myentry == "mdcy") this->pydat3().mdcy(myint1,myint2)=myint3;
      else if(myentry == "mdme") this->pydat3().mdme(myint1,myint2)=myint3;
      else if(myentry == "brat") this->pydat3().brat(myint1)=myfl1;
      else if(myentry == "kfdp") this->pydat3().kfdp(myint1,myint2)=myint3;
      else ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT3  HAS KFDP, MDCY, BRAT AND MDME : YOU HAVE SPECIFIED " << myentry);
    }
    else if (myblock == "pydatr")
    {
      if(myentry == "mrpy")
      {
        ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!! ");
        ATH_MSG_INFO(" THE ATHENA/PYTHIA DOES NOT USE ANY MORE THE STANDARD  ");
        ATH_MSG_INFO(" PYTHIA RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE");
        ATH_MSG_INFO(" ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  ");
        ATH_MSG_INFO(" DETAILS LOOK IN  ");
        ATH_MSG_INFO(" http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf ");
        ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
	
	this->pydatr().mrpy(myint1)=myint2;
      }
      else if(myentry == "rrpy") this->pydatr().rrpy(myint1)=myfl1;
      else  ATH_MSG_ERROR("PYTHIA ERROR, block PYDATR HAS MRPY AND RRPY: YOU HAVE SPECIFIED " << myentry);
    }

    else if (myblock == "pyint2")
    {
      if (myentry == "iset") this->pyint2().iset(myint1)=myint2;
      else if (myentry == "kfpr") this->pyint2().kfpr(myint1,myint2)=myint3;
      else if (myentry == "coef") this->pyint2().coef(myint1,myint2)=myfl2;
      else if (myentry == "icol") this->pyint2().icol(myint1,myint2,myint3)=myint4;
      else  ATH_MSG_ERROR("PYTHIA ERROR, block PYINT2 has ISET KFPR COEF AND ICOL: YOU HAVE SPECIFIED "<< myentry);
    }
    else if (myblock == "pyqpar")
    {
      if(myentry == "T") this->pyqpar().T0u()=myfl0;
      else if(myentry == "tau") this->pyqpar().tau0u()=myfl0;
      else if(myentry == "nfu") this->pyqpar().nfu()=myint1;
      else if(myentry == "ienglu") this->pyqpar().ienglu()=myint1;
      else if(myentry == "ianglu") this->pyqpar().ianglu()=myint1;
      else ATH_MSG_ERROR("PYTHIA ERROR, block PYQPAR has T tau nfu ienlu and ianglu: YOU HAVE SPECIFIED " << myentry);
    }

  }//end loop
}
