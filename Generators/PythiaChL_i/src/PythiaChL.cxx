/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: GeneratorModules/PythiaChL.cxx
// Description: Allows the user to generate pythia events and store 
// the result in the Transient Store.
//
// AuthorList:
//   Ian Hinchliffe:  Initial Code June: 2000
//   Modeled after the CDF code by Stan Thompson
//   Parsing added August 2000
//   More switches added Feb 2001
//   Les Houches external process interface added May 2002
//   Added access to pyint1 common Sept 2002
//   Splited from Pythia_i by Jan Kotanski 2011

/// @todo Remove all std::cout usage
/// @todo Remove use of newlines in ATH_MSG

// Header for this module:-
#include "Pythia_i/Pythia.h"
#include "PythiaChL_i/PythiaChL.h"
#include "TruthUtils/GeneratorName.h"
 
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "GeneratorUtils/StringParse.h"
#include <cstdlib>

// Calls to Fortran routines
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using std::string;



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
}


// File scope declarations:-

// set pointer to zero at start
//Atlas_HEPEVT*  Pythia::atlas_HEPEVT = new Atlas_HEPEVT();


//--------------------------------------------------------------------------


PythiaChL::PythiaChL(const std::string& name, ISvcLocator* pSvcLocator) 
  : Pythia(name, pSvcLocator) 
{
  m_AtlasDefaults = true;
  pythia_stream	= "PYTHIA_INIT";

  // ATLAS default tune
  m_Default_Tune_Name="ATLAS_20110002";
  m_Default_Tune_Type="ATLAS";
  m_Default_Tune_Index=20100002;
  m_Starting_Tune_Name="ATLAS_99999999";
  m_Starting_Tune_Type="ATLAS";
  m_Starting_Tune_Index=99999999;
}


//--------------------------------------------------------------------------


PythiaChL::~PythiaChL() { }


//---------------------------------------------------------------------------


StatusCode PythiaChL::genuserInitialize() {
  // Initialise the listing output, parameter and decay data input streams
  ATH_MSG_INFO("PYTHIACHL INITIALISING.\n");

  /// @todo Use ServiceHandle method
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
                                  PythiaChL::p_AtRndmGenSvc,
                                  CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == PythiaChL::p_AtRndmGenSvc)
  {
      ATH_MSG_ERROR("Could not initialize Random Number Service");
      return RndmStatus;
  }	
  PythiaChL::pythia_stream = "PYTHIA_INIT";

  // set up the input parameters to pyinit: these can be changed by the user
  m_frame  = "CMS   ";
  m_beam   = "P     ";
  m_target = "P  ";
  m_win=14000.;
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

    this->pydat2().pmas(6,1)  =175.;  // top quark mass
    this->pydat2().pmas(24,1) =80.42; // W mass
    //this->pydat2().pmas(24,2) =2.124; // W width calculated dynamically
    this->pydat2().pmas(23,1) =91.19; // Z0 mass
    //this->pydat2().pmas(23,2) =2.495; // Z0 width calculated dynamically
    this->pypars().mstp(128)  =1;     // fix junk output for documentary particles
    this->pydat1().mstu(21)   =1;     // error handling switch

    this->pypars().mstp(81)   =21; // ATLAS default for MI and also for the associated 
                                   // treatment of initial- and final-state showers
                                   // and beam remnants. MI on; new model for PYEVNW.
    
    // Check the user request here and reset mstp(81)
    for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++) {
      StringParse mystring(m_pythiaCommandVector[i]);
      string myblock=mystring.piece(1);
      string myentry=mystring.piece(2);
      int myint1=mystring.intpiece(3);
      int myint2=mystring.intpiece(4);
      if (myblock == "pypars"){
        if(myentry == "mstp" && myint1 == 81){
          this->pypars().mstp(myint1)=myint2;
        }
      }
    }
    
    // The new pyevnw routine is used which accesses the new scenario with pT-ordered showers and
    // interleaved multiple interactions
    if (this->pypars().mstp(81) >= 20) {
      // this->pypars().mstp(68)=1; Removed for 6.4xxx
      this->pypars().mstp(70)=2;      // (virtuality scale for ISR)
      this->pypars().mstp(72)=0;
      this->pypars().mstp(82)=4;      // (mutiple interaction model)
      this->pypars().mstp(84)=1;
      this->pypars().mstp(85)=1;
      this->pypars().mstp(86)=2;
      this->pypars().mstp(87)=4;
      this->pypars().mstp(88)=0;
      this->pypars().mstp(89)=1;
      this->pypars().mstp(90)=1;
      this->pypars().mstp(95)=1;
      this->pypars().parp(78)=0.2;
      this->pypars().parp(80)=0.01;
      this->pypars().parp(82)=1.9;    // (cut off scale)
      this->pypars().parp(83)=0.3;
      this->pypars().parp(84)=0.5;    // (matter distribution)
      this->pypars().parp(89)=1800.;
      this->pypars().parp(90)=0.22;   // (cut off scale)
      this->pydat1().mstj(11)=3;      // (select peterson for charm fragmentation)
      this->pydat1().mstj(22)=2;      // (Make K_S, Lambda stable)
      this->pydat1().parj(54)=-0.07;  // (c hadronization)
      this->pydat1().parj(55)=-0.006; // (b hadronization)
      this->pydat1().parj(81)=0.14;
      this->pypars().mstp(52)=2;      // (needed for CTEQ6L)
      this->pypars().mstp(54)=2;      // (needed for CTEQ6L)
      this->pypars().mstp(56)=2;      // (needed for CTEQ6L)
      this->pypars().mstp(51)=10042;  // (needed for CTEQ6L)
      this->pypars().mstp(53)=10042;  // (needed for CTEQ6L)
      this->pypars().mstp(55)=10042;  // (needed for CTEQ6L)
    } else {
      this->pypars().mstp(81) = 1;
      this->pypars().mstp(82) = 4;
      this->pypars().mstp(86) = 2;
      this->pypars().parp(67) = 3.;
      this->pypars().parp(82) = 2.2;
      this->pypars().parp(83) = 0.5;
      this->pypars().parp(84) = 0.3;
      this->pypars().parp(85) = 0.33;
      this->pypars().parp(86) = 0.66;
      this->pypars().parp(89) = 1800.;
      this->pypars().parp(90) = 0.24;
      this->pypars().mstp(52) = 2;
      this->pypars().mstp(54) = 2;
      this->pypars().mstp(56) = 2;
      this->pypars().mstp(51) = 10042;
      this->pypars().mstp(53) = 10042;
      this->pypars().mstp(55) = 10042;
      this->pydat1().mstj(11) = 3;
      this->pydat1().mstj(22) = 2;
      this->pydat1().parj(54) = -0.07;
      this->pydat1().parj(55) = -0.006;
    }
  }

  //
  // Parse Commands and Set Values from Properties Service...
  //
  //   for(CommandVector::iterator it = m_pythiaCommandVector.begin(); 
  //     it != m_pythiaCommand.end(); it++ ) { 
  for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++) {
    ATH_MSG_INFO(" Command is: " << m_pythiaCommandVector[i] );
    StringParse mystring(m_pythiaCommandVector[i]);
    string myblock=mystring.piece(1);
    string myentry=mystring.piece(2);
    string mystr=mystring.piece(3);
    int myint1=mystring.intpiece(3);
    int myint2=mystring.intpiece(4);
    int myint3=mystring.intpiece(5);
    int myint4=mystring.intpiece(6);
    int myint5=mystring.intpiece(7);
    double  myfl1=mystring.numpiece(4);
    double  myfl2=mystring.numpiece(5);
    double  myfl0=mystring.numpiece(3);

    // Note that PythiaChL needs doubles hence the convert here
    ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);
    if (myblock=="pyinit") {
      if(myentry=="user"){
        m_frame="USER  ";
        m_external=mystr;
       	m_ExternalProcess = generator_int(mystr);
      /*	if(mystr=="comphep"){
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
          } else if(mystr== "lhef") {
            m_ExternalProcess = 11;
      } else {
	    ATH_MSG_ERROR(
          "PYTHIACHL ERROR, entry PYINIT USER has comphep, acermc, alpgen, madgraph, madcup, "
          << "toprex, lhaext, hvgen and user as options: YOU HAVE SPECIFIED " << myentry);
      }*/
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
            ATH_MSG_ERROR("INCONSISTENT SET OF rndm_IO PARAMETERS : "
                          << "FirstEvent < 1 || SkipEvents < 0. DUMPING RNDM SEEDS SWITCHED OFF" << myentry);
            m_RndmSwitch = 0;
          }
        }

        ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIACHL RANDOM NUMBERS !!!!!!!! ");
        ATH_MSG_INFO(" THE ATHENA/PYTHIACHL DOES NOT USE ANY MORE THE STANDARD  ");
        ATH_MSG_INFO(" PYTHIACHL RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE");
        ATH_MSG_INFO(" ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  ");
        ATH_MSG_INFO(" DETAILS LOOK IN  ");
        ATH_MSG_INFO(" http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf ");
        ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");

        m_RndmSwitch = 0;
      }
      else {
        ATH_MSG_ERROR("PYTHIACHL ERROR, entry PYINIT has  USER PBAR PYLISTI  PYLISTF  PYSTAT  OUTPUT DUMPR WIN AND RNDM_IO: YOU HAVE SPECIFIED " << myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYSUBS has MSEL, MSUB, KFIN AND CKIN: YOU HAVE SPECIFIED " << myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYPARS has MSTP,PARP, MSTI AND PARI: YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYDAT1  HAS MSTU, MSTJ, PARU AND PARJ: YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYDAT2  HAS KCHG, PMAS, PARF AND VCKM: YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYDAT3  HAS KFDP, MDCY, BRAT AND MDME : YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else if (myblock == "pydatr"){
      if(myentry == "mrpy"){
        ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIACHL RANDOM NUMBERS !!!!!!!! ");
        ATH_MSG_INFO(" THE ATHENA/PYTHIACHL DOES NOT USE ANY MORE THE STANDARD  ");
        ATH_MSG_INFO(" PYTHIACHL RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE");
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYDATR  HAS MRPY AND RRPY : YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYMSSM has IMSS AND RMSS: YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYINT2 has ISET KFPR COEF AND ICOL: YOU HAVE SPECIFIED "<< myentry);
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
        ATH_MSG_ERROR("PYTHIACHL ERROR, block PYTCM has ITCM AND RTCM: YOU HAVE SPECIFIED " << myentry);
      }
      
    }

    // AcerMC tt~ decay mode switching
    else if (myblock == "acermc" && myentry=="acset12") {
      if (m_ExternalProcess == ACERMC) {
        this->acermc_acset().acset12()=myint1;
      }
    }
    
    else {
      ATH_MSG_ERROR(" ERROR in PYTHIACHL PARAMETERS   " << myblock << " is and invalid common block name !");
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
  
  // !!!!! PROTECT AGAINST mstp(81)/mstp(82) PythiaChL looping
  if (this->pypars().mstp(81) == 0 && this->pypars().mstp(82) > 2)
  {
    this->pypars().mstp(82) = 1;
    ATH_MSG_INFO(" !!!!!!!!!!  WARNING ON PYTHIACHL LOOPING !!!!!!!! ");
    ATH_MSG_INFO(" YOU HAVE SWITCHED OFF MULTIPLE INTERACTIONS, mstp(81) = 0 ");
    ATH_MSG_INFO(" THE DEFAULT ATLAS MULTIPLE INTERACTIONS SCENARIO, mstp(82) = 4 ");
    ATH_MSG_INFO(" CHANGED TO mstp(82) = 1, BECAUSE PYTHIACHL IS LOOPING WHEN ");
    ATH_MSG_INFO(" mstp(81) = 0 and mstp(82) > 2 ");
    ATH_MSG_INFO(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
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
    if (m_RndmSwitch ==2 ) 
      ATH_MSG_INFO("THERE ARE " << m_RndmFileLength << " EVENTS STORED IN THE PYTHIACHL RANDOM NUMBER FILE \n");
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
  
  // Save the PYTHIACHL_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = PythiaChL::p_AtRndmGenSvc->GetEngine(PythiaChL::pythia_stream);
  const long* sip = engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  pyinit_(frame,beam,target,&winval,
          strlen(frame),strlen(beam),strlen(target)
          );

  // ... and set them back to the stream for proper save
  PythiaChL::p_AtRndmGenSvc->CreateStream(si1, si2, PythiaChL::pythia_stream);
  
  PythiaChL::pythia_stream = "PYTHIA";
  
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
  //----------------------------------------------------------
  // This is the decay table  file 
  //
  //   int mode=2;
  //   int lun=37;
  //   pyupda_(&mode,&lun);
  //   cldcay_(&lun);

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaChL::callGenerator() {
  ATH_MSG_DEBUG("PYTHIACHL generating.\n" );

  // Write/Read the random numbers to/from file if requested
  if (m_RndmSwitch > 0)	RandomNumberIO();
  
  // Save the random number seeds in the event
  CLHEP::HepRandomEngine* engine = PythiaChL::p_AtRndmGenSvc->GetEngine(PythiaChL::pythia_stream);
  const long* s	= engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);
  
  // Generate event
  pyevnt_();

  // Update event counter
  ++m_events;
  int mevlist=m_eventlistlevel; 
  // Is the event to be listed
  if ( m_events >= m_firstlistevent &&
       m_events <= m_lastlistevent) {
    ATH_MSG_DEBUG("PYEVNT event no. " << m_events << " will be listed");
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


StatusCode PythiaChL::genFinalize() {
  ATH_MSG_INFO(" PYTHIACHL Ending.  \n");
  for (std::vector<int>::iterator i = m_pystatlistlevel.begin(); i != m_pystatlistlevel.end(); ++i) {
    ATH_MSG_INFO("Call PYSTAT at endRun with level " << *i);
    pystat_(&(*i));
  }
  std::cout << "MetaData: cross-section (nb)= " << 1000000. * this->pyint5().xsec(0,3) << std::endl;
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode PythiaChL::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG("PYTHIACHL Atlas_HEPEVT Filling.");
  store_Atlas_HEPEVT();

  ATH_MSG_DEBUG("PYTHIACHL Filling.");
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);
  //HepMC::IO_GenEvent output("dump.dat",ios::out);

  // Fill event into HepMC and transient store
  // std::cout << "====================================================================" << std::endl;
  // std::cout << "                 === Event dump IN PYTHIACHL: ===                      " << std::endl;
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


  //  output << evt;

  // Convert cm->mm and GeV->MeV
  // cmTomm(evt);
  GeVToMeV(evt);
  
 return StatusCode::SUCCESS;
}



