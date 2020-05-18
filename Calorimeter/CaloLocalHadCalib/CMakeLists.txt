################################################################################
# Package: CaloLocalHadCalib
################################################################################

# Declare the package name:
atlas_subdir( CaloLocalHadCalib )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Control/StoreGate
			  Control/CxxUtils
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloDmDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Calorimeter/CaloUtils
                          Control/AthenaKernel
                          Generators/GeneratorObjects
                          TestBeam/TBEvent
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Gpad Graf Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# this line failed automatic conversion in cmt2cmake :
# use AtlasReflex   AtlasReflex-*    External -no_CALOATHENAPOOL_ROOTauto_imports

# Component(s) in the package:
atlas_add_component( CaloLocalHadCalib
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloGeoHelpers AthenaBaseComps StoreGateLib CxxUtils 
		     SGtests xAODCaloEvent GaudiKernel CaloDetDescrLib CaloDmDetDescr CaloEvent CaloIdentifier CaloSimEvent CaloUtilsLib AthenaKernel GeneratorObjects TBEvent PathResolver )

atlas_add_dictionary( CaloLocalHadCalibDict
                      CaloLocalHadCalib/CaloLocalHadCalibDict.h
                      CaloLocalHadCalib/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloGeoHelpers AthenaBaseComps StoreGateLib CxxUtils
		      SGtests xAODCaloEvent GaudiKernel CaloDetDescrLib CaloDmDetDescr CaloEvent CaloIdentifier CaloSimEvent CaloUtilsLib AthenaKernel GeneratorObjects TBEvent PathResolver )

# Install files from the package:
atlas_install_headers( CaloLocalHadCalib )
atlas_install_joboptions( share/*.txt share/*.py )
atlas_install_runtime( share/CaloHadDMCoeff_init_v1.txt share/CaloHadDMCoeff_init_v2.txt )

