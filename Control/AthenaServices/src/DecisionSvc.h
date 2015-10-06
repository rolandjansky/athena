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
  const std::vector<std::string> * getAcceptAlgs(const std::string& stream) const;
  /// Return list of RequireAlg names
  const std::vector<std::string> * getRequireAlgs(const std::string& stream) const;
  /// Return list of VetoAlg names
  const std::vector<std::string> * getVetoAlgs(const std::string& stream) const;
  /// Return list of Streams
  const std::vector<std::string> * getStreams() const;

private:
  StatusCode interpretAlgMap();

  /// Decode list of Algorithms that this stream accepts
  //StatusCode decodeAcceptAlgs(const std::string& stream, 
  // 	 	                const std::vector<std::string>& acceptNames, 
  //			        std::map<std::string, std::vector<Algorithm*> > * theAlgMap );
  /// Decode list of Algorithms that this stream requires
  //StatusCode decodeRequireAlgs(const std::string& stream, 
  //			         const std::vector<std::string>& requireNames,
  //			         std::map<std::string, std::vector<Algorithm*> > * theAlgMap );
  /// Decode list of Algorithms that this stream is vetoed by
  //StatusCode decodeVetoAlgs(const std::string& stream, 
  //			      const std::vector<std::string>& vetoNames,
  //			      std::map<std::string, std::vector<Algorithm*> > * theAlgMap );


  /// Decode specified list of Algorithms
  StatusCode decodeAlgorithms(const std::string& stream, 
			      const std::vector<std::string>& theNames,
			      std::map<std::string, std::vector<Algorithm*> > * theAlgMap );


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

public:
  /// Test whether this event should be output
  bool isEventAccepted(const std::string& stream ) const;

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvi );

private: 

  /// Maps of streams -- algorithm names' vectors
  std::map<std::string, std::vector<std::string> > m_stream_accept;
  std::map<std::string, std::vector<std::string> > m_stream_require;
  std::map<std::string, std::vector<std::string> > m_stream_veto;

  /// Maps of streams -- Algorithms' object pointers' vectors 
  std::map<std::string, std::vector<Algorithm*> > m_stream_acceptAlgs; 
  std::map<std::string, std::vector<Algorithm*> > m_stream_requireAlgs; 
  std::map<std::string, std::vector<Algorithm*> > m_stream_vetoAlgs; 

  std::vector<std::string> * m_streamNames;
  std::vector<std::string> * m_SacceptAlgNames;
  std::vector<std::string> * m_SrequireAlgNames;
  std::vector<std::string> * m_SvetoAlgNames;

  bool m_calcStats;
  bool m_frozen;
  ServiceHandle<ICutFlowSvc> m_cutflowSvc;

  std::map<std::string, std::vector<unsigned int> > m_stream2Counts;
  unsigned int m_eventCount;
  unsigned int m_badEvents;
  std::map<unsigned int, unsigned int> m_overlapLevelCounts;

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

