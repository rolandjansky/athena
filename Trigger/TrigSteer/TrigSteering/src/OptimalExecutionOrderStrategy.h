/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERING_OptimaExecutionOrderStrategy_h
#define TRIGSTEERING_OptimaExecutionOrderStrategy_h
#include <vector>
#include <string>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSteering/IExecutionOrderStrategy.h"


namespace TrigConf {
  class ITrigConfigSvc;
}

namespace HLT {

  class Bag {
  public:
    virtual ~Bag() {}
    
    virtual bool prepare(const HLT::SteeringChain* ) = 0;
    virtual bool insert(const HLT::SteeringChain* );
    void flush(std::vector<HLT::SteeringChain*>& chains);
  protected:
    std::vector<const HLT::SteeringChain*> m_content;
    
    std::set<const HLT::SteeringChain*> m_matching;
  };
  
  /**
   * @brief The implementation of optimal execution strategy.
   * 
   **/
  class OptimalExecutionOrderStrategy: public AthAlgTool, public virtual IExecutionOrderStrategy {
  public:
    OptimalExecutionOrderStrategy(const std::string& name, const std::string& type,
				  const IInterface* parent);
    virtual ~OptimalExecutionOrderStrategy() {} 
    
    StatusCode initialize();
    StatusCode finalize();

    
    virtual StatusCode prepare(std::vector<HLT::SteeringChain*>& all_configured_chains); //!< return FAILURE when there is chain which does not match any of the sorting categories
    virtual void apply(std::vector<SteeringChain*>& active_chains) const; //!< actual reordering is done inside
  private:
    std::vector<std::string> m_orderSpecification;
    mutable std::vector<Bag*> m_orderingBags;
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

  };
}
#endif
