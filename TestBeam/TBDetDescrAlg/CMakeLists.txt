################################################################################
# Package: TBDetDescrAlg
################################################################################

# Declare the package name:
atlas_subdir( TBDetDescrAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          DetectorDescription/DetDescrCnvSvc
                          GaudiKernel
                          TestBeam/TBDetDescr )

# Component(s) in the package:
atlas_add_library( TBDetDescrAlgLib
                   src/TB*.cxx
                   PUBLIC_HEADERS TBDetDescrAlg
                   LINK_LIBRARIES AthenaBaseComps DetDescrCnvSvcLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel TBDetDescr )

atlas_add_component( TBDetDescrAlg
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DetDescrCnvSvcLib GaudiKernel TBDetDescr TBDetDescrAlgLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

