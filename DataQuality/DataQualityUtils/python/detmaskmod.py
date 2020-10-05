# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import eformat
import operator
from functools import reduce

detmaskmap = {
    'FORWARD_AFP': 'AFP',
    'FORWARD_ALPHA': 'ALFA',
    'FORWARD_BCM': 'IDBCM',                                     
    'FORWARD_LUCID': 'LCD',                                       
    'FORWARD_ZDC': 'ZDC',
    'LAR_EM_BARREL_A_SIDE': 'EMBA',
    'LAR_EM_BARREL_C_SIDE': 'EMBC',
    'LAR_EM_ENDCAP_A_SIDE': 'EMECA',                               
    'LAR_EM_ENDCAP_C_SIDE': 'EMECC',                              
    'LAR_FCAL_A_SIDE': 'FCALA',                              
    'LAR_FCAL_C_SIDE': 'FCALC',                                   
    'LAR_HAD_ENDCAP_A_SIDE': 'HECA', 
    'LAR_HAD_ENDCAP_C_SIDE': 'HECC',                              
    'MUON_CSC_ENDCAP_A_SIDE': 'CSCEA',                              
    'MUON_CSC_ENDCAP_C_SIDE': 'CSCEC',                               
    'MUON_MDT_BARREL_A_SIDE': 'MDTBA',                                
    'MUON_MDT_BARREL_C_SIDE': 'MDTBC',                                
    'MUON_MDT_ENDCAP_A_SIDE': 'MDTEA',
    'MUON_MDT_ENDCAP_C_SIDE': 'MDTEC',                            
    'MUON_RPC_BARREL_A_SIDE': 'RPCBA',                            
    'MUON_RPC_BARREL_C_SIDE': 'RPCBC',                            
    'MUON_TGC_ENDCAP_A_SIDE': 'TGCEA',
    'MUON_TGC_ENDCAP_C_SIDE': 'TGCEC',                            
    'PIXEL_BARREL': 'PIXB',                            
    'PIXEL_B_LAYER': 'PIX0',
    'PIXEL_DISK_SIDE': ('PIXEA', 'PIXEC'),
    ('SCT_BARREL_A_SIDE', 'SCT_BARREL_C_SIDE'): 'SCTB',
    'SCT_ENDCAP_A_SIDE': 'SCTEA',
    'SCT_ENDCAP_C_SIDE': 'SCTEC',
    'TDAQ_CTP': 'L1CTP',
    'TILECAL_BARREL_A_SIDE': 'TILBA',
    'TILECAL_BARREL_C_SIDE': 'TILBC',                                     
    'TILECAL_EXT_A_SIDE': 'TIEBA',                                 
    'TILECAL_EXT_C_SIDE': 'TIEBC',
    ('TRT_BARREL_A_SIDE', 'TRT_BARREL_C_SIDE'): 'TRTB',
    'TRT_ENDCAP_A_SIDE': 'TRTEA',
    'TRT_ENDCAP_C_SIDE': 'TRTEC',
    ('TRT_BARREL_A_SIDE', 'TRT_BARREL_C_SIDE', 'TRT_ENDCAP_A_SIDE', 'TRT_ENDCAP_C_SIDE'): 'TRTTR',
    }

detmaskmap_defects = {
    #'FORWARD_AFP': 'AFP_DISABLED', # not yet functional
    'FORWARD_ALPHA': 'ALFA_DISABLED',
    #'FORWARD_LUCID': 'LCD_DISABLED', # LUCID now operates outside ATLAS partition
    'FORWARD_ZDC': 'ZDC_DISABLED',
    'LAR_EM_BARREL_A_SIDE': 'LAR_EMBA_DISABLED',
    'LAR_EM_BARREL_C_SIDE': 'LAR_EMBC_DISABLED',
    'LAR_EM_ENDCAP_A_SIDE': 'LAR_EMECA_DISABLED',                        
    'LAR_EM_ENDCAP_C_SIDE': 'LAR_EMECC_DISABLED',                              
    'LAR_FCAL_A_SIDE': 'LAR_FCALA_DISABLED',                              
    'LAR_FCAL_C_SIDE': 'LAR_FCALC_DISABLED',                                   
    'LAR_HAD_ENDCAP_A_SIDE': 'LAR_HECA_DISABLED', 
    'LAR_HAD_ENDCAP_C_SIDE': 'LAR_HECC_DISABLED',                              
    'MUON_CSC_ENDCAP_A_SIDE': 'MS_CSC_EA_DISABLED',                            
    'MUON_CSC_ENDCAP_C_SIDE': 'MS_CSC_EC_DISABLED',                           
    'MUON_MDT_BARREL_A_SIDE': 'MS_MDT_BA_DISABLED',                          
    'MUON_MDT_BARREL_C_SIDE': 'MS_MDT_BC_DISABLED',                          
    'MUON_MDT_ENDCAP_A_SIDE': 'MS_MDT_EA_DISABLED',
    'MUON_MDT_ENDCAP_C_SIDE': 'MS_MDT_EC_DISABLED',                            
    'MUON_RPC_BARREL_A_SIDE': 'MS_RPC_BA_DISABLED',                            
    'MUON_RPC_BARREL_C_SIDE': 'MS_RPC_BC_DISABLED',                            
    'MUON_TGC_ENDCAP_A_SIDE': 'MS_TGC_EA_DISABLED',
    'MUON_TGC_ENDCAP_C_SIDE': 'MS_TGC_EC_DISABLED',                            
    'PIXEL_BARREL': 'PIXEL_BARREL_DISABLED',                            
    'PIXEL_B_LAYER': 'PIXEL_LAYER0_DISABLED',
    'PIXEL_DISK_SIDE': 'PIXEL_ENDCAPS_DISABLED',
    ('SCT_BARREL_A_SIDE', 'SCT_BARREL_C_SIDE',
     'SCT_ENDCAP_A_SIDE', 'SCT_ENDCAP_C_SIDE'): 'SCT_GLOBAL_DISABLED',
    'TILECAL_BARREL_A_SIDE': 'TILE_LBA_DISABLED',
    'TILECAL_BARREL_C_SIDE': 'TILE_LBC_DISABLED',                     
    'TILECAL_EXT_A_SIDE': 'TILE_EBA_DISABLED',                                 
    'TILECAL_EXT_C_SIDE': 'TILE_EBC_DISABLED',
    ('TRT_BARREL_A_SIDE', 'TRT_BARREL_C_SIDE',
     'TRT_ENDCAP_A_SIDE', 'TRT_ENDCAP_C_SIDE'): 'TRT_DISABLED',
    }

def getSubDetectorObj(sdstr):
    return getattr(eformat.helper.SubDetector, sdstr)

def decode(mask):
    dm = eformat.helper.DetectorMask(mask)
    rv = []
    for keys, value in detmaskmap.items():
        if isinstance(keys, str):
            keys = [keys]
        if reduce(operator.or_,
                  [dm.is_set(getSubDetectorObj(key)) for key in keys]):
            flags = value
            if isinstance(flags, str):
                flags = [flags]
            rv += list(flags)

    return sorted(rv)

def decodeBlack(mask, defects=False):
    dm = eformat.helper.DetectorMask(mask)
    rv = []
    dmap = detmaskmap if not defects else detmaskmap_defects
    for keys, value in sorted(dmap.items()):
        if isinstance(keys, str):
            keys = [keys]
        if reduce(operator.and_,
                  [not dm.is_set(getSubDetectorObj(key)) for key in keys]):
            flags = value
            if isinstance(flags, str):
                flags = [flags]
            rv += list(flags)

    return sorted(rv)

def getRunMask(run):
    from PyCool import cool
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    # Necessary to work around COOL database lookup stupidity
    db = dbSvc.openDatabase('oracle://ATLAS_COOLPROD;dbname=CONDBR2;schema=ATLAS_COOLONL_TDAQ', True)
    iov = run << 32
    folder = db.getFolder('/TDAQ/RunCtrl/SOR')
    obj = folder.findObject(iov, 0)
    mask = obj.payload()['DetectorMask']
    db.closeDatabase()
    return mask
  
def getNumLumiBlocks(run):
    from DQUtils.db import fetch_iovs
    lbs = fetch_iovs('LBLB', (run, 1), (run, 0xFFFFFFFF), database='oracle://ATLAS_COOLPROD;dbname=CONDBR2;schema=ATLAS_COOLONL_TRIGGER')
    return len(lbs)
    
    
