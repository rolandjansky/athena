# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMonitoringEvent )

# Component(s) in the package:
atlas_add_library( TrigMonitoringEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigMonitoringEvent
                   LINK_LIBRARIES AthContainers AthenaKernel TrigSerializeCnvSvcLib
                   PRIVATE_LINK_LIBRARIES TrigNavigationLib )

atlas_add_sercnv_library( TrigMonitoringEventSerCnv
                          FILES TrigMonitoringEvent/TrigMonEvent.h TrigMonitoringEvent/TrigMonEventCollection.h TrigMonitoringEvent/TrigMonConfig.h TrigMonitoringEvent/TrigMonConfigCollection.h
                          LINK_LIBRARIES TrigMonitoringEvent )

atlas_add_dictionary( TrigMonitoringEventDict
                      TrigMonitoringEvent/TrigMonitoringEventDict.h
                      TrigMonitoringEvent/selection.xml
                      LINK_LIBRARIES TrigMonitoringEvent )
