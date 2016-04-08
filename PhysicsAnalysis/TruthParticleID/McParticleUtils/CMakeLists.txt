################################################################################
# Package: McParticleUtils
################################################################################

# Declare the package name:
atlas_subdir( McParticleUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/DataModel
                          Event/EventKernel
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CppUnit )
find_package( HepMC )
find_package( HepPDT )
find_package( PythonLibs )

# Component(s) in the package:
atlas_add_library( McParticleUtils
                   src/*.cxx
                   PUBLIC_HEADERS McParticleUtils
                   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaKernel DataModel EventKernel GaudiKernel AnalysisUtilsLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${PYTHON_LIBRARIES} ${CLHEP_LIBRARIES} ${CPPUNIT_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py )

