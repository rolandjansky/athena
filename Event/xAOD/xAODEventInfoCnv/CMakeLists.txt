################################################################################
# Package: xAODEventInfoCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODEventInfoCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCnvInterfaces
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          LumiBlock/LumiBlockComps
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( xAODEventInfoCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} xAODCnvInterfaces LumiBlockCompsLib AthenaBaseComps AthenaKernel AthenaPoolUtilities EventInfo xAODEventInfo GaudiKernel )

# Install files from the package:
atlas_install_headers( xAODEventInfoCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

