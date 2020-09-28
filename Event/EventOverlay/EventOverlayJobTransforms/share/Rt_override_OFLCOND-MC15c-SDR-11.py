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
conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed",force=True,className="AthenaAttributeList")

conddb.blockFolder("/TRT/Cond/DigVers")
conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True,
                        className = 'AthenaAttributeList')

###################################################
printfunc ("RT OVERRIDE, for CONDBR2-BLKPA-2015-12")
#See https://atlas-tagservices.cern.ch/tagservices/RunBrowser/runBrowserReport/rBR_CB_Report.php?CBAction=GlobalTagReport&cbgt=CONDBR2-BLKPA-2015-15#INDET

###These are not really needed anymore during overlay. Setup in muAlign_reco.py for reco.
#TRT T0 constants
#conddb.blockFolder("/TRT/Calib/T0")
#conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/T0","TrtCalibT0-RUN2-Physics-BLK-UPD4-00-01",force=True,forceData=True)
#conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/T0","TrtCalibT0-13TeV_ArgonScenario1_00-00",force=True,forceMC=True)
#TRT RT functions
#conddb.blockFolder("/TRT/Calib/RT")
#conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/RT","TrtCalibRt-R2-MC-run2-scenario1_01-00",force=True,forceMC=True)
#conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/RT","TrtCalibRt-13TeV_ArgonScenario1_00-00",force=True,forceMC=True)

#use data MDT T0 constants
#conddb.blockFolder("/MDT/T0")
#conddb.addFolderWithTag("MDT_OFL","/MDT/T0","MDTT0-UPD4-11",force=True,forceData=True)
#conddb.blockFolder("/MDT/T0BLOB")
#conddb.addFolderWithTag("MDT_OFL","/MDT/T0BLOB","MDTT0-RUN2-UPD4-15",force=True,forceData=True)

if not "EOJT_alignMC" in globals():
    printfunc ("EOJT_alignMC not found in globals(), so aligning ID to data conditions")
    #conddb.blockFolder("/TRT/Align")
    #conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign-RUN2-BLK-UPD4-10",force=True,forceData=True)
    #conddb.blockFolder("/LAR/Align")
    #conddb.addFolderWithTag("LAR_ONL","/LAR/Align","LARAlign-RUN2-UPD4-02",force=True,forceData=True)
    #conddb.blockFolder("/Indet/Align")
    #conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-RUN2-BLK-UPD4-13",force=True,forceData=True)
else:
    printfunc ("EOJT_alignMC found in globals(), so aligning ID to MC conditions")
    conddb.blockFolder("/TRT/Align")
    conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign_Nominal2",force=True,forceMC=True)
    conddb.blockFolder("/LAR/Align")
    conddb.addFolderWithTag("LAR_OFL","/LAR/Align","LARAlign-IOVDEP-00",force=True,forceMC=True)
    conddb.blockFolder("/Indet/Align")
    conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign_R2_Nominal",force=True,forceMC=True) #InDetAlign_IOVDEP-03

#to set beam spot from MC
#conddb.blockFolder("/Indet/Beampos")
#conddb.addFolderWithTag("INDET_OFL","/Indet/Beampos","IndetBeampos-RunDep-MC15-BestKnowledge-002",force=True,forceMC=True)

#to run overlay chain with trigger                      
if (hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE") or (hasattr(recAlgs,'doTrigger') and recAlgs.doTrigger() and DetFlags.LVL1_on()):
    printfunc ("running with trigger  " )
    conddb.blockFolder("/PIXEL/HLT/DCS/HV")
    conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/HV","PixDCSHV-RUN2-UPD1-00",force=True,forceData=True)
    conddb.blockFolder("/PIXEL/HLT/DCS/TEMPERATURE")
    conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE","PixDCSTemp-RUN2-UPD1-00",force=True,forceData=True)
    conddb.addOverride("/GLOBAL/Onl/TrigBTagCalib/RUN12","TrigBTagCalibRUN12Onl-08-10")
    conddb.addOverride("/GLOBAL/Onl/BTagCalib/RUN12","BTagCalibRUN12Onl-08-15")
else:
    printfunc ("running with no trigger  ")

if DetFlags.writeRDOPool.pixel_on():
    conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")

conddb.blockFolder("/PIXEL/PixdEdx")
conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/PixdEdx","PixdEdx-SIM-RUN124-000-00",force=True,forceMC=True,className="AthenaAttributeList")
mcIDoverlay=False
if mcIDoverlay:
    printfunc ("ACH777: Using MC overlay Lorentz DB settings")
    conddb.blockFolder("/PIXEL/DCS/HV")
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/HV","PixDCSHV-SIM-RUN1-000-00",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/DCS/TEMPERATURE")
    conddb.addFolderWithTag("DCS_OFL","/PIXEL/DCS/TEMPERATURE","PixDCSTemp-SIM-RUN1-000-00",force=True,forceMC=True)
    conddb.blockFolder("/SCT/DCS/HV")
    conddb.addFolderWithTag("DCS_OFL","/SCT/DCS/HV","SctDcsHv-01",force=True,forceMC=True)
    conddb.blockFolder("/SCT/DCS/MODTEMP")
    conddb.addFolderWithTag("DCS_OFL","/SCT/DCS/MODTEMP","SctDcsModtemp-Apr10-01",force=True,forceMC=True)
mcIDoverlay2=False
if mcIDoverlay2:
    printfunc ("ACH777: Using more MC overlay ID DB settings")
    conddb.blockFolder("/Indet/PixelDist")
    conddb.addFolderWithTag("INDET_OFL","/Indet/PixelDist","InDetPixelDist-nominal",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/PixelClustering/PixelClusNNCalib")
    conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib","PixelClusNNCalib-SIM-RUN12-000-03",force=True,forceMC=True)
    conddb.blockFolder("/PIXEL/LorentzAngleScale")
    conddb.addFolderWithTag("PIXEL_OFL","/PIXEL/LorentzAngleScale","PixLorentzAngleScale-SIM-RUN124-000-00",force=True,forceMC=True)

conddb.blockFolder("/MUONALIGN/MDT/BARREL")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEA")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEC")
conddb.blockFolder("/MUONALIGN/TGC/SIDEA")
conddb.blockFolder("/MUONALIGN/TGC/SIDEC")

#use updated muon alignments
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-RUN2-BA_ROLLING_09-BLKP-UPD4-00",force=True,forceData=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-RUN2-ECA_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-RUN2-ECC_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True,className='CondAttrListCollection')
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-BA_ROLLING_2010_03-BLKP-UPD4-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-ECA_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-ECC_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
    
#the TGC alignments had overlap issues... caused G4 crash
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-TGCA_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-TGCC_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
#G4 overlap crash is still here:
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-RUN2-TGCA_ROLLING_2011_01-ES1-UPD1-03",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-RUN2-TGCC_ROLLING_2011_01-ES1-UPD1-03",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceMC=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceMC=True,className='CondAttrListCollection')

conddb.blockFolder("/SCT/DAQ/Config/Chip")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Chip","/SCT/DAQ/Config/Chip",force=True)
conddb.blockFolder("/SCT/DAQ/Config/Module")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Config/Module","/SCT/DAQ/Config/Module",force=True)

