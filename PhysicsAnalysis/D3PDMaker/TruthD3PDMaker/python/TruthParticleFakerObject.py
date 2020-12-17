# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file TruthD3PDMaker/python/TruthParticleFakerObject.py
## @brief Truth D3PD object for single particles
## @author Zach Marshall <zach.marshall@cern.ch>
## @date Nov, 2010
##

import TruthD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from TruthD3PDAnalysis.AllTruthFilterTool import AllTruthFilterTool

from AthenaCommon.SystemOfUnits import GeV

def make_TruthParticleFaker_D3PDObject( default_prefix, default_sgkey,
                                 default_object_name = "",
                                 default_filter = AllTruthFilterTool(),
                                 default_label = None, **other_defaults ):

    def make_obj( name, prefix, object_name,
                  getter = None, sgkey = None, filter = default_filter,
                  label = default_label, **kw ):

        if sgkey is None: sgkey = default_sgkey
        if label is None: label = prefix
        if getter is None:
            getter = TruthD3PDMaker.GenParticleGetterTool (name + "_Getter",
                                                           Label = label,
                                                           SGKey = sgkey,
                                                           Selector = filter )

        defs = other_defaults.copy()
        defs.update( kw )

        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        return D3PDMakerCoreComps.VectorFillerTool( name,
                                                    Prefix = prefix,
                                                    Getter = getter,
                                                    ObjectName = object_name,
                                                    SaveMetadata = \
                                                    D3PDMakerFlags.SaveObjectMetadata(),
                                                    **defs )

    return D3PDObject( make_obj, default_prefix, default_object_name )

TruthParticleFakerD3PDObject = make_TruthParticleFaker_D3PDObject( 'tpf_' ,
                                                     D3PDMakerFlags.TruthParticlesSGKey(),
                                                     "TruthParticleFakerD3PDObject",
                                                     AllTruthFilterTool(),
                                                     'TruthParticleFaker_' )

TruthParticleFakerElD3PDObject = make_TruthParticleFaker_D3PDObject( 'tpfel_' ,
                                                     D3PDMakerFlags.TruthParticlesSGKey(),
                                                     "TruthParticleFakerElD3PDObject",
                                                     AllTruthFilterTool(),
                                                     'tpfel_' )

TruthParticleFakerElD3PDObject.defineBlock( 0, 'TruthParticleFaker',
                                         TruthD3PDMaker.TruthParticleFakerTool , PDG_ID=11 , 
                                         WriteCharge=True , WritePn=True , WriteE=True , WriteEt=True )

TruthParticleFakerMuD3PDObject = make_TruthParticleFaker_D3PDObject( 'tpfmu_' ,
                                                     D3PDMakerFlags.TruthParticlesSGKey(),
                                                     "TruthParticleFakerMuD3PDObject",
                                                     AllTruthFilterTool(),
                                                     'tpfmu_' )

TruthParticleFakerMuD3PDObject.defineBlock( 0, 'TruthParticleFaker',
                                         TruthD3PDMaker.TruthParticleFakerTool , PDG_ID=13 , 
                                         WriteCharge=True , WritePn=True , WriteE=True , WriteEt=False )

TruthParticleFakerPhD3PDObject = make_TruthParticleFaker_D3PDObject( 'tpfph_' ,
                                                     D3PDMakerFlags.TruthParticlesSGKey(),
                                                     "TruthParticleFakerPhD3PDObject",
                                                     AllTruthFilterTool(),
                                                     'tpfph_' )

TruthParticleFakerPhD3PDObject.defineBlock( 0, 'TruthParticleFaker',
                                         TruthD3PDMaker.TruthParticleFakerTool , PDG_ID=22 , 
                                         WriteCharge=False  , WritePn=True , WriteE=True , WriteEt=True )

TruthParticleFakerTauD3PDObject = make_TruthParticleFaker_D3PDObject( 'tpftau_' ,
                                                     D3PDMakerFlags.TruthParticlesSGKey(),
                                                     "TruthParticleFakerTauD3PDObject",
                                                     AllTruthFilterTool(),
                                                     'tpftau_' )

TruthParticleFakerTauD3PDObject.defineBlock( 0, 'TruthParticleFaker',
                                         TruthD3PDMaker.TruthParticleFakerTool , PDG_ID=15 , 
                                         WriteCharge=True  , WritePn=False , WriteE=False , WriteEt=True )

# Create stupid little collections for each of these...
from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from McParticleAlgs.JobOptCfg                 import createMcAodBuilder
import TruthD3PDAnalysis
from AthenaCommon                             import CfgMgr
def simpleParticleConfig (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                         sgkey = 'SimpleTruthCollection',
                         prefix = '',
                         pdg_id = 11,
                         min_pt = -1.*GeV):

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('TruthParticleContainer', sgkey): return

    # Is the algorithm already registered?
    algname = prefix + sgkey + 'Builder'
    if hasattr (seq, algname): return

    filtname = prefix + sgkey + 'Filter'

    seq += createMcAodBuilder\
           (algname,
            inMcEvtCollection = D3PDMakerFlags.TruthSGKey(),
            outMcEvtCollection = sgkey + '_GEN_D3PD',
            outTruthParticles = sgkey,
            filterTool = TruthD3PDAnalysis.SimpleTruthParticleFilterTool
              (filtname,
               PDG_ID = pdg_id,
               MinPt  = min_pt),
            cnvTool = CfgMgr.TruthParticleCnvTool('D3PDTruthParticleCnvTool'),
            )

    cfgKeyStore.addTransient ('TruthParticleContainer', sgkey)
    return


from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
def simpleTruthParticleD3PDObject( sgkey_in , prefix = 'mc_' , skipDressing=False ):

    simpleTPDO = make_SGDataVector_D3PDObject \
      ('TruthParticleContainer',
       sgkey_in,
       prefix,
       'TruthParticleD3PDObject')

    simpleTPDO.defineBlock (0, 'TruthKin',
                                         EventCommonD3PDMaker.FourMomFillerTool)
    
    simpleTPDO.defineBlock (0, 'TruthInfo',
                                         TruthD3PDMaker.TruthParticleFillerTool)

    if not skipDressing:
        simpleTPDO.defineBlock(0, 'TruthNearby_Info',
                                             TruthD3PDMaker.TruthLeptonNearbyAssociationTool)

    return simpleTPDO

