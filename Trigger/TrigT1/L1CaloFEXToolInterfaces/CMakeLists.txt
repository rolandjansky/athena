################################################################################
# Package: L1CaloFEXToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( L1CaloFEXToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps 
			  GaudiKernal
			  )

atlas_add_library( L1CaloFEXToolInterfaces
                   PUBLIC_HEADERS L1CaloFEXToolInterfaces
                   LINK_LIBRARIES AthenaBaseComps CaloEvent xAODTrigL1Calo CaloIdentifier )#GaudiKernal )
