infile = 'aod/AOD-17.2.0.2/AOD-17.2.0.2-full.pool.root'
reffile = 'egamma-aod-d3pd.ref'
jo = 'D3PDMakerConfig/AODToEgammaD3PD.py'

from RecExConfig.RecFlags import rec
import RecExConfig.RecAlgsFlags


# Force AODFix off, to get repeatible results across releases.
from RecExConfig.RecFlags import rec
rec.doApplyAODFix.set_Value_and_Lock(False)

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-16-00-00'
##import AtlasGeoModel.GeoModelInit
#import AtlasGeoModel.SetGeometryVersion
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-00-00')
   
include ('D3PDMakerTest/run-d3pd-test.py')

svcMgr.GeoModelSvc.IgnoreTagDifference = True

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.GetStream('StreamNTUP_EGAMMA')

# Test adding another electron with a different container type.
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
#alg += ElectronD3PDObject (10,
#                           sgkey = 'HLT_egamma',
#                           typeName = 'egammaContainer',
#                           exclude = ['EFInfo', 'L2Info', 'L1Info',
#                                      'EFIndex', 'L2Index', 'L1Index'],
#                           prefix = 'ef_el_')

# Try doing some filtering...
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey          import resolveSGKey
from AthenaCommon.AlgSequence                 import AlgSequence
from D3PDMakerCoreComps.D3PDObject            import D3PDObject
import EventCommonD3PDMaker
import D3PDMakerCoreComps
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
from AnalysisUtils.AnalysisUtilsConf import IParticleSelector
#preseq += IParticleSelector \
#          ('elesel',
#           PtMin = 15*GeV,
#           InputKey = resolveSGKey ('ElectronContainer',
#                                    D3PDMakerFlags.ElectronSGKey()),
#           OutSelectedParticlesKey = 'elesel',
#           OutParticleLinksKey = 'elelinks')
#alg += ElectronD3PDObject (10,
#                           sgkey = D3PDMakerFlags.ElectronSGKey(),
#                           getterFilter = 'elesel',
#                           prefix = 'elfilt_')

#def make_Ele2D3PDObject (name, prefix, object_name):
#    getter = EventCommonD3PDMaker.IParticleLinksCollectionGetterTool \
#             (SGKey = 'elelinks')
#    return D3PDMakerCoreComps.VectorFillerTool (name,
#                                                Prefix = prefix,
#                                                Getter = getter)
#Ele2D3PDObject = D3PDObject (make_Ele2D3PDObject, 'elfilt2_')
#Ele2D3PDObject.defineBlock (0, 'Kinematics',
#                            EventCommonD3PDMaker.FourMomFillerTool)
#alg += Ele2D3PDObject (10)

# Test egtruth
from egammaD3PDMaker.egammaTruthD3PDObject import egammaTruthD3PDObject
alg += egammaTruthD3PDObject (10)

# Test CorrectionCluster
from egammaD3PDMaker.CorrectionClusterD3PDObject import CorrectionClusterD3PDObject
alg += CorrectionClusterD3PDObject(10, prefix='cocl_')

# Test filtering.
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
class MyFilter (PyAthena.Alg):
    def __init__ (self, name = 'myfilter', exclude=1, **kw):
        super (MyFilter, self).__init__ (name = name, **kw)
        self.count = 0
        self.exclude = exclude
        return
    def execute (self):
        self.setFilterPassed ( self.count != self.exclude )
        self.count += 1
        return StatusCode.Success

# Adding a filter the old way.
alg.filterSeq += MyFilter ('myfilter1', exclude=1)

# Adding a filter the new way.
topSequence += MyFilter('myfilter2', exclude=5)
alg.AddRequireAlgs (MyFilter('myfilter2').getName())


from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
#preseq += D2PDElectronSelector \
#          ('elesel2',
#           ptMin = 15*GeV,
#           inputCollection = resolveSGKey ('ElectronContainer',
#                                           D3PDMakerFlags.ElectronSGKey()),
#           outputCollection = 'elesel2',
#           outputLinkCollection = 'elelink2')
#alg += ElectronD3PDObject (0,
#                           sgkey = 'elelink2',
#                           #typeName = 'INav4MomLinkContainer',
#                           getterClass = 
#                           EventCommonD3PDMaker.INav4MomLinkContainerGetterTool,
#                           prefix = 'elfilt3_')
#alg += ElectronD3PDObject (0,
#                           prefix = 'elfilt4_',
#                           SelectionFlags_FlagNames = ['selected@INav4MomLinkContainer/elelink2:Test selection'],
#                           )


# Test composite particles...
#from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
#preseq += D2PDParticleCombiner\
#          ('emucombiner',
#           outputCollection = 'emuCombined',
#           inputCollection1 = 'ElectronAODCollection',
#           inputCollection2 = 'StacoMuonCollection')

# from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject
# from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
# import EventCommonD3PDMaker
# MyCompositeD3PDObject = \
#            make_SGDataVector_D3PDObject ('CompositeParticleContainer',
#                                          'emuCombined',
#                                          'emu_')
# MyCompositeD3PDObject.defineBlock (0, 'Kinematics',
#                                    EventCommonD3PDMaker.FourMomFillerTool)
# IndexMultiAssociation (MyCompositeD3PDObject,
#                        EventCommonD3PDMaker.CompositeParticleAssociationTool,
#                        ['el_', 'mu_'],
#                        containerIndexName = 'cont',
#                        nrowName = '')

# alg += MyCompositeD3PDObject (10)




# Test non-existent egamma.
alg += ElectronD3PDObject (0,
                           sgkey = 'xxx',
                           prefix = 'el_xxx_',
                           allowMissing = True)

from egammaD3PDMaker.PhotonD3PDObject       import PhotonD3PDObject
alg += PhotonD3PDObject (0,
                         sgkey = 'xxx',
                         prefix = 'ph_xxx_',
                         allowMissing = True)

from MuonD3PDMaker.MuonD3PDObject       import MuonD3PDObject
alg += MuonD3PDObject (0,
                       sgkey = 'xxx',
                       prefix = 'mu_xxx_',
                       allowMissing = True)

from JetD3PDMaker.JetD3PDObject       import JetD3PDObject
alg += JetD3PDObject (0,
                      sgkey = 'xxx',
                      prefix = 'jet_xxx_',
                      allowMissing = True)

from egammaD3PDMaker.GSFElectronD3PDObject       import GSFElectronD3PDObject
alg += GSFElectronD3PDObject (0,
                              sgkey = 'xxx',
                              prefix = 'el_gxx_',
                              allowMissing = True)

# Test fix in D3PDMakerCoreComps-00-01-79 that was leading to crosstalk
# between TriggerBitFiller objects from different trees.
from D3PDMakerCoreComps.MakerAlg import MakerAlg
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
trigtest_alg = MakerAlg ('trigtest', file = tupleFileOutput)
trigtest_alg += EventInfoD3PDObject (0)
trigtest_alg += ElectronD3PDObject (0,
                                    exclude=['D3PD__GenParticleTruthParticleAssociationTool'])

