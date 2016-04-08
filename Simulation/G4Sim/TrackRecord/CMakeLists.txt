################################################################################
# Package: TrackRecord
################################################################################

# Declare the package name:
atlas_subdir( TrackRecord )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          Simulation/HitManagement
                          PRIVATE
                          Database/AtlasSealCLHEP )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrackRecordDict
                      TrackRecord/TrackRecordDict.h
                      TrackRecord/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} SGTools HitManagement )

# Install files from the package:
atlas_install_headers( TrackRecord )

