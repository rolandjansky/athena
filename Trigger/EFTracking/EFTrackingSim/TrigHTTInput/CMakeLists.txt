# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

atlas_subdir( TrigHTTInput )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigHTTInputLib
   src/*.cxx
   PUBLIC_HEADERS            TrigHTTInput
   INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES            ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry
                             TrigHTTConfToolsLib TrigHTTMapsLib TrigHTTObjectsLib
   PRIVATE_LINK_LIBRARIES    AthContainers EventInfo InDetIdentifier  StoreGateLib TrigHTTSGInputLib )

atlas_add_component( TrigHTTInput
   src/components/*.cxx
   LINK_LIBRARIES            TrigHTTInputLib
)

# Install files from the package:
atlas_install_joboptions( share/*.py test/*.py)
atlas_install_scripts( scripts/*)
atlas_install_python_modules( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 ) 

# Test in the package:
atlas_add_test(             testReadRawHit
  SCRIPT                    test/test_TrigHTTReadRawHitsWrapperAlg.sh
  LOG_SELECT_PATTERN        "HTTRawHitsWrapp"
  PRIVATE_WORKING_DIRECTORY
)

atlas_add_test(             testRawToLogical
  SCRIPT                    test/test_TrigHTTRawToLogicalWrapperAlg.sh 
  LOG_SELECT_PATTERN        "HTTReadLogical"
  PRIVATE_WORKING_DIRECTORY
)

atlas_add_test(             testDumpOutputStat
  SCRIPT                    test/test_TrigHTTDumpOutputStatAlg.sh  
  LOG_SELECT_PATTERN        "HTTDumpOutputSt"
  PRIVATE_WORKING_DIRECTORY
)

