# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: addMissingEtInfo.py
# by Lei Zhou
# dump MEt info into D3PD stream

from RecExConfig.RecFlags                           import rec
from AthenaCommon.AlgSequence                       import AlgSequence
topSequence = AlgSequence()

from MissingETD3PDMaker.MissingETD3PDObject          import MissingETD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject          import MissingETCompositionD3PDObject
from MissingETD3PDMaker.MissingETD3PDMakerFlags      import MissingETD3PDMakerFlags
from D3PDMakerConfig.CommonD3PDMakerFlags            import CommonD3PDMakerFlags
from MissingETD3PDMaker.MissingETGoodnessD3PDObject  import MissingETGoodnessD3PDObject

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw            


def addMissingEtInfo   ( alg = None,
                         file = 'NTUP_PHYSICS.root',
                         tuplename = 'physics',
                         seq = topSequence,
                         **kw ):
    
    print "adding MissingEt info..."

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(tuplename, file, asAlg=True)
        pass

    flag = CommonD3PDMakerFlags
    MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey = 'AntiKt4LCTopoNewJets'
    MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_"

    ######################### SMWZ MET part #############################
    ## HSG5 MET
    HSG5_customMETs = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                       'MET_CellOut', 'MET_Cryo', 'MET_SoftJets',
                       'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                       'MET_CellOut_Eflow_STVF',
                       #'MET_CellOut_Eflow_JetArea',
                       'MET_RefFinal_STVF' ]
    for custom in HSG5_customMETs:
        alg += MissingETD3PDObject (level=0, sgkey = custom+'_HSG5',prefix='MET_HSG5_'+custom[4:],
                                    exclude = ['MET_Regions','MET_Comps','MET_Phi','MET_Et'],
                                    allowMissing=True)
        pass

    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_HSG5', suffix='HSG5_', allowMissing =True,
                                            exclude=['MET_Track_Weights','MET_Cluster_Weights'],
                                            jetSGKey='AntiKt4TopoEMNewJets', jetPrefix='jet_AntiKt4TopoEMJet_MET_')

    
    SMWZ_exclude_list = ['MET_RefFinal_Comps','MET_Calib_Comps',
                         'MET_CorrTopo_Comps','MET_Cryo_Comps','MET_CryoCone_Comps',
                         'MET_LocHadTopoObj_Comps','MET_Muid_Comps','MET_Muid_Spectro_Comps','MET_Muid_Track_Comps',
                         'MET_MuonBoy_Spectro_Comps','MET_MuonBoy_Track_Comps','MET_MuonMuid_Comps',
                         'MET_Muon_Comps',    # adjusted for SUSY include list
                         'MET_Topo_Comps','MET_Comps',
                         'MET_Track_Weights','MET_Cluster_Weights']

    #alg += MissingETD3PDObject        (**_args (10, 'MissingET', kw,   # moved to SUSY part
    #                                            exclude= SMWZ_exclude_list,
    #                                            allowMissing=True))
    
    alg += MissingETCompositionD3PDObject ( level=4,
                                            sgkey = 'MET_RefComposition_STVF',
                                            suffix='STVF_',
                                            allowMissing=True,
                                            jetSGKey='AntiKt4LCTopoNewJets',
                                            jetPrefix='jet_AntiKt4LCTopo_MET_',
                                            exclude = ['MET_Muid_Weights','MET_Muons_Weights'])

    alg += MissingETCompositionD3PDObject ( level=4,
                                            sgkey = 'MET_RefComposition_STVFMuid',
                                            suffix='STVF_',
                                            allowMissing=True,
                                            jetSGKey='AntiKt4LCTopoNewJets',
                                            jetPrefix='jet_AntiKt4LCTopo_MET_',
                                            exclude = ['MET_Electron_Weights','MET_Photon_Weights',
                                                       'MET_Staco_Weights','MET_Muons_Weights',
                                                       'MET_Tau_Weights','MET_Jet_Weights',
                                                       'MET_Cluster_Weights','MET_Track_Weights'])

    alg += MissingETCompositionD3PDObject ( level=4,
                                            sgkey = 'MET_RefComposition_STVFMuons',
                                            suffix='STVF_',
                                            allowMissing=True,
                                            jetSGKey='AntiKt4LCTopoNewJets',
                                            jetPrefix='jet_AntiKt4LCTopo_MET_',
                                            exclude = ['MET_Electron_Weights','MET_Photon_Weights',
                                                       'MET_Staco_Weights','MET_Muid_Weights',
                                                       'MET_Tau_Weights','MET_Jet_Weights',
                                                       'MET_Cluster_Weights','MET_Track_Weights'])

    # ... DR = 0.15
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_15",                prefix = "hr_roughRecoil_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_15",               prefix = "hr_ueCorrection_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_15",              prefix = "hr_corrRecoil_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_15",                     prefix = "hr_MET_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_15",          prefix = "hr_roughRecoil_Eflow_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_15",         prefix = "hr_ueCorrection_Eflow_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_15",        prefix = "hr_corrRecoil_Eflow_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_15",               prefix = "hr_MET_Eflow_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_15",          prefix = "hr_roughRecoil_track_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_15",         prefix = "hr_ueCorrection_track_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_15",        prefix = "hr_corrRecoil_track_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_15",               prefix = "hr_MET_track_15",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    # ... DR = 0.20
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_20",                prefix = "hr_roughRecoil_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_20",               prefix = "hr_ueCorrection_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_20",              prefix = "hr_corrRecoil_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_20",                     prefix = "hr_MET_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_20",          prefix = "hr_roughRecoil_track_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_20",         prefix = "hr_ueCorrection_track_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_20",        prefix = "hr_corrRecoil_track_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_20",               prefix = "hr_MET_track_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_clusNoTrack_20",    prefix = "hr_roughRecoil_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_clusNoTrack_20",   prefix = "hr_ueCorrection_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_clusNoTrack_20",  prefix = "hr_corrRecoil_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_clusNoTrack_20",         prefix = "hr_MET_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_20",          prefix = "hr_roughRecoil_Eflow_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_20",         prefix = "hr_ueCorrection_Eflow_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_20",        prefix = "hr_corrRecoil_Eflow_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_20",               prefix = "hr_MET_Eflow_20",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    # ... DR = 0.25
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_25",                prefix = "hr_roughRecoil_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_25",               prefix = "hr_ueCorrection_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_25",              prefix = "hr_corrRecoil_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_25",                     prefix = "hr_MET_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_25",          prefix = "hr_roughRecoil_Eflow_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_25",         prefix = "hr_ueCorrection_Eflow_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_25",        prefix = "hr_corrRecoil_Eflow_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_25",               prefix = "hr_MET_Eflow_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_25",          prefix = "hr_roughRecoil_track_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_25",         prefix = "hr_ueCorrection_track_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_25",        prefix = "hr_corrRecoil_track_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_25",               prefix = "hr_MET_track_25",
                                exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )


    # HR using truth leptons
    if rec.doTruth():

        ## ... DR = 0.15
        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_15",
                                    prefix = "hr_roughRecoil_trLep_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_15",
                                    prefix = "hr_ueCorrection_trLep_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_15",
                                    prefix = "hr_corrRecoil_trLep_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_15",
                                    prefix = "hr_MET_trLep_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_15",
                                    prefix = "hr_roughRecoil_trLep_Eflow_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_15",
                                    prefix = "hr_ueCorrection_trLep_Eflow_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_15",
                                    prefix = "hr_corrRecoil_trLep_Eflow_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_15",
                                    prefix = "hr_MET_trLep_Eflow_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_15",
                                    prefix = "hr_roughRecoil_trLep_track_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_15",
                                    prefix = "hr_ueCorrection_trLep_track_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_15",
                                    prefix = "hr_corrRecoil_trLep_track_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_15",
                                    prefix = "hr_MET_trLep_track_15",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        ## ... DR = 0.20
        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_20",
                                    prefix = "hr_roughRecoil_trLep_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_20",
                                    prefix = "hr_ueCorrection_trLep_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_20",
                                    prefix = "hr_corrRecoil_trLep_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_20",
                                    prefix = "hr_MET_trLep_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_20",
                                    prefix = "hr_roughRecoil_trLep_Eflow_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_20",
                                    prefix = "hr_ueCorrection_trLep_Eflow_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_20",
                                    prefix = "hr_corrRecoil_trLep_Eflow_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_20",
                                    prefix = "hr_MET_trLep_Eflow_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_20",
                                    prefix = "hr_roughRecoil_trLep_track_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_20",
                                    prefix = "hr_ueCorrection_trLep_track_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_20",
                                    prefix = "hr_corrRecoil_trLep_track_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_20",
                                    prefix = "hr_MET_trLep_track_20",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        ## ... DR = 0.25
        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_25",
                                    prefix = "hr_roughRecoil_trLep_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_25",
                                    prefix = "hr_ueCorrection_trLep_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_25",
                                    prefix = "hr_corrRecoil_trLep_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_25",
                                    prefix = "hr_MET_trLep_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_25",
                                    prefix = "hr_roughRecoil_trLep_Eflow_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_25",
                                    prefix = "hr_ueCorrection_trLep_Eflow_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_25",
                                    prefix = "hr_corrRecoil_trLep_Eflow_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_25",
                                    prefix = "hr_MET_trLep_Eflow_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_25",
                                    prefix = "hr_roughRecoil_trLep_track_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_25",
                                    prefix = "hr_ueCorrection_trLep_track_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_25",
                                    prefix = "hr_corrRecoil_trLep_track_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_25",
                                    prefix = "hr_MET_trLep_track_25",
                                    exclude=['MET_Regions','MET_Comps','MET_Phi','MET_Et'] )
        pass  # end of rec.doTruth() 
    

    ######################### end SMWZ MET part #############################


    ######################### SUSY MET part #############################
    SUSY_METIncludes = ['MET_RefEle_Comps', 'MET_RefJet_Comps', 'MET_RefMuon_Comps', 'MET_SoftJets_Comps',
                        'MET_MuonBoy_Comps', 'MET_RefMuon_Staco_Comps', 'MET_RefMuon_Muid_Comps',
                        'MET_RefTau_Comps', 'MET_CellOut_Comps', 'MET_CellOut_Eflow_Comps', 'MET_LocHadTopo_Comps',
                        'MET_Muon_Isol_Staco_Comps', 'MET_Muon_NonIsol_Staco_Comps','MET_RefGamma_Comps',
                        'MET_Muon_Total_Staco_Comps', 'MET_Final_Comps', 'MET_Muon_Isol_Muid_Comps', 'MET_Muon_NonIsol_Muid_Comps',
                        'MET_Muon_Total_Muid_Comps', 'MET_Truth_Comps',  'MET_RefMuon_Track_Staco_Comps',
                        'MET_RefMuon_Track_Muid_Comps', 'MET_Track_Comps', 'MET_TopoObj_Comps',
                        'MET_RefFinal_em_Comps', 'MET_RefJet_em_Comps','MET_RefEle_em_Comps','MET_RefGamma_em_Comps', 'MET_RefTau_em_Comps',
                        'MET_RefMuon_Track_em_Comps', 'MET_SoftJets_em_Comps', 'MET_RefMuon_em_Comps','MET_RefMuon_Track_Comps', 'MET_Muon_Isol_Comps',
                        'MET_Muon_NonIsol_Comps', 'MET_Muon_Total_Comps','MET_CellOut_em_Comps']

    #MET Truth
    SUSY_METIncludes += [ 'MET_Truth_NonInt', 'MET_Truth_NonInt_SumEt', 'MET_Truth_Int', 'MET_Truth_Int_SumEt','MET_Truth_SumEt']
        
    #MET SumET
    SUSY_METIncludes += ['MET_RefEle_SumEt', 'MET_RefJet_SumEt','MET_RefMuon_SumEt', 'MET_SoftJets_SumEt', 'MET_MuonBoy_SumEt',
                         'MET_RefMuon_Staco_SumEt','MET_RefMuon_Muid_SumEt','MET_RefTau_SumEt', 'MET_CellOut_SumEt', 'MET_CellOut_Eflow_SumEt',
                         'MET_LocHadTopo_SumEt', 'MET_Muon_Isol_Staco_SumEt','MET_Muon_NonIsol_Staco_SumEt', 'MET_Muon_Total_Staco_SumEt',
                         'MET_Final_SumEt', 'MET_Muon_Isol_Muid_SumEt','MET_Muon_NonIsol_Muid_SumEt', 'MET_Muon_Total_Muid_SumEt',
                         'MET_RefMuon_Track_Staco_SumEt','MET_RefMuon_Track_Muid_SumEt', 'MET_Track_SumEt', 'MET_TopoObj_SumEt', 'MET_RefFinal_em_SumEt',
                         'MET_RefJet_em_SumEt','MET_RefEle_em_SumEt','MET_RefGamma_em_SumEt', 'MET_RefTau_em_SumEt', 'MET_CellOut_em','MET_RefMuon_Track_em_SumEt',
                         'MET_SoftJets_em_SumEt', 'MET_RefMuon_em_SumEt', 'MET_RefMuon_Track_SumEt','MET_Muon_Isol_SumEt', 'MET_Muon_NonIsol_SumEt',
                         'MET_Muon_Total_SumEt']

    #MET STVF
    SUSY_METIncludes += ['MET_CellOut_Eflow_STVF','MET_RefJet_JVF','MET_RefJet_JVFCut']                       

    #MET Regions
    #SUSY_METIncludes += ['MET_RefEle_Regions','MET_RefJet_Regions','MET_RefMuon_Regions','MET_SoftJets_Regions','MET_MuonBoy_Regions',
    #                     'MET_RefMuon_Staco_Regions','MET_RefMuon_Muid_Regions', 'MET_RefTau_Regions','MET_CellOut_Regions','MET_CellOut_Eflow_Regions',
    #                     'MET_LocHadTopo_Regions','MET_RefMuon_Track_Staco_Regions','MET_RefMuon_Track_Muid_Regions','MET_Muon_Isol_Staco_Regions',
    #                     'MET_Muon_NonIsol_Staco_Regions','MET_Muon_Total_Staco_Regions','MET_Final_Regions','MET_Muon_Isol_Muid_Regions',
    #                     'MET_Muon_NonIsol_Muid_Regions','MET_Muon_Total_Muid_Regions','MET_Track_Regions','MET_TopoObj_Regions','MET_RefMuon_Track_Regions',
    #                     'MET_Muon_Isol_Regions','MET_Muon_NonIsol_Regions','MET_Muon_Total_Regions','MET_RefGamma_Regions','MET_RefFinal_STVF_Regions',
    #                     'MET_CellOut_Eflow_STVF_Regions','MET_RefJet_JVF_Regions','MET_RefJet_JVFCut_Regions']

    MissingETD3PDMakerFlags.doMissingETRegions = True
    MissingETD3PDMakerFlags.doCellOutEflow = True
    #alg += MissingETD3PDObject(**_args (0, 'MissingEt', kw, allowMissing=True, include = SUSY_METIncludes, exclude = ["MET_Cluster_Weights"]))

    SUSY_custom_parts = ['MET_RefGamma', 'MET_RefFinal', 'MET_RefJet', 'MET_RefEle', 'MET_Muon_Total_Staco',
                         'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_CellOut','MET_CellOut_Eflow',
                         'MET_Muon_Isol_Muid', 'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid', 'MET_Track',
                         'MET_CellOut_Eflow_STVF','MET_RefJet_JVF','MET_RefJet_JVFCut','MET_RefFinal_STVF',
                         'MET_SoftJets','MET_RefMuon','MET_MuonBoy','MET_RefMuon_Staco','MET_RefMuon_Muid','MET_RefMuon_Track',
                         'MET_RefMuon_Track_Staco','MET_RefMuon_Track_Muid']

    SUSY_custom_parts_PU = ['MET_RefGamma', 'MET_RefFinal', 'MET_RefJet', 'MET_RefEle', 'MET_Muon_Total_Staco',
                            'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_CellOut','MET_CellOut_Eflow',
                            'MET_Muon_Isol_Muid', 'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid', 'MET_Track',
                            'MET_CellOut_Eflow_STVF','MET_RefJet_JVF','MET_RefJet_JVFCut','MET_RefFinal_STVF',
                            'MET_SoftJets','MET_RefMuon','MET_MuonBoy','MET_RefMuon_Staco','MET_RefMuon_Muid','MET_RefMuon_Track',
                            'MET_RefMuon_Track_Staco','MET_RefMuon_Track_Muid',
                            'MET_CellOut_Eflow_JetArea', 'MET_CellOut_Eflow_JetAreaJVF','MET_CellOut_Eflow_JetAreaRhoEta5JVF']

    #for custom in SUSY_custom_parts_PU:
        #alg += MissingETD3PDObject (level=0, sgkey = custom+'_Egamma10NoTau',
        #                            prefix=custom+'_Egamma10NoTau',
        #                            allowMissing=True, exclude=['MET_Regions', 'MET_Phi','MET_Et'])
        #pass

    #- MET weights
    SUSY_exclusionList = ['MET_Photon_Weights', 'MET_Tau_Weights', 'MET_Cluster_Weights','MET_Track_Weights']
    SUSY_exclusionList_withPhotons = ['MET_Tau_Weights', 'MET_Cluster_Weights','MET_Track_Weights']

    #alg += MissingETCompositionD3PDObject (level=4, sgkey = 'MET_RefComposition_Egamma10NoTau', suffix='Egamma10NoTau_',
    #                                       allowMissing = True, exclude = ['MET_Tau_Weights', 'MET_Cluster_Weights','MET_Track_Weights'],
    #                                       jetSGKey = MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey()) # jets default calib for MET_RefFinal
    

    ######################### end SUSY MET part #############################

    ######################### TOP MET part #############################
    #main Top custom MET blocks
    TOP_custom_MET =      [ "MET_RefEle", "MET_RefJet", "MET_SoftJets", "MET_CellOut", "MET_Muon_Isol_Muid",
                          "MET_Muon_NonIsol_Muid", "MET_Muon_Total_Muid", "MET_RefGamma", "MET_RefFinal",
                          "MET_Muon_Isol_Muons", "MET_Muon_NonIsol_Muons", "MET_Muon_Total_Muons" ]
    TOP_MEt_exclude=["MET_Regions", "MET_Phi", "MET_Et"]

    for custom in TOP_custom_MET:
        #alg += MissingETD3PDObject        (**_args (2, custom+'_em_tightpp', kw,
        #                                            sgkey  = custom+'_em_tightpp',
        #                                            prefix = custom+'_em_tightpp',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_em_mediumpp', kw,
        #                                            sgkey  = custom+'_em_mediumpp',
        #                                            prefix = custom+'_em_mediumpp',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_em_loosepp', kw,
        #                                            sgkey  = custom+'_em_loosepp',
        #                                            prefix = custom+'_em_loosepp',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        alg += MissingETD3PDObject        (**_args (2, custom+'_tightpp', kw,
                                                    sgkey  = custom+'_tightpp',
                                                    prefix = custom+'_tightpp',
                                                    exclude = TOP_MEt_exclude,
                                                    allowMissing = True))

        alg += MissingETD3PDObject        (**_args (2, custom+'_tightpp_AntiKt5LCTopo', kw,
                                                    sgkey  = custom+'_tightpp_AntiKt5LCTopo',
                                                    prefix = custom+'_tightpp_AntiKt5LCTopo',
                                                    exclude = TOP_MEt_exclude,
                                                    allowMissing = True))

        alg += MissingETD3PDObject        (**_args (2, custom+'_mediumpp', kw,
                                                    sgkey  = custom+'_mediumpp',
                                                    prefix = custom+'_mediumpp',
                                                    exclude = TOP_MEt_exclude,
                                                    allowMissing = True))

        alg += MissingETD3PDObject        (**_args (2, custom+'_mediumpp_AntiKt5LCTopo', kw,
                                                    sgkey  = custom+'_mediumpp_AntiKt5LCTopo',
                                                    prefix = custom+'_mediumpp_AntiKt5LCTopo',
                                                    exclude = TOP_MEt_exclude,
                                                    allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_loosepp', kw,
        #                                            sgkey  = custom+'_loosepp',
        #                                            prefix = custom+'_loosepp',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_tightpp_photonTightAR', kw,
        #                                            sgkey  = custom+'_tightpp_photonTightAR',
        #                                            prefix = custom+'_tightpp_photonTightAR',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_mediumpp_photonTightAR', kw,
        #                                            sgkey  = custom+'_mediumpp_photonTightAR',
        #                                            prefix = custom+'_mediumpp_photonTightAR',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))

        #alg += MissingETD3PDObject        (**_args (2, custom+'_tightpp_photonLooseAR', kw,
        #                                            sgkey  = custom+'_tightpp_photonLooseAR',
        #                                            prefix = custom+'_tightpp_photonLooseAR',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))
        pass

    #default MET blocks
    #alg += MissingETD3PDObject        (**_args (2, 'MET_RefFinal_em', kw,
    #                                            sgkey  = 'MET_RefFinal_em',
    #                                            prefix = 'MET_RefFinal_em',
    #                                            exclude = TOP_MEt_exclude,
    #                                            allowMissing = True))

    #alg += MissingETD3PDObject        (**_args (2, 'MET_RefFinal', kw,
    #                                            sgkey  = 'MET_RefFinal',
    #                                            prefix = 'MET_RefFinal',
    #                                            exclude = TOP_MEt_exclude,
    #                                            allowMissing = True))

    #default ExclRefCalib MET with tightpp electrons and muid muons
    TOP_customMETs_ExclRefCalib = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                                   'MET_CellOut', 'MET_Cryo', 'MET_SoftJets', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                                   'MET_CellOut_Eflow_STVF'] #, 'MET_RefFinal_STVF' ]
    for custom in TOP_customMETs_ExclRefCalib:
        #alg += MissingETD3PDObject        (**_args (2, custom+'_tightpp_ExclRefCalib', kw,
        #                                            sgkey  = custom+'_tightpp_ExclRefCalib',
        #                                            prefix = custom+'_tightpp_ExclRefCalib',
        #                                            exclude = TOP_MEt_exclude,
        #                                            allowMissing = True))
        pass

    #STVF MET blocks
    TOP_customMETs_STVF = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                           'MET_RefMuon' ,'MET_RefMuon_Staco',
                           'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco',  'Muon_Total_Staco',
                           'MET_RefMuon_Track', 'MET_RefMuon_Track_Staco','MET_CellOut', 'MET_CellOut_Track',
                           'MET_CellOut_TrackPV', 'MET_CellOutCorr', 'MET_Cryo', 'MET_SoftJets']
    for custom in TOP_customMETs_STVF:
        for postfix in ['','_top']:
            if (custom == 'MET_RefFinal') and (postfix == ''):
                continue
            alg += MissingETD3PDObject (**_args (2, custom+'STVF'+postfix, kw, sgkey = custom+'_STVF'+postfix, prefix=custom+'_STVF'+postfix,
                                                 exclude = TOP_MEt_exclude, allowMissing = True))
            pass
        pass


    TOP_composition_exclude=[ "MET_Cluster_Weights",'MET_Track_Weights','MET_Tau_Weights']
    
    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_em_tightpp",kw,
    #                                                sgkey ='MET_RefComposition_em_tightpp',
    #                                                suffix="em_tightpp_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4TopoEMNewJets",
    #                                                jetPrefix='jet_AntiKt4TopoEM_MET_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_em_mediumpp",kw,
    #                                                sgkey ='MET_RefComposition_em_mediumpp',
    #                                                suffix="em_mediumpp_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4TopoEMNewJets",
    #                                                jetPrefix='jet_AntiKt4TopoEM_MET_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_em_loosepp",kw,
    #                                                sgkey ='MET_RefComposition_em_loosepp',
    #                                                suffix="em_loosepp_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4TopoEMNewJets",
    #                                                jetPrefix='jet_AntiKt4TopoEM_MET_'))

    alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_tightpp",kw,
                                                    sgkey ='MET_RefComposition_tightpp',
                                                    suffix="tightpp_",
                                                    exclude = TOP_composition_exclude,
                                                    allowMissing = True,
                                                    jetSGKey="AntiKt4LCTopoNewJets",
                                                    jetPrefix='jet_AntiKt4LCTopo_MET_'))

    alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_tightpp_AntiKt5LCTopo",kw,
                                                    sgkey ='MET_RefComposition_tightpp_AntiKt5LCTopo',
                                                    suffix="tightpp_AntiKt5LCTopo_",
                                                    exclude = TOP_composition_exclude,
                                                    allowMissing = True,
                                                    jetSGKey="AntiKt5LCTopoNewJets",
                                                    jetPrefix='jet_AntiKt5LCTopo_MET_'))

    alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_mediumpp",kw,
                                                    sgkey ='MET_RefComposition_mediumpp',
                                                    suffix="mediumpp_",
                                                    exclude = TOP_composition_exclude,
                                                    allowMissing = True,
                                                    jetSGKey="AntiKt4LCTopoNewJets",
                                                    jetPrefix='jet_AntiKt4LCTopo_MET_'))

    alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_mediumpp_AntiKt5LCTopo",kw,
                                                    sgkey ='MET_RefComposition_mediumpp_AntiKt5LCTopo',
                                                    suffix="mediumpp_AntiKt5LCTopo_",
                                                    exclude = TOP_composition_exclude,
                                                    allowMissing = True,
                                                    jetSGKey="AntiKt5LCTopoNewJets",
                                                    jetPrefix='jet_AntiKt5LCTopo_MET_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_loosepp",kw,
    #                                                sgkey ='MET_RefComposition_loosepp',
    #                                                suffix="loosepp_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4LCTopoNewJets",
    #                                                jetPrefix='jet_AntiKt4LCTopo_MET_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_tightpp_photonTightAR",kw,
    #                                                sgkey ='MET_RefComposition_tightpp_photonTightAR',
    #                                                suffix="tightpp_photonTightAR_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4LCTopoNewJets",
    #                                                jetPrefix='jet_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_mediumpp_photonTightAR",kw,
    #                                                sgkey ='MET_RefComposition_mediumpp_photonTightAR',
    #                                                suffix="mediumpp_photonTightAR_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4LCTopoNewJets",
    #                                                jetPrefix='jet_'))

    #alg += MissingETCompositionD3PDObject (**_args (4,"MET_RefComposition_tightpp_photonLooseAR",kw,
    #                                                sgkey ='MET_RefComposition_tightpp_photonLooseAR',
    #                                                suffix="tightpp_photonLooseAR_",
    #                                                exclude = TOP_composition_exclude,
    #                                                allowMissing = True,
    #                                                jetSGKey="AntiKt4LCTopoNewJets",
    #                                                jetPrefix='jet_'))
                           

    ######################### end TOP MET part #############################

    ######################### TAU MET part #############################
    TAU_METIncludeList=['MET_MuonBoy', 'MET_Track', 'MET_RefGamma', 'MET_RefEle', 'MET_RefJet', 'MET_RefMuon',
                        'MET_RefMuon_Track', 'MET_RefTau', 'MET_RefFinal', 'MET_RefFinal_em', 'MET_SoftJets',
                        'MET_SoftJets_em', 'MET_Cryo', 'MET_CellOut', 'MET_CellOut_Eflow', 'MET_Muid', 'MET_Muon',
                        'MET_Final', 'MET_Topo', 'MET_Topo_Samplings', 'MET_Topo_Regions', 'MET_LocHadTopo', 'MET_LocHadTopo_Samplings',
                        'MET_LocHadTopo_Regions', 'MET_Truth_NonInt', 'METComposition']


    alg += MissingETGoodnessD3PDObject (0)

    ######################### end TAU MET part #############################

    ######################### QCD MET part #############################
    QCD_MET_include = ['MET_Calo_Samplings',
                       'MET_Staco',
                       'MET_Truth_Int','MET_RefJet','MET_RefGamma','MET_LocHadTopoObj','MET_MuonBoy_Track',
                       'MET_MuonBoy_Spectro']
    QCD_MET_include += ['METComposition']

    ################ Main MET #################
    alg += MissingETD3PDObject (**_args (10, 'MissingET', kw,
                                         exclude= ['MissingET_Phi', 'MissingET_Et'],
                                         include = SUSY_METIncludes + TAU_METIncludeList + QCD_MET_include,
                                         allowMissing=True ) )
    ###########################################

    alg += MissingETCompositionD3PDObject ( level=4,
                                            sgkey = 'MET_RefComposition_noEflow',
                                            suffix='noEflow_',
                                            allowMissing=True,
                                            jetSGKey='AntiKt4LCTopoNewJets',
                                            jetPrefix='jet_AntiKt4LCTopo_MET_',
                                            exclude = ['MET_Muid_Weights','MET_Muons_Weights',
                                                       'MET_Track_Weights','MET_Cluster_Weights'])
    alg += MissingETCompositionD3PDObject ( level=4,
                                            sgkey = 'MET_RefComposition_Eflow_PV',
                                            suffix='Eflow_PV_',
                                            allowMissing=True,
                                            jetSGKey='AntiKt4LCTopoNewJets',
                                            jetPrefix='jet_AntiKt4LCTopo_MET_',
                                            exclude = ['MET_Muid_Weights','MET_Muons_Weights',
                                                       'MET_Track_Weights','MET_Cluster_Weights'])


    QCD_customMETs = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                      'MET_CellOut', 'MET_Cryo', 'MET_SoftJets', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                      'MET_CellOut_Eflow_STVF','MET_RefFinal_STVF' ]
    QCD_customMETs_Eflow_PV = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                               'MET_CellOut', 'MET_Cryo', 'MET_SoftJets', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                               'MET_CellOut_Eflow_STVF','MET_CellOut_Eflow_PV','MET_RefFinal_STVF' ]
    QCD_customMETs_PU = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                         'MET_CellOut', 'MET_Cryo', 'MET_SoftJets', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                         'MET_CellOut_Eflow_STVF','MET_RefFinal_STVF',
                         'MET_CellOut_Eflow_JetArea','MET_CellOut_Eflow_JetAreaJVF','MET_CellOut_Eflow_JetAreaRhoEta5JVF']


    customMETs_Staco = ['MET_RefMuon' ,'MET_RefMuon_Staco',
                        'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_Muon_Total_Staco',
                        'MET_RefMuon_Track', 'MET_RefMuon_Track_Staco']
    customMETs_Muid = ['MET_RefMuon' ,'MET_RefMuon_Muid',
                       'MET_Muon_Isol_Muid', 'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid',
                       'MET_RefMuon_Track', 'MET_RefMuon_Track_Muid']
    customMETs_conflistStaco = ['_noEflow']
    customMETs_conflistMuid =  ['_LCW_pt20', '_LCW_pt20_EM']
    customMETs_conflistPU = []

    missEtIncludeNew = ['MET_Calo_Samplings','MET_Topo','MET_LocHadTopo','MET_MuonBoy','MET_RefMuon_Track',
                        'MET_Muid','MET_Staco','MET_CellOut','MET_RefEle','MET_RefMuon',
                        'MET_Truth_NonInt','MET_Truth_Int','MET_RefJet','MET_RefGamma',
                        'MET_LocHadTopoObj','MET_MuonBoy_Track','MET_MuonBoy_Spectro']
    
    for conflist in customMETs_conflistStaco:
        #for custom in QCD_customMETs:
        for custom in (QCD_customMETs + customMETs_Staco):
            alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
                                                prefix=custom+conflist,
                                                #exclude=['MET_Phi','MET_Et'],
                                                exclude=['MET_Regions','MET_Phi','MET_Et'],
                                                include=missEtIncludeNew, allowMissing = True))
            pass
        #for custom in customMETs_Staco_wo_region:
        #    alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
        #                                        prefix=custom+conflist,
        #                                        include=missEtIncludeNew, exclude = ['MET_Regions','MET_Phi','MET_Et'],
        #                                        allowMissing = True))
        #    pass
        pass

    for conflist in customMETs_conflistMuid:
        #for custom in QCD_customMETs:
        for custom in (QCD_customMETs + customMETs_Muid):
            alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
                                                prefix=custom+conflist,
                                                #exclude=['MET_Phi','MET_Et'],
                                                exclude=['MET_Regions','MET_Phi','MET_Et'],
                                                include=missEtIncludeNew, allowMissing = True))
            pass
        #for custom in customMETs_Muid_wo_region:
        #    alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
        #                                        prefix=custom+conflist,
        #                                        include=missEtIncludeNew, exclude = ['MET_Regions','MET_Phi','MET_Et'],
        #                                        allowMissing = True))
        #    pass
        pass
    for conflist in customMETs_conflistPU:
        #for custom in QCD_customMETs_PU:
        for custom in (QCD_customMETs_PU + customMETs_Staco):
            alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
                                                prefix=custom+conflist,
                                                #exclude=['MET_Phi','MET_Et'],
                                                exclude=['MET_Regions','MET_Phi','MET_Et'],
                                                include=missEtIncludeNew, allowMissing = True))
            pass
        #for custom in customMETs_Staco_wo_region:
        #    alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist,
        #                                        prefix=custom+conflist,
        #                                        include=missEtIncludeNew, exclude = ['MET_Regions','MET_Phi','MET_Et'],
        #                                        allowMissing = True))
        #    pass
        pass
    #Eflow_PV
    for custom in QCD_customMETs_Eflow_PV:
        alg += MissingETD3PDObject (**_args(0, 'MissingET_Eflow_PV', kw, sgkey = custom+'_Eflow_PV',
                                            prefix=custom+'_Eflow_PV',
                                            #exclude=['MET_Phi','MET_Et'],
                                            exclude=['MET_Regions','MET_Phi','MET_Et'],
                                            include=missEtIncludeNew, allowMissing = True))
        pass

    #alg += MissingETGoodnessD3PDObject(**_args(3, 'MissingETGoodness', kw, allowMissing = True))
            

    ######################### end QCD MET part #############################

    ######################### Photon MET part #############################

    METFlavors = ['MET_CellOutCorr','MET_CellOut_Track',
                  'MET_CellOut_TrackPV',
                  'MET_RefGamma', 'MET_RefFinal', 'MET_RefFinal_STVF', 'MET_RefJet', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                  'MET_RefEle', 'MET_Muon_Total_Staco', 'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_Muon_Isol_Muid',
                  'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid', 'MET_Track', 'MET_CellOut', 'MET_CellOut_Eflow', 'MET_CellOut_Eflow_STVF', 
                  'MET_CellOut_Eflow_JetArea', 'MET_RefTau', 'MET_RefMuon']

    PhotonCustomMETCollections = [] #'_PhotonLoose_Calib_OR_stdvert', '_PhotonTight_Calib_OR_stdvert', '_NNTight_Calib_OR_stdvert']

    for coll in PhotonCustomMETCollections:
        for flavor in METFlavors:
            alg += MissingETD3PDObject (level=0, sgkey =flavor+coll,
                                        prefix=flavor+coll,
                                        #exclude=['MET_Phi','MET_Et'],
                                        exclude=['MET_Regions','MET_Phi','MET_Et'],
                                        allowMissing=True)
            pass

        alg += MissingETCompositionD3PDObject (level=4,
                                               sgkey = 'MET_RefComposition'+coll,
                                               suffix=coll[1:]+"_",
                                               jetSGKey='AntiKt4LCTopoNewJets',
                                               exclude=['MET_Track_Weights','MET_Cluster_Weights'],
                                               allowMissing = True)
        pass


    ######################### end Photon MET part #############################


    return alg
