################################################################################
# Package: HLTTestApps
################################################################################

# Declare the package name:
atlas_subdir( HLTTestApps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system python )
find_package( PythonLibs )
find_package( tdaq COMPONENTS dynlibs owl ipc )
find_package( tdaq-common COMPONENTS CTPfragment hltinterface pyeformat_util )

# this line failed automatic conversion in cmt2cmake :
# macro_remove_regexp AtlasCxxPolicy_pp_cppflags "'-DPACKAGE_VERSION=.*'"

# Component(s) in the package:
atlas_add_library( pyhlttestapps
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} GaudiKernel TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/HLTTestApps/*.py python/HLTTestApps/plugins )
atlas_install_scripts( python/scripts/*.py python/scripts/*.sh )

