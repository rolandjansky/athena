# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####Jet Reco ############

# Before adding your algorithms to topSequence
from JetRec.JetGetters import *
from JetRec.JetRecFlags import jetFlags
from JetRecTools.JetRecToolsConf import *
from AthenaCommon.AlgSequence import AlgSequence
from JetRec.JetMomentGetter import  add_jetMoments_to_JetAlg, make_JetMomentGetter
from JetRec.JetRecConf import JetSorterTool
#from JetMomentTools.JetMomentToolsConf import JetWidthTool

import JetSubStructure

_GroomingTool =  {
     'Trimming'      : JetSubStructure.SimpleTrimmingTool,
     'BDRSFiltering' : JetSubStructure.BDRSFilteringTool,
}

def buildName(GroomedDict):

  if type(GroomedDict) != dict:
     raise TypeError("GroomedDicts must contain dicts!")

  if GroomedDict['Type'] in _GroomingTool:
     strconfig = ""
     theArgs = GroomedDict['args']
     for k in theArgs.keys():
         if type(theArgs[k]) == int:
            strconfig += k + '%d' % theArgs[k]
         elif type(theArgs[k]) == float:
            strconfig += k + '%d' % (theArgs[k]*100)

     toolname = strconfig

     return toolname
  else:
      raise KeyError("Couldn't find grooming tool %s" % GroomedDict['Type'])


def getGroomingTools(GroomedDicts):
    
  myGroomingTools = []
  catNames = []
  for gr in GroomedDicts:
     toolname = buildName(gr)
     defname =  _GroomingTool[gr['Type']].__slots__['Name']   ## safe hack ?

     defsubjetsname = ''
     if "SubjetsName" in _GroomingTool[gr['Type']].__slots__.keys():
        defsubjetsname = _GroomingTool[gr['Type']].__slots__['SubjetsName']

     kw = gr['args'].copy()
     if 'Name' not in kw.keys():
         kw['Name'] = defname
     kw['Name'] += toolname

     if 'SubjetsName' not in kw.keys() and defsubjetsname != '':
         kw['SubjetsName'] = defsubjetsname
     kw['SubjetsName'] += toolname

     myGroomingTools += [_GroomingTool[gr['Type']](gr['Type']+toolname, StoreConfigParam = True, **kw)]

     catNames += [ kw['Name'] ]

     ## are we requesting subjets ?
     if "SaveSubjets" in kw.keys():
        if kw["SaveSubjets"] ==	True:
       	   catNames += [ kw['SubjetsName'] ]

  return myGroomingTools, catNames  


######################
##    Truth Jets    ##
######################

def setupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq):
  if rec.doTruth():
    myGroomingTools, catNames = getGroomingTools(GroomedDicts)    
      
    sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3)

    assoc_tool_truth = JetSubStructure.JetAssociationTool(
                        name = 'JetAssocToolTruth',
                        GroomingTools = myGroomingTools, UsedSignalState = 2,
                        JetAlgTools = [sortertool], OutputLevel = 3
                       )

    myjetgetter_truth = make_StandardJetGetter(myjetfinder, myjetdr, 'Truth', seq = theseq, doCalib=False ,addDefaultMoment=False)
    truthcontname = myjetgetter_truth.outputKey() 

    make_JetMomentGetter(truthcontname , [assoc_tool_truth], theseq )

    return [ truthcontname, catNames ]
  else:
    return [ None, None ]

######################
##    Track Jets    ##
######################

def setupTrackJets(myjetfinder, myjetdr, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3 )

  assoc_tool_track = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocToolTrack',
                      GroomingTools = myGroomingTools, UsedSignalState = 2, 
                      JetAlgTools = [sortertool], OutputLevel = 3
                     )


  #---- TrackJet ----

  from JetSubStructure.JSTrackJets import createJSTrackJets
  mygetter = createJSTrackJets(theseq, myjetfinder,myjetdr)

  trackjetcontname =  mygetter.outputKey()

  #---- End TrackJet  ----

  make_JetMomentGetter(trackjetcontname , [assoc_tool_track], theseq )

  return [ trackjetcontname, catNames ]


######################
##    Calo Jets     ##
######################

def setupCaloJets(myjetfinder, myjetdr, myjetinput, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3 )

  grjets_calib = []

  ## Origin correction / Eta-Mass-JES hack
  
  from JetSubStructure.mySetupJetCalibrators import doEtaMassJESCorrection
  from JetCalibTools.MakeCalibSequences import calibTags
  calibTags['ETAMASSJES'] = doEtaMassJESCorrection

  mydoCalib = False
  mycalibName = ''

  if 'LCTopo' in myjetinput:
    mydoCalib =	True
    if (myjetfinder + '%d'%(myjetdr*10) == 'AntiKt10') or (myjetfinder + '%d'%(myjetdr*10) == 'CamKt12'):
      mycalibName = 'LC:ORIGIN_ETAMASSJES'
    else:
      mycalibName = 'LC:ORIGIN'

    stdcaliblist = []
    allowedcalib = ['AntiKt10LCTopoTrimmedJets', 'AntiKt10LCTopoTrimmedPtFrac3SmallR30Jets','AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets','CamKt12LCTopoTrimmedPtFrac5SmallR30Jets']
    for x in catNames:
       author  = myjetfinder + '%d'%(myjetdr*10) + myjetinput + x + 'Jets'
       if author in allowedcalib :
           grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin_etamassjes_'+x,
                                                               JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput+x, doCalib=True, calibName='LC:ORIGIN_ETAMASSJES')[0],
                                                               GroomedjetsAuthor = [x] )]
       else:
           stdcaliblist += [x]
                                                       
    if stdcaliblist != [] :
        grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin',
                                                            JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=True, calibName='LC:ORIGIN')[0],
                                                            GroomedjetsAuthor = stdcaliblist )]
  elif 'Topo' in myjetinput:
    mydoCalib = True
    mycalibName = 'EM:ORIGIN'
    grjets_calib += getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=True, calibName='EM:ORIGIN')
 
  assoc_tools = []
  if rec.doTruth():
    truthcontname = myjetfinder + '%dTruthJets' % (myjetdr*10)
    if hasattr(theseq, truthcontname):
      truthj_assoc = JetSubStructure.JetJetAssociatorTool("Truth_JetJetAssociatorTool", ContainerName = truthcontname, AssociationName = "TruthJets", MatchOrigin = False)
      
  trackjetcontname =  myjetfinder + '%dTrackZJets' % (myjetdr*10)
  if hasattr(theseq, trackjetcontname):
    trkj_assoc = JetSubStructure.JetJetAssociatorTool("Trk_JetJetAssociatorTool", ContainerName = trackjetcontname, AssociationName = "TrackJets", MatchOrigin = True)


  ## batgging ##
  subjets_names = [x for x in catNames if "Subjets" in x]
  btag_tool = []
  if subjets_names != '':
    from JetRec.JetGetters import getStandardBTaggerTool
    from JetRec.JetRecFlags import jetFlags
    jetFlags.doBTagging = True
    _btag_tool = getStandardBTaggerTool('AntiKt', 0.4, 'LCTopo')
    if len(_btag_tool) > 0:
      btag_tool += [ JetSubStructure.JetAlgToolWrapper(name = 'JetBTagger_' + ''.join(subjets_names), JetAlgTool = _btag_tool[0], 
                                                       GroomedjetsAuthor = subjets_names ) ] 
    jetFlags.doBTagging = False
  
  ## btagging end ##

  assoc_tool = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocTool',
                      AssociatorTools = assoc_tools, 
                      GroomingTools = myGroomingTools, UsedSignalState = 2,
                      JetAlgTools = grjets_calib +  [sortertool] + btag_tool, OutputLevel = 3
                     )


  myjetgetter = make_StandardJetGetter(myjetfinder, myjetdr, myjetinput, seq = theseq, doCalib=mydoCalib, calibName=mycalibName,addDefaultMoment=False)
  contname = myjetgetter.outputKey()
  

  # ------------------------------------------------
  # Associate a vertex and set the JVF moment. Must be placed AFTER vertex correction
  # ------------------------------------------------
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.AppMgr import ToolSvc
  noVertex = jobproperties.Beam.beamType == 'cosmics' or jobproperties.Beam.beamType == 'singlebeam'
  if DetFlags.detdescr.ID_on() and jetFlags.doJVF() and 'Truth' not in myjetinput and not noVertex:
     from JetSubStructure.mySetupJetMomentTools import mygetJetVertexAssociationTool
     myjvatool = mygetJetVertexAssociationTool(myjetfinder, myjetdr,myjetinput,'Custom%d'%(myjetdr*10))
     add_jetMoments_to_JetAlg(myjetgetter, [myjvatool])

  make_JetMomentGetter(contname , [assoc_tool], theseq )

  return [ contname, catNames ]


def createJSJets( ParentDict, GroomedDicts,  theseq = AlgSequence()) :
 
  if type(ParentDict) != dict:
    raise TypeError("ParentDict must be a dict!")

  if type(GroomedDicts) != list:
    raise TypeError("GroomedDicts must be a list!")

  myjetfinder = ParentDict["JetFinder"]
  myjetdr = ParentDict["JetdR"]
  myjetinput = ParentDict["JetInput"]

  if myjetinput == 'Truth':
    return setupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq)
  elif myjetinput == 'Track':
    return setupTrackJets(myjetfinder, myjetdr, GroomedDicts, theseq)
  else :
    return setupCaloJets(myjetfinder, myjetdr, myjetinput, GroomedDicts, theseq)

