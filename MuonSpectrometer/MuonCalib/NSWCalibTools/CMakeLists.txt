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
                           PRIVATE
			   Control/AthenaBaseComps )
 
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist )
 
# Component(s) in the package:
 atlas_add_library( NSWCalibToolsLib
                    src/*.cxx
                    PUBLIC_HEADERS NSWCalibTools
		    PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                    LINK_LIBRARIES GaudiKernel MuonPrepRawData
		    PRIVATE_LINK_LIBRARIES AthenaBaseComps )

 atlas_add_component( NSWCalibTools
                      src/components/*.cxx
		      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps MuonPrepRawData NSWCalibToolsLib )

 atlas_install_python_modules( python/*.py )
