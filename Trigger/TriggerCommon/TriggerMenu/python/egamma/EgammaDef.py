# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__  = 'Ryan White'
__version__=""
__doc__="Implementation of Electron/Photon Sequences"

"""
Common Electron/Photon configuration
Three classes provided to facilitate the configuration:
EgammaFexBuilder
EgammaHypoBuilder
EgammaSequence

EgammaSequence
All sequences stored in a map as data member of EgammaSequence
key values to use in ChainDef to set sequences with TEs:
        self.sequences['fastcalo'] 
        self.sequences['fastringer'] 
        self.sequences['fasttrack']
        self.sequences['fastrec'] 
        self.sequences['precisecalocalib'] 
        self.sequences['precisetrack']
        self.sequences['trackrec'] 
        self.sequences['preciserec'] 

Instance of EgammaSequence initiliazes chainDef class (L2EFChainDef_e/g ...)
setup methods access sequences via public attribute 'sequences' (see above) 

EgammaFexBuilder
Single instance of class, configures all possible FEX algos

EgammaHypoBuilder
Instance per chain to configure hypo algos from chainDict

Fex and Hypo Builder classes use a map 
with key value of sequence step (above)
and value that is method to configure the hypo algo
See _gensequence in both classes:
        self._gensequence = {'fastcalo':self._get_fastcalo,
                'fastrec':self._get_fastrec,
                'fastringer':self._get_fastringer,
                'fasttrack':self._get_fasttrack,
                'precisecalo':self._get_precisecalo,
                'precisecalocalib':self._get_precisecalocalib,
                'precisetrack':self._get_precisetrack,
                'trackrec':self._get_trackrec,
                'preciserec':self._get_preciserec
                }

"""

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.EgammaDef")

# Imports for HLT ID
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence

# Imports for HLT Calo
# Note that Egamma now configures slw algorithm (TrigEgammaRec)
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import (T2CaloEgamma_eGamma,
                                                     T2CaloEgamma_Ringer)

from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_eGamma,
                                           TrigCaloTowerMaker_eGamma)

from TrigEgammaRec.TrigEgammaToolFactories import TrigCaloClusterMaker_slw

# FEX for Egamma
from TrigEgammaHypo.TrigL2ElectronFexConfig import L2ElectronFex_1
from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1

from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec
from TrigEgammaHypo.TrigEFCaloCalibFexConfig import (TrigEFCaloCalibFex_Electron,TrigEFCaloCalibFex_Photon)


from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig import (TrigL2CaloRingerFexHypo_e_ID,
                                                         TrigL2CaloRingerFexHypo_e_NoCut,
                                                         TrigL2CaloRingerFexHypo_g_NoCut,
                                                         TrigL2CaloRingerFexHypo_e_EtCut,
                                                         TrigL2CaloRingerFexHypo_g_EtCut)

# EF Calo
from TrigHIHypo.UE import theTrigCaloCellMaker_eGamma as theTrigCaloCellMaker_eGammaHI

# Hypos for Egamma
from TrigEgammaHypo.TrigL2CaloHypoConfig import (L2CaloHypo_EtCut,
                                                 L2CaloHypo_NoCut,
                                                 L2CaloHypo_e_ID,
                                                 L2CaloHypo_g7, 
                                                 L2CaloHypo_g_nocut 
                                                 )

from TrigEgammaHypo.TrigL2PhotonHypoConfig import (L2PhotonHypo_g_EtCut, 
                                                   L2PhotonHypo_g_ID, 
                                                   L2PhotonHypo_g_NoCut)

from TrigEgammaHypo.TrigL2ElectronHypoConfig import (L2ElectronHypo_e_ID,
                                                     L2ElectronHypo_e_NoCut)

from TrigEgammaHypo.TrigEFCaloHypoConfig import (TrigEFCaloHypo_All,
                                                 TrigEFCaloHypo_EtCut,
                                                 TrigEFCaloHypo_e_ID,
                                                 TrigEFCaloHypo_g_ID
                                                 )

from TrigEgammaHypo.TrigEFTrackHypoConfig import (EFTrackHypo_e,
                                                  EFTrackHypo_e_NoCut)

from TrigEgammaHypo.TrigEFElectronHypoConfig import (TrigEFElectronHypo_e_ID,
                                                     TrigEFElectronHypo_e_ID_HI,
                                                     TrigEFElectronHypo_e_Iso,
                                                     TrigEFElectronHypo_e_LH,
                                                     TrigEFElectronHypo_e_LH_Iso,
                                                     TrigEFElectronHypo_e_NoCut,
                                                     TrigEFElectronHypo_e_WTP)

from TrigEgammaHypo.TrigEFPhotonHypoConfig import (EFPhotonHypo_g_EtCut, 
                                                   EFPhotonHypo_g_ID_CaloOnly, 
                                                   EFPhotonHypo_g_NoCut, 
                                                   EFPhotonHypo_g_ID_CaloOnly_Iso)

from TriggerMenu.egamma.EgammaCleanMonitoring import EgammaChainsToKeepMonitoring
from TriggerMenu.menu.CleanMonitoring import KeepMonitoring, DisableMonitoringButValAndTime

# Helper utilities
def print_attributes(obj):
    for attr in vars(obj):
        print(attr, getattr(obj,attr))

def update_map(seq):
    sequences = {}
    for key,items in seq.items():
        algos = filter(lambda item: item is not None, items)
        if algos:
            sequences[key] = algos
    return sequences        


# Class to hold all possible Fex configurables
# Can contain both sequences and single instances
class EgammaFexBuilder(object):
    """
    Summary:
    Class to configure all possible fex algos
    Requires chainDict to return map of algos for given chain

    Attributes:
    Algos are private
    get_sequences(chainDict) to return map of algos
    """
    # Define frequently used instances here as class variables
    inst_count = 0
    
    # Define sequence
    def __init__(self):
        '''
        Inits EgammaFexBuilder
        '''
        self._fast_track = []
        self._precise_track = []

        self._fast_calo_egamma          = T2CaloEgamma_eGamma()
        self._fast_calo_ringer          = T2CaloEgamma_Ringer()
        
        self._fast_electron                = L2ElectronFex_1()
        self._fast_photon                 = L2PhotonFex_1()
        self._egamma_rec         = TrigEgammaRec.copy(name = "TrigEgammaRec_eGamma",doPrint=False)()
        self._egamma_rec_conv    = TrigEgammaRec.copy(name = "TrigEgammaRec_Conv_eGamma", doConversions = True,doPrint=False)()
        self._egamma_rec_noid    = TrigEgammaRec.copy(name = "TrigEgammaRec_NoIDEF_eGamma",doTrackMatching = False,doTrackIsolation = False,doPrint=False)()
        self._egamma_rec_ph_caloiso    = TrigEgammaRec.copy(name = "TrigEgammaRec_CaloIso_photon",PhotonContainerName="egamma_Iso_Photons",
                                                     doTrackMatching = False,doTrackIsolation = False,
                                                     doCaloTopoIsolation=True,doPrint=False)()
        self._egamma_rec_el_caloiso    = TrigEgammaRec.copy(name = "TrigEgammaRec_CaloIso_electron",
                                                    ElectronContainerName="egamma_Iso_Electrons",
                                                    PhotonContainerName="egamma_Iso_Photons",
                                                    doCaloTopoIsolation=True,doPrint=False)()
                                                     
        self._cell_maker = TrigCaloCellMaker_eGamma()
        self._tower_maker    = TrigCaloTowerMaker_eGamma()
        self._tower_maker_ion    = TrigCaloTowerMaker_eGamma("TrigCaloTowerMaker_eGamma_heavyIon")
        self._cluster_maker  = TrigCaloClusterMaker_slw()
        self._electron_calib           = TrigEFCaloCalibFex_Electron()
        self._photon_calib           = TrigEFCaloCalibFex_Photon()

        ########### Sequences ###########
        [self._fast_track, self._precise_track] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()
        
        self._gensequence = {'fastcalo':self._get_fastcalo,
                             'fastcalorec':self._get_fastringer,
                             'fastcalohypo':self._get_fastcalohypo,
                             'fastringer':self._get_fastringer,
                             'fastringerhypo':self._get_fastringerhypo,
                             'fastrec':self._get_fastrec,
                             'fasttrack':self._get_fasttrack,
                             'precisecalo':self._get_precisecalo,
                             'precisecalocalib':self._get_precisecalocalib,
                             'precisetrack':self._get_precisetrack,
                             'trackrec':self._get_trackrec,
                             'preciserec':self._get_preciserec}
        
        
        EgammaFexBuilder.inst_count += 1
        if(EgammaFexBuilder.inst_count > 1):
            log.error('EgammaFexBuilder multiple instances %s' % EgammaFexBuilder.inst_count)

        log.debug('EgammaFexBuilder init complete')
    
    def __str__(self):
        descr = ''
        return descr       
    
    def get_sequences(self,chainDict):
        ''' Retrieve the correct reco algorithms and sequences for given chain type '''
        seq = {}
        for key in self._gensequence:
            log.debug('Fex %s'%key)
            seq[key] = self._gensequence[key](chainDict)

        return update_map(seq) 
    
    def _get_fastcalo(self,chainDict):
        chain_part = chainDict['chainParts']
        idperf = False
        if 'trkInfo' in chain_part:
            if chain_part['trkInfo']=='idperf':
                idperf = True
        seq = []
        if 'perf' in chain_part['addInfo'] or 'etcut' in chain_part['addInfo'] or idperf:
            seq = [self._fast_calo_ringer]
        else:
            seq = [self._fast_calo_egamma]
        log.debug('fastcalo %s',seq) 
        return seq
    
    def _get_fastcalohypo(self,chainDict):
        return []
    
    def _get_fastringerhypo(self,chainDict):
        return []

    def _get_fastringer(self,chainDict):
        seq = [self._fast_calo_ringer]
        log.debug('fastringer %s',seq) 
        return seq 

    def _get_fasttrack(self,chainDict):
        seq = self._fast_track
        log.debug('fasttrack %s',seq) 
        return seq

    def _get_precisetrack(self,chainDict):
        seq = self._precise_track
        log.debug('precisetrack %s',seq) 
        return seq

    def _get_trackrec(self,chainDict):
        seq = self._fast_track + self._precise_track
        log.debug('hlttrack %s',seq) 
        return seq

    def _get_fastrec(self,chainDict):
        chain_part = chainDict['chainParts']
        seq = []
        if(chain_part['trigType'] == 'e'):
            seq = [self._fast_electron]
        elif(chain_part['trigType'] == 'g'):
            seq = [self._fast_photon]
        else:
            log.error('Trigger type not known, cannot configured fastrec')
        log.debug('fastrec %s',seq)
        return seq    
    
    def _get_precisecalo(self,chainDict):
        chain_part = chainDict['chainParts']
        calo_ion = False
        seq=[]

        if 'extra' in chain_part:
            if chain_part['extra'] == 'ion':
                calo_ion = True
        if calo_ion: 
            seq = [theTrigCaloCellMaker_eGammaHI, self._tower_maker_ion, self._cluster_maker] 
        else:
            seq = [self._cell_maker,self._tower_maker,self._cluster_maker]
        
        log.debug('precisecalo %s',seq)
        return seq    

    def _get_precisecalocalib(self,chainDict):
        chain_part = chainDict['chainParts']
        seq = []
        if(chain_part['trigType'] == 'e'):
            seq = [self._electron_calib]
        elif(chain_part['trigType'] == 'g'):
            seq = [self._photon_calib]
        else:
            log.error('Trigger type not known, cannot configured precisecalocalib')
        log.debug('precisecalocalib %s',seq)
        return seq    
    
    def _get_preciserec(self,chainDict):
        chain_part = chainDict['chainParts']
        seq = []
        if(chain_part['trigType'] == 'e'):
            seq = [self._get_electronrec(chainDict)]
        elif(chain_part['trigType'] == 'g'):
            seq = [self._get_photonrec(chainDict)]
        else:
            log.error('Trigger type not known, cannot configured preciserec')
        log.debug('preciserec %s',seq)
        return seq    

    def _get_electronrec(self,chainDict):
        chain_part = chainDict['chainParts']
        do_caloiso = False
        log.debug('Electron preciserec')
        if 'isoInfo' in chain_part:
            iso = [x for x in chain_part['isoInfo'] if 'icalo' in x]
            if len(iso)>0:
                log.debug('Electron calo isolation %s',iso[0])
                do_caloiso = True
    
        if do_caloiso:
            log.debug('Electron calo + track isolation')
            return self._egamma_rec_el_caloiso
        else:
            log.debug('Electron default rec')
            return self._egamma_rec

    def _get_photonrec(self,chainDict):
        chain_part = chainDict['chainParts']
        do_conv = False
        do_caloiso = False
        log.debug('Photon preciserec') 
        if 'addInfo' in chain_part:
            if 'conv' in chain_part['addInfo']:
                do_conv = True
        
        if 'isoInfo' in chain_part:
            iso = [x for x in chain_part['isoInfo'] if 'icalo' in x]
            if len(iso)>0:
                log.debug('Electron calo isolation %s',iso[0])
                do_caloiso = True
        if do_caloiso:
            log.debug('Photon isolation')
            return self._egamma_rec_ph_caloiso
        elif do_conv:
            log.debug('Photon conversions')
            return self._egamma_rec_conv
        else:
            log.debug('Photon default rec')
            return self._egamma_rec_noid

class EgammaHypoBuilder(object):
    '''
    Summary:
    EgammaHypoBuilder class defines the Hypos for each chain
    Requires the chainDict to configure the hypos
    chainDict contains an allowed value from SignatureDict
    1 object per chain 

    Attributes:
    '''
    def __init__(self,chainDict):
        '''
        Inits EgammaHypoBuilder with chainDict from TM
        use chainParts dictionary to obtain all relevant info to configure hypos
        '''
        self._base_name = ''
        self._algo_suffix = ''

        ''' Properties of chain required to configure the hypos '''
        self._properties = {'threshold':0,
                            'IDinfo':'',
                            'isoInfo':[],
                            'lhInfo':'',
                            'etcut':False,
                            'perf':False,
                            'ringer':False,
                            'g':False,
                            'e':False,
                            'hiptrt':False,
                            'ion':False,
                            'trkcut':False,
                            'idperf':False}

        self._chain_part = {} 

        ''' 
        Restructure code to return the hypo algorithm for each step 
        this works well for fex sequences
        '''

        self._gensequence = {'fastcalo':self._get_fastcalo,
                             'fastcalorec':self._get_fastcalorec,
                             'fastcalohypo':self._get_fastcalo,
                             'fastringer':self._get_fastringer,
                             'fastringerhypo':self._get_fastringer,
                             'fastrec':self._get_fastrec,
                             'fasttrack':self._get_fasttrack,
                             'precisecalo':self._get_precisecalo,
                             'precisecalocalib':self._get_precisecalocalib,
                             'precisetrack':self._get_precisetrack,
                             'trackrec':self._get_trackrec,
                             'preciserec':self._get_preciserec}

        self._chain_part = chainDict['chainParts']   
        self._set_properties()
        self._set_base_name()
        self._set_algo_suffix()
        log.debug('chainPart %s',self._chain_part)
        log.debug('properties %s',self._properties)
        log.debug('Name %s, suffix %s' % (self._base_name,self._algo_suffix))
        log.debug('init complete')

    def __str__(self):
        descr=''
        return descr    
    
    def get_sequences(self):
        ''' 
        Method called by EgammaSequence
        uses gensequence variable to create hypos for each steps
        '''
        sequences = {}
        for key in self._gensequence:
            log.debug('Hypo %s'%key)
            sequences[key] = self._gensequence[key]()
        # Remove key for algos not defined
        return update_map(sequences) 
    
    def _set_base_name(self):
        thr = str(self._chain_part['threshold'])
        thr = thr.split('.')[0]
        tt = self._chain_part['trigType']
        self._base_name = tt + thr
    
    def _set_algo_suffix(self):
        suffix = self._base_name
        parts = [self._properties['IDinfo'],
                 self._properties['lhInfo'],
                 '_'.join(self._properties['isoInfo'])]
        
        for item in parts:
            if item:
                suffix = suffix + "_" + item

        log.debug('Chain Suffix %s' % suffix)
        self._algo_suffix = suffix
    
    def _set_properties(self):
        ''' Set all properties required to configure each hypo '''
        for key in self._chain_part:
            val = self._chain_part[key]
            if key == 'threshold':
                thr = str(val)
                thr = thr.split('.')[0]
                self._properties[key]=thr
            elif (key == 'IDinfo' or key == 'isoInfo' or key == 'lhInfo'):
                self._properties[key] = val
            elif key == 'trkInfo':
                if val == 'idperf':
                    self._properties[val] = True 
            elif type(val) == list:
                for item in val:
                    if item in self._properties: 
                        self._properties[item] = True
            elif val in self._properties:
                self._properties[val] = True
            else:
                log.debug("Property not required")

        log.debug("Chain Properties Gotcha %s", self._properties) 

    def _get_fastcalo(self):
        seq = []
        #if 'ringer' in self._properties:
        #    if self._properties['ringer']:
        #        seq = self._get_fastringer()
        if self._properties['e']:
            seq = [self._get_fastcalo_electron()]
        elif self._properties['g']:
            seq = [self._get_fastcalo_photon()]
        else:
            log.error("Cannot configure fastcalo_hypo")
            
        log.debug('fastcalo %s',seq) 
        return seq

    def _get_fastcalorec(self):
        seq = [] 
        log.debug('fastcalorec %s',seq) 
        return seq
    
    def _get_fastcalo_electron(self):
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['etcut']:
            algo = L2CaloHypo_EtCut("L2CaloHypo_"+name+"_EtCut",thr )
        elif self._properties['perf']:
            algo = L2CaloHypo_NoCut("L2CaloHypo_"+name+"_NoCut",thr)  
        elif self._properties['ion']:   
            if self._properties['idperf']:
                algo = L2CaloHypo_NoCut("L2CaloHypo_"+name+"_NoCut",thr )
            else:
                algo =  L2CaloHypo_EtCut("L2CaloHypo_"+name+"_EtCut",thr )
        elif idinfo: 
            algo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(thr)+"_"+idinfo,thr,idinfo)
        else:
            log.error("Cannot configure fastcalo_electron")
        return algo
    
    def _get_fastcalo_photon(self):
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if (self._properties['etcut'] or self._properties['perf'] or self._properties['hiptrt'] or self._properties['ion']):
            algo = L2CaloHypo_g_nocut()
        elif self._properties['ion']:   
            if self._properties['idperf']:
                algo = L2CaloHypo_NoCut("L2CaloHypo_"+name+"_NoCut",thr )
            else:
                algo = L2CaloHypo_EtCut("L2CaloHypo_"+name+"_EtCut",thr )
        elif idinfo: 
            algo = L2CaloHypo_g7()
        else:
            log.error("Cannot configure fastcalo_photon")
        return algo

    def _get_fastringer(self):
        ''' 
        Ringer configured as a pair of algorithms
        Fex to compute the ANN from rings
        Hypo to apply cut
        Only case that a fex is included in HypoBuilder
        since depends on several chain properties
        '''
        fex = None
        hypo = None
        tt = self._chain_part['trigType']
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']

        if self._properties['hiptrt']:   
            return [None,None]
        
        if 'merged' in idinfo:  
            fex,hypo = TrigL2CaloRingerFexHypo_e_EtCut(thr)
        elif self._properties['perf']:
            if(tt == 'e'):
                fex,hypo = TrigL2CaloRingerFexHypo_e_NoCut(thr)
            if(tt == 'g'):
                fex,hypo = TrigL2CaloRingerFexHypo_g_NoCut(thr)
        elif self._properties['etcut']:
            if(tt == 'e'):
                fex,hypo = TrigL2CaloRingerFexHypo_e_EtCut(thr)
            if(tt == 'g'):
                fex,hypo = TrigL2CaloRingerFexHypo_g_EtCut(thr)
        elif idinfo:
            if(tt == 'e'):
                fex, hypo = TrigL2CaloRingerFexHypo_e_ID(thr,idinfo,tt)
            if(tt == 'g'):
                # For now, there is no photon ringer tuning. 
                fex, hypo = TrigL2CaloRingerFexHypo_g_NoCut(thr)
        else:
            log.error('Cannot configure ringer')
        seq = [fex,hypo]
        log.debug('fastringer %s',seq) 
        return seq 

    def _get_fasttrack(self):
        seq = [] 
        log.debug('fasttrack %s',seq) 
        return seq

    def _get_precisetrack(self):
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['g']:
            return [algo]
        if (self._properties['etcut'] or self._properties['perf']):
            if self._properties['ion']:
                algo = EFTrackHypo_e_NoCut("EFTrackHypo_"+name+"_NoCut_heavyIon",thr)
            else:
                algo = EFTrackHypo_e_NoCut("EFTrackHypo_"+name+"_NoCut",thr) 
        elif idinfo:
            algo = EFTrackHypo_e("EFTrackHypo_e"+str(thr)+"_"+idinfo,thr)
        else:
            log.error("Cannot configure fastcalo_photon")
        seq = [algo]
        log.debug('precisetrack %s',seq) 
        return seq

    def _get_trackrec(self):
        seq = [] 
        log.debug('hlttrack %s',seq) 
        return seq

    def _get_fastrec(self):
        seq = []
        if self._properties['e']:
            seq = [self._get_fastrec_electron()]
        elif self._properties['g']:
            seq = [self._get_fastrec_photon()]
        else:
            log.error("Cannot configure fastrec_hypo")
        log.debug('fasthypo %s',seq)
        return seq    
    
    def _get_fastrec_electron(self):
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['etcut'] or self._properties['perf'] or self._properties['idperf']:
            algo = L2ElectronHypo_e_NoCut("L2ElectronHypo_"+name+"_NoCut",thr ) 
        elif idinfo:
            algo = L2ElectronHypo_e_ID("TrigL2ElectronHypo_e"+str(thr)+"_"+idinfo,thr,idinfo)
        else:
            log.error("Cannot configure fastrec_electron")
        return algo
    
    def _get_fastrec_photon(self):
        algo = None 
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['hiptrt']:
            return algo
        if self._properties['etcut'] or self._properties['ion']:
            algo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_"+name+"_EtCut",thr )
        elif self._properties['perf']:
            algo  = L2PhotonHypo_g_NoCut("L2PhotonHypo_"+name+"_NoCut",thr )
        elif idinfo:
            algo  = L2PhotonHypo_g_ID("L2PhotonHypo_g"+str(thr)+"_"+idinfo,thr,idinfo)
        else:
            log.error("Cannot configure fastrec_photon")
        return algo

    def _get_precisecalo(self):
        seq = [] 
        log.debug('precisecalo %s',seq)
        return seq    

    def _get_precisecalocalib(self):
        seq = []
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        if (self._properties['etcut']):
            if self._properties['ion']:
                algo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_"+name+"_EtCut_heavyIon",thr)
            else:    
                algo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_"+name+"_EtCut",thr)
        elif (self._properties['perf']):
            if self._properties['ion']:
                algo = TrigEFCaloHypo_All("TrigEFCaloHypo_"+name+"_NoCut_heavyIon",thr)
            else:
                algo = TrigEFCaloHypo_All("TrigEFCaloHypo_"+name+"_NoCut",thr)
        elif self._properties['e']:
            algo = self._get_precisecalocalib_electron()
        elif self._properties['g']:
            algo = self._get_precisecalocalib_photon()
        else:
            log.error('Cannot configure precisecalocalib')
        seq = [algo] 
        log.debug('precisecalocalib %s',seq)
        return seq    
    
    def _get_precisecalocalib_electron(self):
        algo = None 
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['ion']:
            if self._properties['idperf']:
                algo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(thr)+"_"+idinfo+"_heavyIon",thr,idinfo)
            elif idinfo:
                algo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(thr)+"_heavyIon",thr)
            else:
                log.error("Cannot configure precisecalobcalib_electron for ion")
        elif self._properties['ringer']:
            algo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_"+name+"_EtCut",thr)
        elif idinfo:
            algo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(thr)+"_"+idinfo,thr,idinfo)
        else:
            log.error("Cannot configure precisecalobcalib_electron")
        return algo    
        
    def _get_precisecalocalib_photon(self):
        algo = None 
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        if self._properties['hiptrt']:
            algo = TrigEFCaloHypo_All("TrigEFCaloHypo_g_hiptrt_NoCut",0)
        elif self._properties['ion']: 
            if idinfo:
                algo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_"+name+"_EtCut_heavyIon",thr)
        elif idinfo:
            algo = TrigEFCaloHypo_g_ID("TrigEFCaloHypo_g"+str(thr)+"_"+idinfo,thr,idinfo)
        else:
            log.error("Cannot configure precisecalobcalib_photon")
        return algo

    def _get_preciserec(self):
        seq = []
        if self._properties['e']:
            seq = [self._get_electron_hypo()]
        elif self._properties['g']:
            seq = [self._get_photon_hypo()]
        else:
            log.error("Cannot configure preciserec_hypo")
        log.debug('preciserec %s',seq)
        
        return seq    

    def _get_photon_hypo(self):
        log.debug('photon chain')
        algo=None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        isoinfo = self._properties['isoInfo']
        
        suffix = self._algo_suffix
        if self._properties['ion']:
            suffix = suffix +"_heavyIon"

        if not thr:
            log.error("Expect a threshold, none given")
            return algo
        if self._properties['hiptrt']:
            algo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g_hiptrt_NoCut",0) 
        elif self._properties['perf']:
            if self._properties['ion']:
                algo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(thr)+"_NoCut_heavyIon",thr)
            else:
                algo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_"+name+"_NoCut",thr) 
        elif self._properties['etcut']:
            if self._properties['ion']:
                algo = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(thr)+"_EtCut_heavyIon",thr)
            else:
                algo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_"+name+"_EtCut",thr) 
        elif(isoinfo):
            algo  = EFPhotonHypo_g_ID_CaloOnly_Iso("EFPhotonHypo_"+suffix,thr,idinfo,isoinfo)
        elif idinfo:
            algo  = EFPhotonHypo_g_ID_CaloOnly("EFPhotonHypo_"+suffix,thr,idinfo)
        else:
            log.error('Cannot configure precise_photon')
        return algo   

    def _get_electron_hypo(self):     
        log.debug('electron chain')
        algo = None
        name = self._base_name
        thr = self._properties['threshold']
        idinfo = self._properties['IDinfo']
        lhinfo = self._properties['lhInfo']
        isoinfo = self._properties['isoInfo']
        wtp = self._properties['trkcut']
        
        suffix = self._algo_suffix
        if not thr:
            log.error("Expect a threshold, none given")
            return algo
        
        if self._properties['ion']:
            if self._properties['etcut']:
                algo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_"+name+"_NoCut",thr)
            elif self._properties['perf']:
                algo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_"+name+"_NoCut_heavyIon",thr)
            elif idinfo:
                if self._properties['idperf']:
                    algo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(thr)+"_NoCut_heavyIon",thr)
                elif lhinfo:
                    algo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_"+suffix+"_heavyIon",thr,idinfo,lhinfo)
                else: 
                    algo = TrigEFElectronHypo_e_ID_HI("TrigEFElectronHypo_"+suffix+"_heavyIon",thr,idinfo)
            else:
                log.error('Cannot configure precise_electorn for ion')
        elif wtp: 
            algo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(thr)+"_WTP",thr)
        elif (self._properties['idperf'] or self._properties['etcut'] or self._properties['perf']):
            algo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(thr)+"_NoCut",thr)
        elif idinfo:
            if isoinfo:
                if lhinfo:
                    algo = TrigEFElectronHypo_e_LH_Iso("TrigEFElectronHypo_"+suffix,thr,idinfo,lhinfo,isoinfo)
                else:
                    algo = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_"+suffix,thr,idinfo,isoinfo)
            elif lhinfo:
                algo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_"+suffix,thr,idinfo,lhinfo)
            else: 
                algo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_"+suffix,thr,idinfo)
        else:
            log.error("Cannot configure electron hypo")

        return algo   

# Class used to create the sequences
class EgammaSequence(object):
    ''' 
    Summary:
    Top class to generate the Egamma sequence 
    EgammaFexBuilder object as class variable
    
    requires for init: 
    chain dictionary (from TM)
    
    Hypo/Fex Builder return lists of algos
    EgammaSequence merges list to create complete sequences
    for each step, which is kept in a map with key values

    Attributes:
    sequences set with get_sequences()

    sequences with key values:
        self.sequences['fastcalo'] 
        self.sequences['fastcalohypo']
        self.sequences['fastringer'] 
        self.sequences['fasttrack']
        self.sequences['fastrec'] 
        self.sequences['precisecalocalib'] 
        self.sequences['precisetrack']
        self.sequences['trackrec'] 
        self.sequences['preciserec'] 
    
    '''
    fex_obj = EgammaFexBuilder()
    def __init__(self, chainDict):

        '''
        Inits EgammaSequence with chainDict and EgammaFexBuilder object
        merges sequences from Fex and Hypo Builder instances
        '''
        self._chain_name = chainDict['chainName']
        self._disable_mon = not KeepMonitoring(self._chain_name,EgammaChainsToKeepMonitoring)
        self._fex = EgammaSequence.fex_obj.get_sequences(chainDict)
        self._hypoObj = EgammaHypoBuilder(chainDict)
        self.sequences = {}
        log.debug('Fex: %s',EgammaSequence.fex_obj)
        log.debug('Hypos: %s',self._hypoObj)
        log.debug('init complete')
     
    def _config_monitoring(self,seq):
        ''' Disables monitoring for hypos '''
        #if ( disableMon ) : theEFPhotonHypo.AthenaMonTools=DisableMonitoringButValAndTime(theEFPhotonHypo.AthenaMonTools)
        for key,hypo in seq.items():
            hypo[0].AthenaMonTools=DisableMonitoringButValAndTime(hypo[0].AthenaMonTools)
        return
    
    def get_sequences(self):
        log.debug('Get sequences')
        seq = self._fex 
        hypo = self._hypoObj.get_sequences()
        log.debug('Fex: %s',seq)
        log.debug('Hypos: %s',hypo)

        for step in seq:
            log.debug('Creating sequence for step %s', step)
            if seq[step]:
                if step in hypo:
                    log.debug('Merging %s %s', seq[step],hypo[step])
                    seq[step].extend(hypo[step])
                else:
                    log.debug('No hypo for step %s ', step)
            else:
                log.debug('Step %s found, sequence empty for fex ', step)
                seq[step]=hypo[step]
        # The sequences are cleaned to remove None
        # Now need to check for hypos that don't have fex
        # Allows to reset some hypos in a setup function 
        for step in hypo:
            if step not in seq:
                log.debug('Hypo only step %s ', step)
                seq[step]=hypo[step]
        if ( self._disable_mon ):
            self._config_monitoring(hypo)
        self.sequences = seq
        return seq       
    
    def __str__(self):
        descr = ''
        for step in self.sequences:    
            descr = descr + 'Sequence %s, Algos:\n' % (step)
            for algo in self.sequences[step]:
                if(algo):
                    descr = descr + '%s, ' % (algo.name())
                else:
                    log.debug('no algo added for sequence %s ' % step)
            descr = descr + '\n'    
        return descr       

    def print_sequence(self,step):
        for algo in self.sequences[step]:
            if algo:
                log.debug('%s, %s, ' % (step,algo.name()))
            else:
                log.debug('no algo added for sequence %s ' % step)

    def extend_sequence(self,seq,algo):
        """Append an algo to a prefined sequence"""
        sequence = seq.extend([algo])
        return sequence

# try to test standalone
# Test chain dictionary 
test_chain_parts = {
    'trigType':'g',
    'threshold':26.,
    'IDinfo':'medium',
    'addInfo':''
}   
test_chainDict = {'chainParts':test_chain_parts, 'chainName':'testChain'}
print(test_chainDict)
def test_sequence():
    ''' 
    Use a dummy chainParts to test classes
    '''
    electron_seq = EgammaSequence(test_chainDict)
    log.debug('Sequences: %s',electron_seq)
    print_attributes(electron_seq)
    required_seq = ['fastcalo','fastrec','precisecalo','preciserec']
    # Check for required sequences
    for item in required_seq:
        if item in electron_seq.sequences:
            log.debug('%s sequence defined', item)
        else:    
            log.error('%s sequence not defined', item)


    







