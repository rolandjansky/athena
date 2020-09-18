
from IOVDbSvc.CondDB import conddb

########################################
#These top ones are MC folders (and other random things) and shouldn't have to change much

#use MC TRT RT functions
conddb.blockFolder("/TRT/Calib/RT")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/RT","TrtCalibRt-MC09_900GeV-REP-Field-00",force=True,forceMC=True)

#use MC MDT RT functions
conddb.blockFolder("/MDT/RT")
conddb.addFolderWithTag("MDT_OFL","/MDT/RT","MDTRT_Sim-00",force=True,forceMC=True)
conddb.blockFolder("/MDT/RTBLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/RTBLOB","MDTRT_Sim-00-BLOB",force=True,forceMC=True)

conddb.blockFolder("/PIXEL/HLT/DCS/HV")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/HV","PixDCSHV-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/HLT/DCS/TEMPERATURE")
conddb.addFolderWithTag("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE","PixDCSTemp-UPD1-00",force=True,forceData=True)
conddb.blockFolder("/PIXEL/ReadoutSpeed")
conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed",force=True)

conddb.blockFolder("/TRT/Cond/DigVers")
conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True,
                        className = 'AthenaAttributeList')

###################################################
printfunc ("Haas: RT OVERRIDE, old conditions (from Rt_override.py)")
#See https://twiki.cern.ch/twiki/pub/AtlasComputing/ConditionsTagComCondBlkpa00611/checkDB_COMCOND-BLKPA-006-11-17.2.10.6.txt

#use data TRT T0 constants
conddb.blockFolder("/TRT/Calib/T0")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/T0","TrtCalibT0-Physics-BLK-UPD4-FieldOn-00-00",force=True,forceData=True)

#use data MDT T0 constants
conddb.blockFolder("/MDT/T0")
conddb.addFolderWithTag("MDT_OFL","/MDT/T0","MDTT0-UPD4-09",force=True,forceData=True)
conddb.blockFolder("/MDT/T0BLOB")
conddb.addFolderWithTag("MDT_OFL","/MDT/T0BLOB","MDTT0-UPD4-09-BLOB",force=True,forceData=True)

conddb.blockFolder("/TRT/Align")
conddb.addFolderWithTag("TRT_OFL","/TRT/Align","TRTAlign_Collision_2009_04",force=True,forceData=True)
conddb.blockFolder("/LAR/Align")
conddb.addFolderWithTag("LAR_ONL","/LAR/Align","LARAlign-Repro2011-00",force=True,forceData=True)
conddb.blockFolder("/Indet/Align")
conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign_Collision_2009_08",force=True,forceData=True)

conddb.blockFolder("/MUONALIGN/MDT/BARREL")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEA")
conddb.blockFolder("/MUONALIGN/MDT/ENDCAP/SIDEC")
conddb.blockFolder("/MUONALIGN/TGC/SIDEA")
conddb.blockFolder("/MUONALIGN/TGC/SIDEC")

#use updated muon alignments
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/BARREL","MuonAlignMDTBarrelAlign-BA_ROLLING_2010_02-ES1-UPD1-00",force=True,forceData=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEA","MuonAlignMDTEndCapAAlign-COSMICS-200909-01-ES1-UPD1-00",force=True,forceData=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/MDT/ENDCAP/SIDEC","MuonAlignMDTEndCapCAlign-COSMICS-200909-01-ES1-UPD1-00",force=True,forceData=True,className='CondAttrListCollection')

conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEA","MuonAlignTGCEndCapAAlign-0001-DEFAULT",force=True,forceData=True,className='CondAttrListCollection')
conddb.addFolderWithTag("MUONALIGN_OFL","/MUONALIGN/TGC/SIDEC","MuonAlignTGCEndCapCAlign-0001-DEFAULT",force=True,forceData=True,className='CondAttrListCollection')

