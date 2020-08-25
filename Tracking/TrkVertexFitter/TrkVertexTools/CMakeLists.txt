################################################################################
# Package: TrkVertexTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Event/EventPrimitives
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkVertexTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel xAODTracking GaudiKernel TrkVertexFitterInterfaces EventPrimitives 
		     GeneratorObjects TrkEventPrimitives VxVertex BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( TrkVertexTools )

