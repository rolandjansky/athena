///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef USERDATASTORETEST_READUSERDATAWITHDATA_H
#define USERDATASTORETEST_READUSERDATAWITHDATA_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IUserDataSvc.h"


namespace UserDataExamples {

  class ReadDataWithUserData
    : public ::AthAlgorithm
  { 
    
  public: 
    /// Constructor with parameters: 
    ReadDataWithUserData( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor: 
    virtual ~ReadDataWithUserData(); 
    
    // Athena algorithm's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();
    
  private: 
    
    /// Default constructor: 
    ReadDataWithUserData();
    
    ServiceHandle<IUserDataSvc> m_userDataSvc; 
    
    std::string m_contName;
    
    //counters:
    unsigned m_events;
    unsigned m_nAttributes;
    
  }; 
}
#endif
