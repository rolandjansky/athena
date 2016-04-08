################################################################################
# Package: TrigCollQuery
################################################################################

# Declare the package name:
atlas_subdir( TrigCollQuery )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TrigCollQueryLib
                   src/TrigCollQuery.cxx
                   src/TrigCollQueryTool.cxx
                   PUBLIC_HEADERS TrigCollQuery
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} )

atlas_add_component( TrigCollQuery
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrigCollQueryLib )

atlas_add_executable( runCollUtil
                      src/runCollUtil.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrigCollQueryLib )

atlas_add_executable( decodeTriggerQuery
                      src/decodeTriggerQuery.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrigCollQueryLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

