################################################################################
# Package: ZdcConditions
################################################################################

# Declare the package name:
atlas_subdir( ZdcConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          ForwardDetectors/ZDC/ZdcIdentifier
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ZdcConditions
                   src/*.cxx
                   PUBLIC_HEADERS ZdcConditions
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier ZdcIdentifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

