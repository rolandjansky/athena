# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from HIJetRec.HIJetRecFlags import HIJetFlags
from HIJetRec.HIJetRecTools import jtm
from JetRec.JetRecFlags import jetFlags

def AddToOutputList(tname, objType='xAOD::JetContainer') : 

    #filter container based on package flags
    if HIJetFlags.UnsubtractedSuffix() in tname and not HIJetFlags.WriteUnsubtracted() : return
    if HIJetFlags.SeedSuffix() in tname and not HIJetFlags.WriteSeeds() : return

    has_key=False
    for k in HIJetFlags.HIJetOutputList() : 
        if tname==k.split('#')[1] : 
            has_key=True
            break
    if not has_key :
        aux_suffix='Aux.'
        if 'CaloCluster' in objType : 
            HIJetFlags.HIJetOutputList += [ objType.replace("Container","CellLinkContainer") + "#" + tname + "_links" ]
            if not HIJetFlags.WriteClusterMoments() : aux_suffix+='-'
        else  :
            for k in HIJetFlags.MomentsSkipped() :
                if 'ScaleMomentum' in k :
                    for var in ['pt','eta','phi','m'] : aux_suffix+='-%s_%s.' % (k,var)
                else : aux_suffix+='-%s.' % k
        HIJetFlags.HIJetOutputList += [ objType + "#" + tname ]
        HIJetFlags.HIJetOutputList += [ objType.replace("Container","AuxContainer") + "#" + tname + aux_suffix ]



def AppendOutputList(HIAODItemList=[]) :
    """Adds HIJetOutputList to the list passed in as an argument"""

    if HIJetFlags.WriteClusters() : AddToOutputList(HIJetFlags.HIClusterKey(),"xAOD::CaloClusterContainer")
    from RecExConfig.RecFlags import rec
    if not rec.doESD():
        for R in HIJetFlags.AntiKtRValues() : 
            AddToOutputList("AntiKt%dHIJets" % int(10*R))
            if jetFlags.useTruth(): AddToOutputList("AntiKt%dTruthJets" % int(10*R))
        AddToOutputList(HIJetFlags.TrackJetContainerName())
    HIAODItemList+=HIJetFlags.HIJetOutputList()


def HIClusterGetter(tower_key="CombinedTower", cell_key="AllCalo", cluster_key="") :
    """Function to equip HI cluster builder from towers and cells, adds to output AOD stream"""

    if cluster_key == "" : cluster_key=HIJetFlags.HIClusterKey()

    from HIJetRec.HIJetRecConf import HIClusterMaker
    theAlg=HIClusterMaker()
    theAlg.InputTowerKey=tower_key
    theAlg.CaloCellContainerKey=cell_key
    theAlg.OutputContainerKey=cluster_key

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += theAlg

    if HIJetFlags.WriteClusters() : AddToOutputList(HIJetFlags.HIClusterKey(),"xAOD::CaloClusterContainer")
    return theAlg


def AddHIJetFinder(R=0.4) :
    unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()
    cname="AntiKt%dHIJets_%s" % (int(10*R),unsubtr_suffix)
    #'HI' is not allowed 'Label'
    #Name parsing for JetPtAssociationTool in JetToolSupport.buildModifiers will break
    #when building PtAssociations, build and add them manually
    myMods=jtm.modifiersMap["HI_Unsubtr"]
    #myMods += AddPtAssociationTools(R)
    finder=jtm.addJetFinder(cname, "AntiKt", R, "HI",myMods,
                            consumers=None, ivtxin=None,
                            ghostArea=0.0, ptmin = 0., ptminFilter= 5000)
    jtm.HIJetRecs+=[finder]


def AddPtAssociationTools(R, doTracks=True) :
    tlist=[]
    if doTracks and jetFlags.useTracks(): 
        cname=HIJetFlags.TrackJetContainerName()
        tname='hitrackassoc_04'
        if not tname in jtm.tools:
            from JetMomentTools.JetMomentToolsConf import JetPtAssociationTool
            jtm.add(JetPtAssociationTool(tname, InputContainer=cname, AssociationName="GhostTrack"))
        tlist += [ jtm.tools[tname] ]
    if jetFlags.useTruth(): 
        cname='AntiKt%dTruthJets' % int(10*R)
        tname='truthassoc_0%d' % int(10*R)
        if not tname in jtm.tools:
            from JetMomentTools.JetMomentToolsConf import JetPtAssociationTool
            jtm.add(JetPtAssociationTool(tname, InputContainer=cname, AssociationName="GhostTruth"))
        tlist += [ jtm.tools[tname] ]
    return tlist


def MakeModulatorTool(mod_key, **kwargs) :
    harmonics=[]
    if 'harmonics' in kwargs.keys() : harmonics=kwargs['harmonics']
    else : harmonics=HIJetFlags.HarmonicsForSubtraction()

    tname="Modulator_%s" % BuildHarmonicName(mod_key,harmonics=harmonics)
    if 'suffix' in kwargs.keys() : tname+='_%s' % kwargs['suffix']

    if(len(harmonics)==0) : return GetNullModulator()
    if hasattr(jtm,tname) : return getattr(jtm,tname)

    from HIJetRec.HIJetRecConf import HIUEModulatorTool
    mod=HIUEModulatorTool(tname)
    mod.EventShapeKey=mod_key
    for n in [2,3,4] :
        val=(n in harmonics)
        attr_name='DoV%d' % n
        setattr(mod,attr_name,val)

    jtm.add(mod)
    return mod

def MakeSubtractionTool(shapeKey, moment_name='', momentOnly=False, **kwargs) : 
    from HIJetRec.HIJetRecConf import HIJetConstituentSubtractionTool
    suffix=shapeKey
    if momentOnly : suffix+='_'+moment_name;

    if 'modulator' in kwargs.keys() : mod_tool=kwargs['modulator']
    else : mod_tool=GetNullModulator()

    subtr=HIJetConstituentSubtractionTool("HICS_"+suffix)
    subtr.EventShapeKey=shapeKey
    subtr.Modulator=mod_tool
    subtr.MomentName='JetSubtractedScale%sMomentum' % moment_name
    subtr.SetMomentOnly=momentOnly
    subtr.ApplyOriginCorrection=HIJetFlags.ApplyOriginCorrection()
    subtr.Subtractor=GetSubtractorTool(**kwargs)
    jtm.add(subtr)
    return subtr

def ApplySubtractionToClusters(**kwargs) :
    if 'event_shape_key' in kwargs.keys() : event_shape_key=kwargs['event_shape_key']
    else :
        from HIGlobal.HIGlobalFlags import jobproperties
        event_shape_key=jobproperties.HIGlobalFlags.EventShapeKey()

    if 'cluster_key' in kwargs.keys() : cluster_key=kwargs['cluster_key']
    else : cluster_key=HIJetFlags.HIClusterKey()
        

    if 'modulator' in kwargs.keys() : mod_tool=kwargs['modulator']
    else : mod_tool=GetNullModulator()
    
    if 'update_only' in kwargs.keys() : update_only = kwargs['update_only']
    else : update_only = False
    
    from HIJetRec.HIJetRecConf import HIClusterSubtraction
    toolName='HIClusterSubtraction'
    if 'name' in kwargs.keys() : toolName = kwargs['name']
    theAlg=HIClusterSubtraction(toolName)
    theAlg.ClusterKey=cluster_key
    theAlg.EventShapeKey=event_shape_key
    theAlg.Subtractor=GetSubtractorTool(**kwargs)
    theAlg.Modulator=mod_tool
    theAlg.UpdateOnly=update_only
    
    do_cluster_moments=False
    if 'CalculateMoments' in kwargs.keys() : do_cluster_moments=kwargs['CalculateMoments']
    if do_cluster_moments :
        from CaloRec.CaloRecConf import CaloClusterMomentsMaker
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool = CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theCaloNoiseTool

        HIClusterMoments = CaloClusterMomentsMaker ("HIClusterMoments")
        #HIClusterMoments.MaxAxisAngle = 20*deg
        HIClusterMoments.CaloNoiseTool = theCaloNoiseTool
        HIClusterMoments.UsePileUpNoise = False
        HIClusterMoments.MinBadLArQuality = 4000
        HIClusterMoments.MomentsNames = ["CENTER_MAG",
                                         "LONGITUDINAL",
                                         "FIRST_ENG_DENS", 
                                         "SECOND_ENG_DENS", 
                                         "ENG_FRAC_EM", 
                                         "ENG_FRAC_MAX", 
                                         "ENG_FRAC_CORE", 
                                         "ENG_BAD_CELLS",
                                         "N_BAD_CELLS",
                                         "N_BAD_CELLS_CORR",
                                         "BAD_CELLS_CORR_E",
                                         "BADLARQ_FRAC",
                                         "ENG_POS",
                                         "SIGNIFICANCE",
                                         "CELL_SIGNIFICANCE",
                                         "CELL_SIG_SAMPLING",
                                         "AVG_LAR_Q",
                                         "AVG_TILE_Q"]

        from IOVDbSvc.CondDB import conddb
        if not conddb.isOnline:
            from LArRecUtils.LArHVScaleRetrieverDefault import LArHVScaleRetrieverDefault
            HIClusterMoments.LArHVScaleRetriever=LArHVScaleRetrieverDefault()
            HIClusterMoments.MomentsNames += ["ENG_BAD_HV_CELLS","N_BAD_HV_CELLS"]
        
        theAlg.ClusterCorrectionTools=[HIClusterMoments]

    jtm.add(theAlg)
    jtm.jetrecs += [theAlg]
    jtm.HIJetRecs+=[theAlg]


def AddIteration(seed_container,shape_name, **kwargs) :

    out_shape_name=shape_name
    if 'suffix' in kwargs.keys() : out_shape_name+='_%s' % kwargs['suffix']
    mod_shape_key=out_shape_name+'_Modulate'
    remodulate=True
    if 'remodulate' in kwargs.keys() :
        if not kwargs['remodulate'] : 
            mod_tool=GetNullModulator()
            remodulate=False

    if remodulate :
        if 'modulator' in kwargs.keys() : mod_tool=kwargs['modulator']
        else : 
            mod_shape_name=BuildHarmonicName(out_shape_name,**kwargs)
            mod_tool=MakeModulatorTool(mod_shape_key,**kwargs)

 
    assoc_name=jtm.HIJetDRAssociation.AssociationName
    from HIJetRec.HIJetRecConf import HIEventShapeJetIteration
    iter_tool=HIEventShapeJetIteration('HIJetIteration_%s' % out_shape_name )
    
    iter_tool.InputEventShapeKey=shape_name
    iter_tool.OutputEventShapeKey=out_shape_name
    iter_tool.AssociationKey=assoc_name
    iter_tool.CaloJetSeedContainerKey=seed_container
    iter_tool.Subtractor=GetSubtractorTool(**kwargs)
    iter_tool.ModulationScheme=HIJetFlags.ModulationScheme()
    iter_tool.RemodulateUE=HIJetFlags.Remodulate()
    iter_tool.Modulator=mod_tool
    iter_tool.ShallowCopy=False
    iter_tool.ModulationEventShapeKey=mod_shape_key
    if 'track_jet_seeds' in kwargs.keys() : 
        iter_tool.TrackJetSeedContainerKey=kwargs['track_jet_seeds']
    jtm.add(iter_tool)
    jtm.jetrecs += [iter_tool]
    jtm.HIJetRecs+=[iter_tool]
    return iter_tool

def JetAlgFromTools(rtools, suffix="HI",persistify=True) :
    #insert exe tools at front of list, e.g. tracksel and tvassoc for HI etc.
    HIJet_exe_tools=[]
    from JetRec.JetRecFlags import jetFlags
    if jetFlags.useTruth() and not jetFlags.Enabled() : HIJet_exe_tools += HITruthParticleCopy()
    #if jetFlags.useCells():  HIJet_exe_tools += [jtm.missingcells]
    if HIJetFlags.UseHITracks() : HIJet_exe_tools += [jtm.tracksel_HI,jtm.gtracksel_HI,jtm.tvassoc_HI]
    rtools=HIJet_exe_tools+rtools
    from JetRec.JetRecConf import JetToolRunner
    runner=JetToolRunner("jetrun"+suffix, Tools=rtools, Timer=jetFlags.timeJetToolRunner())
    jtm.add(runner)
    
    from JetRec.JetRecConf import JetAlgorithm
    theAlg=JetAlgorithm("jetalg"+suffix)
    theAlg.Tools = [runner]
    from AthenaCommon.AlgSequence import AlgSequence
    topsequence = AlgSequence()
    topsequence += theAlg
    
    from GaudiKernel.Constants import DEBUG
    if jetFlags.debug > 0:
        
        jtm.setOutputLevel(runner, DEBUG)
        theAlg.OutputLevel = DEBUG
        
    if jetFlags.debug > 3:
        jtm.setOutputLevel(jtm.jetBuilderWithoutArea, DEBUG)
            
    if persistify :
        for t in rtools: 
            if hasattr(t,"OutputContainer") :
                AddToOutputList(t.OutputContainer)
    return theAlg

def HITruthParticleCopy() :
    from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
    rtools = scheduleCopyTruthParticles()
    #following fixes oversight in schduleCopyTruthParticles
    for ptype in jetFlags.truthFlavorTags():
        toolname = "CopyTruthTag" + ptype
        if toolname in jtm.tools and toolname not in rtools: rtools += [jtm.tools[toolname]]
    rtools += [ jtm.truthpartcopy ]#, jtm.truthpartcopywz ]
    return rtools


def BuildHarmonicName(shape_key, **kwargs) :
    tname=shape_key
    if 'harmonics' in kwargs.keys() : 
        for n in kwargs['harmonics'] : tname += '_V%d' % n
    return tname

def GetNullModulator() :
    tname='NullUEModulator'
    if hasattr(jtm,tname) : return getattr(jtm,tname)
    from HIJetRec.HIJetRecConf import HIUEModulatorTool
    mod=HIUEModulatorTool(tname)
    mod.EventShapeKey='NULL'
    for n in [2,3,4] : setattr(mod,'DoV%d' % n,False)
    jtm.add(mod)
    return mod
        


def GetFlowMomentTools(key,mod_key) :
    mtools=[]

    #only compute no flow moment if subtraction actually used flow
    if len(HIJetFlags.HarmonicsForSubtraction()) > 0 :
        null_mod_tool=GetNullModulator()
        mtools+=[MakeSubtractionTool(key,moment_name='NoVn',momentOnly=True,modulator=null_mod_tool)]
    
    if not HIJetFlags.ExtraFlowMoments() : return mtools

    #only add these tools if requested by package flag
    for n in [2]:

        #if flow subtraction only used one harmonic
        #then extra moment for that harmonic is redundant, skip it
        if len(HIJetFlags.HarmonicsForSubtraction()) == 1 :
            if n == HIJetFlags.HarmonicsForSubtraction()[0] : continue

        mod_tool=MakeModulatorTool(mod_key,harmonics=[n])
        subtr_tool=MakeSubtractionTool(key,moment_name='V%dOnly' % n,momentOnly=True,modulator=mod_tool)
        mtools+=[subtr_tool]

    return mtools


def GetSubtractorTool(**kwargs) :
    useClusters=False 
    if 'useClusters' in kwargs.keys() : useClusters=kwargs['useClusters'] 
    elif HIJetFlags.DoCellBasedSubtraction() : useClusters=False
    else : useClusters=True

    if useClusters : 
        if not hasattr(jtm,"HIJetClusterSubtractor") : 
            from HIJetRec.HIJetRecConf import HIJetClusterSubtractorTool
            jtm.add(HIJetClusterSubtractorTool("HIJetClusterSubtractor"))
        return jtm.HIJetClusterSubtractor
    else:
        if not hasattr(jtm,"HIJetCellSubtractor") : 
            from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
            jtm.add(HIJetCellSubtractorTool("HIJetCellSubtractor"))
        return jtm.HIJetCellSubtractor

    
def GetHIModifierList(coll_name='AntiKt4HIJets',prepend_tools=[],append_tools=[]) :
    if coll_name not in jtm.HICalibMap.keys() :
        print 'Calibration for R=%d not available using default R=0.4 calibration'
        coll_name='AntiKt4HIJets'
    mod_list=prepend_tools
    mod_list+=[jtm.HICalibMap[coll_name]]
    mod_list+=jtm.modifiersMap['HI']
    mod_list+=append_tools
    return mod_list
def HasCollection(collection='') :
	from PyUtils import AthFile
	from AthenaCommon.AppMgr import ServiceMgr as svcMgr
	af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
	containers=af.fileinfos['eventdata_items']
	for c in containers:
		if c[1] is not None and collection in c[1] : return True
	return False    
