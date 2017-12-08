///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef USERDATASTORETEST_CREATEUSERDATAWITHDATA_H
#define USERDATASTORETEST_CREATEUSERDATAWITHDATA_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IUserDataSvc.h"


namespace UserDataExamples {

  class CreateDataWithUserData
    : public ::AthAlgorithm
  { 
    
  public: 
    /// Constructor with parameters: 
    CreateDataWithUserData( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor: 
    virtual ~CreateDataWithUserData(); 
    
    // Athena algorithm's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();
    
  private: 
    
    /// Default constructor: 
    CreateDataWithUserData();
    
    ServiceHandle<IUserDataSvc> m_userDataSvc; 
    
    std::string m_contName;
    unsigned m_vecSize;
    
    //counters:
    unsigned m_nEvents;
    unsigned m_nClusters;
    unsigned m_nAttributes;
    
    template<typename T>
    bool checkvalues(const T& a, const T& b, const std::string& label);


  }; 
  template<typename T>
  bool CreateDataWithUserData::checkvalues(const T& a, const T& b, const std::string& label) {
    if (a==b) 
      return true;
    else {
      msg(MSG::ERROR) << "Values with label << " << label << " differ!" << endmsg;
      return false;
    }
  }

}
#endif
