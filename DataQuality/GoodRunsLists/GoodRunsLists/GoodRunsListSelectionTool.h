/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_GoodRunsListSelectionTool_H
#define GoodRunsLists_GoodRunsListSelectionTool_H

/** @file GoodRunsListSelectionTool.h
 *  @brief This file contains the class definition for the GoodRunsListSelectionTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "GoodRunsLists/IGoodRunsListSelectionTool.h"
#include "GoodRunsLists/RegularFormula.h"
#include <AsgTools/AsgTool.h>

#include <vector>
#include <string>
#include <map>

//typedef std::pair< std::vector<std::string>, std::vector<std::string> > vvPair; 

/** @class GoodRunsListSelectionTool
 *  @brief This file contains the class definition for the GoodRunsListSelectionTool class.
 **/

class TFormula;

#ifndef ROOTCORE
class EventInfo;
#endif

namespace Root {
  class TGRLCollection;
  class TGoodRunsListReader;
}

class IDetStatusSvc;

class GoodRunsListSelectionTool : virtual public IGoodRunsListSelectionTool, virtual public asg::AsgTool
{
ASG_TOOL_CLASS( GoodRunsListSelectionTool, IGoodRunsListSelectionTool )

 public:   
GoodRunsListSelectionTool (const std::string& name);
  virtual ~GoodRunsListSelectionTool();

  /// Initialize AlgTool
  StatusCode initialize();
  /// called for each event by EventSelector to decide if the event should be passed
#ifndef ROOTCORE
  bool passEvent(const EventInfo* pEvent) ;
#endif

  virtual bool passRunLB( const xAOD::EventInfo& event,
                           const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
                           const std::vector<std::string>& brlnameVec=std::vector<std::string>()) ;
  /// called for each event by GoodRunsListSelectorAlg to decide if the event should be passed
  bool passRunLB( int runNumber, int lumiBlockNr,
                  const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
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
#ifndef ROOTCORE
  std::vector<std::string> m_dqflagsqueryVec;
#endif

  Root::TGRLCollection* m_grlcollection;
  Root::TGRLCollection* m_brlcollection;

  Root::TGoodRunsListReader* m_reader;

  std::map< std::string,TFormula* > m_dqformula;

  int  m_boolop;
  bool m_passthrough;
#ifndef ROOTCORE
  bool m_usecool;
#endif
  bool m_verbose;
  bool m_rejectanybrl;
#ifndef ROOTCORE
  const IDetStatusSvc* m_detstatussvc;
#endif

  std::map< std::string, vvPair > m_registry;

  std::string  m_runrangeexpr;
  Root::RegularFormula m_inrunrange;
};

#endif

