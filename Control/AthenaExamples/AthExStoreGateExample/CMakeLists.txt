################################################################################
# Package: AthExStoreGateExample
################################################################################

# Declare the package name:
atlas_subdir( AthExStoreGateExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/SGTools
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/PileUpTools
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( SGTutorialLib
                   Tutorial/LinkObj.cxx
                   Tutorial/SGRead.cxx
                   Tutorial/SGWrite.cxx
                   PUBLIC_HEADERS AthExStoreGateExample
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel SGTools PileUpToolsLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthContainers AthLinks AthenaBaseComps CxxUtils EventInfo GaudiKernel )

atlas_add_library( SGTutorial
                   Tutorial/SGTutorial_entries.cxx
                   PUBLIC_HEADERS AthExStoreGateExample
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel SGTools PileUpToolsLib StoreGateLib SGtests SGTutorialLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthContainers AthLinks AthenaBaseComps CxxUtils EventInfo GaudiKernel )

atlas_add_component( AthExStoreGateExample
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools TestTools AthContainers AthLinks AthenaBaseComps CxxUtils PileUpToolsLib StoreGateLib SGtests EventInfo GaudiKernel SGTutorialLib SGTutorial )

atlas_add_component( AthExDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools TestTools AthContainers AthLinks AthenaBaseComps CxxUtils PileUpToolsLib StoreGateLib SGtests EventInfo GaudiKernel SGTutorialLib SGTutorial )

atlas_add_dictionary( AthExStoreGateExampleDict
                      AthExStoreGateExample/AthExStoreGateExampleDict.h
                      AthExStoreGateExample/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools TestTools AthContainers AthLinks AthenaBaseComps CxxUtils PileUpToolsLib StoreGateLib SGtests EventInfo GaudiKernel SGTutorialLib SGTutorial )

# Install files from the package:
atlas_install_joboptions( share/StoreGateExample_Gen_jobOptions.txt share/StoreGateExample_Gen_jobOptions.py share/StoreGateExample_Del_jobOptions.py share/StoreGateExample_Reentrant_jobOptions.py share/dflow_jobo.py share/StoreGateHiveExample.py )

