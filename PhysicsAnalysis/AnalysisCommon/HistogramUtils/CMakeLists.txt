################################################################################
# Package: HistogramUtils
################################################################################

# Declare the package name:
atlas_subdir( HistogramUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HistogramUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigDecisionToolLib AthenaBaseComps GaudiKernel PATCoreLib ExpressionEvaluationLib )

# Install files from the package:
atlas_install_headers( HistogramUtils )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

