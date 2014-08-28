// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETINTERFACE_IFASTJETINTERFACETOOL_H
#define JETINTERFACE_IFASTJETINTERFACETOOL_H


#include "fastjet/PseudoJet.hh"
#include "AsgTools/IAsgTool.h"

#include <vector>

namespace fastjet
{
  class ClusterSequence;
  class JetDefinition;
  class AreaDefinition;
}

class IFastJetInterfaceTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IFastJetInterfaceTool)

public:

  typedef fastjet::PseudoJet  fjet_t;
  typedef std::vector<fjet_t> fjetlist_t;

  virtual ~IFastJetInterfaceTool() { };

  virtual StatusCode initialize() = 0;
  virtual StatusCode execute(const fjetlist_t& inJets,fjetlist_t& outJets) = 0;
  virtual       fastjet::ClusterSequence* clusterSequence() = 0;
  virtual const fastjet::ClusterSequence* clusterSequence() const = 0;

  virtual const fastjet::JetDefinition* getJetDefinition() const = 0;
  virtual const fastjet::AreaDefinition* getAreaDefinition() const = 0;
  virtual const std::string getAreaDefinitionType() const = 0;

};

/*! @class IFastJetInterfaceTool
 *  @brief General tool configuring @c fastjet functionality. 
 */
#endif
