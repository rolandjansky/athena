################################################################################
# Package: TrkDigTools
################################################################################

# Declare the package name:
atlas_subdir( TrkDigTools )
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDigitization/TrkDigEvent
                          Tracking/TrkDigitization/TrkDigInterface
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces )

# Component(s) in the package:
atlas_add_component( TrkDigTools
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel GeoPrimitives TrkDetDescrUtils TrkDigEvent TrkDigInterfaces TrkSurfaces)
# Install files from the package:
atlas_install_headers( TrkDigTools )

