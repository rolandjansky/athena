# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyAnalysisCore )

# Component(s) in the package:
atlas_add_library( PyAnalysisCoreLib
                   PyAnalysisCore/*.h
                   INTERFACE
                   PUBLIC_HEADERS PyAnalysisCore
                   LINK_LIBRARIES AthenaKernel GaudiKernel PersistentDataModel StoreGateLib )

atlas_add_dictionary( PyAnalysisCoreDict
                      PyAnalysisCore/PyAnalysisCoreDict.h
                      PyAnalysisCore/selection.xml
                      LINK_LIBRARIES PyAnalysisCoreLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

