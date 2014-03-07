# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, string ,re
import xmlrpclib
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen
from DQDefects import DefectsDB
  
#server = xmlrpclib.ServerProxy('https://cmarino:9a869871-f116-43ce-baa0-51dd41d9bf51@atlasdqm.cern.ch') #FIXME

try:
  serverfile=open("/afs/cern.ch/user/l/larmon/dqmAccess/passfile.txt","r")
  password=serverfile.readline().strip()
  serverfile.close()
except Exception,e:
  print "Failed to read xmlrpc server connection details from AFS location"
  print e
  sys.exit(-1)


def getLBsToIgnore(runnum,burstsFromCosmic=True,bulkProcessing=False): 

  # 1. Get LB range for this run
  minLB=0xFFFFFFFF
  maxLB=0
  
  since=(runnum<<32)+1
  until=(1+runnum)<<32

  trigdb=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
  if (trigdb is None):
    print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
    sys.exit(-1)
    
  fLB=trigdb.getFolder('/TRIGGER/LUMI/LBLB')
  itr = fLB.browseObjects(since,until,cool.ChannelSelection.all())
  while itr.goToNext():
    obj=itr.currentRef()
    run1=obj.since()>>32
    LB1=obj.since() & 0xFFFFFF
    if (run1==runnum):
      if (LB1 < minLB): minLB=LB1
      if (LB1 > maxLB): maxLB=LB1
  pass
  itr.close()
  trigdb.closeDatabase()

  if (minLB==0xFFFFFFFF or maxLB==0):
    print "Nothing found about run",runnum,"Wrong run number?"
    return set()

  print "Run %i goes from LB %i to LB %i" % (runnum,minLB,maxLB)
 
  # 2. Get LBs without "ATLAS-READY"
  badLBs=set()
  nReadyLBs=0
  nNotReadyLBs=0
  tdaqdb=indirectOpen('COOLONL_TDAQ/COMP200')
  if (tdaqdb is None):
    print "ERROR: Can't access COOLONL_TDAQ/COMP200"
    sys.exit(-1)
    
  fmode=tdaqdb.getFolder("/TDAQ/RunCtrl/DataTakingMode")  

  since=(runnum<<32)+minLB
  until=(runnum<<32)+maxLB+1

  itr=fmode.browseObjects(since,until,cool.ChannelSelection.all())
  while itr.goToNext():
    obj=itr.currentRef()
    pl=obj.payload()
    isReady=pl["ReadyForPhysics"]
    lb1=max(since,obj.since()) & 0xFFFFFFFF
    lb2=min(until,obj.until()) & 0xFFFFFFFF
    if not isReady:
      print "Ignoring LumiBlocks %i - %i not ATLAS READY" % (lb1,lb2)
      for lb in xrange(lb1,lb2):
        badLBs.add(lb)
      nNotReadyLBs+=(lb2-lb1)
    else:
      nReadyLBs+=(lb2-lb1)
    pass
  itr.close()
  tdaqdb.closeDatabase()
  
  #3. Get problematic LBs
  #3.1 Look for collisions in empty bunches - Fetch from DQ Web Server
  source = 'tier0'
  stream = 'physics_CosmicCalo'

  serverstring="https://larmon:%s@atlasdqm.cern.ch" % password
  server = xmlrpclib.ServerProxy(serverstring)
  multicall = xmlrpclib.MultiCall(server)

  # Look for the highest(latest) processing version of CosmicCalo by retrieving amitag
  run_spec = {'source': source, 'high_run': runnum, 'low_run': runnum}
  multicall.get_procpass_amitag_mapping(run_spec)
  results = multicall()
  proc = 0
  try:
    list = results[0][str(runnum)]
    for item in list:
      if ("f" in item[2] and bulkProcessing and "CosmicCalo" in item[1] and item[0]>proc):
        proc = 2
      if ("x" in item[2] and (not bulkProcessing) and "CosmicCalo" in item[1] and item[0]>proc):
        print item
        proc = 1
      pass
    pass
  except Exception,e:
    print "ERROR: can't retrieve the AMI Tag"
    print e

  if (proc == 0):
    print "I haven't found any processing version for CosmicCalo"


  try:
    multicall = xmlrpclib.MultiCall(server)
    run_spec = {'source': source, 'high_run': runnum, 'stream': stream, 'proc_ver': proc, 'low_run': runnum}
    multicall.get_timestamp(run_spec)
    results=multicall()
    timestamp=results[0][str(runnum)]
    from time import asctime,localtime
    print "DQM server timestamp:", asctime(localtime(timestamp))
    print "Now: ",asctime()
  except Exception,e:
    print "ERROR: can't get timestamp from DQM server"
    print e

  
  multicall = xmlrpclib.MultiCall(server)
  run_spec = {'source': source, 'high_run': runnum, 'stream': stream, 'proc_ver': proc, 'low_run': runnum}
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Collisions/LArCollTimeLumiBlockTimeCut')
  results = multicall()
  RE = re.compile(r'\((?P<lb>\S+)\.0*\)')

  try:
    list = results[0][str(runnum)]
    for item in list:
      if 'NBins' in item: continue
      m = RE.search(item).groupdict()
      lb=int(m['lb'])
      ncollisions=int(results[0][str(runnum)][item])
      if ncollisions > 50:
        badLBs.add(lb)
        print "LumiBlock %i ignored because it is empty bunches are polluted with collisions" % lb
      pass
    pass
  except Exception,e:
    print "ERROR: can't get LArCollTimeLumiBlockTimeCut from DQM server"
    print e

  if (burstsFromCosmic):# CosmicCalo stream : from the DQ web
    histoName = {'EMBC':'BarrelC','EMBA':'BarrelA','EMECC':'EMECC','EMECA':'EMECA'}
    for iPart in histoName.keys():
      multicall = xmlrpclib.MultiCall(server)
      multicall.get_dqmf_all_results(run_spec,'LAr/%s/Noise/Partition/NoisyEvent_TimeVeto_%s'%(iPart,histoName[iPart]))
      results = multicall()
      try:
        list = results[0][str(runnum)]
        #print "Got %i items for NoisyEvent_TimeVeto_%s" % (len(list),histoName[iPart])
        for item in list:
          if 'NBins' in item: continue
          m = RE.search(item).groupdict()
          lb=int(m['lb'])
          yieldbursts=float(results[0][str(runnum)][item])
          if yieldbursts > 0:
            badLBs.add(lb)
            print "LumiBlock %i ignored because it contains bursts in CosmicCalo stream in %s" % (lb,iPart)
          pass
        pass
      except Exception,e:
        print "ERROR: can't get NoisyEvent from DQM server"
        print e
      

  del multicall
  del server
  
  #3.2 Get defects from Defects DB
  db = DefectsDB()
  lar_defects = [d for d in (db.defect_names | db.virtual_defect_names) if d.startswith("LAR")]
  defects = db.retrieve((runnum, minLB), (runnum, maxLB), lar_defects)
  for defect in defects:
    part=defect.channel.split("_")[1]
    #3.2.1 Check for HV trip
    if "HVTRIP" in defect.channel and defect.present:
      for lb in range(defect.since.lumi,defect.until.lumi):
        badLBs.add(lb)
        print "LumiBlock %i ignored because of a HV trip in partition %s" % (lb,part)
      pass
    pass
    #3.2.2 Check for Noise Bursts from the defects
    if (not burstsFromCosmic):
      if not bulkProcessing:
        if "NOISEBURST" in defect.channel  and defect.present:
          for lb in range(defect.since.lumi,defect.until.lumi):
            badLBs.add(lb)
            print "LumiBlock %i ignored because of a noise burst in partition %s" % (lb,part)
          pass
        pass
      else: #not bulk processing
        if "SEVNOISEBURST" in defect.channel  and defect.present:
          for lb in range(defect.since.lumi,defect.until.lumi):
            badLBs.add(lb)
            print "LumiBlock %i ignored because of a severe noise burst in partition %s" % (lb,part)
          pass
        pass
        
  del db #Close Defects DB

  print "Found %i not-ready LBs and %i bad LBs among %i atlas-ready LBs" % (nNotReadyLBs,len(badLBs)-nNotReadyLBs,nReadyLBs)


  return badLBs



########################################################################

if __name__ == "__main__":
  if len(sys.argv) == 1 :
      print
      print "usage: python %s run"%(sys.argv[0])
      print
      sys.exit(1)

  run=int(sys.argv[1])
  bulk = False
  if (len(sys.argv)>2):
    if (sys.argv[2] == "b"):
      bulk= True
      
  if (bulk):
    print "Searching for bad lumi blocks in run %d for bulk processing"%run
  else:
    print "Searching for bad lumi blocks in run %d for express processing"%run

  badLBset=getLBsToIgnore(run,True,bulk)
  print "LBs to ignore:",sorted(badLBset)
#  badLBset2=getLBsToIgnore(run)
#  print "LBs to ignore:",sorted(badLBset)
