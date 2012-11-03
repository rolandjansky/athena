# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TrigEgammaD3PDMaker/python/EFPhotonD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Apr, 2010
# @brief EF photon D3PD object.
#
# For now, only basic kinematics and the list of trigger decisions.
#

import egammaD3PDMaker
import EventCommonD3PDMaker
import CaloD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
from egammaD3PDMaker.PhotonD3PDObject import PhotonD3PDObject

from D3PDMakerCoreComps.D3PDObject           import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation    import SimpleAssociation
from RecExConfig.RecFlags                    import rec
from egammaD3PDMaker.isem_version            import isem_version

import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaParameters
from ROOT import egammaPID


EFPhotonD3PDObject = \
           make_SGDataVector_D3PDObject ('egammaContainer',
                                         'HLT_egamma_Photons,HLT_egamma', 
                                         'trig_EF_ph_', 'EFPhotonD3PDObject')

EFPhotonD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
EFPhotonD3PDObject.defineBlock \
       (1, 'Triggers',
        egammaD3PDMaker.egammaObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.PhotonEFTrigPattern() ])


# Same structure as offline photon
EFPhotonD3PDObject.defineBlock (0, 'Author',
                              egammaD3PDMaker.egammaAuthorFillerTool,
                              RecoveredFlag = True)
EFPhotonD3PDObject.defineBlock (0, 'IsEM',
                              egammaD3PDMaker.egammaIsEMoneFillerTool
                              )
EFPhotonD3PDObject.defineBlock (0, 'Conversion0',
                              egammaD3PDMaker.egammaConversion0FillerTool,
                              )

## if rec.doTruth():
##     from TruthD3PDMaker.MCTruthClassifierConfig \
##          import D3PDMCTruthClassifier
##     EFPhotonD3PDObject.defineBlock (1, 'TruthClassification',
##                                   egammaD3PDMaker.egammaTruthClassificationFillerTool,
##                                   Classifier = D3PDMCTruthClassifier)
##     PhotonGenPartAssoc = SimpleAssociation \
##         (EFPhotonD3PDObject,
##          egammaD3PDMaker.egammaGenParticleAssociationTool,
##          prefix = 'truth_',
##          matched = 'matched',
##          blockname = 'TruthAssoc',
##          DRVar = 'deltaRRecPhoton',
##          Classifier = D3PDMCTruthClassifier)
##     PhotonGenPartAssoc.defineBlock (0, 'Truth',
##                                     EventCommonD3PDMaker.GenParticleFillerTool,
##                                     WriteE = True,
##                                     WriteM = False)


if isem_version() == 1:
    EFPhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                  egammaD3PDMaker.egammaIsEMFillerTool,
                                  IsEM  = [egammaPID.PhotonTightOLDRel,
                                           'tight',
                                           ])
else:
    EFPhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                  egammaD3PDMaker.egammaIsEMFillerTool,
                                  IsEM  =
        [egammaPID.PhotonLoose,         'loose',
         egammaPID.PhotonLooseIso,      'looseIso',
         egammaPID.PhotonTight,         'tight',
         egammaPID.PhotonTightIso,      'tightIso',

         egammaPID.PhotonLooseAR,       'looseAR',
         egammaPID.PhotonLooseARIso,    'looseARIso',
         egammaPID.PhotonTightAR,       'tightAR',
         egammaPID.PhotonTightARIso,    'tightARIso',
                                           ])


EFPhotonD3PDObject.defineBlock (1, 'HadLeakage',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.ethad,
                                           'Ethad',
                                           egammaParameters.ethad1,
                                           'Ethad1',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'Layer0Shape',
                              egammaD3PDMaker.egammaDetailFillerTool,
                              Details = [egammaParameters.e033,
                                         'E033'
                                         ])
EFPhotonD3PDObject.defineBlock (1, 'Layer1Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f1,
                                           'f1',
                                           egammaParameters.f1core,
                                           'f1core',
                                           egammaParameters.emins1,
                                           'Emins1',
                                           egammaParameters.fracs1,
                                           'fside',
                                           egammaParameters.e2tsts1,
                                           'Emax2',
                                           egammaParameters.weta1,
                                           'ws3',
                                           egammaParameters.wtots1,
                                           'wstot',
                                           egammaParameters.e132,
                                           'E132',
                                           egammaParameters.e1152,
                                           'E1152',
                                           egammaParameters.emaxs1,
                                           'emaxs1',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                              egammaD3PDMaker.egammaLayer1ExtraFillerTool)
EFPhotonD3PDObject.defineBlock (1, 'Layer2Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.e233,
                                           'E233',
                                           egammaParameters.e237,
                                           'E237',
                                           egammaParameters.e277,
                                           'E277',
                                            egammaParameters.weta2,
                                           'weta2',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'Layer3Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f3,
                                           'f3',
                                           egammaParameters.f3core,
                                           'f3core',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'Iso',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.r33over37allcalo,
                                           'rphiallcalo',
                                           egammaParameters.etcone,
                                           'Etcone45',
                                           egammaParameters.etcone15,
                                           'Etcone15',
                                           egammaParameters.etcone20,
                                           'Etcone20',
                                           egammaParameters.etcone25,
                                           'Etcone25',
                                           egammaParameters.etcone30,
                                           'Etcone30',
                                           egammaParameters.etcone35,
                                           'Etcone35',
                                           egammaParameters.etcone40,
                                           'Etcone40',
                                           egammaParameters.ptcone20,
                                           'ptcone20',
                                           egammaParameters.ptcone30,
                                           'ptcone30',
                                           egammaParameters.ptcone40,
                                           'ptcone40',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'ConvFlags',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.convAngleMatch,
                                           'convanglematch',
                                           egammaParameters.convTrackMatch,
                                           'convtrackmatch',
                                           ])
EFPhotonD3PDObject.defineBlock (1, 'Conversion',
                              egammaD3PDMaker.egammaConversionFillerTool)
EFPhotonD3PDObject.defineBlock (1, 'Retaphi',
                              egammaD3PDMaker.egammaRetaphiFillerTool)
                                           


EFPhotonD3PDObject.defineBlock (2, 'Rings',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.etringnoisedR03Sig2,
               'EtringnoisedR03sig2',
               egammaParameters.etringnoisedR03Sig3,
               'EtringnoisedR03sig3',
               egammaParameters.etringnoisedR03Sig4,
               'EtringnoisedR03sig4',
               ])
EFPhotonD3PDObject.defineBlock (2, 'PhotDiscrim',
                              egammaD3PDMaker.egammaPIDFillerTool,
    PID  = [egammaPID.IsolationLikelihood_jets,
            'isolationlikelihoodjets',
            egammaPID.IsolationLikelihood_HQDelectrons,
            'isolationlikelihoodhqelectrons',
            egammaPID.PhotonWeight,
            'loglikelihood',
            egammaPID.PhotonWeight,
            'photonweight',
            egammaPID.BgPhotonWeight,
            'photonbgweight',
            egammaPID.NeuralNet,
            'neuralnet',
            egammaPID.Hmatrix,
            'Hmatrix',
            egammaPID.Hmatrix5,
            'Hmatrix5',
            egammaPID.AdaBoost,
            'adaboost',
            ])
EFPhotonD3PDObject.defineBlock (2, 'Pointing',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.zvertex,
                                           'zvertex',
                                           egammaParameters.errz,
                                           'errz',
                                           egammaParameters.etap,
                                           'etap',
                                           egammaParameters.depth,
                                           'depth'
                                           ])

PhotonClusterAssoc = SimpleAssociation \
    (EFPhotonD3PDObject,
     egammaD3PDMaker.egammaClusterAssociationTool)
PhotonClusterAssoc.defineBlock \
    (2, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
PhotonClusterAssoc.defineBlock \
    (1, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     prefix = 'cl_',
     WriteE = True,
     WriteM = False)
def _clustereta2_lod (lod, args):
    return lod==1        # Only include this block if exactly level 1.
PhotonClusterAssoc.defineBlock \
    (_clustereta2_lod, 'ClusterEta2', CaloD3PDMaker.ClusterEMSamplingFillerTool,
     prefix = 'cl_',
     WriteEnergy = False,
     WritePhi = False,
     Samplings = [2])
