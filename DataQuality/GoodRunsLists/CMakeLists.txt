################################################################################
# Package: GoodRunsLists
################################################################################

# Declare the package name:
atlas_subdir( GoodRunsLists )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/DetDescrCond/DetectorStatus
                          Event/EventInfo
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS XMLIO XMLParser Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )
find_package( xml )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTCintexLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_root_dictionary( GoodRunsListsLib
                           GoodRunsListsLibDictSource
                           ROOT_HEADERS GoodRunsLists/TLumiBlockRange.h GoodRunsLists/RegularFormula.h GoodRunsLists/TGoodRunsListReader.h GoodRunsLists/TGoodRunsListWriter.h GoodRunsLists/DQHelperFunctions.h Root/LinkDef.h
                           EXTERNAL_PACKAGES xml ROOT )

atlas_add_library( GoodRunsListsLib
                   Root/*.cxx
                   ${GoodRunsListsLibDictSource}
                   PUBLIC_HEADERS GoodRunsLists
                   INCLUDE_DIRS ${XML_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps AthenaKernel xAODEventInfo GaudiKernel ${XML_LIBRARIES} StoreGateLib SGtests DetectorStatusLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventInfo PathResolver )

atlas_add_component( GoodRunsLists
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XML_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XML_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel xAODEventInfo GaudiKernel StoreGateLib SGtests DetectorStatusLib EventInfo PathResolver GoodRunsListsLib )

atlas_add_dictionary( GoodRunsListsDict
                      GoodRunsLists/GoodRunsListsDict.h
                      GoodRunsLists/selection.xml
                      INCLUDE_DIRS ${XML_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XML_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel xAODEventInfo GaudiKernel StoreGateLib SGtests DetectorStatusLib EventInfo PathResolver GoodRunsListsLib )

atlas_add_executable( getLumi
                      Utils/getLumi.cxx
                      INCLUDE_DIRS ${XML_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XML_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel xAODEventInfo GaudiKernel StoreGateLib SGtests DetectorStatusLib EventInfo PathResolver GoodRunsListsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( test/*.xml )
atlas_install_scripts( scripts/define* scripts/create* scripts/merge* scripts/verify* scripts/overlap* scripts/diff* )

