################################################################################
# Package: MuonAGDD
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDD )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   # the dependency on AmdcAth is only for dumping the XML!
   # in principle AGDD is independent from AMDB and all
   # information could be accessed via AGDD itself, however,
   # this complicates the job configuration for the moment
   set( extra_dep MuonSpectrometer/Amdcsimrec/AmdcAth )
   set( extra_lib AmdcAthLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDControl
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/AGDD/AGDD2GeoSvc
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/AGDD/AGDDModel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          MuonSpectrometer/MuonDetDescr/MuonAGDDBase
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
			  MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          ${extra_dep} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( MuonAGDD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AGDDControl GaudiKernel StoreGateLib SGtests AGDDKernel AGDDModel MuonAGDDBase MuonReadoutGeometry MuonDetDescrUtils ${extra_lib}
                     PRIVATE_LINK_LIBRARIES AGDD2GeoSvcLib )

# Install files from the package:
atlas_install_headers( MuonAGDD )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
