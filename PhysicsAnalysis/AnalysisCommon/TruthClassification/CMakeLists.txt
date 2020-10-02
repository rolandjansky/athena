# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#######################
# TruthClassification #
#######################
atlas_subdir(TruthClassification)

atlas_add_library(TruthClassificationLib
  TruthClassification/*.h Root/*.cxx
  PUBLIC_HEADERS TruthClassification
  LINK_LIBRARIES AsgTools AsgAnalysisInterfaces xAODBase xAODEgamma xAODMuon
  PRIVATE_LINK_LIBRARIES MCTruthClassifierLib)

atlas_add_dictionary(TruthClassificationDict
  TruthClassification/TruthClassificationDict.h
  TruthClassification/selection.xml
  LINK_LIBRARIES TruthClassificationLib)

if (NOT XAOD_STANDALONE)
  atlas_add_component(TruthClassification
    src/*.h src/*.cxx src/components/*.cxx
    LINK_LIBRARIES TruthClassificationLib)
endif ()

# Extra test libraries based on the build environment:
set(xaod_access_lib)
set(extra_libs)
# ... for AnalysisBase
if (XAOD_STANDALONE)
  set(xaod_access_lib xAODRootAccess)
else()
  # ... for AthAnalysisBase (Athena calls this POOLRootAccess)
  set(xaod_access_lib POOLRootAccessLib)
endif()

# Executable(s) in the package:
atlas_add_executable(testTruthClassificationTool
  test/test_TruthClassificationTool.cxx
  LINK_LIBRARIES TruthClassificationLib xAODEventInfo xAODTruth MuonAnalysisInterfacesLib
  ${xaod_access_lib} ${extra_libs})

# Tests
# this test currently needs a special input file as the default ASG test files
# do not contain all necessary information
atlas_add_test(TruthClassificationToolTest
  SCRIPT testTruthClassificationTool
  /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16e_p4017.PHYSVAL.pool.root)
