################################################################################
# Package: TrigAFPReco
################################################################################

# Declare the package name:
atlas_subdir( TrigAFPReco )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
      			  Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
			  Control/AthLinks
                          Control/AthContainers
                          ForwardDetectors/AFP/AFP_ByteStream2RawCnv
			  ForwardDetectors/AFP/AFP_DigiEv
			  ForwardDetectors/AFP/AFP_RawEv
                          ForwardDetectors/AFP/AFP_Raw2Digi
                          ForwardDetectors/AFP/AFP_Reconstruction/AFP_LocRecoInterfaces
			  Event/EventInfo
			  Generators/GeneratorObjects
			  Event/xAOD/xAODForward
			  Trigger/TrigEvent/TrigParticle
			  Trigger/TrigEvent/TrigSteeringEvent
			  Trigger/TrigSteer/TrigInterfaces
                          GaudiKernel )

# External dependencies:
find_package( tdaq-common  )
find_package( ROOT COMPONENTS pthread  Core Tree Hist HepMC CLHEP)

# Component(s) in the package:
atlas_add_component( TrigAFPReco
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} AFP_RawEv
		     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AFP_RawEv AthContainers xAODForward AFP_ByteStream2RawCnvLib GaudiKernel TrigParticle TrigInterfaces)

# Install files from the package:
atlas_install_headers( TrigAFPReco )
#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
