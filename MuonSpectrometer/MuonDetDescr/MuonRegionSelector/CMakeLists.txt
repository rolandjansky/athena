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
                          DetectorDescription/IRegionSelector
			  MuonSpectrometer/MuonCablings/MuonCablingData
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCablings/MuonTGC_Cabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
			  MuonSpectrometer/MuonDetDescr/MuonAGDDDescription )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

atlas_add_library( MuonRegionSelectorLib
                   MuonRegionSelector/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonRegionSelector
                   LINK_LIBRARIES RegSelLUT AthenaBaseComps GaudiKernel GeoPrimitives )

# Component(s) in the package:
atlas_add_component( MuonRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives 
		     RegSelLUT GaudiKernel Identifier CSCcablingLib 
		     MuonCablingData
		     MuonMDT_CablingLib MuonTGC_CablingLib RPCcablingInterfaceLib 
		     MuonReadoutGeometry

	             MuonAGDDDescription MuonRegionSelectorLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

