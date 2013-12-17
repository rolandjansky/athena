# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigCostBase.TrigCostBaseConfig import AlgConfig, getLog

clog = getLog('TrigCostExecConfig')

#-----------------------------------------------------------------------------
def _setBoolKey(reg, key, value):
    if value: reg.Set(key, "yes")
    else:     reg.Set(key, "no")

#-----------------------------------------------------------------------------
def _makeTimes(alg_name, options):
    if   alg_name.count('L2'): level = 2
    elif alg_name.count('EF'): level = 3
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    study = AlgConfig(alg_name, 'StudyTime')

    study.SetKey("Level", level)
    study.SetKey("DirName", alg_name)    
    study.SetKey("HistOpts", "logy rebin")
    
    study.SetPar("TrigTimeAlg::HistOpts",    "logy rebin")
    study.SetPar("TrigTimeAlg::Hists::Save", "yes")    
    study.SetPar("StudyTime::SlowNEvent",    1000)
    study.SetPar("TrigTimeROB::CostData",    "makeCount")
    study.SetPar("TrigTimeROB::CoolData",    "readCool")

    filename = (options.root_file)[(options.root_file).rfind('/'):]
    study.SetPar("StudyTime::RootFile",    filename)

    if options.test.count("run-corr"):
        study.SetPar("StudyTime::RunCorr",   True)
        study.SetPar("TrigTimeAlg::RunCorr", True)
    else:
        study.SetPar("StudyTime::RunCorr",   False)
        study.SetPar("TrigTimeAlg::RunCorr", False)
        
    if level == 2: study.SetKey("Timeout",  500.0)
    if level == 3: study.SetKey("Timeout", 5000.0)

    return study

#-----------------------------------------------------------------------------
def _makeEnablePrefetch(alg_name, options):

    prefet = AlgConfig(alg_name, 'EnablePrefetch')

    prefet.SetKey("Level", 2)
    prefet.SetKey("DirName", alg_name)    

    if options.test.count('noPrefetch'):
        prefet.SetKey("PrefetchType", 'no')
    elif options.test.count('newPrefetch'):
        prefet.SetKey("PrefetchType", 'new')
    else:
        prefet.SetKey("PrefetchType", 'old')
        
    return prefet

#-----------------------------------------------------------------------------
def _makeExecs(alg_name, options, minimal_config = False):
    
    if   alg_name.count('L2'):
        level = 2
        lvl   = "L2"
    elif alg_name.count('EF'):
        level = 3
        lvl   = "EF"
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    alg = AlgConfig(alg_name, 'MakeExecs')

    alg.SetKey('Level',        level)
    alg.SetKey('DirName',      alg_name)
    alg.SetKey('WrtLB',        options.wrt_lb)
    alg.SetKey('NLBFullHtml',  options.full_lb)
    alg.SetKey('CostEvtPassL2',options.cost_pass_l2)

    root_file = options.root_file[options.root_file.rfind('/')+1:]
    print "ROOTFILE NAME:",root_file

    if len(options.root_file) > 5: alg.SetKey("RootFile", root_file)
        
    import ROOT
    tables_all = {}
    tables_lbs = {}
    survey     = {}
    MakeHist='yes'
    ###
    ### Prepare module for algorithm execution analysis
    ###
    table_alg = ROOT.Anp.Registry()
    table_alg.Set('ClientType', 'ExecTableAlg')
    table_alg.Set('ClientLink', 'algorithm')
    table_alg.Set('Level',      level)
    table_alg.Set('HistOpt',    'logy rebin png')
    table_alg.Set('HistKey',    'ExecTableAlg')
    table_alg.Set('FillNode',   'yes')
    table_alg.Set('MakeHist',   MakeHist)
    
    _setBoolKey(table_alg, 'Debug', options.debug)

    if   level == 2: table_alg.Set('Timeout', 500.0)        
    elif level == 3: table_alg.Set('Timeout', 5000.0)
    
    if not options.do_bg or lvl=='EF':
        tables_all['tableAlg'+lvl] = table_alg
        tables_lbs['tableAlg'+lvl] = table_alg

    ###
    ### Prepare module for ROS request analysis
    ###
    if level == 2:
        
        table_ros = ROOT.Anp.Registry()
        table_ros.Set('ClientType', 'ExecTableROS')
        table_ros.Set('ClientLink', 'ROS')
        table_ros.Set('Level',      level)
        table_ros.Set('HistOpt',    'rebin png')
        table_ros.Set('HistKey',    'ExecTableROS')
        table_ros.Set('FillNode',   'yes')
        table_ros.Set('MakeHist',   MakeHist)
        table_ros.Set('RoiPlots',   'yes')
        table_ros.Set('Timeout',    500.0)        
        _setBoolKey(table_ros, 'Debug', options.debug)

        if not options.do_bg:
            tables_all['tableROS'+lvl] = table_ros
            tables_lbs['tableROS'+lvl] = table_ros

        table_robin = ROOT.Anp.Registry()
        table_robin.Set('ClientType', 'ExecTableROBIN')
        table_robin.Set('ClientLink', 'ROBIN')
        table_robin.Set('Level',      level)
        table_robin.Set('HistOpt',    'rebin png')
        table_robin.Set('HistKey',    'ExecTableROBIN')
        table_robin.Set('FillNode',   'yes')
        table_robin.Set('MakeHist',   MakeHist)
        table_robin.Set('RoiPlots',   'yes')
        table_robin.Set('Timeout',    500.0)        
        _setBoolKey(table_robin, 'Debug', options.debug)
        if not options.do_bg:
            tables_all['tableROBIN'+lvl] = table_robin
            tables_lbs['tableROBIN'+lvl] = table_robin        

    ###
    ### Prepare module for ROS request prediction analysis
    ###
    if level == 2 and options.do_bg:
        
        table_pred = ROOT.Anp.Registry()
        table_pred.Set('ClientType', 'ExecTablePred')
        table_pred.Set('ClientLink', 'ROSPrediction')
        table_pred.Set('Level',      level)
        table_pred.Set('HistOpt',    'rebin png')
        table_pred.Set('HistKey',    'ExecTablePred')
        table_pred.Set('FillNode',   'yes')
        table_pred.Set('MakeHist',   MakeHist)
        table_pred.Set('ScaleBG',    'scaleBG')
        table_pred.Set('RoiPlots',   'yes')
        table_pred.Set('PassLB',     'yes')
        table_pred.Set('Timeout',    500.0)
        if level == 3:
            table_pred.Set('Timeout',    5000.0)
        _setBoolKey(table_pred, 'Debug', options.debug)
        
        tables_all['tablePred'+lvl] = table_pred
        
    ###
    ### Prepare module for Algorithm prediction analysis
    ###
    if options.do_bg:
        table_pred_alg = ROOT.Anp.Registry()
        table_pred_alg.Set('ClientType', 'ExecTablePredAlg')
        table_pred_alg.Set('ClientLink', 'ALGPrediction')
        table_pred_alg.Set('Level',      level)
        table_pred_alg.Set('HistOpt',    'rebin png')
        table_pred_alg.Set('HistKey',    'ExecTablePredAlg')
        table_pred_alg.Set('FillNode',   'yes')
        table_pred_alg.Set('MakeHist',   MakeHist)
        table_pred_alg.Set('ScaleBG',    'scaleBG')
        table_pred_alg.Set('RoiPlots',   'yes')
        table_pred_alg.Set('PassLB',     'yes')
        table_pred_alg.Set('Timeout',    500.0) 
        if level == 3:
            table_pred_alg.Set('Timeout',    5000.0)     
        _setBoolKey(table_pred_alg, 'Debug', options.debug)
        
        tables_all['tablePred_alg'+lvl] = table_pred_alg

    ###
    ### Prepare module for chain execution analysis
    ###
    table_chn = ROOT.Anp.Registry()
    table_chn.Set('ClientType', 'ExecTableChn')
    table_chn.Set('ClientLink', 'chain')
    table_chn.Set('Level',      level)
    table_chn.Set('HistOpt',    'logy rebin png')
    table_chn.Set('HistKey',    'ExecTableChn')
    table_chn.Set('FillNode',   'yes')
    table_chn.Set('MakeHist',   MakeHist)
    _setBoolKey(table_chn, 'Debug', options.debug)

    if   level == 2: table_chn.Set('Timeout', 500.0)        
    elif level == 3: table_chn.Set('Timeout', 5000.0)
    if not options.do_bg or lvl=='EF':
        tables_all['tableChn'+lvl] = table_chn
        tables_lbs['tableChn'+lvl] = table_chn

    ###
    ### Prepare module for chain execution analysis
    ###
    table_seq = ROOT.Anp.Registry()
    table_seq.Set('ClientType', 'ExecTableSeq')
    table_seq.Set('ClientLink', 'sequence')
    table_seq.Set('Level',      level)
    table_seq.Set('HistOpt',    'logy rebin png')
    table_seq.Set('HistKey',    'ExecTableSeq')
    table_seq.Set('FillNode',   'yes')
    table_seq.Set('MakeHist',   MakeHist)
    _setBoolKey(table_seq, 'Debug', options.debug)

    if   level == 2: table_seq.Set('Timeout', 500.0)        
    elif level == 3: table_seq.Set('Timeout', 5000.0)
    if not options.do_bg:
        tables_all['tableSeq'+lvl] = table_seq
        tables_lbs['tableSeq'+lvl] = table_seq    

    ###
    ### Prepare module for sub detector event execution analysis
    ###
    if level == 2:
        
        table_det = ROOT.Anp.Registry()
        table_det.Set('ClientType', 'ExecTableDet')
        table_det.Set('ClientLink', 'SubDetector')
        table_det.Set('Level',      level)
        table_det.Set('HistOpt',    'rebin png')
        table_det.Set('HistKey',    'ExecTableDet')
        table_det.Set('FillNode',   'yes')
        table_det.Set('MakeHist',   MakeHist)
        _setBoolKey(table_det, 'Debug', options.debug)
        
        tables_all['tableDet'+lvl] = table_det
        #tables_lbs['tableDet'+lvl] = table_det

    ###
    ### Prepare module for mini event execution analysis
    ###
    if level == 2 and options.mini:
        table_mini = ROOT.Anp.Registry()
        table_mini.Set('ClientType',     'ExecMiniEvents')
        table_mini.Set('ClientLink',     'mini')
        table_mini.Set('Level',          level)
        _setBoolKey(table_mini, 'Debug', options.debug)
    
        tables_all['tableMini'+lvl] = table_mini

    ###
    ### Prepare module for slow event execution analysis
    ###
    table_slow = ROOT.Anp.Registry()
    table_slow.Set('ClientType',     'ExecTableSlow')
    table_slow.Set('ClientLink',     'slow')
    table_slow.Set('Level',          level)
    table_slow.Set('HistOpt',        'logy rebin png')
    table_slow.Set('HistKey',        'ExecTableSlow')
    table_slow.Set('FillNode',       'yes')
    table_slow.Set('MakeHist',       'no')
    table_slow.Set('NumSlowEvt',     500)
    table_slow.Set('DetailSlowEvt',  200)
    table_slow.Set('CollectSlowest', 'yes')
    _setBoolKey(table_slow, 'PassIPage', options.debug_stream)
    _setBoolKey(table_slow, 'WriteSeq',  options.write_seq)
    _setBoolKey(table_slow, 'Debug',     options.debug)

    if   level == 2: table_slow.Set('Timeout', 500.0)        
    elif level == 3: table_slow.Set('Timeout', 5000.0)
    
    if options.write_evt == 1:   table_slow.Set('WriteTxtEvt', 'yes')
    elif options.write_evt == 2: table_slow.Set('WriteXMLEvt', 'yes')
    if not options.do_bg:
        tables_all['tableSlow'+lvl] = table_slow
    
    ###
    ### Prepare module for random events execution analysis
    ###
    table_event = ROOT.Anp.Registry()
    table_event.Set('ClientType',    'ExecTableSlow')
    table_event.Set('ClientLink',    'sample_events')
    table_event.Set('Level',         level)
    table_event.Set('HistOpt',       'logy rebin png')
    table_event.Set('HistKey',       'ExecTableEvent')
    table_event.Set('FillNode',      'yes')
    table_event.Set('MakeHist',      'no')
    table_event.Set('NumSlowEvt',     500)
    table_event.Set('DetailSlowEvt',  200)
    table_event.Set('CollectSlowest', 'no')
    _setBoolKey(table_event, 'WriteSeq', options.write_seq)
    _setBoolKey(table_event, 'Debug',    options.debug)
    
    if   level == 2: table_event.Set('Timeout', 500.0)        
    elif level == 3: table_event.Set('Timeout', 5000.0)
    if not options.do_bg:
        tables_all['tableEvent'+lvl] = table_event

    ###
    ### Prepare module for HLT Farm analysis
    ###
    table_farm = ROOT.Anp.Registry()
    table_farm.Set('ClientType','ExecTableFarm')
    table_farm.Set('ClientLink','farm')
    table_farm.Set('Level',     level)
    table_farm.Set('HistOpt',   'rebin png')
    table_farm.Set('MakeHist',  MakeHist)
    table_farm.Set('PassLB',    'yes')
    _setBoolKey(table_farm, 'Debug', options.debug)
    if not options.do_bg:
        tables_all['tableFarm'+lvl] = table_farm

    ###
    ### Prepare module for HLT Farm analysis
    ###
    table_te = ROOT.Anp.Registry()
    table_te.Set('ClientType','ExecTableTE')
    table_te.Set('ClientLink','TE')
    table_te.Set('Level',     level)
    table_te.Set('HistOpt',   'rebin png')
    table_te.Set('MakeHist',  'no')
    _setBoolKey(table_te, 'Debug', options.debug)

    if False:
        tables_all['tableTE'+lvl] = table_te

    ###
    ### Prepare module for ROI based analysis
    ###
    table_roi = ROOT.Anp.Registry()
    table_roi.Set('ClientType','ExecTableRoI')
    table_roi.Set('ClientLink','roi')
    table_roi.Set('Level',     level)
    table_roi.Set('HistOpt',   'rebin png')
    table_roi.Set('HistKey',   'ExecTableRoI')
    table_roi.Set('MakeHist',  MakeHist)
    table_roi.Set('PassLB',    'yes')
    table_roi.Set('MakeRoIPlot',MakeHist)
    table_roi.Set('AlgDir',    'tableAlg'+lvl)
    table_roi.Set('ChnDir',    'tableChn'+lvl)
    _setBoolKey(table_roi, 'Debug', options.debug)
    if not options.do_bg:
        tables_all['tableRoI'+lvl] = table_roi

    ###
    ### Prepare module for Event based analysis
    ###
    table_evtPlot = ROOT.Anp.Registry()
    table_evtPlot.Set('ClientType','ExecTableEvt')
    table_evtPlot.Set('ClientLink','event_plots')
    table_evtPlot.Set('Level',     level)
    table_evtPlot.Set('HistOpt',   'rebin png')
    table_evtPlot.Set('HistKey',   'ExecTableEvt')        
    table_evtPlot.Set('MakeHist',  'yes')
    if options.do_bg:
        table_evtPlot.Set('ScaleBG',    'scaleBG')
    _setBoolKey(table_evtPlot, 'Debug', options.debug)
        
    tables_all['tableEventPlot'+lvl] = table_evtPlot
    
    ###
    ### Prepare module for survey analysis
    ###
    survey_lb = ROOT.Anp.Registry()
    survey_lb.Set('ClientType', 'SurveyLBs')
    survey_lb.Set('ClientLink', 'LBs')
    survey_lb.Set('Level',      level)
    survey_lb.Set('HistOpt',    'rebin png')
    survey_lb.Set('MakeHist',   MakeHist)
    _setBoolKey(survey_lb, 'Debug', options.debug)

    survey['surveyLB'+lvl] = survey_lb
    
    ###
    ### Add all cost analysis modules
    ###   
    for key, reg in tables_all.iteritems(): alg.SetPar(key, reg)
    for key, reg in tables_lbs.iteritems(): alg.SetPar(key, reg)

    for key, reg in survey.iteritems():
        alg.SetPar(key, reg)

    if not minimal_config:
        alg.SetKey('ClientListAll', str(' ').join(tables_all.keys()))
        alg.SetKey('ClientListLBs', str(' ').join(tables_lbs.keys()))
        alg.SetKey('SurveyList',    str(' ').join(survey.keys()))
    
    return alg

#-----------------------------------------------------------------------------
def _makeFarmTimes(alg_name, options):
    if   alg_name.count('L2'): level = 2
    elif alg_name.count('EF'): level = 3
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    studyfarm = AlgConfig(alg_name, 'StudyFarm')
    studyfarm.SetKey('Level', level)
    studyfarm.SetPar('StudyFarm::CoolData','readCool')
    studyfarm.SetPar('StudyFarm::CostData','makeCount')

    return studyfarm

#-----------------------------------------------------------------------------
def _makePredExec(alg_name, options):
    if   alg_name.count('L2'): level = 2
    elif alg_name.count('EF'): level = 3
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    predexec = AlgConfig(alg_name, 'PredExec')
    predexec.SetKey('Level', level)
    #predexec.SetPar("PredExec::rmChnName",'L2_vtxbeamspot_FSTracks')
    #predexec.SetPar("PredExec::rmChnName",'L2_MU4_DiMu_FS')
    #
    # For 159041 EF
    #
    #predexec.SetPar("PredExec::rmChnName",'EF_xe30_allL1_allCells')
    #
    #for L2 159041
    #
    #predexec.SetPar("PredExec::rmChnName",'L2_e5_medium_TRT')
    #
    # For MC - Remove L2_L2IdScan_FS
    #
    #setChn=['L2_L2IdScan_FS','L2_L2SiTrack_FS']
    predexec.SetPar("PredExec::rmChnName",'L2_L2IdScan_FS')
    #predexec.SetPar("PredExec::rmChnSet",setChn);
    return predexec

#-----------------------------------------------------------------------------
def _makePuTimes(alg_name, options):
    if   alg_name.count('L2'): level = 2
    elif alg_name.count('EF'): level = 3
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    pu_time = AlgConfig(alg_name, 'MakePuTimes')
    pu_time.SetKey("VarNames", "nevent ncalls ncalls_per_event timer_sum timer_call_mean timer_event_mean")
    pu_time.SetKey("Level", level)

    return pu_time

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareExecAlgs(run_module, options, args):

    import ROOT, PyCintex
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostExec')

    import os    
    testArea = os.environ.get("TestArea")
    
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeAlg.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeChn.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeROB.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeSeq.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/StudyTimeL2.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/StudyTimeEF.xml")

    #=========================================================================
    # Configure algorithms
    #
    predexec_L2 = _makePredExec('predexecL2', options)
    predexec_EF = _makePredExec('predexecEF', options)

    read_rob = AlgConfig('readRob', 'ReadRob')
    time_L2  = _makeTimes('timeL2', options)
    time_EF  = _makeTimes('timeEF', options)

    pu_timeL2 = _makePuTimes('puTimeL2', options)
    pu_timeEF = _makePuTimes('puTimeEF', options)

    result = {}

    if options.test.count("pred-exec"):
        result['alg_list'] = [predexec_L2, predexec_EF, read_rob, time_L2, time_EF, pu_timeL2, pu_timeEF]
        result['alg_html'] = [predexec_L2, predexec_EF,           time_L2, time_EF, pu_timeL2, pu_timeEF]
    else:
        result['alg_list'] = [read_rob, time_L2, time_EF, pu_timeL2, pu_timeEF]
        result['alg_html'] = [          time_L2, time_EF, pu_timeL2, pu_timeEF]

    return result

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareT0Algs(run_module, options, args):

    import ROOT, PyCintex
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostExec')

    import os
    testArea = os.environ.get("TestArea")

    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostRate/config/MakeCount.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostRate/config/ReadCool.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeAlg.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeChn.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeROB.xml")    
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/TrigTimeSeq.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/StudyTimeL2.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/StudyTimeEF.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableAlg.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableROS.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableROBIN.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableDet.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableChn.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableSeq.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableEvt.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTableRoI.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTablePred.xml")
    run_module.AddHistFile(testArea+"/Trigger/TrigCost/TrigCostExec/config/ExecTablePredAlg.xml")
  
    #=========================================================================
    # Configure algorithms
    #    
    alg_conf    = AlgConfig('configAlg',   'ConfigAlg')
    alg_mydq    = AlgConfig('processDQ',   'ProcessDQ')
    alg_html    = AlgConfig('htmlAlg',     'HtmlAlg')
    alg_pass    = AlgConfig('passEvent',   'PassEvent')    
    alg_read_db = AlgConfig('readCool',    'ReadCool')
    alg_conf_db = AlgConfig('configDB',    'ConfigDB')
    alg_overlap = AlgConfig('overlapMon',  'OverlapMon')
    alg_count   = AlgConfig('makeCount',   'MakeCount')

    alg_conf.SetKey("ReadXML_PS",    options.prescale_xml)
    alg_conf.SetKey("ReadXML_Group", options.group_xml)
    alg_conf.SetKey("WriteXML",      options.write_xml)
    alg_conf.SetKey("Debug",         False)
    alg_conf.SetKey("UseMenuGroups", not options.no_menu_groups)
    #alg_conf.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
    alg_conf_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')

    #alg_conf_db.SetKey('UseKeys', 'yes')
    #alg_conf_db.SetKey('SMK',     535)
    #alg_conf_db.SetKey('LV1_PS', 98)
    #alg_conf_db.SetKey('HLT_PS', 340)
    
    alg_read_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
    alg_read_db.SetKey('COOLTDAQID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200')
        
    alg_read_db.SetKey('ReadCounts', 'yes')
    alg_read_db.SetKey('ReadBCIDs', 'no')
    alg_read_db.SetKey('HistOpt', 'png logy')
    
    alg_html.SetKey("HtmlPath", options.html_path.rstrip('/'))
    alg_html.SetKey("IndexKeys", "AtlasProject AtlasVersion triggerMenuSetup L1PrescaleSet HLTPrescaleSet")

    alg_count.SetKey('CoolLumi', alg_read_db.GetAlgName())
    alg_count.SetKey('FillBCID', 'yes')

    alg_pass.SetKey("Accept",  options.pass_trigger)
    alg_pass.SetKey("Veto",    options.veto_trigger)
    alg_pass.SetKey("LBLimit", options.evt_lb)
    alg_pass.SetKey("LumiBeg", options.beg_lb)
    alg_pass.SetKey("LumiEnd", options.end_lb)    
    
    #=========================================================================
    # Add basic algorithms
    #
    top_list  = [alg_pass,
                 alg_conf,
                 alg_conf_db,
                 alg_read_db,
                 #alg_pass,
                 alg_count]
    
    top_html = [alg_read_db, alg_count]

    if options.do_bg:
        import ROOT, PyCintex
        #=========================================================================
        # Configure run module
        #  
        import TrigCostRate.TrigCostRateConfig as rateConfig
        
        alg_scbg = rateConfig._makeScaleBG('scaleBG', options)

        alg_conf_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTDAQID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200')

        alg_scbg.SetKey('Debug',       False)
        alg_scbg.SetKey('DataLumiAlg', alg_read_db.GetAlgName())
        alg_scbg.SetKey('DataConfAlg', alg_conf_db.GetAlgName())
        alg_scbg.SetKey('CAFReprocess',True)
        alg_scbg.SetPar("ReadXML_EBConfig", True)
        
        if options.default_ps != None: alg_conf.SetKey('DefaultPS', options.default_ps)
        if options.default_pt != None: alg_conf.SetKey('DefaultPT', options.default_pt)
        
        if len(options.dqxml) > 3:
            alg_mydq.SetKey("FileXML",  options.dqxml)
            alg_mydq.SetKey("SelectBX", options.dq_bxid)
            
        top_list  = [alg_pass,
                     alg_conf,
                     alg_mydq,
                     alg_conf_db,
                     alg_read_db,
                     #alg_pass,
                     alg_count]
        if True:
            top_list += [alg_scbg]
    
    #=========================================================================
    # Add cost algorithms
    #    
    predexec_L2 = _makePredExec('predexecL2', options)
    predexec_EF = _makePredExec('predexecEF', options)

    read_rob = AlgConfig('readRob', 'ReadRob')
    read_lbs = AlgConfig('readLBs', 'ReadCostLB')
    time_L2 = _makeTimes('timeL2', options)
    time_EF = _makeTimes('timeEF', options)

    exec_L2 = _makeExecs('execL2', options)
    exec_EF = _makeExecs('execEF', options)
    
    exec_L2.SetKey('LumiAlg', 'makeCount')
    exec_EF.SetKey('LumiAlg', 'makeCount')
    
    farmtime_L2 = _makeFarmTimes('farmtimeL2', options)
    farmtime_EF = _makeFarmTimes('farmtimeEF', options)

    read_rob.SetPar('ROSXML', options.ros_xml)

    disablePrefetch = _makeEnablePrefetch('disablePrefetch', options)

    if options.test.count("pred-exec"):
        top_list  = [predexec_L2, predexec_EF,read_lbs] + top_list
        top_list += [predexec_L2, predexec_EF,read_rob, exec_L2]
        top_html += [exec_L2]
        #top_list += [predexec_L2, predexec_EF, read_rob, time_L2, time_EF]
        #top_html += [predexec_L2, predexec_EF,           time_L2, time_EF]

    elif options.test.count("old"):
        top_list += [read_rob, time_L2, time_EF, farmtime_L2, farmtime_EF]
        top_html += [          time_L2, time_EF, farmtime_L2, farmtime_EF]
    elif options.test.count("Prefetch"):
        top_list += [read_lbs, read_rob, disablePrefetch, exec_L2, exec_EF]
        top_html += [exec_L2, exec_EF]
    else:
        top_list += [read_lbs, read_rob, exec_L2, exec_EF]
        top_html += [exec_L2, exec_EF]
   
    #=========================================================================
    # Set HtmlAlg list
    #    
    alg_html.SetKey('HtmlList', str(' ').join([halg.GetAlgName() for halg in top_html]))
       
    top_list += [alg_html]

    #========================================================================
    # Pileup monitor Configuration
    #
    result = {}

    if options.test.count('RoiCpuPu'):
        result['alg_list'] = [read_rob, exec_L2, exec_EF]
        result['alg_html'] = [exec_L2, exec_EF                    ]
        return result

    if options.test.count('MakeCount'):
        result['alg_list'] = top_list
        result['alg_html'] = top_html
        return result    

    #=========================================================================
    # Final adjustments to RunModule
    #
    run_module.AddTopAlg('topAlg', top_list)
    if options.do_bg:
        run_module.SetGlobalPar("ConfAlg", alg_conf.GetAlgName())
    else:
        #
        # Force the config to be read from the file
        #
        if options.read_conf_file:
            run_module.SetGlobalPar("ConfAlg", alg_conf.GetAlgName())
        else:
            run_module.SetGlobalPar("ConfAlg", alg_conf_db.GetAlgName())

    return top_list

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareMinimalConfig(options, args):

    #=========================================================================
    # Configure run module
    #  
    import TrigCostRate.TrigCostRateConfig as rateConfig

    run_module = rateConfig.prepareRunModule(options, args)

    import ROOT, PyCintex
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostExec')

    #=========================================================================
    # Configure basic algorithms
    #    
    alg_conf = AlgConfig('configAlg', 'ConfigAlg')    
    top_list = [alg_conf]

    #=========================================================================
    # Add cost algorithms
    #    
    read_rob = AlgConfig('readRob', 'ReadRob')
    exec_L2  = _makeExecs('execL2', options, minimal_config = True)
    exec_EF  = _makeExecs('execEF', options, minimal_config = True)
    
    read_rob.SetPar('ROSXML', options.ros_xml)

    top_list += [read_rob, exec_L2, exec_EF]

    #=========================================================================
    # Final adjustments to RunModule
    #
    run_module.AddTopAlg('topAlg', top_list)
    
    run_module.SetGlobalPar("ConfAlg",    alg_conf.GetAlgName())
    run_module.SetGlobalPar("IsData",     True)
    run_module.SetGlobalPar("Debug",      options.debug)
    run_module.SetGlobalPar("CanvasType", options.graphics)   

    return run_module


#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareNtupleAlgs(run_module, options, args):

    #=========================================================================
    # Configure run module
    #  
    import ROOT, PyCintex
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostExec')

    import os
    testArea = os.environ.get('TestArea')

    run_module.AddHistFile(testArea+'/Trigger/TrigCost/TrigCostExec/config/StudyCachingL2.xml')
    run_module.AddHistFile(testArea+'/Trigger/TrigCost/TrigCostExec/config/StudyCachingEF.xml')
    run_module.AddHistFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/ReadCool.xml')

    #=========================================================================
    # Configure basic algorithms
    #    
    alg_conf = AlgConfig('configAlg', 'ConfigAlg')    

    #=========================================================================
    # Add cost algorithms
    #    
    read_rob = AlgConfig('readRob', 'ReadRob')
    read_rob.SetPar('ROSXML', options.ros_xml)

    alg_study_L2 = _makeCaching('caching_L2', options)
    alg_study_EF = _makeCaching('caching_EF', options)
   
    #=========================================================================
    # Set top and html lists
    #    
    top_list = [alg_conf,
                read_rob, 
                alg_study_L2,
                alg_study_EF,
                ]

    #=========================================================================
    # Final adjustments to RunModule
    #
    run_module.AddTopAlg('topAlg', top_list)
    
    run_module.SetGlobalPar('ConfAlg',    alg_conf.GetAlgName())
    run_module.SetGlobalPar('IsData',     True)
    run_module.SetGlobalPar('Debug',      options.debug)
    run_module.SetGlobalPar('CanvasType', options.graphics)   

    return run_module

#-----------------------------------------------------------------------------
def _makeCaching(alg_name, options):

    study = AlgConfig(alg_name, 'StudyCaching')

    study.SetKey('DirName',   alg_name)    
    study.SetKey('HistOpts', 'logy rebin')

    import os
    testArea = os.environ.get("TestArea", None)

    if testArea and False:
        study.SetKey('EventFile', '%s/Trigger/TrigCost/TrigCostExec/data/common_events_r4405_r4398.txt' %testArea)

    if   alg_name.count('L2'): study.SetKey('Level', 2)
    elif alg_name.count('EF'): study.SetKey('Level', 3)
    else:
        raise NameError('Failed to get level for algorithm: '+alg_name)

    return study
