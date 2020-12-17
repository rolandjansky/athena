# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyAnalysisExamples )

# External dependencies:
find_package( CLHEP )
find_package( Python COMPONENTS Development )

# Component(s) in the package:
atlas_add_library( PyAnalysisExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS PyAnalysisExamples
                   INCLUDE_DIRS ${Python_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Python_LIBRARIES} AthenaBaseComps AthenaKernel AthenaKernel GaudiKernel egammaEvent
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers PyKernel RootUtils )

atlas_add_component( PyAnalysisExamples
                     src/components/*.cxx
                     LINK_LIBRARIES PyAnalysisExamplesLib )

atlas_add_dictionary( PyAnalysisExamplesDict
                      PyAnalysisExamples/PyAnalysisExamplesDict.h
                      PyAnalysisExamples/selection.xml
                      LINK_LIBRARIES PyAnalysisExamplesLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

