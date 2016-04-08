# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


class HSG2(JobPropertyContainer):
          """ HSG2 related flags """
          
jobproperties.add_Container(HSG2)


class eQual(JobProperty):
    """ Specify electron quality
    """ 
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','Loose','RobustLoose','Medium','Tight']
    StoredValue='any'

jobproperties.HSG2.add_JobProperty(eQual)

class diLeptonEQual(JobProperty):
    """ Specify electron quality
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['any','any']

jobproperties.HSG2.add_JobProperty(diLeptonEQual)

class diLeptonMixQual(JobProperty):
    """ Specify lepton quality
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['any','inMS']

jobproperties.HSG2.add_JobProperty(diLeptonMixQual)


class muQual(JobProperty):
    """ Specify muon quality
    """ 
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','combined','standalone','lowPt','combined+lowPt','inMS']
    StoredValue='inMS'

jobproperties.HSG2.add_JobProperty(muQual)

class diLeptonMuQual(JobProperty):
    """ Specify muon quality
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['inMS','inMS']

jobproperties.HSG2.add_JobProperty(diLeptonMuQual)

class singleLeptonPtCut(JobProperty):
    """ Specify lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=7*Units.GeV

jobproperties.HSG2.add_JobProperty(singleLeptonPtCut)


class diLeptonPtCut(JobProperty):
    """ Specify di lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=10*Units.GeV

jobproperties.HSG2.add_JobProperty(diLeptonPtCut)



class diLeptonEPtCuts(JobProperty):
    """ Specify di lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[10*Units.GeV,10*Units.GeV]

jobproperties.HSG2.add_JobProperty(diLeptonEPtCuts)

class diLeptonMuPtCuts(JobProperty):
    """ Specify di lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[10*Units.GeV,10*Units.GeV]

jobproperties.HSG2.add_JobProperty(diLeptonMuPtCuts)

class diLeptonMixPtCuts(JobProperty):
    """ Specify di lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[10*Units.GeV,10*Units.GeV]

jobproperties.HSG2.add_JobProperty(diLeptonMixPtCuts)

class fourLeptonElectronEtCut(JobProperty):
    """ Specify 4 lepton electron et cut
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=6*Units.GeV

jobproperties.HSG2.add_JobProperty(fourLeptonElectronEtCut)

class fourLeptonMuonPtCut(JobProperty):
    """ Specify 4 lepton muon pt cut
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=4*Units.GeV

jobproperties.HSG2.add_JobProperty(fourLeptonMuonPtCut)

class fourLeptonMassCut(JobProperty):
    """ Specify 4 lepton mass cut
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=10*Units.GeV

jobproperties.HSG2.add_JobProperty(fourLeptonMassCut)

class diLeptonMassCut(JobProperty):
    """ Specify di lepton pt cut
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=0*Units.GeV

jobproperties.HSG2.add_JobProperty(diLeptonMassCut)

class diLeptonAcceptFrwdE(JobProperty):
    """ Accepts frwd electron in dilepton skims
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

jobproperties.HSG2.add_JobProperty(diLeptonAcceptFrwdE)

class fourLeptonAcceptFrwdE(JobProperty):
    """ Accepts frwd electron in four-lepton skims
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

jobproperties.HSG2.add_JobProperty(fourLeptonAcceptFrwdE)

class DoEBremmFix(JobProperty):
    """ runs e Bremm fix
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

jobproperties.HSG2.add_JobProperty(DoEBremmFix)

class PhotonQual(JobProperty):
    """ quality of photons
    """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='any'

jobproperties.HSG2.add_JobProperty(PhotonQual)

class PhotonEt(JobProperty):
    """ Et of photons
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=7000.

jobproperties.HSG2.add_JobProperty(PhotonEt)

class fourLeptonKinematicsPtCuts(JobProperty):
    """ Additional pT cuts for four leptons
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0*Units.GeV,0*Units.GeV,0*Units.GeV,0*Units.GeV] 

jobproperties.HSG2.add_JobProperty(fourLeptonKinematicsPtCuts)
    
class fourLeptonKinematicsMassCuts(JobProperty):
    """ Additional di-lepton cuts for four leptons
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0*Units.GeV,0*Units.GeV] 

jobproperties.HSG2.add_JobProperty(fourLeptonKinematicsMassCuts)

class fourLeptonKinematicsDeltaRCut(JobProperty):
    """ Additional delta R cut for four leptons
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.0

jobproperties.HSG2.add_JobProperty(fourLeptonKinematicsDeltaRCut)

class diLeptonCheckLArError(JobProperty):
    """ Check LAr Error in dilepton skims
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

jobproperties.HSG2.add_JobProperty(diLeptonCheckLArError)

class fourLeptonCheckLArError(JobProperty):
    """ Check LAr Error in four-lepton skims
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

jobproperties.HSG2.add_JobProperty(fourLeptonCheckLArError)

class llqqElectronEtCut(JobProperty):
    """ Specify electron ET cut for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=6*Units.GeV

jobproperties.HSG2.add_JobProperty(llqqElectronEtCut)

class llqqMuonPtCut(JobProperty):
    """ Specify muon pT cut for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=6*Units.GeV

jobproperties.HSG2.add_JobProperty(llqqMuonPtCut)

class llqqJetPtCut(JobProperty):
    """ Specify jet pT cut for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=15*Units.GeV

jobproperties.HSG2.add_JobProperty(llqqJetPtCut)

class llqqElectronQual(JobProperty):
    """ Specify electron quality for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','MediumPP','LoosePP']
    StoredValue='LoosePP'

jobproperties.HSG2.add_JobProperty(llqqElectronQual)

class llqqMuonQual(JobProperty):
    """ Specify muon quality for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','combined+lowpt','combined+lowpt+standalone']
    StoredValue='combined+lowpt+standalone'

jobproperties.HSG2.add_JobProperty(llqqMuonQual)

class llqqCaloMuonQual(JobProperty):
    """ Specify calo muon quality for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','calo']
    StoredValue='calo'

jobproperties.HSG2.add_JobProperty(llqqCaloMuonQual)

class llqqJetQual(JobProperty):
    """ Specify jet quality for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['any','barrel']
    StoredValue='barrel'

jobproperties.HSG2.add_JobProperty(llqqJetQual)

class llqqElectronCollection(JobProperty):
    """ Specify electron collection for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='ElectronAODCollection'

jobproperties.HSG2.add_JobProperty(llqqElectronCollection)

class llqqStacoMuonCollection(JobProperty):
    """ Specify Staco muon collection for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='StacoMuonCollection'

jobproperties.HSG2.add_JobProperty(llqqStacoMuonCollection)

class llqqMuonsCollection(JobProperty):
    """ Specify Muons collection for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='Muons'

jobproperties.HSG2.add_JobProperty(llqqMuonsCollection)

class llqqCaloMuonCollection(JobProperty):
    """ Specify Calo muon collection for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='CaloMuonCollection'

jobproperties.HSG2.add_JobProperty(llqqCaloMuonCollection)

class llqqJetCollection(JobProperty):
    """ Specify jet collection for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='AntiKt4TopoEMJets'

jobproperties.HSG2.add_JobProperty(llqqJetCollection)

class llqqDiLeptonMassCut(JobProperty):
    """ Specify di-lepton mass cut for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=5*Units.GeV

jobproperties.HSG2.add_JobProperty(llqqDiLeptonMassCut)

class llqqElectronJetDRCut(JobProperty):
    """ Specify Electron-jet dR cut for 2L2Q filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.05

jobproperties.HSG2.add_JobProperty(llqqElectronJetDRCut)

class doMV3BTagInD3PD(JobProperty):
    """ Perform MV3 b-tagging when creating D3PD 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

jobproperties.HSG2.add_JobProperty(doMV3BTagInD3PD)

class tagAndProbeEQual(JobProperty):
    """ Specify electron quality for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['any','any']

jobproperties.HSG2.add_JobProperty(tagAndProbeEQual)

class tagAndProbeMuQual(JobProperty):
    """ Specify muon quality for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['any','any']

jobproperties.HSG2.add_JobProperty(tagAndProbeMuQual)

class tagAndProbeMixQual(JobProperty):
    """ Specify lepton quality for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['any','any']

jobproperties.HSG2.add_JobProperty(tagAndProbeMixQual)

class tagAndProbeEPtCuts(JobProperty):
    """ Specify di-electron Et cut for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[15*Units.GeV,5*Units.GeV]

jobproperties.HSG2.add_JobProperty(tagAndProbeEPtCuts)

class tagAndProbeMuPtCuts(JobProperty):
    """ Specify di-muon pt cut for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[15*Units.GeV,4*Units.GeV]

jobproperties.HSG2.add_JobProperty(tagAndProbeMuPtCuts)

class tagAndProbeMixPtCuts(JobProperty):
    """ Specify di-lepton Et/pt cut for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[15*Units.GeV,4*Units.GeV]

jobproperties.HSG2.add_JobProperty(tagAndProbeMixPtCuts)

class tagAndProbeMassCut(JobProperty):
    """ Specify di-lepton mass cut for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=50*Units.GeV

jobproperties.HSG2.add_JobProperty(tagAndProbeMassCut)

class tagAndProbeAcceptFrwdE(JobProperty):
    """ Accepts frwd electron in the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

jobproperties.HSG2.add_JobProperty(tagAndProbeAcceptFrwdE)

class tagAndProbeCheckLArError(JobProperty):
    """ Check LAr Error in the Tag and Probe filter
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
jobproperties.HSG2.add_JobProperty(tagAndProbeCheckLArError)

class tagAndProbeElectronTriggers(JobProperty):
    """ Specify electron triggers for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['EF_e24vhi_medium1','EF_e60_medium1']

jobproperties.HSG2.add_JobProperty(tagAndProbeElectronTriggers)

class tagAndProbeMuonTriggers(JobProperty):
    """ Specify muon triggers for the Tag and Probe filter
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=['EF_mu24i_tight','EF_mu36_tight']

jobproperties.HSG2.add_JobProperty(tagAndProbeMuonTriggers)

