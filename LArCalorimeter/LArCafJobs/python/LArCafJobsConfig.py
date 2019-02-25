# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from LArCafJobs.LArCafJobsConf import LArShapeDumperTool, LArShapeDumper
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker

triggersToCheck = [ 'EF_g[0-9].*',  'EF_.*_larcalib', 'EF_e[0-9].*', 'EF_j[0-9].*', 'EF_fj[0-9].*' ]

#FIXME: Auto-check number of samples

class DefaultShapeDumperTool(LArShapeDumperTool) :
  def __init__(self, name = 'LArShapeDumperTool', doShape = True) :
    super(LArShapeDumperTool, self).__init__(name)

    self.DoShape = doShape    
    if doShape:
      from LArConditionsCommon.LArCool import larcool
      if larcool is not None:
        nSamples=larcool.nSamples()
      else:
        print "WARNING, can't determine number of samples for this run! Assume 4"
        nSamples=4
        
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      svcMgr.PoolSvc.ReadCatalog += ["apcfile:poolcond/PoolCat_comcond_castor.xml"]
      from IOVDbSvc.CondDB import conddb

      #Run 1 version:
      #conddb.addFolder('LAR_OFL', '/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases<tag>LARElecCalibOflShapeRTM5samples3bins17phases-RUN2-UPD3-00</tag><key>LArShape5samples3bins17phases</key>')
      if (nSamples>=5):
        conddb.addFolder('LAR_OFL', '/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases<tag>LARElecCalibOflShapeRTM5samples3bins17phases-RUN2-UPD3-00</tag><key>LArShape17phases</key>')
      elif (nSamples==4):
        conddb.addFolder('LAR_OFL', '/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases<tag>LARElecCalibOflShapeRTM4samples3bins17phases-RUN2-UPD3-00</tag><key>LArShape17phases</key>')
      else:
        print "ERROR: Unexpected number of samples, got ",nSamples
        pass
      pass
    pass
  pass

class DefaultShapeDumper(LArShapeDumper) :

  def __init__(self, name = 'LArShapeDumper', digitsKey = 'FREE', noiseSignifCut = 3,
               doStream = False, doShape = True, doTrigger = True, doOFCIter = True, prescale = 1, 
               triggerNames = triggersToCheck, caloType = 'EMHECFCAL', dumpChannelInfos = False,
               addToAlgSeq = True) :
    super(LArShapeDumper, self).__init__(name)

    theDumperTool = DefaultShapeDumperTool(doShape=doShape)
    self.LArShapeDumperTool = theDumperTool 

    if doStream :
      from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
      StreamLArSamples = AthenaPoolOutputStream('StreamLArSamples', 'LArSamples.root', True)
      StreamLArSamples.MetadataItemList += ['LArSamplesContainer#Samples']
      StreamLArSamples.WriteOnExecute = False
      StreamLArSamples.WriteOnFinalize = True
    else :
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      svcMgr.AthenaSealSvc.DictNames += [ "LArCafJobsDict" ]

    self.CaloType = caloType
    self.Prescale = prescale
    self.NoiseSignifCut = noiseSignifCut
    self.DoTrigger = doTrigger
    self.DoStream = doStream
    self.DoOFCIter = doOFCIter
    self.DumpChannelInfos = dumpChannelInfos
    self.DumpDisconnected = dumpChannelInfos
    #self.ChannelsKey = 'LArRawChannels_FromDigits'
    self.ChannelsKey = 'LArRawChannels'
    self.TriggerNames = triggerNames

    from AthenaCommon.AppMgr import ToolSvc
    from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
    theADC2MeVTool = LArADC2MeVToolDefault()
    ToolSvc += theADC2MeVTool
    self.ADC2MeVTool = theADC2MeVTool

    if doShape :
      from IOVDbSvc.CondDB import conddb
      conddb.addFolder('LAR_OFL', '/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<tag>LARElecCalibOflAutoCorrsAutoCorr-RUN2-UPD3-00</tag>')
      # use the shapes without residual corrections by default (makes it easier to compute new residuals)
      conddb.addOverride('/LAR/ElecCalibOfl/Shape/RTM/5samples1phase','LARElecCalibOflShapeRTM5samples1phase-RUN2-UPD1-04')
    masker = LArBadChannelMasker('LArBadChannelMasker')
    masker.DoMasking=True
    masker.ProblemsToMask=[
         'deadReadout', 'deadPhys',
         'almostDead', 'short',
         'highNoiseHG','highNoiseMG','highNoiseLG'
           ]
    self.BadChannelMasker = masker

    self.DigitsKey = digitsKey

    if doOFCIter :
      from AthenaCommon.AppMgr import ToolSvc
      if hasattr(ToolSvc, 'LArRawChannelBuilderToolOFCIter') :
        ToolSvc.LArRawChannelBuilderToolOFCIter.StoreTiming = True

    if addToAlgSeq :
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence += self
      print self

  def setDefaults(self, handle):
    pass
