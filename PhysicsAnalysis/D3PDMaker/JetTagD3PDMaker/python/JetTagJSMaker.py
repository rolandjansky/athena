# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetGetters import *
from JetRec.JetRecFlags import jetFlags
from JetRecTools.JetRecToolsConf import *
from AthenaCommon.AlgSequence import AlgSequence
from JetRec.JetMomentGetter import  add_jetMoments_to_JetAlg, make_JetMomentGetter
from JetRec.JetRecConf import JetSorterTool
import JetSubStructure

from HSG5DPDUtils.JSjets import *

### overload function from HSG5DPDUtils.JSjets to be able tag truth jets

def jtsetupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq):
  if rec.doTruth():
    myGroomingTools, catNames = getGroomingTools(GroomedDicts)   
     
    sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3)

### tagging truth subjets
    subjets_names = [x for x in catNames if "Subjets" in x]
    btag_tool = []
    if subjets_names != '':
        from JetRec.JetGetters import getStandardBTaggerTool
        from JetRec.JetRecFlags import jetFlags
        jetFlags.doBTagging = True
        _btag_tool = getStandardBTaggerTool('AntiKt', 0.4, 'Truth')
        if len(_btag_tool) > 0:
            btag_tool += [ JetSubStructure.JetAlgToolWrapper(name = 'JetBTagger_' + ''.join(subjets_names)
                                                             , JetAlgTool = _btag_tool[0], 
                                                             GroomedjetsAuthor = subjets_names ) ] 
        jetFlags.doBTagging = False


    assoc_tool_truth = JetSubStructure.JetAssociationTool(
                        name = 'JetAssocToolTruth',
                        GroomingTools = myGroomingTools, UsedSignalState = 2,
                        JetAlgTools = [sortertool]+btag_tool, OutputLevel = 3
                       )

    myjetgetter_truth = make_StandardJetGetter(myjetfinder, 
                                               myjetdr, 'Truth', 
                                               seq = theseq, doCalib=False ,
                                               addDefaultMoment=False)

    truthcontname = myjetgetter_truth.outputKey() 

    make_JetMomentGetter(truthcontname , [assoc_tool_truth], theseq )

    return [ truthcontname, catNames ]
  else:
    return [ None, None ]


def jtcreateJSJets( ParentDict, GroomedDicts,  theseq = AlgSequence()) :
 
  if type(ParentDict) != dict:
    raise TypeError("ParentDict must be a dict!")

  if type(GroomedDicts) != list:
    raise TypeError("GroomedDicts must be a list!")

  myjetfinder = ParentDict["JetFinder"]
  myjetdr = ParentDict["JetdR"]
  myjetinput = ParentDict["JetInput"]

  if myjetinput == 'Truth':
    return jtsetupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq)
  elif myjetinput == 'Track':
    return setupTrackJets(myjetfinder, myjetdr, GroomedDicts, theseq)
  else :
    return setupCaloJets(myjetfinder, myjetdr, myjetinput, GroomedDicts, theseq)
