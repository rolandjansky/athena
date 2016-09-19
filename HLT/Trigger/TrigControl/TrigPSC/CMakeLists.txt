################################################################################
# Package: TrigPSC
################################################################################

# Declare the package name:
atlas_subdir( TrigPSC )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/RDBAccessSvc
                          HLT/Trigger/TrigControl/TrigKernel
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfBase )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( PythonLibs )
find_package( tdaq-common COMPONENTS ers eformat eformat_write hltinterface )

# Component(s) in the package:
atlas_add_library( TrigPSC
                   src/*.cxx
                   PUBLIC_HEADERS TrigPSC
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${PYTHON_LIBRARIES} TrigKernel PathResolver TrigConfBase )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

