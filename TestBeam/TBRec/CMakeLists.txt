# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBRec )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary( TBRecLib
                           TBRecLibDictSource
                           ROOT_HEADERS TBRec/TBH6RunHeader.h TBRec/LinkDef.h
                           EXTERNAL_PACKAGES  ROOT Boost CLHEP Eigen )

atlas_add_library( TBRecLib
                   src/*.cxx
                   ${TBRecLibDictSource}
                   PUBLIC_HEADERS TBRec
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel AthContainers GeoPrimitives Identifier EventInfo xAODTracking GaudiKernel LArIdentifier LArRawEvent TBEvent TrkDetDescrUtils TrkParameters TrkTrack CaloRecLib CaloUtilsLib StoreGateLib SGtests LArRawUtilsLib LArCablingLib CaloDetDescrLib TBCaloGeometryLib TBCondRunParLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloDmDetDescr CaloSimEvent CondDBObjects xAODCaloEvent LArG4TBSimEvent Particle PathResolver TrkSurfaces TrkEventPrimitives TrkExInterfaces CaloTrackingGeometryLib TrackRecordLib )

atlas_add_component( TBRec
                     src/components/*.cxx
                     LINK_LIBRARIES TBRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.xml )

