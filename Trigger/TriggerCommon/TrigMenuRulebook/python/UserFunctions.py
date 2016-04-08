# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
The rulebookProcessor will look here for two user defined functions:

modifyRules(rules, userArgs) which allows direct access to the
rulebook dictionary before the processor uses it.

modifyPrescales(prescales, userArgs) which allows direct access to the
trigger nodes and their prescales after the processor has finished all
its calculations.

It is expected that args will be filled with user specified arguments,
and is specified last on the command line as:

python processRules.py ... 'userArgs = {"argument1" : value1, "argument2" : value2}'

or

python runRuleBook.py ... 'userArgs = {"argument1" : value1, "argument2" : value2}'

qA dictionary is just one possibility. It is up to the user to
determine what data type should be held by args, and how the values
should be used in their functions.

"""
from RuleClasses import TriggerRule

def modifyRules(rules, userArgs=None):

  if userArgs == None:
    userArgs = {}

  if userArgs.get("B0Run", False):
    trigs = ['L1_MU0_J15_UNPAIRED_NONISO', 'L1_MU0_J15_FIRSTEMPTY', 'L1_MU0_J15_EMPTY', 'L1_MU0_J15_UNPAIRED_ISO', 'L1_2MU6_FIRSTEMPTY', 'L1_2MU6_EMPTY', 'L1_2MU6_UNPAIRED_ISO', 'L1_2MU6_UNPAIRED_NONISO', 'L1_MU10_UNPAIRED_ISO', 'L1_MU0_J30', 'L1_MU0_UNPAIRED_ISO', 'L1_MU0_J50', 'L1_2MU6', 'L1_2MU10', 'L1_3MU0', 'L1_2MU0_MU6', 'L1_MU0_J20_XE20', 'L1_MU15', 'L1_MU0', 'L1_MU6', 'L1_MU20', 'L1_MU11', 'L1_2MU0', 'L1_MU10_EMPTY', 'L1_2MU0_EMPTY', 'L1_MU0_J10', 'L1_MU10_J10', 'L1_MU0_FIRSTEMPTY', 'L1_MU20_FIRSTEMPTY', 'L1_MU0_EMPTY', 'L1_2MU0_FIRSTEMPTY', 'L1_MU10_FIRSTEMPTY', 'L1_MU6_FIRSTEMPTY', 'L1_MU11_EMPTY', 'L1_3MU6', 'L1_MU6_NL', 'L1_MU10_XE20', 'L1_MU0_UNPAIRED_NONISO', 'L1_2MU6_NL', 'L1_MU0_J15', 'L1_MU0_J75', 'L1_MU10']

    for trig in trigs:
      rules[trig] = {100: TriggerRule(PS=-1)}
  if userArgs.get("MU10", -1) > 0:
    rules['L1_MU10'] = {100: TriggerRule(PS=userArgs.get("MU10"))}

  

  ###################################
  # Brian's pseudo-tags
  ###################################
  #some handy shortcuts to modify rules (see bottom for code)
  disableEndOfFill=userArgs.get("disableEndOfFill", False)
  disablePriority1=userArgs.get("disablePriority1", False)
  disablePriority2=userArgs.get("disablePriority2", False)
  disablePending=userArgs.get("disablePending", False)
  disableNonCollision=userArgs.get("disableNonCollision", False)

  disableExtras=userArgs.get("disableExtras",False)
  
  disableStartup=userArgs.get("disableStartup", False)
  disableStreamers=userArgs.get("disableStreamers",False)
  
  firstBeam=userArgs.get("firstBeam", False)
  
  enableHighMult=userArgs.get("enableHighMult", False)
  removePassthrough=userArgs.get("removePassthrough",False)

  if removePassthrough:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
        if 'PTValue' in rules[r][k].keys():
          rules[r][k]['PTValue']=0
        if 'PTRate' in rules[r][k].keys():
          rules[r][k]['PTRate']=0


  if firstBeam:
    disableEndOfFill=True
    disableStartup=True
    disableStreamers=False

  if disableExtras:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
        if 'comment' in rules[r][k].keys() and ('Extra' in rules[r][k]['comment']):
          rules[r][k]['PS']=-1
        
  #disable end-of-fill triggers
  if disableEndOfFill:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
          if 'comment' in rules[r][k].keys() and ('end-of-fill' in rules[r][k]['comment']):
              rules[r][k]['PS']=-1
  #disable priority 1 triggers
  if disablePriority1:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
          if 'comment' in rules[r][k].keys() and ('Priority 1' in rules[r][k]['comment']):
              rules[r][k]['PS']=-1
  
  #disable priority 2 triggers
  if disablePriority2:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
          if 'comment' in rules[r][k].keys() and ('Priority 2' in rules[r][k]['comment']):
              rules[r][k]['PS']=-1
  
  #disable pending (non-approved) triggers
  if disablePending:
    for r in rules:
      keys=rules[r].keys()
      for k in keys:
          if 'comment' in rules[r][k].keys() and ('Pending' in rules[r][k]['comment']):
              rules[r][k]['PS']=-1
  
  #disable startup fill triggers
  if disableStartup:
    for r in rules:
      keys=rules[r].keys()
      if 1 in keys:
        del rules[r][1]
  
  
  #disable unpaired and firstempty
  if disableNonCollision:
    for r in rules:
      if 'unpaired' in r or 'firstempty' in r:
        keys=rules[r].keys()
        for k in keys:
          rules[r][k]['PS']=-1
  
  #disable unpaired and firstempty
  if disableStreamers:
    for r in rules:
      if 'Streamers' in r:
        keys=rules[r].keys()
        for k in keys:
          rules[r][k]['PS']=-1
  
  if enableHighMult:
    rules['L1_TE20'] = {1000: TriggerRule(rate=7000, scaling="bunches")}
    rules['EF_mbSpTrkVtxMh'] = {1000: TriggerRule(PS=1)}
    rules['EF_mbSpTrkVtxMh_medium'] = {1000: TriggerRule(PS=1)}
    rules['EF_mbSpTrkVtxMh_eff'] = {1000: TriggerRule(rate=10)}
    rules['L2_mbSpTrkVtxMh_eff'] = {1000: TriggerRule(PTRate=5)}
    rules['EF_mbSpTrkVtxMh_medium_eff'] = {1000: TriggerRule(rate=10)}
    rules['L2_mbSpTrkVtxMh_medium_eff'] = {1000: TriggerRule(PTRate=5)}
      
  ###################################
  # Changing rules on the fly
  ###################################
  if "overrideRate" in userArgs:
    for trigger, value in userArgs["overrideRate"].items():
      rules[trigger] = {1: TriggerRule(rate=value)}

  if "overridePS" in userArgs:
    for trigger, value in userArgs["overridePS"].items():
      rules[trigger] = {1: TriggerRule(PS=value)}

  pass


def modifyPrescales(prescales, userArgs=None):
  #  print 'HALLO',userArgs
  if userArgs.get("UnprescaledOnly", False):
    for item in prescales.itervalues():
      if item.ps!=1:
        item.ps=-1
      if 'EMPTY' in item.name:
        item.ps=-1
      if 'UNPAIRED' in item.name:
        item.ps=-1
      if not item.name.startswith('EF'): continue


      total_ps=item.total_ps()
      if item.ps!=1 or (item.nparents()==1 and item.parents[0].ps!=1): total_ps=-1
      if item.nparents()==1 and item.parents[0].name=='L1_RD0_FILLED': total_ps=-1
      if item.nparents()==1 and item.parents[0].nparents()==1 and item.parents[0].parents[0].name=='L1_RD0_FILLED': total_ps=-1
      #if total_ps!=1: item.ps=-1
      if total_ps!=1:
        print 'disable',item.name
        item.ps=-1
        if item.nparents()==1 and item.parents[0].ps!=1:
          item.parents[0].ps=-1
          if item.parents[0].nparents()==1 and item.parents[0].parents[0].ps!=1:
             item.parents[0].parents[0].ps=-1

import sys
def updateWithCheck(refdict, newdict, tag=None):
	for key in newdict.iterkeys():
		if refdict.has_key(key):
			print 'FATAL	Duplicated rule in {}, cannot be added {}:{}'.format(tag,key,newdict[key])
			print 'FATAL	Would overwrite {}:{}'.format(tag,key,refdict[key])
			sys.exit(0)
		else:
			refdict[key] = newdict[key]

def updateNoCheck(refdict, newdict, tag=None):
	for key in newdict.iterkeys():
		if refdict.has_key(key):
			print 'INFO 	Known duplicated rule in {}, will be added {}:{}'.format(tag,key,newdict[key])
			print 'INFO 	Will overwrite {}:{}'.format(key,refdict[key])
	refdict.update(newdict)
