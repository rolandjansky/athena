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
