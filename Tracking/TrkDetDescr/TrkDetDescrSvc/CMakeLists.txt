################################################################################
# Package: TrkDetDescrSvc
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkDetDescrSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetDescrSvc
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES EventInfo TrkVolumes )

atlas_add_component( TrkDetDescrSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry StoreGateLib SGtests EventInfo TrkVolumes TrkDetDescrSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

