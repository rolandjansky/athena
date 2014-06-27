# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags                            import rec
from AthenaCommon.SystemOfUnits                      import GeV
from AthenaPython.PyAthena                           import egammaPID
from MissingET.METRefGetter_plup                     import make_METRefAlg
from PhotonAnalysisUtils.PhotonNeuralNetworkTool     import PhotonNeuralNetworkTool
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from AthenaCommon.AlgSequence                        import AlgSequence
from QcdD3PDMaker.QcdD3PDMakerFlags                  import QcdD3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags

import D3PDMakerCoreComps
import PhotonD3PDMaker

import AthenaPython.PyAthena as PyAthena
PyAthena.load_library('egammaEnumsDict')


def addPhotonMET():
    from AthenaCommon.AppMgr                             import ToolSvc

    topSequence = AlgSequence()    

    photongetter = D3PDMakerCoreComps.SGDataVectorGetterTool('photonTight_Getter',
                                                             TypeName = 'PhotonContainer',
                                                             SGKey = D3PDMakerFlags.PhotonSGKey())
    ToolSvc += photongetter

    electrongetter = D3PDMakerCoreComps.SGDataVectorGetterTool('electronTight_Getter',
                                                               TypeName = 'ElectronContainer',
                                                               SGKey = D3PDMakerFlags.ElectronSGKey())
    ToolSvc += electrongetter

    # -------------------------------------------------------------------------------------------
    #
    # Filters for NN!!!
    #
    # add a filter to select NN tight photons
    #
    photonFilterNNTight = PhotonD3PDMaker.PhotonFilter(OutputCollectionName="SelectedPhotonsNNTight",
                                                       photonGetter = photongetter,
                                                       ptCut = 20000,
                                                       etaCut = 2.5,
                                                       IsEM = 0,
                                                       NNCut = True,
                                                       PhotonNNTool = PhotonNeuralNetworkTool())
    topSequence += photonFilterNNTight

    #
    # add a filter to select normal electrons
    #
    electrongetter = D3PDMakerCoreComps.SGDataVectorGetterTool('electronNNTight_Getter',
                                                               TypeName = 'ElectronContainer',
                                                               SGKey = D3PDMakerFlags.ElectronSGKey())
    ToolSvc += electrongetter

    #
    # add a filter that will create both photon and electron containers, with proper overlap removal (OR)
    #
    egammaORFilter_NN = PhotonD3PDMaker.EgammaFilter('egammaOR_Filter_NN',
                                                     OutputElectronCollectionName="SelectedORElectronsNNTight",
                                                     ElectronGetter = electrongetter,
                                                     ElectronPtCut = 10000,
                                                     ElectronEtaCut = 2.47,
                                                     ElectronIsEM = egammaPID.ElectronIDMediumPP,
                                                     OutputPhotonCollectionName="SelectedORPhotonsNNTight",
                                                     PhotonGetter = photongetter,
                                                     PhotonPtCut = 20000,
                                                     PhotonEtaCut = 2.37,
                                                     PhotonIsEM = 0,
                                                     PhotonNNCut = True,
                                                     PhotonNNTool = PhotonNeuralNetworkTool())
    topSequence += egammaORFilter_NN


    #
    # add a filter that will create both photon and electron containers, with proper overlap removal (OR)
    # but using standard Tight
    #    
    egammaORFilter_tight = PhotonD3PDMaker.EgammaFilter('egamma_OR_Filter_tight',
                                                        OutputElectronCollectionName="SelectedORElectronsTight",
                                                        ElectronGetter = electrongetter,
                                                        ElectronPtCut = 10000,
                                                        ElectronEtaCut = 2.47,
                                                        ElectronIsEM = egammaPID.ElectronIDMediumPP,
                                                        OutputPhotonCollectionName="SelectedORPhotonsTight",
                                                        PhotonGetter = photongetter,
                                                        PhotonPtCut = 20000,
                                                        PhotonEtaCut = 2.37,
                                                        PhotonIsEM = egammaPID.PhotonIDTight,
                                                        PhotonNNCut = False,
                                                        PhotonNNTool = PhotonNeuralNetworkTool())
    topSequence += egammaORFilter_tight                      
    # add a filter that will create both photon and electron containers, 
    # with proper overlap removal (OR) but using standard Loose   
    egammaORFilter_loose = PhotonD3PDMaker.EgammaFilter('egamma_OR_Filter_loose',
                                                        OutputElectronCollectionName="SelectedORElectronsLoose",
                                                        ElectronGetter = electrongetter,
                                                        ElectronPtCut = 10000,
                                                        ElectronEtaCut = 2.47,
                                                        ElectronIsEM = egammaPID.ElectronIDMediumPP,
                                                        OutputPhotonCollectionName="SelectedORPhotonsLoose",
                                                        PhotonGetter = photongetter,
                                                        PhotonPtCut = 20000,
                                                        PhotonEtaCut = 2.37,
                                                        PhotonIsEM = egammaPID.PhotonIDLoose,
                                                        PhotonNNCut = False,
                                                        PhotonNNTool = PhotonNeuralNetworkTool())
    topSequence += egammaORFilter_loose

    # -------------------------------------------------------------------------------------------

    METcollections=[]

    # Toggle for setting some options that we used to enable for various STVF configurations,
    # before the MET configuration changes in 17.2.7.4.1.
    doOldConfiguration=False

    # Loose photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 1:
        suffix='_PhotonLoose_Calib_OR_stdvert'
        MET_PhotonLoose_Calib_OR_stdvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonLoose_Calib_OR_stdvert.photon_doPhotonTool              = True
        MET_PhotonLoose_Calib_OR_stdvert.gamma_noCut                      = True
        MET_PhotonLoose_Calib_OR_stdvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsLoose"
        MET_PhotonLoose_Calib_OR_stdvert.gamma_EleInputCollectionKey      = "SelectedORElectronsLoose"
        MET_PhotonLoose_Calib_OR_stdvert.gamma_calibType                  = "RefCalib"
        MET_PhotonLoose_Calib_OR_stdvert.ele_noCut                        = True
        MET_PhotonLoose_Calib_OR_stdvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsLoose"
        MET_PhotonLoose_Calib_OR_stdvert.ele_calibType                    = "RefCalib"
        MET_PhotonLoose_Calib_OR_stdvert.jet_calibType                    = 'ExclRefCalib'
        if doOldConfiguration:
            MET_PhotonLoose_Calib_OR_stdvert.jet_UseCustomVxContainer         = False
            MET_PhotonLoose_Calib_OR_stdvert.jet_JetInputCollectionKey        ='AntiKt4LCTopoJets'
            MET_PhotonLoose_Calib_OR_stdvert.jet_JetPtCut                     = 20.0*GeV
            MET_PhotonLoose_Calib_OR_stdvert.jet_ApplyJetScale                = "Yes"
            MET_PhotonLoose_Calib_OR_stdvert.jet_UseJetMomentForScale         = True
            MET_PhotonLoose_Calib_OR_stdvert.jet_JetMomentForScale            = "LCJES"
            MET_PhotonLoose_Calib_OR_stdvert.jet_ApplyJetJVF                  = "Yes"
            MET_PhotonLoose_Calib_OR_stdvert.jet_RunSoftJetsTool              = False
            MET_PhotonLoose_Calib_OR_stdvert.cellout_calibType                ='Eflow'
            MET_PhotonLoose_Calib_OR_stdvert.tau_calibType                    ='ExclRefCalib'
            MET_PhotonLoose_Calib_OR_stdvert.cryo_ApplyCorrection             = "Off"
            MET_PhotonLoose_Calib_OR_stdvert.muon_algorithm                   = "Staco"
            MET_PhotonLoose_Calib_OR_stdvert.muon_isolationAlg                = "dRJet"
        MET_PhotonLoose_Calib_OR_stdvert()
        METcollections += [suffix]


    # tight photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 1:
        suffix='_PhotonTight_Calib_OR_stdvert'
        MET_PhotonTight_Calib_OR_stdvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_Calib_OR_stdvert.photon_doPhotonTool              = True
        MET_PhotonTight_Calib_OR_stdvert.gamma_noCut                      = True
        MET_PhotonTight_Calib_OR_stdvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsTight"
        MET_PhotonTight_Calib_OR_stdvert.gamma_EleInputCollectionKey      = "SelectedORElectronsTight"
        MET_PhotonTight_Calib_OR_stdvert.gamma_calibType                  = "RefCalib"
        MET_PhotonTight_Calib_OR_stdvert.ele_noCut                        = True
        MET_PhotonTight_Calib_OR_stdvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsTight"
        MET_PhotonTight_Calib_OR_stdvert.ele_calibType                    = "RefCalib"
        MET_PhotonTight_Calib_OR_stdvert.jet_calibType                    = 'ExclRefCalib'
        if doOldConfiguration:
            MET_PhotonTight_Calib_OR_stdvert.jet_UseCustomVxContainer         = False
            MET_PhotonTight_Calib_OR_stdvert.jet_JetInputCollectionKey        ='AntiKt4LCTopoJets'
            MET_PhotonTight_Calib_OR_stdvert.jet_JetPtCut                     = 20.0*GeV
            MET_PhotonTight_Calib_OR_stdvert.jet_ApplyJetScale                = "Yes"
            MET_PhotonTight_Calib_OR_stdvert.jet_UseJetMomentForScale         = True
            MET_PhotonTight_Calib_OR_stdvert.jet_JetMomentForScale            = "LCJES"
            MET_PhotonTight_Calib_OR_stdvert.jet_ApplyJetJVF                  = "Yes"
            MET_PhotonTight_Calib_OR_stdvert.jet_RunSoftJetsTool              = False
            MET_PhotonTight_Calib_OR_stdvert.cellout_calibType                ='Eflow'
            MET_PhotonTight_Calib_OR_stdvert.tau_calibType                    ='ExclRefCalib'
            MET_PhotonTight_Calib_OR_stdvert.cryo_ApplyCorrection             = "Off"
            MET_PhotonTight_Calib_OR_stdvert.muon_algorithm                   = "Staco"
            MET_PhotonTight_Calib_OR_stdvert.muon_isolationAlg                = "dRJet"
        MET_PhotonTight_Calib_OR_stdvert()
        METcollections += [suffix]

    
    # NNTight photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 1:
        suffix = '_NNTight_Calib_OR_stdvert'
        MET_NNTight_Calib_OR_stdvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_Calib_OR_stdvert.photon_doPhotonTool              = True
        MET_NNTight_Calib_OR_stdvert.gamma_noCut                      = True
        MET_NNTight_Calib_OR_stdvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsNNTight"
        MET_NNTight_Calib_OR_stdvert.gamma_EleInputCollectionKey      = "SelectedORElectronsNNTight"
        MET_NNTight_Calib_OR_stdvert.gamma_calibType                  = "RefCalib"
        MET_NNTight_Calib_OR_stdvert.ele_noCut                        = True
        MET_NNTight_Calib_OR_stdvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsNNTight"
        MET_NNTight_Calib_OR_stdvert.ele_calibType                    = "RefCalib"
        MET_NNTight_Calib_OR_stdvert.jet_calibType                    = 'ExclRefCalib'
        if doOldConfiguration:
            MET_NNTight_Calib_OR_stdvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
            MET_NNTight_Calib_OR_stdvert.jet_JetPtCut                = 20.0*GeV
            MET_NNTight_Calib_OR_stdvert.jet_ApplyJetScale           = "Yes"
            MET_NNTight_Calib_OR_stdvert.jet_UseJetMomentForScale    = True
            MET_NNTight_Calib_OR_stdvert.jet_JetMomentForScale       = "LCJES"
            MET_NNTight_Calib_OR_stdvert.jet_ApplyJetJVF             = "Yes"
            MET_NNTight_Calib_OR_stdvert.jet_RunSoftJetsTool         = False
            MET_NNTight_Calib_OR_stdvert.cellout_calibType           ='Eflow'
            MET_NNTight_Calib_OR_stdvert.tau_calibType               ='ExclRefCalib'
            MET_NNTight_Calib_OR_stdvert.cryo_ApplyCorrection        = "Off"
            MET_NNTight_Calib_OR_stdvert.muon_algorithm              = "Staco"
            MET_NNTight_Calib_OR_stdvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_Calib_OR_stdvert()
        METcollections += [suffix]


    #-------------------------------------------------------------------------------------------------
    # Probably not-needed collections, but kept for detailed studies if people need them.
    # Use at your own risk!
    #-------------------------------------------------------------------------------------------------

    # Loose photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonLoose_EmScale'
        MET_PhotonLoose_EmScale = make_METRefAlg(_suffix=suffix)
        MET_PhotonLoose_EmScale.photon_doPhotonTool         = True
        MET_PhotonLoose_EmScale.gamma_EtCut                 = 20.0*GeV;
        MET_PhotonLoose_EmScale.gamma_isEM                  = egammaPID.PhotonIDLoose
        MET_PhotonLoose_EmScale.gamma_calibType             = "EmScale"
        MET_PhotonLoose_EmScale()
        METcollections += [suffix]

    # tight photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_EmScale_stdvert'
        MET_PhotonTight_EmScale_stdvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_EmScale_stdvert.photon_doPhotonTool         = True
        MET_PhotonTight_EmScale_stdvert.gamma_EtCut                 = 20.0*GeV;
        MET_PhotonTight_EmScale_stdvert.gamma_isEM                  = egammaPID.PhotonIDTight
        MET_PhotonTight_EmScale_stdvert.gamma_calibType             = "EmScale"
        MET_PhotonTight_EmScale_stdvert.jet_UseCustomVxContainer    = False
        MET_PhotonTight_EmScale_stdvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_PhotonTight_EmScale_stdvert.jet_JetPtCut                = 20.0*GeV
        MET_PhotonTight_EmScale_stdvert.jet_ApplyJetScale           = "Yes"
        MET_PhotonTight_EmScale_stdvert.jet_UseJetMomentForScale    = True
        MET_PhotonTight_EmScale_stdvert.jet_JetMomentForScale       = "LCJES"
        MET_PhotonTight_EmScale_stdvert.jet_ApplyJetJVF             = "Yes"
        MET_PhotonTight_EmScale_stdvert.jet_RunSoftJetsTool         = False
        MET_PhotonTight_EmScale_stdvert.jet_calibType               = 'ExclRefCalib'
        MET_PhotonTight_EmScale_stdvert.ele_calibType               ='RefCalib'
        MET_PhotonTight_EmScale_stdvert.cellout_calibType           ='Eflow'
        MET_PhotonTight_EmScale_stdvert.tau_calibType               ='ExclRefCalib'
        MET_PhotonTight_EmScale_stdvert.cryo_ApplyCorrection        = "Off"
        MET_PhotonTight_EmScale_stdvert.muon_algorithm              = "Staco"
        MET_PhotonTight_EmScale_stdvert.muon_isolationAlg           = "dRJet"
        MET_PhotonTight_EmScale_stdvert()
        METcollections += [suffix]
    
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_EmScale_newvert'
        MET_PhotonTight_EmScale_newvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_EmScale_newvert.photon_doPhotonTool         = True
        MET_PhotonTight_EmScale_newvert.gamma_EtCut                 = 20.0*GeV;
        MET_PhotonTight_EmScale_newvert.gamma_isEM                  = egammaPID.PhotonIDTight
        MET_PhotonTight_EmScale_newvert.gamma_calibType             = "EmScale"
        MET_PhotonTight_EmScale_newvert.jet_vxcontainerkey          = "PAUDiphotonVertexCollection"
        MET_PhotonTight_EmScale_newvert.jet_UseCustomVxContainer    = True
        MET_PhotonTight_EmScale_newvert.cellout_vxcontainerkey      = "PAUDiphotonVertexCollection"
        MET_PhotonTight_EmScale_newvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_PhotonTight_EmScale_newvert.jet_JetPtCut                = 20.0*GeV
        MET_PhotonTight_EmScale_newvert.jet_ApplyJetScale           = "Yes"
        MET_PhotonTight_EmScale_newvert.jet_UseJetMomentForScale    = True
        MET_PhotonTight_EmScale_newvert.jet_JetMomentForScale       = "LCJES"
        MET_PhotonTight_EmScale_newvert.jet_ApplyJetJVF             = "Yes"
        MET_PhotonTight_EmScale_newvert.jet_RunSoftJetsTool         = False
        MET_PhotonTight_EmScale_newvert.jet_calibType               = 'ExclRefCalib'
        MET_PhotonTight_EmScale_newvert.ele_calibType               ='RefCalib'
        MET_PhotonTight_EmScale_newvert.cellout_calibType           ='Eflow'    
        MET_PhotonTight_EmScale_newvert.tau_calibType               ='ExclRefCalib'
        MET_PhotonTight_EmScale_newvert.cryo_ApplyCorrection        = "Off"
        MET_PhotonTight_EmScale_newvert.muon_algorithm              = "Staco"
        MET_PhotonTight_EmScale_newvert.muon_isolationAlg           = "dRJet"
        MET_PhotonTight_EmScale_newvert()
        METcollections += [suffix]
    
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_Calib_newvert'
        MET_PhotonTight_Calib_newvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_Calib_newvert.photon_doPhotonTool         = True
        MET_PhotonTight_Calib_newvert.gamma_EtCut                 = 20.0*GeV;
        MET_PhotonTight_Calib_newvert.gamma_isEM                  = egammaPID.PhotonIDTight
        MET_PhotonTight_Calib_newvert.gamma_calibType             = "RefCalib"
        MET_PhotonTight_Calib_newvert.jet_vxcontainerkey          = "PAUDiphotonVertexCollection"
        MET_PhotonTight_Calib_newvert.jet_UseCustomVxContainer    = True
        MET_PhotonTight_Calib_newvert.cellout_vxcontainerkey      = "PAUDiphotonVertexCollection"
        MET_PhotonTight_Calib_newvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_PhotonTight_Calib_newvert.jet_JetPtCut                = 20.0*GeV
        MET_PhotonTight_Calib_newvert.jet_ApplyJetScale           = "Yes"
        MET_PhotonTight_Calib_newvert.jet_UseJetMomentForScale    = True
        MET_PhotonTight_Calib_newvert.jet_JetMomentForScale       = "LCJES"
        MET_PhotonTight_Calib_newvert.jet_ApplyJetJVF             = "Yes"
        MET_PhotonTight_Calib_newvert.jet_RunSoftJetsTool         = False
        MET_PhotonTight_Calib_newvert.jet_calibType               = 'ExclRefCalib'
        MET_PhotonTight_Calib_newvert.ele_calibType               ='RefCalib'
        MET_PhotonTight_Calib_newvert.cellout_calibType           ='Eflow'
        MET_PhotonTight_Calib_newvert.tau_calibType               ='ExclRefCalib'
        MET_PhotonTight_Calib_newvert.cryo_ApplyCorrection        = "Off"
        MET_PhotonTight_Calib_newvert.muon_algorithm              = "Staco"
        MET_PhotonTight_Calib_newvert.muon_isolationAlg           = "dRJet"
        MET_PhotonTight_Calib_newvert()
        METcollections += [suffix]
    
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_Calib_OR_newvert'
        MET_PhotonTight_Calib_OR_newvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_Calib_OR_newvert.photon_doPhotonTool              = True
        MET_PhotonTight_Calib_OR_newvert.gamma_noCut                      = True
        MET_PhotonTight_Calib_OR_newvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsTight"
        MET_PhotonTight_Calib_OR_newvert.gamma_EleInputCollectionKey      = "SelectedORElectronsTight"
        MET_PhotonTight_Calib_OR_newvert.gamma_calibType                  = "RefCalib"
        MET_PhotonTight_Calib_OR_newvert.ele_noCut                        = True
        MET_PhotonTight_Calib_OR_newvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsTight"
        MET_PhotonTight_Calib_OR_newvert.ele_calibType                    = "RefCalib"
        MET_PhotonTight_Calib_OR_newvert.jet_vxcontainerkey               = "PAUDiphotonVertexCollection"
        MET_PhotonTight_Calib_OR_newvert.jet_UseCustomVxContainer         = True
        MET_PhotonTight_Calib_OR_newvert.cellout_vxcontainerkey           = "PAUDiphotonVertexCollection"    
        MET_PhotonTight_Calib_OR_newvert.jet_JetInputCollectionKey        ='AntiKt4LCTopoJets'
        MET_PhotonTight_Calib_OR_newvert.jet_JetPtCut                     = 20.0*GeV
        MET_PhotonTight_Calib_OR_newvert.jet_ApplyJetScale                = "Yes"
        MET_PhotonTight_Calib_OR_newvert.jet_UseJetMomentForScale         = True
        MET_PhotonTight_Calib_OR_newvert.jet_JetMomentForScale            = "LCJES"
        MET_PhotonTight_Calib_OR_newvert.jet_ApplyJetJVF                  = "Yes"
        MET_PhotonTight_Calib_OR_newvert.jet_RunSoftJetsTool              = False
        MET_PhotonTight_Calib_OR_newvert.jet_calibType                    = 'ExclRefCalib'
        MET_PhotonTight_Calib_OR_newvert.ele_calibType                    ='RefCalib'
        MET_PhotonTight_Calib_OR_newvert.cellout_calibType                ='Eflow'
        MET_PhotonTight_Calib_OR_newvert.tau_calibType                    ='ExclRefCalib'
        MET_PhotonTight_Calib_OR_newvert.cryo_ApplyCorrection             = "Off"
        MET_PhotonTight_Calib_OR_newvert.muon_algorithm                   = "Staco"
        MET_PhotonTight_Calib_OR_newvert.muon_isolationAlg                = "dRJet"
        MET_PhotonTight_Calib_OR_newvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_EmScale_OR_newvert'
        MET_PhotonTight_EmScale_OR_newvert = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_EmScale_OR_newvert.photon_doPhotonTool              = True
        MET_PhotonTight_EmScale_OR_newvert.gamma_noCut                      = True
        MET_PhotonTight_EmScale_OR_newvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsTight"
        MET_PhotonTight_EmScale_OR_newvert.gamma_EleInputCollectionKey      = "SelectedORElectronsTight"
        MET_PhotonTight_EmScale_OR_newvert.gamma_calibType                  = "EmScale"
        MET_PhotonTight_EmScale_OR_newvert.ele_noCut                        = True
        MET_PhotonTight_EmScale_OR_newvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsTight"
        MET_PhotonTight_EmScale_OR_newvert.ele_calibType                    = "RefCalib"
        MET_PhotonTight_EmScale_OR_newvert.jet_vxcontainerkey               = "PAUDiphotonVertexCollection"
        MET_PhotonTight_EmScale_OR_newvert.jet_UseCustomVxContainer         = True
        MET_PhotonTight_EmScale_OR_newvert.cellout_vxcontainerkey           = "PAUDiphotonVertexCollection"
        MET_PhotonTight_EmScale_OR_newvert.jet_JetInputCollectionKey        ='AntiKt4LCTopoJets'
        MET_PhotonTight_EmScale_OR_newvert.jet_JetPtCut                     = 20.0*GeV
        MET_PhotonTight_EmScale_OR_newvert.jet_ApplyJetScale                = "Yes"
        MET_PhotonTight_EmScale_OR_newvert.jet_UseJetMomentForScale         = True
        MET_PhotonTight_EmScale_OR_newvert.jet_JetMomentForScale            = "LCJES"
        MET_PhotonTight_EmScale_OR_newvert.jet_ApplyJetJVF                  = "Yes"
        MET_PhotonTight_EmScale_OR_newvert.jet_RunSoftJetsTool              = False
        MET_PhotonTight_EmScale_OR_newvert.jet_calibType                    = 'ExclRefCalib'
        MET_PhotonTight_EmScale_OR_newvert.ele_calibType                    ='RefCalib'
        MET_PhotonTight_EmScale_OR_newvert.cellout_calibType                ='Eflow'
        MET_PhotonTight_EmScale_OR_newvert.tau_calibType                    ='ExclRefCalib'
        MET_PhotonTight_EmScale_OR_newvert.cryo_ApplyCorrection             = "Off"
        MET_PhotonTight_EmScale_OR_newvert.muon_algorithm                   = "Staco"
        MET_PhotonTight_EmScale_OR_newvert.muon_isolationAlg                = "dRJet"
        MET_PhotonTight_EmScale_OR_newvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_PhotonTight_Calib'
        MET_PhotonTight_Calib = make_METRefAlg(_suffix=suffix)
        MET_PhotonTight_Calib.photon_doPhotonTool         = True
        MET_PhotonTight_Calib.gamma_EtCut                 = 20.0*GeV;
        MET_PhotonTight_Calib.gamma_isEM                  = egammaPID.PhotonIDTight
        MET_PhotonTight_Calib.gamma_calibType             = "RefCalib"
        MET_PhotonTight_Calib()
        METcollections += [suffix]
    
    # NN tight photons
    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_NNTight_EmScale_stdvert'
        MET_NNTight_EmScale_stdvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_EmScale_stdvert.photon_doPhotonTool              = True
        MET_NNTight_EmScale_stdvert.gamma_noCut                      = True
        MET_NNTight_EmScale_stdvert.gamma_EgammaInputCollectionKey   = "SelectedPhotonsNNTight"
        MET_NNTight_EmScale_stdvert.gamma_calibType                  = "EmScale"
        MET_NNTight_EmScale_stdvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_NNTight_EmScale_stdvert.jet_JetPtCut                = 20.0*GeV
        MET_NNTight_EmScale_stdvert.jet_ApplyJetScale           = "Yes"
        MET_NNTight_EmScale_stdvert.jet_UseJetMomentForScale    = True
        MET_NNTight_EmScale_stdvert.jet_JetMomentForScale       = "LCJES"
        MET_NNTight_EmScale_stdvert.jet_ApplyJetJVF             = "Yes"
        MET_NNTight_EmScale_stdvert.jet_RunSoftJetsTool         = False
        MET_NNTight_EmScale_stdvert.jet_calibType               = 'ExclRefCalib'
        MET_NNTight_EmScale_stdvert.cellout_calibType           ='Eflow'
        MET_NNTight_EmScale_stdvert.tau_calibType               ='ExclRefCalib'
        MET_NNTight_EmScale_stdvert.cryo_ApplyCorrection        = "Off"
        MET_NNTight_EmScale_stdvert.muon_algorithm              = "Staco"
        MET_NNTight_EmScale_stdvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_EmScale_stdvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_NNTight_EmScale_OR_stdvert'
        MET_NNTight_EmScale_OR_stdvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_EmScale_OR_stdvert.photon_doPhotonTool              = True
        MET_NNTight_EmScale_OR_stdvert.gamma_noCut                      = True
        MET_NNTight_EmScale_OR_stdvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsNNTight"
        MET_NNTight_EmScale_OR_stdvert.gamma_EleInputCollectionKey      = "SelectedORElectronsNNTight"
        MET_NNTight_EmScale_OR_stdvert.gamma_calibType                  = "EmScale"
        MET_NNTight_EmScale_OR_stdvert.ele_noCut                        = True
        MET_NNTight_EmScale_OR_stdvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsNNTight"
        MET_NNTight_EmScale_OR_stdvert.ele_calibType                    = "RefCalib"
        MET_NNTight_EmScale_OR_stdvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_NNTight_EmScale_OR_stdvert.jet_JetPtCut                = 20.0*GeV
        MET_NNTight_EmScale_OR_stdvert.jet_ApplyJetScale           = "Yes"
        MET_NNTight_EmScale_OR_stdvert.jet_UseJetMomentForScale    = True
        MET_NNTight_EmScale_OR_stdvert.jet_JetMomentForScale       = "LCJES"
        MET_NNTight_EmScale_OR_stdvert.jet_ApplyJetJVF             = "Yes"
        MET_NNTight_EmScale_OR_stdvert.jet_RunSoftJetsTool         = False
        MET_NNTight_EmScale_OR_stdvert.jet_calibType               = 'ExclRefCalib'
        MET_NNTight_EmScale_OR_stdvert.cellout_calibType           ='Eflow'
        MET_NNTight_EmScale_OR_stdvert.tau_calibType               ='ExclRefCalib'
        MET_NNTight_EmScale_OR_stdvert.cryo_ApplyCorrection        = "Off"
        MET_NNTight_EmScale_OR_stdvert.muon_algorithm              = "Staco"
        MET_NNTight_EmScale_OR_stdvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_EmScale_OR_stdvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_NNTight_EmScale_newvert'
        MET_NNTight_EmScale_newvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_EmScale_newvert.photon_doPhotonTool              = True
        MET_NNTight_EmScale_newvert.gamma_noCut                      = True
        MET_NNTight_EmScale_newvert.gamma_EgammaInputCollectionKey   = "SelectedPhotonsNNTight"
        MET_NNTight_EmScale_newvert.gamma_calibType                  = "EmScale"
        MET_NNTight_EmScale_newvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_NNTight_EmScale_newvert.jet_JetPtCut                = 20.0*GeV
        MET_NNTight_EmScale_newvert.jet_ApplyJetScale           = "Yes"
        MET_NNTight_EmScale_newvert.jet_UseJetMomentForScale    = True
        MET_NNTight_EmScale_newvert.jet_JetMomentForScale       = "LCJES"
        MET_NNTight_EmScale_newvert.jet_ApplyJetJVF             = "Yes"
        MET_NNTight_EmScale_newvert.jet_RunSoftJetsTool         = False
        MET_NNTight_EmScale_newvert.jet_calibType               = 'ExclRefCalib'
        MET_NNTight_EmScale_newvert.cellout_calibType           ='Eflow'
        MET_NNTight_EmScale_newvert.tau_calibType               ='ExclRefCalib'
        MET_NNTight_EmScale_newvert.cryo_ApplyCorrection        = "Off"
        MET_NNTight_EmScale_newvert.muon_algorithm              = "Staco"
        MET_NNTight_EmScale_newvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_EmScale_newvert.jet_UseCustomVxContainer    = True
        MET_NNTight_EmScale_newvert.jet_vxcontainerkey          = "PAUDiphotonVertexCollection"
        MET_NNTight_EmScale_newvert.cellout_vxcontainerkey      = "PAUDiphotonVertexCollection"    
        MET_NNTight_EmScale_newvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_NNTight_Calib'
        MET_NNTight_Calib = make_METRefAlg(_suffix=suffix)
        MET_NNTight_Calib.photon_doPhotonTool              = True
        MET_NNTight_Calib.gamma_noCut                      = True
        MET_NNTight_Calib.gamma_EgammaInputCollectionKey   = "SelectedPhotonsNNTight"
        MET_NNTight_Calib.gamma_calibType                  = "RefCalib"
        MET_NNTight_Calib()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix='_NNTight_Calib_newvert'
        MET_NNTight_Calib_newvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_Calib_newvert.photon_doPhotonTool              = True
        MET_NNTight_Calib_newvert.gamma_noCut                      = True
        MET_NNTight_Calib_newvert.gamma_EgammaInputCollectionKey   = "SelectedPhotonsNNTight"
        MET_NNTight_Calib_newvert.gamma_calibType                  = "RefCalib"
        MET_NNTight_Calib_newvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_NNTight_Calib_newvert.jet_JetPtCut                = 20.0*GeV
        MET_NNTight_Calib_newvert.jet_ApplyJetScale           = "Yes"
        MET_NNTight_Calib_newvert.jet_UseJetMomentForScale    = True
        MET_NNTight_Calib_newvert.jet_JetMomentForScale       = "LCJES"
        MET_NNTight_Calib_newvert.jet_ApplyJetJVF             = "Yes"
        MET_NNTight_Calib_newvert.jet_RunSoftJetsTool         = False
        MET_NNTight_Calib_newvert.jet_calibType               = 'ExclRefCalib'
        MET_NNTight_Calib_newvert.cellout_calibType           ='Eflow'
        MET_NNTight_Calib_newvert.tau_calibType               ='ExclRefCalib'
        MET_NNTight_Calib_newvert.cryo_ApplyCorrection        = "Off"
        MET_NNTight_Calib_newvert.muon_algorithm              = "Staco"
        MET_NNTight_Calib_newvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_Calib_newvert.jet_UseCustomVxContainer    = True
        MET_NNTight_Calib_newvert.jet_vxcontainerkey          = "PAUDiphotonVertexCollection"
        MET_NNTight_Calib_newvert.cellout_vxcontainerkey      = "PAUDiphotonVertexCollection"    
        MET_NNTight_Calib_newvert()
        METcollections += [suffix]

    if PhotonD3PDMakerFlags.AdditionalMETOutputLevel() > 3:
        suffix = '_NNTight_Calib_OR_newvert'
        MET_NNTight_Calib_OR_newvert = make_METRefAlg(_suffix=suffix)
        MET_NNTight_Calib_OR_newvert.photon_doPhotonTool              = True
        MET_NNTight_Calib_OR_newvert.gamma_noCut                      = True
        MET_NNTight_Calib_OR_newvert.gamma_EgammaInputCollectionKey   = "SelectedORPhotonsNNTight"
        MET_NNTight_Calib_OR_newvert.gamma_EleInputCollectionKey      = "SelectedORElectronsNNTight"
        MET_NNTight_Calib_OR_newvert.gamma_calibType                  = "RefCalib"
        MET_NNTight_Calib_OR_newvert.ele_noCut                        = True
        MET_NNTight_Calib_OR_newvert.ele_EgammaInputCollectionKey     = "SelectedORElectronsNNTight"
        MET_NNTight_Calib_OR_newvert.ele_calibType                    = "RefCalib"
        MET_NNTight_Calib_OR_newvert.jet_JetInputCollectionKey   ='AntiKt4LCTopoJets'
        MET_NNTight_Calib_OR_newvert.jet_JetPtCut                = 20.0*GeV
        MET_NNTight_Calib_OR_newvert.jet_ApplyJetScale           = "Yes"
        MET_NNTight_Calib_OR_newvert.jet_UseJetMomentForScale    = True
        MET_NNTight_Calib_OR_newvert.jet_JetMomentForScale       = "LCJES"
        MET_NNTight_Calib_OR_newvert.jet_ApplyJetJVF             = "Yes"
        MET_NNTight_Calib_OR_newvert.jet_RunSoftJetsTool         = False
        MET_NNTight_Calib_OR_newvert.jet_calibType               = 'ExclRefCalib'
        MET_NNTight_Calib_OR_newvert.cellout_calibType           ='Eflow'
        MET_NNTight_Calib_OR_newvert.tau_calibType               ='ExclRefCalib'
        MET_NNTight_Calib_OR_newvert.cryo_ApplyCorrection        = "Off"
        MET_NNTight_Calib_OR_newvert.muon_algorithm              = "Staco"
        MET_NNTight_Calib_OR_newvert.muon_isolationAlg           = "dRJet"
        MET_NNTight_Calib_OR_newvert.jet_UseCustomVxContainer    = True
        MET_NNTight_Calib_OR_newvert.jet_vxcontainerkey          = "PAUDiphotonVertexCollection"
        MET_NNTight_Calib_OR_newvert.cellout_vxcontainerkey      = "PAUDiphotonVertexCollection"    
        MET_NNTight_Calib_OR_newvert()
        METcollections += [suffix]


    return METcollections



