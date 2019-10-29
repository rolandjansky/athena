/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IMonitoringGroupLookup.h

#ifndef IMonitoringGroupLookup_H
#define IMonitoringGroupLookup_H

#include "GaudiKernel/INamedInterface.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class IMonitoringGroupLookup : virtual public INamedInterface {
  public:
    /// InterfaceID
    DeclareInterfaceID( IMonitoringGroupLookup, 1, 0 );

    virtual ToolHandle<GenericMonitoringTool> findGroup( const std::string& name ) const = 0;

};

#endif
