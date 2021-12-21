#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@brief configuration for the trigger efficiency monitoring
'''
from TrigConfigSvc.TriggerConfigAccess import getHLTMenuAccess
from AthenaConfiguration.ComponentFactory import CompFactory


def _TrigEff(flags, triggerAndRef, algname='HLTMinBiasEffMonitoringAlg'):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(flags, algname)

    alg = monConfig.addAlgorithm(
        CompFactory.HLTEfficiencyMonitoringAlg, algname)

    trkSel = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetTrackSelectionTool_LoosePrimary", CutLevel="LoosePrimary"
    )
    alg.TrackSelectionTool = trkSel

    alg.triggerList = [ el["chain"] for el in  triggerAndRef ]
    alg.refTriggerList = [ el["refchain"] for el in  triggerAndRef ]

    length = len(alg.triggerList)

    mainGroup = monConfig.addGroup(
        alg, 'TrigAll', topPath='HLT/MinBiasMon/Counts/')

    alreadyConfigured = set()
    for cdef in triggerAndRef:
        chain = cdef['chain']
        refchain = cdef['refchain']
        xmin  = cdef['xmin']
        xmax  = cdef['xmax']
        xbins = xmax-xmin
        effGroup = monConfig.addGroup(
            alg, chain+refchain, topPath='HLT/MinBiasMon/EffAll/')

        whichcounter='nTrkOffline'
        # if the chain cuts on higher pt (there is a few predefined chains) use different counter
        if '_pt' in chain:
            whichcounter += '_'+chain.split('_')[3]
            effGroup.defineHistogram(f'EffPassed,leadingTrackPt;{chain}_ref_{refchain}_pt', type='TEfficiency',
                                      title=chain+';Leading track pt;Efficiency', xbins=100, xmin=0.0, xmax=10)
        effGroup.defineHistogram(f'EffPassed,{whichcounter};{chain}_ref_{refchain}', type='TEfficiency',
                                    title=chain+';Offline Good nTrk;Efficiency', xbins=xbins, xmin=xmin, xmax=xmax)


        if chain not in alreadyConfigured:
            alreadyConfigured.add(chain)
            # need this protection because we can measure efficiency with several reference trigger, but want counts irrespective of ref. triggers
            mainGroup.defineHistogram('nTrkOffline_counts_' + chain, type='TH1F',
                                      title=chain+';Offline Good nTrk;Events', xbins=300, xmin=0, xmax=300)

    mainGroup.defineHistogram('TrigCounts', title='Trigger counts;;Event rate',
                              xbins=length, xmin=0, xmax=len(alreadyConfigured), xlabels=list(alreadyConfigured))

    return monConfig.result()


def TrigMinBiasEff(flags):

    # configure the monitoring dynamically according to the chains present in the menu
    #mbChains = [ c for c in GetFileMD(flags.Input.Files)['TriggerMenu']['HLTChains'] if '_mb_' in c]
    mbChains = [c for c in getHLTMenuAccess(flags) if '_mb_' in c]
    if len(mbChains) == 0:
        return _TrigEff(flags, [])


    # here we generate config with detailed settings
    def _c(chain, refchain, **kwargs):
        conf = {"chain":chain, "refchain": refchain, "xmin":0, "xmax":20 }
        conf.update(kwargs)
        return conf

    # check all mb_sptrk chains w.r.t. random noalg
    triggerAndRef = [ _c(chain, "HLT_noalg_L1RD0_FILLED")  for chain in mbChains
                    if ("HLT_mb_sptrk_" in chain or "HLT_mb_sp_" in chain or "HLT_mb_mbts_" in chain)]
    triggerAndRef += [ _c("HLT_mb_sptrk_L1RD0_FILLED", "HLT_mb_sp_L1RD0_FILLED") ]

    hmt = [c for c in mbChains if ('_hmt_' in c and '_pusup' not in c)]
    if len(hmt) != 0:
        # sort by trk threshold
        def _trk(chain):
            part = chain.split("_")
            for el in part:
                if el.startswith("trk"):
                    return int(el.strip("trk"))
            raise RuntimeError(f"Chain {chain} is not the hmt chain")

        hmt.sort(key=lambda c: int(_trk(c)))
        # monitor first hmt w.r.t sptrk
        triggerAndRef += [ _c(hmt[0], "HLT_mb_sptrk_L1RD0_FILLED", xmax=_trk(hmt[0])+30)]

        # group set the ref for each trigger to be one of lower threshold
        triggerAndRef += [  _c(chain, ref, xmin=_trk(chain)-20, xmax=_trk(chain)+50) for chain,ref in zip(hmt[1:], hmt) ]

        # pu suppressing trigger should be monitored using trigger of the same threshold w/o pu suppression
        pusup = [c for c in mbChains if '_hmt_' in c and '_pusup' in c]
        def _dropsup(chain):
            s = chain.split("_")
            return "_".join(s[:3]+s[4:])
        triggerAndRef += [  _c(chain, _dropsup(chain),  xmin=_trk(chain)-20, xmax=_trk(chain)+50) for chain in pusup ]
    # add here all the special cases
    return _TrigEff(flags, triggerAndRef)



if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    #ConfigFlags.Input.Files = ['myAOD.pool.root']
#     ConfigFlags.Input.Files = [
#         'AOD.25577237._000120.pool.root.1'
# #        '/afs/cern.ch/user/k/kburka/workspace/mbts/AOD.25577237._000120.pool.root.1'
#     ]
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigMinBiasEff(ConfigFlags))

# for testing is it sometimes useful to enable also this monitoring
#    from TrigMinBiasMonitoring.TrigSPTRKMonitoringMT import TrigSPTRK
#    cfg.merge(TrigSPTRK(ConfigFlags))

    cfg.getEventAlgo('HLTMinBiasEffMonitoringAlg').OutputLevel = DEBUG  # DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run()
    # to run:
    # python -m TrigMinBiasMonitoring.TrigEffMonitoring --filesInput=filepath --evtMax=XYZ
