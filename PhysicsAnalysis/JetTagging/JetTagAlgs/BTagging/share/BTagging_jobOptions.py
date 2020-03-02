########################################
# BTagging_jobOptions.py
# author: andreas.wildauer@cern.ch
#         devivie@lal.in2p3.fr
#         vacavant@in2p3.fr 
#
# Main jobO for b-tagging:
# - load all the necessary tools
# - configure the b-tagging algorithms
########################################

# <================= IMPORTANT ==============================================>
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: The inclusion of the LoadTools is no longer needed with the
  # new configuration; the default configuration is automatically set up
  # for any unconfigured jet collection where which setupJetBTaggerTool
  # is called. In fact the only thing the LoadTools does now is just call
  # this default setup on all jet collections in BTaggingFlags.Jets.
  #
  # If you need to modify the default setup permanently can modify
  # BTaggingConfiguration_LoadTools.py in the ./python/ directory.
  #
  # If you want different settings not obtainable via the BTaggingFlags,
  # you need to use the new configuration scheme before any call to
  # setupJetBTaggerTools is made for the jet collection in question.
  # You can start by calling BTaggingConfiguration_LoadTools.py's
  # Initiate() function.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# <================= IMPORTANT ==============================================>

# <================= IMPORTANT ==============================================>
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # NOTE: Consider adding some of the stuff found here to the Initiate()
  # function in BTaggingConfiguration_LoadTools.py. The code there is
  # run exactly once; if B-tagging is enabled.
  #
  # DOING SO WILL MAKE THE CODE COMPATIBLE WITH A FUTURE CHANGE IN JETREC
  # WHERE THEY WILL RETRIEVE OUR BTAGGING FUNCTION IF REQUESTED BY A USER.
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# <================= IMPORTANT ==============================================>

import re

if not BTaggingFlags.DoNotSetupBTagging: # Temporary measure so the JetRec people can test setting this all up from their side.
  #
  # ========== Load and configure everything
  #
  
  from BTagging.BTaggingConfiguration import getConfiguration
  ConfInstance = getConfiguration()

  if ConfInstance.checkFlagsUsingBTaggingFlags():

    #Jet collections
    #JetCollectionList = ['AntiKt4LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4TrackJets', 'AntiKt4EMPFlowJets', 'AntiKt2TrackJets']
    JetCollectionList = ['AntiKt4EMTopoJets']

    BTaggingFlags.Jets = [ name[:-4] for name in JetCollectionList]

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    for i, jet in enumerate(JetCollectionList):
          btagger = ConfInstance.setupJetBTaggerAlg(ToolSvc, jet) #The [:-4] is not needed here; this function automatically removes trailing 'jets' or 'Jets'.
          if btagger is None:
            continue
          topSequence += btagger
          #jet = jet.replace("Track", "PV0Track")
          #jetname = getattr(jtm, jet)
          #jetname.unlock()
          #jetname.JetModifiers += [ btagger ]
          #jetname.lock()
          if BTaggingFlags.OutputLevel < 3:
            printfunc (ConfInstance.getJetCollectionTool(jet[:-4]))

