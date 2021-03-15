#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

# Declare the package
atlas_subdir(TrigT1CaloFexPerf)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist Matrix RIO pthread)

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
  LINK_LIBRARIES GaudiKernel ${ROOT_LIBRARIES} TrigT1CaloFexPerfLib TrigAnalysisInterfaces JetInterface JetCalibToolsLib xAODEventInfo CaloEvent CaloInterfaceLib TileEvent xAODTrigL1Calo xAODTrigCalo xAODMissingET xAODJet xAODBase xAODCore CaloDetDescrLib CaloIdentifier AthenaBaseComps AthAnalysisBaseCompsLib GaudiKernel LArToolsLib CaloTriggerToolLib PathResolver xAODTrigger 
)	      

# Install files from the package
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
