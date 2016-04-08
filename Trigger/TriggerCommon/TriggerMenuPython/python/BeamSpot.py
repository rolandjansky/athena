# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
#

from TriggerMenuPython.HltConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import *
from AthenaCommon.Logging import logging
from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
from TrigT2BeamSpot.T2VertexBeamSpotConfig import *
from TrigDetCalib.TrigDetCalibConfig import *

from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_BeamSpot
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_BeamSpot
from TrigGenericAlgs.TrigGenericAlgsConf import  PESA__DummyUnseededAllTEAlgo

tracking = {"SiTrack" : TrigSiTrack_BeamSpot,
            "IDScan"  : TrigIDSCAN_BeamSpot,
            }

try:
    TrigL2SiTrackFinder_Config = __import__('TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config', fromlist=[""])
except ImportError:
    TrigL2SiTrackFinder_Config = None

for strategy in ("A", "B", "F" ):
    toimport = "TrigL2SiTrackFinder_BeamSpot" + strategy
    if TrigL2SiTrackFinder_Config == None:
        tracking["L2Star_" + strategy] = lambda : None
    else:
        tracking["L2Star_" + strategy] = getattr(TrigL2SiTrackFinder_Config, toimport)

l2beamspotfex_idscan       = T2VertexBeamSpot_IDScan()
l2beamspotfex_teidscan     = T2VertexBeamSpot_activeTE_IDScan()
l2beamspotfex_allteidscan  = T2VertexBeamSpot_activeAllTE_IDScan()
l2beamspotfex_sitrack      = T2VertexBeamSpot_SiTrack()
l2beamspotfex_tesitrack    = T2VertexBeamSpot_activeTE_SiTrack()
l2beamspotfex_alltesitrack = T2VertexBeamSpot_activeAllTE_SiTrack()
l2beamspotfex_l2stara       = T2VertexBeamSpot_L2StarA()
l2beamspotfex_tel2stara     = T2VertexBeamSpot_activeTE_L2StarA()
l2beamspotfex_alltel2stara  = T2VertexBeamSpot_activeAllTE_L2StarA()
l2beamspotfex_l2starb       = T2VertexBeamSpot_L2StarB()
l2beamspotfex_tel2starb     = T2VertexBeamSpot_activeTE_L2StarB()
l2beamspotfex_alltel2starb  = T2VertexBeamSpot_activeAllTE_L2StarB()

l2beamspotfex_l2starf       = T2VertexBeamSpot_L2StarF()

robListWriter = TrigSubDetListWriter('IDSubDetListWriter')
robListWriter.SubdetId = [ "SiOnly","TDAQ_CTP" ]
robListWriter.addL1Calo = False

terminateAlgo = PrescaleAlgo("terminateAlgo")

class L2EFChain_BeamSpot(L2EFChainDef):
    isSiTrk = False
    fex = l2beamspotfex_idscan
    alg = terminateAlgo
    trk_alg = tracking["IDScan"]
    trk_chain = 'L2_BStracks_IDScan'
    
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

        self.AlgoName = []

        mlog = logging.getLogger( 'BeamSpot.py' )

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig

    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = ['beamspot']
        self.trigger_type = ['hello']
        self.groups = ['BW:Detector']


    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs,
                           [self.fex],
                           'L2_fex' )
        ### This will probably have to have an if around it
        self.addL2Sequence('L2_fex',
                           [self.alg],
                           'L2_')

        self.addL2Sequence("",
                           [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"),
                            self.trk_alg()],
                           self.trk_chain)
        
    def defineSignatures(self):
        self.addL2Signature(['L2_fex'])
        self.addL2Signature(['L2_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':     'L2' + self.sig_id,
            'L2_fex':  'L2' + self.sig_id + 'fex',
            }
        pass

Config   = L2EFChain_BeamSpot.Config
L2create = L2EFChain_BeamSpot.L2Config
EFcreate = L2EFChain_BeamSpot.EFConfig


#change a LVL1 seed for heavy-ion setup
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
    seeds = "L1_ZDC_A_C_VTE50"
    backup_seeds = "L1_ZDC_A_C_VTE50"
    seed_beamSpot=seeds
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus() :    
    #seeds = "L1_MBTS_2_2"
    #backup_seeds = "L1_MBTS_2_2"
    seeds = "L1_J15"
    backup_seeds = "L1_J15"
    seed_beamSpot = "L1_J10"    
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    seeds = "L1_3J10"
    backup_seeds = "L1_3J10"
    seed_beamSpot=seeds
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    seeds = "L1_4J10"
    backup_seeds = "L1_3J10"
    seed_beamSpot=seeds
else:
    seeds = "L1_4J15"
    backup_seeds = "L1_4J15"
    seed_beamSpot=seeds

low_lumi_seeds = "L1_MBTS_2"


BeamSpots = []

#
# IDScan
#
class L2EFChain_BeamSpot_IDScan(L2EFChain_BeamSpot):
    SiTrk = True
    fex = l2beamspotfex_idscan
    alg = terminateAlgo
    trk_alg = tracking["IDScan"]
    trk_chain = 'L2_BStracks_IDScan'

BeamSpots += [
    L2EFChain_BeamSpot_IDScan('beamspot_vertex_FSTracks_IDScan',
                              'L2_vtxbeamspot_FSTracks_IDScan', 1025, seeds,
                              '', None, ['L2_BStracks_IDScan'],
                              Config(L2create(None), EFcreate(None))),
    ]

#
# activeTE
#
class L2EFChain_BeamSpot_activeTE_IDScan(L2EFChain_BeamSpot):
    fex = l2beamspotfex_teidscan
    alg = robListWriter
    trk_alg = tracking["IDScan"]
    trk_chain = 'L2_BStracks_IDScan'


BeamSpots += [
    L2EFChain_BeamSpot_activeTE_IDScan('beamspot_vertex_activeTE_IDScan_peb',
                                       'L2_vtxbeamspot_activeTE_IDScan_peb', 1028, seeds,
                                       '', None, ['L2_BStracks_IDScan'],
                                       Config(L2create(None), EFcreate(None))),
    ]

#
# allTE
#
class L2EFChain_BeamSpot_allTE_IDScan(L2EFChain_BeamSpot):
    fex = l2beamspotfex_allteidscan
    alg = robListWriter
    trk_alg = tracking["IDScan"]
    trk_chain = 'L2_BStracks_IDScan'

BeamSpots += [
    L2EFChain_BeamSpot_allTE_IDScan('beamspot_vertex_allTE_IDScan_peb',
                                    'L2_vtxbeamspot_allTE_IDScan_peb', 1029, seeds,
                                    '', None, ['L2_BStracks_IDscan'],
                                    Config(L2create(None), EFcreate(None))),
    ]

#Chains seeded off of MBTS for low-lumi running

BeamSpots += [
    L2EFChain_BeamSpot_IDScan('beamspot_vertex_FSTracks_IDScan_MBTS',
                              'L2_vtxbeamspot_FSTracks_IDScan_MBTS', 2025, low_lumi_seeds,
                              '', None, ['L2_BStracks_IDScan'],
                              Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_activeTE_IDScan('beamspot_vertex_activeTE_IDScan_peb_MBTS',
                                       'L2_vtxbeamspot_activeTE_IDScan_peb_MBTS', 2028, low_lumi_seeds,
                                       '', None, ['L2_BStracks_IDScan'],
                                       Config(L2create(None), EFcreate(None))),
    
    L2EFChain_BeamSpot_allTE_IDScan('beamspot_vertex_allTE_IDScan_peb_MBTS',
                                    'L2_vtxbeamspot_allTE_IDScan_peb_MBTS', 2029, low_lumi_seeds,
                                    '', None, ['L2_BStracks_IDscan'],
                                    Config(L2create(None), EFcreate(None))),
    ]


#
# SiTrack
#
class L2EFChain_BeamSpot_SiTrk(L2EFChain_BeamSpot):
    SiTrk = True
    fex = l2beamspotfex_sitrack
    alg = terminateAlgo
    trk_alg = tracking["SiTrack"]
    trk_chain = 'L2_BStracks_SiTrk'    

BeamSpots += [
    L2EFChain_BeamSpot_SiTrk('beamspot_vertex_FSTracks_SiTrk',
                               'L2_vtxbeamspot_FSTracks_SiTrk', 1037, seeds,
                               '', None, ['L2_BStracks_SiTrk'],
                               Config(L2create(None), EFcreate(None))),
    
    L2EFChain_BeamSpot_SiTrk('beamspot_vertex_FSTracks_SiTrk_backup',
                               'L2_vtxbeamspot_FSTracks_SiTrk_backup', 1047, backup_seeds,
                               '', None, ['L2_BStracks_SiTrk'],
                               Config(L2create(None), EFcreate(None))),    
    ]

#
# activeTE
#
class L2EFChain_BeamSpot_activeTE_SiTrk(L2EFChain_BeamSpot):
    SiTrk = True
    fex = l2beamspotfex_tesitrack
    alg = robListWriter
    trk_alg = tracking["SiTrack"]
    trk_chain = 'L2_BStracks_SiTrk'

BeamSpots += [
    L2EFChain_BeamSpot_activeTE_SiTrk('beamspot_vertex_activeTE_SiTrk_peb',
                                        'L2_vtxbeamspot_activeTE_SiTrk_peb', 1038, seeds,
                                        '', None, ['L2_BStracks_SiTrk'],
                                        Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_activeTE_SiTrk('beamspot_vertex_activeTE_SiTrk_peb_backup',
                                        'L2_vtxbeamspot_activeTE_SiTrk_peb_backup', 1048, backup_seeds,
                                        '', None, ['L2_BStracks_SiTrk'],
                                        Config(L2create(None), EFcreate(None))),    
]

#
# allTE
#
class L2EFChain_BeamSpot_allTE_SiTrk(L2EFChain_BeamSpot):
    SiTrk = True
    fex = l2beamspotfex_alltesitrack
    alg = robListWriter
    trk_alg = tracking["SiTrack"]
    trk_chain = 'L2_BStracks_SiTrk'

BeamSpots += [
    L2EFChain_BeamSpot_allTE_SiTrk('beamspot_vertex_allTE_SiTrk_peb',
                                     'L2_vtxbeamspot_allTE_SiTrk_peb', 1039, seeds,
                                     '', None, ['L2_BStracks_SiTrk'],
                                     Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_allTE_SiTrk('beamspot_vertex_allTE_SiTrk_peb_backup',
                                     'L2_vtxbeamspot_allTE_SiTrk_peb_backup', 1049, backup_seeds,
                                     '', None, ['L2_BStracks_SiTrk'],
                                     Config(L2create(None), EFcreate(None))),    
    ]

#Chains seeded off of MBTS for low-lumi running
BeamSpots += [
    L2EFChain_BeamSpot_SiTrk('beamspot_vertex_FSTracks_SiTrk_MBTS',
                               'L2_vtxbeamspot_FSTracks_SiTrk_MBTS', 2037, low_lumi_seeds,
                               '', None, ['L2_BStracks_SiTrk'],
                               Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_activeTE_SiTrk('beamspot_vertex_activeTE_SiTrk_peb_MBTS',
                                        'L2_vtxbeamspot_activeTE_SiTrk_peb_MBTS', 2038, low_lumi_seeds,
                                        '', None, ['L2_BStracks_SiTrk'],
                                        Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_allTE_SiTrk('beamspot_vertex_allTE_SiTrk_peb_MBTS',
                                     'L2_vtxbeamspot_allTE_SiTrk_peb_MBTS', 2039, low_lumi_seeds,
                                     '', None, ['L2_BStracks_SiTrk'],
                                     Config(L2create(None), EFcreate(None))),    
    ]


#
# L2Star
#

class L2EFChain_BeamSpot_L2Star_A(L2EFChain_BeamSpot):
    fex = l2beamspotfex_l2stara # Still waiting on the new fexes
    alg = terminateAlgo
    trk_alg = tracking["L2Star_A"]
    trk_chain = "L2_BStracks_L2Star_A"

class L2EFChain_BeamSpot_activeTE_L2Star_A(L2EFChain_BeamSpot):
    fex = l2beamspotfex_tel2stara # Still waiting on the new fexes
    alg = robListWriter
    trk_alg = tracking["L2Star_A"]
    trk_chain = "L2_BStracks_L2Star_A"

class L2EFChain_BeamSpot_allTE_L2Star_A(L2EFChain_BeamSpot):
    fex = l2beamspotfex_alltel2stara # Still waiting on the new fexes
    alg = robListWriter
    trk_alg = tracking["L2Star_A"]
    trk_chain = "L2_BStracks_L2Star_A"        
    

class L2EFChain_BeamSpot_L2Star_B(L2EFChain_BeamSpot):
    fex = l2beamspotfex_l2starb # Still waiting on the new fexes
    alg = terminateAlgo
    trk_alg = tracking["L2Star_B"]
    trk_chain = "L2_BStracks_L2Star_B"


class L2EFChain_BeamSpot_L2Star_F(L2EFChain_BeamSpot):
    fex = l2beamspotfex_l2starf # Still waiting on the new fexes
    alg = terminateAlgo
    trk_alg = tracking["L2Star_F"]
    trk_chain = "L2_BStracks_L2Star_F"

class L2EFChain_FTKVtx_L2Star_F(L2EFChain_BeamSpot):
    fex = l2beamspotfex_l2starf
    alg = terminateAlgo
    trk_alg = tracking["L2Star_F"]
    trk_chain = "L2_BStracks_L2Star_F"

    def defineSequences(self, config):
        #L2 sequences
        from InDetTrigRecExample.EFInDetConfig import *
        thevtx = TrigEFIDSequence("FullScan", "fullScan", "StandAloneVtxAllTE").getSequence()
        self.addL2Sequence(self.inputTEs,
                           thevtx,
                           'L2_fex' )
        self.addL2Sequence('L2_fex',
                           [PrescaleAlgo("terminateAlgo1",Prescale=1.)],
                           'L2_')



class L2EFChain_BeamSpot_activeTE_L2Star_B(L2EFChain_BeamSpot):
    fex = l2beamspotfex_tel2starb # Still waiting on the new fexes
    alg = robListWriter
    trk_alg = tracking["L2Star_B"]
    trk_chain = "L2_BStracks_L2Star_B"

class L2EFChain_BeamSpot_allTE_L2Star_B(L2EFChain_BeamSpot):
    fex = l2beamspotfex_alltel2starb # Still waiting on the new fexes
    alg = robListWriter
    trk_alg = tracking["L2Star_B"]
    trk_chain = "L2_BStracks_L2Star_B"        
    

BeamSpots += [
    L2EFChain_BeamSpot_L2Star_A('beamspot_vertex_FSTracks_L2Star_A',
                               'L2_vtxbeamspot_FSTracks_L2Star_A', 2050, seeds,
                               '', None, ['L2_BStracks_L2Star_A'],
                               Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_activeTE_L2Star_A('beamspot_vertex_activeTE_L2Star_A_peb',
                                        'L2_vtxbeamspot_activeTE_L2Star_A_peb', 2051, seeds,
                                        '', None, ['L2_BStracks_L2Star_A'],
                                        Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_allTE_L2Star_A('beamspot_vertex_allTE_L2Star_A_peb',
                                   'L2_vtxbeamspot_allTE_L2Star_A_peb', 2052, seeds,
                                   '', None, ['L2_BStracks_L2Star_A'],
                                   Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_L2Star_B('beamspot_vertex_FSTracks_L2Star_B',
                               'L2_vtxbeamspot_FSTracks_L2Star_B', 2053,seed_beamSpot,
                               '', None, ['L2_BStracks_L2Star_B'],
                               Config(L2create(None), EFcreate(None))),


    L2EFChain_BeamSpot_activeTE_L2Star_B('beamspot_vertex_activeTE_L2Star_B_peb',
                                        'L2_vtxbeamspot_activeTE_L2Star_B_peb', 2054, seed_beamSpot,
                                        '', None, ['L2_BStracks_L2Star_B'],
                                        Config(L2create(None), EFcreate(None))),

    L2EFChain_BeamSpot_allTE_L2Star_B('beamspot_vertex_allTE_L2Star_B_peb',
                                   'L2_vtxbeamspot_allTE_L2Star_B_peb', 2055, seed_beamSpot,
                                   '', None, ['L2_BStracks_L2Star_B'],
                                   Config(L2create(None), EFcreate(None))),        


    L2EFChain_BeamSpot_L2Star_F('beamspot_vertex_FSTracks_L2Star_F',
#                              'L2_vtxbeamspot_FSTracks_L2Star_F', 2057, seeds,
                                'L2_vtxbeamspot_FSTracks_L2Star_F', 2057, low_lumi_seeds,
                                '', None, ['L2_BStracks_L2Star_F'],
                                Config(L2create(None), EFcreate(None))),

    
    L2EFChain_FTKVtx_L2Star_F('ftk_vertex_FSTracks_L2Star_F',
                              'L2_FTKVtx', 2058, '',
                              '', None, ['L2_BStracks_L2Star_F'],
                              Config(L2create(None), EFcreate(None))),    
    ]

