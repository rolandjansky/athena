from IOVDbSvc.CondDB import conddb
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecAlgsFlags import recAlgs
########################################

#use MC MDT RT functions
#conddb.blockFolder("/MDT/RT")
#conddb.addFolderWithTag("MDT_OFL","/MDT/RT","MDTRT_Sim-04",force=True,forceMC=True)
conddb.blockFolder("/MDT/RTBLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/RTBLOB","MDTRT_Sim-04-BLOB",force=True,forceMC=True)

#from Guillaume
conddb.blockFolder("/LAR/LArCellPositionShift");
conddb.addFolderWithTag("LAR_OFL","/LAR/LArCellPositionShift","LArCellPositionShift-ideal",force=True,forceMC=True); 
conddb.addOverride("/LAR/ElecCalibOfl/Shape/RTM/4samples1phase","LARElecCalibOflShapeRTM4samples1phase-RUN2-UPD4-00")
conddb.addOverride("/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase","LARElecCalibOflOFCPhysWaveRTM4samples1phase-RUN2-UPD4-00")
svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:"+"/cvmfs/atlas-condb.cern.ch/repo/conditions/poolcond/PoolFileCatalog.xml"]

conddb.blockFolder("/PIXEL/ReadoutSpeed")
conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed",force=True)

conddb.blockFolder("/TRT/Cond/DigVers")
conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)


###################################################
print "RT OVERRIDE, for OFLCOND-MC16-SDR-26 MC condition tag "
#See https://atlas-tagservices.cern.ch/tagservices/RunBrowser/runBrowserReport/rBR_CB_Report.php?CBAction=GlobalTagReport&cbgt=OFLCOND-MC16-SDR-26
# https://atlas-tagservices.cern.ch/tagservices/RunBrowser/runBrowserReport/rBR_CB_Report.php?CBAction=GlobalTagReport&cbgt=CONDBR2-BLKPA-2018-12

conddb.blockFolder("/LAR/BadChannels/BadChannels");
conddb.blockFolder("/LAR/BadChannelsOfl/BadChannels");
conddb.addFolderWithTag("LAR_OFL","/LAR/BadChannels/BadChannels","LArBadChannelsBadChannels-IOVDEP-08",force=True,forceMC=True);
conddb.blockFolder("/LAR/BadChannels/MissingFEBs");
conddb.blockFolder("/LAR/BadChannelsOfl/MissingFEBs");
conddb.addFolderWithTag("LAR_OFL","/LAR/BadChannels/MissingFEBs","LArBadChannelsMissingFEBs-IOVDEP-04",force=True,forceMC=True)

if not "EOJT_alignMC" in globals():
    print "EOJT_alignMC not found in globals(), so aligning ID to data conditions"
    #conddb.blockFolder("/TRT/Align")
    #conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign-RUN2-BLK-UPD4-10",force=True,forceData=True)
    #conddb.blockFolder("/LAR/Align")
    #conddb.addFolderWithTag("LAR_ONL","/LAR/Align","LARAlign-RUN2-UPD4-02",force=True,forceData=True)
    #conddb.blockFolder("/Indet/Align")
    #conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-RUN2-BLK-UPD4-13",force=True,forceData=True)
else:
    print "EOJT_alignMC found in globals(), so aligning ID to MC conditions"
    conddb.blockFolder("/TRT/Align")
    conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign-RUN2-BLK-UPD4-13",force=True,forceMC=True)
    conddb.blockFolder("/LAR/Align")
    conddb.addFolderWithTag("LAR_OFL","/LAR/Align","LARAlign-RUN2-UPD4-03",force=True,forceMC=True)
    conddb.blockFolder("/Indet/Align")
    conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-RUN2-BLK-UPD4-16",force=True,forceMC=True) #InDetAlign_IOVDEP-03

#to set beam spot from MC
#conddb.blockFolder("/Indet/Beampos")
#conddb.addFolderWithTag("INDET_OFL","/Indet/Beampos","IndetBeampos-RunDep-MC15-BestKnowledge-002",force=True,forceMC=True)

#to run overlay chain with trigger                      
if (hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE") or (hasattr(recAlgs,'doTrigger') and recAlgs.doTrigger() and DetFlags.LVL1_on()):
    print "running with trigger  " 
    conddb.addOverride("/GLOBAL/Onl/BTagCalib/RUN12","BTagCalibRUN12Onl-08-40")
else:
    print "running with no trigger  "

if DetFlags.writeRDOPool.pixel_on():
    conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")

conddb.blockFolder("/PIXEL/PixdEdx")
conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/PixdEdx","PixdEdx-SIM-RUN124-000-00",force=True,forceMC=True)
mcIDoverlay=False
if mcIDoverlay:
    print "ACH777: Using MC overlay Lorentz DB settings"
    conddb.blockFolder("/PIXEL/DCS/HV")
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/HV","PixDCSHV-SIM-MC16-000-07",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/DCS/TEMPERATURE")
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/TEMPERATURE","PixDCSTemp-SIM-MC16-000-09",force=True,forceMC=True)
    conddb.blockFolder("/SCT/DCS/HV")
    conddb.addFolderWithTag("DCS_OFL","/SCT/DCS/HV","SctDcsHv-MC16",force=True,forceMC=True)
    conddb.blockFolder("/SCT/DCS/MODTEMP")
    conddb.addFolderWithTag("DCS_OFL","/SCT/DCS/MODTEMP","SctDcsModtemp-MC16",force=True,forceMC=True)
mcIDoverlay2=False
if mcIDoverlay2:
    print "ACH777: Using more MC overlay ID DB settings"
    conddb.blockFolder("/Indet/PixelDist")
    conddb.addFolderWithTag("INDET_OFL","/Indet/PixelDist","InDetPixelDist-nominal",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/PixelClustering/PixelClusNNCalib")
    conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib","PixelClusNNCalib-SIM-RUN12-000-06",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/LorentzAngleScale")
    conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/LorentzAngleScale","PixLorentzAngleScale-SIM-RUN124-000-00",force=True,forceMC=True)

conddb.blockFolder("/MUONALIGN/MDT/BARREL")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEA")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEC")
conddb.blockFolder("/MUONALIGN/TGC/SIDEA")
conddb.blockFolder("/MUONALIGN/TGC/SIDEC")

#use updated muon alignments
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-RUN2-BA_ROLLING_09-BLKP-UPD4-00",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-RUN2-ECA_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-RUN2-ECC_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True)
    
#the TGC alignments had overlap issues... caused G4 crash
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceMC=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceMC=True)

conddb.blockFolder("/SCT/DAQ/Config/Chip")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Chip","/SCT/DAQ/Config/Chip",force=True)
conddb.blockFolder("/SCT/DAQ/Config/Module")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Module","/SCT/DAQ/Config/Module",force=True)

dofolderoverrides = True
runN=365681
lumiBlock=500
if dofolderoverrides:
	print "ACH - overriding folder access patterns"
	conddb.dumpFolderTags('myconddb.txt',True)

overlaylongfolders = []
def setrunforfolder(name):
	if conddb.folderRequested(name):
            print "setting run to "+str(runN)+" and lumiblock "+str(lumiBlock)
            frnTag="<forceRunNumber>"+str(runN)+"</forceRunNumber>"
            flbTag="<forceLumiblockNumber>"+str(lumiBlock)+"</forceLumiblockNumber>"
            conddb.addMarkup(name,frnTag)
            conddb.addMarkup(name,flbTag)
            global overlaylongfolders
            overlaylongfolders += [name]

if dofolderoverrides:
	setrunforfolder("/CALO/CaloSwClusterCorrections/calhits")
	setrunforfolder("/CALO/CaloSwClusterCorrections/clcon")
	setrunforfolder("/CALO/CaloSwClusterCorrections/deadOTXback")
	setrunforfolder("/CALO/CaloSwClusterCorrections/deadOTXps")
	setrunforfolder("/CALO/CaloSwClusterCorrections/etamod")
	setrunforfolder("/CALO/CaloSwClusterCorrections/etaoff")
	setrunforfolder("/CALO/CaloSwClusterCorrections/gap")
	setrunforfolder("/CALO/CaloSwClusterCorrections/larupdate")
	setrunforfolder("/CALO/CaloSwClusterCorrections/lwc")
	setrunforfolder("/CALO/CaloSwClusterCorrections/phimod")
	setrunforfolder("/CALO/CaloSwClusterCorrections/phioff")
	setrunforfolder("/CALO/CaloSwClusterCorrections/rfac")
	setrunforfolder("/CALO/CaloSwClusterCorrections/trcorr")
	setrunforfolder("/CALO/H1Weights/H1WeightsCone4Topo")
	setrunforfolder("/CALO/Identifier/CaloTTOnAttrIdMapAtlas")
	setrunforfolder("/CALO/Identifier/CaloTTOnOffIdMapAtlas")
	setrunforfolder("/CALO/Identifier/CaloTTPpmRxIdMapAtlas")
	setrunforfolder("/CALO/JetCalib/EtaMassEnergyCorr")
	setrunforfolder("/CALO/JetCalib/JetPropertyInvertResponseTool")
	setrunforfolder("/CALO/JetCalib/MCNumInversionResponse")
	setrunforfolder("/CALO/JetCalib/OffsetCorrection")
	setrunforfolder("/CALO/JetCalib/TrackAngularCorrection")
	setrunforfolder("/CALO/JetCalib/TrackBasedResponseCorrection")
	setrunforfolder("/CALO/Ofl/Pedestal/CellPedestal")
	setrunforfolder("/CALO/Pedestal/CellPedestal")
	setrunforfolder("/CSC/PSLOPE")
	setrunforfolder("/CSC/RMS")
	setrunforfolder("/CSC/T0BASE")
	setrunforfolder("/CSC/T0PHASE")
	setrunforfolder("/GLOBAL/DETSTATUS/DEFECTLOGIC")
	setrunforfolder("/Indet/TrkErrorScaling")
	setrunforfolder("/LAR/CellCorrOfl/deadOTX")
	setrunforfolder("/LAR/ElecCalibOfl/LArPileupShape")
	setrunforfolder("/MDT/CABLING/MAP_SCHEMA")
	setrunforfolder("/MDT/CABLING/MEZZANINE_SCHEMA")
	setrunforfolder("/MUONALIGN/MDT/ASBUILTPARAMS")
	setrunforfolder("/MUONALIGN/TGC/SIDEA")
	setrunforfolder("/MUONALIGN/TGC/SIDEC")
	setrunforfolder("/PIXEL/LorentzAngleScale")
	setrunforfolder("/PIXEL/NoiseMapLong")
	setrunforfolder("/PIXEL/PixMapLong")
	setrunforfolder("/PIXEL/PixMapShort")
	setrunforfolder("/PIXEL/PixdEdx")
	setrunforfolder("/PIXEL/PixelClustering/PixelClusNNCalib")
	setrunforfolder("/PIXEL/PixelClustering/PixelCovCorr")
	setrunforfolder("/RPC/CABLING/MAP_SCHEMA")
	setrunforfolder("/RPC/CABLING/MAP_SCHEMA_CORR")
	setrunforfolder("/RPC/DQMF/ELEMENT_STATUS")
	setrunforfolder("/TGC/CABLING/MAP_SCHEMA")
	setrunforfolder("/TILE/OFL02/CALIB/CIS/NLN")
	setrunforfolder("/TILE/OFL02/CALIB/EMS")
	setrunforfolder("/TILE/OFL02/CALIB/LAS/FIBER")
	setrunforfolder("/TILE/OFL02/CALIB/LAS/NLN")
	setrunforfolder("/TILE/OFL02/NOISE/AUTOCR")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/CIS/LEAK100")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/CIS/PULSE100")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/LAS")
	setrunforfolder("/TILE/OFL02/PULSESHAPE/PHY")
	setrunforfolder("/TILE/OFL02/TIME/CHANNELOFFSET/CIS")
	setrunforfolder("/TRT/Calib/DX")
	setrunforfolder("/TRT/Calib/HTCalib")
	setrunforfolder("/TRT/Calib/PID")
	setrunforfolder("/TRT/Calib/PID_RToT")
	setrunforfolder("/TRT/Calib/PID_RToTver_New")
	setrunforfolder("/TRT/Calib/PID_vector")
	setrunforfolder("/TRT/Calib/PIDver_New")
	setrunforfolder("/TRT/Calib/ToT/ToTValue")
	setrunforfolder("/TRT/Calib/ToT/ToTVectors")
	setrunforfolder("/TRT/Calib/ToTCalib")
	setrunforfolder("/TRT/Calib/errors")
	setrunforfolder("/TRT/Calib/errors2d")
	setrunforfolder("/TRT/Calib/slopes")
	setrunforfolder("/CSC/FTHOLD")
	setrunforfolder("/CSC/NOISE")
	setrunforfolder("/CSC/PED")
	setrunforfolder("/GLOBAL/BTagCalib/Onl/RUN12")
	setrunforfolder("/Indet/PixelDist")
	setrunforfolder("/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples3bins17phases")
	setrunforfolder("/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases")
	setrunforfolder("/PIXEL/CablingMap")
	setrunforfolder("/PIXEL/NoiseMapShort")
	setrunforfolder("/PIXEL/PixReco")
	setrunforfolder("/Indet/AlignL3")
	setrunforfolder("/MUONALIGN/ERRS")
	setrunforfolder("/RPC/TRIGGER/CM_THR_ETA")
	setrunforfolder("/RPC/TRIGGER/CM_THR_PHI")
	setrunforfolder("/TRT/AlignL2")

print "set run "+str(runN)+" and lumiblock "+str(lumiBlock)+" for folders: ", overlaylongfolders



