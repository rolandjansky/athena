################################################################################
# Package: TrigT1CaloCondSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCondSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RegistrationServices
                          GaudiKernel
                          Trigger/TrigT1/TrigT1CaloCalibConditions )

# Component(s) in the package:
atlas_add_component( TrigT1CaloCondSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities GaudiKernel TrigT1CaloCalibConditions )

# Install files from the package:
atlas_install_headers( TrigT1CaloCondSvc )

