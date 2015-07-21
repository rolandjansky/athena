/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGEGAMMAPHYSVALMONTOOL_H
#define __TRIGEGAMMAPHYSVALMONTOOL_H

#include "TrigEgammaMonTool.h"
class TrigEgammaPhysValMonTool: public TrigEgammaMonTool {
    public:
        TrigEgammaPhysValMonTool( const std::string & type, const std::string & name, const IInterface* parent);
        virtual ~TrigEgammaPhysValMonTool();
        virtual StatusCode initialize();
        virtual StatusCode bookHistograms();
        virtual StatusCode fillHistograms();
        virtual StatusCode procHistograms();

};
#endif
