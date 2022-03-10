# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys, re
import xmlrpc.client
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen
from DQDefects import DefectsDB
  
try:
  serverfile=open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
  password=serverfile.readline().strip()
  serverfile.close()
except Exception as e:
  print ("Failed to read xmlrpc server connection details from AFS location")
  print (e)
  sys.exit(-1)


def getLBsToIgnore(runnum,burstsFromCosmic=True,bulkProcessing=False, dropNonReady=True): 

  badLBs=set()

  # 1. Get LB range for this run and LBs without "ATLAS-READY"
  nReadyLBs=0
  nNotReadyLBs=0
  tdaqdb=indirectOpen('COOLONL_TDAQ/CONDBR2')
  if (tdaqdb is None):
    print ("ERROR: Can't access COOLONL_TDAQ/CONDBR2")
    sys.exit(-1)
    
  fmode=tdaqdb.getFolder("/TDAQ/RunCtrl/DataTakingMode")  

  since=(runnum<<32)+1
  until=((1+runnum)<<32)-1
  maxLb=0
  minLb=1
  itr=fmode.browseObjects(since,until,cool.ChannelSelection.all())
  while itr.goToNext():
    obj=itr.currentRef()
    pl=obj.payload()
    isReady=pl["ReadyForPhysics"]
    lb1=max(since,obj.since()) & 0xFFFFFFFF
    ts2=obj.until()
    if ts2<until: #ignore the IOV beyond the end of the run
      lb2=ts2 & 0xFFFFFFFF
      if lb2>maxLb:
        maxLb=lb2
      if not isReady:
        if dropNonReady:
          print ("Ignoring LumiBlocks %i - %i not ATLAS READY" % (lb1,lb2))
          badLBs.update(range(lb1,lb2))
        nNotReadyLBs+=(lb2-lb1)
      else:
        nReadyLBs+=(lb2-lb1)
        pass
      pass
    pass
  pass
  itr.close()
  tdaqdb.closeDatabase()
  
  print ("Run %i goes up to LB %i" % (runnum,maxLb))

  #2. Get problematic LBs
  #2.1 Look for collisions in empty bunches - Fetch from DQ Web Server
  source = 'tier0'
  stream = 'physics_CosmicCalo'

  serverstring="https://%s@atlasdqm.cern.ch" % password
  server = xmlrpc.client.ServerProxy(serverstring)
  multicall = xmlrpc.client.MultiCall(server)

  # Look for the highest(latest) processing version of CosmicCalo by retrieving amitag
  run_spec = {'source': source, 'high_run': runnum, 'low_run': runnum}
  multicall.get_procpass_amitag_mapping(run_spec)
  results = multicall()
  if len(results[0])==0:
    print ("Nothing found about run",runnum,"on DQM server")
  proc = 0
  try:
    list = results[0][str(runnum)]
    for item in list:
      if ("f" in item[2] and bulkProcessing and "CosmicCalo" in item[1] and item[0]>proc):
        proc = 2
      if ("x" in item[2] and (not bulkProcessing) and "CosmicCalo" in item[1] and item[0]>proc):
        print (item)
        proc = 1
      pass
    pass
  except Exception as e:
    print ("ERROR: can't retrieve the AMI Tag")
    print (e)

  if (proc == 0):
    print ("I haven't found any processing version for CosmicCalo. Assume express processing")
    proc=1 


  try:
    multicall = xmlrpc.client.MultiCall(server)
    run_spec = {'source': source, 'high_run': runnum, 'stream': stream, 'proc_ver': proc, 'low_run': runnum}
    multicall.get_timestamp(run_spec)
    results=multicall()
    timestamp=results[0][str(runnum)]
    from time import asctime,localtime
    print ("DQM server timestamp:", asctime(localtime(timestamp)))
    print ("Now: ",asctime())
  except Exception as e:
    print ("ERROR: can't get timestamp from DQM server")
    print (e)

  
  multicall = xmlrpc.client.MultiCall(server)
  run_spec = {'source': source, 'high_run': runnum, 'stream': stream, 'proc_ver': proc, 'low_run': runnum}
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Collisions-Bkg/LArCollTimeLumiBlockTimeCut')
  results = multicall()
  RE = re.compile(r'\((?P<lb>\S+)\.0*\)')

  try:
    list = results[0][str(runnum)]
    for item in list:
      if 'NBins' in item:
        continue
      m = RE.search(item).groupdict()
      lb=int(m['lb'])
      ncollisions=int(results[0][str(runnum)][item])
      if ncollisions > 50:
        badLBs.add(lb)
        print ("LumiBlock %i ignored because it is empty bunches are polluted with collisions" % lb)
      pass
    pass
  except Exception as e:
    print ("ERROR: can't get LArCollTimeLumiBlockTimeCut from DQM server")
    print (e)

  if (burstsFromCosmic):# CosmicCalo stream : from the DQ web
    histoName = {'EMBC':'BarrelC','EMBA':'BarrelA','EMECC':'EMECC','EMECA':'EMECA'}
    for iPart in histoName.keys():
      multicall = xmlrpc.client.MultiCall(server)
      #multicall.get_dqmf_all_results(run_spec,'LAr/%s/Noise/Partition/NoisyEvent_TimeVeto_%s'%(iPart,histoName[iPart]))
      multicall.get_dqmf_all_results(run_spec,'/LAr/%s/Occupancy-Noise/Noise_Burst/NoisyEvent_TimeVeto_%s'%(iPart,iPart))

      results = multicall()
      try:
        resultlist = results[0][str(runnum)]
        #print ("Got %i items for NoisyEvent_TimeVeto_%s" % (len(list),histoName[iPart]))
        for item in resultlist:
          if 'NBins' in item:

            continue
          m = RE.search(item).groupdict()
          lb=int(m['lb'])
          yieldbursts=float(results[0][str(runnum)][item])
          if yieldbursts > 0:
            badLBs.add(lb)
            print ("LumiBlock %i ignored because it contains bursts in CosmicCalo stream in %s" % (lb,iPart))
          pass
        pass
      except Exception as e:
        print ("ERROR: can't get NoisyEvent from DQM server")
        print (e)
      

  del multicall
  del server
  
  #3.2 Get defects from Defects DB
  db = DefectsDB()
  lar_defects = [d for d in (db.defect_names | db.virtual_defect_names) if d.startswith("LAR")]
  defects = db.retrieve((runnum, minLb), (runnum, maxLb), lar_defects)
  for defect in defects:
    part=defect.channel.split("_")[1]
    #3.2.1 Check for HV trip
    if "HVTRIP" in defect.channel and defect.present:
      for lb in range(defect.since.lumi,defect.until.lumi):
        badLBs.add(lb)
        print ("LumiBlock %i ignored because of a HV trip in partition %s" % (lb,part))
      pass
    pass
    #3.2.2 Check for Noise Bursts from the defects
    if (not burstsFromCosmic):
      if not bulkProcessing:
        if "NOISEBURST" in defect.channel  and defect.present:
          for lb in range(defect.since.lumi,defect.until.lumi):
            badLBs.add(lb)
            print ("LumiBlock %i ignored because of a noise burst in partition %s" % (lb,part))
          pass
        pass
      else: #not bulk processing
        if "SEVNOISEBURST" in defect.channel  and defect.present:
          for lb in range(defect.since.lumi,defect.until.lumi):
            badLBs.add(lb)
            print ("LumiBlock %i ignored because of a severe noise burst in partition %s" % (lb,part))
          pass
        pass
        
  del db #Close Defects DB

  nBadLBs=len(badLBs)
  if dropNonReady:
    nBadLBs=nBadLBs-nNotReadyLBs

  print ("Found %i not-ready LBs, %i atlas-ready LBs and %i bad LBs" % (nNotReadyLBs,nReadyLBs,nBadLBs))

  return badLBs



########################################################################

if __name__ == "__main__":
  import getopt
  if len(sys.argv) == 1 :
      print ()
      print ("usage: python %s <options> <runnumber> "%(sys.argv[0]))
      print ()
      sys.exit(1)

  burstsFromCosmic=True
  bulkProcessing=False
  dropNonReady=True

  outputFN=None

  opts,args=getopt.getopt(sys.argv[1:],"brco:",[])
  for o,a in opts:
    if (o=='-c'):
      burstsFromCosmics=False
    if (o=='-b'):
      bulkProcessing=True
    if (o=='-r'):
      dropNonReady=False
    if (o=='-o'):
      outputFN=a

  if len(args)<0:
    print ("No run number found")
    sys.exit(-1)

  if len(args)>1:
    print ("Too many arguments")
    sys.exit(-1)

  run=int(args[0])
      
  if (bulkProcessing):
    print ("Searching for bad lumi blocks in run %d for bulk processing"%run)
  else:
    print ("Searching for bad lumi blocks in run %d for express processing"%run)

  if (dropNonReady):
    print ("LB not marked as AtlasReady will be considered bad")
  else:
    print ("LB not marked as AtlasReady will be considered good")

    

  badLBset=getLBsToIgnore(run,burstsFromCosmic,bulkProcessing, dropNonReady)

  badLBsorted=sorted(badLBset)
  print ("LBs to ignore:",badLBsorted)

  if outputFN is not None:
    out=open(outputFN,"w")
    out.write(', '.join([ str(i) for i in badLBsorted ]))
    out.write("\n")
    out.close()


#  badLBset2=getLBsToIgnore(run)
#  print ("LBs to ignore:",sorted(badLBset))
