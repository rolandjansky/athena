# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from TrigCostBase.TrigCostBaseConfig import AlgConfig, RunModule, getLog, loadTrigCostBase
from string import split

clog = getLog('TrigCostRateConfig')

#-----------------------------------------------------------------------------
# Create and configure option parser for trigger rates and cost jobs
#

def search_file(filename, search_path):
   """Given a search path, find file
   """
   file_found = 0 
   paths = split(search_path, os.pathsep)
   for path in paths:
      if os.path.exists(os.path.join(path, filename)):
          file_found = 1 
          break
   if file_found:
      return os.path.abspath(os.path.join(path, filename))
   else:
      return None

   if __name__ == '___main__':
      search_path = '/bin' + os.pathsep + '/usr/bin'  # ; on windows, : on unix
      find_file = search_file('ls',search_path)
   if find_file:
      print "File found at %s" % find_file
   else:
      print "File not found"

def prepareRatesParser():
    
    from optparse import OptionParser
    p = OptionParser(usage="usage: <ntuple path> -k <input file search key> -o <output html path>", version="1.1")
    
    p.add_option( '-k', type="string", default=".root", dest="key",         help="filter input file names with this key")
    p.add_option( '-c', type="string", default="",         dest="config_path", help="path to optional trigger configuration ROOT file")
    p.add_option( '-o', type="string", default="",         dest="html_path",   help="path to output html directory")
    
    p.add_option( '-n', "--nevents",   type="int",          default=0,     dest="nevent",           help="number of events")
    p.add_option( '-r', '--root-file', type='string',       default='',    dest="root_file",        help="output ROOT file")
    p.add_option( '--max-num-files',   type="int",          default=10000,  dest="max_num_files",    help="Max number of files to be used. set to -1 for all")    
    
    p.add_option( '-d', '--debug',          action='store_true', default=False, dest="debug",             help="turn on global debug option")
    p.add_option('--histman',               action='store_true', default=False, dest="histman",           help="print histogram definitions")
    p.add_option('--data',                  action='store_true', default=False, dest="isdata",            help="processing data events")
    p.add_option('--prescaledmc',           action='store_true', default=False, dest="isprescaledmc",     help="processing prescaled mc events (scale but don't weight)")
    p.add_option('--dry-run',               action='store_true', default=False, dest="dryrun",            help="do not run C++ code")
    p.add_option('--do-unique',             action='store_true', default=False, dest="do_unique",         help="compute group unique rates")
    p.add_option('--do-overlap',            action='store_true', default=False, dest="do_overlap",        help="compute group overlap rates")
    p.add_option('--do-overlapmon-online',  action='store_true', default=False, dest="do_overlap_online", help="calculate overlap for chains, streams and groups NO PS accounted for")
    p.add_option('--do-overlapmon-pred',    action='store_true', default=False, dest="do_overlap_pred",   help="calculate overlap for chains, streams and groups with ps accounted for")
    p.add_option('--do-cumul',              action='store_true', default=False, dest="do_cumul",          help="compute group cumulatave rates")
    p.add_option('--do-time',               action='store_true', default=False, dest="do_time",           help="run timing code")
    p.add_option('--do-cps',                action='store_true', default=True,  dest="do_cps",            help="enable CPS rate code")
    p.add_option('--dq-bxid',               action='store_true', default=False, dest="dq_bxid",           help="use bunch x-ing id for DQ")
    p.add_option('--no-menu-groups',        action="store_true", default=False, dest="no_menu_groups",    help="ignore groups defined in menu")
    p.add_option('--poll-events',           action='store_true', default=False, dest="poll_events",       help="count number of input events")
    p.add_option('--do-bandwidth',          action='store_true', default=False, dest="do_bandwidth",      help="output bandwidth calculation")
    p.add_option('--simlPS',                action='store_true', default=False, dest="simlps",            help="apply random PS and PT")
    p.add_option('--predict-rates',         action='store_true', default=False, dest="predict",           help="configure code for rates prediction")
    p.add_option('--do-recursive',          action='store_true', default=False, dest="do_recursive",      help="do recursive input directory search")
    p.add_option('--do-bg',                 action='store_true', default=False, dest="do_bg",             help="Run the bunch group scaling")
    p.add_option('--read-conf-file',        action='store_true', default=False, dest="read_conf_file",    help="Read the triggerconfiguration from the TrigCost root file")
    p.add_option('--do-bg-fromXML',         action='store_true', default=False, dest="do_bg_fromXML",     help="Run the bunch group scaling, reading configuration from XML file")
    p.add_option('--no-scaleBG',            action='store_true', default=False, dest="no_scaleBG",        help="Turns scaleBG off for rates prediction")
    p.add_option('--no-event-tree',         action='store_true', default=False, dest="no_event_tree",     help="do not read event tree")    
    p.add_option('--do-raw-overlap',        action='store_true', default=False, dest="do_raw_overlap",    help="calculated raw overlap for online cost data")
    p.add_option('--no-batch',              action='store_true', default=False, dest="no_batch",          help="Do not set ROOT batch mode")
    p.add_option('--write-seq',             action='store_true', default=False, dest="write_seq",         help="Write Seq calls for Slow Events")
    p.add_option('--debug-stream',          action='store_true', default=False, dest="debug_stream",      help="Modifications for debug streams")
    p.add_option('--no-trig-wo-ps',         action='store_true', default=False, dest="remove_nops",       help="Remove triggers for which there is no XML PS value")
    p.add_option('--do-reduced-html',       action='store_true', default=False, dest="reduced_html",      help="Print only the rates HTML pages")
    p.add_option('--do-mini-evt',           action='store_true', default=False, dest="mini",              help="Write a root file with mini events")
    p.add_option('--cost-pass-l2',          action='store_true', default=False, dest="cost_pass_l2",      help="Cost events are required to pass the L2")

    p.add_option('--dq-xml',       type="string", default="", dest="dqxml",          help="xml file with DQ information")
    p.add_option('--groupXML',     type="string", default="", dest="group_xml",      help="xml file defining groups")
    p.add_option('--prescaleXML',  type="string", default="", dest="prescale_xml",   help="comma separated list of PS xml files")
    p.add_option('--normXML',      type="string", default="", dest="norm_xml",       help="xml file of dataset info")
    p.add_option('--normDS',       type="string", default="", dest="norm_ds",        help="dataset for rates normaliztion")
    p.add_option('--writeXML',     type="string", default="", dest="write_xml",      help="write LV1 xml configuration file")
    p.add_option('--rosXML',       type="string", default="", dest="ros_xml",        help="XML file with ROS to ROB map")
    p.add_option('--pass-trigger', type="string", default="", dest="pass_trigger",   help="pass trigger(s)")
    p.add_option('--veto-trigger', type="string", default="", dest="veto_trigger",   help="veto trigger(s)")
    p.add_option('--bg-scaleXML',  type="string", default="", dest="bg_scaleXML",    help="XML file for the bunch group scaling")
    
    p.add_option('--simlComb',     action='store_true', default=False, dest="simlcomb",         help="make simulated combined triggers")
    p.add_option('--simlXML',      type="string",       default="",    dest="simlxml",          help="xml file with simulated triggers")
    
    p.add_option('--pred-lumi',    type="float", default=1.0e+31, dest="pred_lumi",  help="Prediction luminosity")
    p.add_option('--pred-run',     type="int",   default=0,       dest="pred_run",   help="Use this run for prediction")
    p.add_option('--pred-lb',      type="int",   default=0,       dest="pred_lb",    help="Use this LB  for prediction")

    p.add_option('--evt-lb',       type="int",   default=0,       dest="evt_lb",    help="# events in LB < evt-lb")
    p.add_option('--beg-lb',       type="int",   default=0,       dest="beg_lb",    help="event LB >= beg-lb")
    p.add_option('--end-lb',       type="int",   default=0,       dest="end_lb",    help="event LB <= end-lb")
    p.add_option('--wrt-lb',       type="int",   default=1,       dest="wrt_lb",    help="create a (short or long) lb summary for every mod wrt_lb")
    p.add_option('--full-lb',      type="int",   default=10,      dest="full_lb",   help="create full lb summary for every mod full_lb lb")
    p.add_option('--write-evt',    type="int",   default=0,       dest="write_evt", help="Write Detailed Event: use 1 for csv and 2 for xml")

    p.add_option('--nColl'               , type="float"       , default=1.0                  , dest="nColl"               , help="number of collisions per crossing")
    p.add_option('--nCollInput'          , type="float"       , default=1.0                  , dest="nCollInput"          , help="number of collisions per crossing in input")
    p.add_option('--pileupMinProbCut'    , type="float"       , default=-1.0                 , dest="pileupMinProbCut"    , help="Exclude events where P(NV|mu) is less than this value")
    p.add_option('--useTruth'            , action="store_true", default=False                , dest="useTruth"            , help="Use true number of collisions")
    p.add_option('--doPileup'            , action="store_true", default=False                , dest="doPileup"            , help="Apply pileup scaling")
    p.add_option('--pileupWeightFilename', type="string"      , default="pileup_weights.root", dest="pileupWeightFilename", help="output ROOT file for pileup weights")
    p.add_option('--pileupVtxEffA'       , type="float"       , default=0.601                , dest="pileupVtxEffA"       , help="vertex efficiency for pileup weighting")
    p.add_option('--pileupVtxEffB'       , type="float"       , default=0.0                  , dest="pileupVtxEffB"       , help="vertex efficiency for pileup weighting")
    p.add_option('--pileupPowerOffset'   , type="float"       , default=1.0                  , dest="pileupPowerOffset"   , help="power law for pileup scaling")
    p.add_option('--readTiny-lb-mu-xml'  , type="string"      , default=""                   , dest="readTiny_lb_mu_xml"  , help="location of xml file for looking up mu as a function of lb")
    p.add_option('--readTiny-eos-xml'    , type="string"      , default="ReadTiny_eos.xml"   , dest="readTiny_eos_xml"    , help="path to xml file for readTiny eos files")
    p.add_option('--readTiny-bcid-xml'   , type="string"      , default="ReadTiny_BCID.xml"  , dest="readTiny_bcid_xml"   , help="path to xml file for readTiny BCID information")

    p.add_option('--lumi',       type="float", default=1.0e+31, dest="lumi",       help="luminosity")
    p.add_option('--xsec',       type="float", default=6.2e-26, dest="xsec",       help="cross section of dataset")
    p.add_option('--geff',       type="float", default=1.0,     dest="geff",       help="generator/filter effieciency for signal")
    p.add_option('--total',      type="float", default=-1.0,    dest="total",      help="total number of events for EB rates")
    p.add_option('--rate-scale', type="float", default=-1.0,    dest="rate_scale", help="scale factor for rate calculation")
    p.add_option('--default-ps', type='float', default=-1.0,    dest="default_ps", help="default PS for all triggers")
    p.add_option('--default-pt', type='float', default=None,    dest="default_pt", help="default PT for all triggers")
    
    p.add_option('--smk',        type='int', default=0, dest='smk',        help='SMK for prediction (test only)')
    p.add_option('--hlt-ps-key', type='int', default=0, dest='hlt_ps_key', help='HLT PS key for prediction (test only)')
    p.add_option('--lv1-ps-key', type='int', default=0, dest='lv1_ps_key', help='LV1 PS key for prediction (test only)')
    
    p.add_option('--runModXML', type="string", default="RunModule.xml", dest="runmodxml", help="name of xml file for job configuration")
    p.add_option('--graphics',  type="string", default="png",           dest="graphics",  help="graphics format: eps or png")
    p.add_option('--alg-list',  type="string", default="",              dest="alg_list",  help="algorithm list")
    p.add_option('--ignore',    type="string", default="",              dest="ignore",    help="ignored these chains for rates computation")
    p.add_option('--test',      type="string", default="",              dest="test",      help="option for testing code only")
    p.add_option('--xml-rates', type="string", default="",              dest="xmlrates",  help="write xml file with trigger rates results")
    p.add_option('--add-reg',   type="string", default="",              dest="add_reg",   help="read Registry and merge with main Registry")

    p.add_option('--useSubset',  type='float', default=1, dest="useSubset", help='only process subset of lumiblock' )
    
    return p

#-----------------------------------------------------------------------------
def _makeRates(alg_name, options):
    rate = AlgConfig(alg_name, 'MakeRates')
    
    rate.SetKey("Print",         "yes")
    rate.SetKey("DoGroups",      "yes")
    rate.SetKey("Geff",          options.geff)
    rate.SetKey("Xsec",          options.xsec)
    rate.SetKey("NCollis",       options.nColl)
    rate.SetKey("Lumi",          options.lumi)
    rate.SetKey("Debug",         options.debug)

    if options.ignore == "default":
        rate.SetKey("Ignore", "mb always ZDC RD0 RD1 _J10_MV _J20_MV MBTS MCB LUCID allL1 cost_monitor")
    else:
        rate.SetKey("Ignore", options.ignore)
   
    # AMI xsecs:
    #    10 TeV minbias: 51562800 nanobarn
    #   900 GeV minbias: 34388100 nanobarn
    # TriggerRatesTool xsec:
    #    10 TeV minbias: 65000000 nanobarn

    rate.SetKey("DoUnique",      options.do_unique)
    rate.SetKey("DoBandwidth",   options.do_bandwidth)
    rate.SetKey("DoUnique",      options.do_unique)
    rate.SetKey("DoOverlap",     options.do_overlap)
    rate.SetKey("DoCumulative",  options.do_cumul)
    rate.SetKey("DoCPS",         options.do_cps)

    if options.total > 0 :
        rate.SetKey("Total", options.total)
   
    if options.rate_scale > 0 :
        rate.SetKey("Scale", options.rate_scale)   

    if options.isdata:
        rate.SetKey("DecisionKeyLV1", "TAV")
        rate.SetPar("RateKey",        "COUNT")
    elif options.isprescaledmc:
        rate.SetKey("DecisionKeyLV1", "TAV")
        rate.SetPar("RateKey",        "COUNT_WITHPS")
    elif options.simlps:
        rate.SetKey("DecisionKeyLV1", "TBP")
        rate.SetPar("RateKey",        "COUNT")
    else:
        rate.SetKey("DecisionKeyLV1", "TBP")
        rate.SetPar("RateKey",        "WEIGHTED")
        
    if options.predict:
        rate.SetKey("DecisionKeyLV1", "TBP")
        rate.SetPar("RateKey",        "WEIGHTED")

    if options.test == "rate":
        rate.SetKey("DecisionKeyLV1", "TAV")
        rate.SetPar("RateKey",        "COUNT")
        
    if options.test == "siml":
        rate.SetKey("DecisionKeyLV1", "TBP")
        rate.SetPar("RateKey",        "WEIGHTED")    

    if options.test == "AOD":
        rate.SetKey("DecisionKeyLV1", "TBP")
        rate.SetKey("DecisionKeyHLT", "kPassed")
        rate.SetPar("RateKey",        "WEIGHTED")

    return rate

#-----------------------------------------------------------------------------
def _makePileupRates(alg_name, options):
    pu_rate = AlgConfig(alg_name, "MakePuRates")
    
    pu_rate.SetPar("MakeRates::DoBandwidth", "no")
    pu_rate.SetPar("MakeRates::DoGroups", "no")
    pu_rate.SetPar("MakeRates::DoOverlap", "no")
    pu_rate.SetPar("MakeRates::DoUnique", "no")
    pu_rate.SetPar("MakeRates::IsSubAlg", "yes")
        
    pu_rate.SetPar("MakeRates::Geff",    options.geff)
    pu_rate.SetPar("MakeRates::Xsec",    options.xsec)
    pu_rate.SetPar("MakeRates::Lumi",    options.lumi)
    pu_rate.SetPar("MakeRates::NCollis", options.nColl)    

    pu_rate.SetPar("CanvasType",         options.graphics)

    pu_rate.SetPar("MakeRates::DecisionKeyLV1", "TBP")
    pu_rate.SetPar("MakeRates::DecisionKeyHLT", "kPassed")
    pu_rate.SetPar("RateKey", "COUNT")

    return pu_rate

#-----------------------------------------------------------------------------
def _makeScaleBG(alg_name, options):
    bg_scale = AlgConfig(alg_name, "ScaleBG")
    
    bg_scale.SetPar("ScaleBG::XMLPath", options.bg_scaleXML)
    bg_scale.SetPar("ScaleBG::ConfAlg", "configDB")
    bg_scale.SetPar("ScaleBG::PredLumi", options.pred_lumi)
    bg_scale.SetPar("ScaleBG::PredRun",  options.pred_run)
    bg_scale.SetPar("ScaleBG::PredLB",   options.pred_lb)
    bg_scale.SetPar("ScaleBG::ReadXML_PS", options.prescale_xml)
    bg_scale.SetPar("ScaleBG::ReadXML_EBConfig", options.do_bg_fromXML)

    return bg_scale

#-----------------------------------------------------------------------------
def _makeScalePileup(alg_name, options):
    ncoll_scale = AlgConfig(alg_name, "ScalePileup")
    
    ncoll_scale.SetPar("ScalePileup::InputPileup"      , options.nCollInput           )
    ncoll_scale.SetPar("ScalePileup::PredictPileup"    , options.nColl                )
    ncoll_scale.SetPar("ScalePileup::useTruth"         , options.useTruth             )
    ncoll_scale.SetPar("ScalePileup::WeightNtupleName" , options.pileupWeightFilename )
    ncoll_scale.SetPar("ScalePileup::MinProbCut"       , options.pileupMinProbCut     )
    ncoll_scale.SetPar("ScalePileup::PowerOffset"      , options.pileupPowerOffset    )
    ncoll_scale.SetPar("ScalePileup::VtxEffA"          , options.pileupVtxEffA        )
    ncoll_scale.SetPar("ScalePileup::VtxEffB"          , options.pileupVtxEffB        )
    
    return ncoll_scale

#-----------------------------------------------------------------------------
def _makeReadTiny(alg_name, options):
    readTiny = AlgConfig(alg_name, "ReadTiny")

    readTiny.SetPar("ReadTiny::TreeNameEvent" , "eb_event" )
    readTiny.SetPar("ReadTiny::TreeNameConfig", "eb_config")
    
    # Some job descriptions don't specify readTiny_eos
    if options.readTiny_eos_xml == '':
        options.readTiny_eos_xml = 'ReadTiny_eos.xml'
    
    XMLPATHS=os.environ.get("XMLPATH")
    XMLFILE_eos  = search_file("TrigCostRate/"+options.readTiny_eos_xml  , XMLPATHS)
    XMLFILE_mu   = search_file("TrigCostRate/"+options.readTiny_lb_mu_xml, XMLPATHS)
    XMLFILE_bcid = search_file("TrigCostRate/"+options.readTiny_bcid_xml , XMLPATHS)
    
    print XMLFILE_eos
    print XMLFILE_mu
    print XMLFILE_bcid
    print options.readTiny_lb_mu_xml
    
    readTiny.SetPar("ReadTiny::eosXMLPath"  , XMLFILE_eos )
    readTiny.SetPar("ReadTiny::MuXMLPath"   , XMLFILE_mu  )
    readTiny.SetPar("ReadTiny::BCIDXMLPath" , XMLFILE_bcid)
    
    return readTiny

#-----------------------------------------------------------------------------
# Create and configure RunModule
#
def prepareRunModule(options, args):

    loadTrigCostBase()    
    import ROOT, PyCintex

    if not options.no_batch:
        ROOT.gROOT.SetBatch(True)
    
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostRate')

    run = RunModule()
    
    run.SetKey("TreeThread", "event")
    run.SetKey("NPrint", 100000)
    run.SetKey("NEvent", options.nevent)
    run.SetKey("FileKeys", ".root")
    run.SetKey("DoThreads", "no")

    if options.no_event_tree:
        run.SetKey("TreeName", "config")
    else:
        run.SetKey("TreeName", "config event")        
        
    if options.poll_events:        run.SetKey("TreePoll", "event")
    if len(options.root_file) > 5: run.SetKey("OutputFile", options.root_file)
        
    if len(options.config_path) > 0:
        run.SetKey("FileInit", options.config_path)
        run.SetKey("TreeInit", "config")

    run.SetPar("EventSvc::NormDS",   options.norm_ds)
    run.SetPar("EventSvc::Datasets", str(' ').join(args))
    
    XMLPATHS=os.environ.get("XMLPATH")
    XMLPATH=search_file("TrigCostRate/TrigCostRate_ntuple_index.xml",XMLPATHS)

    if options.norm_xml == "default":
        run.SetPar("EventSvc::NormXML", XMLPATH)
    elif len(options.norm_xml) > 0:
        run.SetPar("EventSvc::NormXML", options.norm_xml)

    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareRunConfig(options, args):

    run = prepareRunModule(options, args)

    #=========================================================================
    # Configure algorithms
    #
    alg_conf  = AlgConfig('configAlg',   'ConfigAlg')
    alg_mydq  = AlgConfig('processDQ',   'ProcessDQ')
    alg_pass  = AlgConfig('passEvent',   'PassEvent')
    alg_siml  = AlgConfig('simlPS',      'SimlPS')
    alg_html  = AlgConfig('htmlAlg',     'HtmlAlg')
    html_rate = AlgConfig('htmlRates',   'HtmlRates')
    plot_evt  = AlgConfig('plotEvent',   'PlotEvent')
    alg_overL1= AlgConfig('overlapMonL1','OverlapMon')
    alg_overL2= AlgConfig('overlapMonL2','OverlapMon')
    alg_overEF= AlgConfig('overlapMonEF','OverlapMon')
    #alg_count = AlgConfig('makeCount',   'MakeCount')
    
    alg_html.SetKey("HtmlPath", options.html_path.rstrip('/'))
    alg_html.SetKey("IndexKeys", "AtlasProject AtlasVersion triggerMenuSetup L1PrescaleSet HLTPrescaleSet")
    if options.reduced_html:
       alg_html.SetKey("PrintAll", False)
    
    html_rate.SetKey("RatesName", "makeRates")
    html_rate.SetKey("ConfigKeys", "AtlasProject AtlasVersion triggerMenuSetup L1PrescaleSet HLTPrescaleSet CMTPATH")
    html_rate.SetKey("Debug",      False)
    
    alg_overL1.SetKey("Debug",       False)
    alg_overL1.SetKey("DebugDetails",False)
    alg_overL1.SetKey("Level",       1)
    alg_overL2.SetKey("Level",       2)
    alg_overEF.SetKey("Level",       3)
    
    if options.do_overlap_pred:
       alg_overL1.SetKey("DoPS",    "yes")
       alg_overL2.SetKey("DoPS",    "yes")
       alg_overEF.SetKey("DoPS",    "yes")
       alg_overL1.SetKey('ScaleBG', 'scaleBG')
       alg_overL2.SetKey('ScaleBG', 'scaleBG')
       alg_overEF.SetKey('ScaleBG', 'scaleBG')
    
    if len(options.xmlrates) > 3:
        html_rate.SetKey("XMLPath", options.xmlrates)
    
    alg_pass.SetKey("Accept",  options.pass_trigger)
    alg_pass.SetKey("Veto",    options.veto_trigger)
    alg_pass.SetKey("LBLimit", options.evt_lb)
    alg_pass.SetKey("LumiBeg", options.beg_lb)
    alg_pass.SetKey("LumiEnd", options.end_lb)

    alg_conf.SetKey("ReadXML_PS",    options.prescale_xml)
    alg_conf.SetKey("ReadXML_Group", options.group_xml)
    alg_conf.SetKey("WriteXML",      options.write_xml)
    alg_conf.SetKey("RemoveNoPS",    options.remove_nops)
    alg_conf.SetKey("Debug",         False)
    alg_conf.SetKey("UseMenuGroups", not options.no_menu_groups)

    if options.default_ps != None: alg_conf.SetKey('DefaultPS', options.default_ps)
    if options.default_pt != None: alg_conf.SetKey('DefaultPT', options.default_pt)    
    
    if len(options.dqxml) > 3:
        alg_mydq.SetKey("FileXML",  options.dqxml)
        alg_mydq.SetKey("SelectBX", options.dq_bxid)
        alg_mydq.SetKey("useSubset",options.useSubset)

    alg_rate = _makeRates('makeRates', options)
    apu_rate = _makePileupRates('puRates', options)        
    
    #=========================================================================
    # Fill top level algorithm list and algorithms for HtmlAlg 
    #
    alg_list = [alg_conf, alg_mydq,  alg_pass]
    if options.test.count('RoiCpuPu'):
       run_html = [html_rate]
    else:   
       run_html = [html_rate, apu_rate]
    
    if options.do_overlap_online or options.do_overlap_pred:
        run_html +=[alg_overL1,
                    alg_overL2,
                    alg_overEF]

    #=========================================================================
    # Create algorithms for reading EB data and re-scaling of number of
    # collisions per event for MC/data
    #
    alg_readTiny = _makeReadTiny('readTiny', options)
    alg_scncoll  = _makeScalePileup('scalePileup', options)

    #=========================================================================
    # Simulate PS (using randoms)
    #         
    if options.simlps:
        alg_siml.SetKey("DecisionKeyLV1", "TBP")

    #=========================================================================
    # Simulate Combined objects
    #
    if options.simlcomb:
        alg_comb = AlgConfig('simlComb',      'SimlComb'   )
        alg_comb.SetKey("SimlXML",options.simlxml)		
        alg_comb.SetKey("ConfAlg", 'configAlg'   )
        alg_comb.SetKey("Debug",  False)
        alg_comb.SetKey("DecisionKeyLV1", "TBP")
        alg_comb.SetKey("DecisionKeyHLT", "kPassed")
        alg_rate.SetKey("SimulList", "simlComb")
        alg_list += [alg_comb]


    #=========================================================================
    # Add additional algorithms from TrigCostAlgs
    # 
    if options.test.count('algs'):
        try:
            from TrigCostAlgs.TrigCostAlgsConfig import prepareAlgList
            
            for alg in prepareAlgList(run):
                if  options.test.count(alg.GetTarget()):
                    alg_list += [alg]
                    clog.info('Adding '+alg.GetAlgName()+' with target: '+alg.GetTarget())
        except:
            clog.info('Can not import algorithms from TriggerAlgs... continue without them')

    if options.test.count('RoiCpuPu'):
       alg_list += [alg_rate, html_rate]
    else:       
       alg_list += [alg_rate, apu_rate, html_rate]

    #=========================================================================
    # Configuration for rates prediction with EB data and ScaleBG algorithm
    #
    rate_scale_list = []

    if options.do_bg or options.do_bg_fromXML or options.isdata:
        import ROOT, PyCintex
        
        alg_conf_db = AlgConfig('configDB', 'ConfigDB')
        alg_read_db = AlgConfig('readCool', 'ReadCool')        
        alg_scbg    = _makeScaleBG('scaleBG', options)
        
        alg_conf_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTDAQID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200')
        
        alg_scbg.SetKey('DataLumiAlg', alg_read_db.GetAlgName())
        alg_scbg.SetKey('DataConfAlg', alg_conf_db.GetAlgName())        
        alg_rate.SetKey('LumiAlg',     alg_read_db.GetAlgName())
        
        alg_list = [alg_mydq,
            alg_read_db,
            alg_conf_db]
        
        if options.doPileup :
            alg_list        += [alg_readTiny]
            alg_list        += [alg_scncoll]
            rate_scale_list += [alg_scncoll]
        
        if not options.no_scaleBG:
            rate_scale_list += [alg_scbg]
            if not options.do_bg_fromXML and not options.doPileup:
               alg_list        += [alg_readTiny]
            alg_list        += [alg_scbg]
        
        alg_list += [alg_conf]

        if options.simlcomb:
            alg_list += [alg_comb]                
        
        alg_list += [alg_pass,
                    alg_rate,
                    html_rate]
        
        if options.do_overlap_online or options.do_overlap_pred:
            alg_list += [alg_overL1,
                         alg_overL2,
                         alg_overEF]
            
        if options.pred_run > 0 and options.pred_lb > 0 or True:
            run_conf_db = AlgConfig('configDB_pred', 'ConfigDB')
            run_read_db = AlgConfig('readCool_pred', 'ReadCool')

            run_conf_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
            run_read_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
            run_read_db.SetKey('COOLTDAQID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200')
            #alg_count.SetKey('CoolLumi', alg_read_db.GetAlgName())

            alg_conf.SetKey('UpdateAlg', run_conf_db.GetAlgName())
            
            run_read_db.SetKey("ConfigRun", options.pred_run)
            run_conf_db.SetKey("ConfigRun", options.pred_run)
            run_conf_db.SetKey("ConfigLB",  options.pred_lb)

            alg_scbg.SetKey("PredRun",     options.pred_run)
            alg_scbg.SetKey("PredLB",      options.pred_lb)
            alg_scbg.SetKey('PredLumiAlg', run_read_db.GetAlgName())
            alg_scbg.SetKey('PredConfAlg', run_conf_db.GetAlgName())

            alg_list.insert(0, run_conf_db)
            alg_list.insert(0, run_read_db)

            #alg_list +=[alg_count]
            #run_html +=[alg_count]
    #=========================================================================
    # Add event weight scaling algorithms
    #
    alg_rate.SetKey('ScaleAlg', str(' ').join([alg.GetAlgName() for alg in rate_scale_list]))

    #=========================================================================
    # Add timing algorithms
    #
    if options.do_time:

        exec_algs = {}
        
        if options.test.count('RoiCpuPu'):
            from TrigCostExec.TrigCostExecConfig import prepareT0Algs
            exec_algs = prepareT0Algs(run, options, args)

        elif options.test.count('MakeCount'):
            from TrigCostExec.TrigCostExecConfig import prepareT0Algs
            exec_algs = prepareT0Algs(run, options, args)

            
        else:
            from TrigCostExec.TrigCostExecConfig import prepareExecAlgs
            exec_algs = prepareExecAlgs(run, options, args)
            
        alg_list += exec_algs['alg_list']
        run_html += exec_algs['alg_html']

    #=========================================================================
    # html algorithm is added last
    #
    alg_list += [alg_html]
    
    alg_html.SetKey('HtmlList', str(' ').join([halg.GetAlgName() for halg in run_html if alg_list.count(halg)]))

    #=========================================================================
    # Final adjustments to RunModule
    #
    run.AddTopAlg('topAlg', alg_list)

    run.SetGlobalPar("IsData",     options.isdata)
    run.SetGlobalPar("Debug",      options.debug)
    run.SetGlobalPar("CanvasType", options.graphics)
    run.SetGlobalPar("ConfAlg",    alg_conf.GetAlgName())
    
    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareT0Config(options, args):

    run = prepareRunModule(options, args)

    #=========================================================================
    # Import timing algorithms
    #  
    from TrigCostExec.TrigCostExecConfig import prepareT0Algs
    
    prepareT0Algs(run, options, args)    

    run.SetGlobalPar("IsData",     True)
    run.SetGlobalPar("Debug",      options.debug)
    run.SetGlobalPar("CanvasType", options.graphics)    
    
    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareNtupleConfig(options, args):

    run = prepareRunModule(options, args)

    #=========================================================================
    # Import timing algorithms
    #  
    import TrigCostExec.TrigCostExecConfig as Exec

    Exec.prepareNtupleAlgs(run, options, args)

    run.SetGlobalPar("IsData",     True)
    run.SetGlobalPar("Debug",      options.debug)
    run.SetGlobalPar("CanvasType", options.graphics)    
    
    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareROSConfig(options, args):

    run = prepareRunModule(options, args)

    #=========================================================================
    # Configure algorithms
    #
    alg_conf  = AlgConfig('configAlg',                    'ConfigAlg')
    alg_mydq  = AlgConfig('processDQ',                    'ProcessDQ')
    alg_pass  = AlgConfig('passEvent',                    'PassEvent')
    alg_cnt   = AlgConfig('countTriggers_1st',            'CountTriggers')
    alg_cnt_2 = AlgConfig('countTriggers_afterDQcheck',   'CountTriggers')
    alg_cnt_3 = AlgConfig('countTriggers_afterpassEvent', 'CountTriggers')
    alg_cnt_4 = AlgConfig('countTriggers_afterscaleBG',   'CountTriggers')
    alg_cnt_5 = AlgConfig('countTriggers_aftermakeRates', 'CountTriggers')
    alg_html  = AlgConfig('htmlAlg',                      'HtmlAlg')
    html_rate = AlgConfig('htmlRates',                    'HtmlRates')
    alg_over  = AlgConfig('overlapMon',                   'OverlapMon')
    
    alg_html.SetKey("HtmlPath", options.html_path.rstrip('/'))
    alg_html.SetKey("IndexKeys", "AtlasProject AtlasVersion triggerMenuSetup L1PrescaleSet HLTPrescaleSet")
    
    html_rate.SetKey("RatesName", "makeRates")
    html_rate.SetKey("ConfigKeys", "AtlasProject AtlasVersion triggerMenuSetup L1PrescaleSet HLTPrescaleSet CMTPATH")
    html_rate.SetKey("Debug",      False)

    alg_pass.SetKey("LBLimit", options.evt_lb)
    alg_pass.SetKey("LumiBeg", options.beg_lb)
    alg_pass.SetKey("LumiEnd", options.end_lb)

    alg_cnt.SetKey("LumiBeg", options.beg_lb)
    alg_cnt.SetKey("LumiEnd", options.end_lb)
    
    alg_conf.SetKey("Debug",         False)
    alg_over.SetKey("Debug",         True)
    
    if options.default_ps != None: alg_conf.SetKey('DefaultPS', options.default_ps)
    if options.default_pt != None: alg_conf.SetKey('DefaultPT', options.default_pt)    
    
    if len(options.dqxml) > 3:
        alg_mydq.SetKey("FileXML",  options.dqxml)
        alg_mydq.SetKey("SelectBX", options.dq_bxid)
        alg_mydq.SetKey("useSubset",options.useSubset)

    alg_rate = _makeRates('makeRates', options)
    alg_rate.SetKey("DecisionKeyLV1", "TAV")
    alg_rate.SetPar("RateKey",        "WEIGHTED")

    #=========================================================================
    # Create algorithms for reading EB data and re-scaling of number of
    # collisions per event for MC/data
    #
    if options.doPileup:
        alg_readTiny = _makeReadTiny('readTiny', options)
        alg_scncoll  = _makeScalePileup('scalePileup', options)
        
    #=========================================================================
    # Configuration for rates prediction with EB data and ScaleBG algorithm
    #
    if options.do_bg or options.do_bg_fromXML or options.isdata:
        import ROOT, PyCintex
        
        alg_conf_db = AlgConfig('configDB', 'ConfigDB')
        alg_read_db = AlgConfig('readCool', 'ReadCool')        
        alg_scbg    = _makeScaleBG('scaleBG', options)

        alg_conf_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTRIGID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200')
        alg_read_db.SetKey('COOLTDAQID', 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=MONP200')

        alg_scbg.SetKey('DataLumiAlg', alg_read_db.GetAlgName())
        alg_scbg.SetKey('DataConfAlg', alg_conf_db.GetAlgName())        
        alg_rate.SetKey('LumiAlg',     alg_read_db.GetAlgName())
        alg_scbg.SetKey("ReadXML_EBConfig", True)
        alg_scbg.SetKey('CAFReprocess',  True)

        rate_scale_list = []
        
        if not options.no_scaleBG:
            rate_scale_list += [alg_scbg]

        alg_list = [alg_conf,
                    alg_cnt,
                    alg_mydq,
                    alg_cnt_2,
                    alg_read_db,
                    alg_conf_db,
                    alg_pass,
                    alg_cnt_3,
                    alg_over]
        
        if not options.no_scaleBG:
           if not options.do_bg_fromXML and not options.doPileup:
              alg_list += [alg_readTiny]
           alg_list += [alg_scbg,
                        alg_cnt_4]

        if options.doPileup:
            alg_list        += [alg_readTiny]
            alg_list        += [alg_scncoll]
            rate_scale_list += [alg_scncoll]
            
        alg_list +=[alg_rate,
                    alg_cnt_5,
                    html_rate]
        
        run_html = [alg_cnt,
                    alg_cnt_2,
                    alg_cnt_3]

        if not options.no_scaleBG:
            run_html += [alg_cnt_4]
        
        run_html += [alg_cnt_5,
                    alg_rate,
                    html_rate,
                    alg_over]
        
    #=========================================================================
    # Add event weight scaling algorithms
    #
    alg_rate.SetKey('ScaleAlg', str(' ').join([alg.GetAlgName() for alg in rate_scale_list]))

    #=========================================================================
    # html algorithm is added last
    #
    alg_list += [alg_html]
    
    alg_html.SetKey('HtmlList', str(' ').join([halg.GetAlgName() for halg in run_html if alg_list.count(halg)]))

    #=========================================================================
    # Final adjustments to RunModule
    #
    run.AddTopAlg('topAlg', alg_list)

    run.SetGlobalPar("ConfAlg",    alg_conf.GetAlgName())
    run.SetGlobalPar("IsData",     True)
    run.SetGlobalPar("Debug",      options.debug)
    run.SetGlobalPar("CanvasType", options.graphics)    
    
    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareRatePrediction(options, args):

    import ROOT, PyCintex

    #=========================================================================
    # Configure:
    #   - trigger configuration algorithms
    #   - simulation algorithm    
    #   - rates algorithms
    #   - rates prediction
    #   - luminosity algorithm for rates normalization
    #
    alg_confDB = AlgConfig('configDB',      'ConfigDB')
    alg_conf1  = AlgConfig('configAlg_1',   'ConfigAlg')
    alg_conf2  = AlgConfig('configAlg_2',   'ConfigAlg')    
    alg_pred   = AlgConfig('predRates',     'PredRates')
    alg_siml   = AlgConfig('simlPS',        'SimlPS')

    alg_confDB.SetKey('UseKeys', 'yes')
    alg_confDB.SetKey('SMK',     765)
    alg_confDB.SetKey('LV1_PS', 1292)
    alg_confDB.SetKey('HLT_PS', 1210)
    alg_confDB.SetKey('WriteXML', '')

    #alg_conf1.SetKey('UpdateAlg', alg_confDB.GetAlgName())
    #alg_conf2.SetKey('UpdateAlg', alg_confDB.GetAlgName())

    alg_conf1.SetKey('ReadXML_PS', 'predPS_1.xml')
    alg_conf2.SetKey('ReadXML_PS', 'predPS_2.xml')

    alg_conf1.SetKey('DefaultPS', -1.0)
    alg_conf1.SetKey('DefaultPT',  0.0)

    alg_conf2.SetKey('DefaultPS', -1.0)
    alg_conf2.SetKey('DefaultPT',  0.0)

    alg_rate1         = _makeRates('makeRates_1',         options)
    alg_rate2         = _makeRates('makeRates_2',         options)
    alg_rate1_afterPS = _makeRates('makeRates_1_afterPS', options)
    alg_rate2_afterPS = _makeRates('makeRates_2_afterPS', options)

    alg_rate1        .SetKey('ConfAlg', alg_conf1.GetAlgName())
    alg_rate2        .SetKey('ConfAlg', alg_conf2.GetAlgName())
    alg_rate1_afterPS.SetKey('ConfAlg', alg_conf1.GetAlgName())
    alg_rate2_afterPS.SetKey('ConfAlg', alg_conf2.GetAlgName())

    alg_rate1.SetPar("DecisionKeyLV1", "TBP")
    alg_rate1.SetPar("RateKey",        "WEIGHTED")
    alg_rate2.SetPar("DecisionKeyLV1", "TBP")
    alg_rate2.SetPar("RateKey",        "WEIGHTED")

    alg_rate1_afterPS.SetPar("DecisionKeyLV1", "TBP")
    alg_rate1_afterPS.SetPar("RateKey",        "COUNT")
    alg_rate2_afterPS.SetPar("DecisionKeyLV1", "TBP")
    alg_rate2_afterPS.SetPar("RateKey",        "COUNT")

    alg_rate1        .SetKey("GroupXML", "predPS_group.xml")
    alg_rate1_afterPS.SetKey("GroupXML", "predPS_group.xml")
    alg_rate2        .SetKey("GroupXML", "predPS_group.xml")
    alg_rate2_afterPS.SetKey("GroupXML", "predPS_group.xml")

    alg_rate1        .SetKey("UseMenuGroups", "no")
    alg_rate1_afterPS.SetKey("UseMenuGroups", "no")    
    alg_rate2        .SetKey("UseMenuGroups", "no")
    alg_rate2_afterPS.SetKey("UseMenuGroups", "no")

    alg_rate1        .SetKey("DoOverlap", "yes")
    alg_rate1_afterPS.SetKey("DoOverlap", "yes")
    alg_rate2        .SetKey("DoOverlap", "yes")
    alg_rate2_afterPS.SetKey("DoOverlap", "yes")

    alg_siml.SetPar("DecisionKeyLV1", "TBP")
    alg_siml.SetKey('ConfAlg', alg_conf1.GetAlgName())
    
    alg_pred.SetKey('RatesName1', alg_rate1_afterPS.GetAlgName())
    alg_pred.SetKey('RatesName2', alg_rate2_afterPS.GetAlgName())    

    alg_list = [alg_conf1,
                alg_conf2,
                alg_rate1,
                alg_rate2,
                alg_siml,
                alg_rate1_afterPS,
                alg_rate2_afterPS,
                alg_pred]

    #=========================================================================
    # Configure html algorithms
    #
    alg_html1         = AlgConfig('htmlAlg_1',         'HtmlAlg')
    alg_html2         = AlgConfig('htmlAlg_2',         'HtmlAlg')
    alg_html1_afterPS = AlgConfig('htmlAlg_1_afterPS', 'HtmlAlg')
    alg_html2_afterPS = AlgConfig('htmlAlg_2_afterPS', 'HtmlAlg')    
    alg_html_predict  = AlgConfig('htmlAlg_predict',   'HtmlAlg')
    
    html_rate1         = AlgConfig('htmlRates_1',         'HtmlRates')
    html_rate2         = AlgConfig('htmlRates_2',         'HtmlRates')
    html_rate1_afterPS = AlgConfig('htmlRates_1_afterPS', 'HtmlRates')
    html_rate2_afterPS = AlgConfig('htmlRates_2_afterPS', 'HtmlRates')
    html_rate_predict  = AlgConfig('htmlRates_predict',   'HtmlRates')

    if len(options.html_path):
        alg_html1        .SetKey('HtmlPath', options.html_path+'_1')
        alg_html2        .SetKey('HtmlPath', options.html_path+'_2')
        alg_html1_afterPS.SetKey('HtmlPath', options.html_path+'_1_afterPS')
        alg_html2_afterPS.SetKey('HtmlPath', options.html_path+'_2_afterPS')
        alg_html_predict .SetKey('HtmlPath', options.html_path+'_predict')

    alg_html1        .SetKey('HtmlList', html_rate1.GetAlgName())
    alg_html2        .SetKey('HtmlList', html_rate2.GetAlgName())
    alg_html1_afterPS.SetKey('HtmlList', html_rate1_afterPS.GetAlgName())
    alg_html2_afterPS.SetKey('HtmlList', html_rate2_afterPS.GetAlgName())
    alg_html_predict .SetKey('HtmlList', html_rate_predict.GetAlgName())

    alg_html1        .SetKey('ConfAlg', alg_conf1.GetAlgName())
    alg_html2        .SetKey('ConfAlg', alg_conf2.GetAlgName())
    alg_html1_afterPS.SetKey('ConfAlg', alg_conf1.GetAlgName())
    alg_html2_afterPS.SetKey('ConfAlg', alg_conf2.GetAlgName())
    alg_html_predict .SetKey('ConfAlg', alg_conf2.GetAlgName())
    
    html_rate1        .SetKey('RatesName', alg_rate1.GetAlgName())
    html_rate2        .SetKey('RatesName', alg_rate2.GetAlgName())
    html_rate1_afterPS.SetKey('RatesName', alg_rate1_afterPS.GetAlgName())
    html_rate2_afterPS.SetKey('RatesName', alg_rate2_afterPS.GetAlgName())    
    html_rate_predict .SetKey('RatesName', alg_pred.GetAlgName())
        
    alg_list += [html_rate1,
                 html_rate2,
                 html_rate1_afterPS,
                 html_rate2_afterPS,
                 html_rate_predict,
                 alg_html1,
                 alg_html2,
                 alg_html1_afterPS,
                 alg_html2_afterPS,
                 alg_html_predict]

    #=========================================================================
    # Configure RunModule
    #    
    run = prepareRunModule(options, args)    
    run.AddTopAlg('topAlg', alg_list)

    run.SetGlobalPar('IsData',     options.isdata)
    run.SetGlobalPar('Debug',      options.debug)
    run.SetGlobalPar('CanvasType', options.graphics)

    import os

    XMLPATH=search_file("ReadCool.xml",XMLPATHS)
    run.AddHistFile(XMLPATH+"ReadCool.xml")
    XMLPATH=search_file("ReadCool.xml",XMLPATHS)
    run.AddHistFile(XMLPATH+"PlotEvent.xml")
    
    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def prepareOnlinePrediction(options, args):

    import ROOT, PyCintex
    ROOT.gSystem.Load('libTriggerOnline')

    options.smk        = 765
    options.hlt_ps_key = 1210
    options.lv1_ps_key = 1210    

    try:
        from TriggerOnline.TrigOnlineConfig import prepareOnlineAlgs
        alg_list = prepareOnlineAlgs(options, args)
    except:
        clog.error('Failed to import algorithms from TriggerOnline')

    #=========================================================================
    # Configure RunModule
    #
    run = prepareRunModule(options, args)    

    return run

#-----------------------------------------------------------------------------
# Create and configure top level module
#
def doAfterExecWork(options, args, run_module):

    if os.path.isdir(options.html_path):        
        #
        # Copy xml configuration to output html directory
        #
        import filecmp
        from subprocess import Popen

        f = options.runmodxml
        h = options.html_path.rstrip('/')        
        same_file = False
        
        if os.path.isfile(f) and os.path.isfile(h+'/'+f) and filecmp.cmp(f, h+'/'+f):
            same_file = True
        
        if not same_file:
            pCp = Popen('cp ' + f + ' ' +  options.html_path, shell=True)
            pCp.wait()
            
        #
        # Copy sorttable script
        #
        DATAPATHS=os.environ.get("DATAPATH")
        files=[]
        
        datafile=search_file("TrigCostRate/sorttable.js",DATAPATHS)
        files.append(datafile)
        datafile=search_file("TrigCostRate/jquery.min.js",DATAPATHS)
        files.append(datafile)
        datafile=search_file("TrigCostRate/jquery.columnfilters.js",DATAPATHS)
        files.append(datafile)
        
        copyPath=options.html_path.rstrip('/')+'/';
        
        for file in files:
            head,tail = os.path.split(file)
            if not os.path.isfile(copyPath+tail) or not filecmp.cmp(file, copyPath+tail):
                pCp = Popen('cp ' + file + ' ' +  copyPath+tail, shell=True)
                pCp.wait()

        
        for idir in os.listdir(options.html_path):
            ndir = options.html_path.rstrip('/')+'/'+idir
            
            if os.path.isdir(ndir) and idir.count('.') == 0:
                copyPath = ndir+"/"
                for file in files:
                    head,tail = os.path.split(file)
                    if not os.path.isfile(copyPath+file) or not filecmp.cmp(file, copyPath+tail):
                        pCp = Popen('cp ' + file + ' ' +  copyPath + tail, shell=True)
                        pCp.wait()
                        
