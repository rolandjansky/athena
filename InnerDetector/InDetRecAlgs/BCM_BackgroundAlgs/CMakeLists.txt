################################################################################
# Package: BCM_BackgroundAlgs
################################################################################

# Declare the package name:
atlas_subdir( BCM_BackgroundAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRecEvent/BCM_CollisionTime )

# Component(s) in the package:
atlas_add_component( BCM_BackgroundAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel AthenaKernel Identifier InDetBCM_RawData BCM_CollisionTimeLib )

# Install files from the package:
atlas_install_headers( BCM_BackgroundAlgs )
atlas_install_joboptions( share/*.py )

