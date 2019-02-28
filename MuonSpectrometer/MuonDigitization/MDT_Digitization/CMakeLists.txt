################################################################################
# Package: MDT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( MDT_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonDigitization/MDT_Response
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          PRIVATE
                          Event/EventPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( MDT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier xAODEventInfo GaudiKernel GeneratorObjects MdtCalibData MdtCalibSvcLib MuonCondInterface MuonReadoutGeometry MuonDigitContainer MDT_Response MuonIdHelpersLib MuonSimData MuonSimEvent HitManagement PathResolver TrkDetDescrUtils EventPrimitives )

# Install files from the package:
atlas_install_headers( MDT_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.rt )

