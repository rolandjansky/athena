################################################################################
# Package: JetUncertainties
################################################################################

# Declare the package name:
atlas_subdir( JetUncertainties )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          Reconstruction/Jet/JetCPInterfaces
                          PRIVATE
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetUncertaintiesLib
                   Root/*.cxx
                   PUBLIC_HEADERS JetUncertainties
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODEventInfo xAODJet PATInterfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCore xAODTracking GaudiKernel PathResolver )

atlas_add_component( JetUncertainties
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet PATInterfaces xAODCore xAODTracking GaudiKernel PathResolver JetUncertaintiesLib )

atlas_add_dictionary( JetUncertaintiesDict
                      JetUncertainties/JetUncertaintiesDict.h
                      JetUncertainties/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet PATInterfaces xAODCore xAODTracking GaudiKernel PathResolver JetUncertaintiesLib )

# Install files from the package:
atlas_install_generic( share/JES_2011/Final/*.config
                       DESTINATION share/JetUncertainties/JES_2011/Final
                       EXECUTABLE )
atlas_install_generic( share/JES_2012/Moriond2013/*.config
                       DESTINATION share/JetUncertainties/JES_2012/Moriond2013
                       EXECUTABLE )
atlas_install_generic( share/JES_2012/Final/*.config
                       DESTINATION share/JetUncertainties/JES_2012/Final
                       EXECUTABLE )
atlas_install_generic( share/JES_2015/Prerec/*.config
                       DESTINATION share/JetUncertainties/JES_2015/Prerec
                       EXECUTABLE )
atlas_install_generic( share/JES_2015/Moriond2016/*.config
                       DESTINATION share/JetUncertainties/JES_2015/Moriond2016
                       EXECUTABLE )
atlas_install_generic( share/JES_2015/ICHEP2016/*.config
                       DESTINATION share/JetUncertainties/JES_2015/ICHEP2016
                       EXECUTABLE )
atlas_install_generic( share/UJ_2012/*.config
                       DESTINATION share/JetUncertainties/UJ_2012
                       EXECUTABLE )
atlas_install_generic( share/UJ_2015/Prerec/*.config
                       DESTINATION share/JetUncertainties/UJ_2015/Prerec
                       EXECUTABLE )
atlas_install_generic( share/UJ_2015/Moriond2016/*.config
                       DESTINATION share/JetUncertainties/UJ_2015/Moriond2016
                       EXECUTABLE )
atlas_install_generic( share/UJ_2015/ICHEP2016/*.config
                       DESTINATION share/JetUncertainties/UJ_2015/ICHEP2016
                       EXECUTABLE )
atlas_install_generic( share/analysisInputs/*.root
                       DESTINATION share/JetUncertainties/analysisInputs
                       EXECUTABLE )

