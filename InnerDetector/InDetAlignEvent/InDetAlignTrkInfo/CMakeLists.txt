################################################################################
# Package: InDetAlignTrkInfo
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignTrkInfo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools )

# Component(s) in the package:
atlas_add_library( InDetAlignTrkInfo
                   src/AlignTrk.cxx
                   src/AlignTrkContainer.cxx
                   src/AlignSiHit.cxx
                   src/AlignTRTHit.cxx
                   PUBLIC_HEADERS InDetAlignTrkInfo
                   LINK_LIBRARIES DataModel SGTools )

