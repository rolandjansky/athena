# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import sqlite3
import hashlib
import cx_Oracle
conn = cx_Oracle.connect('tklimk/IiUTYhtOYL955!@devdb11')
print conn.version
#conn.close()



#tkconn = sqlite3.connect('test.db') #test.db is a database that gets created with ./createDB.sh

#You can browse the databse with:
#>sqlite3 test.db
#and then inside the interactive environment:
#.tables
#(see more options with .help)


c = conn.cursor()

#c.execute("SELECT * FROM sig_config")
#r = c.fetchone()                                                                                
#print r                                                                                         

#This DB has five tables: master_table, mt2type, type, type2sig, sig_config                      

#c.execute("INSERT INTO cool_table VALUES (1, 'SMK', 'Super Master Key')")

c.execute("INSERT INTO master_table VALUES (1, 'MCK', 'Monitoring Configuration Key')")
#c.execute("SELECT * FROM master_table")                                                         
#r = c.fetchone()                                                                                
#print r                                                                                         

#c.execute("INSERT INTO smk2mck VALUES (1,1)")

# Fill up values for different signature groups -- these go in the sig_config table      

mon_tau_config = {}                                                                       
mon_tau_config["MonitoringTriggers"] = [                                                  
    "TauNoCut", "EF_tau45",                                                                      
]                                                                                                
mon_tau_config["Config"] = []                                                             
mon_tau_config["Config"] += [("HLTTauMon", {"EffOffTauPtCut" : 15.0,})]                   
mon_tau_config["PkgName"]     = "TrigTauMonitoring.TrigTauMonitoringConfig"               
mon_tau_config["CreatorName"] = "TrigTauMonitoringTool"                                   
str_mon_tau_config_ = str(mon_tau_config)
#print "hash is ", hashlib.md5(str_mon_tau_config_).hexdigest()
hash_mon_tau_config=hashlib.md5(str_mon_tau_config_).hexdigest()
#hash_mon_tau_config=hash(str_mon_tau_config_)
str_mon_tau_config=str_mon_tau_config_.replace("'",'"')
#print "here", str_mon_tau_config

mon_tau_config2 = {}                                                                       
mon_tau_config2["MonitoringTriggers"] = [                                                  
    "TauNoCut", "EF_tau45",                                                                      
]                                                                                                
mon_tau_config2["Config"] = []                                                             
mon_tau_config2["Config"] += [("HLTTauMon", {"EffOffTauPtCut" : 17.0,})]                   
mon_tau_config2["PkgName"]     = "TrigTauMonitoring.TrigTauMonitoringConfig"               
mon_tau_config2["CreatorName"] = "TrigTauMonitoringTool"                                   
str_mon_tau_config2_ = str(mon_tau_config2)
#hash_mon_tau_config2=hash(str_mon_tau_config2_)
hash_mon_tau_config2=hashlib.md5(str_mon_tau_config2_).hexdigest()
str_mon_tau_config2=str_mon_tau_config2_.replace("'",'"')


mon_tau_config3 = {}                                                                       
mon_tau_config3["MonitoringTriggers"] = [                                                  
    "TauNoCut", "EF_tau45",                                                                      
]                                                                                                
mon_tau_config3["Config"] = []                                                             
mon_tau_config3["Config"] += [("HLTTauMon", {"EffOffTauPtCut" : 20.0,})]                   
mon_tau_config3["PkgName"]     = "TrigTauMonitoring.TrigTauMonitoringConfig"               
mon_tau_config3["CreatorName"] = "TrigTauMonitoringTool"                                   
str_mon_tau_config3_ = str(mon_tau_config3)
#hash_mon_tau_config3=hash(str_mon_tau_config3_)
hash_mon_tau_config3=hashlib.md5(str_mon_tau_config3_).hexdigest()
str_mon_tau_config3=str_mon_tau_config3_.replace("'",'"')


mon_egamma_config = {}                                                                       
mon_egamma_config["MonitoringTriggers"] = [ " ",]                                                   
mon_egamma_config["Config"] = []                                                             
mon_egamma_config["Config"] += [("HLTEgammaMon", {"L2Et" : 3500.0,}),                                                        
("HLTEgammaNavMon", {"L1EmClus" : 3500.0,}),                                                                                  
("HLTEgammaNavSigTEMon", {"L1EmIsol" : 98000.0,}),                                                                           
]                             
mon_egamma_config["PkgName"]     = "TrigEgammaMonitoring.TrigEgammaMonitoringConfig"               
mon_egamma_config["CreatorName"] = "HLTEgammaMonitoringTool"                                  
str_mon_egamma_config_ = str(mon_egamma_config)
#hash_mon_egamma_config=hash(str_mon_egamma_config_)
hash_mon_egamma_config=hashlib.md5(str_mon_egamma_config_).hexdigest()
str_mon_egamma_config=str_mon_egamma_config_.replace("'",'"')


mon_bjet_config = {}                                                                      
mon_bjet_config["MonitoringTriggers"] = [ " ",]                                           
mon_bjet_config["Config"] = []                                                            
mon_bjet_config["Config"] += [("HLTBjetMon", {"Taggers" : ["IP1D", "IP2D", "IP3D"],})]    
mon_bjet_config["PkgName"]     = "TrigBjetMonitoring.TrigBjetMonitoringConfig"            
mon_bjet_config["CreatorName"] = "TrigBjetMonitoringConfig"                               
str_mon_bjet_config_ = str(mon_bjet_config)
#hash_mon_bjet_config=hash(str_mon_bjet_config_)
hash_mon_bjet_config=hashlib.md5(str_mon_bjet_config_).hexdigest()
str_mon_bjet_config=str_mon_bjet_config_.replace("'",'"')

mon_bphys_config = {}                                                                     
mon_bphys_config["MonitoringTriggers"] = [ " ",]                                          
mon_bphys_config["Config"] = []                                                           
mon_bphys_config["Config"] += [("HLTBphysMon", {})]                                       
mon_bphys_config["PkgName"]     = "TrigBphysMonitoring.TrigBphysMonitoringConfig"         
mon_bphys_config["CreatorName"] = "TrigBphysMonitoringTool"                               
str_mon_bphys_config_ = str(mon_bphys_config)
#hash_mon_bphys_config=hash(str_mon_bphys_config_)
hash_mon_bphys_config=hashlib.md5(str_mon_bphys_config_).hexdigest()
str_mon_bphys_config=str_mon_bphys_config_.replace("'",'"')


mon_caloraw_config = {}                                                                      
mon_caloraw_config["MonitoringTriggers"] = [ " ",]                                           
mon_caloraw_config["Config"] = []                                                            
mon_caloraw_config["Config"] += [("HLTCalo", {}), ("HLTCaloFEB", {"sigma" : 2,})]            
mon_caloraw_config["PkgName"]     = "TrigCaloMonitoring.TrigCaloMonitoringConfig"            
mon_caloraw_config["CreatorName"] = "HLTCaloMonitoringTool"                                  
str_mon_caloraw_config_ = str(mon_caloraw_config)
#hash_mon_caloraw_config=hash(str_mon_caloraw_config_)
hash_mon_caloraw_config=hashlib.md5(str_mon_caloraw_config_).hexdigest()
str_mon_caloraw_config=str_mon_caloraw_config_.replace("'",'"')

mon_caloesd_config = {}                                                                      
mon_caloesd_config["MonitoringTriggers"] = [ " ",]                                           
mon_caloesd_config["Config"] = []                                                            
mon_caloesd_config["Config"] += [("HLTCaloL2", {}),("HLTCaloCluster", {})]                   
mon_caloesd_config["PkgName"]     = "TrigCaloMonitoring.TrigCaloMonitoringConfig"            
mon_caloesd_config["CreatorName"] = "HLTCaloMonitoringTool"                                  
str_mon_caloesd_config_ = str(mon_caloesd_config)
#hash_mon_caloesd_config=hash(str_mon_caloesd_config_)
hash_mon_caloesd_config=hashlib.md5(str_mon_caloesd_config_).hexdigest()
str_mon_caloesd_config=str_mon_caloesd_config_.replace("'",'"')

mon_jet_config = {}                                                                       
mon_jet_config["MonitoringTriggers"] = [ " ",]                                            
mon_jet_config["Config"] = []                                                             
mon_jet_config["Config"] += [("HLTJetMon",{"EMFractionCut" : 0.8,} )]                     
mon_jet_config["PkgName"]     = "TrigJetMonitoring.TrigJetMonitoringConfig"               
mon_jet_config["CreatorName"] = "TrigJetMonitoringTool"                                   
str_mon_jet_config_ = str(mon_jet_config)
#hash_mon_jet_config=hash(str_mon_jet_config_)
hash_mon_jet_config=hashlib.md5(str_mon_jet_config_).hexdigest()
str_mon_jet_config=str_mon_jet_config_.replace("'",'"')

mon_met_config = {}                                                                       
mon_met_config["MonitoringTriggers"] = [ " ",]                                            
mon_met_config["Config"] = []                                                             
mon_met_config["Config"] += [("HLTMETMon", {}), ("HLTMETMon_FEB", {}), ("HLTMETMon_topocl", {}),]
mon_met_config["PkgName"]     = "TrigMETMonitoring.TrigMETMonitoringConfig"               
mon_met_config["CreatorName"] = "HLTMETMonitoringTool"                                    
str_mon_met_config_ = str(mon_met_config)
#hash_mon_met_config=hash(str_mon_met_config_)
hash_mon_met_config=hashlib.md5(str_mon_met_config_).hexdigest()
str_mon_met_config=str_mon_met_config_.replace("'",'"')


mon_minbias_config = {}                                                                   
mon_minbias_config["MonitoringTriggers"] = [ " ",]                                        
mon_minbias_config["Config"] = []                                                         
#mon_minbias_config["Config"] += [("MbtsMinBiasMon", {}), ("BcmMinBiasMon", {}), ("HLTMinBiasMon", {}),
#("LucidMinBiasMon",{}), ("ZdcMinBiasMon",{})                                                     
#]                                                                                                
mon_minbias_config["Config"] += [("HLTMinBiasMon", {})]                                                                                               
mon_minbias_config["PkgName"]     = "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig"   
mon_minbias_config["CreatorName"] = "TrigMinBiasMonitoringTool"                           
str_mon_minbias_config_ = str(mon_minbias_config)
#hash_mon_minbias_config=hash(str_mon_minbias_config_)
hash_mon_minbias_config=hashlib.md5(str_mon_minbias_config_).hexdigest()
str_mon_minbias_config=str_mon_minbias_config_.replace("'",'"')

mon_muon_config = {}                                                                      
mon_muon_config["MonitoringTriggers"] = [ " ",]                                           
mon_muon_config["Config"] = []                                                            
mon_muon_config["Config"] += [("HLTMuonMon", {"ZTPPtCone20RelCut" : 0.15,}),]             
mon_muon_config["PkgName"]     = "TrigMuonMonitoring.TrigMuonMonitoringConfig"            
mon_muon_config["CreatorName"] = "TrigMuonMonitoringTool"                                 
str_mon_muon_config_ = str(mon_muon_config)
#hash_mon_muon_config=hash(str_mon_muon_config_)
hash_mon_muon_config=hashlib.md5(str_mon_muon_config_).hexdigest()
#print hash_mon_muon_config
#print mon_muon_config
#print str_mon_muon_config_
str_mon_muon_config=str_mon_muon_config_.replace("'",'"')

mon_idtrk_config = {}                                                                     
mon_idtrk_config["MonitoringTriggers"] = [ " ",]                                          
mon_idtrk_config["Config"] = []                                                           
mon_idtrk_config["Config"] += [("HLTIDpvtx", {"OnlineEfficiancyRangeCutY" : 0.02,}), ("HLTIDtrkDump", {"MinSiHits" : 7,}),
("tidatool", {}), ("MyAtlasExtrapolator", {}), ("MyCaloExtrapolatorTool", {}), ("HLTIDZeeTag", {})
]                                                                                                
mon_idtrk_config["PkgName"]     = "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig"         
mon_idtrk_config["CreatorName"] = "TrigIDtrkMonitoringTool"                               
str_mon_idtrk_config_ = str(mon_idtrk_config)
#hash_mon_idtrk_config=hash(str_mon_idtrk_config_)
hash_mon_idtrk_config=hashlib.md5(str_mon_idtrk_config_).hexdigest()
str_mon_idtrk_config=str_mon_idtrk_config_.replace("'",'"')

mon_idjpsi_config = {}                                                                    
mon_idjpsi_config["MonitoringTriggers"] = [ " ",]                                         
mon_idjpsi_config["Config"] = []                                                          
mon_idjpsi_config["Config"] += [("TrigJpsiMon", { }),]                                    
mon_idjpsi_config["PkgName"]     = "TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig"      
mon_idjpsi_config["CreatorName"] = "TrigIDJpsiMonitoringTool"                             
str_mon_idjpsi_config_ = str(mon_idjpsi_config)
#hash_mon_idjpsi_config=hash(str_mon_idjpsi_config_)
hash_mon_idjpsi_config=hashlib.md5(str_mon_idjpsi_config_).hexdigest()
str_mon_idjpsi_config=str_mon_idjpsi_config_.replace("'",'"')


# Simple script that could go in the third argument of the sig_config table                      

testTauScript = 'print "Test script" '
str_testTauScript = str(testTauScript)
hash_mon_tau_script=hashlib.md5(testTauScript).hexdigest()
#hash_mon_tau_config=hash(testTauScript)

testTauScript2 = 'print "Test script" '
str_testTauScript2=str(testTauScript2)
hash_mon_tau_script2=hashlib.md5(testTauScript2).hexdigest()

testEgammaScript = 'print "Test script" '
str_testEgammaScript =str(testEgammaScript)
hash_mon_egamma_script=hashlib.md5(testEgammaScript).hexdigest()

testBjetScript = 'print "Test script" '
str_testBjetScript=str(testBjetScript)
hash_mon_bjet_script=hashlib.md5(testBjetScript).hexdigest()

testBphysScript = 'print "Test script" '
str_testBphysScript=str(testBphysScript)
hash_mon_bphys_script=hashlib.md5(testBphysScript).hexdigest()

testCaloRawScript = 'print "Test script" '
str_testCaloRawScript = str(testCaloRawScript)
hash_mon_caloraw_script=hashlib.md5(testCaloRawScript).hexdigest()

testCaloEsdScript = 'print "Test script" '
str_testCaloEsdScript = str(testCaloEsdScript)
hash_mon_caloesd_script=hashlib.md5(testCaloEsdScript).hexdigest()

testJetScript = 'print "Test script" '
str_testJetScript = str(testJetScript)
hash_mon_jet_script=hashlib.md5(testJetScript).hexdigest()

testMETScript = 'print "Test script" '
str_testMETScript=str(testMETScript)
hash_mon_met_script=hashlib.md5(testMETScript).hexdigest()

testMinBiasScript = 'print "Test script" '
str_testMinBiasScript= str(testMinBiasScript)
hash_mon_minbias_script=hashlib.md5(testMinBiasScript).hexdigest()

testMuonScript = 'print "Test script" '
str_testMuonScript = str(testMuonScript)
hash_mon_muon_script=hashlib.md5(testMuonScript).hexdigest()

testIDtrkScript = 'print "Test script" '
str_testIDtrkScript= str(testIDtrkScript)
hash_mon_idtrk_script=hashlib.md5(testIDtrkScript).hexdigest()

testIDJpsiScript = 'print "Test script" '
str_testIDJpsiScript = str(testIDJpsiScript)
hash_mon_idjpsi_script=hashlib.md5(testIDJpsiScript).hexdigest()

# Put values in the database. This will go under the sig_config table with names "TAU" and "EGAMMA", etc.
c.execute("INSERT INTO sig_config VALUES (1,'TAU','ESD','"+str_mon_tau_config+"','"+str_testTauScript+"','"+hash_mon_tau_config+"','"+hash_mon_tau_script+"')")
c.execute("INSERT INTO sig_config VALUES (2,'EGAMMA','ESD','"+str_mon_egamma_config+"','"+str_testEgammaScript+"','"+hash_mon_egamma_config+"','"+hash_mon_tau_script+"')")
c.execute("INSERT INTO sig_config VALUES (3,'BJET','ESD','"+str_mon_bjet_config+"','"+str_testBjetScript+"','"+hash_mon_bjet_config+"','"+hash_mon_bjet_script+"')")
c.execute("INSERT INTO sig_config VALUES (4,'BPHYS','ESD','"+str_mon_bphys_config+"','"+str_testBphysScript+"','"+hash_mon_bphys_config+"','"+hash_mon_bphys_script+"')")
c.execute("INSERT INTO sig_config VALUES (5,'JET','ESD','"+str_mon_jet_config+"','"+str_testJetScript+"','"+hash_mon_jet_config+"','"+hash_mon_jet_script+"')")
c.execute("INSERT INTO sig_config VALUES (6,'MET','ESD','"+str_mon_met_config+"','"+str_testMETScript+"','"+hash_mon_met_config+"','"+hash_mon_met_script+"')")
c.execute("INSERT INTO sig_config VALUES (7,'MINBIAS','ESD','"+str_mon_minbias_config+"','"+str_testMinBiasScript+"','"+hash_mon_minbias_config+"','"+hash_mon_minbias_script+"')")
c.execute("INSERT INTO sig_config VALUES (8,'MUON','ESD','"+str_mon_muon_config+"','"+str_testMuonScript+"','"+hash_mon_muon_config+"','"+hash_mon_muon_script+"')")
c.execute("INSERT INTO sig_config VALUES (9,'IDTRK','ESD','"+str_mon_idtrk_config+"','"+str_testIDtrkScript+"','"+hash_mon_idtrk_config+"','"+hash_mon_idtrk_script+"')")
c.execute("INSERT INTO sig_config VALUES (10,'IDJPSI','ESD','"+str_mon_idjpsi_config+"','"+str_testIDJpsiScript+"','"+hash_mon_idjpsi_config+"','"+hash_mon_idjpsi_script+"')")
c.execute("INSERT INTO sig_config VALUES (11,'CALORAW','RAW','"+str_mon_caloraw_config+"','"+str_testCaloRawScript+"','"+hash_mon_caloraw_config+"','"+hash_mon_caloraw_script+"')")
c.execute("INSERT INTO sig_config VALUES (12,'CALOESD','ESD','"+str_mon_caloesd_config+"','"+str_testCaloEsdScript+"','"+hash_mon_caloesd_config+"','"+hash_mon_caloesd_script+"')")

#c.execute("INSERT INTO sig_config VALUES (11,'TAU','ESD','"+str_mon_tau_config2+"','"+str_testTauScript2+"','"+hash_mon_tau_config2+"','"+hash_mon_tau_script2+"')")
#c.execute("INSERT INTO sig_config VALUES (12,'TAU','ESD','"+str_mon_tau_config3+"','"+str_testTauScript2+"','"+hash_mon_tau_config3+"','"+hash_mon_tau_script2+"')")

# fill type table                                                                                                                                          
c.execute("INSERT INTO type_table VALUES (1,'Physics')")
c.execute("INSERT INTO type_table VALUES (2,'Standby')")
c.execute("INSERT INTO type_table VALUES (3,'Cosmics')")

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
#c.execute("INSERT INTO type2sig VALUES (1,13)")

c.execute("INSERT INTO type2sig VALUES (2,1)")
#c.execute("INSERT INTO type2sig VALUES (2,11)")
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
#c.execute("INSERT INTO type2sig VALUES (2,13)")

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
#c.execute("INSERT INTO type2sig VALUES (3,13)")

#c.execute("SELECT * FROM sig_config")
#r = c.fetchone()
#print r

# Commit to the database the new entries and close the connection                                                                                          
conn.commit()
conn.close()
