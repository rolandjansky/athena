################################################################################
# Package: HIRecTPCnv
################################################################################

# Declare the package name:
atlas_subdir( HIRecTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetEventCnv/InDetEventTPCnv
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/HeavyIonRec/HIGlobal
                          Tracking/TrkEventCnv/TrkEventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_headers( HIRecTPCnv )

