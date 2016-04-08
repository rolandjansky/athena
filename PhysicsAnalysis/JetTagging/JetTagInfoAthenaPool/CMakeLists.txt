################################################################################
# Package: JetTagInfoAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( JetTagInfoAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/JetTagging/JetTagInfoTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( JetTagInfoAthenaPoolPoolCnv
                           src/*.cxx
                           FILES JetTagInfoAthenaPool/JetTagInfo.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib JetTagInfoTPCnv )

# Install files from the package:
atlas_install_headers( JetTagInfoAthenaPool )

