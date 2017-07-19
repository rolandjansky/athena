/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page EventTagAlgs_page EventTagAlgs

@section EventTagAlgs_introductionEventTagAlgs Introduction
This package contains the algorithms for aggregating the TAG fragments from the various groups.

@section EventTagAlgs_packagecontentEventTagAlgs Package Contents
EventTagAlgs contains the following algs/files:

- TagBuilderBase ... base class for TAG builders
- AnalysisTagBuilder ... builder for analysis word from each physics and performance group
- DataQualityTagBuilder ... builder for detector status tag and good for physics tag
- EventSplitter ... splitter for streaming
- ElectronTagBuilder ... builder for Electron
- EventSplitter ... splitter for streaming
- GlobalEventTagBuilder ... builder for EventInfo
- GlobalTriggerTagBuilder ... builder for Trigger Info
- MissingETTagBuilder ... builder for Missing ET
- MuonTagBuilder ... builder for Muon
- ParticleJetTagBuilder ... builder for ParticleJet
- PhotonTagBuilder ... builder for Photon
- TauJetTagBuilder ... builder for TauJet
- EventTag_jobOptions.py ... contains all builder job options
- It is possible to customize what goes in the TAG at runtime: look in EventTagFlags.py 

- for questions and comments: ketevi@bnl.gov or tmaeno@bnl.gov



*/
