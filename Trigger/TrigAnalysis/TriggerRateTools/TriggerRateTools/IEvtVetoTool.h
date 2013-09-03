/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEvtVetoTool_H
#define IEvtVetoTool_H

#include "GaudiKernel/IAlgTool.h"

#include <vector>
#include <string> 

static const InterfaceID IID_IEvtVetoTool("IEvtVetoTool", 1, 0);

class IEvtVetoTool : virtual public IAlgTool {

    public:

        //EvtVetoTool(const std::string & type, const std::string & name, const IInterface* parent); 
        //~EvtVetoTool();

        static const InterfaceID& interfaceID( ) { return IID_IEvtVetoTool; };

        virtual bool isPassed() = 0;

};

#endif
