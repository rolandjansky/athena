################################################################################
#Package: NSWCalibTools
################################################################################
  
# Declare the package name:
atlas_subdir( NSWCalibTools )
   
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                           GaudiKernel
                           MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                           MuonSpectrometer/MuonIdHelpers
                           MagneticField/MagFieldElements
                           MagneticField/MagFieldConditions
                           PRIVATE
                           MuonSpectrometer/MuonRDO
                           Control/AthenaBaseComps )
  
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist )
  
# Component(s) in the package:
atlas_add_library( NSWCalibToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS NSWCalibTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel MuonPrepRawData MagFieldElements MagFieldConditions MuonRDO
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps MuonIdHelpersLib )
 

atlas_add_component(NSWCalibTools
                    src/components/*.cxx
                    PUBLIC_HEADERS NSWCalibTools
                    PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                    LINK_LIBRARIES NSWCalibToolsLib
                    PRIVATE_LINK_LIBRARIES AthenaBaseComps )
 
atlas_install_python_modules( python/*.py )
