################################################################################
# Package: TRT_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( TRT_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          PRIVATE
                          Control/CxxUtils
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
			  DetectorDescription/GeoModel/GeoModelInterfaces
			  DetectorDescription/GeoPrimitives
                          DetectorDescription/GeometryDBSvc
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( TRT_GeoModel
		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel TRT_ConditionsServicesLib InDetGeoModelUtils InDetReadoutGeometry TRT_ReadoutGeometry CxxUtils SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions IdDictDetDescr TRT_ConditionsData InDetIdentifier GeometryDBSvcLib )

atlas_add_test( TRT_GMConfig_test
                SCRIPT test/TRT_GMConfig_test.py
                PROPERTIES TIMEOUT 600 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )

