# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####Jet Reco ############

# Before adding your algorithms to topSequence
from JetRec.JetGetters import *
from JetRec.JetRecFlags import jetFlags
from JetRecTools.JetRecToolsConf import *
from AthenaCommon.AlgSequence import AlgSequence

import JetSubStructure

from JetRec.JetMomentGetter import  add_jetMoments_to_JetAlg, make_JetMomentGetter
from JetRec.JetRecConf import JetSorterTool
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from AthenaCommon.SystemOfUnits import GeV

_GroomingTool =  {
     'Trimming'      : JetSubStructure.SimpleTrimmingTool,
     'BDRSFiltering' : JetSubStructure.BDRSFilteringTool,
     'Pruning'       : JetSubStructure.PruningTool
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

     kw = gr['args'].copy()
     if 'Name' not in kw.keys():
         kw['Name'] = defname
     kw['Name'] += toolname

     myGroomingTools += [ _GroomingTool[gr['Type']](gr['Type']+toolname, StoreConfigParam = True, OutputLevel=3, **kw) ]
     catNames += [ kw['Name'] ]
  
  return myGroomingTools, catNames  


######################
##    Truth Jets    ##
######################

def setupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq):
  if rec.doTruth():

    myGroomingTools, catNames = getGroomingTools(GroomedDicts)    
      
    sortertool = JetSorterTool('JS_JetSorter', SortOrder="ByPtDown", OutputLevel=3 )
    widthtool = JetWidthTool('JS_JetWidthTool')

    assoc_tools = [ JetSubStructure.GhostJetJetAssociatorTool("myTruthGhostJetJetAssociatorTool", JetdR = myjetdr, JetFinder = myjetfinder, JetInputTool = getStandardInputTools(input='Truth')[0],OutputLevel=3) ]

    assoc_tool_truth = JetSubStructure.JetAssociationTool(
                        name = 'JetAssocToolTruth',
                        AssociatorTools = assoc_tools, Area = True,
                        GroomingTools = myGroomingTools, UsedSignalState = 2,
                        JetAlgTools = [sortertool,widthtool], OutputLevel = 3
                       )

    jetsubstructure_tool_truth = JetSubStructure.JetSubstructureTool( 
                                  name = 'JetSubstructureToolTruth', UsedSignalState = 2, OutputLevel=3,
                                  SubstructureTools = [JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames),
                                              JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames),
                                              JetSubStructure.DipolarityTool("myDipolarityTool",GroomedjetsAuthor = catNames),
                                              JetSubStructure.JetVoronoiAreaCalcTool("myVoronoiArea", GroomedjetsAuthor = catNames, JetdR = myjetdr, JetInputTool = getStandardInputTools(input='Truth')[0]) ])

    myjetgetter_truth = make_StandardJetGetter(myjetfinder, myjetdr, 'Truth', seq = theseq, doCalib=False, minPt=30*GeV)#,addDefaultMoment=False)
    truthcontname = myjetgetter_truth.outputKey()

    extra_moments_list = [] 
    if myjetgetter_truth.jetAlgorithmHandle() == None:
      if truthcontname ==  None:
         truthcontname = myjetgetter_truth.buildName()
      print 'yot: ', truthcontname
      extra_moments_list += [widthtool]  ## jet moments lost for truth jets in ESD->AOD transition

    make_JetMomentGetter(truthcontname , extra_moments_list + [assoc_tool_truth,jetsubstructure_tool_truth], theseq )


    return [ truthcontname, catNames ]
  
  else:
    return [ None, None ]


######################
##    Track Jets    ##
######################

def setupTrackJets(myjetfinder, myjetdr, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  sortertool = JetSorterTool('JS_JetSorter', SortOrder="ByPtDown", OutputLevel=3 )
  widthtool = JetWidthTool('JS_JetWidthTool')

  assoc_tool_track = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocToolTrack',
                      AssociatorTools = [],
                      GroomingTools = myGroomingTools, UsedSignalState = 2, 
                      JetAlgTools = [sortertool, widthtool], OutputLevel = 3
                     )

  jetsubstructure_tool_track = JetSubStructure.JetSubstructureTool(
                                  name = 'JetSubstructureToolTrack', UsedSignalState = 2, OutputLevel=3,
                                  SubstructureTools = [JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames),
                                              JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames),
                                              JetSubStructure.DipolarityTool("myDipolarityTool",GroomedjetsAuthor = catNames) ])


  #---- TrackJet ----
  jetFlags.finalMinEt = 30*GeV

  from QcdD3PDMaker.JSTrackJets import createJSTrackJets
  mygetter = createJSTrackJets(theseq, myjetfinder,myjetdr)

  jetFlags.finalMinEt =30*GeV

  trackjetcontname =  mygetter.outputKey()

  add_jetMoments_to_JetAlg(mygetter, [widthtool])

  #---- End TrackJet  ----

  make_JetMomentGetter(trackjetcontname , [assoc_tool_track, jetsubstructure_tool_track], theseq )

  return [ trackjetcontname, catNames ]


######################
##    Calo Jets     ##
######################

def setupCaloJets(myjetfinder, myjetdr, myjetinput, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  sortertool = JetSorterTool('JS_JetSorter', SortOrder="ByPtDown", OutputLevel=3 )
  widthtool = JetWidthTool('JS_JetWidthTool')

  grjets_calib = []

  ## Origin correction

  mydoCalib = False
  mycalibName = ''

  from JetSubStructure.mySetupJetCalibrators import doEtaMassJESCorrection
  from JetCalibTools.MakeCalibSequences import calibTags
  calibTags['ETAMASSJES'] = doEtaMassJESCorrection

  if 'LCTopo' in myjetinput:
    #mydoCalib =	True
    mydoCalib =	False
    if (myjetfinder + '%d'%(myjetdr*10) == 'AntiKt10') or (myjetfinder + '%d'%(myjetdr*10) == 'CamKt12'):
       mycalibName = 'LC:ORIGIN_ETAMASSJES'
    else:
       mycalibName = 'LC:ORIGIN'

    stdcaliblist = []
    allowedcalib = ['AntiKt10LCTopoTrimmedJets', 'AntiKt10LCTopoTrimmedPtFrac3SmallR30Jets','AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets','CamKt12LCTopoTrimmedPtFrac5SmallR30Jets']
    for x in catNames:
       author  = myjetfinder + '%d'%(myjetdr*10) + myjetinput + x + 'Jets'
       if author in allowedcalib :
           #grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin_etamassjes_'+x, JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput+x, doCalib=True, calibName='LC:ORIGIN_ETAMASSJES')[0], GroomedjetsAuthor = stdcaliblist )]
           #grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin_etamassjes_'+x, JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput+x, doCalib=False)[0], GroomedjetsAuthor = stdcaliblist) ]
           pass
       else:
           stdcaliblist += [x]

    if stdcaliblist != [] :
        #grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin',JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=False, calibName='LC:ORIGIN')[0], GroomedjetsAuthor = stdcaliblist )]
        pass
  elif 'Topo' in myjetinput:
    #mydoCalib = True
    mydoCalib = False
    mycalibName = 'EM:ORIGIN'
    #grjets_calib += getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=True, calibName='EM:ORIGIN')
    grjets_calib += getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=False, calibName='EM:ORIGIN')
 

  truthcontname = 'none'
  if rec.doTruth():
    truthcontname = myjetfinder + '%dTruthJets' % (myjetdr*10)
  trackjetcontname =  myjetfinder + '%dTrackZJets' % (myjetdr*10)


  assoc_tools = []

  trkj_assoc = JetSubStructure.JetJetAssociatorTool("Trk_JetJetAssociatorTool", ContainerName = trackjetcontname,
                                                                               AssociationName = "TrackJets",
                                                                               MatchOrigin = True)
  assoc_tools += [ trkj_assoc ]

  if rec.doTruth():
    truthj_assoc = JetSubStructure.JetJetAssociatorTool("Truth_JetJetAssociatorTool", ContainerName = truthcontname,
                                                                               AssociationName = "TruthJets",
                                                                               MatchOrigin = False)
    assoc_tools += [ truthj_assoc ]

  doArea = False
  if 'LCTopo' in myjetinput:
    doArea = True
    assoc_tools += [ JetSubStructure.GhostJetJetAssociatorTool("myGhostJetJetAssociatorTool", JetdR = myjetdr, JetFinder = myjetfinder, JetInputTool = getStandardInputTools(input=myjetinput)[0],OutputLevel=3) ]

  assoc_tool = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocTool',
                      AssociatorTools = assoc_tools, 
                      GroomingTools = myGroomingTools, UsedSignalState = 2, Area = doArea,
                      JetAlgTools = grjets_calib +  [sortertool, widthtool], OutputLevel = 3
                     )

  jetsubstructure_tool = JetSubStructure.JetSubstructureTool(
                           name = 'JetSubstructureTool', UsedSignalState = 2, OutputLevel=3,
                           SubstructureTools = [JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames),
                                       JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames),
                                       JetSubStructure.DipolarityTool("myDipolarityTool",GroomedjetsAuthor = catNames),
                                       JetSubStructure.JetVoronoiAreaCalcTool("myVoronoiArea", GroomedjetsAuthor = catNames, JetdR = myjetdr, JetInputTool = getStandardInputTools(input=myjetinput)[0]) ])


  outkey = myjetfinder + '%d'%(myjetdr*10) + myjetinput
  if myjetinput == 'Topo':
     outkey += 'EM'
  outkey += 'Jets'
 
  myjetgetter = make_StandardJetGetter(myjetfinder, myjetdr, myjetinput, seq = theseq, doCalib=mydoCalib, calibName=mycalibName,addDefaultMoment=False, minPt=30*GeV,
                                       outputCollectionName = outkey)
  contname = myjetgetter.outputKey()

  extra_moments_list = []
  if myjetgetter.jetAlgorithmHandle() == None:     # JetCollection already in AOD
      if contname == None:
         contname=myjetgetter.buildName()
      print 'yo2: ', contname
  else:
      extra_moments_list += [widthtool] 


  #add_jetMoments_to_JetAlg(myjetgetter, [JetWidthTool()])

  # ------------------------------------------------
  # Associate a vertex and set the JVF moment. Must be placed AFTER vertex correction
  # ------------------------------------------------
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.AppMgr import ToolSvc
  noVertex = jobproperties.Beam.beamType == 'cosmics' or jobproperties.Beam.beamType == 'singlebeam'
  if DetFlags.detdescr.ID_on() and jetFlags.doJVF() and 'Truth' not in myjetinput and not noVertex:
     from JetSubStructure.mySetupJetMomentTools import mygetJetVertexAssociationTool
     myjvatool = mygetJetVertexAssociationTool(myjetfinder, myjetdr,myjetinput,'Custom%d'%(myjetdr*10))
     #add_jetMoments_to_JetAlg(myjetgetter, [myjvatool])
     extra_moments_list += [myjvatool]

  make_JetMomentGetter(contname , extra_moments_list + [assoc_tool, jetsubstructure_tool], theseq )

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

