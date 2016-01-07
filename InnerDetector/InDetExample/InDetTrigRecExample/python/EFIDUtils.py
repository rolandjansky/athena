# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Various utilities not used in production
"""

___author___ = "Jiri Masik"
___version___ = "$Id: EFIDUtils.py,v 1.2 2008-10-27 22:37:15 masik Exp $"

def tooldiff():
  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon.AppMgr import ServiceMgr
  alltools = ToolSvc.getAllChildren()
  alltools += ServiceMgr.getAllChildren()
  toolmap = dict()
  for i in alltools:
    tooltype = i.configurables.keys()[0]
    toolitems = i.configurables.items()
    if toolmap.has_key(tooltype):
      if toolmap[tooltype]!=toolitems:
        print '%s seen before' % tooltype,
        print ' and differs'
        print 'previous contents: ', toolmap[tooltype]
        print 'new contents: ', toolitems
    else:
      toolmap[tooltype] =toolitems

  for i in toolmap.keys():
    print ' ';    print ' '
    print '******************************************************************************'
    print 'toolmap key ', i
    print '******************************************************************************'
    if len(toolmap[i])>1:
      firsttool = toolmap[i][1]
      prop = firsttool[1].getProperties()
      prop_differ= False
      diff_list = list()
      diffvals = dict()
      diffvals[firsttool] = prop 
      for j in toolmap[i]:
        print 'toolmap item ', j
        print j[1]
        print '    '
        if prop != j[1].getProperties():
          prop_differ=True
          jprop = j[1].getProperties()
          diffvals[j[1]] = jprop
          for p in prop.keys():
            if prop[p]!=jprop[p]:
              diff_list.append(p)
      if prop_differ:
        print 'Properties differ in ', set(diff_list)
        for kk in diffvals.keys():
          print kk, diffvals[kk]

    else:
      print 'single item'
    print '******************************************************************************'
  

def debuglevel(level=2):
  from AthenaCommon.AlgSequence           import AlgSequence
  job = AlgSequence()
  if hasattr(job,'TrigSteer_HLT'):
    steeringEF = job.TrigSteer_HLT
    steeringEF.OutputLevel=level
    import re
    exp=re.compile("EFID|FTF|IDTrig|FTK|L2ID")
    pokus=None
    reftoconf=list()
    reftoname=list()
    for alg in steeringEF.getAllChildren():
      try:
        algname = alg.name()
      except:
        print 'No algname for ', alg
        algname=''

      if exp.search(algname):
        print algname
        try:
          alg.OutputLevel=level
          pokus=alg
        except:
          pass
   
      for j in alg.properties().items():
        (aa,ab) = j
        if hasattr(type(ab),'__name__'):
          nm = type(ab).__name__
          if nm.find('ToolHandle')>-1 and nm.find('ToolHandleArray')==-1:
            print nm
            reftoconf.append(ab)
            reftoname.append(ab.getName())
   
    from AthenaCommon.AppMgr import ToolSvc
    for t in ToolSvc.getAllChildren():
      for it in reftoname:
        if t.getName().find(it)>-1:
          try:
            t.OutputLevel=level
          except:
            print 'Setting of outputlevel failed for ', t
          #print t


def muondebugoff():
  from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
  lvl=5
  #ServiceMgr.GeoModelSvc.MuonDetectorTool.OutputLevel=lvl
  try:
    ToolSvc.MdtROD_Decoder.OutputLevel=lvl
    ToolSvc.TgcROD_Decoder.OutputLevel=lvl
    ToolSvc.MdtPrepDataProviderTool.OutputLevel=lvl
    ToolSvc.RpcPrepDataProviderTool.OutputLevel=lvl
    ToolSvc.RpcROD_Decoder.OutputLevel=lvl
    ToolSvc.TgcPrepDataProviderToolc.OutputLevel=lvl
    ServiceMgr.MdtCalibrationSvc.OutputLevel=lvl
    ServiceMgr.MuonRPC_CablingSvc.OutputLevel=lvl

    ToolSvc.TrigDataAccess.OutputLevel=lvl

  except:
    pass
