from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from AthenaServices.Configurables import ThinningSvc, createThinningSvc

from RecExConfig.InputFilePeeker import inputFileSummary
inputObjects = inputFileSummary['eventdata_items']
def __addInput( stream, skip=[] ):
    for element in inputObjects:
        type_key = "%s#%s" % element
        if type_key not in skip:
            stream.AddItem( type_key )


elStream = MSMgr.NewPoolStream( 'elXAODStream', 'elXAOD.pool.root' )
svcMgr += createThinningSvc(svcName='elThinningSvc', outStreams=[elStream.Stream])
__addInput(elStream)


phStream = MSMgr.NewPoolRootStream( 'phXAODStream', 'phXAOD.pool.root' )
svcMgr += createThinningSvc(svcName='phThinningSvc', outStreams=[phStream.Stream])

tauStream = MSMgr.NewPoolRootStream( 'tauXAODStream', 'tauXAOD.pool.root' )
svcMgr += createThinningSvc(svcName='tauThinningSvc', outStreams=[tauStream.Stream])


from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool, TrigNavigationThinningTool, HLT__StreamTrigNavSlimming
from TrigNavTools.TrigNavToolsConfig import navigationSlimming
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

dk.ThinningTools +=  [navigationSlimming({'name':'el', 'features':tokeep['electron'], 'chains':'HLT_e1.*', 'mode':'normal', 'ThinningSvc':svcMgr.elThinningSvc})]

dk.ThinningTools +=  [navigationSlimming({'name':'ph', 'features':tokeep['photon'], 'chains':'HLT_g.*|HLT_.g.*', 'mode':'normal', 'ThinningSvc':svcMgr.phThinningSvc})]

dk.ThinningTools +=  [navigationSlimming({'name':'tau', 'features':tokeep['tau'], 'chains':'HLT_tau.*|HLT_.tau.*', 'mode':'normal', 'ThinningSvc':svcMgr.tauThinningSvc})]


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += dk



