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


def PhotonCustomMET():

    print "calculating PhotonCustomMET...."
    
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
    # Filters for NN!!!
    # add a filter to select NN tight photons
    photonFilterNNTight = PhotonD3PDMaker.PhotonFilter(OutputCollectionName="SelectedPhotonsNNTight",
                                                       photonGetter = photongetter,
                                                       ptCut = 20000,
                                                       etaCut = 2.5,
                                                       IsEM = 0,
                                                       NNCut = True,
                                                       PhotonNNTool = PhotonNeuralNetworkTool())
    topSequence += photonFilterNNTight

    # add a filter to select normal electrons
    electrongetter = D3PDMakerCoreComps.SGDataVectorGetterTool('electronNNTight_Getter',
                                                               TypeName = 'ElectronContainer',
                                                               SGKey = D3PDMakerFlags.ElectronSGKey())
    ToolSvc += electrongetter

    # add a filter that will create both photon and electron containers, with proper overlap removal (OR)
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

    # add a filter that will create both photon and electron containers, with proper overlap removal (OR)
    # but using standard Tight
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


    # Toggle for setting some options that we used to enable for various STVF configurations,
    # before the MET configuration changes in 17.2.7.4.1.

    METcollections=[]

    # Loose photons
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
    MET_PhotonLoose_Calib_OR_stdvert.jet_JetInputCollectionKey        = "AntiKt4LCTopoNewJets"

    MET_PhotonLoose_Calib_OR_stdvert.cellout_PUSuppression_JA         = False
    MET_PhotonLoose_Calib_OR_stdvert()
    METcollections += [suffix]

    # tight photons
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
    MET_PhotonTight_Calib_OR_stdvert.jet_JetInputCollectionKey        = "AntiKt4LCTopoNewJets"
    MET_PhotonTight_Calib_OR_stdvert.cellout_PUSuppression_JA         = False
    MET_PhotonTight_Calib_OR_stdvert()
    METcollections += [suffix]

    # NNTight photons
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
    MET_NNTight_Calib_OR_stdvert.jet_JetInputCollectionKey        = "AntiKt4LCTopoNewJets"
    MET_NNTight_Calib_OR_stdvertcellout_PUSuppression_JA          = False
    MET_NNTight_Calib_OR_stdvert()
    METcollections += [suffix]


    #return METcollections

    

    

    

