################################################################################
# Package: MdtCalibInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel )

atlas_add_library( MdtCalibInterfacesLib
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibInterfaces
                   LINK_LIBRARIES AthenaKernel GaudiKernel )

