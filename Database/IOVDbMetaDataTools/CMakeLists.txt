################################################################################
# Package: IOVDbMetaDataTools
################################################################################

# Declare the package name:
atlas_subdir( IOVDbMetaDataTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Control/AthenaKernel
                          Database/IOVDbDataModel )

atlas_add_library( IOVDbMetaDataToolsLib
                   INTERFACE
                   PUBLIC_HEADERS IOVDbMetaDataTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( IOVDbMetaDataTools
                     src/IOVDbMetaDataTool.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib SGtests IOVDbDataModel )

# Install files from the package:
atlas_install_python_modules( python/*.py )

