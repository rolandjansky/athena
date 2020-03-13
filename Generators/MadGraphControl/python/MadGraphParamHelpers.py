# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def set_top_params(process_dir,FourFS=False):
    """ Set default parameters requested by the top group
    This is a convenience helper function
    """
    param_card_settings = {
    'loop' : {
'1':"9.118760e+01",
        },

    'mass' : {
'5':  "0.000000",
'6':  "1.725000e+02",
'15': "1.777000e+00",
'23': "9.118760e+01",
'24': "8.039900e+01",
'25': "1.250000e+02",
        },

    'yukawa' : {
'6': "1.725000e+02 # ymt",
'15': "1.777000e+00 # ymtau",
        },

    'DECAY' : {
'5'  : """DECAY  5   0.000000e+00""",

'6'  : """DECAY  6   1.320000e+00
    1.000000e+00   2    5  24 # 1.32""",

'15' : """DECAY  15   0.000000e+00""",

'23' : """DECAY  23   2.495200e+00""",

'24': """DECAY  24   2.085000e+00
    3.377000e-01   2   -1   2
    3.377000e-01   2   -3   4
    1.082000e-01   2  -11  12
    1.082000e-01   2  -13  14
    1.082000e-01   2  -15  16""",

'25' : """DECAY  25   6.382339e-03""",
        },
    }
    if FourFS: param_card_settings['mass']['5']="4.500000e+00"
    from MadGraphControl.MadGraphUtils import modify_param_card
    modify_param_card(process_dir=process_dir,params=param_card_settings)