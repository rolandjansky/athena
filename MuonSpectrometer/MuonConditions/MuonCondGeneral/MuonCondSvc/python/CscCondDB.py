# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Author Caleb Parnell-Lampen lampen@physics.arizona.edu
#Clarification: Aside from providing a centralized location for the folder names,
#most of what is below is just a convenience, and more specific folder setups can 
#be done by interfacing directly with conddb in the user's joboptions.

import CscCoolFolders

class CscCondDB:
    "Class to hold configuration information for cscCondDbSvc"
    
    def __init__(self):

        from AthenaCommon.Logging import logging

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

        self.athenaCommonFlags = athenaCommonFlags
      
        self.msg = logging.getLogger('CscCondDBSvcConfigurator')
        self.msg.debug("Configuring the CscCoolStrSvc for CSC conditions DB access") 

        from AthenaCommon.AppMgr  import ServiceMgr as svcMgr 
        from MuonCondSvc.MuonCondSvcConf import MuonCalib__CscCoolStrSvc
        
        self.CscCoolStrSvc = MuonCalib__CscCoolStrSvc()
        svcMgr += self.CscCoolStrSvc
        
        self.FolderSuffix = ""
        #svcMgr += MuonCalib__CscCoolStrSvc(name = "CscCoolStrSvc")

        #self.CscCoolStrSvc = svcMgr.MuonCalib__CscCoolStrSvc 

        self.newFolder = False
        
        self.forceFoldersToOnline = False
        #self.gainFolder         = "GAIN"
        #self.pslopeFolder       = "PSLOPE"
        #self.runSlopeFolder     = "RSLOPE"
        #self.pedFolder          = "PED"
        #self.tholdFolder        = "THOLD"
        #self.peaktFolder        = "PEAKT"
        #self.widthFolder        = "WIDTH"
        #self.sat1Folder         = "SAT1"
        #self.sat2Folder         = "SAT2"
        #self.noiseFolder        = "NOISE"
        #self.rmsFolder          = "RMS"
        #self.f001Folder         = "FTHOLD"
        #self.statusFolder       = "STAT"
        
        #used folders
        self.pslopeFolder       = CscCoolFolders.pslope
        self.pedFolder          = CscCoolFolders.ped
        self.noiseFolder        = CscCoolFolders.noise
        self.rmsFolder          = CscCoolFolders.rms
        self.f001Folder         = CscCoolFolders.f001
        self.statusFolder       = CscCoolFolders.status
        self.t0baseFolder       = CscCoolFolders.t0base
        self.t0phaseFolder       = CscCoolFolders.t0phase
        

        #Unused folders
        self.gainFolder         = "GAIN"
        self.runSlopeFolder     = "RSLOPE"
        self.tholdFolder        = "THOLD"
        self.peaktFolder        = "PEAKT"
        self.widthFolder        = "WIDTH"
        self.sat1Folder         = "SAT1"
        self.sat2Folder         = "SAT2"
   
        from IOVDbSvc.CondDB import conddb
        self.conddb = conddb

        self.useLocal = False

    def SetupForNewFolder(self):
        self.msg.info("Setting up to write new folder")
        self.CscCoolStrSvc.DoCaching = False
        self.CscCoolStrSvc.DoMerge = False
        self.newFolder = True;

    def SetupForWritingOnlineFromOffline(self):
      self.forceFoldersToOnline = True

    
    #uselocal file called mycool.db
    def useLocalFile(self,uselocal=True):
      self.useLocal = uselocal
    
    def setFolderSuffix(self,suffix):
      self.FolderSuffix = suffix

    #Add a parameter
    def addPar(self,name,key,dataType,cat,default,folder,database,override=""):
      if(self.CscCoolStrSvc.ParNames.count(name) != 0):
        self.msg.debug("tried to add a parameter twice. Ignoring seconde attempt." )
        return;
      
      self.msg.debug("Adding parameter " + name + " (" + database + ")" )
      self.CscCoolStrSvc.ParNames.append(name)
      self.CscCoolStrSvc.ParSGKeys.append(key)
      self.CscCoolStrSvc.ParDataTypes.append(dataType)
      self.CscCoolStrSvc.ParCats.append(cat)
      self.CscCoolStrSvc.ParDefaults.append(default)
      if(self.athenaCommonFlags.isOnline() or self.forceFoldersToOnline):
        self.CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + folder)
      else:
        self.CscCoolStrSvc.ParFolders.append('/CSC/' + folder)
      
      fullSuffix = " <key>" + key + "</key> " + self.FolderSuffix

      oflFolderString = '/CSC/' + folder + fullSuffix
      onlFolderString = '/CSC/ONL/' + folder + fullSuffix
      
      #If we're setup for new folders, don't load the folder from the database
      #We were just interested in setting CscCoolStrSvc appropriately above
      if(self.newFolder == True):
        return;

      if(self.useLocal):
        #assume when local, the folder will be in offline location. 
        #Maybe add more options in future
        self.conddb.addFolder("LOCAL", oflFolderString)
      elif(database == 'CSC'):
        #self.conddb.addFolderSplitMC(database, folderString, folderString)
        self.conddb.addFolderSplitOnline(database,onlFolderString,oflFolderString)
      elif(database == 'CSC_OFL'):
        self.conddb.addFolder(database, oflFolderString);
      else:
        self.msg.fatal("Failed to recognize database: " + database + " for parameter " + name)
        return;

      #override default tag
      if(override):
        if(self.useLocal):
          #assume when local, the folder will be in offline location. 
          #Maybe add more options in future
          overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif(database == 'CSC'):
          #self.conddb.addFolderSplitMC(database, folderString, folderString)
          if(self.athenaCommonFlags.isOnline()):
            overfolder = onlFolderString.split()[0] #Get folder without suffix
          else:
            overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif(database == 'CSC_OFL'):
          overfolder = oflFolderString.split()[0] #Get folder without suffix
        else:
          self.msg.fatal("Failed to recognize database: " + database + " for parameter " + name)
          return;

        self.msg.info("Overriding folder for " + name + "(" + overfolder + ") to " + override )
        self.conddb.addOverride(overfolder,override)



    
    #Register a few useful collections of folders. Provided as a convenience. 
    def addAllFolders(self):
        # set dbConnection string 
        # usually this should be used as default. If one of the folders isn't
        # found by IOVDbSvc, one can use one of the more specific functions
        #self.addGainFolder()
        self.addPSlopeFolder()
        #self.addTholdFolder()
        #self.addRSlopeFolder()
        self.addPedFolder()
        #self.addPeaktFolder()
        #self.addWidthFolder()
        #self.addSat1Folder()
        #self.addSat2Folder()
        #self.addPeakcFolder()
        #self.addSampleTimeRatioFolder()
        #self.addOccupancyFolder()
        self.addT0BaseFolder()
        self.addT0PhaseFolder()
        self.addF001Folder()
        self.addRmsFolder()
        self.addNoiseFolder()
        self.addStatusFolder()
    
    def addPedFolders(self):
        # initializes pedestals and noise only
        self.addNoiseFolder()
        self.addPedFolder()
  
    def addPulseShapeFolders(self):
        #intializes bipolar pulse shape info
        self.addWidthFolder()
        self.addPeaktFolder()

    #Single folder registration
    def addGainFolder(self):
        #initialize single folder
        self.msg.error("The Gain folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "gain"
        #key = "CSC_GAIN"
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "58000"
        #folder = self.gainFolder 
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override):

    def addPSlopeFolder(self,override=""):
      #initialize single folder
        name = "pslope"
        key = "CSC_PSLOPE"
        dataType = "float" 
        cat = "CHANNEL"
        default = "0.189"
        folder = self.pslopeFolder
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addRSlopeFolder(self,override=""):
        self.msg.error("The rslope folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "rslope"
        #key = "CSC_RSLOPE"
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "0.189"
        #folder = self.runSlopeFolder
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)
      
    def addPedFolder(self,override=""):
        name = "ped"
        key = "CSC_PED"
        dataType = "float"
        cat = "CHANNEL"
        default = "2048"
        folder = self.pedFolder 
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)
    
    def addT0BaseFolder(self,override=""):
        name = "t0base"
        key = "CSC_T0BASE"
        dataType = "float"
        cat = "CHANNEL"
        default = "0"
        folder = self.t0baseFolder 
        database = "CSC_OFL" #Offline only
        self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addT0PhaseFolder(self,override=""):
        name = "t0phase"
        key = "CSC_T0PHASE"
        #dataType = "float"
        dataType = "bool"
        cat = "ASM"
        default = "0"
        folder = self.t0phaseFolder 
        database = "CSC_OFL" #Offline only
        self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addTholdFolder(self,override=""):
        self.msg.error("The thold folder is retired. Please contact Caleb Lampen <lampen[AT SPAMNOT]physics{ dOT} arizona |DOT|edu> if it is needed.")
        #name = "thold"
        #key = "CSC_THOLD"
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "2058.85"
        #folder = self.tholdFolder 
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addPeaktFolder(self,override=""):
        self.msg.error("The peakt folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "peakt"
        #key = "CSC_PEAKT" 
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "46.824"
        #folder = self.peaktFolder
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addWidthFolder(self,override=""):
        self.msg.error("The peakt folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "width"
        #key = "CSC_WIDTH"
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "14.4"
        #folder = self.widthFolder 
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)
      
    def addSat1Folder(self,override=""):
        self.msg.error("The peakt folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "sat1"
        #key = "CSC_SAT1" 
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "22.26"
        #folder = self.sat1Folder 
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addSat2Folder(self,override=""):
        self.msg.error("The peakt folder is retired. Please contact Caleb Lampen <lampen@gmail.com if it is needed.")
        #name = "sat2"
        #key = "CSC_SAT2" 
        #dataType = "float"
        #cat = "CHANNEL"
        #default = "1789.0"
        #folder = self.sat2Folder 
        #database = "CSC_OFL"
        #self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addNoiseFolder(self,override=""):
        name = "noise"
        key = "CSC_NOISE"
        dataType = "float"
        cat = "CHANNEL"
        default = "3.5"
        folder = self.noiseFolder 
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)
    
    def addRmsFolder(self,override=""):
        name = "rms"
        key = "CSC_RMS"
        dataType = "float"
        cat = "CHANNEL"
        default = "2.56"
        folder = self.rmsFolder 
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)
    
    def addFtholdFolder(self,override=""):
        self.addF001Folder(override)

    def addF001Folder(self,override=""):
        name = "f001"
        key = "CSC_FTHOLD"
        dataType = "float"
        cat = "CHANNEL"
        default = "0"
        folder = self.f001Folder
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)

    def addStatusFolder(self,override=""):
        name = "status"
        key = "CSC_STAT"
        dataType = "uint32_t"
        cat = "CHANNEL"
        default = "0"
        folder = self.statusFolder 
        database = "CSC"
        self.addPar(name,key, dataType, cat, default, folder, database, override)

    def overrideBaseTag(self,tag):
      #Thie is simply a wrapper for the conddb's override tag     
        try:
          self.conddb.addOverride('/CSC',tag)
        except:
          self.msg.error("Error while trying to use tag")   


    def forcePreCache(self,preCache= True):
        #Set to true if you want to force CscCoolStrSvc to cache
        #on startup. Use if initialized in CBNT_initialize().
        self.CscCoolStrSvc.preCache = preCache

    def setOutputLevel(self, outputLevel):
        self.CscCoolStrSvc.OutputLevel= outputLevel

cscCondDB = CscCondDB()
