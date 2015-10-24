# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import EventCommonD3PDMaker
import CaloD3PDMaker
import egammaD3PDMaker
import D3PDMakerCoreComps
import TrackD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker.xAODTrackSummaryFiller  import xAODTrackSummaryFiller


EFElectronD3PDObject = \
           make_SGDataVector_D3PDObject ('xAOD::ElectronContainer',
                                         'HLT_egamma_Electrons,HLT_egamma', 
                                         'trig_EF_el_', 'EFElectronD3PDObject')

EFElectronD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
EFElectronD3PDObject.defineBlock (
    0, 'Charge',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['charge'])
EFElectronD3PDObject.defineBlock (
    0, 'Author',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['author'])
EFElectronD3PDObject.defineBlock (
    0, 'Pass',
    D3PDMakerCoreComps.AuxDataFillerTool,
    # These are present only for central electrons.
    Vars = ['loose = Loose < int8_t: 0',
            'medium = Medium < int8_t: 0',
            'tight = Tight < int8_t : 0'
            ])


EFElectronD3PDObject.defineBlock (
    1, 'HadLeakage',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Ethad = ethad',
            'Ethad1 = ethad1'])
EFElectronD3PDObject.defineBlock (
    1, 'Layer1Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f1',
            'f1core',
            'Emins1 = emins1',
            'fside = fracs1',
            'Emax2 = e2tsts1',
            'ws3 = weta1',
            'wstot = wtots1',
            'emaxs1'])
EFElectronD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                                egammaD3PDMaker.egammaLayer1ExtraFillerTool)
EFElectronD3PDObject.defineBlock (
    1, 'Layer2Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E233 = e233',
            'E237 = e237',
            'E277 = e277',
            'weta2'])
EFElectronD3PDObject.defineBlock (
    1, 'Layer3Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f3', 'f3core'])
EFElectronD3PDObject.defineBlock (
    1, 'Iso',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rphiallcalo = r33over37allcalo',
            'Etcone20 = etcone20',
            'Etcone30 = etcone30',
            'Etcone40 = etcone40',
            'ptcone20 < float: 0',
            'ptcone30 < float: 0',
            'ptcone40 < float: 0',
        ])
EFElectronD3PDObject.defineBlock (
    1, 'TrkMatch',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['pos7',
            'deltaeta1 = deltaEta1',
            'deltaeta2 = deltaEta2',
            'deltaphi2 = deltaPhi2',
            ])
EFElectronD3PDObject.defineBlock (1, 'Retaphi',
                                egammaD3PDMaker.egammaRetaphiFillerTool)


EFElectronD3PDObject.defineBlock (
    2, 'Pointing',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['zvertex', 'errz', 'etap', 'depth'])

ElectronClusterAssoc = SimpleAssociation \
    (EFElectronD3PDObject,
     egammaD3PDMaker.egammaClusterAssociationTool)
ElectronClusterAssoc.defineBlock \
    (2, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
ElectronClusterAssoc.defineBlock \
    (1, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     prefix = 'cl_',
     WriteE = True,
     WriteM = False)
def _clustereta2_lod (lod, args):
    return lod==1        # Only include this block if exactly level 1.
ElectronClusterAssoc.defineBlock \
    (_clustereta2_lod, 'ClusterEta2', CaloD3PDMaker.ClusterEMSamplingFillerTool,
     prefix = 'cl_',
     WriteEnergy = False,
     WritePhi = False,
     Samplings = [2])



EFElectronD3PDObject.defineBlock \
       (1, 'Triggers',
        egammaD3PDMaker.xAODElectronObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.ElectronEFTrigPattern() ])


ElectronTPAssoc = SimpleAssociation \
    (EFElectronD3PDObject,
     egammaD3PDMaker.ElectronTrackParticleAssociationTool,
     matched = 'hastrack',
     blockname = 'TrkInfo')

TrackParticlePerigeeAssoc = SimpleAssociation \
    (ElectronTPAssoc,
     TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
     prefix = 'track')

TrackParticlePerigeeAssoc.defineBlock (1, 'Trk',
                                       TrackD3PDMaker.PerigeeFillerTool,
                                       FillMomentum = True)
TrackParticleCovarAssoc = SimpleAssociation (TrackParticlePerigeeAssoc,
                                             TrackD3PDMaker.PerigeeCovarianceAssociationTool)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     DiagCovariance = True)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovOffDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     OffDiagCovariance = True)


ElectronTPAssoc.defineBlock (
    2, 'TrkFitQuality',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['chi2 = chiSquared',
            'ndof = numberDoF'],
    prefix = 'trackfit')

xAODTrackSummaryFiller (ElectronTPAssoc, 1, 'IDHits',
                        IDSharedHits = True,
                        IDHits = True,
                        IDOutliers = True,
                        MuonHits = False,
                        HitSum = False,
                        HoleSum = False)
ElectronTPAssoc.defineBlock (
    1, 'TrackSummaryPID',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['eProbabilityComb',
            'eProbabilityHT',
            'eProbabilityToT < float:0',
            'eProbabilityBrem < float:0'])

ElectronVertAssoc = SimpleAssociation \
                    (ElectronTPAssoc,
                     TrackD3PDMaker.TrackParticleVertexAssociationTool,
                     Fast = False,
                     prefix = 'vert')
ElectronVertAssoc.defineBlock (
    2, 'Vertex',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['x', 'y', 'z'])
