
#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

################################################################################
# Package: MyPackage
################################################################################

# Declare the package name:
atlas_subdir( AthExUnittest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
			  Control/AthenaBaseComps
                          AtlasTest/GoogleTestTools )

# The component lib:
atlas_add_component( AthExUnittest
                     AthExUnittest/*.h src/*.cxx
                     src/components/*.cxx
                     Root/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps )

# The test:
atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps )


