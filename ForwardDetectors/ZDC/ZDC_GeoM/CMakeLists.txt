################################################################################
# Package: ZDC_GeoM
################################################################################

# Declare the package name:
atlas_subdir( ZDC_GeoM )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( ZDC_GeoMLib
                   src/*.cxx
                   PUBLIC_HEADERS ZDC_GeoM
                   LINK_LIBRARIES AthenaKernel SGTools GeoModelKernel GeoModelUtilities StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_component( ZDC_GeoM
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel SGTools GeoModelKernel GeoModelUtilities StoreGateLib SGtests GaudiKernel ZDC_GeoMLib )

