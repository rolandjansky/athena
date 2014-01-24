# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sqlite3
conn = sqlite3.connect('test.db')
#This test.db now is the updated databased after having run the insertInfoToDB script.

c = conn.cursor()
#c.execute("SELECT * FROM sig_config")                                                                                                                      
c.execute("SELECT * FROM sig_config, type2sig WHERE type2sig.type_id=2 and type2sig.sig_config_id=sig_config.id")

montool_config = {}

for x in c.fetchall():
    print "Printing...", x
    testPythonCode = x[3]
    print "x[3]= ", x[3]
    exec testPythonCode
    testPythonCode2 = x[4]
    print "x[4]= ", x[4]
    exec testPythonCode2

print "All configuration info now is in this dictionary:\n",montool_config
#                                                                                                                                                           
#                                                                                                                                                           
from AthenaCommon.AppMgr import ToolSvc

# import tool instantiator functions                                                                                                                      
exec "from %s import %s" % (montool_config["TAU"]["PkgName"],montool_config["TAU"]["CreatorName"])
# create monitoring tools                                                                                                                                 
exec "%s()" % (montool_config["TAU"]["CreatorName"])

exec "from %s import %s" % (montool_config["EGAMMA"]["PkgName"],montool_config["EGAMMA"]["CreatorName"])
exec "%s()" % (montool_config["EGAMMA"]["CreatorName"])

exec "from %s import %s" % (montool_config["BJET"]["PkgName"],montool_config["BJET"]["CreatorName"])
exec "%s()" % (montool_config["BJET"]["CreatorName"])

exec "from %s import %s" % (montool_config["BPHYS"]["PkgName"],montool_config["BPHYS"]["CreatorName"])
exec "%s()" % (montool_config["BPHYS"]["CreatorName"])

exec "from %s import %s" % (montool_config["CALO"]["PkgName"],montool_config["CALO"]["CreatorName"])
exec "%s()" % (montool_config["CALO"]["CreatorName"])

exec "from %s import %s" % (montool_config["JET"]["PkgName"],montool_config["JET"]["CreatorName"])
exec "%s()" % (montool_config["JET"]["CreatorName"])

exec "from %s import %s" % (montool_config["MET"]["PkgName"],montool_config["MET"]["CreatorName"])
exec "%s()" % (montool_config["MET"]["CreatorName"])

exec "from %s import %s" % (montool_config["MINBIAS"]["PkgName"],montool_config["MINBIAS"]["CreatorName"])
exec "%s()" % (montool_config["MINBIAS"]["CreatorName"])

exec "from %s import %s" % (montool_config["MUON"]["PkgName"],montool_config["MUON"]["CreatorName"])
exec "%s()" % (montool_config["MUON"]["CreatorName"])

exec "from %s import %s" % (montool_config["IDTRK"]["PkgName"],montool_config["IDTRK"]["CreatorName"])
exec "%s()" % (montool_config["IDTRK"]["CreatorName"])

exec "from %s import %s" % (montool_config["IDJPSI"]["PkgName"],montool_config["IDJPSI"]["CreatorName"])
exec "%s()" % (montool_config["IDJPSI"]["CreatorName"])


### The following print statement shows the default values. See the value for the TauPTCut...                                                               
#print ToolSvc.HLTTauMon
#print ToolSvc.HLTEgammaMon                                                                                                                                 
#print ToolSvc.HLTEgammaNavMon                                                                                                                              
#print ToolSvc.HLTEgammaNavSigTEMon                                                                                                                         
#print ToolSvc.HLTBjetMon                                                                                                                                   
#print ToolSvc.HLTBphysMon
#print ToolSvc.HLTCaloL2                                                                                                                                    
#print ToolSvc.HLTCaloCluster                                                                                                                               
#print ToolSvc.TrigJpsiMon                                                                                                                                  
#print ToolSvc.HLTJetMon                                                                                                                                    
#print ToolSvc.HLTMETMon                                                                                                                                    
#print ToolSvc.HLTMETMon_FEB                                                                                                                                
#print ToolSvc.HLTMETMon_topocl                                                                                                                             
#print ToolSvc.MbtsMinBiasMon                                                                                                                               
#print ToolSvc.BcmMinBiasMon                                                                                                                                
#print ToolSvc.HLTMinBiasMon                                                                                                                                
#print ToolSvc.LucidMinBiasMon                                                                                                                              
#print ToolSvc.ZdcMinBiasMon                                                                                                                                
#print ToolSvc.HLTMuonMon                                                                                                                                   
#print ToolSvc.HLTIDpvtx                                                                                                                                    
#print ToolSvc.HLTIDtrkDump                                                                                                                                 
#print ToolSvc.tidatool 
#print ToolSvc.MyAtlasExtrapolator                                                                                                                          
#print ToolSvc.MyCaloExtrapolatorTool                                                                                                                       
#print ToolSvc.HLTIDZeeTag                                                                                                                                  

# modify default according to montool_config dictionary                                                                                                  

for (toolname, toolconfig) in montool_config['TAU']['Config']:                                                                                             
#for (toolname, toolconfig) in montool_config['EGAMMA']['Config']:                                                                                          
#for (toolname, toolconfig) in montool_config['BJET']['Config']:
#for (toolname, toolconfig) in montool_config['BPHYS']['Config']:
#for (toolname, toolconfig) in montool_config['JET']['Config']:                                                                                             
#for (toolname, toolconfig) in montool_config['MET']['Config']:                                                                                             
#for (toolname, toolconfig) in montool_config['MINBIAS']['Config']:
#for (toolname, toolconfig) in montool_config['MUON']['Config']:                                                                                            
#for (toolname, toolconfig) in montool_config['IDTRK']['Config']:                                                                                           
#for (toolname, toolconfig) in montool_config['CALO']['Config']:                                                                                            
#for (toolname, toolconfig) in montool_config['IDJPSI']['Config']:                                                                                          
    tool = getattr(ToolSvc,toolname)
    for confattr,confvalue in toolconfig.items():
        tool.__setattr__(confattr, confvalue)
    # Now you can see the updated value!
    print tool

# Another way to see the updated values:                                                                                                                      
print ToolSvc.HLTTauMon                                                                                                                                

conn.close()

