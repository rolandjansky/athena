# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def configTrackMET(derivationFrameworkJob, ptCutList):
    
    from METReconstruction.METRecoFlags import metFlags
    from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig,clusterSigStates
    from RecExConfig.RecFlags import rec
    doInDet = rec.doInDet()

    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import ToolSvc
    from METReconstruction.METRecoConfig import getMETRecoAlg

    cfg_tab = [METConfig('Track'+str(ptCut),[BuildConfig('SoftTrk','Track')],
                        [RefConfig('TrackFilter','PVTrack')],
                        doTracks=doInDet) for ptCut in ptCutList]

    for i in xrange(len(ptCutList)):
        cfg_tab[i].refiners['TrackFilter'].DoLepRecovery=True
        cfg_tab[i].refiners['TrackFilter'].DoVxSep=True
        cfg_tab[i].refiners['TrackFilter'].DoEoverPSel=True

        metFlags.METConfigs()[cfg_tab[i].suffix] = cfg_tab[i]
        metFlags.METOutputList().append(cfg_tab[i].suffix)

        indet = CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_MET"+str(ptCutList[i]),
                                                                     CutLevel="HITight",
                                                                     minPt=ptCutList[i])
        ToolSvc += indet

        cfg_tab[i].refiners['TrackFilter'].TrackSelectorTool  = indet
    
    derivationFrameworkJob += getMETRecoAlg('METReconstruction')



def addMETOutputs(slimhelper,ptCutList):
	for ptCut in ptCutList:
		slimhelper.StaticContent.append("xAOD::MissingETContainer#MET_Track"+str(ptCut))
		slimhelper.StaticContent.append("xAOD::MissingETAuxContainer#MET_Track"+str(ptCut)+"Aux.")
