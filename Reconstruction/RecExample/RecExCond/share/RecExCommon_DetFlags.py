include.block("RecExCond/RecExCommon_DetFlags.py")
from AthenaCommon.Logging import logging
logRecExCommon_DetFlags = logging.getLogger( 'RecExCommon_DetFlags' )

from AthenaCommon.Resilience import treatException
if 'DetFlags' in dir():
   logRecExCommon_DetFlags.info("DetFlags already defined.by user : user should have fully configured it already! ")
else:

   from AthenaCommon.GlobalFlags import globalflags

   # include DetFlags
   # by default everything is off
   from AthenaCommon.DetFlags import DetFlags
   # switch on tasks for all detectors
   DetFlags.detdescr.all_setOn()
   # switch on dcs for all detectors
   try:
      # need AthenaCommon
      DetFlags.dcs.all_setOn()   
   except Exception:
      logRecExCommon_DetFlags.warning("could not set DetFlags.dcs")
      pass

   from RecExConfig.RecFlags  import rec
   if  not rec.readESD() and not rec.readAOD() and not rec.doWriteRDO() :
      DetFlags.makeRIO.all_setOn()
   else:
      DetFlags.makeRIO.all_setOff()
      #calo cells are there on ESD
      if rec.readESD() :
         DetFlags.readRIOPool.Calo_setOn()
         # savannah 73686
         from RecExConfig.AutoConfiguration import IsInInputFile
         from InDetRecExample.InDetKeys import InDetKeys
         if IsInInputFile('InDet::PixelClusterContainer',InDetKeys.PixelClusters()):
            DetFlags.readRIOPool.pixel_setOn()
         if IsInInputFile('InDet::SCT_ClusterContainer',InDetKeys.SCT_Clusters()):
            DetFlags.readRIOPool.SCT_setOn()
         if IsInInputFile('InDet::TRT_DriftCircleContainer',InDetKeys.TRT_DriftCircles()):
            DetFlags.readRIOPool.TRT_setOn()
         if rec.doESD():
            if IsInInputFile('PixelRDO_Container',InDetKeys.PixelRDOs()):
               DetFlags.makeRIO.pixel_setOn()
            if IsInInputFile('SCT_RDO_Container',InDetKeys.SCT_RDOs()):
               DetFlags.makeRIO.SCT_setOn()
            if IsInInputFile('TRT_RDO_Container',InDetKeys.TRT_RDOs()):
               DetFlags.makeRIO.TRT_setOn()
               
         DetFlags.readRIOPool.Muon_setOn()
         
   if rec.readRDO:      
      if  globalflags.InputFormat=='pool':
         DetFlags.readRDOPool.all_setOn()
         DetFlags.readRDOBS.all_setOff()
         DetFlags.readRIOBS.all_setOff()
         from RecExConfig.AutoConfiguration import IsInInputFile
         from InDetRecExample.InDetKeys import InDetKeys
         if IsInInputFile('InDet::PixelClusterContainer',InDetKeys.PixelClusters()):
            DetFlags.readRIOPool.pixel_setOn()
            DetFlags.makeRIO.pixel_setOff()
         if IsInInputFile('InDet::SCT_ClusterContainer',InDetKeys.SCT_Clusters()):
            DetFlags.readRIOPool.SCT_setOn()
            DetFlags.makeRIO.SCT_setOff()
         if IsInInputFile('InDet::TRT_DriftCircleContainer',InDetKeys.TRT_DriftCircles()):
            DetFlags.readRIOPool.TRT_setOn()
            DetFlags.makeRIO.TRT_setOff()
      elif globalflags.InputFormat=='bytestream':
         DetFlags.readRDOPool.all_setOff()
         DetFlags.readRDOBS.all_setOn()
   else:
      DetFlags.readRDOPool.all_setOff()
      DetFlags.readRDOBS.all_setOff()

   
#synch muon flags to detflags
try:
   from MuonRecExample.MuonRecFlags import muonRecFlags
   muonRecFlags.sync_DetFlags()
except Exception:
   treatException ("Could not synch DetFlags to muonRecFlags")
