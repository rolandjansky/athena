################################################################################
# Package: TrigInDetPattRecoTools
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetPattRecoTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigInDetPattRecoEvent
                          PRIVATE
                          DetectorDescription/IRegionSelector )

# Component(s) in the package:
atlas_add_library( TrigInDetPattRecoTools
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetPattRecoTools
                   LINK_LIBRARIES TrigInDetEvent TrigInDetPattRecoEvent
                   PRIVATE_LINK_LIBRARIES IRegionSelector )

