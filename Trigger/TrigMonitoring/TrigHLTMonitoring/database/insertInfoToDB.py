# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sqlite3
conn = sqlite3.connect('test.db') #test.db is a database that gets created with ./createDB.sh

#You can browse the databse with:
#>sqlite3 test.db
#and then inside the interactive environment:
#.tables
#(see more options with .help)


c = conn.cursor()

c.execute("SELECT * FROM sig_config")
#r = c.fetchone()                                                                                
#print r                                                                                         

#This DB has five tables: master_table, mt2type, type, type2sig, sig_config                      

c.execute("INSERT INTO master_table VALUES (1, 'MCK1', 'Comment..')")
#c.execute("SELECT * FROM master_table")                                                         
#r = c.fetchone()                                                                                
#print r                                                                                         


# Fill up values for different signature groups -- these go in the sig_config table      
testTauConfig = """                                                                              
montool_config["TAU"] = {}                                                                       
montool_config["TAU"]["MonitoringTriggers"] = [                                                  
    "TauNoCut", "EF_tau45",                                                                      
]                                                                                                
montool_config["TAU"]["Config"] = []                                                             
montool_config["TAU"]["Config"] += [("HLTTauMon", {"EffOffTauPtCut" : 15.0,})]                   
montool_config["TAU"]["PkgName"]     = "TrigTauMonitoring.TrigTauMonitoringConfig"               
montool_config["TAU"]["CreatorName"] = "TrigTauMonitoringTool"                                   
"""

testEgammaConfig = """                                                                           
montool_config["EGAMMA"] = {}                                                                    
montool_config["EGAMMA"]["MonitoringTriggers"] = [ " ",]                                         
montool_config["EGAMMA"]["Config"] = []                                                          
montool_config["EGAMMA"]["Config"] += [("HLTEgammaMon", {"L2Et" : 3500.0,}),                     
("HLTEgammaNavMon", {"L1EmClus" : 3500.0,}),                                                     
("HLTEgammaNavSigTEMon", {"L1EmIsol" : 98000.0,}),                                               
]                                                                                                
montool_config["EGAMMA"]["PkgName"]     = "TrigEgammaMonitoring.TrigEgammaMonitoringConfig"      
montool_config["EGAMMA"]["CreatorName"] = "HLTEgammaMonitoringTool"                              
"""

testBjetConfig = """                                                                             
montool_config["BJET"] = {}                                                                      
montool_config["BJET"]["MonitoringTriggers"] = [ " ",]                                           
montool_config["BJET"]["Config"] = []                                                            
montool_config["BJET"]["Config"] += [("HLTBjetMon", {"Taggers" : ["IP1D", "IP2D", "IP3D"],})]    
montool_config["BJET"]["PkgName"]     = "TrigBjetMonitoring.TrigBjetMonitoringConfig"            
montool_config["BJET"]["CreatorName"] = "TrigBjetMonitoringConfig"                               
"""

testBphysConfig = """                                                                            
montool_config["BPHYS"] = {}                                                                     
montool_config["BPHYS"]["MonitoringTriggers"] = [ " ",]                                          
montool_config["BPHYS"]["Config"] = []                                                           
montool_config["BPHYS"]["Config"] += [("HLTBphysMon", {})]                                       
montool_config["BPHYS"]["PkgName"]     = "TrigBphysMonitoring.TrigBphysMonitoringConfig"         
montool_config["BPHYS"]["CreatorName"] = "TrigBphysMonitoringTool"                               
"""

testCaloRawConfig = """                                                                          
montool_config["CALO"] = {}                                                                      
montool_config["CALO"]["MonitoringTriggers"] = [ " ",]                                           
montool_config["CALO"]["Config"] = []                                                            
montool_config["CALO"]["Config"] += [("HLTCalo", {}), ("HLTCaloFEB", {"sigma" : 2,})]            
montool_config["CALO"]["PkgName"]     = "TrigCaloMonitoring.TrigCaloMonitoringConfig"            
montool_config["CALO"]["CreatorName"] = "HLTCaloMonitoringTool"                                  
"""

testCaloEsdConfig = """                                                                          
montool_config["CALO"] = {}                                                                      
montool_config["CALO"]["MonitoringTriggers"] = [ " ",]                                           
montool_config["CALO"]["Config"] = []                                                            
montool_config["CALO"]["Config"] += [("HLTCaloL2", {}),("HLTCaloCluster", {})]                   
montool_config["CALO"]["PkgName"]     = "TrigCaloMonitoring.TrigCaloMonitoringConfig"            
montool_config["CALO"]["CreatorName"] = "HLTCaloMonitoringTool"                                  
"""

testJetConfig = """                                                                              
montool_config["JET"] = {}                                                                       
montool_config["JET"]["MonitoringTriggers"] = [ " ",]                                            
montool_config["JET"]["Config"] = []                                                             
montool_config["JET"]["Config"] += [("HLTJetMon",{"EMFractionCut" : 0.8,} )]                     
montool_config["JET"]["PkgName"]     = "TrigJetMonitoring.TrigJetMonitoringConfig"               
montool_config["JET"]["CreatorName"] = "TrigJetMonitoringTool"                                   
"""
testMETConfig = """                                                                              
montool_config["MET"] = {}                                                                       
montool_config["MET"]["MonitoringTriggers"] = [ " ",]                                            
montool_config["MET"]["Config"] = []                                                             
montool_config["MET"]["Config"] += [("HLTMETMon", {}), ("HLTMETMon_FEB", {}), ("HLTMETMon_topocl", {}),]
montool_config["MET"]["PkgName"]     = "TrigMETMonitoring.TrigMETMonitoringConfig"               
montool_config["MET"]["CreatorName"] = "HLTMETMonitoringTool"                                    
"""
testMinBiasConfig = """                                                                          
montool_config["MINBIAS"] = {}                                                                   
montool_config["MINBIAS"]["MonitoringTriggers"] = [ " ",]                                        
montool_config["MINBIAS"]["Config"] = []                                                         
montool_config["MINBIAS"]["Config"] += [("MbtsMinBiasMon", {}), ("BcmMinBiasMon", {}), ("HLTMinBiasMon", {}),
("LucidMinBiasMon",{}), ("ZdcMinBiasMon",{})                                                     
]                                                                                                
montool_config["MINBIAS"]["PkgName"]     = "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig"   
montool_config["MINBIAS"]["CreatorName"] = "TrigMinBiasMonitoringTool"                           
"""
testMuonConfig = """                                                                             
montool_config["MUON"] = {}                                                                      
montool_config["MUON"]["MonitoringTriggers"] = [ " ",]                                           
montool_config["MUON"]["Config"] = []                                                            
montool_config["MUON"]["Config"] += [("HLTMuonMon", {"ZTPPtCone20RelCut" : 0.15,}),]             
montool_config["MUON"]["PkgName"]     = "TrigMuonMonitoring.TrigMuonMonitoringConfig"            
montool_config["MUON"]["CreatorName"] = "TrigMuonMonitoringTool"                                 
"""
testIDtrkConfig = """                                                                            
montool_config["IDTRK"] = {}                                                                     
montool_config["IDTRK"]["MonitoringTriggers"] = [ " ",]                                          
montool_config["IDTRK"]["Config"] = []                                                           
montool_config["IDTRK"]["Config"] += [("HLTIDpvtx", {"OnlineEfficiancyRangeCutY" : 0.02,}), ("HLTIDtrkDump", {"MinSiHits" : 7,}),
("tidatool", {}), ("MyAtlasExtrapolator", {}), ("MyCaloExtrapolatorTool", {}), ("HLTIDZeeTag", {})
]                                                                                                
montool_config["IDTRK"]["PkgName"]     = "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig"         
montool_config["IDTRK"]["CreatorName"] = "TrigIDtrkMonitoringTool"                               
"""

testIDJpsiConfig = """                                                                           
montool_config["IDJPSI"] = {}                                                                    
montool_config["IDJPSI"]["MonitoringTriggers"] = [ " ",]                                         
montool_config["IDJPSI"]["Config"] = []                                                          
montool_config["IDJPSI"]["Config"] += [("TrigJpsiMon", { }),]                                    
montool_config["IDJPSI"]["PkgName"]     = "TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig"      
montool_config["IDJPSI"]["CreatorName"] = "TrigIDJpsiMonitoringTool"                             
"""

# Simple script that could go in the third argument of the sig_config table                      
testTauScript = """                                                                              
print montool_config["TAU"]                                                                      
"""
testEgammaScript = """                                                                           
print montool_config["EGAMMA"]                                                                   
"""
testBjetScript = """                                                                             
print montool_config["BJET"]                                                                     
"""
testBphysScript = """                                                                            
print montool_config["BPHYS"]                                                                    
"""
testCaloRawScript = """                                                                          
print montool_config["CALO"]                                                                     
"""
testCaloEsdScript = """                                                                          
print montool_config["CALO"]                                                                     
"""
testJetScript = """                                                                              
print montool_config["JET"]                                                                      
"""
testMETScript = """                                                                              
print montool_config["MET"]                                                                      
"""
testMinBiasScript = """                                                                          
print montool_config["MINBIAS"]                                                                  
"""
testMuonScript = """                                                                             
print montool_config["MUON"]                                                                     
"""
testIDtrkScript = """                                                                            
print montool_config["IDTRK"]                                                                    
"""
testIDJpsiScript = """                                                                           
print montool_config["IDJPSI"]                                                                   
"""

# Put values in the database. This will go under the sig_config table with names "TAU" and "EGAMMA", etc.
c.execute("INSERT INTO sig_config VALUES (1,'TAU','ESD','"+testTauConfig+"','"+testTauScript+"')")
c.execute("INSERT INTO sig_config VALUES (2,'EGAMMA','ESD','"+testEgammaConfig+"','"+testEgammaScript+"')")
c.execute("INSERT INTO sig_config VALUES (3,'BJET','ESD','"+testBjetConfig+"','"+testBjetScript+"')")
c.execute("INSERT INTO sig_config VALUES (4,'BPHYS','ESD','"+testBphysConfig+"','"+testBphysScript+"')")
c.execute("INSERT INTO sig_config VALUES (5,'JET','ESD','"+testJetConfig+"','"+testJetScript+"')")
c.execute("INSERT INTO sig_config VALUES (6,'MET','ESD','"+testMETConfig+"','"+testMETScript+"')")
c.execute("INSERT INTO sig_config VALUES (7,'MINBIAS','ESD','"+testMinBiasConfig+"','"+testMinBiasScript+"')")
c.execute("INSERT INTO sig_config VALUES (8,'MUON','ESD','"+testMuonConfig+"','"+testMuonScript+"')")
c.execute("INSERT INTO sig_config VALUES (9,'IDTRK','ESD','"+testIDtrkConfig+"','"+testIDtrkScript+"')")
c.execute("INSERT INTO sig_config VALUES (10,'CALO','ESD','"+testCaloEsdConfig+"','"+testCaloEsdScript+"')")
c.execute("INSERT INTO sig_config VALUES (11,'CALO','RAW','"+testCaloRawConfig+"','"+testCaloRawScript+"')")
c.execute("INSERT INTO sig_config VALUES (12,'IDJPSI','ESD','"+testIDJpsiConfig+"','"+testIDJpsiScript+"')")
c.execute("INSERT INTO sig_config VALUES (13,'TAU','ESD','"+testTauConfig+"','"+testTauScript+"')")

# fill type table                                                                                                                                           
c.execute("INSERT INTO type VALUES (1,'Physics')")
c.execute("INSERT INTO type VALUES (2,'Standby')")
c.execute("INSERT INTO type VALUES (3,'Cosmics')")

# Create the correspondence between the master_table and type                                                                                               
c.execute("INSERT INTO mt2type VALUES (1,1)")
c.execute("INSERT INTO mt2type VALUES (1,2)")
c.execute("INSERT INTO mt2type VALUES (1,3)")

# Create the correspondence between type and sig_config                                                                                                     
c.execute("INSERT INTO type2sig VALUES (1,1)")
c.execute("INSERT INTO type2sig VALUES (1,2)")
c.execute("INSERT INTO type2sig VALUES (1,3)")
c.execute("INSERT INTO type2sig VALUES (1,4)")
c.execute("INSERT INTO type2sig VALUES (1,5)")
c.execute("INSERT INTO type2sig VALUES (1,6)")
c.execute("INSERT INTO type2sig VALUES (1,7)")
c.execute("INSERT INTO type2sig VALUES (1,8)")
c.execute("INSERT INTO type2sig VALUES (1,9)")
c.execute("INSERT INTO type2sig VALUES (1,10)")
c.execute("INSERT INTO type2sig VALUES (1,11)")
c.execute("INSERT INTO type2sig VALUES (1,12)")
c.execute("INSERT INTO type2sig VALUES (1,13)")

c.execute("INSERT INTO type2sig VALUES (2,1)")
c.execute("INSERT INTO type2sig VALUES (2,2)")
c.execute("INSERT INTO type2sig VALUES (2,3)")
c.execute("INSERT INTO type2sig VALUES (2,4)")
c.execute("INSERT INTO type2sig VALUES (2,5)")
c.execute("INSERT INTO type2sig VALUES (2,6)")
c.execute("INSERT INTO type2sig VALUES (2,7)")
c.execute("INSERT INTO type2sig VALUES (2,8)")
c.execute("INSERT INTO type2sig VALUES (2,9)")
c.execute("INSERT INTO type2sig VALUES (2,10)")
c.execute("INSERT INTO type2sig VALUES (2,11)")
c.execute("INSERT INTO type2sig VALUES (2,12)")
c.execute("INSERT INTO type2sig VALUES (2,13)")

c.execute("INSERT INTO type2sig VALUES (3,1)")
c.execute("INSERT INTO type2sig VALUES (3,2)")
c.execute("INSERT INTO type2sig VALUES (3,3)")
c.execute("INSERT INTO type2sig VALUES (3,4)")
c.execute("INSERT INTO type2sig VALUES (3,5)")
c.execute("INSERT INTO type2sig VALUES (3,6)")
c.execute("INSERT INTO type2sig VALUES (3,7)")
c.execute("INSERT INTO type2sig VALUES (3,8)")
c.execute("INSERT INTO type2sig VALUES (3,9)")
c.execute("INSERT INTO type2sig VALUES (3,10)")
c.execute("INSERT INTO type2sig VALUES (3,11)")
c.execute("INSERT INTO type2sig VALUES (3,12)")
c.execute("INSERT INTO type2sig VALUES (3,13)")

# Commit to the database the new entries and close the connection                                                                                           
conn.commit()
conn.close()
