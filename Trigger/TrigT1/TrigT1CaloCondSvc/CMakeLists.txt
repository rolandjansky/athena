################################################################################
# Package: TrigT1CaloCondSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCondSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/RegistrationServices
   GaudiKernel
   Trigger/TrigT1/TrigT1CaloCalibConditions )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCondSvcLib
   TrigT1CaloCondSvc/*.h TrigT1CaloCondSvc/*.icc src/*.cxx
   PUBLIC_HEADERS TrigT1CaloCondSvc
   LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib
   AthenaPoolUtilities GaudiKernel TrigT1CaloCalibConditions )

atlas_add_component( TrigT1CaloCondSvc
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel TrigT1CaloCondSvcLib )
