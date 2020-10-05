# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV

from HIJetRec.HIJetRecFlags import HIJetFlags
from TrigHLTJetRec.TrigHLTJetRecConf import TrigHLTJetRecFromCluster

#def _getTriggerHIPseudoJetGetter(hicluster_name):
#    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
#
#    # declare jtm as global as this function body may modify it
#    # with the += operator
#    global jtm
#
#    label = hicluster_name
#    pjg_name = 'triggerPseudoJetGetter_%s' % label
#
#    try:
#        triggerPseudoJetGetter = getattr(jtm, pjg_name)
#    except AttributeError:
#        # Add TriggerPSeudoJetGetter to the JetTool Manager,
#        # which pushes it to the ToolSvc in __iadd__
#        # This is done in the same as PseudoJetGetter is added in
#        # JetRecStandardTools.py.
#        # The 'Label' must be one of the values found in JetContainerInfo.h
#        pjg = TriggerPseudoJetGetter(pjg_name, Label=label)
#
#        # adds arg to Tool Service and returns arg
#        jtm.add(pjg)
#        triggerPseudoJetGetter = getattr(jtm, pjg_name)
#
#    return triggerPseudoJetGetter


def _getHIJetBuildTool(merge_param,
                     ptmin=20000.,
                     ptminFilter=20000.,
                     jet_calib='jes',
                     cluster_calib='EM',
                     hicluster_name='HICluster',
                     name=''):

    global jtm

    msg = 'Naming convention breaks with merge param %d' % merge_param
    int_merge_param = int(10 * merge_param)
    assert 10 * merge_param == int_merge_param, msg
    assert merge_param > 0.

    if not name:
        name = 'TrigAntiKt%dHIJets' % int_merge_param

    EventShapeKey = "HLT_xAOD__HIEventShapeContainer_TrigHIEventShape"
    ClusterKey = hicluster_name

    # Plug in directly tools from HIJetTools.py e.g. to avoid PseudoJetGetter which is different at HLT and for other reasons
    # -------------------------------------------------------------------------------------------------------------------------

    #jet filters
    from JetRec.JetRecConf import JetFilterTool
    jetfil5=JetFilterTool("HLT_a"+str(int_merge_param)+"jetfil5", PtMin = 5000)
    jtm.add(jetfil5)
    jetfil8=JetFilterTool("HLT_a"+str(int_merge_param)+"jetfil8", PtMin = 8000)
    jtm.add(jetfil8)
    jetfil20=JetFilterTool("HLT_a"+str(int_merge_param)+"jetfil20", PtMin = 20000)
    jtm.add(jetfil20)

    #DR association- form element links for all clusters w/in DR of each jet
#    DR=HIJetFlags.ClusterDRAssociation()
#    from HIJetRec.HIJetRecConf import HIJetDRAssociationTool
#    assoc=HIJetDRAssociationTool("HLT_a"+str(int_merge_param)+"HIJetDRAssociation")
#    assoc.ContainerKey=ClusterKey
#    assoc.DeltaR=DR
#    #assoc.AssociationName="%s_DR%dAssoc" % (ClusterKey,int(10*DR))
#    assoc.AssociationName="HLT_a"+str(int_merge_param)+"HIClDR%dAssoc" % (int(10*DR))
#    #assoc.AssociationName="GhostTrack"
#    jtm.add(assoc)
#
#    assoc_name=assoc.AssociationName

    #calculate discriminants as moments
    from HIJetRec.HIJetRecConf import HIJetMaxOverMeanTool
    max_over_mean=HIJetMaxOverMeanTool("HLT_a"+str(int_merge_param)+"HIJetMaxOverMean")
    jtm.add(max_over_mean)

    #discriminants for jet filtering
    from HIJetRec.HIJetRecConf import HIJetDiscriminatorTool
    discrim=HIJetDiscriminatorTool("HLT_a"+str(int_merge_param)+"HIJetDiscriminator")
    discrim.MaxOverMeanCut=HIJetFlags.DCutMaxOverMean()
    discrim.MinimumETMaxCut=HIJetFlags.DCutMax()
    jtm.add(discrim)
    #jtm.trigjetrecs += [discr]

    #null modulator
    #tname="HLT_a"+str(int_merge_param)+"NullUEModulator"
    #if hasattr(jtm,tname) : return getattr(jtm,tname)
    from HIJetRec.HIJetRecConf import HIUEModulatorTool
    mod_tool=HIUEModulatorTool("HLT_a"+str(int_merge_param)+"NullUEModulator")
    mod_tool.EventShapeKey='NULL'
    for n in [2,3,4] : setattr(mod_tool,'DoV%d' % n,False)
    jtm.add(mod_tool)

    # subtraction
    from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
    cell_subtr=HIJetCellSubtractorTool("HLT_a"+str(int_merge_param)+"HIJetSubtractor")
    jtm.add(cell_subtr)

    # calibration
    from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
    #calib_tool=JetCalibrationTool("HLT_a"+str(int_merge_param)+"HICalibTool",JetCollection="AntiKt4TopoEM",ConfigFile="JES_Full2012dataset_Preliminary_Jan13.config",CalibSequence="AbsoluteEtaJES")
    #calib_tool=JetCalibrationTool("HLT_a"+str(int_merge_param)+"HICalibTool",JetCollection="AntiKt4EMTopo",ConfigFile="JES_Full2012dataset_May2014.config",CalibSequence="JetArea_Residual_Origin_EtaJES_GSC",IsData=False)
    #calib_tool=JetCalibrationTool("HLT_a"+str(int_merge_param)+"HICalibTool",JetCollection="AntiKt4EMTopo",ConfigFile="JES_MC15Prerecommendation_April2015.config",CalibSequence="EtaJES", IsData=False)
    calib_tool=JetCalibrationTool("HLT_a"+str(int_merge_param)+"HICalibTool",JetCollection="AntiKt4EMTopo",ConfigFile="JES_Full2012dataset_Preliminary_Jan13.config",CalibSequence="AbsoluteEtaJES", IsData=False)


    jtm.add(calib_tool)

    from  TrigHLTJetRec.TrigHLTJetRecConfig import _getTriggerPseudoJetGetter

    # -------------------------------------------------------------------------------------------------

    a2_unsubtracted_name = "TrigAntiKt2HIJets_Unsubtracted_a"+str(int_merge_param)
    seed_finder = jtm.addJetFinder(a2_unsubtracted_name,
                                  "AntiKt",
                                  0.2,
                                  gettersin=[_getTriggerPseudoJetGetter(cluster_calib)],
                                  #modifiersin=[assoc,max_over_mean,jetfil5], # jtm.modifiersMap['HI_Unsubtr'],	# may think about TrigHI_Unsubtracted with just max_over_mean
                                  modifiersin=[max_over_mean,jetfil5], # jtm.modifiersMap['HI_Unsubtr'],	# may think about TrigHI_Unsubtracted with just max_over_mean
                                  #modifiersin=[assoc,max_over_mean,jetfil5,discrim], # jtm.modifiersMap['HI_Unsubtr'],	# may think about TrigHI_Unsubtracted with just max_over_mean
                                  ghostArea=0.0,
                                  isTrigger=True,
                                  ptmin = 5000,
                                  ptminFilter= 5000)

    seeds0_name = "TrigAntiKt2HIJets_seeds0_a"+str(int_merge_param)
    seeds0=jtm.addJetCopier(seeds0_name,a2_unsubtracted_name,[discrim],isTrigger=True,shallow=False)
    #seeds0 = seed_finder



    #from HIJetRec.HIJetRecConf import HIEventShapeJetIterationHLT
    from TrigHIRec.TrigHIRecConf import TrigHIEventShapeJetIteration
    iter0_name = "a"+str(int_merge_param)+"iter0"
    iter0=TrigHIEventShapeJetIteration(iter0_name)
    iter0.InputEventShapeKey=EventShapeKey
    iter0.OutputEventShapeKey=EventShapeKey+"_"+iter0_name
    #iter0.AssociationKey=assoc_name
    iter0.InputClustersKey=ClusterKey
    #iter0.SeedContainerKeys=[seeds0.OutputContainer]
    iter0.SeedContainerKey=seeds0.OutputContainer
    #if not hasattr(jtm,"HLT_a"+str(int_merge_param)+"HIJetSubtractor") :
    #    from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
    #    cell_subtr=HIJetCellSubtractorTool("HLT_a"+str(int_merge_param)+"HIJetSubtractor")
    #    jtm.add(cell_subtr)
    #iter0.Subtractor=jtm.HIJetSubtractor
    iter0.Subtractor=cell_subtr
    from HIEventUtils.HIEventUtilsConf import HIEventShapeMapTool
    iter0.EventShapeMapTool=HIEventShapeMapTool()
    jtm.add(iter0)
    jtm.jetrecs += [iter0]

    #subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,moment_name="subtr1")
    from HIJetRec.HIJetRecConf import HIJetConstituentSubtractionTool
    subtr1=HIJetConstituentSubtractionTool("HIConstituentSubtractor_%s" % iter0.OutputEventShapeKey)
    subtr1.EventShapeKey=iter0.OutputEventShapeKey
    subtr1.SetMomentOnly=False
    subtr1.MomentName="JetSubtractedScaleMomentum" #"subtr1"
    #subtr1.Subtractor=jtm.HIJetSubtractor
    subtr1.Subtractor=cell_subtr
    subtr1.Modulator=mod_tool
    jtm.add(subtr1)

    seeds1_name = "TrigAntiKt2HIJets_seeds1_a"+str(int_merge_param)
    seeds1=jtm.addJetCopier(seeds1_name,a2_unsubtracted_name,[subtr1,calib_tool,jetfil8],isTrigger=True,shallow=False) #add calib tool

    iter1_name = "a"+str(int_merge_param)+"iter1"
    iter1=TrigHIEventShapeJetIteration(iter1_name)
    iter1.InputEventShapeKey=EventShapeKey
    iter1.OutputEventShapeKey=EventShapeKey+"_"+iter1_name
    #iter1.AssociationKey=assoc_name
    iter1.InputClustersKey=ClusterKey
    #iter1.SeedContainerKeys=[seeds1.OutputContainer]
    iter1.SeedContainerKey=seeds1.OutputContainer
    #iter1.Subtractor=jtm.HIJetSubtractor
    iter1.Subtractor=cell_subtr
    #iter1.ModulationScheme=1;
    #iter1.RemodulateUE=remodulate
    #iter1.Modulator=mod_tool
    #iter1.ModulationEventShapeKey=mod_tool.EventShapeKey
    jtm.add(iter1)
    jtm.jetrecs += [iter1]

    #subtr2=MakeSubtractionTool(iter1.OutputEventShapeKey,moment_name="subtr2")
    from HIJetRec.HIJetRecConf import HIJetConstituentSubtractionTool
    subtr2=HIJetConstituentSubtractionTool("HIConstituentSubtractor_%s" % iter1.OutputEventShapeKey)
    subtr2.EventShapeKey=iter1.OutputEventShapeKey
    subtr2.MomentName="JetSubtractedScaleMomentum"
    subtr2.SetMomentOnly=False
    #subtr2.Subtractor=jtm.HIJetSubtractor
    subtr2.Subtractor=cell_subtr
    subtr2.Modulator=mod_tool
    jtm.add(subtr2)


    finder=jtm.addJetFinder(name+"_finder",
                            "AntiKt",
                            merge_param,
                            gettersin=[_getTriggerPseudoJetGetter(cluster_calib)],
                            modifiersin=[subtr2,calib_tool],
                            ghostArea=0.0,
                            rndseed=0,
                            isTrigger=True,
                            ptmin=ptmin,
                            ptminFilter=ptminFilter)

    exe_tools=[seed_finder,seeds0,iter0,seeds1,iter1,finder]
    from JetRec.JetRecConf import JetToolRunner
    runner=JetToolRunner("jetrunHI_a"+str(int_merge_param),Tools=exe_tools)
    jtm.add(runner)

    builder=jtm.addJetCopier(name,name+"_finder",[],isTrigger=True,shallow=False)
    builder.unlock()
    builder.InputTool=runner
    builder.lock()

    jetBuildTool = builder

    ## we could also add to hi_modifiers the calibration as defined below:

    #_is_calibration_supported(int_merge_param, jet_calib, cluster_calib)

    # # tell the offline code which calibration is requested
    #calib_str = {'jes': 'calib:j:triggerNoPileup:HLTKt4',
    #             'subjes': 'calib:aj:trigger:HLTKt4',
    #             'sub': 'calib:a:trigger:HLTKt4'}.get(jet_calib, '')

    #myMods = [calib_str] if calib_str else []

    return jetBuildTool



class TrigHLTHIJetRecFromHICluster(TrigHLTJetRecFromCluster):

    def __init__(self,
                 name,
                 alg="AntiKt",
                 merge_param="04",
                 #r_values=[0.2,0.4],
                 ptmin=8.0 * GeV,
                 ptminFilter=8.0 * GeV,
                 jet_calib='jes',
                 cluster_calib='EM',	# not needed (our clusters are made of towers at EM scale)
                 output_collection_label='HIJetsWithSubtraction',
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromCombinedTower',
                 ):

       #TrigHLTJetRecConf.
       TrigHLTJetRecFromCluster.__init__(self, name=name)

       hicluster_name = "TrigHIClusterMaker_hijet_ionlcwFS"

       doSubtraction = True
       doHIClusters = True

       from  TrigHLTJetRec.TrigHLTJetRecConfig import _getTriggerPseudoJetGetter

       if (doHIClusters) :
         #self.pseudoJetGetter = _getTriggerPseudoJetGetter(hicluster_name)
         self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)
       else :
         self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)
         hicluster_name = "TrigCaloClusterMaker_topo_tcemFS"
         if (doSubtraction) :
            hicluster_name = "TrigCaloClusterMaker_topo_ionemFS"

       from  TrigHLTJetRec.TrigHLTJetRecConfig import _getPseudoJetSelectorAll		#_getIParticleSelectorAll

       #self.iParticleSelector = _getIParticleSelectorAll(
       #    'iParticleSelectorAll')

       self.iPseudoJetSelector = _getPseudoJetSelectorAll(
           'iPseudoJetSelectorAll')

       if (doSubtraction) :
          jetBuildTool = _getHIJetBuildTool(
             float(int(merge_param))/10.,
             ptmin=ptmin,
             ptminFilter=ptminFilter,
             jet_calib=jet_calib,
             hicluster_name=hicluster_name,
             name=name
             )
          #self.inputTool = inputTool
          #self.doInputs = False
          self.jetBuildTool = jetBuildTool 
       else :
          from  TrigHLTJetRec.TrigHLTJetRecConfig import _getJetBuildTool
          self.jetBuildTool = _getJetBuildTool(
             float(int(merge_param))/10.,
             ptmin=ptmin,
             ptminFilter=ptminFilter,
             jet_calib=jet_calib,
             cluster_calib=cluster_calib,
             do_minimalist_setup=True,
             name=name
             )

       self.output_collection_label = output_collection_label
       self.pseudojet_labelindex_arg = pseudojet_labelindex_arg
