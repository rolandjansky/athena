################################################################################
# Package: HGTD_ReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( HGTD_ReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          DetectorDescription/ReadoutGeometryBase
                          GaudiKernel
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/IdDictDetDescr )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( HGTD_ReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_ReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel SGTools AtlasDetDescr GeoModelKernel GeoPrimitives Identifier ReadoutGeometryBase GaudiKernel HGTD_Identifier TrkSurfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr )
