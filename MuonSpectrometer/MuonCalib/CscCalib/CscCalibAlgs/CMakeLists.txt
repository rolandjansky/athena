################################################################################
# Package: CscCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( CscCalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          TestPolicy
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthContainers
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibData
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CscCalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthContainers StoreGateLib SGtests GaudiKernel CscCalibData MuonCondInterface MuonCondData MuonReadoutGeometry MuonIdHelpersLib MuonRDO )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

