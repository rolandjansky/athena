################################################################################
# Package: PixelConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/Identifier
			  DetectorDescription/GeoModel/GeoModelInterfaces
			  DetectorDescription/GeoModel/GeoModelUtilities
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/PixelCoralClientUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          TestPolicy
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( PixelConditionsTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel AthenaBaseComps SGTools AthenaPoolUtilities DetDescrConditions Identifier PixelConditionsData PixelCoralClientUtils InDetIdentifier GeoModelUtilities InDetReadoutGeometry AtlasCLHEP_RandomGenerators PathResolver )

# Install files from the package:
atlas_install_headers( PixelConditionsTools )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.py )

