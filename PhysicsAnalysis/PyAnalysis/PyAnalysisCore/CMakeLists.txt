################################################################################
# Package: PyAnalysisCore
################################################################################

# Declare the package name:
atlas_subdir( PyAnalysisCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( PyAnalysisCoreDict
                      PyAnalysisCore/PyAnalysisCoreDict.h
                      PyAnalysisCore/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel StoreGateLib SGtests PersistentDataModel GaudiKernel )

# Install files from the package:
atlas_install_headers( PyAnalysisCore )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

