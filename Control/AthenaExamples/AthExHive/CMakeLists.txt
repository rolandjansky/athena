################################################################################
# Package: AthExHive
################################################################################

# Declare the package name:
atlas_subdir( AthExHive )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

atlas_add_library( AthExHiveLib
                   AthExHive/*.h
                   INTERFACE
                   PUBLIC_HEADERS AthExHive
                   LINK_LIBRARIES AthenaKernel GaudiKernel)

# Component(s) in the package:
atlas_add_component( AthExHive
                     src/*.cxx
                     src/condEx/*.cxx
                     src/loopTest/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel AthenaBaseComps xAODEventInfo AthExHiveLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/condDb.txt )

atlas_add_test ( AthExHive_test
   SCRIPT test/AthExHive_test.sh
   PROPERTIES TIMEOUT 300
   )

atlas_add_test ( AthExHiveCond_test
   SCRIPT test/AthExHiveCond_test.sh
   PROPERTIES TIMEOUT 300
   )

