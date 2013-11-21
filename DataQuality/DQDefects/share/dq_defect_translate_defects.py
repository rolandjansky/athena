#!/usr/bin/env python

def copyfunc(indb, outdb, func_iov, func_output, runends):
    '''
    indb = input database
    outdb = output database
    func_iov = function that takes a DB connection and returns some IOVs
               (includes folder/channel selection and any filtering)
    func_output = takes IOVs and writes them to the output DB connection
    '''
    iovs = func_iov(indb)
    iovs = truncate_iovs_to_runs(iovs, runends)
    iovs = filter(lambda x: not(x.until.lumi-x.since.lumi == 0), iovs)
    print 'There are %d IOVs to process' % len(iovs)
    ## if len(iovs) < 10:
    ##     print iovs
    for i, iov in enumerate(iovs):
        func_output(outdb, iov)
        if i % 100 == 1:
            print 'IOV', i

def truncate_iovs_to_runs(iovs, runends):
    for iov in iovs:
        if iov.since.run not in runends:
            print 'Missing run', iov.since.run, 'from EOR_Params'
            
    return filter(None,
                  [iov.intersect(runends[iov.since.run])
                   for iov in iovs if iov.since.run in runends])
   
def get_runends():
    from DQUtils.db import fetch_iovs
    eor_params = fetch_iovs('EOR_Params', 
                             with_channel=False, what=[])
    runends = {}
    for iov in eor_params:
        runends[iov.since.run] = iov
    return runends

def dq_code_filter(foldername, tag, channel, codes):
    from DQUtils.db import fetch_iovs
    def dq_code_filter_internals(indb):
        folder = indb.getFolder(foldername)
        iovs = fetch_iovs(folder, channels=[channel], tag=tag)
        iovs = [iov for iov in iovs if iov.Code in codes]
        return iovs
    return dq_code_filter_internals

def defect_output(channel):
    def defect_output_internals(outdb, iov):
        db = outdb
        db.insert(channel, iov.since, iov.until,
                  (iov.Comment if 'Comment' in dir(iov) else 'Automatic'),
                  'sys:copy')
    return defect_output_internals

def atlas_notready_filter(indb):
    from DQUtils.db import fetch_iovs
    folder = indb.getFolder('/TDAQ/RunCtrl/DataTakingMode')
    iovs = fetch_iovs(folder, channels=[0])
            
    return filter(None, [iov for iov in iovs if iov.ReadyForPhysics == 0])

if __name__ == '__main__':
    from DQUtils.db import Databases
    from DQDefects import DefectsDB

    TRIPLES = [
        ('ATLTOR', [-1], 'GLOBAL_TOROID_OFF'),
        ('ATLTOR', [1], 'GLOBAL_TOROID_RAMPING'),
        ('ATLSOL', [-1], 'GLOBAL_SOLENOID_OFF'),
        ('ATLSOL', [1], 'GLOBAL_SOLENOID_RAMPING'),
        ('PIX0', [-1], 'PIXEL_LAYER0_DISABLED'),
        ('PIXB', [-1], 'PIXEL_BARREL_DISABLED'),
        ('PIXEA', [-1], 'PIXEL_ENDCAPS_DISABLED'),
        ('PIXEC', [-1], 'PIXEL_ENDCAPS_DISABLED'),
        ('PIX0', [-1, 0, 1, 2], 'PIXEL_LAYER0_2010NONGREEN'),
        ('PIXB', [-1, 0, 1, 2], 'PIXEL_BARREL_2010NONGREEN'),
        ('PIXEA', [-1, 0, 1, 2], 'PIXEL_ENDCAPA_2010NONGREEN'),
        ('PIXEC', [-1, 0, 1, 2], 'PIXEL_ENDCAPC_2010NONGREEN'),
        ('SCTB', [-1], 'SCT_GLOBAL_DISABLED'),
        ('SCTEA', [-1], 'SCT_GLOBAL_DISABLED'),
        ('SCTEC', [-1], 'SCT_GLOBAL_DISABLED'),
        ('SCTB', [-1, 0, 1, 2], 'SCT_SCTB_2010NONGREEN'),
        ('SCTEA', [-1, 0, 1, 2], 'SCT_SCTEA_2010NONGREEN'),
        ('SCTEC', [-1, 0, 1, 2], 'SCT_SCTEC_2010NONGREEN'),
        ('TRTTR', [-1, 0, 1, 2], 'TRT_TRTTR_2010NONGREEN'),
        ('ATLGL', [1], 'GLOBAL_NOTCONSIDERED'),
        ('RCOPS', [-1, 0, 1, 2], 'GLOBAL_RCOPS_2010NONGREEN'),
        ('TILBA', [-1], 'TILE_LBA_DISABLED'),
        ('TILBC', [-1], 'TILE_LBC_DISABLED'),
        ('TIEBA', [-1], 'TILE_EBA_DISABLED'),
        ('TIEBC', [-1], 'TILE_EBC_DISABLED'),
        ('TILBA', [-1,0,1,2], 'TILE_LBA_2010NONGREEN'),
        ('TILBC', [-1,0,1,2], 'TILE_LBC_2010NONGREEN'),
        ('TIEBA', [-1,0,1,2], 'TILE_EBA_2010NONGREEN'),
        ('TIEBC', [-1,0,1,2], 'TILE_EBC_2010NONGREEN'),
        ('ALFA', [-1], 'ALFA_DISABLED'),
        ('MBTSA', [-1], 'MBTS_A_DISABLED'),
        ('MBTSC', [-1], 'MBTS_C_DISABLED'),
        ('LCDA', [-1,0,1,2], 'LCD_LCDA_2010NONGREEN'),
        ('LCDC', [-1,0,1,2], 'LCD_LCDC_2010NONGREEN'),
        ('LUMI', [-1,0,1,2], 'LUMI_LUMI_2010NONGREEN'),
        ('LUMIONL', [-1,0,1,2], 'LUMI_LUMIONL_2010NONGREEN'),
        ('IDBS', [1], 'ID_BS_2010RED'),
        ('IDBS', [2], 'ID_BS_2010YELLOW'),
        ('IDVX', [-1,0,1,2], 'ID_VX_2010NONGREEN'),
        ('IDGL', [-1,0,1,2], 'ID_GL_2010NONGREEN'),
        ('IDAL', [-1,0,1,2], 'ID_AL_2010NONGREEN'),
        ('L1CTP', [1], 'TRIG_L1_CTP_CLOCK'),
        ('EIDCR', [-1,0,1,2], 'EGAMMA_EIDCR_2010NONGREEN'),
        ('EMBA', [-1,0,1,2], 'LAR_EMBA_2010NONGREEN'),
        ('EMBC', [-1,0,1,2], 'LAR_EMBC_2010NONGREEN'),
        ('EMECA', [-1,0,1,2], 'LAR_EMECA_2010NONGREEN'),
        ('EMECC', [-1,0,1,2], 'LAR_EMECC_2010NONGREEN'),
        ('HECA', [-1,0,1,2], 'LAR_HECA_2010NONGREEN'),
        ('HECC', [-1,0,1,2], 'LAR_HECC_2010NONGREEN'),
        ('FCALA', [-1,0,1,2], 'LAR_FCALA_2010NONGREEN'),
        ('FCALC', [-1,0,1,2], 'LAR_FCALC_2010NONGREEN'),
        ('MDTBA', [-1,0,1,2], 'MS_MDT_BA_2010NONGREEN'),
        ('MDTBC', [-1,0,1,2], 'MS_MDT_BC_2010NONGREEN'),
        ('MDTEA', [-1,0,1,2], 'MS_MDT_EA_2010NONGREEN'),
        ('MDTEC', [-1,0,1,2], 'MS_MDT_EC_2010NONGREEN'),
        ('RPCBA', [-1,0,1,2], 'MS_RPC_BA_2010NONGREEN'),
        ('RPCBC', [-1,0,1,2], 'MS_RPC_BC_2010NONGREEN'),
        ('CSCEA', [-1,0,1,2], 'MS_CSC_EA_2010NONGREEN'),
        ('CSCEC', [-1,0,1,2], 'MS_CSC_EC_2010NONGREEN'),
        ('TGCEA', [-1,0,1,2], 'MS_TGC_EA_2010NONGREEN'),
        ('TGCEC', [-1,0,1,2], 'MS_TGC_EC_2010NONGREEN'),
        ('ZDC', [-1,0,1,2], 'ZDC_2010NONGREEN'),
        ('TRTB', [-1,0,1,2], 'TRT_TRTB_2010NONGREEN'),
        ('TRTEA', [-1,0,1,2], 'TRT_TRTEA_2010NONGREEN'),
        ('TRTEC', [-1,0,1,2], 'TRT_TRTEC_2010NONGREEN'),
        ('L1CAL', [-1,0,1,2], 'TRIG_L1_CAL_2010NONGREEN'),
        ('TRBJT', [-1,0,1,2], 'TRIG_HLT_BJT_2010NONGREEN'),
        ('TRBPH', [-1,0,1,2], 'TRIG_HLT_BPH_2010NONGREEN'),
        ('TRCAL', [-1,0,1,2], 'TRIG_HLT_CAL_2010NONGREEN'),
        ('TRELE', [-1,0,1,2], 'TRIG_HLT_ELE_2010NONGREEN'),
        ('TRGAM', [-1,0,1,2], 'TRIG_HLT_GAM_2010NONGREEN'),
        ('TRIDT', [-1,0,1,2], 'TRIG_HLT_IDT_2010NONGREEN'),
        ('TRJET', [-1,0,1,2], 'TRIG_HLT_JET_2010NONGREEN'),
        ('TRMBI', [-1,0,1,2], 'TRIG_HLT_MBI_2010NONGREEN'),
        ('TRMET', [-1,0,1,2], 'TRIG_HLT_MET_2010NONGREEN'),
        ('TRMUO', [-1,0,1,2], 'TRIG_HLT_MUO_2010NONGREEN'),
        ('TRTAU', [-1,0,1,2], 'TRIG_HLT_TAU_2010NONGREEN'),
        ('TRCOS', [-1,0,1,2], 'TRIG_HLT_COS_2010NONGREEN'),
        ('TAUB', [-1,0,1,2], 'TAU_B_2010NONGREEN'),
        ('TAUCR', [-1,0,1,2], 'TAU_CR_2010NONGREEN'),
        ('TAUE', [-1,0,1,2], 'TAU_E_2010NONGREEN'),
        ('BTGLIFE', [-1,0,1,2], 'BTAG_BTGLIFE_2010NONGREEN'),
        ('BTGSOFTE', [-1,0,1,2], 'BTAG_BTGSOFTE_2010NONGREEN'),
        ('BTGSOFTM', [-1,0,1,2], 'BTAG_BTGSOFTM_2010NONGREEN'),
        ('MMUIDVX', [-1,0,1,2], 'MCP_MMUIDVX_2010NONGREEN'),
        ]

    runends = get_runends()
    outdb = DefectsDB('defectstest.db/COMP200', read_only=False)
    with outdb.storage_buffer:
        for trip in TRIPLES:
            print trip
            copyfunc(Databases.get_instance('COOLOFL_GLOBAL/COMP200'),
                     outdb,
                     dq_code_filter('/GLOBAL/DETSTATUS/LBSUMM',
                                    'DetStatusLBSUMM-repro05-01', trip[0], trip[1]),
                     defect_output(trip[2]), runends)
            copyfunc(Databases.get_instance('COOLOFL_GLOBAL/COMP200'),
                     outdb,
                     dq_code_filter('/GLOBAL/DETSTATUS/LBSUMM',
                                    'DetStatusLBSUMM-pass1-analysis-2010J-01', trip[0], trip[1]),
                     defect_output(trip[2]), runends)


        copyfunc(Databases.get_instance('COOLONL_TDAQ/COMP200'),
                 outdb,
                 atlas_notready_filter,
                 defect_output('GLOBAL_NOTREADY'), runends)
