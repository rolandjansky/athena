################################################################################
# Package: L1CaloFEXSim
################################################################################

# Declare the package name:
atlas_subdir( L1CaloFEXSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps 
			  Trigger/TrigT1/L1CaloFEXToolInterfaces
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODJet  )

atlas_add_library( L1CaloFEXSimLib
                   L1CaloFEXSim/*.h src/*.cxx
                   PUBLIC_HEADERS L1CaloFEXSim
                   LINK_LIBRARIES AthenaBaseComps CaloEvent xAODTrigL1Calo CaloIdentifier xAODTruth xAODJet)

atlas_add_component( L1CaloFEXSim
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps L1CaloFEXSimLib L1CaloFEXToolInterfaces xAODTruth xAODJet)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
