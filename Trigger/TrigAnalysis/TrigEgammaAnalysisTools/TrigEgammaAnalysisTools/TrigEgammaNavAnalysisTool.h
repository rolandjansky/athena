// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavAnalysisTool_H
#define TrigEgammaNavAnalysisTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
class TrigEgammaNavAnalysisTool : public TrigEgammaNavBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool
{
    ASG_TOOL_CLASS(TrigEgammaNavAnalysisTool, ITrigEgammaAnalysisBaseTool)
    public:
        TrigEgammaNavAnalysisTool( const std::string& myname );
        virtual ~TrigEgammaNavAnalysisTool() {};
        virtual StatusCode childInitialize() override;
        virtual StatusCode childExecute( const EventContext& ctx) const override;
        virtual StatusCode childFinalize() override;

};
#endif
