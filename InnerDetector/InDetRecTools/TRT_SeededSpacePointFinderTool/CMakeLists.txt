################################################################################
# Package: TRT_SeededSpacePointFinderTool
################################################################################

# Declare the package name:
atlas_subdir( TRT_SeededSpacePointFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          GaudiKernel
			  Control/StoreGate
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkSpacePoint
                          PRIVATE
                          DetectorDescription/RoiDescriptor
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_SeededSpacePointFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IRegionSelector GaudiKernel SiSpacePointsSeed InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkSpacePoint RoiDescriptor InDetIdentifier InDetPrepRawData TrkSurfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TRT_SeededSpacePointFinderTool )

