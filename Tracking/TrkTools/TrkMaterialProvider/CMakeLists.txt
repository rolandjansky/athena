################################################################################
# Package: TrkMaterialProvider
################################################################################

# Declare the package name:
atlas_subdir( TrkMaterialProvider )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkMaterialProvider
                     src/TrkMaterialProviderTool.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces AtlasDetDescr Identifier EventPrimitives GaudiKernel MuidEvent MuidInterfaces muonEvent RecoToolInterfaces TrkEventUtils TrkMaterialOnTrack MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_headers( TrkMaterialProvider )

