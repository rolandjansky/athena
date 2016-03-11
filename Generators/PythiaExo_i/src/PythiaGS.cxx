/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//
// PythiaGS.cxx Description: Allows the user to generate gravitational 
// scattering pythia events and store the result in the Transient Store.
//
// Adapted from Pythia.cxx by Nele Boelaert
//
//
//
//
// Header for this module:-
#include "PythiaExo_i/PythiaGS.h"
#include "TruthUtils/GeneratorName.h"

// Framework Related Headers:-
// #include "GaudiKernel/MsgStream.h"

// Other classes used by this class:-
#include "StoreGate/StoreGateSvc.h"
 
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "GeneratorUtils/StringParse.h"
#include <stdlib.h>
//-------------------------------
// calls to fortran routines
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// For GravADD
#include "GravADD_i/GravADD.h"
#include "GravADD_i/gammafn.h"


// Pointer On AtRndmGenSvc
IAtRndmGenSvc* 	PythiaGS::p_AtRndmGenSvc  = 0;
std::string	PythiaGS::pythia_stream	=	"PYTHIA_INIT";
//extern massesGRAVADD masses_; 

extern "C" double atl_pyr_( int* /*idummy*/ )
{
  CLHEP::HepRandomEngine* engine = 	PythiaGS::p_AtRndmGenSvc->GetEngine(PythiaGS::pythia_stream);
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
  void pyeffevt_();
  void pystat_(int*);
  void pylist_(int*);
  void pyupda_(int*, int*);
  void pyrget_(int*, int*);
  void pyrset_(int*, int*);
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
  double wtgrav_(int&, double&,int&); 
  int gravfl_();
  void pyevwt_(double*);
  void initgravadd_();
  //void init_datapath_(const char*,const int&);	
}

//-----------------------------------
//using HepMC::Vertex;
//using HepMC::Particle;  
using HepMC::IO_HEPEVT;
//using HepMC::IO_Ascii;

// File scope declarations:-

// set pointer to zero at start
Atlas_HEPEVT*  PythiaGS::s_atlas_HEPEVT = new Atlas_HEPEVT();

//--------------------------------------------------------------------------
PythiaGS::PythiaGS(const std::string& name, 
      ISvcLocator* pSvcLocator): GenModule(name,pSvcLocator)
{
//--------------------------------------------------------------------------  
  pythia_stream	=	"PYTHIA_INIT";
  declareProperty("SusyInputFile",    m_read_Filesusy = " ");
  declareProperty("PythiaCommand",    m_pythiaCommandVector);
  declareProperty("SetAtlasDefaults", m_AtlasDefaults = true );
  m_firstlistevent = -1;
  m_lastlistevent  = -1;

  m_ExternalProcess = 0;
  m_RndmFileLength=0;
  m_RndmFileNumber=0;
  m_RndmFirstEvent=1;
  m_RndmMOVE=0;
  m_RndmSkipEvents=0;
  m_RndmSwitch=0;
  m_eventlistlevel=0;
  m_events = 0;
  m_fortout=0;
  m_gsonly = 0;
  m_initlistlevel=11;
  m_msel=6;
  m_randomseed=19780503;
  m_win=14000.;

  m_sum_pari10=0.;
  m_sum_w=0.;
}
//--------------------------------------------------------------------------
PythiaGS::~PythiaGS(){
//--------------------------------------------------------------------------
}
//-------------------------------------------------------------
//--------------
// Operations --
//--------------
Pydat1& PythiaGS::pydat1() {
   return m_pydat1;
}
Pydat2& PythiaGS::pydat2() {
   return m_pydat2;
}
Pydat3& PythiaGS::pydat3() {
   return m_pydat3; 
}
Pysubs& PythiaGS::pysubs() {
   return m_pysubs;
}
Pypars& PythiaGS::pypars() {
   return m_pypars;
}
Pydatr& PythiaGS::pydatr() {
   return m_pydatr;
}
Pymssm& PythiaGS::pymssm() {
   return m_pymssm;
}
//Pypued& PythiaGS::pypued() {
//   return m_pypued;
//}
//Pymsrv& PythiaGS::pymsrv() {
//   return m_pymsrv;
//}
Pyint1& PythiaGS::pyint1() {
   return m_pyint1;
}
Pyint2& PythiaGS::pyint2() {
   return m_pyint2;
}
Pyint5& PythiaGS::pyint5() {
   return m_pyint5;
}
Pytcsm& PythiaGS::pytcsm() {
   return m_pytcsm;
}
AcerMC_acset& PythiaGS::acermc_acset() {
   return m_acermc_acset;
}

//---------------------------------------------------------------------------
StatusCode PythiaGS::genInitialize() {
  //---------------------------------------------------------------------------
  // Initialise the listing output, parameter and decay data input streams
  //
  //  MsgStream log(messageService(), name());
  //  log << MSG::INFO
  if(msgLvl(MSG::INFO)){ 
   msg(MSG::INFO)  << " PYTHIA INITIALISING.  \n"  << endreq;
  }

  m_sum_pari10=0.;
  m_sum_w=0.;

/*  std::string datapath =  PathResolverFindDataFile ("PythiaExo_i/f2abs.dat");
  datapath = datapath.substr(0,datapath.length()-9);	
  std::cout << "stringpath: " << datapath << "\n" ;	
  init_datapath_(datapath.c_str(),datapath.length());	*/

  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
				  PythiaGS::p_AtRndmGenSvc,
				  CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == PythiaGS::p_AtRndmGenSvc)
  {
   if(msgLvl(MSG::ERROR)){
     msg(MSG::ERROR) << " Could not initialize Random Number Service" << endreq;}
      return RndmStatus;
  }	
  PythiaGS::pythia_stream	=	"PYTHIA_INIT";

  // set up the input parameters to pyinit: these can be changed by the user
  m_frame  = "CMS   ";
  m_beam   = "P     ";
  m_target = "P  ";
  m_win=7000.;
  m_initlistlevel=1;
  m_pystatlistlevel.push_back(1);
  m_fortout=0;
  m_gsonly = 0;


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
    //   Weighted events
    this->pypars().mstp(142)  =1;
    // subprocesses
    this->pysubs().msel() = 0;
    this->pysubs().msub(11)= 1;
    this->pysubs().msub(12)= 1;
    this->pysubs().msub(13)= 1;
    this->pysubs().msub(28)= 1;
    this->pysubs().msub(53)= 1;
    this->pysubs().msub(68)= 1;

    
    this->pypars().mstp(81)=1;         
    this->pypars().mstp(71)=1;         
    this->pypars().mstp(61)=1;           
    this->pypars().mstp(91)=1;             
    this->pypars().mstp(111)=1;     

    this->pypars().mstp(52)=2;      // (needed for CTEQ6L)
    this->pypars().mstp(54)=2;      // (needed for CTEQ6L)
    this->pypars().mstp(56)=2;      // (needed for CTEQ6L)
    this->pypars().mstp(51)=10042;  // (needed for CTEQ6L)
    this->pypars().mstp(53)=10042;  // (needed for CTEQ6L)
    this->pypars().mstp(55)=10042;  // (needed for CTEQ6L)

   
  }

  for (unsigned int i = 0; i < m_pythiaCommandVector.size(); i++) {
    if(msgLvl(MSG::INFO)){
      msg(MSG::INFO) << " Command is: " << m_pythiaCommandVector[i]  << endreq;}
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
// for GravADD_i user param. input
    int myint6=mystring.intpiece(2);
//
    // Note that Pythia needs doubles hence the convert here
    //    log << MSG:: INFO 
    if(msgLvl(MSG::INFO)){
      msg(MSG::INFO) << myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1 <<endreq;}
    if (myblock=="pyinit") {
      if(myentry=="user"){
	m_frame="USER  ";
        m_external=mystr;
	m_ExternalProcess = generator_int(mystr);
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
	
	if (m_RndmSwitch > 0)
	{
	    if (m_RndmFirstEvent < 1 || m_RndmSkipEvents < 0)
	    {
	      //		log << MSG:: ERROR
            if(msgLvl(MSG::ERROR)){
            msg(MSG::ERROR) 
		    << " INCOSISTENT SET OF rndm_IO PARAMETERS : FirstEvent < 1 || SkipEvents < 0. DUMPING RNDM SEEDS SWITCHED OFF"
		    << myentry <<endreq ;}
		m_RndmSwitch = 0;
	    }
	}
      if(msgLvl(MSG::INFO)){
	msg(MSG::INFO) << " !!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!! " << endreq;
	msg(MSG::INFO) << " THE ATHENA/PYTHIA DOES NOT USE ANY MORE THE STANDARD  " << endreq;
	msg(MSG::INFO) << " PYTHIA RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE" << endreq;
	msg(MSG::INFO) << " ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  " << endreq;
	msg(MSG::INFO) << " DETAILS LOOK IN  " << endreq;
	msg(MSG::INFO) << " http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf "
	    << endreq;
	msg(MSG::INFO) << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endreq;}

	m_RndmSwitch = 0;
      }
      else {
	//	log << MSG:: ERROR 
       if(msgLvl(MSG::ERROR)){ 
	 msg(MSG::ERROR) << "PYTHIA ERROR, entry PYINIT has  USER PBAR PYLISTI  PYLISTF  PYSTAT  OUTPUT DUMPR WIN AND RNDM_IO: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR)  << "PYTHIA ERROR, block PYSUBS has MSEL, MSUB, KFIN AND CKIN: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYPARS has MSTP,PARP, MSTI AND PARI: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYDAT1  HAS MSTU, MSTJ, PARU AND PARJ: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYDAT2  HAS KCHG, PMAS, PARF AND VCKM: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYDAT3  HAS KFDP, MDCY, BRAT AND MDME : YOU HAVE SPECIFIED "<< myentry <<endreq ;}
      }
    }
    else if (myblock == "pydatr"){
      if(myentry == "mrpy"){
      if(msgLvl(MSG::INFO)){
	msg(MSG::INFO) << " !!!!!!!!!!  WARNING ON PYTHIA RANDOM NUMBERS !!!!!!!! " << endreq;
	msg(MSG::INFO) << " THE ATHENA/PYTHIA DOES NOT USE ANY MORE THE STANDARD  " << endreq;
	msg(MSG::INFO) << " PYTHIA RANDOM NUMBER SERVICES. SINCE RELEASE 5.2.0 THE" << endreq;
	msg(MSG::INFO) << " ATHENA SERVICE AtRndmGenSvc IS USED. PLEASE FOR MORE  " << endreq;
	msg(MSG::INFO) << " DETAILS LOOK IN  " << endreq;
	msg(MSG::INFO) << " http://atlassw1.phy.bnl.gov/lxrsource/current/atlas/Generators/GeneratorModules/doc/Pythia.pdf "
	    << endreq;
	msg(MSG::INFO) << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endreq;}
	  
	this->pydatr().mrpy(myint1)=myint2;
      }       
      else if(myentry == "rrpy"){
	this->pydatr().rrpy(myint1)=myfl1;
      }
      else {
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYDATR  HAS MRPY AND RRPY : YOU HAVE SPECIFIED "<< myentry <<endreq ;}
      }
    }
    else if (myblock == "pymssm"){
      if (myentry=="imss"){
	if (myint1 == 21 || myint1 == 22) {
	  //	  log << MSG::WARNING
          if(msgLvl(MSG::WARNING)){ 
	    msg(MSG::WARNING) << "The seting of imss(21) and imss(22) is not allowed. When imss(1)=11 is chosen imss(21) and imss(22) are set to 66 by default" <<endreq;}
	} else {
 	  this->pymssm().imss(myint1)=myint2;
	}
      }
      else if (myentry=="rmss"){
	this->pymssm().rmss(myint1)=myfl1;
      }
      else {
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYMSSM has IMSS AND RMSS: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
      if(msgLvl(MSG::ERROR)){
	msg(MSG::ERROR) << "PYTHIA ERROR, block PYINT2 has ISET KFPR COEF AND ICOL: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
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
        if(msgLvl(MSG::ERROR)){
	  msg(MSG::ERROR) << "PYTHIA ERROR, block PYTCM has ITCM AND RTCM: YOU HAVE SPECIFIED "<< myentry <<endreq ;}
      }

    }

    // AcerMC tt~ decay mode switching
    else if (myblock == "acermc" && myentry=="acset12") {
      if (m_ExternalProcess == 3) {
	this->acermc_acset().acset12()=myint1;
      }
    }
// for ExoGraviton user parameter setup
    else if (myblock == "grav")
     {
//    	::WriteGravParam(myint6,myint1,(double)myfl0);
     }
      else if (myblock == "gravADD")
	{
	  ::writegravaddparam(myint6,myint1,(double)myfl0);
	  //        std::cout << "gravADD params: " << myint6 << " " << myint1 << " " << (double)myfl0 << "\n"
	  if (myint6==6 && myint1==1){
	    m_gsonly=1;
	  }
	  if (myint6==2) {
	    m_win = (double)myfl0;
	  }	  
	}
    
    else {
      if(msgLvl(MSG::ERROR)){
	msg(MSG::ERROR) << " ERROR in PYTHIA PARAMETERS   " << myblock << " is and invalid common block name !" << endreq;}
    }
  }
  // end of parsing
  //------------------------------------------------------------------

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
      if(msgLvl(MSG::INFO)){
      msg(MSG::INFO) << " !!!!!!!!!!  WARNING ON PYTHIA LOOPING !!!!!!!! " << endreq;
      msg(MSG::INFO) << " YOU HAVE SWITCHED OFF MULTIPLE INTERACTIONS, mstp(81) = 0 " << endreq;
      msg(MSG::INFO) << " THE DEFAULT ATLAS MULTIPLE INTERACTIONS SCENARIO, mstp(82) = 4 " << endreq;
      msg(MSG::INFO) << " CHANGED TO mstp(82) = 1, BECAUSE PYTHIA IS LOOPING WHEN " << endreq;
      msg(MSG::INFO) << " mstp(81) = 0 and mstp(82) > 2 " << endreq;
      msg(MSG::INFO) << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endreq;}
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
      if (m_RndmSwitch ==2 ) msg(MSG::INFO) << " THEY ARE " << m_RndmFileLength
				 << " EVENTS STORED IN THE PYTHIA RANDOM NUMBER FILE \n"  << endreq;
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
  

  // Save the PYTHIA_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = PythiaGS::p_AtRndmGenSvc->GetEngine(PythiaGS::pythia_stream);
  const long*	sip	=	engine->getSeeds();
  long	int	si1	=	sip[0];
  long	int	si2	=	sip[1];

  if (m_gsonly == 0){  // initialize to run black holes and gravitational scattering
    pyinit_(frame,beam,target,&winval,
	    strlen(frame),strlen(beam),strlen(target)
	    );
  }
  else {   // unitialize to run gravitational scattering but no (!) black holes
    initgravadd_();
    pyinit_("CMS",beam,target,&winval,
	    strlen(frame),strlen(beam),strlen(target)
	 );  
  }

  // ... and set them back to the stream for proper save
  PythiaGS::p_AtRndmGenSvc->CreateStream(si1, si2, PythiaGS::pythia_stream);

  PythiaGS::pythia_stream	=	"PYTHIA";
  
  pylist_(&minlist);
  m_events = 0;
  //  cout << " kfpr ============ " << this->pyint2().kfpr(186,2) <<endl;
  //cout << " kfpr ============ " << this->pyint2().kfpr(187,2) <<endl;
  //decay table
  const char  *p_envar1="PYDAT";
  //  char  *p_envval1=0;
  //p_envval1=
  getenv(p_envar1);
  //----------------------------------------------------------
  // This is the decay table  file 
  //
  //   int mode=2;
  //   int lun=37;
  //   pyupda_(&mode,&lun);
  //    cldcay_(&lun);
  // Set size of common blocks in HEPEVT: note these correspond to stdhep
//   HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof(int));
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode PythiaGS::callGenerator() {
  //---------------------------------------------------------------------------
  //  MsgStream log(messageService(), name());
  //  log << MSG::DEBUG 
  if(msgLvl(MSG::DEBUG)){
    msg(MSG::DEBUG) << " PYTHIA generating.  \n"  << endreq;
  }
  // Write/Read the random numbers to/from file if requested
  if (m_RndmSwitch > 0)	RandomNumberIO();

  // Save the random number seeds in the event
  CLHEP::HepRandomEngine*	engine	=	PythiaGS::p_AtRndmGenSvc->GetEngine(PythiaGS::pythia_stream);
  const long*		s	=	engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);
  
  // Generate event
  //  pyevnt_();
  pyeffevt_();	

  // update event counter
  ++m_events;
  /* int mevlist=m_eventlistlevel; 
  // Is the event to be listed
  if ( m_events >= m_firstlistevent &&
       m_events <= m_lastlistevent) {
    log<< MSG:: INFO << "PYEVNT event no. " << m_events << " will be listed" << endreq;
    if (m_frame == "USER  ")
    {
	int uspr = 7;
	pylist_(&uspr); // Special listing when external generator was selected
    }
    pylist_(&mevlist); // List this event if required
  }  */  
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv=1;				
  // Do the conversion
//    lunhep_(&mconv); // Use STDHEP translation routine 1999/01/27 CG
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
//   HepMC::HEPEVT_Wrapper::print_hepevt();
  return StatusCode::SUCCESS;  
}

//---------------------------------------------------------------------------
StatusCode PythiaGS::genFinalize() {
  //---------------------------------------------------------------------------
  //  MsgStream log(messageService(), name());
if(msgLvl(MSG::INFO)){
  msg(MSG::INFO) << " PYTHIA Ending.  \n"  << endreq;
 }
  for (std::vector<int>::iterator i = m_pystatlistlevel.begin(); i != m_pystatlistlevel.end(); ++i)
  {
   if(msgLvl(MSG::INFO)){ 
     msg(MSG::INFO) <<"Call PYSTAT at endRun with level " << *i << endreq;}
      pystat_(&(*i));
  }
  std::cout << "MetaData: cross-section (nb)= " << 1000000. * this->pyint5().xsec(0,3)*m_sum_pari10/m_sum_w  << std::endl;
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------
StatusCode PythiaGS::fillEvt(HepMC::GenEvent* evt) {
  //---------------------------------------------------------------------------
  //  MsgStream log(messageService(), name());
  if(msgLvl(MSG::DEBUG)){
   msg(MSG::DEBUG) << " PYTHIA Atlas_HEPEVT Filling.  \n"  << endreq;}
  store_Atlas_HEPEVT();

  if(msgLvl(MSG::INFO)){
    msg(MSG::DEBUG) << " PYTHIA Filling.  \n"  << endreq;}
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);

  hepio.fill_next_event(evt);
  int pr_id = PYTHIA + m_ExternalProcess + this->pyint1().mint(1);
  if (m_ExternalProcess > 0) pr_id = PYTHIA + m_ExternalProcess;
  int gravev = 0;  // flag to distinguish QCD, BH and GS events
  m_sum_w +=this->pypars().pari(10);
  if (this->pypars().msti(1) == 4) {  // weight set by BH generator
    this->pypars().pari(10) = 1.0/this->pypars().parp(200);
    gravev = 2;
  }
  else {  // reweight cross section with (sigma QCD + sigma GS) / (sigma GS)
    int mymsti = this->pypars().msti(1);
    double wt;
    wtgrav_(mymsti,wt,gravev);
    this->pypars().pari(10) = this->pypars().pari(10)*wt;
  } 
   m_sum_pari10 +=this->pypars().pari(10);
 
  evt->set_signal_process_id(pr_id);
  evt->set_random_states(m_seeds);
  evt->weights().push_back(this->pypars().pari(7));
  evt->weights().push_back(this->pypars().pari(9));
  evt->weights().push_back(this->pypars().pari(10));
  evt->weights().push_back(gravev);	

  //PDF WEIGHTS - using HepMC::PdfInfo object.. 
  //pdf_id x_1, x_1, Q2, f_1, f_2 
  int id1=this->pypars().msti(15);
  int id2=this->pypars().msti(16);
  double x1=this->pypars().pari(33);
  double x2=this->pypars().pari(34);
  double q=this->pypars().pari(23);
  double pdf1=(double)this->pypars().mstp(51); //pdg id - awkward but..
  double pdf2=0.; //dummy 
  HepMC::PdfInfo tmp_pdi(id1,id2,x1,x2,q,pdf1,pdf2);
  evt->set_pdf_info(tmp_pdi);
  /*  std::cout << "pdfiinfo:"<<  evt->pdf_info()->id1() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->id2() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->x1() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->x2() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->scalePDF() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->pdf1() << std::endl;
  std::cout << "pdfiinfo:"<<  evt->pdf_info()->pdf2() << std::endl;
  */


  // Convert cm->mm and GeV->MeV
//   cmTomm(evt);
  GeVToMeV(evt);
  
 return StatusCode::SUCCESS;
}

void
PythiaGS::RandomNumberIO()
{
  //    MsgStream log(messageService(), name());

    // m_RndmSwitch == 1 : Write the random numbers of the selected events into
    //                     the file m_RndmFileNumber
    if (m_RndmSwitch == 1)
    {
	int WriteEvent = 	m_events - m_RndmFirstEvent + 1;
	if (WriteEvent >= 0)
	{
	    WriteEvent	 =	WriteEvent%m_RndmSkipEvents;
	    if (WriteEvent == 0) pyrget_(&m_RndmFileNumber, &m_RndmMOVE);
	}
    }
    // m_RndmSwitch == 2 : Read the random numbers from the selected records from
    //                     the file m_RndmFileNumber
    else if (m_RndmSwitch == 2)
    {
	int MOVE	=	m_RndmMOVE;
	if (MOVE == 0)
	{
	    if (m_RndmFirstEvent > 0)
	    {
		if (m_events == 0)
		{
		    MOVE	=	m_RndmFirstEvent - 1;
		}
		else
		{
		    MOVE	=	m_RndmSkipEvents;
		}
	    }
	    int c_pos	=	this->pydatr().mrpy(6)+MOVE;
	    if (c_pos < m_RndmFileLength)
	    {
              if(msgLvl(MSG::INFO)){
		msg(MSG::INFO) << "EVENT " << m_events+1
		   << " will be generated with the random seeds read from record "
			       << c_pos+1 << " in file " << m_RndmFileName << endreq; }
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
PythiaGS::store_Atlas_HEPEVT(void)
{

//   std::cout << "s_atlas_HEPEVT------" << s_atlas_HEPEVT->nhep()  << std::endl;
//   std::cout << "s_atlas_HEPEVT------" << s_atlas_HEPEVT->isthep(10)  << std::endl;
//   std::cout << "s_atlas_HEPEVT------" << s_atlas_HEPEVT->idhep(10)  << std::endl;
//   std::cout << "s_atlas_HEPEVT------" << s_atlas_HEPEVT->jmohep(1,10)  << std::endl;
//   std::cout << "s_atlas_HEPEVT------" << s_atlas_HEPEVT->jdahep(2,10)  << std::endl;

  s_atlas_HEPEVT->fill();

  Atlas_HEPEVT* Ahep = new Atlas_HEPEVT();
  *(Ahep)=*(s_atlas_HEPEVT);
  static const std::string keyid = "PythiaGS";
  StatusCode sc = evtStore() ->record(Ahep, keyid);
  if (!sc.isSuccess()) {
    //    MsgStream msg(messageService(), name());
    //    msg << MSG::WARNING 
    if(msgLvl(MSG::WARNING)){
      msg(MSG::WARNING) << " Could not record Atlas_HEPEVT" << endreq;}
  }

}
