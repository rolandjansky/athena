################################################################################
# Package: SiZvertexTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiZvertexTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed )

# Component(s) in the package:
atlas_add_component( SiZvertexTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces SiSpacePointsSeed )

# Install files from the package:
atlas_install_headers( SiZvertexTool_xk )

