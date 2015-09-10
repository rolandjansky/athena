# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

print "hello from MaDQM.py"

def getAndSetConfig(dbconnection, MCK, runtype, slice_name, reco_step, *vargs, **kwargs):
  # here we check which entry there is in the DB for this slice
  config_thisFunction = readSliceConfigFromDB(dbconnection, MCK, runtype, slice_name, reco_step) 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_thisFunction != None:
    for (toolname, toolconfig) in config_thisFunction['Config']:
      # Ben: if the tool is not loaded into ToolSvc then we best load it now!
      if hasattr(ToolSvc,toolname) == False:
        exec "from %s import %s" % (config_thisFunction["PkgName"],config_thisFunction["CreatorName"])
        exec "%s()" % (config_thisFunction["CreatorName"])
      # modify defaults according to config dictionary
      tool = getattr(ToolSvc,toolname)
      for confattr,confvalue in toolconfig.items():
        tool.__setattr__(confattr, confvalue)

# read monitoring configureation from the database
# calling readSliceConfigFromDB(dbconnection, MCK, runtype, slicename, recostep) returns monitoring configuration
# dbconnection is 'oracle'
# MCK can be: any MCK number or
#             'ACTIVE_KEY' or
#             'LAST_MCK'      

# runtype can be 'Physics', 'Standby' or 'Cosmics'
# slicename: 'TAU','EGAMMA','JET','BJET','BPHYS','MET','MINBIAS','MUON','IDTRK','IDJPSI', 'CALORAW', 'CALOESD'
# recostep: 'RAW' or 'ESD'

include("TrigHLTMonitoring/readConfigFromDB.py")

# if HLTMonFlags.doCalo and DQMonFlags.monManEnvironment == 'tier0Raw':
#   # here we check which entry there is in the DB for this slice
#   config_caloraw = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'CALORAW' , 'RAW') 
#   # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
#   if config_caloraw != None:
#     for (toolname, toolconfig) in config_caloraw['Config']:
#       #print "BENtest1:"
#       #print "ToolSvc._Configurable__children",ToolSvc._Configurable__children
#       #print "config_caloraw[\"PkgName\"]",config_caloraw["PkgName"]
#       #print "config_caloraw[\"CreatorName\"]",config_caloraw["CreatorName"]
#       exec "from %s import %s" % (config_caloraw["PkgName"],config_caloraw["CreatorName"])
#       exec "%s()" % (config_caloraw["CreatorName"])
#       #print "BENtest2:"
#       #print "ToolSvc._Configurable__children",ToolSvc._Configurable__children
#       #print "hasattr(ToolSvc,\"HLTCalo\")",hasattr(ToolSvc,"HLTCalo")
#       # modify defaults according to config dictionary
#       tool = getattr(ToolSvc,toolname)
#       for confattr,confvalue in toolconfig.items():
#         tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doCalo and DQMonFlags.monManEnvironment == 'tier0Raw':
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'CALORAW' , 'RAW')

if HLTMonFlags.doCalo and DQMonFlags.monManEnvironment == 'tier0ESD':
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'CALOESD' , 'ESD')

if HLTMonFlags.doTau:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'TAU' , 'ESD')

if HLTMonFlags.doBjet:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'BJET' , 'ESD')

if HLTMonFlags.doBphys:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'BPHYS' , 'ESD')

if HLTMonFlags.doMET:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'MET' , 'ESD')

if HLTMonFlags.doJet:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'JET' , 'ESD')

if HLTMonFlags.doEgamma:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'EGAMMA' , 'ESD')

if HLTMonFlags.doMuon:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'MUON' , 'ESD')

if HLTMonFlags.doIDtrk:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'IDTRK' , 'ESD')

if HLTMonFlags.doMinBias:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'MINBIAS' , 'ESD')

if HLTMonFlags.doIDJpsiMon:
  getAndSetConfig('oracle','ACTIVE_KEY','Physics', 'IDJPSI' , 'ESD')
