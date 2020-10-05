################################################################################
# Package: ISF_FastCaloSimParametrization
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimParametrization )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepPDT )
find_package( ROOT COMPONENTS TreePlayer Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint TMVA Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( XercesC )

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
                           EXTERNAL_PACKAGES  ROOT HepPDT XercesC CLHEP HepMC Geant4 )

atlas_add_library( ISF_FastCaloSimParametrizationLib
                   Root/*.cxx src/lib/CaloGeometryFromCaloDDM.cxx src/lib/CaloGeometryLookup.cxx src/lib/CaloGeometry.cxx
                   ${ISF_FastCaloSimParametrizationLibDictSource}
                   PUBLIC_HEADERS ISF_FastCaloSimParametrization
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} AtlasHepMCLib CaloGeoHelpers AthenaBaseComps AthenaKernel Identifier GaudiKernel ISF_InterfacesLib TrkEventPrimitives TrkParameters TrkExInterfaces CaloDetDescrLib StoreGateLib SGtests TileConditionsLib
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} CaloEvent CaloIdentifier AthenaPoolUtilities GeoAdaptors EventInfo NavFourMom GeneratorObjects LArSimEvent ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces TileDetDescr TileSimEvent TrkGeometry TrkSurfaces TrkMaterialOnTrack )

atlas_add_component( ISF_FastCaloSimParametrization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloInterfaceLib AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel ISF_InterfacesLib TrkEventPrimitives TrkParameters TrkExInterfaces CaloEvent CaloIdentifier AthenaPoolUtilities GeoAdaptors EventInfo NavFourMom GeneratorObjects LArSimEvent ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces TileConditionsLib TileDetDescr TileSimEvent TrkGeometry TrkSurfaces TrkMaterialOnTrack ISF_FastCaloSimParametrizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
