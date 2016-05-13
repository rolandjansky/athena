################################################################################
# Package: HitManagement
################################################################################

# Declare the package name:
atlas_subdir( HitManagement )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaKernel
                          Event/EventInfo
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools )

# Component(s) in the package:
atlas_add_library( HitManagement
                   src/*.cxx
                   PUBLIC_HEADERS HitManagement
                   LINK_LIBRARIES AthContainers AthenaKernel EventInfo GaudiKernel
                   PRIVATE_LINK_LIBRARIES TestTools )

atlas_add_test( TimedHitPtrCollection_test
                SOURCES
                test/TimedHitPtrCollection_test.cxx
                LINK_LIBRARIES AthContainers AthenaKernel EventInfo GaudiKernel TestTools HitManagement
                EXTRA_PATTERNS "^Warning in <TEnvRec::ChangeValue>: duplicate entry" )

