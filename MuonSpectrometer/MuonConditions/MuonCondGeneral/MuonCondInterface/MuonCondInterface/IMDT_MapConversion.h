/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_MAPCONVERSION_H
#define MUONCONDINTERFACE_IMDT_MAPCONVERSION_H

// Includes for Gaudi
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
//**********************************************************
//*
//* Tool to convert the online Id in Offline Id for the MDT
//*
//*********************************************************

class IMDT_MapConversion : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMDT_MapConversion("IMDT_MapConversion", 1, 0);

        return IID_IMDT_MapConversion;
    };

    virtual const Identifier& ConvertToOffline(std::string_view OnlineId, bool quiet = false) const = 0;
};

#endif
