################################################################################
# Package: TrigT1CaloFexSim
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloFexSim )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArTools
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          Calorimeter/CaloEvent
                          Calorimeter/CaloTriggerTool
                          LArCalorimeter/LArElecCalib 
                          Reconstruction/Jet/JetCalibTools
                          Reconstruction/Jet/JetInterface
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT1CaloFexSim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigAnalysisInterfaces JetInterface JetCalibToolsLib xAODEventInfo CaloEvent xAODTrigL1Calo xAODBase xAODCore CaloDetDescrLib CaloIdentifier AthenaBaseComps GaudiKernel LArToolsLib CaloTriggerToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_headers( TrigT1CaloFexSim )
