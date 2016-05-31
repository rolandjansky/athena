/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HAVE_CHAINREGISTRY_H
#define HAVE_CHAINREGISTRY_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

#include "TrigTauEmulation/HltChain.h"
#include "TrigTauEmulation/IChainRegistry.h"

class ChainRegistry : virtual public IChainRegistry, virtual public asg::AsgTool {
  ASG_TOOL_CLASS(ChainRegistry, IChainRegistry)

  public:

    ChainRegistry(const std::string& name);
    ChainRegistry(const ChainRegistry& other);
    ChainRegistry& operator=(const ChainRegistry&) = delete;

    virtual ~ChainRegistry();

    bool checkChain(const std::string& name);
    HltChain& getChain(const std::string &name);
    void loadDefaultChain(const std::string &name);
    StatusCode addChain(const std::string &name, const HltChain &chain);

    /// Initialize the tool
    virtual StatusCode initialize();

  private:
    HltChainRegistry m_chains;

};


#endif
