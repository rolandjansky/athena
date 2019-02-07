/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ATHENASERVICES_DECISIONSVC_H
#define ATHENASERVICES_DECISIONSVC_H 

// STL includes
#include <string>
#include <vector>
#include <map>

// FrameWork includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IAlgExecStateSvc.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IDecisionSvc.h"
#include "AthenaKernel/ICutFlowSvc.h"


/**
 * @class DecisionSvc
 * @brief This implementes the methods for IDecisionSvc
 * @author S.Binet<binet@cern.ch>
 * @author B.Radics<radbal@cern.ch>
 */
class DecisionSvc : public extends<AthService, IDecisionSvc, IIncidentListener>
{ 

public: 

  /// Constructor
  DecisionSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~DecisionSvc();

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  virtual StatusCode finalize() override;
  //@}


  /// Incident service handle listening for BeginFile and EndFile.
  virtual void handle(const Incident& incident) override;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

public:
  /// Add a stream
  virtual StatusCode addStream(const std::string& stream) override;
  /// Add an algorithm to the list of AcceptAlgs of a stream
  virtual StatusCode addAcceptAlg(const std::string& name,const std::string& stream) override;
  /// Add an algorithm to the list of RequireAlgs of a stream
  virtual StatusCode addRequireAlg(const std::string& name,const std::string& stream) override;
  /// Add an algorithm to the list of VetoAlgs of a stream
  virtual StatusCode addVetoAlg(const std::string& name,const std::string& stream) override;


  /// Return list of AcceptAlg names
  virtual const std::vector<std::string> getAcceptAlgs(const std::string& stream) const override;
  /// Return list of RequireAlg names
  virtual const std::vector<std::string> getRequireAlgs(const std::string& stream) const override;
  /// Return list of VetoAlg names
  virtual const std::vector<std::string> getVetoAlgs(const std::string& stream) const override;
  /// Return list of Streams
  virtual const std::vector<std::string> getStreams() const override;

private:
  StatusCode interpretAlgMap();


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

public:
  /// Test whether this event should be output
  virtual bool isEventAccepted(const std::string& stream ) const override;
  virtual bool isEventAccepted(const std::string& stream, const EventContext& ) const override;

private:

  /// Maps of streams -- algorithm names' vectors
  std::map<std::string, std::vector<std::string> > m_stream_accept;
  std::map<std::string, std::vector<std::string> > m_stream_require;
  std::map<std::string, std::vector<std::string> > m_stream_veto;

  std::vector<std::string> m_streamNames;

  Gaudi::Property<bool> m_calcStats{this, "CalcStats", false};
  bool m_frozen{false};
  ServiceHandle<ICutFlowSvc> m_cutflowSvc;
  ServiceHandle<IAlgExecStateSvc> m_algstateSvc;

  //std::map<std::string, std::vector<unsigned int> > m_stream2Counts;
  //unsigned int m_eventCount{0};
  //unsigned int m_badEvents;
  //std::map<unsigned int, unsigned int> m_overlapLevelCounts;

  void DeclareToCutFlowSvc();
  StatusCode fillMap(std::map<std::string, std::vector<std::string> >& streamsModeMap, 
                     const std::string& name,
                     const std::string& stream);
}; 

#endif //> !ATHENASERVICES_DECISIONSVC_H
