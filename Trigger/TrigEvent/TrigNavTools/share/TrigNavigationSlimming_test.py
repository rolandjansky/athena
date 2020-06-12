from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PyUtils.MetaReaderPeeker import convert_itemList
inputObjects = convert_itemList(layout=None)

def __addInput( stream, skip=[] ):
    for element in inputObjects:
        type_key = "%s#%s" % element
        if type_key not in skip:
            stream.AddItem( type_key )


elStream = MSMgr.NewPoolStream( 'elXAODStream', 'elXAOD.pool.root' )
__addInput(elStream)


phStream = MSMgr.NewPoolRootStream( 'phXAODStream', 'phXAOD.pool.root' )

tauStream = MSMgr.NewPoolRootStream( 'tauXAODStream', 'tauXAOD.pool.root' )


from TrigNavTools.TrigNavToolsConfig import navigationThinningSvc
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
dk = DerivationFramework__DerivationKernel()

tokeep = {'electron': ['HLT_TrigElectronContainer_L2ElectronFex', 
                       'HLT_TrigElectronContainer_L2IDCaloFex', 
                       'HLT_xAOD__TrigElectronContainer_L2ElectronFex', 
                       'HLT_xAOD__TrigElectronContainer_L2IDCaloFex'],
          'photon':[ 'HLT_xAOD__TrigPhotonContainer_L2PhotonFex', 
                     'HLT_xAOD__PhotonContainer_egamma_Photons', 
                     'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Photon_EFID'],
          'tau':['HLT_xAOD__JetContainer_TrigTauJet', 
                 'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF']
          }

def _addThinning (stream, svc):
    for t in stream.GetEventStream().HelperTools:
        if t.getType() == 'Athena::ThinningCacheTool':
            t.TrigNavigationThinningSvc = svc
            break
    return


_addThinning (elStream,
              navigationThinningSvc ({'name':'el', 'features':tokeep['electron'], 'chains':'HLT_e1.*', 'mode':'normal'}))

_addThinning (phStream,
              navigationThinningSvc ({'name':'ph', 'features':tokeep['photon'], 'chains':'HLT_g.*|HLT_.g.*', 'mode':'normal'}))

_addThinning (tauStream,
              navigationThinningSvc ({'name':'tau', 'features':tokeep['tau'], 'chains':'HLT_tau.*|HLT_.tau.*', 'mode':'normal'}))


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += dk



