/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCORACLE_AMDCDUMPORACLE_H
#define AMDCORACLE_AMDCDUMPORACLE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "AmdcDb/AmdcDb.h"

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
   StatusCode execute(){return StatusCode::SUCCESS;}

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
   void DumpAMDC( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd , int& Kwarn, int& Kchck );

   void DumpHwSwIdMapping( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd ,int& Kwarn, int& Kchck );

   void DumpXtomoData( IRDBAccessSvc* pIRDBAccessSvc , std::string TagAdd ,int& Kwarn, int& Kchck );


   int m_SwitchOff     ; //!< Control execution
 
   int m_WarningLevel  ; //!< Warning Level

   int m_EmergencyOut  ; //!< Optional stop at the end of initialisation

   int m_AsNewTableOn  ; //!< Control sql prod
   int m_UseKeysOn     ; //!< Control sql prod
  
   int m_KountCallsDoIt     ; //!< Kount calls to DoIt
  
   ServiceHandle<IRDBAccessSvc> m_amdcDb;  //!< Pointer to AmdcDb

   StatusCode DoIt();
  
};

#endif

