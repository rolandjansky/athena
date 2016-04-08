################################################################################
# Package: H6G4Sim
################################################################################

# Declare the package name:
atlas_subdir( H6G4Sim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloCnv/CaloDetMgrDetDescrCnv
                          Calorimeter/CaloCnv/CaloIdCnv
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/EventSelectorAthenaPool
                          Event/EventAthenaPool
                          Generators/GeneratorModules
                          LArCalorimeter/LArCnv/LArDetMgrDetDescrCnv
                          Simulation/G4SimCnv/G4SimAthenaPOOL
                          Simulation/G4Utilities/G4Field
                          Simulation/G4Utilities/G4PhysicsLists
                          Simulation/G4Utilities/G4UserActions )

# Install files from the package:
atlas_install_python_modules( python/h6prod_getxy.py python/__init__.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( G4macros/*.mac )
atlas_install_scripts( share/cbtG4_trf.py )

