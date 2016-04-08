################################################################################
# Package: DetectorStatus
################################################################################

# Declare the package name:
atlas_subdir( DetectorStatus )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities )

# Component(s) in the package:
atlas_add_library( DetectorStatusLib
                   src/*.cxx
                   PUBLIC_HEADERS DetectorStatus
                   LINK_LIBRARIES AthenaBaseComps DetDescrConditions GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities )

atlas_add_component( DetectorStatus
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DetDescrConditions GaudiKernel StoreGateLib SGtests AthenaPoolUtilities DetectorStatusLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/DetStatusSvc_CondDB.py share/DetStatusSvc_ToFileMetaData.py )
atlas_install_scripts( share/detStatus_set.py share/detStatus_query.py share/detStatus_upload.py share/detStatus_merge.py )

