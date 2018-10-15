################################################################################
# Package: InDetReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( InDetReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/TRT_ConditionsData )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( InDetReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS InDetReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel SGTools AtlasDetDescr GeoModelKernel GeoModelUtilities GeoPrimitives Identifier GaudiKernel InDetCondServices InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests AthenaBaseComps DetDescrConditions
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities IdDictDetDescr TRT_ConditionsData )

