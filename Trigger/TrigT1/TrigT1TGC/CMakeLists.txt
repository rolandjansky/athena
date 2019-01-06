################################################################################
# Package: TrigT1TGC
################################################################################

# Declare the package name:
atlas_subdir( TrigT1TGC )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Event/EventInfo
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          TileCalorimeter/TileEvent
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigT1TGCLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1TGC
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel TrigT1Interfaces StoreGateLib SGtests TGCcablingInterfaceLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} EventInfo GeneratorObjects MuonDigitContainer MuonRDO MuonCondInterface MuonCondSvcLib AthenaPoolUtilities TileEvent PathResolver TrigConfL1Data )

atlas_add_component( TrigT1TGC
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel TrigT1Interfaces EventInfo GeneratorObjects TGCcablingInterfaceLib MuonDigitContainer MuonIdHelpersLib MuonRDO MuonCondInterface MuonCondSvcLib AthenaPoolUtilities TileEvent PathResolver TrigConfL1Data TrigT1TGCLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TrigT1TGCJobOptions.py share/TrigT1TGC_jobOptions.py )
atlas_install_runtime( share/TrigT1TGCMaskedChannel*.db )
