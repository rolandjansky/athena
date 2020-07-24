# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloEvent )

# Component(s) in the package:
atlas_add_library( TrigT1CaloEventLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloEvent
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel GaudiKernel TrigT1Interfaces )

atlas_add_dictionary( TrigT1CaloEventDict
                      TrigT1CaloEvent/TrigT1CaloEventDict.h
                      TrigT1CaloEvent/selection.xml
                      LINK_LIBRARIES TrigT1CaloEventLib )
