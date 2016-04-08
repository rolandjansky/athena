################################################################################
# Package: EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventInfo
                          Event/EventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventInfo/EventInfo.h EventInfo/EventStreamInfo.h EventInfo/MergedEventInfo.h EventInfo/PileUpEventInfo.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaKernel AthenaPoolCnvSvcLib EventInfo EventTPCnv )

# Install files from the package:
atlas_install_headers( EventAthenaPool )

