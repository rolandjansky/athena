# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

print "hello from MaDQM.py"

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

if HLTMonFlags.doCalo and DQMonFlags.monManEnvironment == 'tier0Raw':
  # here we check which entry there is in the DB for this slice
  config_caloraw = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'CALORAW' , 'RAW') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_caloraw != None:
    for (toolname, toolconfig) in config_caloraw['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)


if HLTMonFlags.doCalo and DQMonFlags.monManEnvironment == 'tier0ESD':
  # here we check which entry there is in the DB for this slice
  config_caloesd = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'CALOESD' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_caloesd != None:
    for (toolname, toolconfig) in config_caloesd['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doTau:
  # here we check which entry there is in the DB for this slice
  config_tau = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'TAU' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_tau != None:
    for (toolname, toolconfig) in config_tau['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doBjet:
  # here we check which entry there is in the DB for this slice
  config_bjet = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'BJET' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_bjet != None:
    for (toolname, toolconfig) in config_bjet['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doBphys:
  # here we check which entry there is in the DB for this slice
  config_bphys = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'BPHYS' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_bphys != None:
    for (toolname, toolconfig) in config_bphys['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doMET:
  # here we check which entry there is in the DB for this slice
  config_met = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'MET' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_met != None:
    for (toolname, toolconfig) in config_met['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doJet:
  # here we check which entry there is in the DB for this slice
  config_jet = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'JET' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_jet != None:
    for (toolname, toolconfig) in config_jet['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doEgamma:
  # here we check which entry there is in the DB for this slice
  config_egamma = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'EGAMMA' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_egamma != None:
    for (toolname, toolconfig) in config_egamma['Config']:
        # modify defaults according to config dictionary
       tool = getattr(ToolSvc,toolname)
       for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doMuon:
  # here we check which entry there is in the DB for this slice
  config_muon = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'MUON' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_muon != None:
    for (toolname, toolconfig) in config_muon['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doIDtrk:
  # here we check which entry there is in the DB for this slice
  config_idtrk = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'IDTRK' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_idtrk != None:
    for (toolname, toolconfig) in config_idtrk['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

if HLTMonFlags.doMinBias:
  # here we check which entry there is in the DB for this slice
  config_minbias = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'MINBIAS' , 'ESD') 
  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
  if config_minbias != None:
    for (toolname, toolconfig) in config_minbias['Config']:
        # modify defaults according to config dictionary
        tool = getattr(ToolSvc,toolname)
        for confattr,confvalue in toolconfig.items():
            tool.__setattr__(confattr, confvalue)

# remove in anticipation of the removal of the obsolete IDJpsiMonitoring code
#
# if HLTMonFlags.doIDJpsiMon:
#  # here we check which entry there is in the DB for this slice
#  config_idjpsi = readSliceConfigFromDB('oracle','ACTIVE_KEY','Physics', 'IDJPSI' , 'ESD') 
#  # if entry is not None then we take monitoring config from DB, else it is taken from the release in a standard way 
#  if config_idjpsi != None:
#    for (toolname, toolconfig) in config_idjpsi['Config']:
#        # modify defaults according to config dictionary
#        tool = getattr(ToolSvc,toolname)
#        for confattr,confvalue in toolconfig.items():
#            tool.__setattr__(confattr, confvalue)

