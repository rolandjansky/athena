################################################################################
# Package: ForwardDetectorsD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( ForwardDetectorsD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_RecoEv/AFP_LocRecoEv
                          ForwardDetectors/AFP/AFP_SimEv
                          ForwardDetectors/ALFA/ALFA_CLinkEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_GloRecEv
                          ForwardDetectors/ALFA/ALFA_SimEv
                          ForwardDetectors/ZDC/ZdcConditions
                          ForwardDetectors/ZDC/ZdcEvent
                          ForwardDetectors/ZDC/ZdcIdentifier
                          GaudiKernel
                          Generators/GeneratorObjects
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ForwardDetectorsD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests Identifier AFP_DigiEv AFP_LocRecoEv AFP_SimEv ALFA_CLinkEv ALFA_GloRecEv ALFA_SimEv ZdcConditions ZdcEvent ZdcIdentifier GaudiKernel GeneratorObjects D3PDMakerUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

