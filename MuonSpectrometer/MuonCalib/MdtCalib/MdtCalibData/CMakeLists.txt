################################################################################
# Package: MdtCalibData
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibData )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MdtCalibData
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers EventPrimitives MuonCalibMath
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel GaudiKernel )

