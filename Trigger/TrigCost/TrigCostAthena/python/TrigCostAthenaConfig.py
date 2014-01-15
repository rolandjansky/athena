# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrigCostMonitor.TrigCostMonitorConf as costMonitor
import TrigCostAthena.TrigCostAthenaConf as costAthena
from AthenaCommon.Logging import logging
from TrigCostAthena.TrigCostAthenaFlags import TrigCostAthenaFlags

#----------------------------------------------------------------------
# prepare TrigCostAlg configuration for trigger rates analysis using AOD/ESD
#
def prepareCostAlg(option = ''):

    log = logging.getLogger('prepareCostAlg')
    
    #
    # Configure output
    #
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()
        
    svcMgr.THistSvc.Output += ["TrigCostAthena DATAFILE='TrigCostAthena.root' OPT='RECREATE'"]

    #
    # Confugure cost algorithm and tools
    #
    alg = costMonitor.TrigCostAlg('TrigCost_'+option)
    log.info('Configuring TrigCostAlg with option: '+option)

    conf_tool = costMonitor.Trig__TrigNtConfTool('TrigCostNtConf')
    post_tool = costMonitor.Trig__TrigNtPostTool('TrigCostNtPost')
    vars_tool = costMonitor.Trig__TrigNtVarsTool('TrigCostNtVars')

    mbts_tool = costAthena.Trig__TrigNtMbtsTool  ('TrigCostNtMbts')
    anpi_tool = costAthena.Trig__TrigNtAnpITool  ('TrigCostNtAnpI')
    vrtx_tool = costAthena.Trig__TrigNtVertexTool('TrigCostNtVrtx')
    vstr_tool = costAthena.Trig__TrigNtStoreVertices('TrigCostNtStoreVertices')
    
    vstr_tool.outputNtuple = TrigCostAthenaFlags.StoreNtVerticesOutputFile.get_Value()
    
    vars_tool.collectTD    = True
    vars_tool.collectMC    = False
    vars_tool.collectPU    = False
    
    anpi_tool.registryXML  = ''
    anpi_tool.streamConfig = ''
    anpi_tool.streamEvent  = ''
    anpi_tool.outputFile   = 'TrigCostAthena.root'
    
    alg.mergeEvent  = False
    alg.printEvent  = True
    
    anpi_tool.OutputLevel = 3 

    alg.tools += [conf_tool,
                  vars_tool,
                  post_tool]

    if option.lower().count('mbts'):
        alg.tools += [mbts_tool]

    if (option.lower().count('vtx') or TrigCostAthenaFlags.DoStoreNtVertices==True):
        alg.tools += [vrtx_tool]
        alg.tools += [vstr_tool]
            
    alg.tools += [anpi_tool]
    return alg

#----------------------------------------------------------------------
# prepare vertex ntuple tool and load shared libraries
#
def prepareNtStoreVertices(name):
      return costAthena.Trig__TrigNtStoreVertices(name)

#----------------------------------------------------------------------
# prepare vertex tool and load shared libraries
#
def prepareVertexTool(name):    
    return costAthena.Trig__TrigNtVertexTool(name)

#----------------------------------------------------------------------
# prepare vertex tool and load shared libraries
#
def prepareInterfaceTool(name):
    
    loadTriggerNtuple('')

    anpi_tool = costAthena.Trig__TrigNtAnpITool(name)
    anpi_tool.streamConfig = 'TrigRateAL'
    anpi_tool.streamEvent  = 'TrigRateAL'
    
    return anpi_tool

#----------------------------------------------------------------------
# prepare TrigCostAlg configuration for trigger rates analysis using AOD/ESD
#
def loadTriggerNtuple(option = 'hist'):

    import os, ROOT, PyCintex
    ROOT.gSystem.Load('libTrigCostBase')
    ROOT.gSystem.Load('libTrigCostaseDict')
    ROOT.gSystem.Load('libTrigCostData')    
    ROOT.gSystem.Load('libTrigCostDataDict')

    if option.count('hist'):
        from ROOT import Anp    
        testArea = os.environ.get('TestArea')
        
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/TrigTimeAlg.xml')
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/TrigTimeChn.xml')
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/TrigTimeSeq.xml')
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/MakePuRates.xml')
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/PlotEvent.xml')
        Anp.HistMan.Instance().ReadFile(testArea+'/Trigger/TrigCost/TrigCostRate/config/PlotMBTS.xml')
    
        ROOT.gStyle.SetOptStat('rmeio')
        ROOT.gStyle.SetFillColor(10)
        ROOT.gStyle.SetFrameFillColor(10)
        ROOT.gStyle.SetCanvasColor(10)
        ROOT.gStyle.SetPadColor(10)
        ROOT.gStyle.SetTitleFillColor(10)
        ROOT.gStyle.SetStatColor(10)
        ROOT.gROOT.SetBatch(True)

#----------------------------------------------------------------------
# List of tools to be picked up TrigCostMonitor with CostAthena targets
#
CostAthenaToolsList = [ prepareVertexTool('VertexCostTool'),
                        prepareInterfaceTool('AnpInterfaceTool'),]
