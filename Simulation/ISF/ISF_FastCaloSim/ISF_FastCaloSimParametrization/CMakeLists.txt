# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FastCaloSimParametrization )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepPDT )
find_package( ROOT COMPONENTS TreePlayer Core Tree MathCore Hist RIO Physics Graf Gpad )

# Component(s) in the package:
atlas_add_root_dictionary( ISF_FastCaloSimParametrizationLib
                           ISF_FastCaloSimParametrizationLibDictSource
                           ROOT_HEADERS
                           ISF_FastCaloSimParametrization/MeanAndRMS.h
                           ISF_FastCaloSimParametrization/TFCSNNLateralShapeParametrization.h
                           ISF_FastCaloSimParametrization/TFCSSimpleLateralShapeParametrization.h
                           ISF_FastCaloSimParametrization/TreeReader.h
                           ISF_FastCaloSimParametrization/FCS_Cell.h
                           ISF_FastCaloSimParametrization/CaloGeometry.h
                           ISF_FastCaloSimParametrization/CaloGeometryLookup.h
                           Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT HepPDT CLHEP Geant4 )

atlas_add_library( ISF_FastCaloSimParametrizationLib
                   Root/*.cxx src/lib/CaloGeometryFromCaloDDM.cxx src/lib/CaloGeometryLookup.cxx src/lib/CaloGeometry.cxx
                   ${ISF_FastCaloSimParametrizationLibDictSource}
                   PUBLIC_HEADERS ISF_FastCaloSimParametrization
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AtlasHepMCLib BarcodeEventLib CaloDetDescrLib CaloInterfaceLib GaudiKernel ISF_FastCaloSimEvent ISF_InterfacesLib LArElecCalib LArReadoutGeometry StoreGateLib TrkEventPrimitives TrkExInterfaces TrkParameters
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers )

atlas_add_component( ISF_FastCaloSimParametrization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaPoolUtilities AtlasHepMCLib CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier EventInfo GaudiKernel GeneratorObjects GeoAdaptors GeoModelInterfaces GeoPrimitives ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib ISF_InterfacesLib LArSimEvent NavFourMom StoreGateLib TileConditionsLib TileDetDescr TileSimEvent TrackRecordLib TrkExInterfaces TrkGeometry TrkMaterialOnTrack TrkParameters TrkSurfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
