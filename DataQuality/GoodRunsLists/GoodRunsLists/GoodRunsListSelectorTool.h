/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_GoodRunsListSelectorTool_H
#define GoodRunsLists_GoodRunsListSelectorTool_H

/** @file GoodRunsListSelectorTool.h
 *  @brief This file contains the class definition for the GoodRunsListSelectorTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "GoodRunsLists/RegularFormula.h"

#include <vector>
#include <string>
#include <map>

//typedef std::pair< std::vector<std::string>, std::vector<std::string> > vvPair; 

/** @class GoodRunsListSelectorTool
 *  @brief This file contains the class definition for the GoodRunsListSelectorTool class.
 **/

class TFormula;
class EventInfo;

namespace Root {
  class TGRLCollection;
  class TGoodRunsListReader;
}

class IDetStatusSvc;
class StoreGateSvc;

class GoodRunsListSelectorTool : virtual public AthAlgTool, virtual public IGoodRunsListSelectorTool, virtual public IAthenaEvtLoopPreSelectTool
{
 public:    
  GoodRunsListSelectorTool( const std::string&, const std::string&, const IInterface* );
  virtual ~GoodRunsListSelectorTool();

  /// To allow access to the IGoodRunsListSelectorTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  /// Initialize AlgTool
  StatusCode initialize();
  /// called for each event by EventSelector to decide if the event should be passed
  bool passEvent(const EventInfo* pEvent) ;
  /// Finalize AlgTool
  StatusCode finalize();

  ///// to resolve possible conflicts with IProperty::interfaceID()
  //static const InterfaceID& interfaceID() { 
  //  return IAthenaEvtLoopPreSelectTool::interfaceID();
  //  //return IGoodRunsListSelectorTool::interfaceID(); 
  //}

  /// called for each event by GoodRunsListSelectorAlg to decide if the event should be passed
  bool passRunLB( int runNumber, int lumiBlockNr,
                  const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
                  const std::vector<std::string>& brlnameVec=std::vector<std::string>() ) ;
  /// called for each event by GoodRunsListSelectorAlg to decide if the event should be passed
  bool passThisRunLB( const std::vector<std::string>& grlnameVec=std::vector<std::string>(), 
                      const std::vector<std::string>& brlnameVec=std::vector<std::string>() ) ;
  /// register grl/brl combination 
  bool registerGRLSelector(const std::string& name, const std::vector<std::string>& grlnameVec, const std::vector<std::string>& brlnameVec);
  /// get GRL selector registry
  const std::map< std::string, vvPair >& getGRLSelectorRegistry() { return m_registry; }

  /// get grl/brl collection
  const Root::TGRLCollection* getGRLCollection() const { return m_grlcollection; } 
  const Root::TGRLCollection* getBRLCollection() const { return m_brlcollection; }

 protected:

  bool fileExists(const char* fileName);

  std::vector<std::string> m_goodrunslistVec;
  std::vector<std::string> m_blackrunslistVec;
  std::vector<std::string> m_dqflagsqueryVec;

  Root::TGRLCollection* m_grlcollection;
  Root::TGRLCollection* m_brlcollection;

  Root::TGoodRunsListReader* m_reader;

  std::map< std::string,TFormula* > m_dqformula;

  int  m_boolop;
  bool m_passthrough;
  bool m_usecool;
  bool m_verbose;
  bool m_rejectanybrl;
  bool m_eventselectormode;

  const IDetStatusSvc* m_detstatussvc;
  StoreGateSvc* m_storeGate;

  std::map< std::string, vvPair > m_registry;

  std::string  m_runrangeexpr;
  Root::RegularFormula m_inrunrange;
};

#endif

