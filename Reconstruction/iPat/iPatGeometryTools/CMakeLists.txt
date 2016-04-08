################################################################################
# Package: iPatGeometryTools
################################################################################

# Declare the package name:
atlas_subdir( iPatGeometryTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          Reconstruction/iPat/iPatGeometry
                          Reconstruction/iPat/iPatInterfaces
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# Component(s) in the package:
atlas_add_component( iPatGeometryTools
                     src/DetectorSelection.cxx
                     src/LayerAllocator.cxx
                     src/LayerNumberAllocator.cxx
                     src/SiDetectorLocator.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel iPatGeometry iPatInterfaces InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( iPatGeometryTools )
atlas_install_joboptions( share/*.py )

