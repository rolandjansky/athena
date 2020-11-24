## automatically generated CMakeLists.txt file

# Declare the package
atlas_subdir( TrigL0GepPerf )

# Declare external dependencies ... default here is to include ROOT
find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist )

# Declare public and private dependencies
# Public dependencies are anything that appears in the headers in public include dir
# Private is anything else

# An example is included
atlas_depends_on_subdirs(
    PUBLIC
    Control/AthenaBaseComps
    Control/AthAnalysisBaseComps
    GaudiKernel
    Event/xAOD/xAODEventInfo
    Event/xAOD/xAODTrigL1Calo
    Event/xAOD/xAODTrigCalo
    Event/xAOD/xAODCaloEvent
    Event/xAOD/xAODTrigger
    Event/xAOD/xAODBase
    Event/xAOD/xAODCore
    Event/xAOD/xAODEventInfo
    Event/xAOD/xAODJet
    Event/xAOD/xAODEgamma
    Event/xAOD/xAODMissingET
    Event/xAOD/xAODTruth

    # new:
    Calorimeter/CaloInterface
    Calorimeter/CaloIdentifier
    Event/xAOD/xAODCaloEvent
    Reconstruction/Jet/JetRec
    LArCalorimeter/LArROD

    InnerDetector/InDetExample/InDetSLHC_Example

    PRIVATE
    # Control/AthAnalysisBaseComps
)

# Declare package as a library
# Note the convention that library names get "Lib" suffix
# Any package you add to dependencies above, you should add
# to LINK_LIBRARIES line below (see the example)
#atlas_add_library( TrigL0GepPerfLib src/*.cxx
#                   PUBLIC_HEADERS TrigL0GepPerf
#                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                   LINK_LIBRARIES ${ROOT_LIBRARIES}
#                                    #AthAnalysisBaseCompsLib
#)

# if you add components (tools, algorithms) to this package
# uncomment the next lines
atlas_add_component( TrigL0GepPerf 
		     src/*.cxx
		     src/components/*.cxx
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthAnalysisBaseCompsLib GaudiKernel AthContainers TrigAnalysisInterfaces JetInterface JetCalibToolsLib xAODEventInfo CaloEvent xAODTrigL1Calo xAODTruth xAODMissingET xAODJet xAODEgamma xAODTrigCalo xAODBase xAODCore CaloDetDescrLib CaloIdentifier AthenaBaseComps AthAnalysisBaseCompsLib GaudiKernel LArToolsLib CaloTriggerToolLib PathResolver TrigT1Interfaces xAODTrigger TrigDecisionToolLib TriggerMatchingToolLib AsgTools IsolationSelectionLib JetEDM xAODJet

                     #CaloTools                     

		     PRIVATE_LINK_LIBRARIES TrigEFMissingETLib
)

# Install python modules, joboptions, and share content
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )
atlas_install_headers( TrigL0GepPerf )
# You can access your data from code using path resolver, e.g.
# PathResolverFindCalibFile("TrigL0GepPerf/file.txt")



