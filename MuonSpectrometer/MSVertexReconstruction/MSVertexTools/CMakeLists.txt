################################################################################
# Package: MSVertexTools
################################################################################

# Declare the package name:
atlas_subdir( MSVertexTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MSVertexTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthContainers StoreGateLib GeoPrimitives Identifier 
                                    EventPrimitives xAODTracking GaudiKernel MSVertexUtils MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData 
                                    TrkParameters TrkExInterfaces MSVertexToolInterfaces FourMomUtils)
