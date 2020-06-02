################################################################################
# Package: MdtCalibSvc
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibSvc )

# Component(s) in the package:
atlas_add_library( MdtCalibSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibSvc
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel MuonReadoutGeometry Identifier GaudiKernel MuonMDT_CablingLib MdtCalibData MuonCalibIdentifier MuonPrepRawData StoreGateLib SGtests MuonIdHelpersLib PathResolver MagFieldElements MagFieldConditions MuonCablingData MdtCalibInterfacesLib MuonCalibEvent )

atlas_add_component( MdtCalibSvc
                     src/components/*.cxx
                     LINK_LIBRARIES MdtCalibSvcLib )

# Install files from the package:
atlas_install_runtime( share/DC2_t0.dat share/DC2_rt.dat )
atlas_install_joboptions( share/*.py )

