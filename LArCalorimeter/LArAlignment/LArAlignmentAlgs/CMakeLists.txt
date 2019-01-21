################################################################################
# Package: LArAlignmentAlgs
################################################################################

# Declare the package name:
atlas_subdir( LArAlignmentAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/RegistrationServices
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/DetDescrCond/DetDescrConditions )

# Component(s) in the package:
atlas_add_component( LArAlignmentAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel DetDescrConditions )

# Install files from the package:
atlas_install_headers( LArAlignmentAlgs )
atlas_install_joboptions( share/*.py )

