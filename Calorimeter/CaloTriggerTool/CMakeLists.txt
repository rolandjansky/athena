# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTriggerTool )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( CaloTriggerToolLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloTriggerTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloIdentifier GaudiKernel Identifier StoreGateLib TrigT1CaloCalibConditions
                   PRIVATE_LINK_LIBRARIES LArCablingLib LArIdentifier ${ROOT_LIBRARIES} )

atlas_add_component( CaloTriggerTool
                     src/components/*.cxx
                     LINK_LIBRARIES CaloTriggerToolLib )

atlas_add_dictionary( CaloTriggerToolDict
                      CaloTriggerTool/CaloTriggerToolDict.h
                      CaloTriggerTool/selection.xml
                      LINK_LIBRARIES CaloTriggerToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

