################################################################################
# Package: AFP_GlobalRecoTools
################################################################################

# Declare the package name:
atlas_subdir( AFP_GlobalRecoTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel 
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
			  Event/xAOD/xAODForward
			  ForwardDetectors/AFP/AFP_Geometry
			  PRIVATE
			  Control/AthLinks
			  )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Math GenVector )

atlas_add_component( AFP_GlobalRecoTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities GaudiKernel GeneratorObjects xAODForward AthLinks AFP_Geometry)

# Install files from the package:
atlas_install_headers( AFP_GlobalRecoTools )

