/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPCTRIGGERLOGICSIMTOOL_H
#define IRPCTRIGGERLOGICSIMTOOL_H 

#include "GaudiKernel/IAlgTool.h"
#include <vector>

/** Must declare this, with name of interface*/
static const InterfaceID IID_IRpcTriggerLogicSimTool("IRpcTriggerLogicSimTool", 1, 0);

class IRpcTriggerLogicSimTool : virtual public IAlgTool {
        public:
        /**Declared here, and defined below*/
        static const InterfaceID& interfaceID();
        
        /**Pure virtual*/
        virtual bool doTrigger(std::vector<unsigned int>, int) = 0;
};

inline const InterfaceID& IRpcTriggerLogicSimTool::interfaceID()
{ 
        return IID_IRpcTriggerLogicSimTool; 
}

#endif // IRPCTRIGGERLOGICSIMTOOL_H 
