################################################################################
# Package: InDetCaloClusterROISelector
################################################################################

# Declare the package name:
atlas_subdir( InDetCaloClusterROISelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkEvent/TrkCaloClusterROI )

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( InDetCaloClusterROISelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODCaloEvent GaudiKernel CaloEvent CaloUtilsLib InDetRecToolInterfaces TrkCaloClusterROI egammaInterfacesLib )

# Install files from the package:
atlas_install_headers( InDetCaloClusterROISelector )

