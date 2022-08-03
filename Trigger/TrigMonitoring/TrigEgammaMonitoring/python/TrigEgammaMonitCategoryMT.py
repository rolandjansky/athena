# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#Categories currently used by offline Egamma TO monitoringMT tool

monitoring_photon = [
    'HLT_g20_tight_L1EM15VHI',
    'HLT_g20_tight_icaloloose_L1EM15VHI',
    'HLT_g25_tight_icaloloose_L1EM22VHI',
    'HLT_g25_tight_icalotight_L1EM22VHI',
    'HLT_g22_tight_L1EM15VHI',
    'HLT_g35_medium_L1EM20VH',
    'HLT_g25_medium_L1EM20VH',
    'HLT_g50_loose_L1EM20VH',
    'HLT_g140_loose_L1EM22VHI',
    'HLT_g300_etcut_L1EM22VHI'
        ]

monitoring_electron = [
    'HLT_e300_etcut_L1EM22VHI',
            ]

monitoringTP_electron = [
    'HLT_e17_lhvloose_L1EM15VHI',
    'HLT_e20_lhvloose_L1EM15VH',
    'HLT_e20_lhloose_L1EM15VH',
    'HLT_e20_lhmedium_L1EM15VH',
    'HLT_e20_lhtight_L1EM15VH',
    'HLT_e20_lhtight_ivarloose_L1EM15VH',
    'HLT_e20_lhtight_ivarloose_L1EM15VHI',
    'HLT_e140_lhloose_L1EM22VHI',
    'HLT_e140_lhloose_noringer_L1EM22VHI',
    'HLT_e60_lhmedium_L1EM22VHI',
    'HLT_e26_lhtight_ivarloose_L1EM22VH',
    'HLT_e26_lhtight_L1EM22VHI',   
    'HLT_e26_lhtight_ivarmedium_L1EM22VHI',
    'HLT_e28_lhtight_ivarloose_L1EM22VHI'
        ]

monitoring_tags = []

# monitoring_tags = [
#     'HLT_e26_lhtight_ivarloose_L1EM22VHI',
#     'HLT_e26_dnntight_ivarloose_L1EM22VHI'
# ]

validation_photon = [
        'HLT_g20_tight_icaloloose_L1EM15VHI',
        'HLT_g25_tight_icaloloose_L1EM22VHI',
        'HLT_g25_tight_icalotight_L1EM22VHI',
        'HLT_g20_medium_L1EM15VHI',
        'HLT_g20_loose_L1EM15VHI',        
        'HLT_g140_loose_L1EM22VHI',
        'HLT_g140_loose_L1eEM26M',
        'HLT_g300_etcut_L1EM22VHI',
        'HLT_g25_tight_icaloloose_L1EM22VHI',
        'HLT_g25_tight_icalotight_L1EM22VHI'
        ]

validation_photon_ringer = [ #this chains will be monitored once photon ringer is updated 
        'HLT_g20_tight_ringer_L1EM15VHI',
        'HLT_g20_medium_ringer_L1EM15VHI',
        'HLT_g20_loose_ringer_L1EM15VHI',
        'HLT_g20_tight_L1EM15VHI',
        'HLT_g20_medium_L1EM15VHI',
        'HLT_g20_loose_L1EM15VHI',
        ]



validation_electron = [
        'HLT_e5_etcut_L1EM3',
        'HLT_e5_lhtight_noringer_L1EM3',
        'HLT_e60_etcut_L1EM22VHI',
        'HLT_e26_etcut_L1EM22VHI',
        'HLT_e300_etcut_L1EM22VHI'
        ]

validationTP_electron = [
        'HLT_e26_lhtight_L1EM22VHI',
        'HLT_e26_lhtight_ivarloose_L1EM15VH',
        'HLT_e26_lhtight_ivarloose_L1EM22VHI',
        'HLT_e26_lhtight_ivarmedium_L1EM22VHI',
        'HLT_e28_lhtight_ivarloose_L1EM22VHI'
        'HLT_e140_lhloose_L1EM22VHI',
        'HLT_e140_lhloose_noringer_L1EM22VHI',
        'HLT_e60_lhmedium_L1EM22VHI',
        ]



validationTP_electron_DNN = [ # will be added when new DNN is ready 
        'HLT_e26_dnnloose_L1EM22VHI',
        'HLT_e26_dnnmedium_L1EM22VHI',
        'HLT_e26_dnntight_L1EM22VHI'
        ]

validationTP_electron_eEM = [
        'HLT_e26_lhtight_L1eEM26',
        'HLT_e26_lhtight_L1eEM26L',
        'HLT_e26_lhtight_ivarloose_L1eEM26',
        'HLT_e26_lhtight_ivarloose_L1eEM26M',
        'HLT_e26_lhtight_ivarloose_L1eEM26T',
        'HLT_e60_lhmedium_L1eEM26M',
        ]


validation_jpsi = [
        'HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
        'HLT_e5_lhtight_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',
        'HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
        'HLT_e5_lhtight_e14_etcut_Jpsiee_L1JPSI-1M5-EM12',
        'HLT_e9_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
        'HLT_e5_lhtight_noringer_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',
        'HLT_e14_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
        'HLT_e5_lhtight_noringer_e14_etcut_Jpsiee_L1JPSI-1M5-EM12'
        ]

validationTP_jpsiee = ['HLT_e5_lhtight_L1EM3']


validation_Zee = [
        'HLT_e26_lhtight_e15_etcut_Zee_L1EM22VHI', 
        'HLT_e26_lhtight_e15_etcut_idperf_Zee_L1EM22VHI', 
        'HLT_e20_lhtight_ivarloose_L1ZAFB-25DPHI-eEM18I'
        ] # adding Zee/special chains later

#
# All single triggers 
#
single_electron_triggers = set(monitoring_electron + monitoringTP_electron + validation_electron + validationTP_electron)
single_photon_triggers = set(monitoring_photon + validation_photon)


#
# Topo chains
#

topo_config = {
                    'Zee'   : {'mass':(50 , 130) , 'dphi':(1.5,   5) },
                    'Jpsiee': {'mass':( 1 ,   5) , 'dphi':(1.5,   5) },
                    'Heg'   : {'mass':(90 , 140) , 'dphi':(1.5,   5) },
              }

monitoring_topo = [

        # HLT_e26_lhtight_e15_etcut_Zee_L1EM22VHI
        {# leg0 e26_lhtight, leg1 e15_etcut
          "trigger_num" : "HLT_e26_lhtight_e15_etcut_Zee_L1EM22VHI",
          "trigger_den" : "HLT_e26_lhtight_e15_etcut_L1EM22VHI",
          "leg0_key"    : "Electrons", # HLT/Offline key
          "leg1_key"    : "Electrons", # HLT/Offline key
          "topo"        : "Zee",
        },

        ]


