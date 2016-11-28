from TrigHLTMonitoring.TrigHLTMonitoringConfig import HLTGeneralTool
for toolname in HLTGeneralTool():
    tool1 = getattr(ToolSvc, toolname.split('/')[-1])
    #    tool1.histoPathBase      = "/Trigger/HLT"                                                                                                                                                    
    #    tool1.OutputLevel = WARNING                                                                                                                                                                  
    #tool1.DetailLevel = 10
    #in case something is wrong with the matching below check and fix here: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigMonitoring/TrigHLTMonitoring/trunk/python/TrigHLTMonitoringConfig.py                     
    #    tool1.RegExes = {                                                                                                                                                                            
    #                     'AllChains' : 'HLT_.*',                                                                                                                                                     
    #                     'Electrons' : 'HLT_e[0-9]+.*',                                                                                                                                              
    #                     'Gamma'     : 'HLT_g[0-9]+.*',                                                                                                                                              
    #                     'Muons'     : 'HLT_[0-9]*mu[0-9]+.*',                                                                                                                                       
    #                     'MinBias'   : 'HLT_mb.*',                                                                                                                                                   
    #                     'MissingET' : 'HLT_(t|x)e[0-9]+.*',                                                                                                                                         
    #                     'Taus'      : 'HLT_(tau[0-9]*|trk.*Tau).*',                                                                                                                                 
    #                     'Jets'      : 'HLT_(FJ|j)[0-9]+.*',                                                                                                                                         
    #                     }                                                                                                                                                                           
    tool1.EnableLumi = False
    ToolSvc += tool1
    monMan.AthenaMonTools += [tool1]
