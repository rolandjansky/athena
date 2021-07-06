/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaMonitorSuperClusterAlgorithm_H
#define egammaMonitorSuperClusterAlgorithm_H

#include "egammaMonitorBaseAlgorithm.h"

class egammaRec;

class egammaMonitorSuperClusterAlgorithm: public egammaMonitorBaseAlgorithm 
{

  public:

    egammaMonitorSuperClusterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;

  protected:

    
    void fillSuperClusterQuantities(const EventContext& ctx) const;
    
    
  
  private:
    SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey{
      this,
       "InputEgammaRecContainerName",
       "egammaRecCollection",
       "input egammaRec container"
    };

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
 


#endif
