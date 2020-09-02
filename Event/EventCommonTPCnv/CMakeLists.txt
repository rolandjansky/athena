# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventCommonTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( EventCommonTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventCommonTPCnv
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${CLHEP_LIBRARIES} AthLinks AthenaPoolCnvSvcLib DataModelAthenaPoolLib FourMom NavFourMom
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel StoreGateLib )

atlas_add_dictionary( EventCommonTPCnvDict
                      EventCommonTPCnv/EventCommonTPCnvDict.h
                      EventCommonTPCnv/selection.xml
                      LINK_LIBRARIES EventCommonTPCnv )

atlas_add_dictionary( OLD_EventCommonTPCnvDict
                      EventCommonTPCnv/EventCommonTPCnvDict.h
                      EventCommonTPCnv/OLD_selection.xml
                      LINK_LIBRARIES EventCommonTPCnv )

# Helper variable for running the tests:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

foreach( name
    HepLorentzVectorCnv_p1_test
    P4EEtaPhiMCnv_p1_test
    P4EEtaPhiMCnv_p2_test
    P4IPtCotThPhiMCnv_p1_test
    P4ImplEEtaPhiMCnv_p1_test
    P4ImplEEtaPhiMCnv_p2_test
    P4ImplPtEtaPhiMCnv_p1_test
    P4ImplPtEtaPhiMCnv_p2_test
    P4PtEtaPhiMCnv_p1_test
    P4PtEtaPhiMCnv_p2_test
    P4PxPyPzECnv_p1_test
    INav4MomAssocsCnv_p1_test
    INav4MomAssocsCnv_p2_test
    INav4MomAssocsCnv_p3_test
         )

       atlas_add_test( ${name}
         SOURCES
         test/${name}.cxx
         LINK_LIBRARIES TestTools EventCommonTPCnv
         ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

endforeach()
                    
