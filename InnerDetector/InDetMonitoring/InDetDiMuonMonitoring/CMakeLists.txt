# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetDiMuonMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RooFitCore RooFit Gpad )
find_package( FFTW ) # Needed for RooFFTConvPdf

# Component(s) in the package:
atlas_add_component( InDetDiMuonMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODMuon xAODTracking
   GaudiKernel CxxUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )
