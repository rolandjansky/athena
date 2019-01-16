################################################################################
# Package: MdtCalibSvc
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
			  MuonSpectrometer/MuonCablings/MuonMDT_Cabling
           		  MuonSpectrometer/MuonCablings/MuonCablingData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          Control/StoreGate
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
			  Tools/PathResolver )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MdtCalibSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibSvc
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel MuonMDT_CablingLib MdtCalibData MuonCalibIdentifier MuonPrepRawData StoreGateLib SGtests MuonIdHelpersLib PathResolver
                   PRIVATE_LINK_LIBRARIES MagFieldInterfaces MuonCalibEvent MuonReadoutGeometry )

atlas_add_component( MdtCalibSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES MdtCalibSvcLib )

# Install files from the package:
atlas_install_runtime( share/DC2_t0.dat share/DC2_rt.dat )
atlas_install_joboptions( share/*.py )

