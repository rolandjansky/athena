################################################################################
# Package: DetDescrCondTools
################################################################################

# Declare the package name:
atlas_subdir( DetDescrCondTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Database/APR/FileCatalog
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/PoolSvc
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Hist Tree RIO Core MathCore pthread )

# Component(s) in the package:
atlas_add_component( DetDescrCondTools
                     src/AlignTransTool.cxx
                     src/CoolStrFileSvc.cxx
                     src/CoolHistSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel FileCatalog AthenaPoolUtilities DetDescrConditions Identifier )

# Install files from the package:
atlas_install_headers( DetDescrCondTools )
atlas_install_scripts( share/coolHist_setFileIdentifier.sh share/coolHist_extractFileIdentifier.sh share/coolHist_setReference.py share/coolHist_insertFileToCatalog.py share/coolStrFileSvc_set.py )

