/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: GeneratorModules/PythiaTopMdiff.cxx
// Description: Generate pythia events and store the result in the Transient Store.
//
// AuthorList:
//   Ian Hinchliffe:  Initial Code June: 2000
//   Modeled after the CDF code by Stan Thompson
//   Parsing added August 2000
//   More switches added Feb 2001
//   Les Houches external process interface added May 2002
//   Added access to pyint1 common Sept 2002
//   Added access to pypued common Oct. 2008 (L. Mijovic, liza.mijovic@cern.ch)
//   Direct use of generator codes - replaced with gen. names (E.M.Lobodzinska, Dec 2008)
//   Remove obsolete code and pseudo atlas default pythia settings (Cano Ay, April 2009)
//   Move Pythia ATLAS tune from Pythia_i + jO to Pythia_i, LM
//   Added PYGIVE functionality, LM, Jul. 09
//   Tunes schema, LM, Aug. 09
//   Conversion to AthAlgorithm methods (via GenModule) and clean-up, Andy Buckley, Dec 2009
// added access to pypevwt common, March 2010:  Eric Feng (Chicago) <Eric.Feng@cern.ch>

#include "PythiaExo_i/PythiaTopMdiff.h"
#include "TruthUtils/GeneratorName.h"
#include "GeneratorUtils/StringParse.h"
#include <cstdlib>

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "PythiaBc_i/PythiaBc.h"
#include "ExoGraviton_i/ExoGraviton.h"
#include "PathResolver/PathResolver.h"

#include "PythiaExo_i/Lhefinfo.h"

#include "boost/algorithm/string/trim.hpp"


// Pointer to AtRndmGenSvc
IAtRndmGenSvc* PythiaTopMdiff::p_AtRndmGenSvc = 0;
std::string	PythiaTopMdiff::pythia_stream = "PYTHIA_INIT";
extern "C" double atl_pyr_( int* /*idummy*/ )
{
  CLHEP::HepRandomEngine* engine = 	PythiaTopMdiff::p_AtRndmGenSvc->GetEngine(PythiaTopMdiff::pythia_stream);
  return CLHEP::RandFlat::shoot(engine);
}

extern "C" {
  void initpyblock_(int*, const char*);
  int  openrandom_(int*, int*, const char*);
  void pyinit_(const char*,
               const char*,
               const char*,
               double* ,
               int, // lengths of character strings
               int,// (should be value, not reference)
               int
               );
  void pyevnt_();
  void pyevnw_();
  void pystat_(int*);
  void pylist_(int*);
  void pyupda_(int*, int*);
  void pyrget_(int*, int*);
  void pyrset_(int*, int*);
  void addparticle_(const char*, int slen);
  //pygive implementation: calls Pythia SUBROUTINE PYGIVE(CHIN) 
  void pygive_(const char*,int); // character string to pygive && its length
  //pytune : calls Pythia SUBROUTINE PYTUNE(ITUNE)
  //this call should be used in case one wants to override some of the values PYTUNE sets from the jO
  //in contrary to using MSTP(5) value the PYTUNE will in this case act before PYINIT
  void pytune_(int*); // value of int corresponds to the respective ITUNE value 
  int  pycomp_(int*);
  //    void lunhep_(int*); // STDHEP routine for PYJETS->HEPEVT
  void pyhepc_(int*);
  int opdcay_(const char*, int*, char*, int, int);
  void cldcay_(int*);
  void rinpar_(); // comphep
  void subnum_(); // comphep file
  void rheader_(); // comphep initialization
  void extproc_(int*);
  void opensusyfile_(const char* ,int * , int);
  void setecm_(double*);
  void parout_(int*);
  void specix_(int*);
}


Atlas_HEPEVT* PythiaTopMdiff::s_atlas_HEPEVT = new Atlas_HEPEVT();


//--------------------------------------------------------------------------


PythiaTopMdiff::PythiaTopMdiff(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator)
{
  pythia_stream="PYTHIA_INIT";
  // ATLAS default tune
  m_Default_Tune_Name="ATLAS_20110002";
  m_Default_Tune_Type="ATLAS";
  m_Default_Tune_Index=20110002;
  m_Starting_Tune_Name="ATLAS_99999999";
  m_Starting_Tune_Type="ATLAS";
  m_Starting_Tune_Index=99999999;
  //set dummy values at start
  m_Tune_Index=0;
  m_Tune_Type="";
  //for non-0 value call PYTUNE directly
  m_direct_call_to_pytune=0;
  // some defaults
  m_firstlistevent = -1;
  m_lastlistevent  = -1;
  // old schema, backw. compatibility
  m_mc0809_Tune_Type="ATLAS";
  m_mc09_Tune_Index=20090001;
  m_mc08_Tune_Index=20080001;
  m_Default_useAtlasPythiaRecomm=true;
  m_Default_useAtlasPythiaCommon=true;
  m_Default_useAtlasPythiaTune09=false;
  m_Default_useAtlasPythiaTune08=false;
  m_Default_useNoAtlasPythiaParam=false;
  // printing-out manually set x-section for the MetaData
  m_Default_SetCrossSection=-1.0;
  m_Default_xsscale=-99999;
  // properties
  declareProperty("SusyInputFile",    m_read_Filesusy = " ");
  declareProperty("PythiaCommand",    m_pythiaCommandVector);
  declareProperty("PygiveCommand",    m_PygiveCommandVector);
  declareProperty("Direct_call_to_pytune", m_direct_call_to_pytune);  
  declareProperty("Param_Query_AfterInit",m_Param_Query_AfterInit);
  declareProperty("Param_Query_AfterGen", m_Param_Query_AfterGen);
  declareProperty("CrossSectionScaleFactor", m_xsscale=m_Default_xsscale); 
  declareProperty("SetAtlasDefaults", m_AtlasDefaults = false );
  declareProperty("Tune_Name",  m_Tune_Name = m_Starting_Tune_Name );
  declareProperty("SetCrossSection",m_SetCrossSection=m_Default_SetCrossSection);
  declareProperty("addParticle", m_addParticle = false );
  declareProperty("UseTopMdiff", m_UseTopMdiff = 0); 
  // old schema, backw. compatibility
  declareProperty("useAtlasPythiaRecomm",  m_useAtlasPythiaRecomm = m_Default_useAtlasPythiaRecomm);
  declareProperty("useAtlasPythiaCommon",  m_useAtlasPythiaCommon = m_Default_useAtlasPythiaCommon);
  declareProperty("useAtlasPythiaTune09",  m_useAtlasPythiaTune09 = m_Default_useAtlasPythiaTune09);
  declareProperty("useAtlasPythiaTune08",  m_useAtlasPythiaTune08 = m_Default_useAtlasPythiaTune08);
  declareProperty("useNoAtlasPythiaParam", m_useNoAtlasPythiaParam = m_Default_useNoAtlasPythiaParam);

  m_pygive_gen_cstr = 0;
  m_pygive_init_cstr = 0;
  m_pygive_cstr = 0;

  m_win = 0.;
  m_msel = 0;
  m_eventlistlevel = 0;
  m_initlistlevel = 0;
  m_randomseed = 0;
  m_fortout = 0;
  m_RndmFileNumber = 0;
  m_RndmSwitch = 0;
  m_RndmFirstEvent = 0;
  m_RndmSkipEvents = 0;
  m_RndmMOVE = 0;
  m_RndmFileLength = 0;
  m_events = 0;
  m_ExternalProcess = 0;
  m_atlasTune_loop_prot = 0;


}
//--------------------------------------------------------------------------
PythiaTopMdiff::~PythiaTopMdiff(){
//--------------------------------------------------------------------------
}
//-------------------------------------------------------------
//--------------
// Operations --
//--------------
Pydat1& PythiaTopMdiff::pydat1() {
   return m_pydat1;
}
Pydat2& PythiaTopMdiff::pydat2() {
   return m_pydat2;
}
Pydat3& PythiaTopMdiff::pydat3() {
   return m_pydat3;
}
Pysubs& PythiaTopMdiff::pysubs() {
   return m_pysubs;
}
Pypars& PythiaTopMdiff::pypars() {
   return m_pypars;
}
Pydatr& PythiaTopMdiff::pydatr() {
   return m_pydatr;
}
Pymssm& PythiaTopMdiff::pymssm() {
   return m_pymssm;
}
Pypued& PythiaTopMdiff::pypued() {
   return m_pypued;
}
Pypevwt& PythiaTopMdiff::pypevwt() {
   return m_pypevwt;
}
Pymsrv& PythiaTopMdiff::pymsrv() {
   return m_pymsrv;
}
Pyint1& PythiaTopMdiff::pyint1() {
   return m_pyint1;
}
Pyint2& PythiaTopMdiff::pyint2() {
   return m_pyint2;
}
Pyint5& PythiaTopMdiff::pyint5() {
   return m_pyint5;
}
Pytcsm& PythiaTopMdiff::pytcsm() {
   return m_pytcsm;
}
AcerMC_acset& PythiaTopMdiff::acermc_acset() {
   return m_acermc_acset;
}


//---------------------------------------------------------------------------


StatusCode PythiaTopMdiff::genInitialize() {
  // Initialise the listing output, parameter and decay data input streams
  ATH_MSG_INFO(" PYTHIA INITIALISING.");

  /// @todo Use ServiceHandle
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
				  PythiaTopMdiff::p_AtRndmGenSvc,
				  CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == PythiaTopMdiff::p_AtRndmGenSvc)
  {
      ATH_MSG_ERROR(" Could not initialize Random Number Service");
      return RndmStatus;
  }
  PythiaTopMdiff::pythia_stream	= "PYTHIA_INIT";

  m_atlasTune_loop_prot=0;

  // set up the input parameters to pyinit: these can be changed by the user
  m_frame  = "CMS   ";
  m_beam   = "P     ";
  m_target = "P  ";
  m_win=10000.;
  m_initlistlevel=11;
  m_pystatlistlevel.push_back(1);
  m_fortout=0;

  m_RndmFileName=" ";
  m_RndmFileNumber=0;
  m_RndmSwitch=0;
  m_RndmFirstEvent=1;
  m_RndmSkipEvents=0;
  m_RndmMOVE=0;
  m_RndmFileLength=0;
  m_ExternalProcess = 0;

  // end of setup to pyinit
// special initialisation for Pythiasgluon
   if(m_addParticle==true)
 {
//adding a new particle to Pythia for PythiaSgluon
  std::string s_newParticleCARD_path =
  PathResolver::find_file("sgluons.dat", "DATAPATH");
  const char* c_newParticleCARD_path = s_newParticleCARD_path.c_str();
  addparticle_(c_newParticleCARD_path,strlen(c_newParticleCARD_path));
  }
  //now set defaults
  m_randomseed=19780503;
  this->pydatr().mrpy(1) = m_randomseed; // can be overwritten by user
  // default process is ttbar
  // these can be overwritten by user.
  m_msel=6;
  this->pysubs().msel() = m_msel;

  // Set the ATLAS defaults
  if (m_AtlasDefaults)
    {
      ATH_MSG_WARNING("********** You have chosen the default Atlas Settings.  **********");
      ATH_MSG_WARNING("********** This switch is not valid any more.           **********");
    }

  //m_Tune_Name parsing to find m_Tune_Type and m_Tune_Index for setPythiaTune() and atlasTune() calls
  std::string::size_type sep_index = m_Tune_Name.find_first_of('_');
  if (sep_index != std::string::npos)
    {
      m_Tune_Type=m_Tune_Name.substr(0,sep_index);
      // atoi : returns 0 if conversion not possible - no additional protection needed
      m_Tune_Index=atoi((m_Tune_Name.substr(sep_index+1,m_Tune_Name.length()-1)).c_str());
    }
  else
    {
      // invalid Tune_Name, m_Tune_Type and  m_Tune_Index kept at the dummy vals from constructor,
      // let setPythiaTune() handle the error
      ATH_MSG_WARNING("Invalid Tune_Name: "<< m_Tune_Name <<" specified .");
    }
  // set any Pythia parameters different from Pythia defaults
  // allow only one reset
  if (m_atlasTune_loop_prot<2)
    {
      // m_atlasTune_loop_prot counts setPythiaTune() calls in setPythiaTune.cxx
      StatusCode sc=setPythiaTune();
      if ( sc.isFailure() )
	{
	  ATH_MSG_FATAL("ERROR returned from setPythiaTune, exit.");
	  return StatusCode::FAILURE;
	}
    }
  else
    {
      ATH_MSG_FATAL("More than one reset attempt of tune using setPythiaTune.");
      ATH_MSG_FATAL("Please correct your jobOptions.");
      return StatusCode::FAILURE;
    }

  //call pytune directly in case any parameters set py the tune are to be overwritten from jO
  if (m_direct_call_to_pytune!=0) pytune_(&m_direct_call_to_pytune);

  //PYGIVE command parsing && call
  for (unsigned int i = 0; i < m_PygiveCommandVector.size(); i++)
    {
      m_pygive_cstr = (m_PygiveCommandVector[i]).c_str();
      pygive_(m_pygive_cstr,strlen(m_pygive_cstr));
    }

  // Parse Commands and Set Values from Properties Service...
  for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++) {
    ATH_MSG_INFO(" Command is: " << m_pythiaCommandVector[i]);
    StringParse mystring(m_pythiaCommandVector[i]);
    string myblock = mystring.piece(1);
	string myentry = mystring.piece(2);
	string mystr = mystring.piece(3);
	string mystr2 = mystring.piece(4);
	string mystr3 = mystring.piece(5);
	int myint1 = mystring.intpiece(3);
	int myint2 = mystring.intpiece(4);
	int myint3 = mystring.intpiece(5);
	int myint4 = mystring.intpiece(6);
	int myint5 = mystring.intpiece(7);
	double myfl0 = mystring.numpiece(3);
	double myfl1 = mystring.numpiece(4);
	double myfl2 = mystring.numpiece(5);
	double myfl3 = mystring.numpiece(6);
    // For ExoGraviton_i and PythiaBc_i user param. input
    int myint6 = mystring.intpiece(2);

    // Print a warning when attempt is made to choose PYTUNE tune via MSTP(5) ;
    // more than MSTP(5) setting is needed for consistent PYTUNE run within Athena
    if (myblock == "pypars" && myentry == "mstp" && myint1==5)
      {
        ATH_MSG_WARNING("--------------------------------------------------------------------------------\n"
                        << " MSTP(5) set to "<< myint2 <<", Pythia recieves a command to run PYTUNE # " << myint2 << "\n"
                        << " Please note: " << "\n"
                        << " 1. it is up to you to switch off any ATLAS defaults you do not want to use; " << "\n"
                        << " - the interface takes care of this if PYTUNE tune is called via Pythia.Tune_Name=\"PYTUNE_" << myint2 << "\" in the jO." << "\n"
                        << " 2. in the jO overrides of the parameters set by PYTUNE # " << myint2 << " will have no effect " << "\n"
                        << " - this correspods to (standalone) Pythia behavior " << "\n"
                        << " 3. handling of non-existing PYTUNE tune is same as for (default) Pythia standalone." << "\n"
                        << "--------------------------------------------------------------------------------");
      }

    // Note that Pythia needs doubles hence the convert here
    if (myentry!="use_PYINIT")
      {
	ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);
      }
    else ATH_MSG_INFO(myblock << " block  " << myentry << " set to " << mystr << " " <<mystr2<<" " <<mystr3<<" "<<myfl3);

    if (myblock=="pyinit") {
      if (myentry=="user"){
        m_frame = "USER  ";
        m_external=mystr;
        m_ExternalProcess = generator_int(mystr);
        extproc_(&m_ExternalProcess);
      }
      else if(myentry=="use_PYINIT")
	{
	  m_frame=mystr;
	  m_beam=mystr2;
	  m_target=mystr3;
	  m_win=myfl3;
	}
      else if(myentry=="FIXT"){
        m_frame="FIXT";
      }
      else if(myentry=="pbar"){
        m_beam="P~- ";
      }
      else if(myentry=="win"){
        m_win=myfl0;
      }
      else if(myentry=="pylisti"){
        m_initlistlevel=myint1;
      }
      else if(myentry=="pylistf"){
        m_eventlistlevel=myint1;
      }
      else if(myentry=="dumpr"){
        m_firstlistevent=myint1;
        m_lastlistevent=myint2;
      }
      else if(myentry=="output"){
	m_envval1=mystr;
	m_fortout=49;
	this->pydat1().mstu(11)=m_fortout;
      }
      else if(myentry=="rndm_IO"){
        m_RndmFileName=mystr;
        m_RndmFileNumber=36;
        m_RndmSwitch=myint2;
        m_RndmFirstEvent=myint3;
        m_RndmSkipEvents=myint4;
        m_RndmMOVE=myint5;
	if (m_RndmSwitch == 1) ++m_RndmSkipEvents;

	if (m_RndmSwitch > 0)
	  {
	    if (m_RndmFirstEvent < 1 || m_RndmSkipEvents < 0)
	      {
		ATH_MSG_ERROR(" INCONSISTENT SET OF rndm_IO PARAMETERS : FirstEvent < 1 || SkipEvents < 0. DUMPING RNDM SEEDS SWITCHED OFF" << myentry);
		m_RndmSwitch = 0;
	      }
	  }

	ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!! ");
	ATH_MSG_INFO(" THE ATHENA/PYTHIA DOES NOT USE ANY MORE THE STANDARD  ");
	ATH_MSG_INFO(" PYTHIA RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE");
	ATH_MSG_INFO(" ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  ");
	ATH_MSG_INFO(" DETAILS LOOK IN  ");
	ATH_MSG_INFO(" http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf ");
	ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");

	m_RndmSwitch = 0;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, entry PYINIT has  USER PBAR PYLISTI  PYLISTF  PYSTAT  OUTPUT DUMPR WIN use_PYINIT AND RNDM_IO: YOU HAVE SPECIFIED " << myentry);
      }


    }
    else if (myblock == "pysubs") {
      if(myentry == "msel"){
	this->pysubs().msel()=myint1;
      }
      else if (myentry == "msub"){
	this->pysubs().msub(myint1)=myint2;
      }
      else if (myentry == "ckin"){
	this->pysubs().ckin(myint1)=myfl1;
      }
      else if (myentry == "kfin"){
	this->pysubs().kfin(myint1,myint2)=myint3;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYSUBS has MSEL, MSUB, KFIN AND CKIN: YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pypars"){
      if(myentry == "mstp"){
	this->pypars().mstp(myint1)=myint2;
      }
      else if(myentry == "msti"){
	this->pypars().msti(myint1)=myint2;
      }
      else if(myentry == "parp"){
	this->pypars().parp(myint1)=myfl1;
      }
      else if(myentry == "pari"){
	this->pypars().pari(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYPARS has MSTP,PARP, MSTI AND PARI: YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pydat1"){
      if(myentry == "mstu"){
	this->pydat1().mstu(myint1)=myint2;
      }
      else if(myentry == "mstj"){
	this->pydat1().mstj(myint1)=myint2;
      }
      else if(myentry == "paru"){
	this->pydat1().paru(myint1)=myfl1;
      }
      else if(myentry == "parj"){
	this->pydat1().parj(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT1  HAS MSTU, MSTJ, PARU AND PARJ: YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pydat2"){
      if(myentry == "kchg"){
	this->pydat2().kchg(myint1,myint2)=myint3;
      }
      else if(myentry == "pmas"){
	this->pydat2().pmas(pycomp_(&myint1),myint2)=myfl2;
      }
      else if(myentry == "parf"){
	this->pydat2().parf(myint1)=myfl1;
      }
      else if(myentry == "vckm"){
	this->pydat2().vckm(myint1,myint2)=myfl2;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT2  HAS KCHG, PMAS, PARF AND VCKM: YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pydat3"){
      if(myentry == "mdcy"){
	this->pydat3().mdcy(myint1,myint2)=myint3;
      }
      else if(myentry == "mdme"){
	this->pydat3().mdme(myint1,myint2)=myint3;
      }
      else if(myentry == "brat"){
	this->pydat3().brat(myint1)=myfl1;
      }
      else if(myentry == "kfdp"){
	this->pydat3().kfdp(myint1,myint2)=myint3;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT3  HAS KFDP, MDCY, BRAT AND MDME : YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pydatr"){
      if(myentry == "mrpy"){
        ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!! ");
        ATH_MSG_INFO(" THE ATHENA/PYTHIA DOES NOT USE ANY MORE THE STANDARD  ");
        ATH_MSG_INFO(" PYTHIA RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE");
        ATH_MSG_INFO(" ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  ");
        ATH_MSG_INFO(" DETAILS LOOK IN  ");
        ATH_MSG_INFO(" http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf ");
        ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");

	this->pydatr().mrpy(myint1)=myint2;
      }
      else if(myentry == "rrpy"){
	this->pydatr().rrpy(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDATR HAS MRPY AND RRPY: YOU HAVE SPECIFIED " << myentry);
      }
    }
    else if (myblock == "pymssm"){
      if (myentry=="imss"){
        if (myint1 == 21 || myint1 == 22) {
          ATH_MSG_WARNING("The setting of imss(21) and imss(22) is not allowed. When imss(1)=11 is chosen imss(21) and imss(22) are set to 66 by default");
        } else {
          this->pymssm().imss(myint1)=myint2;
        }
      }
      else if (myentry=="rmss"){
        this->pymssm().rmss(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYMSSM has IMSS AND RMSS: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else if (myblock == "pypued"){
      if (myentry=="iued")
        {
          this->pypued().iued(myint1)=myint2;
        }
      else if (myentry=="rued"){
	this->pypued().rued(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYPUED has IUED AND RUED: YOU HAVE SPECIFIED "<< myentry);
      }
    }
   else if (myblock == "pypevwt"){
      if (myentry=="ievwt")
	{
 	  this->pypevwt().ievwt(myint1)=myint2;
	}
      else if (myentry=="revwt"){
	this->pypevwt().revwt(myint1)=myfl1;
      }
      else {
	ATH_MSG_ERROR("PYTHIA ERROR, block PYPEVWT has IEVWT AND REVWT: YOU HAVE SPECIFIED "<< myentry) ;
      }
    }
    //CLA: RPV couplings
    else if (myblock == "pymsrv") {
      //LLE
      if (myentry=="rvlam"){
        this->pymsrv().rvlam(myint1)=myfl1;
        ATH_MSG_INFO("setting Yukawa coupling lambda to "<< myfl1);
      }
      //LQD
      else if (myentry=="rvlamp"){
        this->pymsrv().rvlamp(myint1)=myfl1;
        ATH_MSG_INFO("setting Yukawa coupling lambda^prime to "<< myfl1);
      }
      //UDD
      else if (myentry=="rvlamb"){
        this->pymsrv().rvlamb(myint1)=myfl1;
        ATH_MSG_INFO("setting Yukawa coupling lambda^doubelprime to "<< myfl1);
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYMSRV has RVLAM, RVLAMP AND RVLAMB: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else if (myblock == "pyint2"){
      if (myentry == "iset"){
        this->pyint2().iset(myint1)=myint2;
      }
      else if (myentry == "kfpr"){
        this->pyint2().kfpr(myint1,myint2)=myint3;
      }
      else if (myentry == "coef"){
        this->pyint2().coef(myint1,myint2)=myfl2;
      }
      else if (myentry == "icol"){
        this->pyint2().icol(myint1,myint2,myint3)=myint4;
      }
      else {
      ATH_MSG_ERROR("PYTHIA ERROR, block PYINT2 has ISET KFPR COEF AND ICOL: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else if (myblock == "pystat"){
      m_pystatlistlevel.clear();
      for (unsigned i = 2; i <= mystring.string_size(); ++i)
	    m_pystatlistlevel.push_back(mystring.intpiece(i));
    }
    else if (myblock == "pytcsm"){
      if (myentry == "itcm"){
        this->pytcsm().itcm(myint1)=myint2;
      }
      else if (myentry=="rtcm"){
        this->pytcsm().rtcm(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYTCM has ITCM AND RTCM: YOU HAVE SPECIFIED "<< myentry);
      }

    }

    // AcerMC tt~ decay mode switching
    else if (myblock == "acermc" && myentry=="acset12") {
      if (m_ExternalProcess == ACERMC) {
        this->acermc_acset().acset12()=myint1;
      }
    }
    // for ExoGraviton user parameter setup
    else if (myblock == "grav")
     {
       ::WriteGravParam(myint6, myint1, (double)myfl0);
     }
    // for PythiaBc_i user parameters setup
    else if (myblock == "pythiabc")
     {
       ::WritePythiaBcParam(myint6, myint1, (double)myfl0);
     }

    else {
      ATH_MSG_ERROR(" ERROR in PYTHIA PARAMETERS   " << myblock << " is and invalid common block name !");
    }
  }
  // End of param parsing

  // Set the logical Unit Number for the immss(1)=11 option and open the file
  if (this->pymssm().imss(1) == 11) {
    int syunit = 66;
    this->pymssm().imss(21) = syunit;
    this->pymssm().imss(22) = syunit;
    const std::string& fileName = m_read_Filesusy;
    opensusyfile_(fileName.c_str(),&syunit,fileName.size());
  }

  // !!!!! PROTECT AGAINST mstp(81)/mstp(82) Pythia looping
  if (this->pypars().mstp(81) == 0 && this->pypars().mstp(82) > 2)
  {
      this->pypars().mstp(82) = 1;
      ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIA LOOPING !!!!!!!!");
      ATH_MSG_INFO(" YOU HAVE SWITCHED OFF MULTIPLE INTERACTIONS, mstp(81) = 0");
      ATH_MSG_INFO(" THE DEFAULT ATLAS MULTIPLE INTERACTIONS SCENARIO, mstp(82) = 4");
      ATH_MSG_INFO(" CHANGED TO mstp(82) = 1, BECAUSE PYTHIA IS LOOPING WHEN");
      ATH_MSG_INFO(" mstp(81) = 0 and mstp(82) > 2");
      ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  }

  // Now call pyinit and set listing
  // Note the `extra' arguments to pyinit on the end of the argument
  // list which do *not* explicitly occur in the FORTRAN
  // implementation of the function -- one for each character
  // variable, in the same order (reference Randy Herber,
  // herber@fnal.gov). Note also that the `extern "C"' function
  // declaration matches the C usage rather than the FORTRAN
  // definition of the PYINIT function and therefore includes the 3
  // extra arguments.
  // activate the block data
  const char* envval = m_envval1.c_str();
  initpyblock_(&m_fortout,envval);

  if (m_RndmSwitch > 0)
  {
      const char* RndmFileName = m_RndmFileName.c_str();
      m_RndmFileLength = openrandom_(&m_RndmSwitch, &m_RndmFileNumber, RndmFileName);
      if (m_RndmSwitch ==2 ) {
        ATH_MSG_INFO(" THERE ARE " << m_RndmFileLength << " EVENTS STORED IN THE PYTHIA RANDOM NUMBER FILE");
      }
  }

  const char* frame = m_frame.c_str();
  const char* beam = m_beam.c_str();
  double winval = m_win;
  const char* target = m_target.c_str();
  int minlist   = m_initlistlevel;
  // special initialization for comphep
  if(m_external=="comphep"){
    rinpar_();
    subnum_();
    rheader_();
  }
  // special initialization for matchig
  if(m_external=="matchig")
    {
      ATH_MSG_DEBUG("Calling setecm with argument winval=" << winval);
      setecm_(&winval);
    }

  // end of comphep initialization

  ATH_MSG_DEBUG(" ****** BEFORE PYINIT ********");
  ATH_MSG_DEBUG(" MSTU(11) " << this->pydat1().mstu(11) <<
                " MSTU(20) " << this->pydat1().mstu(20)  <<
                " MSTU(118) " << this->pydat1().mstu(118));
  ATH_MSG_DEBUG(" PARU(108) " << this->pydat1().paru(108) <<
                " PARU(117) " << this->pydat1().paru(117) <<
                " PARU(118) " << this->pydat1().paru(118));
  for (int ii = 210; ii < 353; ++ii)
      {
	      ATH_MSG_DEBUG(" BRAT(" << ii << ") "
		  <<  this->pydat3().brat(ii));
	  }
  ATH_MSG_DEBUG(" MDCY(310) "
                << this->pydat3().mdcy(310,1) << ", "
                << this->pydat3().mdcy(310,2) << ", "
                << this->pydat3().mdcy(310,3));

  // Save the PYTHIA_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = PythiaTopMdiff::p_AtRndmGenSvc->GetEngine(PythiaTopMdiff::pythia_stream);
  const long* sip = engine->getSeeds();
  long int	si1 = sip[0];
  long int	si2 = sip[1];


  // write warning for non p-p collistion, strip white-spaces off beam and target strings
  std::string beam_strp(beam); boost::trim(beam_strp);
  std::string target_strp(target); boost::trim(target_strp);
  if ((beam_strp!="P" && beam_strp!="P+" && beam_strp!="p" && beam_strp!="p+") || (target_strp!="P" && target_strp!="P+" && target_strp!="p" && target_strp!="p+"))
    {
      ATH_MSG_WARNING("PYINIT(FRAME,BEAM,TARGET,WIN) called from within ATHENA using parameters: ");
      ATH_MSG_WARNING("FRAME : "<< frame << ", BEAM : "<< beam <<", TARGET: "<< target<< ", WIN: "<< winval);
      ATH_MSG_WARNING("Initialization with non-standard BEAM-TARGET combination: ");
      ATH_MSG_WARNING("making sure sensible PYTHIA parameters setup is used is up to you.");
    }


  pyinit_(frame, beam, target, &winval,
          strlen(frame), strlen(beam), strlen(target));

  // ... and set them back to the stream for proper save
  PythiaTopMdiff::p_AtRndmGenSvc->CreateStream(si1, si2, PythiaTopMdiff::pythia_stream);

  PythiaTopMdiff::pythia_stream	= "PYTHIA";

  ATH_MSG_DEBUG(" ****** AFTER PYINIT ********");
  ATH_MSG_DEBUG(" MSTU(11) " << this->pydat1().mstu(11) <<
                " MSTU(20) " << this->pydat1().mstu(20)  <<
                " MSTU(118) " << this->pydat1().mstu(118));
  ATH_MSG_DEBUG(" PARU(108) " << this->pydat1().paru(108) <<
                " PARU(117) " << this->pydat1().paru(117) <<
                " PARU(118) " << this->pydat1().paru(118));
  for (int ii = 210; ii < 353; ++ii)
    {
      ATH_MSG_DEBUG(" BRAT(" << ii << ") "
                    <<  this->pydat3().brat(ii));
    }
  ATH_MSG_DEBUG(" MDCY(310) "
                << this->pydat3().mdcy(310,1) << ", "
                << this->pydat3().mdcy(310,2) << ", "
                << this->pydat3().mdcy(310,3));

  ATH_MSG_DEBUG(std::resetiosflags(std::ios::fixed));

  // call specix function to enable collecting modifications from PytiaModified files;
  // specix(int=2)=corresponds to enabling TopMdiff 
  // int can be passed to PythiaTopMdiff using UseTopMdiff property in jO to 1
  int TopMdiff_mode=0;
  if ( m_UseTopMdiff == 1) TopMdiff_mode=2;
  specix_(&TopMdiff_mode);
  
  pylist_(&minlist);
  m_events = 0;
  ATH_MSG_DEBUG(" kfpr (186,2)============ " << this->pyint2().kfpr(186,2));
  ATH_MSG_DEBUG(" kfpr (187,2)============ " << this->pyint2().kfpr(187,2));

  //decay table
  const char *p_envar1="PYDAT";
  //  char *p_envval1=0;
  //p_envval1 = 
  getenv(p_envar1);

  // do parameter querries after PYINIT :
  //PYGIVE command parsing && call

  if (m_Param_Query_AfterInit.size()!=0)
    {
      //save MSTU(11) value before the parout_ call which starts a new stream:
      int tmp_m11=this->pydat1().mstu(11);
      // set output to a dedicated file for params after the initialization
      int tmp_mode=1;
      parout_(&tmp_mode);
      // query the requested parameter values:
      for (unsigned int i = 0; i < m_Param_Query_AfterInit.size(); i++) 
	{
	  m_pygive_init_cstr = (m_Param_Query_AfterInit[i]).c_str();
	  pygive_(m_pygive_init_cstr,strlen(m_pygive_init_cstr));
	}
      //set MSTU(11) back to the default value
      this->pydat1().mstu(11)=tmp_m11;
    }
  
  // get rid of an residual old event generation parameters data:  
  if (m_Param_Query_AfterGen.size()!=0)
    {
      //save MSTU(11) value before the parout_ call which starts a new stream:
      int tmp_m11=this->pydat1().mstu(11);
      // set output to a dedicated file for params after the initialization
      int tmp_mode=2;
      parout_(&tmp_mode);
      //set MSTU(11) back to the default value
      this->pydat1().mstu(11)=tmp_m11;
    }

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof(int));
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaTopMdiff::callGenerator() {
  ATH_MSG_DEBUG(" PYTHIA generating.");

  // Write/Read the random numbers to/from file if requested
  if (m_RndmSwitch > 0)	RandomNumberIO();

  // Save the random number seeds in the event
  CLHEP::HepRandomEngine*	engine	=	PythiaTopMdiff::p_AtRndmGenSvc->GetEngine(PythiaTopMdiff::pythia_stream);
  const long*		s	=	engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

  // Generate event
  pyevnt_();

  // update event counter
  ++m_events;
  int mevlist=m_eventlistlevel;
  // Is the event to be listed
  if ( m_events >= m_firstlistevent &&
       m_events <= m_lastlistevent) {
    ATH_MSG_INFO("PYEVNT event no. " << m_events << " will be listed");
    if (m_frame == "USER  ")
    {
	int uspr = 7;
	pylist_(&uspr); // Special listing when external generator was selected
    }
    pylist_(&mevlist); // List this event if required
  } 

  // make parameter querry after event generation:
  for (unsigned int i = 0; i < m_Param_Query_AfterGen.size(); i++) 
    {
      int tmp_m11=this->pydat1().mstu(11); 
      int tmp_mode=3;
      parout_(&tmp_mode);      
      m_pygive_gen_cstr = (m_Param_Query_AfterGen[i]).c_str();
      pygive_(m_pygive_gen_cstr,strlen(m_pygive_gen_cstr));
      //set MSTU(11) back to the default value
      this->pydat1().mstu(11)=tmp_m11;
    }   
  
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv=1;
  // Do the conversion
  pyhepc_(&mconv); // Use STDHEP translation routine 1999/01/27 CG
  if ( HepMC::HEPEVT_Wrapper::number_entries() <= 0 ) {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "!!! " << generator_name(m_ExternalProcess) << " TERMINATES NORMALY: NO MORE EVENTS IN FILE !!!" << std::endl;
    std::cout << "!!! PLEASE IGNORE ANY ATHENA ERROR MESSAGES LIKE !!!" << std::endl;
    std::cout << "!!! AthenaEventLoopMgr  ERROR Terminating event processing loop due to errors !!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return StatusCode::FAILURE;
  }
  HepMC::HEPEVT_Wrapper::set_event_number(m_events);
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode PythiaTopMdiff::genFinalize() {
  ATH_MSG_INFO(" PYTHIA Ending." );
  for (std::vector<int>::iterator i = m_pystatlistlevel.begin(); i != m_pystatlistlevel.end(); ++i)
  {
      ATH_MSG_INFO("Call PYSTAT at endRun with level " << *i);
      pystat_(&(*i));
  }
  if (m_SetCrossSection==m_Default_SetCrossSection)
    {
      //always print out the non-scaled x-section
      std::cout << "MetaData: cross-section (nb)= " << 1000000. * (this->pyint5().xsec(0,3))<< std::endl;
      //print out additional MetaData fields if cross-section scaling is requested
      if (m_xsscale != m_Default_xsscale)
	{
	    std::cout << "MetaData: cross-section*CrossSectionScaleFactor (nb)= " << 1000000. * (this->pyint5().xsec(0,3)) * m_xsscale << std::endl;
	    std::cout << "MetaData: CrossSectionScaleFactor = " << m_xsscale << std::endl;
	}
    }
  else
    {
      // cross section is a fixed number:
      std::cout << " Printing cross-section passed by SetCrossSection in jO: "<< std::endl;
      std::cout << " MetaData: cross-section (nb)= " << m_SetCrossSection << std::endl;
    }
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode PythiaTopMdiff::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG(" PYTHIA Atlas_HEPEVT Filling.");
  store_Atlas_HEPEVT();

  ATH_MSG_DEBUG(" PYTHIA Filling." );
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);

  hepio.fill_next_event(evt);
  int pr_id = PYTHIA + m_ExternalProcess + this->pyint1().mint(1);
  if (m_ExternalProcess > 0) pr_id = PYTHIA + m_ExternalProcess;
  evt->set_signal_process_id(pr_id);
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

  if ( m_external == "lhef" ) {
    Lhefinfo linfo; 

    ATH_MSG_DEBUG("lhefinfo, id1: " << linfo.id1());
    ATH_MSG_DEBUG("lhefinfo, id2: " << linfo.id2());
    ATH_MSG_DEBUG("lhefinfo, x1: " << linfo.x1());
    ATH_MSG_DEBUG("lhefinfo, x2: " << linfo.x2());
    ATH_MSG_DEBUG("lhefinfo, scale: " << linfo.scalePdf());
    ATH_MSG_DEBUG("lhefinfo, xpdf1: " << linfo.xPdf1());
    ATH_MSG_DEBUG("lhefinfo, xpdf2: " << linfo.xPdf2());

    HepMC::PdfInfo tmp_pdi( linfo.id1(), linfo.id2(), linfo.x1(), linfo.x2(),
      linfo.scalePdf(), linfo.xPdf1(), linfo.xPdf2() );
    evt->set_pdf_info(tmp_pdi);

  } else {
    HepMC::PdfInfo tmp_pdi(id1,id2,x1,x2,q,pdf1,pdf2);
    evt->set_pdf_info(tmp_pdi);
  }

  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->id1());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->id2());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->x1());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->x2());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->scalePDF());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->pdf1());
  ATH_MSG_DEBUG("pdfinfo:"<<  evt->pdf_info()->pdf2());

  // Convert cm->mm and GeV->MeV
  // cmTomm(evt);
  GeVToMeV(evt);

  return StatusCode::SUCCESS;
}


void
PythiaTopMdiff::RandomNumberIO()
{
    // m_RndmSwitch == 1 : Write the random numbers of the selected events into
    //                     the file m_RndmFileNumber
    if (m_RndmSwitch == 1)
    {
      int WriteEvent = m_events - m_RndmFirstEvent + 1;
      if (WriteEvent >= 0)
      {
	    WriteEvent = WriteEvent%m_RndmSkipEvents;
	    if (WriteEvent == 0) pyrget_(&m_RndmFileNumber, &m_RndmMOVE);
      }
    }
    // m_RndmSwitch == 2 : Read the random numbers from the selected records from
    //                     the file m_RndmFileNumber
    else if (m_RndmSwitch == 2)
    {
        int MOVE = m_RndmMOVE;
        if (MOVE == 0)
        {
            if (m_RndmFirstEvent > 0)
            {
                if (m_events == 0)
                {
                    MOVE = m_RndmFirstEvent - 1;
                }
                else
                {
                    MOVE = m_RndmSkipEvents;
                }
            }
            int c_pos = this->pydatr().mrpy(6) + MOVE;
            if (c_pos < m_RndmFileLength)
            {
                ATH_MSG_INFO("EVENT " << m_events+1
                             << " will be generated with the random seeds read from record "
                             << c_pos+1 << " in file " << m_RndmFileName);
                pyrset_(&m_RndmFileNumber, &MOVE);
            }
        }
        else
        {
            pyrset_(&m_RndmFileNumber, &MOVE);
        }
    }
}



void
PythiaTopMdiff::store_Atlas_HEPEVT()
{

    ATH_MSG_DEBUG("s_atlas_HEPEVT------" << s_atlas_HEPEVT->nhep() );
    ATH_MSG_DEBUG("s_atlas_HEPEVT------" << s_atlas_HEPEVT->isthep(10) );
    ATH_MSG_DEBUG("s_atlas_HEPEVT------" << s_atlas_HEPEVT->idhep(10) );
    ATH_MSG_DEBUG("s_atlas_HEPEVT------" << s_atlas_HEPEVT->jmohep(1,10) );
    ATH_MSG_DEBUG("s_atlas_HEPEVT------" << s_atlas_HEPEVT->jdahep(2,10) );

    s_atlas_HEPEVT->fill();

    Atlas_HEPEVT* ahep = new Atlas_HEPEVT();
    *(ahep) = *(s_atlas_HEPEVT);
    static const std::string keyid = "Pythia";
    StatusCode sc = evtStore()->record(ahep, keyid);
    if (!sc.isSuccess()) {
      ATH_MSG_WARNING("Could not record Atlas_HEPEVT");
    }

}
