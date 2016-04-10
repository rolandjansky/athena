/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: GeneratorModules/PythiaRhad.cxx
// Description: Allows the user to generate Pythia-RHad events and 
// store the result in the Transient Store.
//
// AuthorList:
//   Ian Hinchliffe:  Initial Code June: 2000
//   Modeled after the CDF code by Stan Thompson
//   Parsing added August 2000
//   More switches added Feb 2001
//   Les Houches external process interface added May 2002
//   Added access to pyint1 common Sept 2002

/// @todo Remove all std::cout usage
/// @todo Remove use of newlines in ATH_MSG

// Header for this module:-
#include "Pythia_i/Pythia.h"
#include "PythiaRhad_i/PythiaRhad.h"
#include "TruthUtils/GeneratorName.h"
#include "GeneratorUtils/StringParse.h"

#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <fstream>
#include <cstdlib>

// Calls to Fortran routines
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using std::string;



extern "C" {
  void initpyblock_(int*, const char*);
  int  openrandom_(int*, int*, const char*);
  void pyrhad_();
  void pygldc_();
  void pyglfr_(int*);
  void pysthad_();
  void pystfr_(int*);
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
  int  pycomp_(int*);
  // void lunhep_(int*); // STDHEP routine for PYJETS->HEPEVT
  void pyhepc_(int*);
  int opdcay_(const char*, int*, char*, int, int);
  void cldcay_(int*);
  void rinpar_(); // comphep
  void subnum_(); // comphep file
  void rheader_(); // comphep initialization
  void extproc_(int*);
  void opensusyfile_(const char* ,int * , int);

  void pyrhre_();
  void pyrhin_(double*);
  void pysqdc_(int*);
  void pyglfrre_(int*);
  void pyglfrin_(int*);
  void pyggpro_(double*);
  void pystfrre_(int*);
  void pysbfrre_(int*);
  //pygive implementation: calls Pythia SUBROUTINE PYGIVE(CHIN)
  void pygive_(const char*,int); // character string to pygive && its length


}

// File scope declarations:-
using namespace HepMC;


//--------------------------------------------------------------------------

PythiaRhad::PythiaRhad(const std::string& name, ISvcLocator* pSvcLocator) 
  : Pythia(name, pSvcLocator)
{
  pythia_stream	= "PYTHIA_INIT";
  // ATLAS default tune
  m_Default_Tune_Name="ATLAS_20090001";
  // old schema backw. compatibility
  m_mc0809_Tune_Type="ATLAS";
  m_mc09_Tune_Index=20090001;
  m_mc08_Tune_Index=20080001;
  //set dummy values at start
  m_Tune_Index=0;
  m_Tune_Type="";
  // some defaults
  m_firstlistevent = -1;
  m_lastlistevent  = -1;
 
  // Update defaults for properties declared in base class.
  m_useAtlasPythiaTune09 = true;
  m_useAtlasPythiaTune08 = false;
  m_useNoAtlasPythiaParam = false;
  m_AtlasDefaults = false;
  m_Tune_Name = m_Default_Tune_Name;
  m_read_Filesusy = " ";
 
  declareProperty("RHadronPDGids", m_rhs );
  declareProperty("RunGluinoHadrons",  m_run_gluino_hadrons = false );
  declareProperty("RunStopHadrons",  m_run_stop_hadrons = false );


  declareProperty("RunReggeModel",  m_regge = false );
  declareProperty("RunIntermediateModel",  m_intermediate = false );
  declareProperty("RunSbottomHadrons",  m_run_sbottom_hadrons = false );
  declareProperty("GluinoBallProbability", m_gluino_ball_prob = 0.1 );

  declareProperty("EnableAfterInitialize",m_enable_after_initialize = true);//new: if false, turn ourselves off after initialization, so execute (callGnerator) is not called by AthAlogithm  - important to do if we are only here to decay things!


  // For StoppedGluinos
  declareProperty("strip_out_rh",m_strip_out_rh=false);//Get rid of all but the RHadron decay products
  declareProperty("rh_decay",m_rh_decay=false);//Whether to decay the RHadrons. If you don't decay them, the following manipulations are also turned off.
  declareProperty("boost_rh_to_rest_frame",m_boost_rh_to_rest_frame=false);//Boost to that RHadron's rest frame
  declareProperty("rotate_rh",m_rotate_rh=false);//Rotate the RHadron decay randomly
  declareProperty("translate_rh_to_stopping_position",m_translate_rh_to_stopping_position=false);//Move to a stopping point, defined by the parameters below...
  declareProperty("xshift", m_xshift=0);//mm
  declareProperty("yshift", m_yshift=2500);//mm
  declareProperty("zshift", m_zshift=1000);//mm
  declareProperty("tshift", m_tshift=0);//ns
  declareProperty("randomtshift", m_randomtshift=0);//ns
  declareProperty("StoppingInput", m_StoppingInput, "A matrix of stopping locations" );

  m_doubleid = 1;
}


//--------------------------------------------------------------------------


PythiaRhad::~PythiaRhad() { }





//---------------------------------------------------------------------------


StatusCode PythiaRhad::genuserInitialize() {
  // Initialise the listing output, parameter and decay data input streams
  ATH_MSG_INFO(" PYTHIA INITIALISING.  \n");

  //print out m_rhs
  std::cout<<"ACH123, list of rhadron pdgids:";
  for (unsigned int i=0; i<m_rhs.size(); ++i) {std::cout<<" "<<m_rhs[i];}
  std::cout<<std::endl;

  /// @todo Convert to use ServiceHandle method
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
                                  PythiaRhad::p_AtRndmGenSvc,
                                  CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == PythiaRhad::p_AtRndmGenSvc)
  {
    ATH_MSG_ERROR(" Could not initialize Random Number Service" );
    return RndmStatus;
  }
  PythiaRhad::pythia_stream	=	"PYTHIA_INIT";

  m_atlasTune_loop_prot=0;

  // set up the input parameters to pyinit: these can be changed by the user
  m_frame = "CMS   ";
  m_beam = "P     ";
  m_target = "P  ";
  m_win=7000.;
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
  //now set defaults
  m_randomseed=19780503;
  this->pydatr().mrpy(1) = m_randomseed; // can be overwritten by user
  // default process is ttbar
  // these can be overwritten by user.
  m_msel=6;
  this->pysubs().msel() = m_msel;

  // Set the ATLAS defaults
  if (m_AtlasDefaults) {
    ATH_MSG_WARNING( "********** You have chosen the default Atlas Settings.  **********" ); 
    ATH_MSG_WARNING( "********** Tis switch is not valid any more.       **********" );
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
      // atlasTune_loop_prot counts setPythiaTune() calls in setPythiaTune.cxx      
      StatusCode sc=setPythiaTune();
      if ( sc.isFailure() ) 
        {
          ATH_MSG_FATAL("ERROR returned from setPythiaTune, exit." );
          return StatusCode::FAILURE;
        }
    }
  else 
     {
       ATH_MSG_FATAL ("More than one reset attempt of tune using setPythiaTune.");
       ATH_MSG_FATAL ("Please correct your jobOptions." );
      return StatusCode::FAILURE;
    }

  //PYGIVE command parsing && call
  for (unsigned int i = 0; i < m_PygiveCommandVector.size(); i++) 
    {
      //      std::cout<<"RMA: "<<m_PygiveCommandVector[i]<<std::endl;
      m_pygive_cstr = (m_PygiveCommandVector[i]).c_str();
      pygive_(m_pygive_cstr,strlen(m_pygive_cstr));
    }


  //
  // Parse Commands and Set Values from Properties Service...
  //
  // for(CommandVector::iterator it = m_pythiaCommandVector.begin(); it != m_pythiaCommand.end(); it++ ) {
  for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++) {
    ATH_MSG_INFO(" Command is: " << m_pythiaCommandVector[i]);
    StringParse mystring(m_pythiaCommandVector[i]);
    string myblock=mystring.piece(1);
    string myentry=mystring.piece(2);
    string mystr=mystring.piece(3);
    int myint1=mystring.intpiece(3);
    int myint2=mystring.intpiece(4);
    int myint3=mystring.intpiece(5);
    int myint4=mystring.intpiece(6);
    int myint5=mystring.intpiece(7);
    double myfl1=mystring.numpiece(4);
    double myfl2=mystring.numpiece(5);
    double myfl0=mystring.numpiece(3);

    // Note that Pythia needs doubles hence the convert here
    ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);
    if (myblock=="pyinit") {
      if(myentry=="user"){
        m_frame="USER  ";
        m_external=mystr;
        m_ExternalProcess = generator_int(mystr);
        /*	
            if(mystr=="comphep"){
            m_ExternalProcess = 1;
            } else if(mystr=="user"){
            m_ExternalProcess = 2;
            } else if(mystr=="acermc"){
            m_ExternalProcess = 3;
            } else if(mystr=="alpgen"){
            m_ExternalProcess = 4;
            }  else if (mystr == "madgraph") {
            m_ExternalProcess = 5;
            } else if(mystr== "madcup") {
            m_ExternalProcess = 6;
            } else if(mystr== "toprex") {
            m_ExternalProcess = 7;
            } else if(mystr== "lhaext") {
            m_ExternalProcess = 8;
            } else if(mystr== "matchig") {
            m_ExternalProcess = 9;
            } else if(mystr== "hvgen") {
            m_ExternalProcess = 10;
            } else {
            ATH_MSG_ERROR(
            "PYTHIA ERROR, entry PYINIT USER has comphep, acermc, alpgen, madgraph, madcup, "
            << "toprex, lhaext, hvgen and user as options: YOU HAVE SPECIFIED " << myentry);
            }
        */
        extproc_(&m_ExternalProcess);
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
        
        if (m_RndmSwitch > 0) {
          if (m_RndmFirstEvent < 1 || m_RndmSkipEvents < 0) {
            ATH_MSG_ERROR(" INCONSISTENT SET OF rndm_IO PARAMETERS : "
                          << "FirstEvent < 1 || SkipEvents < 0. DUMPING RNDM SEEDS SWITCHED OFF"
                          << myentry);
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
        ATH_MSG_ERROR("PYTHIA ERROR, entry PYINIT has  USER PBAR PYLISTI  PYLISTF  PYSTAT  OUTPUT DUMPR WIN AND RNDM_IO: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYSUBS has MSEL, MSUB, KFIN AND CKIN: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYPARS has MSTP,PARP, MSTI AND PARI: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT1  HAS MSTU, MSTJ, PARU AND PARJ: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT2  HAS KCHG, PMAS, PARF AND VCKM: YOU HAVE SPECIFIED "<< myentry) ;
      }
    }
    else if (myblock == "pydat3"){
      if(myentry == "mdcy"){
        this->pydat3().mdcy(pycomp_(&myint1),myint2)=myint3;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDAT3  HAS KFDP, MDCY, BRAT AND MDME : YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYDATR  HAS MRPY AND RRPY : YOU HAVE SPECIFIED "<< myentry) ;
      }
    }
    else if (myblock == "pymssm"){
      if (myentry=="imss"){
        if (myint1 == 21 || myint1 == 22) {
          ATH_MSG_WARNING("The seting of imss(21) and imss(22) is not allowed. When imss(1)=11 is chosen imss(21) and imss(22) are set to 66 by default");
        } else {
          this->pymssm().imss(myint1)=myint2;
        }
      }
      else if (myentry=="rmss"){
        this->pymssm().rmss(myint1)=myfl1;
      }
      else {
        ATH_MSG_ERROR("PYTHIA ERROR, block PYMSSM has IMSS AND RMSS: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYINT2 has ISET KFPR COEF AND ICOL: YOU HAVE SPECIFIED "<< myentry) ;
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
        ATH_MSG_ERROR("PYTHIA ERROR, block PYTCM has ITCM AND RTCM: YOU HAVE SPECIFIED "<< myentry) ;
      }

    }

    // AcerMC tt~ decay mode switching
    else if (myblock == "acermc" && myentry=="acset12") {
      if (m_ExternalProcess == ACERMC) {
        this->acermc_acset().acset12()=myint1;
      }
    }

    else {
      ATH_MSG_ERROR(" ERROR in PYTHIA PARAMETERS   " << myblock << " is and invalid common block name !");
    }
  }
  // end of parsing

  // Set the logical Unit Number for the immss(1)=11 option and open the file
  if (this->pymssm().imss(1) == 11) {
    int syunit = 66;
    this->pymssm().imss(21) = syunit;
    this->pymssm().imss(22) = syunit;
    const std::string& fileName = m_read_Filesusy;
    opensusyfile_(fileName.c_str(),&syunit,fileName.size());
  }

  // !!!!! PROTECT AGAINST mstp(81)/mstp(82) Pythia looping
  if (this->pypars().mstp(81) == 0 && this->pypars().mstp(82) > 2) {
    this->pypars().mstp(82) = 1;
    ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIA LOOPING !!!!!!!! ");
    ATH_MSG_INFO(" YOU HAVE SWITCHED OFF MULTIPLE INTERACTIONS, mstp(81) = 0 ");
    ATH_MSG_INFO(" THE DEFAULT ATLAS MULTIPLE INTERACTIONS SCENARIO, mstp(82) = 4 ");
    ATH_MSG_INFO(" CHANGED TO mstp(82) = 1, BECAUSE PYTHIA IS LOOPING WHEN ");
    ATH_MSG_INFO(" mstp(81) = 0 and mstp(82) > 2 ");
    ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
  }

 
  // Protect MI and fragmentation settings to ensure they remain in the supported range for R-hadron fragmentation
  if (this->pypars().mstp(81) > 1)
    {
      ATH_MSG_INFO(" !!!!!!!!!! Multiple interactions setting overridden !!!!!!!! ");
      ATH_MSG_INFO(" MSTP(81) must be either 0 (off) or 1. Setting to 1...");
      this->pypars().mstp(81) = 1;
    }
  if (this->pydat1().mstj(11) != 4)
    {
      ATH_MSG_INFO(" !!!!!!!!!! Fragmentation settings overridden !!!!!!!! ");
      ATH_MSG_INFO(" MSTJ(11) must be 4. Enforcing. Have a nice day...");
      this->pydat1().mstj(11) = 4;
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

  if (m_RndmSwitch > 0) {
    const char* RndmFileName = m_RndmFileName.c_str();
    m_RndmFileLength = openrandom_(&m_RndmSwitch, &m_RndmFileNumber, RndmFileName);
    if (m_RndmSwitch ==2 ) 
      ATH_MSG_INFO(" THERE ARE " << m_RndmFileLength
                   << " EVENTS STORED IN THE PYTHIA RANDOM NUMBER FILE \n");
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

  // end of comphep initialization
  //    std::cout << " ****** BEFORE PYINIT ********" << std::endl;
  //    std::cout << setiosflags(ios::fixed);
  //    std::cout << " MSTU(11) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(11)  << " MSTU(20) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(20)  << " MSTU(118) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(118) << std::endl;
  //    std::cout << " PARU(108) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(108) << " PARU(117) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(117) << " PARU(118) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(118) << std::endl;
  //    for (int ii = 210; ii < 353; ++ii)
  //    {
  //        std::cout << " BRAT(" << ii << ") "
  //  		<< std::setw(10) << setprecision(8) << this->pydat3().brat(ii) << std::endl;
  //    }
  //    std::cout << " MDCY(310) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,1) << ", "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,2) << ", "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,3);
  
  //    std::cout << std::resetiosflags(std::ios::fixed)
  //  	    << std::endl;
  
  // Save the PYTHIA_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = PythiaRhad::p_AtRndmGenSvc->GetEngine(PythiaRhad::pythia_stream);
  const long* sip = engine->getSeeds();
  long int si1	= sip[0];
  long int si2	= sip[1];

  //R-hadron initialization
  // std::cout<<"Gluino switch: "<<m_run_gluino_hadrons<<std::endl;
  // std::cout<<"Stop switch: "<<m_run_stop_hadrons<<std::endl;

  if(m_run_gluino_hadrons)
    pyggpro_(&m_gluino_ball_prob);


  pyinit_(frame,beam,target,&winval,
          strlen(frame),strlen(beam),strlen(target)
          );

  // ... and set them back to the stream for proper save
  PythiaRhad::p_AtRndmGenSvc->CreateStream(si1, si2, PythiaRhad::pythia_stream);

  PythiaRhad::pythia_stream	= "PYTHIA";

  //    std::cout << " ****** AFTER PYINIT ********" << std::endl;
  //    std::cout << setiosflags(ios::fixed);
  //    std::cout << " MSTU(11) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(11)  << " MSTU(20) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(20)  << " MSTU(118) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat1().mstu(118) << std::endl;
  //    std::cout << " PARU(108) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(108) << " PARU(117) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(117) << " PARU(118) "
  //  	    << std::setw(10) << setprecision(8) << this->pydat1().paru(118) << std::endl;
  //    for (int ii = 210; ii < 353; ++ii)
  //    {
  //        std::cout << " BRAT(" << ii << ") "
  //  		<< std::setw(10) << setprecision(8) << this->pydat3().brat(ii) << std::endl;
  //    }
  //    std::cout << " MDCY(310) "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,1) << ", "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,2) << ", "
  //  	    << std::setw(3) << setprecision(0)  << this->pydat3().mdcy(310,3);
  
  //    std::cout << std::resetiosflags(std::ios::fixed)
  //  	    << std::endl;

  pylist_(&minlist);
  m_events = 0;
  // cout << " kfpr ============ " << this->pyint2().kfpr(186,2) <<endl;
  // cout << " kfpr ============ " << this->pyint2().kfpr(187,2) <<endl;

  // Decay table
  const char *p_envar1="PYDAT";
  //  char *p_envval1=0;
  //  p_envval1=
  getenv(p_envar1);

  // This is the decay table file
  //
  //   int mode=2;
  //   int lun=37;
  //   pyupda_(&mode,&lun);
  //    cldcay_(&lun);

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  // HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof(int));
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  // ACH- Get last "double stop" integer from last run for tagging them, for StoppedGluinos simulation
  if (m_translate_rh_to_stopping_position){
    std::string line;
    std::ifstream myfile ("doubleID.txt");
    if (myfile.is_open()) {
      //while (! myfile.eof() ){
      std::getline (myfile,line);
      std::cout<<"Got from doubleID.txt: "<<line<<std::endl;
      m_doubleid = atoi(line.c_str());
      //}
      myfile.close();
    }
    else {
      std::cout<<"Unable to open file doubleID.txt, will start at 1"<<std::endl;
      m_doubleid = 1;
    }
    std::cout<<"m_doubleid = "<<m_doubleid<<std::endl;
  }

  if(m_run_stop_hadrons)
    {
      int kfst = 1000006;
      int kcst = pycomp_(&kfst);
      this->pydat3().mdcy(pycomp_(&kfst),1)=0;
      // Setting width of stop explixitly to zero
      std::ostringstream mypygive;
      mypygive<<"MWID("<<kcst<<")=0";
      //      std::cout<<mypygive.str()<<std::endl;
      pygive_(mypygive.str().c_str(),strlen(mypygive.str().c_str()));
      int kfstbar = -1000006;
      this->pydat3().mdcy(pycomp_(&kfstbar),1)=0;
    }
  if(m_run_sbottom_hadrons)
    {
      int kfst = 1000005;
      int kcst = pycomp_(&kfst);
      this->pydat3().mdcy(pycomp_(&kfst),1)=0;
      // Setting width of stop explixitly to zero
      std::ostringstream mypygive;
      mypygive<<"MWID("<<kcst<<")=0";
      //      std::cout<<mypygive.str()<<std::endl;
      pygive_(mypygive.str().c_str(),strlen(mypygive.str().c_str()));
      int kfstbar = -1000005;
      this->pydat3().mdcy(pycomp_(&kfstbar),1)=0;
    }

  //Done initializing

  //Set myself off to prevent generation, if we are in the sim step!
  if (!m_enable_after_initialize){
    ATH_MSG_INFO("Setting Enable to False, at the end of initialization...");
    setProperty("Enable", "False" );
  }

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaRhad::callGenerator() {
  ATH_MSG_INFO(" PYTHIA generating.  \n");

  int ierr=1;

  while (ierr!=0) {

    // Write/Read the random numbers to/from file if requested
    if (m_RndmSwitch > 0)	RandomNumberIO();
    
    // Save the random number seeds in the event
    CLHEP::HepRandomEngine*	engine = PythiaRhad::p_AtRndmGenSvc->GetEngine(PythiaRhad::pythia_stream);
    const long* s = engine->getSeeds();
    m_seeds.clear();
    m_seeds.push_back(s[0]);
    m_seeds.push_back(s[1]);
    
    //...Put gluino/stop stable.
    //       int kfgl=0;
      //...Put gluino/stop stable.
      //       int kfgl=0;
      if(m_run_gluino_hadrons)
	{
	  int kfgl = 1000021;
	  this->pydat3().mdcy(pycomp_(&kfgl),1)=0;// MDCY(PYCOMP(KFGL),1)=0
	}
      if(m_run_stop_hadrons)
	{
	  int kfst = 1000006;
	  this->pydat3().mdcy(pycomp_(&kfst),1)=0;
	  int kfstbar = -1000006;
	  this->pydat3().mdcy(pycomp_(&kfstbar),1)=0;
	}
      if(m_run_sbottom_hadrons)
	{
	  int kfst = 1000005;
	  this->pydat3().mdcy(pycomp_(&kfst),1)=0;
	  int kfstbar = -1000005;
	  this->pydat3().mdcy(pycomp_(&kfstbar),1)=0;
	}
      this->pydat1().mstj(14)=-1;// MSTJ(14)=-1

      // Generate event
      pyevnt_();

      this->pydat1().mstj(14)=1;// MSTJ(14)=1
      //	std::cout<<"IERR: "<<ierr<<", SEEDS: "<<s[0]<<", "<<s[1]<< std::endl;

      if(m_run_gluino_hadrons) {

	// Running selected hadronization
	if(m_regge)
	  pyglfrre_(&ierr);
	else if(m_intermediate)
	  pyglfrin_(&ierr);
	else
	  pyglfr_(&ierr);

	if (m_rh_decay)
	  {
	    std::cout << "\n ------- Doing RHadron Gluino decay ---------- " << std::endl;
	    int kfgl = 1000021;//gluino
	    this->pydat3().mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable gluino
	    pygldc_();//gluino DECAY!
	  }
      }
      if(m_run_stop_hadrons)
	{
	  // Running selected hadronization
	  if(m_regge)
	    pystfrre_(&ierr);
	  else
	    pystfr_(&ierr);

	  if (m_rh_decay)
	    {
	      std::cout << "\n ------- Doing RHadron Stop decay ---------- " << std::endl;
	      int kfgl = 1000006;//stop
	      this->pydat3().mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable stop
	      int sq=1000006;
	      pysqdc_(&sq);//stop DECAY!
	    }
	}
      if(m_run_sbottom_hadrons)
	{
	  pysbfrre_(&ierr);
	  
	  if (m_rh_decay)
	    {
	      std::cout << "\n ------- Doing RHadron Sbottom decay ---------- " << std::endl;
	      int kfgl = 1000005;//sbottom
	      this->pydat3().mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable sbottom
	      int sq=1000005;
	      pysqdc_(&sq);//sbottom DECAY!
	    }
	}
      
  }

  // update event counter
  ++m_events;
  int mevlist=m_eventlistlevel;
  // Is the event to be listed
  if ( m_events >= m_firstlistevent &&
       m_events <= m_lastlistevent) {
    ATH_MSG_INFO("PYEVNT event no. " << m_events << " will be listed");
    if (m_frame == "USER  ") {
      int uspr = 7;
      pylist_(&uspr); // Special listing when external generator was selected
    }
    pylist_(&mevlist); // List this event if required
  }
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv=1;
  // Do the conversion
  // lunhep_(&mconv); // Use STDHEP translation routine 1999/01/27 CG
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
  // HepMC::HEPEVT_Wrapper::print_hepevt();
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaRhad::genFinalize() {
  ATH_MSG_INFO(" PYTHIA Ending.  \n");
  for (std::vector<int>::iterator i = m_pystatlistlevel.begin(); i != m_pystatlistlevel.end(); ++i) {
    ATH_MSG_INFO("Call PYSTAT at endRun with level " << *i);
    pystat_(&(*i));
  }
  std::cout << "MetaData: cross-section (nb)= " << 1000000. * this->pyint5().xsec(0,3) << std::endl;

  //ACH- Save doubleID for tagging future events in future runs, for StoppedGluinos simulation
  if (m_translate_rh_to_stopping_position){
    std::ofstream myfile ("doubleID.txt",std::ios::trunc);
    if (myfile.is_open()) {
      myfile << m_doubleid <<std::endl;
      std::cout<<"Wrote into doubleID.txt: "<<m_doubleid<<std::endl;
      myfile.close();
    }
    else std::cout << "Unable to open file doubleID.txt for writing!"<<std::endl;
  }

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaRhad::fillEvt(GenEvent* evt) {
  ATH_MSG_INFO(" PYTHIA Atlas_HEPEVT Filling.  \n");
  store_Atlas_HEPEVT();

  ATH_MSG_DEBUG(" PYTHIA Filling.  \n");
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);
  // HepMC::IO_Ascii output("dump.dat",ios::out);

  // Fill event into HepMC and transient store
  // std::cout << "====================================================================" << std::endl;
  // std::cout << "                 === Event dump IN PYTHIA: ===                      " << std::endl;
  // std::cout << "====================================================================" << std::endl;
  // HepMC::HEPEVT_Wrapper::print_hepevt();

  hepio.fill_next_event(evt);
  int pr_id = PYTHIA + m_ExternalProcess + this->pyint1().mint(1);
  if (m_ExternalProcess > 0) pr_id = PYTHIA + m_ExternalProcess;
  evt->set_signal_process_id(pr_id);
  evt->set_random_states(m_seeds);
  evt->weights().push_back(this->pypars().pari(7));
  evt->weights().push_back(this->pypars().pari(9));
  evt->weights().push_back(this->pypars().pari(10));
  //evt -> print();
  //cout << " ----------------- END " << endl;

  // Set beam particle status = 4
  if (evt->valid_beam_particles()) {
    evt->beam_particles().first->set_status(4);
    evt->beam_particles().second->set_status(4);
  }


  // output << evt;

  // Convert cm->mm and GeV->MeV
  //cmTomm(evt);
  GeVToMeV(evt);

  //Manipulate HepMC info for StoppedGluino simulation.
  //Pull out just the parts of the event we want, and possibly boost, rotate, and translate the RHadron decay.
  if (m_strip_out_rh) strip_out_rh(evt);
  
  return StatusCode::SUCCESS;
}




//For StoppedGluino simulation
void PythiaRhad::strip_out_rh(GenEvent* evt){

  CLHEP::HepRandomEngine* engine = PythiaRhad::p_AtRndmGenSvc->GetEngine(PythiaRhad::pythia_stream);
  //evt->print();

  if (m_rh_decay){
    //Remove all but (a single) RHadron decay
    ATH_MSG_INFO("\n ------- Stripping out all but RHadron decay ---------- ");
    std::vector<HepMC::GenVertex*> rhvertices, rhparentvertices;
    for (HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();  v != evt->vertices_end(); ++v ) {
      HepMC::GenVertex *vv = *v;
      for (HepMC::GenVertex::particles_in_const_iterator p = vv->particles_in_const_begin(); p!=vv->particles_in_const_end(); ++p){
        if (std::find(m_rhs.begin(),m_rhs.end(),abs((*p)->pdg_id()))!=m_rhs.end()){
          rhvertices.push_back(vv);
          rhparentvertices.push_back((*p)->production_vertex());
          ATH_MSG_INFO("Found RHadron decay!");
        }
      }
    }
    assert (rhvertices.size()==2);
    int rh=rand()%2;
    std::cout<<"Choosing RHadron "<<rh<<" out of "<<rhvertices.size()<<" total"<<std::endl;
    std::list<HepMC::GenVertex*> rhdaughtervertices;
    for (HepMC::GenVertex::vertex_iterator rhv = rhvertices[rh]->vertices_begin(HepMC::descendants); rhv!=rhvertices[rh]->vertices_end(HepMC::descendants); ++rhv){
      rhdaughtervertices.push_back(*rhv);
    }
    std::cout<<"Found "<<rhdaughtervertices.size()<<" descendant vertices of the RHadron"<<std::endl;
    HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
    while (  v != evt->vertices_end() ) {
      if (std::find(rhdaughtervertices.begin(), rhdaughtervertices.end(), *v)!=rhdaughtervertices.end()) {
        ++v;  continue;//a rhadron daughter!
      }
      if (*v==rhvertices[rh]) {
        ++v;  continue;//a rhadron!
      }
      if (*v==rhparentvertices[rh]) {
        ++v;  continue;//a rhadron parent!
      }
      HepMC::GenVertex *vv = *v;
      evt->remove_vertex(vv);
      v = evt->vertices_begin();
    }

    if (m_boost_rh_to_rest_frame){
      std::cout<<"\n--- Boosting to RHadron rest frame---"<<std::endl;
      CLHEP::HepLorentzVector mom;//will store the momentum of the RHadron
      for ( HepMC::GenVertex::particles_in_const_iterator p = rhvertices[rh]->particles_in_const_begin(); p != rhvertices[rh]->particles_in_const_end(); ++p ) {
        HepMC::GenParticle *pp=*p;
        //std::cout<<"particle coming into rhadron: "<<std::endl; pp->print();
        if (std::find(m_rhs.begin(),m_rhs.end(),abs(pp->pdg_id()))!=m_rhs.end()){
          FourVector mom4 = pp->momentum();
          mom = CLHEP::HepLorentzVector(mom4.px(),mom4.py(),mom4.pz(),mom4.e());
          std::cout<<"Found RHadron particle momentum: "<<mom<<"\n"<<std::endl;
        }
      }
      //now loop through again and boost back!
      CLHEP::Hep3Vector tot_before, tot_after; int nfinal=0;
      for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p ) {
        HepMC::GenParticle *pp=*p;
        //pp->print();
        FourVector mom4 = pp->momentum();
	CLHEP::HepLorentzVector bm(mom4.px(),mom4.py(),mom4.pz(),mom4.e());
        if (pp->status()==1){
          tot_before+=bm;
        }
	CLHEP::Hep3Vector vec(-mom.boostVector());
        bm = bm.boost(vec);
        pp->set_momentum(bm);
        //pp->print();
        if (pp->status()==1){
          FourVector mom4a = pp->momentum();
	  CLHEP::HepLorentzVector bma(mom4a.px(),mom4a.py(),mom4a.pz(),mom4a.e());
          tot_after+=bma;
          ++nfinal;
        }
      }
      ATH_MSG_INFO("Momentum before boost:"<< tot_before);
      ATH_MSG_INFO("Momentum after boost: "<< tot_after);
      ATH_MSG_INFO(nfinal << " final-state particles");
      assert(tot_after.mag()<1e-6);
    }//m_boost_rh_to_rest_frame

  }//m_rh_decay
  else {//!m_rh_decay
    //Remove all but the RHadron (stable) particles
    ATH_MSG_INFO("\n ------- Stripping out all but RHadron (stable) particles ---------- ");
    std::vector<HepMC::GenVertex*> rhvertices;
    for (HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();  v != evt->vertices_end(); ++v ) {
      HepMC::GenVertex *vv = *v;
      for (HepMC::GenVertex::particles_out_const_iterator p = vv->particles_out_const_begin(); p!=vv->particles_out_const_end(); ++p){
        HepMC::GenParticle *pp = *p;
        if (std::find(m_rhs.begin(),m_rhs.end(),abs(pp->pdg_id()))!=m_rhs.end()){
          if (pp->status()==1){
            rhvertices.push_back(vv);
            ATH_MSG_INFO("Found RHadron vertex!");
          }
          else ATH_MSG_INFO("Found RHadon but status is "<<pp->status()<<", not 1 (stable)!");
        }
      }
    }
    assert (rhvertices.size()==2);
    HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
    while (  v != evt->vertices_end() ) {
      if (std::find(rhvertices.begin(), rhvertices.end(), *v)!=rhvertices.end()) {
        HepMC::GenVertex *vv = *v;//a rhadron!
        //remove the other particles in the hadronization of this vertex...
        for (HepMC::GenVertex::particles_out_const_iterator p = vv->particles_out_const_begin(); p!=vv->particles_out_const_end(); ++p){
          HepMC::GenParticle *pp = *p;
          if (std::find(m_rhs.begin(),m_rhs.end(),abs(pp->pdg_id()))!=m_rhs.end()){
            //std::cout<<"Keeping the RHadron particle, and status = "<<pp->status()<<std::endl;
          }
          else {
            ATH_MSG_INFO("Killing "<<pp->pdg_id()<<" with status "<<pp->status());
            delete vv->remove_particle(pp);
            p = vv->particles_out_const_begin();//don't forget to go back to the start of the NEW list...
          }
        }
        ++v;  continue;
      }
      HepMC::GenVertex *vv = *v;
      evt->remove_vertex(vv);
      v = evt->vertices_begin();
    }
  }

  if (m_rotate_rh){
    ATH_MSG_INFO("\n--- Rotating ---");
    double rx = CLHEP::RandFlat::shoot(engine,0.0,2.*M_PI);
    double ry = CLHEP::RandFlat::shoot(engine,0.0,2.*M_PI);
    double rz = CLHEP::RandFlat::shoot(engine,0.0,2.*M_PI);
    ATH_MSG_INFO("rx ry rz = "<<rx<<" "<<ry<<" "<<rz);
    CLHEP::Hep3Vector tot_before, tot_after; int nfinal=0;
    for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p ) {
      HepMC::GenParticle *pp=*p;
      //pp->print();
      FourVector mom4 = pp->momentum();
      CLHEP::HepLorentzVector bm(mom4.px(),mom4.py(),mom4.pz(),mom4.e());
      if (pp->status()==1){
        tot_before+=bm;
      }
      bm.rotateX(rx);
      bm.rotateY(ry);
      bm.rotateZ(rz);
      pp->set_momentum(bm);
      //pp->print();
      if (pp->status()==1){
        FourVector mom4a = pp->momentum();
	CLHEP::HepLorentzVector bma(mom4a.px(),mom4a.py(),mom4a.pz(),mom4a.e());
        tot_after+=bma;
        ++nfinal;
      }
    }
    ATH_MSG_INFO("Momentum before rotate:"<< tot_before);
    ATH_MSG_INFO("Momentum after rotate: "<< tot_after);
    ATH_MSG_INFO(nfinal<<" final-state particles");
    assert((tot_before-tot_after).mag()<1e-6);
  }

  if (m_translate_rh_to_stopping_position){
    ATH_MSG_INFO("\n--- Moving ---");
    int rhid = 0;
    if (m_StoppingInput.size()>1) {
      unsigned int e=m_events;
      ATH_MSG_INFO("Using stopping input from vector position "<<e);
      if (e>=m_StoppingInput.size()) {
        ATH_MSG_WARNING("FAILURE! Out of stopping inputs! Only "<<m_StoppingInput.size()<<" were input!");
      }
      m_xshift=m_StoppingInput[e][0];
      m_yshift=m_StoppingInput[e][1];
      m_zshift=m_StoppingInput[e][2];
      //m_tshift=m_StoppingInput[e][3]; //we want the time of the DECAY, not the stopping time...
      if (m_randomtshift!=0){
        //adjust time according to flat distribution within +-randomtshift ns
        m_tshift = CLHEP::RandFlat::shoot(engine,-m_randomtshift,m_randomtshift);
      }
      if (fabs(m_StoppingInput[e][4]-2)<.1){
        if (fabs(m_StoppingInput[e][5]-1)<.1) rhid=1;
        if (fabs(m_StoppingInput[e][5]-2)<.1) rhid=2;
        ATH_MSG_WARNING("Two stopped in an event!"<<rhid);
        if (rhid==1) {
          m_doubleid++;
          ATH_MSG_DEBUG("m_doubleid is now = "<<m_doubleid);
        }
      }
    }
    ATH_MSG_INFO("shift x y z t = "<<m_xshift<<"mm "<<m_yshift<<"mm "<<m_zshift<<"mm "<<m_tshift<<"ns");
    for (HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();  v != evt->vertices_end(); ++v ) {
      HepMC::GenVertex *vv = *v;

      //The id will be used to keep track of a unique ID which will be the same for each pair of "double stop" decays
      if (rhid>0) vv->set_id(m_doubleid);
      else vv->set_id(-1);

      FourVector pos = vv->position();
      CLHEP::HepLorentzVector hpos(pos.x(),pos.y(),pos.z(),pos.t());
      //std::cout<<"position/time before: "<<hpos<<std::endl;
      //tshift gets multiplied by 300 because HepMC uses units of mm/c = 3.33e-12s or 0.00333ns
      vv->set_position(FourVector(hpos.x()+m_xshift,hpos.y()+m_yshift,hpos.z()+m_zshift,hpos.t()+m_tshift*300.));
      FourVector posa = vv->position();
      CLHEP::HepLorentzVector hposa(posa.x(),posa.y(),posa.z(),posa.t());
      //std::cout<<"position/time after: "<<hposa<<std::endl;
    }
  }

  ATH_MSG_INFO("\n--- Done with RHadron decay manipulations ---\n");
}
