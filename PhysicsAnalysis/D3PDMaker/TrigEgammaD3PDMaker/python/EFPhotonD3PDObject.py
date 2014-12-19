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
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags


EFPhotonD3PDObject = \
           make_SGDataVector_D3PDObject ('xAOD::PhotonContainer',
                                         'HLT_egamma_Photons,HLT_egamma', 
                                         'trig_EF_ph_', 'EFPhotonD3PDObject')

EFPhotonD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,
                                WriteEt = True,
                                WriteRect = True)
EFPhotonD3PDObject.defineBlock \
       (1, 'Triggers',
        egammaD3PDMaker.xAODPhotonObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.PhotonEFTrigPattern() ])


# Same structure as offline photon
EFPhotonD3PDObject.defineBlock (
    0, 'Author',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['author'])
EFPhotonD3PDObject.defineBlock (
    0, 'Pass',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['loose = Loose',
            'tight = Tight'
            ])
EFPhotonD3PDObject.defineBlock (0, 'Conversion0',
                              egammaD3PDMaker.egammaConversion0FillerTool,
                              )

EFPhotonD3PDObject.defineBlock (
    1, 'HadLeakage',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['Ethad = ethad',
            'Ethad1 = ethad1'])
EFPhotonD3PDObject.defineBlock (
    1, 'Layer0Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E033 = e033'])
EFPhotonD3PDObject.defineBlock (
    1, 'Layer1Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f1',
            'f1core',
            'Emins1 = emins1',
            'fside = fracs1',
            'Emax2 = e2tsts1',
            'ws3 = weta1',
            'wstot = wtots1',
            'E132 = e132',
            'E1152 = e1152',
            'emaxs1'])
EFPhotonD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                              egammaD3PDMaker.egammaLayer1ExtraFillerTool)
EFPhotonD3PDObject.defineBlock (
    1, 'Layer2Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E233 = e233',
            'E237 = e237',
            'E277 = e277',
            'weta2'])
EFPhotonD3PDObject.defineBlock (
    1, 'Layer3Shape',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['f3', 'f3core'])
EFPhotonD3PDObject.defineBlock (
    1, 'Iso',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rphiallcalo = r33over37allcalo',
            'Etcone20 = etcone20',
            'Etcone30 = etcone30',
            'Etcone40 = etcone40',
            'ptcone20',
            'ptcone30',
            'ptcone40',
        ])
EFPhotonD3PDObject.defineBlock (1, 'Conversion',
                              egammaD3PDMaker.egammaConversionFillerTool)
EFPhotonD3PDObject.defineBlock (1, 'Retaphi',
                              egammaD3PDMaker.egammaRetaphiFillerTool)
                                           


EFPhotonD3PDObject.defineBlock (
    2, 'Pointing',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['zvertex', 'errz', 'etap', 'depth'])

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
