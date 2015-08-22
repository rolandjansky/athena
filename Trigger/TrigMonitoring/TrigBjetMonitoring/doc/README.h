/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************

The TrigBjetMonitoring package provides histograms for 
monitoring the b-jet trigger performances offline on
Tier 0 reconstructed data.

It is steared by the HLTBjetMonTool class, see 
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigMonitoring/TrigBjetMonitoring/trunk/TrigBjetMonitoring/HLTBjetMonTool.h
and
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigMonitoring/TrigBjetMonitoring/trunk/src/HLTBjetMonTool.cxx

The main methods of the HLTBjetMonTool class are:

HLTBjetMonTool::book()
which books histograms and
HLTBjetMonTool::fill()
which checks if the triggers forseen for monitoring have been configured and fired,
and if yes, it fills the relevant histograms.

At present the following histograms are provided:
(for their description see also at https://twiki.cern.ch/twiki/bin/viewauth/Atlas/InnerDetectorBJetOnCall#BJets)

1. Primary vertices (PV) reconstructed off-line based on <xAOD::VertexContainer>("PrimaryVertices"):
      Number of PV
      x, y and z coordinates of the PV's

2. Quantities which passed the online trigger in different combinations:

   2.1 PV based on <xAOD::VertexContainer>("EFHistoPrmVtx"):
          Number of PV  
          x, y and z coordinates of the PV's 
          difference of the z coordinates of the each online PV and the first offline PV
       Note that alternatively, PV's based on <xAOD::VertexContainer>("xPrimVx") can be filled by commenting out
          the #define HISTO directive.

   2.2 Tracks based on <xAOD::TrackParticleContainer>():
          Number of tracks
          d0, z0 of tracks and their errors: ed0 and ez0
          z0 of tracks wrt the first off-line PV and that normalized to ez0 (significance)
          pT of tracks
          Phi vs Eta of tracks

   2.3 Jets based on <xAOD::JetContainer>():
          Number of jets
          pT of jets
          Phi vs Eta of jets

   2.4 b-jets based on <xAOD::BTaggingContainer>():
          Likelihood of the 3 dimensional impact parameter (IP3D) of light, c- and b-jets
          Logarithm of IP3D likelihood ratios of b- to light jets
          Logarithm of secondary vertex (SV1) likelihood ratios of b- to light jets
          Logarithm of joint IP3D and SV1 likelihood ratios of b- to light jets
  
   2.5 b-jets based on <TrigEFBjetContainer>(): 
          MV1 discriminant of the multivariate b-tagging algorithm
          Invariant mass of tracks from secondary vertices
          Number of secondary vertices
   
3. There is a possibility to retrieve "truth jets" in case of simulation, but for the moment no
         histograms are filled for these objects.

The triggers forseen are enumerated in the
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigMonitoring/TrigBjetMonitoring/trunk/python/TrigBjetMonitoringConfig.py
and 
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigMonitoring/TrigBjetMonitoring/trunk/python/TrigBjetMonitCategory.py
python configuration files.

Finally, in the TrigBjetMonitoring package we definitely eliminated several classes: 
    BjetLikelihoodMon
    BjetProbabilityMon
    TaggerHelper
    TuningLikelihood
which were used in Run 1 and used no more in Run 2 to resolve coverity issues present in these classes. 
     

***************/

