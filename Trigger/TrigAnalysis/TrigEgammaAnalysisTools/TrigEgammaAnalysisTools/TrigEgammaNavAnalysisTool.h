// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
        ~TrigEgammaNavAnalysisTool() {};

        StatusCode childInitialize();
        StatusCode childBook();
        StatusCode childExecute();
        StatusCode childFinalize();
    protected:

    private:
        /*! Method to book histograms for each trigger */
        void bookPerSignature(const std::string);
        /*! Method to book histograms for each MaM category */
        void bookPerCategory(const std::string);

        unsigned int        m_eventCounter;
};
#endif
