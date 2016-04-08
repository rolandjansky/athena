# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####Jet Reco ############

# Before adding your algorithms to topSequence
from JetRec.JetGetters import *
from JetRec.JetRecFlags import jetFlags
from JetRecTools.JetRecToolsConf import *
from JetRec.JetMomentGetter import  add_jetMoments_to_JetAlg, make_JetMomentGetter
from JetRec.JetRecConf import JetSorterTool
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AlgSequence import AlgSequence

import JetSubStructure

_GroomingTool =  {
     'Trimming'      : JetSubStructure.SimpleTrimmingTool,
     #'BDRSFiltering' : JetSubStructure.BDRSFilteringTool,
     #'Pruning'       : JetSubStructure.PruningTool,
     'KtSubJets'     : JetSubStructure.KtSubJetsTool
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

     if gr['Type'] == 'KtSubJets':
        myGroomingTools += [ _GroomingTool[gr['Type']](gr['Type']+toolname,  **kw) ] # StoreConfigParam = True ... not implemented for KtSubJetsTool
     else:
        myGroomingTools += [ _GroomingTool[gr['Type']](gr['Type']+toolname, StoreConfigParam = True, **kw) ]

     catNames += [ kw['Name'] ]
  
  return myGroomingTools, catNames  


######################
##    Truth Jets    ##
######################

def setupTruthJets(myjetfinder, myjetdr, GroomedDicts, theseq):
  if rec.doTruth():

    myGroomingTools, catNames = getGroomingTools(GroomedDicts)    

    catNames_trimmed = list(x for x in catNames if 'Trimmed' in x)
      
    sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3 )
    wrapped_sortertool = JetSubStructure.JetAlgToolWrapper("JetSorterWrapper", JetAlgTool = sortertool, GroomedjetsAuthor = catNames_trimmed)

    widthtool = JetWidthTool('JS_JetWidthTool')

    truth_assoc = JetSubStructure.TruthParticleJetAssociatorTool("myTruthLeptonsAssoc", ContainerName = "SpclMC", dR = myjetdr, OutputLevel=3, #SpclMC for AODs, INav4MomTruthEvent for ESD
                                                                   ParticlesPdgID = [11,13], ParticlesStatus = [1, 1], ParticlesType=[2,6], ParticlesOrigin=[-1,-1])
    truth_assoc.mcTruthClassifier.OutputLevel=3
    truth_assoc.mcTruthClassifier.McEventCollection = "GEN_AOD" #"TruthEvent" for ESD
    from MCTruthClassifier.MCTruthClassifierBase import exToCalo
    truth_assoc.mcTruthClassifier.ExtrapolateToCaloTool = exToCalo

    assoc_tool_truth = JetSubStructure.JetAssociationTool(
                        name = 'JetAssocToolTruth',
                        AssociatorTools = [truth_assoc],
                        GroomingTools = myGroomingTools, UsedSignalState = 2,
                        JetAlgTools = [wrapped_sortertool, widthtool], OutputLevel = 3
                       )

    jetsubstructure_tool_truth = JetSubStructure.JetSubstructureTool( 
                                  name = 'JetSubstructureToolTruth', UsedSignalState = 2, OutputLevel=3,
                                  SubstructureTools = [JetSubStructure.KtTruthLeptonSubstructureTool("mytruthleptontool", OutputLevel=3, GroomedjetsAuthor = []),
                                                       JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames_trimmed),
                                                       JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames_trimmed),
                                                       JetSubStructure.JetPullVectorTool("myJetPullVectorTool",  GroomedjetsAuthor = catNames_trimmed) ] )
    #need to use truth collection on disk for smaller R jets for the TrueFlavorComponent block
    if myjetdr < 0.9:
      contname = myjetfinder + '%dTruthJets' % (myjetdr*10) 
    else:
      contname = myjetfinder + '%dTruthJSJets' % (myjetdr*10) 

    myjetgetter_truth = make_StandardJetGetter(myjetfinder, myjetdr, 'Truth', seq = theseq, doCalib=False, outputCollectionName = contname) #,addDefaultMoment=False)
    truthcontname = myjetgetter_truth.outputKey()

    extra_moments_list = [] 
    if myjetgetter_truth.jetAlgorithmHandle() == None:
      if truthcontname ==  None:
         truthcontname = myjetgetter_truth.buildName()
      print 'yot: ', truthcontname
      extra_moments_list += [widthtool]  ## jet moments lost for truth jets in ESD->AOD transition

    sss = make_JetMomentGetter(truthcontname , extra_moments_list + [assoc_tool_truth,jetsubstructure_tool_truth], theseq )

    ## for lepton assoc
    jetcont = truthcontname[:-4] #removing the 'Jets'
    for x in catNames_trimmed:
        make_JetMomentGetter(jetcont+x+'Jets', [JetSubStructure.JetAssociationTool('JetAssocToolTruthGr', AssociatorTools = [truth_assoc], GroomingTools =[], UsedSignalState = 2, JetAlgTools = []),
                                                JetSubStructure.JetSubstructureTool('JetSubstructureToolTruthGr', UsedSignalState = 2, SubstructureTools = [JetSubStructure.KtTruthLeptonSubstructureTool("mytruthleptontool", GroomedjetsAuthor = [])]) ], 
                             theseq)

    return [ truthcontname, catNames ]
  
  else:
    return [ None, None ]


######################
##    Track Jets    ##
######################

def setupTrackJets(myjetfinder, myjetdr, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  catNames_trimmed = list(x for x in catNames if 'Trimmed' in x)

  sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3 )
  wrapped_sortertool = JetSubStructure.JetAlgToolWrapper("JetSorterWrapper", JetAlgTool = sortertool, GroomedjetsAuthor = catNames_trimmed)  

  widthtool = JetWidthTool('JS_JetWidthTool')

  e_assoc = JetSubStructure.ElectronJetAssociatorTool("JSEAssocTrack", ContainerName = 'ElectronAODCollection', UseCluster = False, UseTrack = True, dR = myjetdr, OutputLevel=3)
  mu_assoc = JetSubStructure.MuonJetAssociatorTool("JSMuAssocTrack", ContainerName = 'MuidMuonCollection', UseTrack = True, dR = myjetdr, OutputLevel=3)

  assoc_tool_track = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocToolTrack',
                      AssociatorTools = [e_assoc, mu_assoc],
                      GroomingTools = myGroomingTools, UsedSignalState = 2, 
                      JetAlgTools = [wrapped_sortertool, widthtool], OutputLevel = 3
                     )

  jetsubstructure_tool_track = JetSubStructure.JetSubstructureTool(
                                  name = 'JetSubstructureToolTrack', UsedSignalState = 2, OutputLevel=3,
                                  SubstructureTools = [JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames_trimmed),
                                                       JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames_trimmed),
                                                       JetSubStructure.KtElectronSubstructureTool("myelectool", GroomedjetsAuthor = []),
                                                       JetSubStructure.KtMuonSubstructureTool("mymuontool", GroomedjetsAuthor = []),
                                                       JetSubStructure.JetPullVectorTool("myJetPullVectorTool",  GroomedjetsAuthor = catNames_trimmed) ] )


  #---- TrackJet ----
  #jetFlags.finalMinEt = 5*GeV

  from BoostedTopD3PDMaker.JSTrackJets import createJSTrackJets
  mygetter = createJSTrackJets(theseq, myjetfinder,myjetdr)

  trackjetcontname =  mygetter.outputKey()

  add_jetMoments_to_JetAlg(mygetter, [widthtool])

  #---- End TrackJet  ----

  sss = make_JetMomentGetter(trackjetcontname , [assoc_tool_track, jetsubstructure_tool_track], theseq )

  ## for lepton assoc  
  jetcont = trackjetcontname[:-4] #removing the 'Jets'
  for x in catNames_trimmed:
      make_JetMomentGetter(jetcont+x+'Jets', [JetSubStructure.JetAssociationTool('JetAssocToolTrackGr', AssociatorTools = [e_assoc, mu_assoc], GroomingTools =[], UsedSignalState = 2, JetAlgTools = []),
                                              JetSubStructure.JetSubstructureTool('JetSubstructureToolTrackGr', UsedSignalState = 2, SubstructureTools = [ JetSubStructure.KtElectronSubstructureTool("myelectool", GroomedjetsAuthor = []), 
                                                                                                                                      JetSubStructure.KtMuonSubstructureTool("mymuontool", GroomedjetsAuthor = []) ]) ],
                           theseq)
         
  

  return [ trackjetcontname, catNames ]


######################
##    Calo Jets     ##
######################

def setupCaloJets(myjetfinder, myjetdr, myjetinput, GroomedDicts, theseq):

  myGroomingTools, catNames = getGroomingTools(GroomedDicts)

  catNames_trimmed = list(x for x in catNames if 'Trimmed' in x)

  sortertool = JetSorterTool('myJetSorter', SortOrder="ByPtDown", OutputLevel=3 )
  wrapped_sortertool = JetSubStructure.JetAlgToolWrapper("JetSorterWrapper", JetAlgTool = sortertool, GroomedjetsAuthor = catNames_trimmed)

  widthtool = JetWidthTool('JS_JetWidthTool')

  grjets_calib = []

  from JetSubStructure.mySetupJetCalibrators import doEtaMassJESCorrection
  from JetCalibTools.MakeCalibSequences import calibTags
  calibTags['ETAMASSJES'] = doEtaMassJESCorrection

  ## Origin correction

  mydoCalib = False
  mycalibName = ''

  if 'LCTopo' in myjetinput:
    mydoCalib =	False
    if (myjetfinder + '%d'%(myjetdr*10) == 'AntiKt10') or (myjetfinder + '%d'%(myjetdr*10) == 'CamKt12'):
       mycalibName = 'LC:ORIGIN_ETAMASSJES'
    else:
       mycalibName = 'LC:ORIGIN'
   
    stdcaliblist = []
    allowedcalib = ['AntiKt10LCTopoTrimmedJets', 'AntiKt10LCTopoTrimmedPtFrac3SmallR30Jets','AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets','CamKt12LCTopoTrimmedPtFrac5SmallR30Jets']
    for x in catNames:
       author  = myjetfinder + '%d'%(myjetdr*10) + myjetinput + x + 'Jets'
       #if author in allowedcalib :
           #grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin_etamassjes_'+x, 
           #                                                    JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput+x, doCalib=True, calibName='LC:ORIGIN_ETAMASSJES')[0],
           #                                                    GroomedjetsAuthor = [x] )]
       #elif 'Trimmed' in x:
       #    stdcaliblist += [x]

    #if stdcaliblist != [] :
        #grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin',
        #                                                    JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=True, calibName='LC:ORIGIN')[0],
        #                                                    GroomedjetsAuthor = stdcaliblist )]
  elif 'Topo' in myjetinput:
    mydoCalib = True
    mycalibName = 'EM:ORIGIN'
    grjets_calib += [JetSubStructure.JetAlgToolWrapper(name = 'calibtool_origin',
                                                        JetAlgTool = getStandardCalibTool(myjetfinder,myjetdr, myjetinput, doCalib=True, calibName='EM:ORIGIN')[0],
                                                        GroomedjetsAuthor = catNames_trimmed )]
 
  truthcontname = 'none'
  if rec.doTruth():
    if myjetdr < 0.9: #need to use truth collection on disk for smaller R jets for the TrueFlavorComponent block
      truthcontname = myjetfinder + '%dTruthJets' % (myjetdr*10) 
    else:
      truthcontname = myjetfinder + '%dTruthJSJets' % (myjetdr*10) 

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

  e_assoc = JetSubStructure.ElectronJetAssociatorTool("JSEAssoc", ContainerName = 'ElectronAODCollection', UseCluster = True, UseTrack = False, dR = myjetdr,  ResolveCluster=True, OutputLevel=3)

  e_assoc.METClusterResolver.MaximumRadialDistance = 0.2
  e_assoc.METClusterResolver.LongitudinalExtension = 1.0
  e_assoc.METClusterResolver.RadialExtension = 1.0
  e_assoc.METClusterResolver.EnergyFractionThreshold = 0.1
  e_assoc.METClusterResolver.OutputLevel = 3

  mu_assoc = JetSubStructure.MuonJetAssociatorTool("JSMuAssoc", ContainerName = 'MuidMuonCollection', UseTrack = False, dR = myjetdr, OutputLevel=3)
  assoc_tools += [ e_assoc, mu_assoc]

  assoc_tool = JetSubStructure.JetAssociationTool(                                                                                  
                      name = 'JetAssocTool',
                      AssociatorTools = assoc_tools,
                      GroomingTools = myGroomingTools, UsedSignalState = 2,
                      JetAlgTools = grjets_calib +  [wrapped_sortertool, widthtool], OutputLevel = 5
                     )

  jetsubstructure_tool = JetSubStructure.JetSubstructureTool(
                           name = 'JetSubstructureTool', UsedSignalState = 2, OutputLevel=3,
                           SubstructureTools = [JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=myjetdr, GroomedjetsAuthor = catNames_trimmed),
                                                JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = catNames_trimmed),
                                                JetSubStructure.KtElectronSubstructureTool("myelectool", GroomedjetsAuthor = []),
                                                JetSubStructure.KtMuonSubstructureTool("mymuontool", GroomedjetsAuthor = []),
                                                JetSubStructure.JetPullVectorTool("myJetPullVectorTool",  GroomedjetsAuthor = catNames_trimmed) ] )



  outkey = myjetfinder + '%d'%(myjetdr*10) + myjetinput
  if myjetinput == 'Topo':
     outkey += 'EM'
  outkey += 'Jets' #'JSJets' for rebuilding on the fly
 
  myjetgetter = make_StandardJetGetter(myjetfinder, myjetdr, myjetinput, seq = theseq, doCalib=mydoCalib, calibName=mycalibName,addDefaultMoment=False,
                                       outputCollectionName = outkey)

  contname = myjetgetter.outputKey()

  extra_moments_list = []
  if myjetgetter.jetAlgorithmHandle() == None:     # JetCollection already in AOD
      if contname == None:
         contname=myjetgetter.buildName()

      if contname == 'AntiKt4LCTopoJets':
         contname = 'AntiKt4LCTopoJetsReTagged'
  else:
      extra_moments_list += [widthtool] 

  
  # ------------------------------------------------
  # Associate a vertex and set the JVF moment. Must be placed AFTER vertex correction
  # ------------------------------------------------
  from AthenaCommon.DetFlags import DetFlags
  from AthenaCommon.AppMgr import ToolSvc
  noVertex = jobproperties.Beam.beamType == 'cosmics' or jobproperties.Beam.beamType == 'singlebeam'
  if DetFlags.detdescr.ID_on() and jetFlags.doJVF() and 'Truth' not in myjetinput and not noVertex and myjetdr >= 0.7:
     from JetSubStructure.mySetupJetMomentTools import mygetJetVertexAssociationTool
     myjvatool = mygetJetVertexAssociationTool(myjetfinder, myjetdr,myjetinput,'Custom%d'%(myjetdr*10))
     #add_jetMoments_to_JetAlg(myjetgetter, [myjvatool])
     extra_moments_list += [myjvatool]

  make_JetMomentGetter(contname , extra_moments_list + [assoc_tool, jetsubstructure_tool], theseq )


  ## for lepton assoc
  jetcont = contname[:-4] #removing the 'Jets'
  if contname == 'AntiKt4LCTopoJetsReTagged':
    jetcont = 'AntiKt4LCTopo'

  for x in catNames_trimmed:
      make_JetMomentGetter(jetcont+x+'Jets', [JetSubStructure.JetAssociationTool('JetAssocToolGr', AssociatorTools = [e_assoc, mu_assoc], GroomingTools =[], UsedSignalState = 2, JetAlgTools = [], OutputLevel = 5),
                                              JetSubStructure.JetSubstructureTool('JetSubstructureToolGr',UsedSignalState = 2,  SubstructureTools = [ JetSubStructure.KtElectronSubstructureTool("myelectool", GroomedjetsAuthor = []), 
                                                                                                                                                      JetSubStructure.KtMuonSubstructureTool("mymuontool", GroomedjetsAuthor = []) ]) ],
                           theseq)
    

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

