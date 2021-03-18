#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

# Declare the package
atlas_subdir(TrigT1CaloFexPerf)

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist RIO )

atlas_add_library(
  TrigT1CaloFexPerfLib
  Root/*.cxx
  PUBLIC_HEADERS TrigT1CaloFexPerf
  LINK_LIBRARIES AthenaKernel
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} 
)

atlas_add_component(
  TrigT1CaloFexPerf
  src/components/*.cxx src/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel CaloDetDescrLib CaloEvent CaloIdentifier CxxUtils GaudiKernel PathResolver StoreGateLib TileEvent TrigT1CaloFexPerfLib xAODBase xAODCore xAODTrigCalo xAODTrigL1Calo xAODTrigger )

# Install files from the package
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
