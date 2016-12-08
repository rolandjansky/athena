///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecisionSvc.h 
// Header file for class DecisionSvc
// Author: S.Binet<binet@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_DECISIONSVC_H 
#define ATHENASERVICES_DECISIONSVC_H 

/**
 * @class DecisionSvc
 * @brief This implementes the methods for IDecisionSvc
 */

// for size_t
#include <cstddef>

#include <cassert>

// STL includes
#include <string>
#include <vector>
#include <map>

// FrameWork includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IAlgExecStateSvc.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaKernel/IDecisionSvc.h"
#include "AthenaKernel/ICutFlowSvc.h"


class DecisionSvc : virtual public IDecisionSvc, virtual public IIncidentListener,
		    public AthService 
{ 

public: 

  /// Constructor
  DecisionSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~DecisionSvc();

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode start();
  StatusCode finalize();
  //@}


  /// Incident service handle listening for BeginFile and EndFile.
  void handle(const Incident& incident);

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

public:
  /// Add a stream
  StatusCode addStream(const std::string& stream);
  /// Add an algorithm to the list of AcceptAlgs of a stream
  StatusCode addAcceptAlg(const std::string& name,const std::string& stream);
  /// Add an algorithm to the list of RequireAlgs of a stream
  StatusCode addRequireAlg(const std::string& name,const std::string& stream);
  /// Add an algorithm to the list of VetoAlgs of a stream
  StatusCode addVetoAlg(const std::string& name,const std::string& stream);


  /// Return list of AcceptAlg names
  const std::vector<std::string> getAcceptAlgs(const std::string& stream) const;
  /// Return list of RequireAlg names
  const std::vector<std::string> getRequireAlgs(const std::string& stream) const;
  /// Return list of VetoAlg names
  const std::vector<std::string> getVetoAlgs(const std::string& stream) const;
  /// Return list of Streams
  const std::vector<std::string> getStreams() const;

private:
  StatusCode interpretAlgMap();


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

public:
  /// Test whether this event should be output
  bool isEventAccepted(const std::string& stream ) const;
  bool isEventAccepted(const std::string& stream, const EventContext& ) const;

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvi );

private: 

  /// Maps of streams -- algorithm names' vectors
  std::map<std::string, std::vector<std::string> > m_stream_accept;
  std::map<std::string, std::vector<std::string> > m_stream_require;
  std::map<std::string, std::vector<std::string> > m_stream_veto;

  std::vector<std::string> m_streamNames;

  bool m_calcStats;
  bool m_frozen;
  ServiceHandle<ICutFlowSvc> m_cutflowSvc;
  ServiceHandle<IAlgExecStateSvc> m_algstateSvc;

  //std::map<std::string, std::vector<unsigned int> > m_stream2Counts;
  unsigned int m_eventCount;
  //unsigned int m_badEvents;
  //std::map<unsigned int, unsigned int> m_overlapLevelCounts;

  void DeclareToCutFlowSvc();
  StatusCode fillMap(std::map<std::string, std::vector<std::string> >& streamsModeMap, 
                     const std::string& name,
                     const std::string& stream);


  // Avoid coverity warnings.
  DecisionSvc (const DecisionSvc&);
  DecisionSvc& operator= (const DecisionSvc&);

public:

  static const InterfaceID& interfaceID();
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& DecisionSvc::interfaceID() 
{ 
  return IDecisionSvc::interfaceID(); 
}


#endif //> !ATHENASERVICES_DECISIONSVC_H

