# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypo

from TriggerJobOpts.TriggerFlags import TriggerFlags
menu_name = TriggerFlags.triggerMenuSetup()

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
from AthenaCommon.SystemOfUnits import GeV

# cut map class -- based on L2CaloHypoCutMaps
class L2PhotonCutMaps ():
    def __init__(self, threshold):
        if(float(threshold) >= 0. and float(threshold) < 10):
            self.MapsHADETthr = {
                'loose': [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147],
                'loose1': [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147],
                'medium':[0.0254625, 0.0238875, 0.0270375, 0.0207375, 0.03465, 0.0378, 0.03465, 0.0286125, 0.02625],
                'medium1':[0.0254625, 0.0238875, 0.0270375, 0.0207375, 0.03465, 0.0378, 0.03465, 0.0286125, 0.02625],
                'tight':[0.0254625, 0.0238875, 0.0270375, 0.0207375, 0.03465, 0.0378, 0.03465, 0.0286125, 0.02625],
                'tight1':[0.0254625, 0.0238875, 0.0270375, 0.0207375, 0.03465, 0.0378, 0.03465, 0.0286125, 0.02625],
                }
            self.MapsCARCOREthr = {
                'loose': [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76],
                'loose1': [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76],
                'medium':[0.83125, 0.719625, 0.814625, 0.83125, 0.703, 0.817, 0.83125, 0.8265, 0.719625],
                'medium1':[0.83125, 0.719625, 0.814625, 0.83125, 0.703, 0.817, 0.83125, 0.8265, 0.719625],
                'tight':[0.83125, 0.719625, 0.814625, 0.83125, 0.703, 0.817, 0.83125, 0.8265, 0.719625],
                'tight1':[0.83125, 0.719625, 0.814625, 0.83125, 0.703, 0.817, 0.83125, 0.8265, 0.719625],
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        elif(float(threshold) >= 10. and float(threshold) < 15):
            self.MapsHADETthr = {
                'loose': [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                'loose1': [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                'medium':[0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                'medium1':[0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                'tight':[0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                'tight1':[0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875],
                }
            self.MapsCARCOREthr = {
                'loose': [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                'loose1': [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                'medium':[0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                'medium1':[0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                'tight':[0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                'tight1':[0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625],
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        elif(float(threshold) >= 15. and float(threshold) < 20):
            self.MapsHADETthr = {
                'loose':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                'loose1':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                'medium':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                'medium1':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                'tight':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                'tight1':[0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375], 
                }
            self.MapsCARCOREthr = {
                'loose':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                'loose1':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                'medium':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                'medium1':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                'tight':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                'tight1':[0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125], 
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        elif(float(threshold) >= 20. and float(threshold) < 30):
            self.MapsHADETthr = {
                'loose':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'loose1':[0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375],
                'medium':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'medium1':[0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375],
                'tight':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'tight1':[0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375],
                }
            self.MapsCARCOREthr = {
                'loose':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'loose1':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'medium':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'medium1':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'tight':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'tight1':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        elif(float(threshold) >= 30. and float(threshold) < 40):
            self.MapsHADETthr = {
                'loose':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'loose1':[0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375],
                'medium':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'medium1':[0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375],
                'tight':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'tight1':[0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375],
                }
            self.MapsCARCOREthr = {
                'loose':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'loose1':[0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125],
                'medium':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'medium1':[0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125],
                'tight':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'tight1':[0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125],
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1':[-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        elif(float(threshold) >= 40.):
            self.MapsHADETthr = {
                'loose':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'loose1':[0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875],
                'medium':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'medium1':[0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875],
                'tight':[0.071, 0.062, 0.075, 0.060, 0.051, 0.057, 0.075, 0.072, 0.051],
                'tight1':[0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875],
                }
            self.MapsCARCOREthr = {
                'loose':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'loose1':[0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375],
                'medium':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'medium1':[0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375],
                'tight':[0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625],
                'tight1':[0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375],
                }
            self.MapsCAERATIOthr = {
                'loose': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'loose1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'medium1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                'tight1': [-999., -999., -999., -999., -999., -999., -999., -999., -999.],
                }
        else:
             raise RuntimeError('INCORRECT threshold: No cuts configured')           
# basic cut
class TrigL2PhotonHypoBase (TrigL2PhotonHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2PhotonHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigL2PhotonHypoMonitoring import TrigL2PhotonHypoValidationMonitoring, TrigL2PhotonHypoOnlineMonitoring, TrigL2PhotonHypoCosmicMonitoring
        validation = TrigL2PhotonHypoValidationMonitoring()
        online     = TrigL2PhotonHypoOnlineMonitoring()
        cosmic     = TrigL2PhotonHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]

        self.ETthr          = [0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]

        self.EtaBins        = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.005]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADET2thr      = [999, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.HADETthr       = [0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035]

#----------------------------------------------------------
# DC14
#----------------------------------------------------------
# EtCut only chains
# Et threshold cut applied at L2 only
class L2PhotonHypo_g_EtCut (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2PhotonHypo_g_EtCut, self ).__init__( name ) 
        self.AcceptAll = False
        self.EtCutOnly = True
        #L2Calo uses 3 GeV below threshold
        self.ETthr          = [(float(threshold)-1.)*GeV]*9

class L2PhotonHypo_g_NoCut (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2PhotonHypo_g_NoCut, self ).__init__( name ) 
        self.AcceptAll = True
        self.EtCutOnly = True
        self.ETthr          = [(float(threshold)-1.)*GeV]*9

class L2PhotonHypo_g_ID (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2PhotonHypo_g_ID, self ).__init__( name ) 
        self.AcceptAll = False
        self.EtCutOnly = False
        #Consistent with L2Calo for electrons
        self.ETthr          = [(float(threshold)-3.)*GeV]*9
        self.HADETthr = L2PhotonCutMaps(threshold).MapsHADETthr[IDinfo]
        self.CARCOREthr = L2PhotonCutMaps(threshold).MapsCARCOREthr[IDinfo]
        self.CAERATIOthr =L2PhotonCutMaps(threshold).MapsCAERATIOthr[IDinfo]

class L2PhotonHypo_g140_loose1 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g140_loose1"):
        super( L2PhotonHypo_g140_loose1, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g140_loose (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g140_loose"):
        super( L2PhotonHypo_g140_loose, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

#----------------------------------------------------------
class L2PhotonHypo_1 (TrigL2PhotonHypoBase):  
    def __init__(self, name = "L2PhotonHypo_1"):
        super( L2PhotonHypo_1, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        self.EtCutOnly = False
        self.ETthr          = [7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV]
        self.HADETthr       = [999, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]


#-------------------------------------------------------------------
# g3

class L2PhotonHypo_g3 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g3"):
        super( L2PhotonHypo_g3, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV]
        self.HADETthr       = [0.034, 0.030, 0.030, 0.030, 0.058, 0.070, 0.070, 0.050, 0.040]
        self.CARCOREthr     = [0.680, 0.680, 0.680, 0.680, 0.650, 0.730, 0.800, 0.800, 0.800]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]

class L2PhotonHypo_g3_nocut (L2PhotonHypo_g3):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g3_nocut"):
        super( L2PhotonHypo_g3_nocut, self ).__init__( name )
        self.AcceptAll = True
        
class L2PhotonHypo_g3_loose (L2PhotonHypo_g3):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g3_loose"):
        super( L2PhotonHypo_g3_loose, self ).__init__( name )
        self.AcceptAll = False
        self.HADETthr       = [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76]


#-------------------------------------------------------------------
# g5
              
class L2PhotonHypo_g5 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5"):
        super( L2PhotonHypo_g5, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr = [4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV]
        self.HADETthr       = [0.021, 0.019, 0.018, 0.019, 0.058, 0.056, 0.041, 0.031, 0.040]
        self.CARCOREthr     = [0.695, 0.695, 0.695, 0.695, 0.650, 0.855, 0.875, 0.875, 0.800]
        self.CAERATIOthr    = [0.57, 0.28, 0.07, 0.09, -9999., 0.08, 0.47, 0.58, -9999.]

class L2PhotonHypo_g5_nocut (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_nocut"):
        super( L2PhotonHypo_g5_nocut, self ).__init__( name )
        self.AcceptAll = True

#a hypo config for the g5_nocut that is seeded by the MTBS
class L2PhotonHypo_g5_nocut_mtbs (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_nocut_mtbs"):
        super( L2PhotonHypo_g5_nocut_mtbs, self ).__init__( name )
        self.AcceptAll = True

class L2PhotonHypo_g5_loose (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_loose"):
        super( L2PhotonHypo_g5_loose, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.] 
        self.HADETthr       = [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76]

class L2PhotonHypo_g5_medium (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_medium"):
        super( L2PhotonHypo_g5_medium, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.0254625, 0.0238875, 0.0270375, 0.0207375, 0.03465, 0.0378, 0.03465, 0.0286125, 0.02625]
        self.CARCOREthr     = [0.83125, 0.719625, 0.814625, 0.83125, 0.703, 0.817, 0.83125, 0.8265, 0.719625]

class L2PhotonHypo_g5_loose_TauMass (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_loose_TauMass"):
        super( L2PhotonHypo_g5_loose_TauMass, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.1725, 0.162, 0.126, 0.166, 0.1425, 0.142, 0.162, 0.166, 0.15]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76]

class L2PhotonHypo_g5_tight (L2PhotonHypo_g5):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_tight"):
        super( L2PhotonHypo_g5_tight, self ).__init__( name )
#-------------------------------------------------------------------
# g6

class L2PhotonHypo_g6 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g6"):
        super( L2PhotonHypo_g6, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr = [5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV, 5.*GeV]
        self.HADETthr       = [0.021, 0.019, 0.018, 0.019, 0.058, 0.056, 0.041, 0.031, 0.040]
        self.CARCOREthr     = [0.695, 0.695, 0.695, 0.695, 0.650, 0.855, 0.875, 0.875, 0.800]
        self.CAERATIOthr    = [0.57, 0.28, 0.07, 0.09, -9999., 0.08, 0.47, 0.58, -9999.]

class L2PhotonHypo_g6_etcut (L2PhotonHypo_g6):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g6_etcut"):
        super( L2PhotonHypo_g6_etcut, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g6_loose (L2PhotonHypo_g6):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g6_loose"):
        super( L2PhotonHypo_g6_loose, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76]


#-------------------------------------------------------------------
# g7

class L2PhotonHypo_g7 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g7"):
        super( L2PhotonHypo_g7, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr = [6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV, 6.*GeV]
        self.HADETthr       = [0.021, 0.019, 0.018, 0.019, 0.058, 0.056, 0.041, 0.031, 0.040]
        self.CARCOREthr     = [0.695, 0.695, 0.695, 0.695, 0.650, 0.855, 0.875, 0.875, 0.800]
        self.CAERATIOthr    = [0.57, 0.28, 0.07, 0.09, -9999., 0.08, 0.47, 0.58, -9999.]

class L2PhotonHypo_g7_loose (L2PhotonHypo_g7):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g7_loose"):
        super( L2PhotonHypo_g7_loose, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.1638, 0.1596, 0.1218, 0.1638, 0.0448875, 0.1386, 0.1596, 0.1638, 0.147]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.76]


class L2PhotonHypo_g7_medium (L2PhotonHypo_g7):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g7_medium"):
        super( L2PhotonHypo_g7_medium, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.16600, 0.16200, 0.12600, 0.16600, 0.05275, 0.14200, 0.16200, 0.16600, 0.15000]
        self.CARCOREthr     = [0.532, 0.57, 0.646, 0.684, 0.418, 0.722, 0.684, 0.722, 0.710125]

class L2PhotonHypo_g7_tight (L2PhotonHypo_g7):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g7_tight"):
        super( L2PhotonHypo_g7_tight, self ).__init__( name )
#-------------------------------------------------------------------
# g9

class L2PhotonHypo_g9 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g9"):
        super( L2PhotonHypo_g9, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr = [8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV, 8.*GeV]
        self.HADETthr       = [0.021, 0.019, 0.018, 0.019, 0.058, 0.056, 0.041, 0.031, 0.040]
        self.CARCOREthr     = [0.695, 0.695, 0.695, 0.695, 0.650, 0.855, 0.875, 0.875, 0.800]
        self.CAERATIOthr    = [0.57, 0.28, 0.07, 0.09, -9999., 0.08, 0.47, 0.58, -9999.]

class L2PhotonHypo_g9_etcut (L2PhotonHypo_g9):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g9_etcut"):
        super( L2PhotonHypo_g9_etcut, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g9_tight (L2PhotonHypo_g9):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g9_tight"):
        super( L2PhotonHypo_g9_tight, self ).__init__( name )

#-------------------------------------------------------------------
# g10

class L2PhotonHypo_g10 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10"):
        super( L2PhotonHypo_g10, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV, 9.*GeV]
        # L2 Photon cuts: same as e10_loose
        self.HADETthr       = [0.022, 0.022, 0.022, 0.022, 0.043, 0.043, 0.035, 0.035, 0.035] 
        self.CARCOREthr     = [0.810, 0.810, 0.810, 0.800, 0.660, 0.800, 0.830, 0.830, 0.830]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]

class L2PhotonHypo_g10_nocut (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_nocut"):
        super( L2PhotonHypo_g10_nocut, self ).__init__( name )
        self.AcceptAll = True

class L2PhotonHypo_g10_loose (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_loose"):
        super( L2PhotonHypo_g10_loose, self ).__init__( name )
        self.AcceptAll = False
        self.HADETthr      = [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875]
        self.CARCOREthr    = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625]

class L2PhotonHypo_g10i_loose (L2PhotonHypo_g10_loose):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10i_loose"):
        super( L2PhotonHypo_g10i_loose, self ).__init__( name )

class L2PhotonHypo_g10_calib (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_calib"):
        super( L2PhotonHypo_g10_calib, self ).__init__( name )
        self.AcceptAll = False
        
class L2PhotonHypo_g10_medium (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_medium"):
        super( L2PhotonHypo_g10_medium, self ).__init__( name )
        self.AcceptAll = False
        self.HADETthr      = [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875]
        self.CARCOREthr    = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625]

class L2PhotonHypo_g10_loose_TauMass (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_loose_TauMass"):
        super( L2PhotonHypo_g10_loose_TauMass, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.0875, 0.0875, 0.0825, 0.0575, 0.0725, 0.0825, 0.0725, 0.05325, 0.0475]
        self.CARCOREthr     = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625] 

class L2PhotonHypo_g10_tight (L2PhotonHypo_g10):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g10_tight"):
        super( L2PhotonHypo_g10_tight, self ).__init__( name )
#-------------------------------------------------------------------
# g11

class L2PhotonHypo_g11 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g11"):
        super( L2PhotonHypo_g11, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV, 10.*GeV]
        self.HADETthr       = [0.022, 0.022, 0.022, 0.022, 0.043, 0.043, 0.035, 0.035, 0.035] 
        self.CARCOREthr     = [0.810, 0.810, 0.810, 0.800, 0.660, 0.800, 0.830, 0.830, 0.830]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]

class L2PhotonHypo_g11_etcut (L2PhotonHypo_g11):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g11_etcut"):
        super( L2PhotonHypo_g11_etcut, self ).__init__( name )
        self.EtCutOnly = True


#-------------------------------------------------------------------
# g12

class L2PhotonHypo_g12 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12"):
        super( L2PhotonHypo_g12, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV, 11.*GeV]
        # L2 Electron cuts: #same as e10
        self.HADETthr       = [0.022, 0.022, 0.022, 0.022, 0.043, 0.043, 0.035, 0.035, 0.035] 
        self.CARCOREthr     = [0.810, 0.810, 0.810, 0.800, 0.660, 0.800, 0.830, 0.830, 0.830]
        self.CAERATIOthr    = [0.5 , 0.5 , 0.39 , 0.29 , -9999., 0.29 , 0.6 , 0.6 , -9999.]

# Comply with DC14 - Run1
class L2PhotonHypo_g12_loose1 (L2PhotonHypo_g12):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12_loose1"):
        super( L2PhotonHypo_g12_loose1, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875]
        self.CARCOREthr    = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625]

class L2PhotonHypo_g12_loose (L2PhotonHypo_g12):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12_loose"):
        super( L2PhotonHypo_g12_loose, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875]
        self.CARCOREthr    = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625]

class L2PhotonHypo_g12_medium (L2PhotonHypo_g12):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12_medium"):
        super( L2PhotonHypo_g12_medium, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0359625, 0.0343875, 0.0396375, 0.0291375, 0.0501375, 0.0559125, 0.0548625, 0.0538125, 0.0469875]
        self.CARCOREthr    = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625]

class L2PhotonHypo_g12_loose_TauMass (L2PhotonHypo_g12):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12_loose_TauMass"):
        super( L2PhotonHypo_g12_loose_TauMass, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.0875, 0.0875, 0.0825, 0.0575, 0.0725, 0.0825, 0.0725, 0.05325, 0.0475]
        self.CARCOREthr     = [0.786125, 0.786125, 0.767125, 0.795625, 0.703, 0.776625, 0.819375, 0.805125, 0.681625] 

class L2PhotonHypo_g12_tight (L2PhotonHypo_g12):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g12_tight"):
        super( L2PhotonHypo_g12_tight, self ).__init__( name )
#-------------------------------------------------------------------
# g15

class L2PhotonHypo_g15 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g15"):
        super( L2PhotonHypo_g15, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV, 14.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.015, 0.013, 0.013, 0.015, 0.018, 0.032, 0.025, 0.018, 0.024] 
        self.CARCOREthr     = [0.85, 0.85, 0.85, 0.85, 0.66, 0.85, 0.87, 0.87, 0.87]

class L2PhotonHypo_g15_loose (L2PhotonHypo_g15):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g15_loose"):
        super( L2PhotonHypo_g15_loose, self ).__init__( name )
        self.HADETthr   = [0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375]
        self.CARCOREthr = [0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125]

class L2PhotonHypo_g15i_loose (L2PhotonHypo_g15):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g15i_loose"):
        super( L2PhotonHypo_g15i_loose, self ).__init__( name )
        self.HADETthr   = [0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375]
        self.CARCOREthr = [0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125]

class L2PhotonHypo_g15_medium (L2PhotonHypo_g15):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g15_medium"):
        super( L2PhotonHypo_g15_medium, self ).__init__( name )
        self.HADETthr   = [0.0328125, 0.0312375, 0.0354375, 0.0270375, 0.0459375, 0.0527625, 0.0433125, 0.0485625, 0.0396375]
        self.CARCOREthr = [0.809875, 0.805125, 0.786125, 0.809875, 0.703, 0.795625, 0.819375, 0.814625, 0.691125]

class L2PhotonHypo_g15_tight (L2PhotonHypo_g15):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g15_tight"):
        super( L2PhotonHypo_g15_tight, self ).__init__( name )
#-------------------------------------------------------------------
# g17

class L2PhotonHypo_g17 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g17"):
        super( L2PhotonHypo_g17, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV, 16.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.015, 0.013, 0.013, 0.015, 0.018, 0.032, 0.025, 0.018, 0.024] 
        self.CARCOREthr     = [0.85, 0.85, 0.85, 0.85, 0.66, 0.85, 0.87, 0.87, 0.87]

class L2PhotonHypo_g17_etcut (L2PhotonHypo_g17):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g17_etcut"):
        super( L2PhotonHypo_g17_etcut, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g17i_tight (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g17i_tight"):
        super( L2PhotonHypo_g17i_tight, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.HADETthr       = [0.015, 0.013, 0.013, 0.015, 0.018, 0.032, 0.025, 0.018, 0.024] 
        self.CARCOREthr     = [0.85, 0.85, 0.85, 0.85, 0.66, 0.85, 0.87, 0.87, 0.87]
        self.CAERATIOthr    = [0.71, 0.63, 0.47, 0.43, -9999., 0.39, 0.82, 0.80, -9999.]


#-------------------------------------------------------------------
# g20

class L2PhotonHypo_g20 (TrigL2PhotonHypoBase):  ## Primary Trigger
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20"):
        super( L2PhotonHypo_g20, self ).__init__( name )
        self.AcceptAll = False 
        self.EtCutOnly = False
        self.ETthr          = [19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV, 19.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]
# DC14 - Run1
class L2PhotonHypo_g20_loose1 (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_loose1"):
        super( L2PhotonHypo_g20_loose1, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

#DC14 - Run1 (never used tight!) using loose selection
class L2PhotonHypo_g20_tight1 (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_tight1"):
        super( L2PhotonHypo_g20_tight1, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g20_loose (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_loose"):
        super( L2PhotonHypo_g20_loose, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g20i_loose (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20i_loose"):
        super( L2PhotonHypo_g20i_loose, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g20i_calib (L2PhotonHypo_g20i_loose):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20i_calib"):
        super( L2PhotonHypo_g20i_calib, self ).__init__( name )

# used for 2g20_medium for SM group
class L2PhotonHypo_g20_medium (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_medium"):
        super( L2PhotonHypo_g20_medium, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g20_tight (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_tight"):
        super( L2PhotonHypo_g20_tight, self ).__init__( name )

class L2PhotonHypo_g20_etcut (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_etcut"):
        super( L2PhotonHypo_g20_etcut, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g20_etcut_xe30_noMu (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_etcut_xe30_noMu"):
        super( L2PhotonHypo_g20_etcut_xe30_noMu, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g20_loose_TauMass (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g20_loose_TauMass"):
        super( L2PhotonHypo_g20_loose_TauMass, self ).__init__( name )
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.0525, 0.0525, 0.0425, 0.0425, 0.0575, 0.0525, 0.03725, 0.0375, 0.02975]
        self.CARCOREthr     = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

#-------------------------------------------------------------------
#g24
class L2PhotonHypo_g24_etcut (L2PhotonHypo_g20):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g24_etcut"):
        super( L2PhotonHypo_g24_etcut, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = True
        self.ETthr = [23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV, 23.*GeV]
        
        
#-------------------------------------------------------------------
# g25

class L2PhotonHypo_g25 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25"):
        super( L2PhotonHypo_g25, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV, 24.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g25_loose1 (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25_loose1"):
        super( L2PhotonHypo_g25_loose1, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g25_loose (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25_loose"):
        super( L2PhotonHypo_g25_loose, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g25i_loose (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25i_loose"):
        super( L2PhotonHypo_g25i_loose, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

# DC14 - Run1
class L2PhotonHypo_g25_medium1 (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25_medium1"):
        super( L2PhotonHypo_g25_medium1, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g25_medium (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25_medium"):
        super( L2PhotonHypo_g25_medium, self ).__init__( name )
        self.HADETthr      = [0.0275625, 0.0259875, 0.0291375, 0.0228375, 0.0259875, 0.0391125, 0.0359625, 0.0370125, 0.0291375]
        self.CARCOREthr = [0.819375, 0.819375, 0.800375, 0.828875, 0.7125, 0.805125, 0.843125, 0.824125, 0.700625]

class L2PhotonHypo_g25i_calib (L2PhotonHypo_g25i_loose):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25i_calib"):
        super( L2PhotonHypo_g25i_calib, self ).__init__( name )

class L2PhotonHypo_g25_tight (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25_tight"):
        super( L2PhotonHypo_g25_tight, self ).__init__( name )

class L2PhotonHypo_g25i_tight (L2PhotonHypo_g25_tight):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g25i_tight"):
        super( L2PhotonHypo_g25i_tight, self ).__init__( name )


#-------------------------------------------------------------------
# g27

class L2PhotonHypo_g27_etcut (L2PhotonHypo_g25):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g27_etcut"):
        super( L2PhotonHypo_g27_etcut, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = True
        self.ETthr = [26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV, 26.*GeV]


#-------------------------------------------------------------------
# g30

class L2PhotonHypo_g30 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g30"):
        super( L2PhotonHypo_g30, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV, 29.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

class L2PhotonHypo_g30_loose (L2PhotonHypo_g30):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g30_loose"):
        super( L2PhotonHypo_g30_loose, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]


class L2PhotonHypo_g35 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g35"):
        super( L2PhotonHypo_g35, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV, 34.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g35_loose1 (L2PhotonHypo_g35):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g35_loose1"):
        super( L2PhotonHypo_g35_loose1, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]

class L2PhotonHypo_g35_loose (L2PhotonHypo_g35):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g35_loose"):
        super( L2PhotonHypo_g35_loose, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]

class L2PhotonHypo_g30_medium (L2PhotonHypo_g30):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g30_medium"):
        super( L2PhotonHypo_g30_medium, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]

class L2PhotonHypo_g30_tight (L2PhotonHypo_g30):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g30_tight"):
        super( L2PhotonHypo_g30_tight, self ).__init__( name )

# DC14 - Run1
class L2PhotonHypo_g35_medium1 (L2PhotonHypo_g35):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g35_medium1"):
        super( L2PhotonHypo_g35_medium1, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]

class L2PhotonHypo_g35_medium (L2PhotonHypo_g35):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g35_medium"):
        super( L2PhotonHypo_g35_medium, self ).__init__( name )
        self.HADETthr      = [0.0249375, 0.0233625, 0.0217875, 0.0202125, 0.0202125, 0.0265125, 0.0333375, 0.0328125, 0.0249375]
        self.CARCOREthr   = [0.852625, 0.843125, 0.833625, 0.833625, 0.7505, 0.828875, 0.843125, 0.838375, 0.748125]

#-------------------------------------------------------------------
# g40

class L2PhotonHypo_g40 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g40"):
        super( L2PhotonHypo_g40, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV, 39.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g40_loose1 (L2PhotonHypo_g40):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g40_loose1"):
        super( L2PhotonHypo_g40_loose1, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g40_loose (L2PhotonHypo_g40):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g40_loose"):
        super( L2PhotonHypo_g40_loose, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g40_medium (L2PhotonHypo_g40):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g40_medium"):
        super( L2PhotonHypo_g40_medium, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g40_tight (L2PhotonHypo_g40):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g40_tight"):
        super( L2PhotonHypo_g40_tight, self ).__init__( name )


#-------------------------------------------------------------------
# g50

class L2PhotonHypo_g50 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g50"):
        super( L2PhotonHypo_g50, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV, 49.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g50_loose1 (L2PhotonHypo_g50):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g50_loose1"):
        super( L2PhotonHypo_g50_loose1, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g50_loose (L2PhotonHypo_g50):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g50_loose"):
        super( L2PhotonHypo_g50_loose, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g50_etcut (L2PhotonHypo_g50):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g50_etcut"):
        super( L2PhotonHypo_g50_etcut, self ).__init__( name )
        self.EtCutOnly = True


#-------------------------------------------------------------------
# g55

class L2PhotonHypo_g55 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g55"):
        super( L2PhotonHypo_g55, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV, 53.0*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

class L2PhotonHypo_g55_tight (L2PhotonHypo_g55):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g55_tight"):
        super( L2PhotonHypo_g55_tight, self ).__init__( name )
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]


#-------------------------------------------------------------------
# g60

class L2PhotonHypo_g60 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g60"):
        super( L2PhotonHypo_g60, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV, 58.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1 
class L2PhotonHypo_g60_loose1 (L2PhotonHypo_g60):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g60_loose1"):
        super( L2PhotonHypo_g60_loose1, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g60_loose (L2PhotonHypo_g60):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g60_loose"):
        super( L2PhotonHypo_g60_loose, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]


#-------------------------------------------------------------------
# g80

class L2PhotonHypo_g80 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g80"):
        super( L2PhotonHypo_g80, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV, 77.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g80_loose1 (L2PhotonHypo_g80):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g80_loose1"):
        super( L2PhotonHypo_g80_loose1, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g80_loose (L2PhotonHypo_g80):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g80_loose"):
        super( L2PhotonHypo_g80_loose, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]


#-------------------------------------------------------------------
# g100

class L2PhotonHypo_g100 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g100"):
        super( L2PhotonHypo_g100, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV, 97.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

# DC14 - Run1
class L2PhotonHypo_g100_loose1 (L2PhotonHypo_g100):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g100_loose1"):
        super( L2PhotonHypo_g100_loose1, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g100_loose (L2PhotonHypo_g100):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g100_loose"):
        super( L2PhotonHypo_g100_loose, self ).__init__( name )
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g100_etcut (L2PhotonHypo_g100):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g100_etcut"):
        super( L2PhotonHypo_g100_etcut, self ).__init__( name )
        self.EtCutOnly = True        


#-------------------------------------------------------------------
# g105

class L2PhotonHypo_g105 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g105"):
        super( L2PhotonHypo_g105, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV, 102.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

#-------------------------------------------------------------------
#g120

class L2PhotonHypo_g120_loose1 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g120_loose1"):
        super( L2PhotonHypo_g120_loose1, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]

class L2PhotonHypo_g120_loose (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g120_loose"):
        super( L2PhotonHypo_g120_loose, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV, 117.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr      = [0.0202125, 0.0217875, 0.0170625, 0.0186375, 0.0186375, 0.0238875, 0.0270375, 0.0228375, 0.0196875]
        self.CARCOREthr   = [0.862125, 0.857375, 0.852625, 0.847875, 0.7505, 0.833625, 0.857375, 0.843125, 0.800375]


#-------------------------------------------------------------------
# g150

class L2PhotonHypo_g150 (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g150"):
        super( L2PhotonHypo_g150, self ).__init__( name )
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV, 147.*GeV]
        self.CAERATIOthr    = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.HADETthr       = [0.013, 0.013, 0.013, 0.013, 0.013, 0.023, 0.018, 0.017, 0.017]
        self.CARCOREthr     = [0.925, 0.925, 0.92, 0.92, 0.6675, 0.91, 0.9025, 0.895, 0.895]

class L2PhotonHypo_g150_etcut (L2PhotonHypo_g150):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g150_etcut"):
        super( L2PhotonHypo_g150_etcut, self ).__init__( name )
        self.EtCutOnly = True

class L2PhotonHypo_g200_etcut (L2PhotonHypo_g150):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g200_etcut"):
        super( L2PhotonHypo_g200_etcut, self ).__init__( name )
        self.EtCutOnly = True
        self.ETthr          = [195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV, 195.*GeV]
