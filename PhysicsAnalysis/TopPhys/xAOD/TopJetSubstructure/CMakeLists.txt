# Auto-generated on: 2017-03-08 14:47:37.639679

# Declare the name of this package:
atlas_subdir( TopJetSubstructure None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODPrimitives
                          xAODBase
                          GeoPrimitives
                          xAODCaloEvent
                          xAODJet
                          xAODBTagging
                          EventPrimitives
                          xAODTracking
                          CaloGeoHelpers
                          xAODEgamma)


# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# This package uses FastJet:
find_package( FastJet )
find_package( FastJetContrib )

# Custom definitions needed for this package:
add_definitions( -g )

# Build a library that other components can link against:
atlas_add_library( TopJetSubstructure Root/*.cxx Root/*.h Root/*.icc
                   TopJetSubstructure/*.h TopJetSubstructure/*.icc TopJetSubstructure/*/*.h
                   TopJetSubstructure/*/*.icc 
                   PUBLIC_HEADERS TopJetSubstructure
                   LINK_LIBRARIES xAODPrimitives
                                  xAODBase
                                  GeoPrimitives
                                  xAODCaloEvent
                                  xAODJet
                                  xAODBTagging
                                  EventPrimitives
                                  xAODTracking
                                  CaloGeoHelpers
                                  xAODEgamma
                                  ${ROOT_LIBRARIES}
				  ${FASTJET_LIBRARIES} 
				  ${FASTJETCONTRIB_LIBRARIES}	                                  
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		                ${FASTJET_INCLUDE_DIRS} 
				${FASTJETCONTRIB_INCLUDE_DIRS})                                

# Install data files from the package:
atlas_install_data( share/* )

