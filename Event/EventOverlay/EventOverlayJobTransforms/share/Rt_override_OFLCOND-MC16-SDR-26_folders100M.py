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
print "RT OVERRIDE, for OFLCOND-MC16-SDR-26 "
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
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/HV","PixDCSHV-SIM-MC16a-000-07",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/DCS/TEMPERATURE")
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/TEMPERATURE","PixDCSTemp-SIM-MC16a-000-09",force=True,forceMC=True)
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
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceMC=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceMC=True)

conddb.blockFolder("/SCT/DAQ/Config/Chip")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Chip","/SCT/DAQ/Config/Chip",force=True)
conddb.blockFolder("/SCT/DAQ/Config/Module")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Module","/SCT/DAQ/Config/Module",force=True)

dofolderoverrides = True
if dofolderoverrides:
	print "ACH - overriding folder access patterns"
	conddb.dumpFolderTags('myconddb.txt',True)

overlaylongfolders = []
def adjustlongfolder(name):
	if conddb.folderRequested(name):
		print "setting "+name+" to 100000000s cache"
		conddb.addMarkup(name,"<cache>100000000</cache>")
		global overlaylongfolders
		overlaylongfolders += [name]

if dofolderoverrides:
	adjustlongfolder("/CALO/CaloSwClusterCorrections/calhits")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/clcon")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/deadOTXback")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/deadOTXps")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/etamod")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/etaoff")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/gap")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/larupdate")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/lwc")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/phimod")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/phioff")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/rfac")
	adjustlongfolder("/CALO/CaloSwClusterCorrections/trcorr")
	adjustlongfolder("/CALO/H1Weights/H1WeightsCone4Topo")
	adjustlongfolder("/CALO/Identifier/CaloTTOnAttrIdMapAtlas")
	adjustlongfolder("/CALO/Identifier/CaloTTOnOffIdMapAtlas")
	adjustlongfolder("/CALO/Identifier/CaloTTPpmRxIdMapAtlas")
	adjustlongfolder("/CALO/JetCalib/EtaMassEnergyCorr")
	adjustlongfolder("/CALO/JetCalib/JetPropertyInvertResponseTool")
	adjustlongfolder("/CALO/JetCalib/MCNumInversionResponse")
	adjustlongfolder("/CALO/JetCalib/OffsetCorrection")
	adjustlongfolder("/CALO/JetCalib/TrackAngularCorrection")
	adjustlongfolder("/CALO/JetCalib/TrackBasedResponseCorrection")
	adjustlongfolder("/CALO/Ofl/Pedestal/CellPedestal")
	adjustlongfolder("/CALO/Pedestal/CellPedestal")
	adjustlongfolder("/CSC/PSLOPE")
	adjustlongfolder("/CSC/RMS")
	adjustlongfolder("/CSC/T0BASE")
	adjustlongfolder("/CSC/T0PHASE")
	adjustlongfolder("/GLOBAL/DETSTATUS/DEFECTLOGIC")
	adjustlongfolder("/Indet/TrkErrorScaling")
	adjustlongfolder("/LAR/CellCorrOfl/deadOTX")
	adjustlongfolder("/LAR/ElecCalibOfl/LArPileupShape")
	adjustlongfolder("/MDT/CABLING/MAP_SCHEMA")
	adjustlongfolder("/MDT/CABLING/MEZZANINE_SCHEMA")
	adjustlongfolder("/MUONALIGN/MDT/ASBUILTPARAMS")
	adjustlongfolder("/MUONALIGN/TGC/SIDEA")
	adjustlongfolder("/MUONALIGN/TGC/SIDEC")
	adjustlongfolder("/PIXEL/LorentzAngleScale")
	adjustlongfolder("/PIXEL/NoiseMapLong")
	adjustlongfolder("/PIXEL/PixMapLong")
	adjustlongfolder("/PIXEL/PixMapShort")
	adjustlongfolder("/PIXEL/PixdEdx")
	adjustlongfolder("/PIXEL/PixelClustering/PixelClusNNCalib")
	adjustlongfolder("/PIXEL/PixelClustering/PixelCovCorr")
	adjustlongfolder("/RPC/CABLING/MAP_SCHEMA")
	adjustlongfolder("/RPC/CABLING/MAP_SCHEMA_CORR")
	adjustlongfolder("/RPC/DQMF/ELEMENT_STATUS")
	adjustlongfolder("/TGC/CABLING/MAP_SCHEMA")
	adjustlongfolder("/TILE/OFL02/CALIB/CIS/NLN")
	adjustlongfolder("/TILE/OFL02/CALIB/EMS")
	adjustlongfolder("/TILE/OFL02/CALIB/LAS/FIBER")
	adjustlongfolder("/TILE/OFL02/CALIB/LAS/NLN")
	adjustlongfolder("/TILE/OFL02/NOISE/AUTOCR")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/CIS/LEAK100")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/CIS/LEAK5P2")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/CIS/PULSE100")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/CIS/PULSE5P2")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/LAS")
	adjustlongfolder("/TILE/OFL02/PULSESHAPE/PHY")
	adjustlongfolder("/TILE/OFL02/TIME/CHANNELOFFSET/CIS")
	adjustlongfolder("/TRT/Calib/DX")
	adjustlongfolder("/TRT/Calib/HTCalib")
	adjustlongfolder("/TRT/Calib/PID")
	adjustlongfolder("/TRT/Calib/PID_RToT")
	adjustlongfolder("/TRT/Calib/PID_RToTver_New")
	adjustlongfolder("/TRT/Calib/PID_vector")
	adjustlongfolder("/TRT/Calib/PIDver_New")
	adjustlongfolder("/TRT/Calib/ToT/ToTValue")
	adjustlongfolder("/TRT/Calib/ToT/ToTVectors")
	adjustlongfolder("/TRT/Calib/ToTCalib")
	adjustlongfolder("/TRT/Calib/errors")
	adjustlongfolder("/TRT/Calib/errors2d")
	adjustlongfolder("/TRT/Calib/slopes")
	adjustlongfolder("/CSC/FTHOLD")
	adjustlongfolder("/CSC/NOISE")
	adjustlongfolder("/CSC/PED")
	adjustlongfolder("/GLOBAL/BTagCalib/Onl/RUN12")
	adjustlongfolder("/Indet/PixelDist")
	adjustlongfolder("/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples3bins17phases")
	adjustlongfolder("/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases")
	adjustlongfolder("/PIXEL/CablingMap")
	adjustlongfolder("/PIXEL/NoiseMapShort")
	adjustlongfolder("/PIXEL/PixReco")
	adjustlongfolder("/Indet/AlignL3")
	adjustlongfolder("/MUONALIGN/ERRS")
	adjustlongfolder("/RPC/TRIGGER/CM_THR_ETA")
	adjustlongfolder("/RPC/TRIGGER/CM_THR_PHI")
	adjustlongfolder("/TRT/AlignL2")

print "overlaylongfolders: ", overlaylongfolders

def adjustshortfolder(name):
	global overlaylongfolders 
	if name in overlaylongfolders:
		print "already made "+name+" long"
	else:
		if conddb.folderRequested(name):
        	        print "setting "+name+" to 10s cache"
                	conddb.addMarkup(name,"<cache>10</cache>")
		else:
			print "folder "+name+" was not requested?!"

if dofolderoverrides:
	with open("myconddb.txt") as f:
		for l in f:
			ls=l.split(" ")
			if ls[0].startswith("--folder"):
				ls1=ls[1].rstrip('\r\n')
				ls11=ls1.rstrip('\n')
				adjustshortfolder(ls11)


