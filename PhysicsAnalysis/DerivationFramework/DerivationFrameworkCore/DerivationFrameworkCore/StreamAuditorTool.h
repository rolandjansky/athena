/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StreamAuditorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_STREAMAUDITORTOOL_H
#define DERIVATIONFRAMEWORK_STREAMAUDITORTOOL_H 1

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

namespace DerivationFramework {

  static const InterfaceID IID_StreamAuditorTool("StreamAuditorTool", 1, 0);
  class StreamAuditorTool : public AthAlgTool,
			    virtual public IIncidentListener {
    
  public: 
    /** Constructor with parameters */
    StreamAuditorTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~StreamAuditorTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    static const InterfaceID& interfaceID() { return IID_StreamAuditorTool;};
    
    /** Register name of an algorithm */
    void registerAlgorithmName(const std::string&);
    void eventAccepted(const std::string&);    

    // Incident handle
    void handle(const Incident& inc); 

  private:
    ServiceHandle<IIncidentSvc> m_incidentSvc;
    std::map<std::string,unsigned int> m_algNames;
    std::map<std::pair<std::string,std::string> , unsigned int> m_overlapMap;
    unsigned int m_totalEvents = 0U;
    //m_algCounts,
    //m_overlapCounts,



  }; 
  
}


#endif
