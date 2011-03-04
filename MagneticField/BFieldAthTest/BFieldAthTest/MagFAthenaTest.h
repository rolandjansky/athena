/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MagFAthenaTest_H
#define MagFAthenaTest_H

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  15/06/00
//
// The MagFAthenaTest class
// is an Algorithm class to test MagFieldAthenaSvc
//
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "BFieldAth/MagFieldAthena.h"

class IMagFieldAthenaSvc;
class MagFAthenaTest:public Algorithm {
public:
  MagFAthenaTest (const std::string& name, ISvcLocator* pSvcLocator);
  ~MagFAthenaTest();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode callback( IOVSVC_CALLBACK_ARGS );

private:
   std::vector<std::string> m_MenbersName ;//!< Names of Subalgs
   std::vector<std::string> m_MenbersType ;//!< Types of Subalgs
   std::vector<Algorithm*>    m_ListOfMenbers ;//!< Pointers on Subalgs

   ServiceHandle<IMagFieldAthenaSvc> p_MagFieldAthenaSvc ; //!< Pointer On MagFieldAthenaSvc

//Functions
// Set Access to Basic Muon Svc
   void SetAccessBasicSvc();

};

#endif

