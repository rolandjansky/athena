infile = 'esd/ESD-17.2.0.2.pool.root'
reffile = 'physics-esd-d3pd.ref'
jo = 'D3PDMakerConfig/ESDToPhysicsD3PD.py'

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.FilterCollCand = True

physicsD3PD_args = {
    'AK4TowerJet_JetVertexFraction_FillFullJVF' : True,
    'AK6TowerJet_JetVertexFraction_FromUD' : True,
    }


# Force AODFix off, to get repeatible results across releases.
from RecExConfig.RecFlags import rec
rec.doApplyAODFix.set_Value_and_Lock(False)

# Needed for calo d3pd.
block_override = [
    '/LAR/BadChannels/BadChannels',
    '/LAR/BadChannels/MissingFEBs',
    '/LAR/Identifier/FebRodAtlas',
    '/LAR/Identifier/OnOffIdAtlas',
    ]

include ('D3PDMakerTest/run-d3pd-test.py')


# Make a track d3pd tree too!
from TrackD3PDMaker.TrackD3PD import TrackD3PD
alg = TrackD3PD (tupleFileOutput,
                 Track_trackParametersAtGlobalPerigeeLevelOfDetails = 3,
                 Track_storeDiagonalCovarianceAsErrors = True,
                 PrimaryVertex_storeDiagonalCovarianceAsErrors = True)

# And a calorimeter tree!
from CaloD3PDMaker.CaloD3PD import CaloD3PD
alg = CaloD3PD (file = tupleFileOutput)

# Make supercells into a separate file, if available.
from LArL1Sim import LArL1SimConf
if hasattr (LArL1SimConf, 'LArSCSimpleMaker'):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += LArL1SimConf.LArSCSimpleMaker ('LArSCSimpleMaker',
                                                  CellContainer = 'AllCalo',
                                                  SCellContainer = 'SCells')
  
    from D3PDMakerCoreComps.MakerAlg              import MakerAlg
    from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
    from CaloD3PDMaker.CaloCellD3PDObject         import AllCaloCellD3PDObject
    from CaloD3PDMaker.CaloCellD3PDObject         import SelCaloCellD3PDObject
    scell_alg = MakerAlg ('scells', file = 'scells.root')
    scell_alg += EventInfoD3PDObject (0)
    scell_alg += AllCaloCellD3PDObject (2, sgkey = 'SCells', prefix = 'sc_',
                                        include = ['RawPosition'])

    d3pd_tests += [('scells.root', 'scells.ref')]


# Test LeadingPtAssociationTool.
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
TestJetD3PDObject = make_SGDataVector_D3PDObject ('JetCollection',
                                                  'AntiKt4TopoEMJets,' +
                                                  'AntiKt4TopoJets,' +
                                                  'AntiKt4H1TopoJets',
                                                  'testjet_', 'testjet')
TestJetD3PDObject.defineBlock (0, 'Kin', EventCommonD3PDMaker.FourMomFillerTool)
LeadingConstitAssoc = SimpleAssociation \
  (TestJetD3PDObject,
   EventCommonD3PDMaker.LeadingPtAssociationTool,
   level = 0,
   prefix = 'lconstit_',
   blockname = 'LeadingConstitAssoc',
   MultiAssocTool = EventCommonD3PDMaker.NavigableConstituentAssociationTool
     ('LeadingConstitMAssoc', TypeName = 'CaloCluster'))
LeadingConstitAssoc.defineBlock (0, 'LeadKin',
                                 EventCommonD3PDMaker.FourMomFillerTool)


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.GetStream('StreamNTUP_PHYSICS')
alg += TestJetD3PDObject(10)

