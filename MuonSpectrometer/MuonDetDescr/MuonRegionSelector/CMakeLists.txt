################################################################################
# Package: MuonRegionSelector
################################################################################

# Declare the package name:
atlas_subdir( MuonRegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/RegSelLUT
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCablings/MuonMDT_Cabling
                          MuonSpectrometer/MuonCablings/MuonTGC_Cabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives RegSelLUT GaudiKernel Identifier CSCcablingLib MuonMDT_CablingLib MuonTGC_CablingLib RPCcablingInterfaceLib MuonReadoutGeometry )

# Install files from the package:
atlas_install_headers( MuonRegionSelector )
atlas_install_joboptions( share/*.py )

