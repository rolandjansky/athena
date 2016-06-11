################################################################################
# Package: MuonTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrGeoModelCnv
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkVolumes
                          PRIVATE
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          Tracking/TrkDetDescr/TrkGeometrySurfaces
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoModelKernel GaudiKernel TrkDetDescrGeoModelCnv TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry TrkVolumes StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonIdHelpersLib TrkGeometrySurfaces TrkSurfaces )

# Install files from the package:
atlas_install_headers( MuonTrackingGeometry )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

