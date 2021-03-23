# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfigSvc )

# External dependencies:
find_package( Boost )
find_package( COOL COMPONENTS CoolKernel )
find_package( Oracle )     # Oracle client libraries for DB access
find_package( cx_Oracle )  # Oracle python client
find_package( nlohmann_json )

atlas_add_component( TrigConfigSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${TBB_LIBRARIES} AthAnalysisBaseCompsLib AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils GaudiKernel L1TopoConfig PathResolver StoreGateLib TrigConfBase TrigConfData TrigConfHLTData TrigConfIO TrigConfInterfaces TrigConfL1Data TrigConfStorage nlohmann_json::nlohmann_json )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/jobOptions_commonSetup.py
  share/jobOptions_setupHLTSvc.py
  share/jobOptions_setupLVL1Svc.py
  share/testTriggerFrontierQuery.py )
atlas_install_scripts( share/checkTrigger.py share/trigconf_property.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_xmls( data/*.dtd )

# Aliases:
atlas_add_alias( checkTrigger "checkTrigger.py" )

atlas_add_test( AccumulatorTest
   SCRIPT python -m TrigConfigSvc.TrigConfigSvcConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( FrontierTest
   SCRIPT python -m TrigConfigSvc.TrigConfFrontier
   POST_EXEC_SCRIPT nopost.sh )
