/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

/////////////////////////////////////////////////////////
#include "AmdcDb/IRDBAccessSvcWithUpdate.h"

#include "AmdcOracle/AmdcDumpOracle.h"

#include "AmdcDb/AmdcDbRecord.h"

#include "AmdcDb/AmdcDb2Sql.h"

AmdcDumpOracle::AmdcDumpOracle(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
p_IRDBAccessSvcWithUpdate ( "AmdcDb",name )
{

  m_IRDBAccessSvcWithUpdateUpdatedSvcDONE = false ; 

//Set Default values
   m_KountCallsDoIt   = 0 ;

//Declare the properties
   declareProperty("SwitchOff"     , m_SwitchOff    = 1 ) ;
   declareProperty("WarningLevel"  , m_WarningLevel = 1 ) ;
   declareProperty("EmergencyOut"  , m_EmergencyOut = 0 ) ;
   declareProperty("AsNewTableOn"  , m_AsNewTableOn = 0 ) ;
   declareProperty("UseKeysOn"     , m_UseKeysOn    = 1 ) ;

 }
 
// Initialize
StatusCode AmdcDumpOracle::initialize(){

  StatusCode sc ;
  ATH_MSG_INFO( "Initialisation started     " ) ;


  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  if (m_SwitchOff == 0) {
    ATH_MSG_INFO( "= WarningLevel       " << m_WarningLevel      ) ;
    ATH_MSG_INFO( "= EmergencyOut       " << m_EmergencyOut      ) ;
    ATH_MSG_INFO( "= AsNewTableOn       " << m_AsNewTableOn      ) ;
    ATH_MSG_INFO( "= UseKeysOn          " << m_UseKeysOn      ) ;
  }
  ATH_MSG_INFO( "= SwitchOff          " << m_SwitchOff          ) ;
  ATH_MSG_INFO( "================================" ) ;


  if (m_SwitchOff == 0) {

//  Retrieve p_IRDBAccessSvcWithUpdate and set up call back
    if ( p_IRDBAccessSvcWithUpdate.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve service " << p_IRDBAccessSvcWithUpdate ) ;
      return StatusCode::FAILURE;
    } 
    ATH_MSG_INFO( "Retrieved service " << p_IRDBAccessSvcWithUpdate ) ;

    if (p_IRDBAccessSvcWithUpdate->InitializedSvc()) {
      ATH_MSG_INFO( "p_IRDBAccessSvcWithUpdate->InitializedSvc() is true " ) ;
      m_IRDBAccessSvcWithUpdateUpdatedSvcDONE = true ; 
    }else{
      ATH_MSG_INFO( "p_IRDBAccessSvcWithUpdate->InitializedSvc() is false " ) ;

      sc=regFcnDoIt();
      if ( sc.isFailure() ) {
        ATH_MSG_FATAL("regFcnDoIt failed" ) ;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Done: regFcnDoIt " ) ;
     
    }

//  Do something now if possible
    if ( m_IRDBAccessSvcWithUpdateUpdatedSvcDONE ){
      ATH_MSG_INFO( "m_IRDBAccessSvcWithUpdateUpdatedSvcDONE found true in initialize " ) ;
      sc = DoIt() ;
      if ( sc.isFailure() ) {
        ATH_MSG_FATAL( "DoIt failed" ) ; 
        return StatusCode::FAILURE;
      }
    }
  
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;
  if ( m_EmergencyOut == 1 ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

// Do it
StatusCode AmdcDumpOracle::DoItCallback(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "DoItCallback called     " ) ;  

  if ( !(p_IRDBAccessSvcWithUpdate->UsableSvc()) ) {
    ATH_MSG_INFO( "BUT p_IRDBAccessSvcWithUpdate found NOT usable yet  " ) ;  
    return StatusCode::SUCCESS;
  }else{
    m_IRDBAccessSvcWithUpdateUpdatedSvcDONE = true ;
    ATH_MSG_INFO( "AND p_IRDBAccessSvcWithUpdate found usable   " ) ;  
  }

  StatusCode sc = DoIt() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DoIt failed" ) ; 
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}

StatusCode AmdcDumpOracle::regFcnDoIt()
{

  StatusCode sc = detStore()->regFcn(
                         &IRDBAccessSvcWithUpdate::UpdatedSvc,dynamic_cast<IRDBAccessSvcWithUpdate*>(&*p_IRDBAccessSvcWithUpdate),
                         &AmdcDumpOracle::DoItCallback,this,true
                        );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcDumpOracle::DoItCallback from IRDBAccessSvcWithUpdate::UpdatedSvc " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on AmdcDumpOracle::DoItCallback from IRDBAccessSvcWithUpdate::UpdatedSvc " ) ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcDumpOracle::DoIt()
{
  ATH_MSG_INFO( "DoIt called     " ) ;  

  if ( !m_IRDBAccessSvcWithUpdateUpdatedSvcDONE ){
    ATH_MSG_INFO( "DoIt() called BUT m_IRDBAccessSvcWithUpdateUpdatedSvcDONE is false    " ) ;
    return StatusCode::SUCCESS;
  }

  if (m_SwitchOff == 0) {
  
//  Count calls
    m_KountCallsDoIt = m_KountCallsDoIt + 1 ;
    std::string KountCallsDoItASstring = "" ;
    std::stringstream KountCallsDoItASstringstream;
    KountCallsDoItASstringstream << m_KountCallsDoIt ;
    if (m_KountCallsDoIt > 1 ) KountCallsDoItASstring = KountCallsDoItASstringstream.str();

//  Control ouput names
    std::string StringBidon = "";

//  Outout sql files
    AmdcDb2Sql aAmdcDb2Sql;
    aAmdcDb2Sql.SetAsNewTableOn(m_AsNewTableOn);
    aAmdcDb2Sql.SetUseKeysOn(m_UseKeysOn);
    
    StringBidon = "Out.AmdcOracle.DB" + KountCallsDoItASstring ;
    aAmdcDb2Sql.DoIt(StringBidon,"RDB",(&*p_IRDBAccessSvcWithUpdate));
    
    StringBidon = "Out.AmdcOracle.AM" + KountCallsDoItASstring ;
    aAmdcDb2Sql.DoIt(StringBidon,"Amdc",(&*p_IRDBAccessSvcWithUpdate));

//  Check contents
    StringBidon = "Out.AmdcOracle" + KountCallsDoItASstring ;
    std::ofstream OutFile;
    OutFile.open(StringBidon.c_str());
    OutFile << setiosflags(std::ios::fixed);
    
    int Kwarn = 0 ;
    int Kchck = 0 ;
    int KwarnTot = 0 ;
    int KchckTot = 0 ;

//  Check ASZT
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpASZT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ASZT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;
    
//  Check ISZT
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpISZT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ISZT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;
    
//  Check APTP
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpAPTP((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "APTP: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ALMN
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpALMN((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ALMN: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ALIN
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpALIN((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ALIN: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WMDT
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWMDT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WMDT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WSPA
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWSPA((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WSPA: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WSUP
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWSUP((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WSUP: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WCHV
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWCHV((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WCHV: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WCMI
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWCMI((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WCMI: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WCRO
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWCRO((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WCRO: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WLBI
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWLBI((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WLBI: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WDED
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWDED((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WDED: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ASMP
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpASMP((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ASMP: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check DBAM
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpDBAM((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "DBAM: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WCSC
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWCSC((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WCSC: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ATLN
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpATLN((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ATLN: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check GGLN
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpGGLN((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "GGLN: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WTGC
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWTGC((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WTGC: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check AWLN
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpAWLN((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "AWLN: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check WRPC
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpWRPC((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "WRPC: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ACUT
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpACUT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ACUT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check ATYP
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpATYP((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "ATYP: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check AGDD
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpAGDD((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "AGDD: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check AMDC
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpAMDC((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "AMDC: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

// //  Check ABRT
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpABRT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "ABRT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check AECT
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpAECT((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "AECT: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check ECST
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpECST((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "ECST: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check FEET
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpFEET((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "FEET: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check GENV
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpGENV((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "GENV: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check JDSH
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpJDSH((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "JDSH: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check JFSH
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpJFSH((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "JFSH: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check JTSH
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpJTSH((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "JTSH: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;
// 
// //  Check MBAP
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpMBAP((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "MBAP: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;

// //  Check AgddXMLVariables
//     Kwarn = 0 ;
//     Kchck = 0 ;
//     DumpAgddXMLVariables((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
//     KwarnTot = KwarnTot + Kwarn ;
//     KchckTot = KchckTot + Kchck ;
//     OutFile 
//             << "AgddXMLVariables: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
//             << std::endl;


//  Check HwSwIdMapping
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpHwSwIdMapping((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "HwSwIdMapping: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

//  Check XtomoData
    Kwarn = 0 ;
    Kchck = 0 ;
    DumpXtomoData((&*p_IRDBAccessSvcWithUpdate),KountCallsDoItASstring,Kwarn,Kchck);
    KwarnTot = KwarnTot + Kwarn ;
    KchckTot = KchckTot + Kchck ;
    OutFile 
            << "XtomoData: " << Kwarn << " Warnings " << " for " << Kchck << " tested elements " 
            << std::endl;

    OutFile 
            << "Summary: " << KwarnTot << " Warnings " << " for " << KchckTot << " tested elements " 
            << std::endl;

    OutFile.close();

  }

  return StatusCode::SUCCESS;

}

StatusCode AmdcDumpOracle::execute() {return StatusCode::SUCCESS;}
 
StatusCode AmdcDumpOracle::finalize() {return StatusCode::SUCCESS;}

void AmdcDumpOracle::DumpASZT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ASZT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("TYP",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JFF",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JZZ",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JOB",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpISZT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ISZT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("TYP",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JFF",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JZZ",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JOB",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JLAY",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAPTP( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){\

  std::string NameOfTheSet = "APTP";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      for (int DB_JFF=1; DB_JFF<=8 ; DB_JFF++){
        int DB_JFF_Shifted = DB_JFF - 1 ;
        std::ostringstream Aostringstream;
        Aostringstream << DB_JFF_Shifted ;
        std::string DbVar    = "IPHI_"+Aostringstream.str();
        if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable(DbVar,pAmdcDbRecordFromRDB);
      }
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("TYP",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("IZ",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpALMN( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ALMN";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JTYP",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("INDX",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JOB",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpALIN( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ALIN";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
//DRING: here, because sub line is missing, there is no other way than going assuming that tables are parallel
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end() && itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromRDB++,itFromAmdc++){

    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
    if (pAmdcDbRecordFromAmdc == 0){
      OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
      return;
    } 

    pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

  }


  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWMDT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WMDT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("IW",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWSPA( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WSPA";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWSUP( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WSUP";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWCHV( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WCHV";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWCMI( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WCMI";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWCRO( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WCRO";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWLBI( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WLBI";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWDED( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WDED";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpASMP( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ASMP";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("INDX",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JTYP",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);
  
    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpDBAM( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "DBAM";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWCSC( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WCSC";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpATLN( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ATLN";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);
      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("I",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpGGLN( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "GGLN";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWTGC( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WTGC";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAWLN( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "AWLN";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JSTA",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpWRPC( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "WRPC";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;
     
      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);
 
    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpACUT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ACUT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
//DRING: here, because JTYP is missing, there is no other way than going assuming that tables are parallel
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end() && itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromRDB++,itFromAmdc++){

    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
    if (pAmdcDbRecordFromAmdc == 0){
      OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
      return;
    } 

    pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpATYP( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ATYP";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("JTYP",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAGDD( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "AGDD";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAMDC( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "AMDC";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpABRT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ABRT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAECT( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "AECT";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpECST( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "ECST";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpFEET( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "FEET";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpGENV( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "GENV";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}


void AmdcDumpOracle::DumpJDSH( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "JDSH";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpJFSH( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "JFSH";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpJTSH( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "JTSH";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpMBAP( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "MBAP";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;

      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 
     
      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpAgddXMLVariables( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "AgddXMLVariables";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("NameOfVariable",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpHwSwIdMapping( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "HwSwIdMapping";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("HARDNAME",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}

void AmdcDumpOracle::DumpXtomoData( IRDBAccessSvc* pIRDBAccessSvc, std::string TagAdd, int& Kwarn,int& Kchck){

  std::string NameOfTheSet = "XtomoData";

  std::ofstream OutFile;
  std::string FileName = "Out.AmdcOracle."+ TagAdd + NameOfTheSet;
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);
  OutFile << "Checking " << NameOfTheSet << " content started" << std::endl;
  Kwarn = -1 ;
  Kchck = 0 ;
  
  const IRDBRecordset* pIRDBRecordsetFromRDB = pIRDBAccessSvc->getRecordset(NameOfTheSet,"RDB");
  if (pIRDBRecordsetFromRDB->size() == 0){
    OutFile << "No " << NameOfTheSet << " in RDB " << std::endl;
    return;
  } 
  
  const IRDBRecordset* pIRDBRecordsetFromAmdc = pIRDBAccessSvc->getRecordset(NameOfTheSet,"Amdc");
  if (pIRDBRecordsetFromAmdc->size() == 0){
    OutFile << "No " << NameOfTheSet << " in Amdc " << std::endl;
    return;
  } 
  
  Kwarn = 0 ;
  
  std::vector<IRDBRecord*>::const_iterator itFromRDB = pIRDBRecordsetFromRDB->begin();
  for ( ; itFromRDB<pIRDBRecordsetFromRDB->end(); itFromRDB++){
    const AmdcDbRecord* pAmdcDbRecordFromRDB = dynamic_cast<const AmdcDbRecord*>((*itFromRDB));
    if (pAmdcDbRecordFromRDB == 0){
      OutFile << "No way to cast pAmdcDbRecordFromRDB for " << NameOfTheSet << std::endl;
      return;
    } 
    int Ifound = 0 ;
    std::vector<IRDBRecord*>::const_iterator itFromAmdc = pIRDBRecordsetFromAmdc->begin();
    for ( ; itFromAmdc<pIRDBRecordsetFromAmdc->end(); itFromAmdc++){
      if (Ifound==1) continue;
    
      Ifound = 1;

      const AmdcDbRecord* pAmdcDbRecordFromAmdc = dynamic_cast<const AmdcDbRecord*>((*itFromAmdc));
      if (pAmdcDbRecordFromAmdc == 0){
        OutFile << "No way to cast pAmdcDbRecordFromAmdc for " << NameOfTheSet << std::endl;
        return;
      } 

      if (Ifound == 1) Ifound = pAmdcDbRecordFromAmdc->doWeMatchForThisVariable("XtomoChberName",pAmdcDbRecordFromRDB);

      if (Ifound == 1) pAmdcDbRecordFromAmdc->doIMatch(m_WarningLevel,Kwarn,Kchck,OutFile,pAmdcDbRecordFromRDB);

    }

    if (Ifound!=1){
      Kwarn = Kwarn + 1;
      OutFile << "WARNING: No way to check " << pAmdcDbRecordFromRDB->getUniversalId() << std::endl;
    }

  }

  OutFile << "Checking " << NameOfTheSet << " content ended" << std::endl;

  OutFile.close();
  
}
