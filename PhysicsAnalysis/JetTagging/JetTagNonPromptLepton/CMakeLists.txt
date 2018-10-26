################################################################################
# Package: JetTagNonPromptLepton
################################################################################

# Declare the package name:
atlas_subdir( JetTagNonPromptLepton )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
        PUBLIC
        GaudiKernel
        DetectorDescription/GeoPrimitives
        PRIVATE
        PhysicsAnalysis/AnalysisCommon/LeptonTaggers
  )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py ) 
