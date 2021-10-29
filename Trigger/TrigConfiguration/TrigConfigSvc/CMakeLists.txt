# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfigSvc )

# External dependencies:
find_package( Boost )
find_package( COOL COMPONENTS CoolKernel )
find_package( Oracle )     # Oracle client libraries for DB access
find_package( cx_Oracle )  # Oracle python client
find_package( nlohmann_json )
find_package( TBB )

atlas_add_component( TrigConfigSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps AthenaPoolUtilities CxxUtils GaudiKernel StoreGateLib TrigConfData TrigConfIO TrigConfInterfaces nlohmann_json::nlohmann_json )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/trigconf_property.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( AccumulatorTest
   SCRIPT python -m TrigConfigSvc.TrigConfigSvcCfg
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( FrontierTest
   SCRIPT python -m TrigConfigSvc.TrigConfFrontier
   POST_EXEC_SCRIPT nopost.sh )
