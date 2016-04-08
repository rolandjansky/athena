# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Bphysics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold'
__version__="$Revision: 1.30 $"
__doc__="Bphysics slice specific flags  "


_flags = []

class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)



class signatures(JobProperty):
    """ signatures in Bphysics slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        ##FTK
        '2mu6_DiMu_FTK',
        "mu6_DiMu_FS_FTK",
        #
        "mu4Tiimu6ii_DiMu_DY",
        "mu4Tiimu6ii_DiMu_DY14_noVtx_noOS",
        "mu4Tiimu6ii_DiMu_DY20",
        "mu4_DiMu",
        "mu4_DiMu_noOS",
        "mu4_Jpsimumu",
        "mu4_Jpsimumu_FS",
        "mu4_Jpsimumu_tight",
        "mu6_Jpsimumu_tight",
        "mu4_Bmumu",
        "mu6_Upsimumu_FS",
        "mu6_DiMu",
        "mu6_Bmumu",
        "mu4_BmumuX",
        "mu6_BmumuX",
        "mu10_Upsimumu_FS",
        "mu10_Jpsimumu",
        "mu4_Upsimumu_tight_FS",
        "mu6_Upsimumu_tight_FS",
        "mu4_DiMu_MG",
        "mu4_DiMu_MG_FS",
        "mu4_DiMu_SiTrk",
        "mu4_DiMu_SiTrk_FS",
        "mu4_DiMu_FS",
        "2mu4_DiMu_noVtx",
        "2mu6_Bmumu",
        "2mu6_Jpsimumu",
        "2mu6_Upsimumu",
        "2mu6_Bmumux",
        "mu4mu6_Jpsimumu_IDTrkNoCut",
        "mu4_Trk_Jpsi_loose",
        "mu4_Trk_Jpsi_looser",
        "mu4mu6_DiMu_DY",
        "2mu6_DiMu_DY20",
        "2mu6_DiMu_DY25",
        "2mu6i_DiMu_DY",
        "2mu4i_DiMu_DY_noVtx_noOS",
        "mu4imu6i_DiMu_DY_noVtx_noOS",
        "mu4mu6_DiMu_DY_noVtx_noOS",
        "2mu6i_DiMu_DY_noVtx_noOS",
        "mu6_DiMu_noOS",
        "2mu4_DiMu",
        "2mu4_DiMu_SiTrk",
        "2mu4_Jpsimumu",
        "2mu4_Upsimumu",
        "2mu4_Bmumu",
        "2mu4_Bmumux",
        "mu6_Jpsimumu",
        "mu6_Jpsimumu_SiTrk",
        "mu4_Trk_Jpsi",
        "mu6_Trk_Jpsi_loose",
        "mu4_Upsimumu_FS",
        "mu10_Upsimumu_tight_FS",
        "2mu4_DiMu_noVtx_noOS",
        "mu4_DiMu_FS_noOS",
        "mu4_Upsimumu_SiTrk_FS",
        "2mu4_Jpsimumu_IDTrkNoCut",
        "mu4_Trk_Upsi_FS",
        "2mu13_Zmumu_IDTrkNoCut", "mu4mu6_Jpsimumu",
        "mu4mu6_Upsimumu", "mu4mu6_DiMu", "mu4mu6_Bmumu",
        "mu4mu6_Bmumux", "mu4mu6_DiMu_noVtx_noOS",
        "2mu6_DiMu", "2mu4_DiMu_DY", "2mu4_DiMu_DY20",
        "2mu4i_DiMu_DY", "2mu4_DiMu_DY_noVtx_noOS",
        "mu4imu6i_DiMu_DY", "mu4mu6_DiMu_DY20",
        "mu4imu6i_DiMu_DY14_noVtx_noOS", "3mu4_DiMu",
        "3mu4_Jpsi", "3mu4_Upsi", "3mu4_Tau",
        
        "mu6_DiMu_noOS", "mu4T_Jpsimumu", "mu4T_DiMu",
        "mu4T_DiMu_FS_noOS", "2mu4T_DiMu", "2mu4T_DiMu_SiTrk",
        "2mu4T_Jpsimumu", "2mu4T_Upsimumu",
        "2mu4T_Bmumu", "2mu4T_Bmumux", "mu4T_Trk_Jpsi",
        "mu4T_Upsimumu_FS",
        "mu4T_Upsimumu_tight_FS", "2mu4T_DiMu_noVtx_noOS",
        "mu4T_Upsimumu_SiTrk_FS", "2mu4T_Jpsimumu_IDTrkNoCut","2mu4T_Jpsimumu_IDTrkNoCut_R1",
        "mu4Tmu6_Jpsimumu_IDTrkNoCut", "mu4T_Trk_Upsi_FS",
        "mu4Tmu6_Jpsimumu", "mu4Tmu6_Upsimumu",
        "mu4Tmu6_DiMu", "mu4Tmu6_Bmumu", "mu4Tmu6_Bmumux",
        "mu4Tmu6_DiMu_noVtx_noOS", "2mu4T_DiMu_DY",
        "2mu4T_DiMu_DY20", "2mu4Ti_DiMu_DY",
        "2mu4T_DiMu_DY_noVtx_noOS", "mu4Timu6i_DiMu_DY",
        "mu4Tmu6_DiMu_DY20", "mu4Timu6i_DiMu_DY14_noVtx_noOS",

        "3mu4T_DiMu", "3mu4T_Jpsi", "3mu4T_Upsi", "3mu4T_Tau",
        
        '2mu4T_DiMu_BarrelOnly', '2mu4T_DiMu_Barrel', 'mu4Tmu6_DiMu_Barrel',
        '2mu4T_Bmumu_BarrelOnly', '2mu4T_Bmumu_Barrel', 'mu4Tmu6_Bmumu_Barrel',
        '2mu4T_Jpsimumu_BarrelOnly', '2mu4T_Jpsimumu_Barrel', 'mu4Tmu6_Jpsimumu_Barrel',
        '2mu4T_Upsimumu_BarrelOnly', '2mu4T_Upsimumu_Barrel', 'mu4Tmu6_Upsimumu_Barrel',
        '2mu4T_Bmumux_BarrelOnly', '2mu4T_Bmumux_Barrel', 'mu4Tmu6_Bmumux_Barrel',                  
        '2mu6_DiMu_noVtx_noOS',
        '2mu4T_DiMu_l2muonSA',
        '2mu6_Bmumu_Lxy',
        '2mu4T_Bmumu_Lxy',
        'mu4Tmu6_Bmumu_Lxy',
	'2mu4T_Bmumu_Lxy_BarrelOnly', '2mu4T_Bmumu_Lxy_Barrel', 'mu4Tmu6_Bmumu_Lxy_Barrel',
        '2mu4T_Jpsimumu_prescaled',
        'mu4Tmu6_Jpsimumu_prescaled',
        '2mu4T_Jpsimumu_BarrelOnly_prescaled',
        '2mu4T_Jpsimumu_Barrel_prescaled',
        'mu4Tmu6_Jpsimumu_Barrel_prescaled',


        'mu6_Jpsimumu_L2StarA',
        'mu6_Jpsimumu_L2StarB',
        "mu10_Jpsimumu_L2StarA",
        "2mu6_Bmumux_L2StarA",
        "2mu6_Bmumux_L2StarB",
        "mu6_Trk_Jpsi_loose_L2StarA",
        "mu6_Trk_Jpsi_loose_L2StarB",
        '2mu4T_DiMu_L2StarA',
        '2mu4T_DiMu_L2StarB',
        '2mu4T_DiMu_L2StarC',
        'mu10_Upsimumu_tight_FS_L2StarA',

        #test chains
        '2mu4T_Bmumux_v2',
        'mu4Tmu6_Bmumux_v2',
        '2mu6_Bmumux_v2',

        '2mu6_Bmumux_v2_L2StarA',
        '2mu6_Bmumux_v2_L2StarB',
        '2mu4T_Bmumux_v2_Barrel',
        '2mu4T_Bmumux_v2_BarrelOnly',
        'mu4Tmu6_Bmumux_v2_Barrel',

        '2mu4T_DiMu_muFast',

        '2mu4T_Jpsimumu_L2StarB',
        '2mu4T_Jpsimumu_Barrel_L2StarB',
        '2mu4T_Jpsimumu_BarrelOnly_L2StarB',
        'mu4Tmu6_Jpsimumu_L2StarB',
        '2mu6_Jpsimumu_L2StarB',

        '2mu4T_Bmumu_L2StarB',
        '2mu4T_Bmumu_Barrel_L2StarB',
        '2mu4T_Bmumu_BarrelOnly_L2StarB',
        'mu4Tmu6_Bmumu_L2StarB',
        '2mu6_Bmumu_L2StarB',

        '2mu4T_Bmumux_v2_L2StarB',
        '2mu4T_Bmumux_Barrel_v2_L2StarB',
        '2mu4T_Bmumux_BarrelOnly_v2_L2StarB',
        'mu4Tmu6_Bmumux_v2_L2StarB',
        'mu4Tmu6_Bmumux_Barrel_v2_L2StarB',
        '2mu6_Bmumux_v2_L2StarB',

        '2mu4T_DiMu_noVtx_noOS_L2StarB', 
        '2mu6_DiMu_noVtx_noOS_L2StarB', 
        '2mu6_DiMu_L2StarB',

        'mu6_Jpsimumu_tight_L2StarB',


        'mu4Tmu6_Bmumu_Barrel_L2StarB',
        'mu4Tmu6_Jpsimumu_Barrel_L2StarB',
        'mu4Tmu6_Jpsimumu_L2StarB_prescaled',
        '2mu4T_Jpsimumu_L2StarB_prescaled',
        '2mu4T_Jpsimumu_Barrel_L2StarB_prescaled',
        '2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled',
        'mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled',

        '2mu4T_Upsimumu_L2StarB',
        '2mu4T_Bmumux_L2StarB',

        '2mu4T_Upsimumu_Barrel_L2StarB', 
        '2mu4T_Upsimumu_BarrelOnly_L2StarB', 
        'mu4Tmu6_Upsimumu_L2StarB', 
        'mu4Tmu6_Upsimumu_Barrel_L2StarB', 
        '2mu6_Upsimumu_L2StarB', 

        '2mu6_Bmumux_BcMuMuDs_loose_L2StarB',
        '2mu6_Bmumux_BcMuMuDs_tight_L2StarB', 
        '2mu6_Bmumux_v3_loose_L2StarB',
        '2mu6_Bmumux_v3_tight_L2StarB',
        
        ])
    
    StoredValue   = [
        ]




_flags.append(signatures)



# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class BphysicsSlice(JobPropertyContainer, CommonSliceHelper):
    """ Bphysics Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BphysicsSlice)

# add add common slice flags
TriggerFlags.BphysicsSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BphysicsSlice.add_JobProperty(flag)
del _flags

# make an alias
BphysicsSliceFlags = TriggerFlags.BphysicsSlice

