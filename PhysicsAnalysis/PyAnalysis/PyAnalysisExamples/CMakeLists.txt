################################################################################
# Package: PyAnalysisExamples
################################################################################

# Declare the package name:
atlas_subdir( PyAnalysisExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel
                          Reconstruction/egamma/egammaEvent
                          PRIVATE
                          Control/PyKernel
                          Control/StoreGate )

# External dependencies:
find_package( CLHEP )
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PyAnalysisExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS PyAnalysisExamples
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES} AthenaBaseComps AthenaKernel SGTools GaudiKernel egammaEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} PyKernel )

atlas_add_component( PyAnalysisExamples
                     src/components/*.cxx
                     INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel SGTools GaudiKernel egammaEvent PyKernel StoreGateLib SGtests PyAnalysisExamplesLib )

atlas_add_dictionary( PyAnalysisExamplesDict
                      PyAnalysisExamples/PyAnalysisExamplesDict.h
                      PyAnalysisExamples/selection.xml
                      INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel SGTools GaudiKernel egammaEvent PyKernel StoreGateLib SGtests PyAnalysisExamplesLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

