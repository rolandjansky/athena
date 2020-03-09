/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigEgammaAnalysisBaseTool_H
#define ITrigEgammaAnalysisBaseTool_H

#include "StoreGate/ReadHandleKey.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaInfo.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/Egamma.h"
#include <utility>


class ITrigEgammaAnalysisBaseTool : virtual public asg::IAsgTool {
  
  ASG_TOOL_INTERFACE(ITrigEgammaAnalysisBaseTool)

  public:
  
    virtual StatusCode initialize()=0;
    virtual StatusCode execute(const EventContext& ctx) const =0;
    virtual StatusCode finalize()=0;
    
    virtual StatusCode childInitialize() = 0;
    virtual StatusCode childExecute( const EventContext& ctx ) const = 0;
    virtual StatusCode childFinalize() = 0;
    



    virtual StatusCode toolExecute(const EventContext& ctx, const std::string,const TrigInfo, 
                                   std::vector<std::pair< const xAOD::Egamma*,const TrigEgammaMatchingUtils::Element>>) const = 0;


    virtual void setGMTools( ToolHandleArray<GenericMonitoringTool> & )=0;
    virtual void setAvgmu( float )=0;
    virtual void setDetail(bool)=0;
    virtual void setTP(bool)=0;
};   

#endif
