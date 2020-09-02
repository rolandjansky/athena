################################################################################
# Package: ISF_TrackingTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_TrackingTools )

# Component(s) in the package:
atlas_add_component( ISF_TrackingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ISF_Event ISF_TrackingInterfacesLib TrkDetDescrInterfaces TrkGeometry TrkEventPrimitives TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

