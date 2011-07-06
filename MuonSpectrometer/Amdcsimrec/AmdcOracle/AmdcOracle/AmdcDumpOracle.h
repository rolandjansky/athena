/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcDumpOracle_H
#define AmdcDumpOracle_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "AmdcOracle/AmdcOracleMisc.h"

class IRDBAccessSvcWithUpdate;
class IRDBAccessSvc;

  /**
   @class AmdcDumpOracle

   This class checks the content of Oracle tables
   
  @author samusog@cern.ch
  
  */

class AmdcDumpOracle:public AthAlgorithm {
public:
  AmdcDumpOracle (const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

   StatusCode DoItCallback(IOVSVC_CALLBACK_ARGS);

private:
///////////////////////////////////

   void DumpASZT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpISZT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpAPTP( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpALMN( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpALIN( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWMDT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWSPA( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWSUP( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWCHV( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWCMI( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWCRO( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWLBI( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWDED( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpASMP( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpDBAM( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWCSC( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpATLN( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpGGLN( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWTGC( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpAWLN( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpWRPC( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpACUT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpATYP( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpAGDD( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpAMDC( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpABRT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpAECT( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpECST( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpFEET( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpGENV( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpJDSH( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpJFSH( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpJTSH( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );
   void DumpMBAP( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );

   void DumpAgddXMLVariables( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd ,int& Kwarn, int& Kchck );

   void DumpHwSwIdMapping( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd ,int& Kwarn, int& Kchck );

   void DumpXtomoData( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd ,int& Kwarn, int& Kchck );


   bool m_IRDBAccessSvcWithUpdateUpdatedSvcDONE     ; //!< status of p_IRDBAccessSvcWithUpdate
   
   
   int m_SwitchOff     ; //!< Control execution
 
   int m_WarningLevel  ; //!< Warning Level

   int m_EmergencyOut  ; //!< Optional stop at the end of initialisation

   int m_AsNewTableOn  ; //!< Control sql prod
   int m_UseKeysOn     ; //!< Control sql prod
  
   int m_KountCallsDoIt     ; //!< Kount calls to DoIt
  
   ServiceHandle<IRDBAccessSvcWithUpdate> p_IRDBAccessSvcWithUpdate;  //!< Pointer On IRDBAccessSvcWithUpdate

   StatusCode regFcnDoIt();
   StatusCode DoIt();
  
};

#endif

