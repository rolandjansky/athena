################################################################################
# Package: egammaMVACalib
################################################################################

# Declare the package name:
atlas_subdir( egammaMVACalib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
			  Reconstruction/MVAUtils
                          Reconstruction/egamma/egammaRecEvent
                          Control/CxxUtils
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( egammaMVACalibLib
                   Root/*.cxx
                   PUBLIC_HEADERS egammaMVACalib
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} egammaRecEvent CxxUtils PathResolver MVAUtils )

atlas_add_component( egammaMVACalib
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking egammaRecEvent GaudiKernel CxxUtils PathResolver egammaMVACalibLib MVAUtils )

atlas_add_dictionary( egammaMVACalibDict
                      egammaMVACalib/egammaMVACalibDict.h
                      egammaMVACalib/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking egammaRecEvent GaudiKernel CxxUtils PathResolver egammaMVACalibLib MVAUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )

