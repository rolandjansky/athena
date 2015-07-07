/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_IGoodRunsListSelectionTool_H
#define GoodRunsLists_IGoodRunsListSelectionTool_H

#include "AsgTools/IAsgTool.h"
#include <vector>
#include <string>
#include <map>

#include <xAODEventInfo/EventInfo.h>

typedef std::pair< std::vector<std::string>, std::vector<std::string> > vvPair;

namespace Root {
  class TGRLCollection;
}

class IGoodRunsListSelectionTool : virtual public asg::IAsgTool {

ASG_TOOL_INTERFACE(IGoodRunsListSelectionTool)

public:

   /// DQ Test of run,LBs combination.
virtual bool passRunLB( const xAOD::EventInfo& event,
                           const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
                           const std::vector<std::string>& brlnameVec=std::vector<std::string>() ) = 0 ;
   virtual bool passRunLB( int runNumber, int lumiBlockNr,
                           const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
                           const std::vector<std::string>& brlnameVec=std::vector<std::string>() ) = 0 ;
   /// register grl/brl combination 
   virtual bool registerGRLSelector(const std::string& name, 
                            const std::vector<std::string>& grlnameVec, const std::vector<std::string>& brlnameVec) = 0 ;
   /// get GRL selector registry
   virtual const std::map< std::string, vvPair >& getGRLSelectorRegistry() = 0 ;

   /// get grl/brl collection
   virtual const Root::TGRLCollection* getGRLCollection() const = 0 ;
   virtual const Root::TGRLCollection* getBRLCollection() const = 0 ;
};

#endif // GoodRunsLists_IGoodRunsListSelectionTool_H

