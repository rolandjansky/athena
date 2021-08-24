# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# METCommon.py
# Schedules default DF MET content building tools and writes the
# results into SG. These may then be accessed along the train  
#********************************************************************
from AthenaCommon import CfgMgr
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob

##########################################################################################
# MET
##########################################################################################

METLists = {}

#simplelist = ['EMTopo','LocHadTopo','Truth','Track','EMTopoRegions','LocHadTopoRegions','TruthRegions',
#          'Core_AntiKt4EMPFlow', 'Core_AntiKt4EMTopo', 'Core_AntiKt4LCTopo',
#          'Reference_AntiKt4EMPFlow', 'Reference_AntiKt4EMTopo', 'Reference_AntiKt4LCTopo',]
xaodlist = ['Calo', 'EMTopo', 'EMTopoRegions', 'LocHadTopo', 'LocHadTopoRegions',
        'Track', 'Truth', 'TruthRegions']
xaodmaps = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow']
maplist = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow']
truthmaplist = ['Truth_AntiKt4LCTopo','Truth_AntiKt4EMTopo','Truth_AntiKt4EMPFlow']
METLists['Diagnostic'] = ['Calo','EMTopo','EMTopoRegions','LocHadTopo','LocHadTopoRegions','TruthRegions']
METLists['Assocs'] = [m for m in maplist]
METLists['TruthAssocs'] = truthmaplist

def addMETOutputs(slimhelper, contentlist=[], slimlist=[]):
    suffixlist = ['Truth','AntiKt4EMTopo']
    for content in contentlist:
        if content in METLists.keys():
            suffixlist += METLists[content]
        else:
            suffixlist.append(content)
    for suffix in sorted(set(suffixlist)):
        if suffix in maplist:
            print ("DFMissingET -- Add containers for METAssoc_"+suffix+" to output")
            if suffix in slimlist:
                slimhelper.SmartCollections.append("MET_Reference_"+suffix)
            elif suffix in xaodmaps:
                slimhelper.AllVariables.append("METAssoc_"+suffix)
                slimhelper.AllVariables.append("MET_Core_"+suffix)
                slimhelper.AllVariables.append("MET_Reference_"+suffix)
            else:
                slimhelper.StaticContent.append("xAOD::MissingETAssociationMap#METAssoc_"+suffix)
                slimhelper.StaticContent.append("xAOD::MissingETAuxAssociationMap#METAssoc_"+suffix+"Aux.")
                slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_Core_"+suffix)
                slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_Core_"+suffix+"Aux.")
        elif suffix in truthmaplist:
            print ("DFMissingET -- Add containers for METAssoc_"+suffix+" to output")
            slimhelper.StaticContent.append("xAOD::MissingETAssociationMap#METAssoc_"+suffix)
            slimhelper.StaticContent.append("xAOD::MissingETAuxAssociationMap#METAssoc_"+suffix+"Aux.")
            slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_Core_"+suffix)
            slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_Core_"+suffix+"Aux.")
        elif suffix in xaodlist:
            print ("DFMissingET -- Add containers for MET_"+suffix+" to output")
            if suffix in slimlist:
                slimhelper.SmartCollections.append("MET_"+suffix)
            else:
                slimhelper.AllVariables.append("MET_"+suffix)
        else:
            print ("DFMissingET -- Add containers for MET_"+suffix+" to output")
            slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_"+suffix)
            slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_"+suffix+"Aux.")

customMETConfigs = {}
metalgs = {}

# Set up custom MET reconstruction algorithms
# Build the list here
def addMETTruthMap(jetcoll='AntiKt4EMTopo',configlist="CustomMET"):
    assocname = 'Truth_'+jetcoll
    customMETConfigs.setdefault(configlist,{})
    if assocname not in customMETConfigs[configlist]:
        from METReconstruction.METAssocConfig import METAssocConfig,AssocConfig
        cfg_truthassoc = METAssocConfig(assocname,
                        [AssocConfig('Truth',jetcoll+'Jets')],
                        doPFlow=False,doTruth=True)
        customMETConfigs[configlist][cfg_truthassoc.suffix] = cfg_truthassoc

# Schedule in a single alg
def scheduleMETAssocAlg(sequence=DerivationFrameworkJob,configlist="CustomMET"):
    algname = 'METAssociation_'+configlist
    assocAlg = None
    if algname in metalgs.keys():
        print ("Get preexisting alg:", algname, metalgs[algname])
        assocAlg = metalgs[algname]
    else:
        from METReconstruction.METAssocConfig import getMETAssocAlg
        assocAlg = getMETAssocAlg(algname,customMETConfigs[configlist])
        metalgs[algname] = assocAlg
        print ("Generate MET alg:", algname, assocAlg)
    if not hasattr(sequence,algname):
        sequence += assocAlg

def scheduleStandardMETContent(sequence=DerivationFrameworkJob, algname = 'METStandardAssociationAlg'):
    assocAlg = None
    if algname in metalgs.keys():
        print ("Get preexisting alg:", algname, metalgs[algname])
        assocAlg = metalgs[algname]
    else:
        # This import statement executes code which auto-configures the standard associations.
        # Design to be improved with the migration to the new config.
        import METReconstruction.METConfig_Associator # noqa: F401

        from METReconstruction.METRecoFlags import metFlags
        standardConfigs = {k : v for k, v in metFlags.METAssocConfigs().items() if ("EMTopo" in k or "EMPFlow" in k)}
        from METReconstruction.METAssocConfig import getMETAssocAlg
        assocAlg = getMETAssocAlg(algname,standardConfigs)
        metalgs[algname] = assocAlg
        print ("Generate MET alg:", algname, assocAlg)
    if not hasattr(sequence,algname):
        sequence += assocAlg

def scheduleCustomVtxMETContent(vxColl, jetColl, constituentColl="", sequence=DerivationFrameworkJob, algname = 'METCustomVtxAssociationAlg'):
    assocAlg = None
    if algname in metalgs.keys():
        print ("Get preexisting alg:", algname, metalgs[algname])
        assocAlg = metalgs[algname]
    else:

        from METReconstruction.METAssocConfig import METAssocConfig, AssocConfig, getMETAssocAlg
        jettype = {'AntiKt4EMTopo':'EMJet',
            'AntiKt4LCTopo':'LCJet',
            'AntiKt4EMPFlow':'PFlowJet',
            'AntiKt4PFlowCustomVtx':'PFlowJet'}
        associators = [AssocConfig(jettype[jetColl]),
                AssocConfig('Muon'),
                AssocConfig('Ele'),
                AssocConfig('Gamma'),
                AssocConfig('Tau'),
                AssocConfig('Soft')]
        cfg = METAssocConfig(suffix = jetColl+vxColl,
                             buildconfigs = associators,
                             doPFlow = ('PFlow' in jetColl),
                             modConstKey = constituentColl)

        for assoc in cfg.assoclist:
            assoc.PrimVxColl = vxColl+'PrimaryVertices'
        cfgDict = {cfg.suffix : cfg}
        assocAlg = getMETAssocAlg(algname,cfgDict)
        metalgs[algname] = assocAlg
        print ("Generate MET alg:", algname, assocAlg)

    if not hasattr(sequence,algname):
        sequence += assocAlg

def scheduleMETCustomTrackPtCut(ptcut,jetcoll='AntiKt4EMTopo',
                                configlist="CustomMET",
                                outputlist="CustomMET"):
    metname = ('' if ptcut==500 else 'TrkPt{0}'.format(ptcut))
    trkseltool = CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc"+metname,
                               CutLevel="TightPrimary",
                               maxZ0SinTheta=3,
                               maxD0overSigmaD0=2,
                               minPt=ptcut)

    scheduleMETCustomTrkSel(trkseltool,
                            metname,
                            jetcoll,
                            outputlist,
                            configlist
                            )

def scheduleMETCustomTrkSel(trkseltool,
                            metname,
                            jetcoll='AntiKt4EMTopo',
                            configlist="CustomMET",
                            outputlist="CustomMET",
                            ):
    from METReconstruction.METAssocConfig import METAssocConfig,AssocConfig
    jettype = {'AntiKt4EMTopo':'EMJet',
               'AntiKt4LCTopo':'LCJet',
               'AntiKt4EMPFlow':'PFlowJet'}
    associators = [AssocConfig(jettype[jetcoll]),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]

    cfg = METAssocConfig(suffix=jetcoll+metname,
                 buildconfigs=associators,
                 doPFlow=(jetcoll=='AntiKt4EMPFlow'), # doPFlow
                 trksel=trkseltool)

    customMETConfigs.setdefault(configlist,{})[cfg.suffix] = cfg

    maplist.append(cfg.suffix)
    METLists.setdefault(outputlist,[]).append(cfg.suffix)

# def overwriteMETPFlowWithFix(sequence):

#     from METReconstruction.METRecoFlags import metFlags
#     from METReconstruction.METAssocConfig import AssocConfig,METAssocConfig
#     associators = [AssocConfig('PFlowJet'),
#                    AssocConfig('Muon'),
#                    AssocConfig('Ele'),
#                    AssocConfig('Gamma'),
#                    AssocConfig('Tau'),
#                    AssocConfig('Soft')]
#     cfg = METAssocConfig('AntiKt4EMPFlow',
#                          associators,
#                          doPFlow=True)
#     from METReconstruction.METAssocConfig import getMETAssocAlg
#     metAlg = getMETAssocAlg('METPFlowFix',allowOverwrite=True,configs={"AntiKt4EMPFlow":cfg})
#     sequence += metAlg
