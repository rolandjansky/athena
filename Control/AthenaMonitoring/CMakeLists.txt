################################################################################
# Package: AthenaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( AthenaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGMon/SGAudCore
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          xAOD/xAODEventInfo
                          Tools/LWHists
                          Trigger/TrigEvent/TrigDecisionInterface )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( AthenaMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS AthenaMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel LumiBlockCompsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel SGAudCore AthenaPoolUtilities EventInfo LWHists )

atlas_add_component( AthenaMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel LumiBlockCompsLib AthenaKernel SGAudCore AthenaPoolUtilities EventInfo LWHists AthenaMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

