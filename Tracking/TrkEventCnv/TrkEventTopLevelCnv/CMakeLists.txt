################################################################################
# Package: TrkEventTopLevelCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkEventTopLevelCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Tracking/TrkEventCnv/TrkEventTPCnv
   InnerDetector/InDetEventCnv/InDetEventTPCnv
   MuonSpectrometer/MuonCnv/MuonEventTPCnv
   Reconstruction/RecTPCnv
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils
   )

# Component(s) in the package:
atlas_add_tpcnv_library( TrkEventTopLevelCnv
   TrkEventTopLevelCnv/*.h src/*.cxx
   PUBLIC_HEADERS TrkEventTopLevelCnv
   LINK_LIBRARIES
   InDetEventTPCnv MuonEventTPCnv TrkEventTPCnv RecTPCnv
   PRIVATE_LINK_LIBRARIES CxxUtils
   )

atlas_add_dictionary( TrkEventTopLevelCnvDict
   TrkEventTopLevelCnv/TrkEventTopLevelCnvDict.h
   TrkEventTopLevelCnv/selection.xml
   LINK_LIBRARIES TrkEventTopLevelCnv
   )

