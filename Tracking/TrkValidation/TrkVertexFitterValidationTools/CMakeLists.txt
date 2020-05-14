################################################################################
# Package: TrkVertexFitterValidationTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitterValidationTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkValidation/TrkVertexFitterValidationInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkVertexFitterValidationTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel EventPrimitives xAODEventInfo GeneratorObjects TrkTrack TrkTruthData VxVertex )

# Install files from the package:
atlas_install_headers( TrkVertexFitterValidationTools )

