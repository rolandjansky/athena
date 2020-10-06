# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloLocalHadCalib )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Gpad Graf Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( CaloLocalHadCalib
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		             LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloDmDetDescr CaloEvent CaloGeoHelpers CaloIdentifier CaloSimEvent CaloUtilsLib CxxUtils GaudiKernel GeneratorObjects PathResolver StoreGateLib TBEvent xAODCaloEvent )

atlas_add_dictionary( CaloLocalHadCalibDict
                      CaloLocalHadCalib/CaloLocalHadCalibDict.h
                      CaloLocalHadCalib/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		              LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloDmDetDescr CaloEvent CaloGeoHelpers CaloIdentifier CaloSimEvent CaloUtilsLib CxxUtils GaudiKernel GeneratorObjects PathResolver StoreGateLib TBEvent xAODCaloEvent )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )
atlas_install_runtime( share/CaloHadDMCoeff_init_v1.txt share/CaloHadDMCoeff_init_v2.txt )

