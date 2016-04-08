################################################################################
# Package: ISF_TrackingTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_TrackingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Tracking/ISF_TrackingInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_component( ISF_TrackingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ISF_Event TrkDetDescrInterfaces TrkGeometry TrkEventPrimitives TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

