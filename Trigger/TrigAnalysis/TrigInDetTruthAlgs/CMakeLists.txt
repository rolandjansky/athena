################################################################################
# Package: TrigInDetTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          Generators/GenzModuleEvent
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkTruthData
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( TrigInDetTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} TrigInDetEvent AthenaBaseComps GaudiKernel GenzModuleEvent InDetIdentifier InDetSimData InDetPrepRawData TrkTruthData TrigInDetTruthEvent )

# Install files from the package:
atlas_install_headers( TrigInDetTruthAlgs )
atlas_install_python_modules( python/__init__.py python/TrigInDetTruthAlgsConfig.py )
atlas_install_joboptions( share/jobOfragment*.py )

