# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# JetRecFlags.py
#
# David Adams
# Updated March 2015
#
# These are flags for controlling the behavior of jet reconstruction in RecExCommon
#
# Typical usage is
#   from JetRec.JetRecFlags import jetFlags
#   if jetFlags.UseTruth:
#     doSomethingWithTruth()
#
# Properties:
#   Enabled - Skip all jet reco if false (drop whe rec.doJets is added)
#   debug - JetRecTools are run a DEBUG level
#   useTruth - Truth jets and association are enabled (for MC)
#   useTopo  - Topocluster jets are enabled
#   useTracks - Track jets and association are enabled
#   useLargeD0Tracks - Large D0 track jets and association are enabled
#   useVertices - Toggles whether PFlow jet reconstruction makes use of vertex information.
#   useMuonSegmentss - Muon segemnt association is enabled
#   usePFlow - PFlow jets and associations are enabled\
#   useInDetTrackSelection - The inner detector track selection
#     tool is used. This requires track propagator exist.
#   jetAODList - The list of jet collections to be written out
#   writeJetsToAOD - bool to enable/disable writing jet collections to AOD
#   And much more--see below.

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class JetRecFlags(JobPropertyContainer):
  """ The Jet making flag property container
  """
  pass

class Enabled(JobProperty):
  """ If false will prevent run of any Jet Alg
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class debug(JobProperty):
  """ If > 0, debug (or higher) messages are written by jet tools.
  """
  statusOn     = True
  allowedTypes = ['int']  # type
  StoredValue  = 0        # default value

class useTruth(JobProperty):
  """ If true, truth is present and used in jet reconstruction.
      The status is set on in JetRecStandardToolManager.
  """
  statusOn     = False
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class truthFlavorTags(JobProperty):
  """ List of flavor tags for truth tagging jets.
  """
  statusOn     = True
  allowedTypes = ['array']  # type
  StoredValue  = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                  "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                  "TausFinal",
                  "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                  "Partons",
                 ]

class useTopo(JobProperty):
  """ If true, topoclusters are present and used in jet reconstruction.
      The status is set on in JetRecStandardToolManager.
  """
  statusOn     = False
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class useTracks(JobProperty):
  """ If true, tracks and vertices are present and used in jet reconstruction.
      The status is set on in JetRecStandardToolManager.
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value changed to False for Test by RL 

class useLargeD0Tracks(JobProperty):
  """ If true, large d0 tracks and vertices are present and used in jet
      reconstruction.  The status is set on in JetRecStandardToolManager.
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value changed to False for Test by RL 

class useVertices(JobProperty):
  """ If true, vertices are present and used in pflow jet reconstruction.
  """
  statusOn     = False
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class useMuonSegments(JobProperty):
  """ If true, muon segments are present and used in jet reconstruction.
      The status is set on in JetRecStandardToolManager.
  """
  statusOn     = False
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class usePFlow(JobProperty):
  """ If true, pflow objects are present and used in jet reconstruction.
      The status is set in JetRecStandardToolManager.
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class eventShapeTools(JobProperty):
  """ List of event shape tools that should be called to calculate rho.
      Allowed values are "emtopo", "lctopo", "emorig", "lcorig", "empflow", "emcpflow", "lcpflow".
  """
  statusOn     = True
  allowedTypes = ['None', 'list']  # type
  StoredValue  = None              # default value

class useInDetTrackSelection(JobProperty):
  """ If true, the InDet track selection tool is used.
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = False     # default value

class useCells(JobProperty):
  """ If true, calo cells are accesible
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = False     # default value

class useCaloQualityTool(JobProperty):
  """ If true, the (slow) CaloQuality tool is used
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class useBTagging(JobProperty):
  """ If true, then btagging is done when requested
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = False     # default value

class skipTools(JobProperty):
  """ List of modifier tools to exclude
  """
  statusOn     = True
  allowedTypes = ['list']  # type
  StoredValue  = []        # default value

class additionalTopoGetters(JobProperty):
  """ List of PseudoJet getters to add for Topo jets.
      E.g. to tag jets with track jets
  """
  statusOn     = True
  allowedTypes = ['list']  # type
  StoredValue  = []        # default value

class defaultCalibOpt(JobProperty):
  """ Calibration applied to topo jets during jet building. See JetRecCalibrationFinder.
  """
  statusOn     = True
  allowedTypes = ['str']    # type
  StoredValue  = ""   # default value

class useCalibJetThreshold(JobProperty):
  """ If true, jets are filtered based on their calibrated jet pT (15 GeV for cluster-based jets, 10 GeV for PFlow)
      If false, only pT threshold at constituent-scale is applied (5 GeV for cluster-based and PFlow jets)
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class useCalibThresholdsLCTopo(JobProperty):
  """ If true, LCTopo jets are filtered based on their calibrated jet pT (15 GeV)
      If false, only pT threshold at constituent-scale is applied (5 GeV)
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = False      # default value

class containerNamePrefix(JobProperty):
  """ Prefix for jet collection names
  """
  statusOn     = True
  allowedTypes = ['str']    # type
  StoredValue  = ""         # default value

class separateJetAlgs(JobProperty):
  """ If true, find and build jet containers in separate alg. Used for debugging.
  """
  statusOn     = True
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

class jetAODList(JobProperty):
  """ The collections to be saved in (x)AOD files
  """
  statusOn = True
  allowedTypes = ['list']
  StoredValue  = []

class writeJetsToAOD(JobProperty):
  """ Toggles whether to write jet collections to (x)AOD files
  """
  statusOn = True
  allowedTypes = ['bool']
  StoredValue = False

class useTrackVertexTool(JobProperty):
  """ Toggles whether to use track-vertex tool (only known client is currently pflow jet finding)
  """
  statusOn = True
  allowedTypes = ['bool']
  StoredValue = False

class JetContentDetail:
  Reduced=1
  Full=2
  Validation=3
  Trigger=4

class detailLevel(JobProperty):
  """ Toggles detail level for AOD jet reconstruction when running with RecExCommon
      If doWriteESD is set to True, then all built jet collections are written
      to the AODs otherwise as specified below:
      1. Reduced: This will build jet containers needed for monitoring:
        AntiKt4EMTopoJets, AntiKt4EMPFlowJets, AntiKt4LCTopoJets, AntiKt4TruthJets, AntiKt10LCTopoJets
        If writing to AOD turned on, only R = 0.4 reco jets will be written to disk
      2. Full: Same building of jet collections as 'Reduced'
        If writing to AOD turned on, all jet collections will be written to disk
      3. Validation: Not supported in r22
      4. Trigger: Same building of jet collections as 'Reduced' + trimmed R = 1.0 LCTopo jets
        All reco jet collections (R = 0.4, R = 1.0 ungroomed and groomed) are written to disk
  """
  statusOn = True
  allowedTypes = ['int']
  StoredValue = JetContentDetail.Reduced



jobproperties.add_Container(JetRecFlags)

jobproperties.JetRecFlags.add_JobProperty(Enabled)
jobproperties.JetRecFlags.add_JobProperty(debug)
jobproperties.JetRecFlags.add_JobProperty(useTruth)
jobproperties.JetRecFlags.add_JobProperty(truthFlavorTags)
jobproperties.JetRecFlags.add_JobProperty(useTopo)
jobproperties.JetRecFlags.add_JobProperty(useTracks)
jobproperties.JetRecFlags.add_JobProperty(useLargeD0Tracks)
jobproperties.JetRecFlags.add_JobProperty(useVertices)
jobproperties.JetRecFlags.add_JobProperty(useInDetTrackSelection)
jobproperties.JetRecFlags.add_JobProperty(useMuonSegments)
jobproperties.JetRecFlags.add_JobProperty(usePFlow)
jobproperties.JetRecFlags.add_JobProperty(eventShapeTools)
jobproperties.JetRecFlags.add_JobProperty(jetAODList)
jobproperties.JetRecFlags.add_JobProperty(writeJetsToAOD)
jobproperties.JetRecFlags.add_JobProperty(useCells)
jobproperties.JetRecFlags.add_JobProperty(useCaloQualityTool)
jobproperties.JetRecFlags.add_JobProperty(useBTagging)
jobproperties.JetRecFlags.add_JobProperty(skipTools)
jobproperties.JetRecFlags.add_JobProperty(additionalTopoGetters)
jobproperties.JetRecFlags.add_JobProperty(defaultCalibOpt)
jobproperties.JetRecFlags.add_JobProperty(useCalibJetThreshold)
jobproperties.JetRecFlags.add_JobProperty(useCalibThresholdsLCTopo)
jobproperties.JetRecFlags.add_JobProperty(containerNamePrefix)
jobproperties.JetRecFlags.add_JobProperty(separateJetAlgs)
jobproperties.JetRecFlags.add_JobProperty(useTrackVertexTool)
jobproperties.JetRecFlags.add_JobProperty(detailLevel)

jetFlags = jobproperties.JetRecFlags
