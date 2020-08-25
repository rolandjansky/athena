################################################################################
# Package: InDetRegionSelector
################################################################################

# Declare the package name:
atlas_subdir( InDetRegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/RegSelLUT
                          GaudiKernel
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/Identifier                          
			  DetectorDescription/IRegionSelector
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_Cabling )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( InDetRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps RegSelLUT GaudiKernel PixelConditionsData SCT_CablingLib Identifier InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry TRT_CablingLib AthenaKernel )

# Install files from the package:
atlas_install_headers( InDetRegionSelector )
atlas_install_joboptions( share/*.py )

