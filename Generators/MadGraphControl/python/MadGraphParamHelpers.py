# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def set_SM_params(process_dir,FourFS=False):
    """ Set default SM parameters
    Recommended parameter page from PMG:
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/McProductionCommonParametersMC15
    """
    param_card_settings = {
    'loop' : { '1':"9.118760e+01" },
    'mass' : {
'5':  "0.000000",
'15': "1.777000e+00",
'23': "9.118760e+01",
'24': "8.039900e+01",
'25': "1.250000e+02",
        },
    'yukawa' : { '15': "1.777000e+00 # ymtau" },
    'DECAY' : {
'5'  : """DECAY  5   0.000000e+00""",
'15' : """DECAY  15   0.000000e+00""",
'23' : """DECAY  23   2.495200e+00""",
'24': """DECAY  24   2.085000e+00
    3.377000e-01   2   -1   2
    3.377000e-01   2   -3   4
    1.082000e-01   2  -11  12
    1.082000e-01   2  -13  14
    1.082000e-01   2  -15  16""",
       }
    }
    if FourFS:
        param_card_settings['mass']['5']="4.950000e+00"
    from MadGraphControl.MadGraphUtils import modify_param_card
    modify_param_card(process_dir=process_dir,params=param_card_settings)


def set_top_params(process_dir,mTop=172.5,FourFS=False):
    """ Set default parameters requested by the top group
    This is a convenience helper function
    Recommended parameter page from PMG:
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/McProductionCommonParametersMC15
    """
    # Set SM parameters
    set_SM_params(process_dir,FourFS)
    # Set Higgs parameters
    set_higgs_params(process_dir)
    # Calculate the top width based on the mass
    # From https://gitlab.cern.ch/atlasphys-top/reco/MC/blob/master/MCinfo/get_t_width.py
    import math
    # ATLAS MC11 conventions == PDG2010 
    # Vtb=0.999152 
    # using Vtb=1.0 since part of the inputs was already produced using this approximation 
    Vtb=1.0
    M_W=80.399
    # PDG2010 
    G_F=1.16637*(math.pow(10,-5))
    # MSbar alpha_s(mt)
    alpha_s=0.108
    # Born gamma coeff. 
    C1=G_F/(8*math.pi*math.sqrt(2))
    # Born approximation (taking intermediate W-boson to be on-shell) [1]
    wTop_B=C1*math.pow(float(mTop),3)*math.pow(Vtb,2)*pow((1-math.pow((M_W/float(mTop)),2)),2)*(1+2*pow((M_W/float(mTop)),2))
    # EW and QCD corrections to Born: QCD dominates, EW can be neglected [1],[2],[3]
    wTop=wTop_B*(1-0.81*alpha_s-1.81*pow(alpha_s,2))

    param_card_settings = {
    'mass' : { '6':  str(mTop) },
    'yukawa' : { '6': "1.725000e+02 # ymt" },

    'DECAY' : { '6'  : """DECAY  6   """+str(wTop)+"""
    1.000000e+00   2    5  24 # 1.32""",
        },
    }
    from MadGraphControl.MadGraphUtils import modify_param_card
    modify_param_card(process_dir=process_dir,params=param_card_settings)


def set_higgs_params(process_dir):
    """ Set Higgs mass and decays
    BR from the Higgs XSec working group for a 125.0 GeV Higgs
    https://twiki.cern.ch/twiki/pub/LHCPhysics/LHCHXSWGBRs/BR.central.dat
    """
    param_card_settings = {
     'MASS' : { '25': "1.250000e+02" },
     'DECAY': { '25': """DECAY  25   6.382339e-03
     5.767E-01 2   5  -5 # H->bb
     6.319E-02 2  15 -15 # H->tautau
     2.192E-04 2  13 -13 # H->mumu
     2.462E-04 2   3  -3 # H->ss
     2.911E-02 2   4  -4 # H->cc
     8.569E-02 2  21  21 # H->gg
     2.277E-03 2  22  22 # H->gammagamma
     1.539E-03 2  22  23 # H->Zgamma
     2.146E-01 2  24 -24 # H->WW
     2.641E-02 2  23  23 # H->ZZ""" }
     }
    from MadGraphControl.MadGraphUtils import modify_param_card
    modify_param_card(process_dir=process_dir,params=param_card_settings)