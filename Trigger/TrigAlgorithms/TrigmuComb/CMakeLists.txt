################################################################################
# Package: TrigmuComb
################################################################################

# Declare the package name:
atlas_subdir( TrigmuComb )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMuon
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          Tools/PathResolver
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigSiSpacePointTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigmuComb
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ByteStreamCnvSvcBaseLib xAODTracking xAODTrigMuon GaudiKernel MagFieldInterfaces TrkExInterfaces TrigInDetEvent TrigMuonEvent TrigInterfacesLib TrigTimeAlgsLib AthenaKernel StoreGateLib SGtests PathResolver TrigConfHLTData TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigmuComb )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

