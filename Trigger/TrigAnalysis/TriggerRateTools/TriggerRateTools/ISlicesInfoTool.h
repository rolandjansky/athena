/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISlicesInfoTool_H
#define ISlicesInfoTool_H

#include "GaudiKernel/IAlgTool.h"

#include <vector>
#include <string> 

static const InterfaceID IID_ISlicesInfoTool("ISlicesInfoTool", 1, 0);

class ISlicesInfoTool : virtual public IAlgTool {

    public:

        static const InterfaceID& interfaceID( ) { return IID_ISlicesInfoTool; };

        virtual StatusCode bookSlices() = 0;
        virtual StatusCode fillSlices() = 0;

};

#endif
