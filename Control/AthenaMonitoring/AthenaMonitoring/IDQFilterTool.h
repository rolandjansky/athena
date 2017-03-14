/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDQFILTERTOOL_H
#define IDQFILTERTOOL_H

#include "GaudiKernel/IAlgTool.h"

// Interface class for tools to be used in DQ tools to filter input
// @author Peter Onyisi <ponyisi@cern.ch>

/** Must declare this, with name of interface*/
static const InterfaceID IID_IDQFilterTool("IDQFilterTool", 1, 0);

class IDQFilterTool : virtual public IAlgTool {
        public:
        /**Declared here, and defined below*/
        static const InterfaceID& interfaceID();
        
        /**Pure virtual*/
        virtual bool accept() const = 0;
};

inline const InterfaceID& IDQFilterTool::interfaceID()
{ 
        return IID_IDQFilterTool; 
}

#endif //IDQFILTERTOOL_H
