from IOVDbSvc.CondDB import conddb
########################################

#use MC MDT RT functions
conddb.blockFolder("/MDT/RT")
conddb.addFolderWithTag("MDT_OFL","/MDT/RT","MDTRT_Sim-04",force=True,forceMC=True)
conddb.blockFolder("/MDT/RTBLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/RTBLOB","MDTRT_Sim-04-BLOB",force=True,forceMC=True)

#from Guillaume
conddb.blockFolder("/LAR/LArCellPositionShift");
conddb.addFolderWithTag("LAR_OFL","/LAR/LArCellPositionShift","LArCellPositionShift-ideal",force=True,forceMC=True); 

conddb.blockFolder("/PIXEL/HLT/DCS/HV")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/HV","PixDCSHV-RUN2-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/HLT/DCS/TEMPERATURE")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE","PixDCSTemp-RUN2-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/ReadoutSpeed")
conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed",force=True)

conddb.blockFolder("/TRT/Cond/DigVers")
conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)

###################################################
print "RT OVERRIDE, for CONDBR2-BLKPA-2015-12"
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
conddb.blockFolder("/MDT/T0BLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/T0BLOB","MDTT0-RUN2-UPD4-15",force=True,forceData=True)

conddb.blockFolder("/TRT/Align")
conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign-RUN2-BLK-UPD4-10",force=True,forceData=True)
conddb.blockFolder("/LAR/Align")
conddb.addFolderWithTag("LAR_ONL","/LAR/Align","LARAlign-RUN2-UPD4-02",force=True,forceData=True)
conddb.blockFolder("/Indet/Align")
conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-RUN2-BLK-UPD4-13",force=True,forceData=True)

conddb.blockFolder("/MUONALIGN/MDT/BARREL")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEA")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEC")
conddb.blockFolder("/MUONALIGN/TGC/SIDEA")
conddb.blockFolder("/MUONALIGN/TGC/SIDEC")

#use updated muon alignments
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-RUN2-BA_ROLLING_09-BLKP-UPD4-00",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-RUN2-ECA_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-RUN2-ECC_ROLLING_2015_02_01-UPD4-02",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-BA_ROLLING_2010_03-BLKP-UPD4-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-ECA_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-ECC_ROLLING_2011_02_02-UPD4-01",force=True,forceData=True)
    
#the TGC alignments had overlap issues... caused G4 crash
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-TGCA_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-TGCC_ROLLING_2011_01-ES1-UPD1-00",force=True,forceData=True)
#G4 overlap crash is still here:
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-RUN2-TGCA_ROLLING_2011_01-ES1-UPD1-03",force=True,forceData=True)
#conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-RUN2-TGCC_ROLLING_2011_01-ES1-UPD1-03",force=True,forceData=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceMC=True)
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceMC=True)

