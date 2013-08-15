# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from config.dbInfo import *
#dbString = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;"
dbString = "oracle://ATONR_COOL;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;"
#----------------------------------------------------------------------------------------------------
import sys
import time
import os
import re
import subprocess
#----------------------------------------------------------------------------------------------------
# Load root, kill x 
import sys
sys.argv.append('-b')
print sys.argv
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.keeppolling = 0
#----------------------------------------------------------------------------------------------------
from PyCool import cool
#----------------------------------------------------------------------------------------------------
from model.basics.TRPFile import TRPFile
from model.basics.DataColumn import DataColumn
from model.basics.DataRange import DataRange
from model.basics.DataSelection import DataSelection

from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime

from model.parallelization.ProcessMaster import ProcessMaster
from model.parallelization.ProcessWorker import ProcessWorker
from model.parallelization.ProcessJob import ProcessJob

from model.physics.TriggerStream import TriggerStream 
#----------------------------------------------------------------------------------------------------
class ExtendedProcessJob(ProcessJob):
    """
        Extend ProcessJob so to add support for different run numbers
    """
    def __init__(self, runNumber = None):
        self.SetRunNumber(runNumber)
        ProcessJob.__init__(self)
    
    def GetRunNumber(self): return self.__runNumber
    def SetRunNumber(self, val): self.__runNumber = val
#----------------------------------------------------------------------------------------------------
class ChainNameJob(ExtendedProcessJob):
    """
        Job description for getting relation between channelID and chain name
        The corresponding worker will query COOL to generate a chain name -> channelID dictionary.
    """
    def __init__(self, runNumber = None):
        ExtendedProcessJob.__init__(self, runNumber)
#----------------------------------------------------------------------------------------------------
class ChainNameWorker(ProcessWorker):
    """
        Worker responsible for executing ChainNameJob(s)
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(ChainNameJob().GetType())
    
    def ExecuteJob(self, job):
        query = COOLQuery()
        query.SetIoV(COOLTime(job.GetRunNumber(),0x0000), COOLTime(job.GetRunNumber(),0xFFFF))
        
        dictionary = {}
        
        for obj, payload in query.GetFolder("/TRIGGER/HLT/Menu"):
            
            chainName = payload["ChainName"]
            
            #channelId = 2*payload["ChainCounter"] + int(chainName[0:2] == "EF")
            
            dictionary[chainName] = payload["ChainCounter"]
        
        return dictionary
#----------------------------------------------------------------------------------------------------
class DataDiscoverJob(ExtendedProcessJob):
    """
        Job description for reading ROOT structure
        The corresponding worker will extract a list of branches present in both the ROOT file AND
        in the list of valid branches.
    """
    def __init__(self, runNumber = None):
        ExtendedProcessJob.__init__(self, runNumber)
        self.SetValidChains(None)
    
    def GetValidChains(self): return self.__validChains
    def SetValidChains(self, val): self.__validChains = val
#----------------------------------------------------------------------------------------------------
class DataDiscoverWorker(ProcessWorker):
    """
        Worker responsible for executing DataDiscoverJob(s)
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(DataDiscoverJob().GetType())
    
    def ExecuteJob(self, job):
        TRP = TRPFile(job.GetRunNumber())
        #TRP.make_local()
        
        filePath = TRP.get_path()
        
        rootFile = ROOT.TFile(filePath)
        
        validChains = job.GetValidChains()
        
        
        if not rootFile:
            raise Exception("Invalid root file: %s, #%i" % (filePath, job.GetRunNumber()))
        
        
        structure = {}
        
        treeNames = []
        for fileKey in rootFile.GetListOfKeys():
            treeName = fileKey.GetName()
            treeNames.append(treeName)
            
        treeNames = set(treeNames)
        
        
        for treeName in treeNames:
            
            tree = rootFile.Get(treeName)

            if not tree.__class__ == ROOT.TTree: continue
            
            
            structure[treeName] = []
            for branch in tree.GetListOfBranches():
                branchName = branch.GetName() 
                branchChain = branchName[0:branchName.rfind("_")] 
                
                if branchChain in validChains and not "_grp_" in branchChain and not "_str_" in branchChain:
                    structure[treeName].append(branchName)
            
            
            
            if not len(structure[treeName]):
                structure.pop(treeName)
                
        return structure
        
#----------------------------------------------------------------------------------------------------
class DataReaderJob(ExtendedProcessJob):
    """
        Job description for reading data from TRP file.
        The corresponding worker will extract a list of branches from a specified tree and group them
        by value.
        
    """
    def __init__(self, runNumber = None):
        ExtendedProcessJob.__init__(self, runNumber)
        
        self.SetTreeName(None)
        self.SetBranches([])
        self.SetGroupBy("LumiBlock")
        self.SetStepSize(1)
        
        self.__ranges = {}
        
    def GetFlagging(self): return self.__flag
    def SetFlagging(self, val): self.__flag = val
    
    def GetTreeName(self): return self.__treeName
    def SetTreeName(self, val): self.__treeName = val

    def GetBranches(self): return self.__branches
    def SetBranches(self, val): self.__branches = val
    
    def GetGroupBy(self): return self.__groupBy
    def SetGroupBy(self, val): self.__groupBy = val
    
    def GetStepSize(self): return self.__stepSize
    def SetStepSize(self, val): self.__stepSize = val
    
#----------------------------------------------------------------------------------------------------
class DataReaderWorker(ProcessWorker):
    """
        Worker responsible for executing DataReaderJob(s)
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(DataReaderJob().GetType())
    
    def ExecuteJob(self, job):        
        filePath = TRPFile(job.GetRunNumber()).get_path()
        
        rootFile = ROOT.TFile(filePath)
        
        if not rootFile:
            raise Exception("Invalid root file: Missing %s, #%i" % (filePath, int(job.GetRunNumber())))
        
        if rootFile.IsZombie() or (not rootFile.IsOpen()):
            raise Exception("Invalid root file: corrupted %s, #%i" % (filePath, int(job.GetRunNumber())))        
        
        tree = rootFile.Get(job.GetTreeName())
        if not tree:
            raise Exception("Invalid root file: missing %s, %s, #%i" % (job.GetTreeName(), filePath, int(job.GetRunNumber())))
        
        
        
        #with globalProfiler.new("Prepare tree"): 
        keyName = job.GetGroupBy()
        
        branchNames = job.GetBranches()
        
        branchNames.append(keyName)
        branchNames = set(branchNames)
        
        branchList = [] 
        for branch in tree.GetListOfBranches():
            branchName = branch.GetName()
            if branchName in branchNames:
                branchList.append(branchName)
            else:
                tree.SetBranchStatus(branchName, 0)
        
        if not keyName in branchList:
            raise Exception("Key not found:")
            raise Exception("Key not found: %s, %s, %s, #%i" % (keyName, job.GetTreeName(), filePath, job.GetRunNumber()))
    
    
    
    
        # Read data from ROOT file into structure ordered by GroupBy-values
        #with globalProfiler.new("Read data from ROOT file"):
        branchList.remove(keyName)
        groupedData = {}
        for i in range(0,tree.GetEntries(), job.GetStepSize()):
            tree.GetEntry(i)
            
            keyVal = getattr(tree, keyName)
            if not groupedData.has_key(keyVal):
                groupedData[keyVal] = {}
                for branch in branchList:
                    groupedData[keyVal][branch] = DataColumn()

                        
            for branch in branchList:
                groupedData[keyVal][branch].append(getattr(tree, branch))
                
                
        # Flatten grouped data structure.
        #with globalProfiler.new("Ungroup data"):
        data = {}
        for branch in branchList + [keyName]:
            data[branch] = DataColumn() 
            
        for keyVal in groupedData.keys():
            data[keyName].append(keyVal)
            
            for branch in branchList:
                val = None
                
                if groupedData[keyVal].has_key(branch):
                    val = groupedData[keyVal][branch].Median(True)
            
                data[branch].append(val)
        
        return data
        
        
#----------------------------------------------------------------------------------------------------
class DataWriterJob(ExtendedProcessJob):
    """
        Job description for writing data to COOL....
    """
    def __init__(self, runNumber = None):
        ExtendedProcessJob.__init__(self, runNumber)
        self.SetData(None)
        self.SetChannelDictionary(None)
    
    def GetData(self): return self.__data
    def SetData(self, val): self.__data = val

    def GetFlagging(self): return self.__flag
    def SetFlagging(self, val): self.__flag = val

    def GetChannelDictionary(self): return self.__channelDict    
    def SetChannelDictionary(self, val): self.__channelDict = val
    
    def Test(self):
        if self.GetData() == None:
            raise Exception("No data given")

        if self.GetChannelDictionary() == None and self.GetFlagging() != "TotalRates":
            raise Exception("No dictionary given")        
#----------------------------------------------------------------------------------------------------
class DataWriterWorker(ProcessWorker):
    """
        Worker responsible for executing DataWriterJob(s)
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(DataWriterJob().GetType())


    def ExecuteJob(self, job):
        
        job.Test()
        
        runNumber = job.GetRunNumber()
        flagging = job.GetFlagging()
        data = job.GetData()
        
        channelDict = None
        if flagging == "TotalRates":
            query = COOLQuery()
            folder = query.GetFolderHandle(dbTRFolderName, dbString, False)
    
            channelDict = {}
            for channelId in folder.listChannels():
                name = folder.channelName(channelId)
                channelDict[name] = channelId             
        else:
            channelDict = job.GetChannelDictionary()
        
        
        
        
        
        if not data.has_key("LumiBlock"):
            raise Exception("LumiBlock information missing from data")
        


        #with globalProfiler.new("Prepare writing"):
        # Dictionary of type key->value:
        #     chainName -> (payloadType -> branchName)
        chainNames = {}
        
        for chainName in channelDict.keys():
            
            postfixDict = {"Input": "input", "Prescale": "prescale", "Raw": "raw", "Output": "output"}
            if "L1_" in chainName:
                postfixDict = {"Input": "TBP", "Prescale": "TAP", "Raw": "-", "Output": "TAV"}
            
            
            for payloadType in ["Input", "Prescale", "Raw", "Output"]:
                
                fullName = chainName + "_" + postfixDict[payloadType]
                
                if data.has_key(fullName):
                    
                    if not chainNames.has_key(chainName):
                        chainNames[chainName] = {}
                    
                    chainNames[chainName][payloadType] = fullName
                        

        
        
        
        query = COOLQuery()
        
        dbFolderName = None
        if flagging == "TotalRates":
            dbFolderName = dbTRFolderName
        elif flagging == "EFRates":
            dbFolderName = dbEFFolderName
        elif flagging == "L2Rates":
            dbFolderName = dbL2FolderName
        
        if dbFolderName == None: raise Exception("Unknown flagging...", flagging)
        
        
        folder = query.GetFolderHandle(dbFolderName, dbString, False)
        
        folder.setupStorageBuffer()
        
        
        for i in range(len(data["LumiBlock"])):
            LumiBlock = data["LumiBlock"][i]
            
            since = COOLTime(runNumber, LumiBlock)
            until = COOLTime(runNumber, LumiBlock+1)
            
            for chainName, payload in chainNames.items(): 
                #print ">>>", chainName, runNumber, LumiBlock
                
                channelId = cool.ChannelSelection(channelDict[chainName]).firstChannel()
                entr = cool.Record(folder.payloadSpecification())
                
                                
                for payloadType, branchName in payload.items():
                    if not data[branchName][i] == None:
                        entr[payloadType] = data[branchName][i]
                
                #if flagging == "TotalRates": print since.GetCOOLTime(), until.GetCOOLTime(), entr, channelId, chainName
                folder.storeObject(since.GetCOOLTime(), until.GetCOOLTime(), entr, channelId)
    
        #t = time.time()
        folder.flushStorageBuffer()
        #t = time.time() - t
        #print "DataWriterJob: Buffer flushed in %i seconds.." % (int(t))
        

#----------------------------------------------------------------------------------------------------
class TotalRateFinderJob(ExtendedProcessJob):
    """
        Job description for ....
    """
    def __init__(self, runNumber = None):
        ExtendedProcessJob.__init__(self, runNumber)
#----------------------------------------------------------------------------------------------------
class TotalRateFinderWorker(ProcessWorker):
    """
        Worker responsible for ....
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(TotalRateFinderJob().GetType())
    
    def ExecuteJob(self, job):
        TRP = TRPFile(job.GetRunNumber())
        
        
        filePath = TRP.get_path()
        
        rootFile = ROOT.TFile(filePath)
        
        
        if not rootFile:
            raise Exception("Invalid root file: %s, #%i" % (filePath, job.GetRunNumber()))
        
        
        structure = {}
        
        treeNames = []
        for fileKey in rootFile.GetListOfKeys():
            treeName = fileKey.GetName()
            treeNames.append(treeName)
            
        treeNames = set(treeNames)
        
        
        for treeName in treeNames:
            
            tree = rootFile.Get(treeName)

            if not tree.__class__ == ROOT.TTree: continue
            
            
            structure[treeName] = []
            for branch in tree.GetListOfBranches():
                branchName = branch.GetName()
                branchTotalRates = branchName[0:branchName.rfind("_")]
                
                if not branchTotalRates in structure[treeName]:
                    magicWords = ["_str_", "_total_", "_recording_", "L1_L1A_T"]
                    
                    isGood = False
                    for word in magicWords:
                        if word in branchName and ("L1" in branchName or "L2" in branchName or "EF" in branchName):
                            isGood = True
                            break
                        
                    if isGood:
                        structure[treeName].append(branchTotalRates)
            
            
            
            if not len(structure[treeName]):
                structure.pop(treeName)
                
        return structure
#----------------------------------------------------------------------------------------------------
class TotalRateChannelRegistrationJob(ExtendedProcessJob):
    """
        Job description for ....
    """
    def __init__(self, runNumber = None, totalRateDesc = {}):
        ExtendedProcessJob.__init__(self, runNumber)
        self.SetTotalRatesDescription(totalRateDesc)
        
    def GetTotalRatesDescription(self): return self.__TotalRatesDesc
    def SetTotalRatesDescription(self, val): self.__TotalRatesDesc = val
    
#----------------------------------------------------------------------------------------------------
class TotalRateChannelRegistrationWorker(ProcessWorker):
    """
        Worker responsible for ....
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(TotalRateChannelRegistrationJob().GetType())
        
    def ExecuteJob(self, job):
        totalRatesDesc = job.GetTotalRatesDescription()

        query = COOLQuery()
        folder = query.GetFolderHandle(dbTRFolderName, dbString, False)

        #channelMap = dict(folder.listChannelsWithNames())
        channelMap = {}
        for channelId in folder.listChannels():
            name = folder.channelName(channelId)
            channelMap[channelId] = name 
        
        
        id = len(channelMap.values())
        for list in totalRatesDesc.values():
            for totalRatesName in list:
                if not totalRatesName in channelMap.values():
                    channelId = id
                    print "New >\t",channelId, totalRatesName
                    
                    folder.createChannel(channelId, totalRatesName)
                    
                    id += 1
                else:
                    #print "Exists >\t",channelId, totalRatesName
                    pass
        
        return totalRatesDesc
        
#----------------------------------------------------------------------------------------------------
class SyncJob(ExtendedProcessJob):
    """
        Job description for ....
    """
    def __init__(self):
        ExtendedProcessJob.__init__(self)
        
        fromPath = "pc-tdq-mon-59:/data/coca/cache/tdaq-03-00-01/coca/TRP-Rates"
        toPath = "/tmp/ggalster/sync"
        interval = 600
        self.SetInterval(interval)
        self.SetSyncToPath(toPath)
        self.SetSyncFromPath(fromPath)
        

    def GetSyncFromPath(self): return self.__fromPath
    def SetSyncFromPath(self, val): self.__fromPath = val
    
    def GetSyncToPath(self): return self.__toPath
    def SetSyncToPath(self, val): self.__toPath = val
    
    def GetInterval(self): return self.__interval
    def SetInterval(self, val): self.__interval = val
    
#----------------------------------------------------------------------------------------------------
class SyncWorker(ProcessWorker):
    """
        Worker responsible for ....
    """
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(SyncJob().GetType())
        
    def ExecuteJob(self,job):
        def postfix_slash(string):
            if string[-1] != "/": string = string + "/"
            return string
        

        #get files.
        fromPath = postfix_slash(job.GetSyncFromPath())
                
        toPath = postfix_slash(job.GetSyncToPath())
        
        flag = ""
        # for safety, only delete diff on destination if destination is in /tmp/ and involves user
        #if user in toPath and "/tmp/" in toPath: flag = " --delete"  
        
        syncCmd =  "rsync -avz%s %s %s" % (flag, fromPath, toPath)
        print "[+] Executing",syncCmd 
        sys.stdout.flush()
        subprocess.call(syncCmd, shell=True)            
        
        time.sleep(job.GetInterval())
        
        job.SetRunNumber(None)
        self.GetJobQueue().put(job)
            
#----------------------------------------------------------------------------------------------------
class MonitorJob(ExtendedProcessJob):
    """
        Job description for ....
    """
    def __init__(self, folder = None, force = False, interval = 100, servedRuns = set([])):
        ExtendedProcessJob.__init__(self)
        
        self.SetFolderName(folder)
        self.SetForceLoad(force)
        self.SetInterval(interval)
        self.SetServedRuns(servedRuns)
    
    def GetFolderName(self): return self.__folderName
    def SetFolderName(self, val): self.__folderName = val

    def GetInterval(self): return self.__interval
    def SetInterval(self, val): self.__interval = val
    
    def GetForceLoad(self): return self.__forceLoad
    def SetForceLoad(self, val): self.__forceLoad = val
    
    def GetServedRuns(self): return self.__servedRuns
    def SetServedRuns(self, val): self.__servedRuns = val
#----------------------------------------------------------------------------------------------------
class MonitorWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(MonitorJob().GetType())
        
    def ExecuteJob(self, job):
        def postfix_slash(string):
            if string[-1] != "/": string = string + "/"
            return string

        folderNotFound = Exception("Folder does not exist: %s", job.GetFolderName())
        
        if job.GetFolderName() == None or (not os.path.exists(job.GetFolderName())):
            if job.GetFolderName() == None: job.SetFolderName("None")
            raise folderNotFound

        servedRuns = job.GetServedRuns()
    

        #what files are new to me?
        try:
            syncFiles = os.listdir(job.GetFolderName())
        except:
            raise folderNotFound
        
        syncRuns = set([])
        for fileName in syncFiles:
            match = re.match("TriggerRates_ATLAS_([0-9]+).root", fileName)
            if match:
                syncRuns.add(match.group(1))
        
        
        #do I have memory of files who are no longer there?
        for runNumber in servedRuns - syncRuns:
            servedRuns.discard(runNumber)
        
        
        #so what run numbers are new to me?
        syncRuns = syncRuns - servedRuns
        
        
        #so theses files are genuinely new?
        from model.physics.filters.FilterRunDuration import FilterRunDuration 
        for runNumber in syncRuns:
            sys.stdout.flush()

            #Veto from above?
            if job.GetForceLoad():
                continue
        
        
            #Check if already loaded into db
            triggerStream = TriggerStream("EF_recording_physics", DataSelection(COOLTime(runNumber, 1),COOLTime(runNumber, 20)))
            triggerStream.Load()
            
            if triggerStream.GetIsDummy() == False:
                servedRuns.add(runNumber)                
                print "[?] Skipping run",runNumber,": Already loaded."
                continue
            


            #Check for lhc stable beam
            stableBeam = False
            q = COOLQuery()
            



            """
            runRange = FilterRunDuration().FilterValues(DataSelection(*COOLTime().Run(runNumber))).GetSuperSetAsRange()

            #IoVct = DataSelection(runRange.GetFirst()).Union(DataSelection(runRange.GetLast())) 
            IoVct = DataSelection() 
            IoVct.SetListOfRanges( [ DataRange(runRange.GetFirst()), DataRange(runRange.GetLast())] )
            
            q.SetIoVSelection(IoVct)
            """


            runSelection = FilterRunDuration().FilterValues( DataSelection(*COOLTime().Run(runNumber)) )

            q.SetIoVSelection(runSelection)


            begin = None
            end = None

            for obj, payload in q.GetFolder("/TRIGGER/LUMI/LBLB"):
                end = payload["EndTime"]
                if begin == None: begin = payload["StartTime"]
            
            print "[?] DEBUG:", runNumber, begin, end, end-begin
            q.SetIoV(begin, end)
            for obj, payload in q.GetFolder("/LHC/DCS/FILLSTATE"):
                stableBeam = payload["StableBeams"]
                if payload["StableBeams"]: break
                    
            if not stableBeam:
                print "[?] Skipping run",runNumber,": No stable beam."
                servedRuns.add(runNumber)
                continue
            


            
        
        syncRuns = syncRuns - servedRuns
        
        #serve new jobs
        for runNumber in syncRuns:
            
            # make file available 
            trp = TRPFile(runNumber)
            cpCmd = "cp -f %s%s %s" % (postfix_slash(job.GetFolderName()), trp.get_short_name(), trp.get_local_path())
            print "[+] Executing",cpCmd 
            subprocess.call(cpCmd, shell=True)
            
            job.SetRunNumber(int(runNumber))
            self.GetResultQueue().put(job)
        
            servedRuns.add(runNumber)
            sys.stdout.flush()

        if len(syncRuns):
            print "[+] Booked the following jobs from %s:" % job.GetFolderName(),
            sys.stdout.flush()
            for runNumber in syncRuns:
                print runNumber,
                sys.stdout.flush()
            print                            
            sys.stdout.flush()
               
        time.sleep(job.GetInterval())

        job.SetRunNumber(None)
        self.GetJobQueue().put(job)

#----------------------------------------------------------------------------------------------------
def trp_to_cool(*runList):
    print "Start", time.time()
    runList = list(runList)
    
    daemon = False
    if not len(runList):
        daemon = True
    
    if daemon:
        print "[+] Run mode daemon"
    else:
        print "[+] Run mode normal"
        print "[+] Processing:",runList 
    
    """
        Main loop for application 
    """
    
    # Create a process master 
    processMaster = ProcessMaster()
  
    
    # Add workers
    ran = range(min(5, len(runList)))
    #if daemon: ran = range(5)
    if daemon: ran = range(2)
    
    for i in ran:
        processMaster.AddWorker(ChainNameWorker())
        processMaster.AddWorker(DataDiscoverWorker())
    
        processMaster.AddWorker(DataReaderWorker())
        processMaster.AddWorker(DataReaderWorker())
        
        processMaster.AddWorker(TotalRateFinderWorker())
   
    processMaster.AddWorker(TotalRateChannelRegistrationWorker())
    processMaster.AddWorker(DataWriterWorker())

    # Result buffer:
    channelDict = {}
    jobsRunning = {}
    

    # sub function to book jobs
    def book_jobs_for_run(runNumber):
        if jobsRunning.has_key(runNumber): return
        
        print "Booking initial jobs for run", runNumber, ", t:", time.time() 
        job = ChainNameJob(runNumber)
        processMaster.SubmitJob(job)
    
        job = TotalRateFinderJob(runNumber)
        processMaster.SubmitJob(job)        
        
        jobsRunning[runNumber] = 2

    
    
    
    # Add initial jobs
    if not daemon:
        for runNumber in runList:    
            book_jobs_for_run(runNumber)
    else:
        user = os.environ["USER"]
        
        
        
        
        
        syncFromPath = "pc-tdq-mon-59:/data/coca/cache/tdaq-03-00-01/coca/TRP-Rates/"
        syncToPath = "/tmp/%s/sync/" % user        
        interval = 1800

        processMaster.AddWorker(SyncWorker())

        job = SyncJob()
        job.SetSyncFromPath(syncFromPath)
        job.SetSyncToPath(syncToPath)
        job.SetInterval(interval)
        
        processMaster.SubmitJob(job)

        processMaster.AddWorker(MonitorWorker())
               
        job = MonitorJob(syncToPath)
        job.SetForceLoad(False)
        processMaster.SubmitJob(job)






        syncFromPath = "pc-tdq-mon-62:/tmp/ggalster/"
        syncToPath = "/tmp/%s/manual/" % user
        interval = 1800
        
        processMaster.AddWorker(SyncWorker())

        job = SyncJob()
        job.SetSyncFromPath(syncFromPath)
        job.SetSyncToPath(syncToPath)
        job.SetInterval(interval)
        
        processMaster.SubmitJob(job)

        processMaster.AddWorker(MonitorWorker())
               
        job = MonitorJob(syncToPath)
        job.SetForceLoad(False)
        processMaster.SubmitJob(job)






        processMaster.AddWorker(MonitorWorker())
        
        forceLoadPath = "/tmp/%s/force/" % user
        job = MonitorJob(forceLoadPath)
        job.SetForceLoad(True)
        processMaster.SubmitJob(job)

                
    termFileName = "/tmp/%s/RatesToCOOL.term" % os.environ["USER"]    
        
            
    # Server forever
    try:
        
        lastRound = False 
        
        while True: 
            
            """
                Result handling for ChainNameJobs
            """
            chainNameResult = processMaster.GetResult(ChainNameJob().GetType())
            if len(chainNameResult):

            
                for job in chainNameResult:
                                        
                    print "ChainNameJobs finished, run", job.GetRunNumber(), ", t:", time.time() 
                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1
                    
                    channelDict[runNumber] = result
                    
                    discoverJob = DataDiscoverJob(runNumber)
                    discoverJob.SetValidChains(result.keys())
                    
                    processMaster.SubmitJob(discoverJob)
                    jobsRunning[runNumber] += 1
                    
            
            
            
            
            
            
            """
                Result handling for DataDiscoverJob
            """
            dataDiscoverResult = processMaster.GetResult(DataDiscoverJob().GetType())
            if len(dataDiscoverResult):
                for job in dataDiscoverResult:
                    
                    print "DataDiscoverJob finished, run", job.GetRunNumber(), ", t:", time.time()
                    
                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1
                    
                    for tree, branches in result.items():                        
                        dataJob = DataReaderJob(runNumber)
                        
                        dataJob.SetFlagging(tree[0:2] + "Rates")
                        
                        dataJob.SetGroupBy("LumiBlock")

                        dataJob.SetTreeName(tree)
                        dataJob.SetBranches(branches)
                        
                        processMaster.SubmitJob(dataJob)        
                        jobsRunning[runNumber] += 1
                        
            
            
            
            """
                Result handling for DataReaderJobs
            """
            dataReaderResult = processMaster.GetResult(DataReaderJob().GetType())
            if len(dataReaderResult):
                for job in dataReaderResult:
                    
                    print "DataReaderJob finished, run", job.GetRunNumber(), ", t:", time.time()

                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1
                    
                    writeJob = DataWriterJob(runNumber)
                    writeJob.SetData(result)
                    writeJob.SetFlagging(job.GetFlagging())
                    
                    if job.GetFlagging() != "TotalRates":
                        writeJob.SetChannelDictionary(channelDict[runNumber])
                                        
                    processMaster.SubmitJob(writeJob)
                    jobsRunning[runNumber] += 1
            
            
            
            
            
            
    

            
            """
                Result handling for TotalRatesFinderJobs
            """
            TotalRatesFinderResult = processMaster.GetResult(TotalRateFinderJob().GetType())
            if len(TotalRatesFinderResult):
                for job in TotalRatesFinderResult:
                    
                    print "TotalRatesFinderJob finished, run", job.GetRunNumber(), ", t:", time.time()

                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1                    
                    
                    newJob = TotalRateChannelRegistrationJob(runNumber, result)
                    
                    processMaster.SubmitJob(newJob)
                    jobsRunning[runNumber] += 1




            """
                Result handling for TotalRateChannelRegistrationJobs
            """
            TotalRatesChannelRegistrationResult = processMaster.GetResult(TotalRateChannelRegistrationJob().GetType())
            if len(TotalRatesChannelRegistrationResult):
                for job in TotalRatesChannelRegistrationResult:
                    
                    print "TotalRateChannelRegistrationJob finished, run", job.GetRunNumber(), ", t:", time.time()
                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1                    
    
                    for tree, branches in result.items():
                        
                        branchList = []
                        for branch in branches:
                            if "L1" in branch:
                                branchList.append(branch + "_TBP")
                                branchList.append(branch + "_TAV")                                
                            else: 
                                branchList.append(branch + "_input")
                                branchList.append(branch + "_output")
                            jobsRunning
                        dataJob = DataReaderJob(runNumber)
                        
                        dataJob.SetGroupBy("LumiBlock")
                        
                        dataJob.SetFlagging("TotalRates")
                        
                        #dataJob.SetStepSize(20)
                        
                        dataJob.SetTreeName(tree)
                        dataJob.SetBranches(branchList)
                        
                        processMaster.SubmitJob(dataJob)        
                        jobsRunning[runNumber] += 1











    
    
    
    
    
            """
                Result handling for DataWriterJobs
            """
            dataWriterResult = processMaster.GetResult(DataWriterJob().GetType())
            if len(dataWriterResult):
                for job in dataWriterResult:
                    
                    print "DataWriterJob finished, run", job.GetRunNumber(), ", t:", time.time()
                    runNumber = job.GetRunNumber()
                    result = job.GetResult()

                    jobsRunning[runNumber] += -1                    

                    print ">>> DataWriterJob finished. Run",runNumber,"Jobs remaining:", jobsRunning[runNumber]
                    
                    if jobsRunning[runNumber] == 0: 
                        jobsRunning.pop(runNumber)
                        
                        trp = TRPFile(runNumber)
                        if trp.is_local():
                            os.remove(trp.get_path())

                         


            """
                Result handling for MonitorJobs
            """
            syncResult = processMaster.GetResult(MonitorJob().GetType())
            if len(syncResult):
                for job in syncResult:
                    runNumber = job.GetRunNumber()

                    if runNumber == None: continue


                    book_jobs_for_run(runNumber)




            """
                Time to terminate....?
            """
            if os.path.exists(termFileName):
                os.remove(termFileName)
                raise Exception("Found file " + termFileName + "- will stop serving after all workers finish.")
                                    
            
            """
                Error handling....
            """            
            failedJobsDict = processMaster.GetError()
            if len(failedJobsDict):
                failedJobs = []
                
                for jobList in failedJobsDict.values():
                    failedJobs += jobList 
                
                for job in failedJobs:
                    
                    runNumber = job.GetRunNumber()
                    result = job.GetResult()
                    

                    
                    if jobsRunning.has_key(runNumber):
                        jobsRunning[runNumber] -= 1

                    isCritical = True
                    isUnderstood = False
                    
                    type = job.GetType()
                    
                    # Put error logic here

                    if type == DataReaderJob().GetType():
                        if "Invalid root file" in str(result):
                            isCritical = False
                            isUnderstood = True
                            print "[-] TRP file for run",runNumber,"is either invalid or corrupted:", str(result)
                    
                    if type == DataWriterJob().GetType():
                        if "Unknown flagging" in str(result):
                            isCritical = False
                    
                    if type == MonitorJob().GetType():
                        if "Folder does not exist" in str(result):
                            isCritical = False
                            isUnderstood = True
                            print "[-] I am not able to monitor a non existing folder. Sorry. You told me to monitor:",job.GetFolderName()
                
                    if not isUnderstood:
                        print "*"
                        print "* A worker failed for an unknown reason. Here is some debug:"
                        print "* Job type:", type
                        print "* Run number:", runNumber
                        print "* Error msg:", str(result)
                        print "* Is critical:", isCritical
                        print "* Time:", time.time()
                        print "*"
                        
                        sys.stdout.flush()
                        
                    if isCritical:
                        raise result
                            
            """
            print
            print "All:\t"processMaster.GetNJobs(), processMaster.GetNErrors(), processMaster.GetWorkerStates().values()
            print "DataReaderJob: ", processMaster.GetNJobs(DataReaderJob().GetType()), processMaster.GetNErrors(DataReaderJob().GetType()), processMaster.GetWorkerStates(DataReaderJob().GetType()).values()
            print "DataWriterJob: ", processMaster.GetNJobs(DataWriterJob().GetType()), processMaster.GetNErrors(DataWriterJob().GetType()), processMaster.GetWorkerStates(DataWriterJob().GetType()).values()
            print
            """
            
            sys.stdout.flush()
            
            """
                Control loop for serving processes... How may I serve you....
            """
            if not processMaster.IsActive():
                if lastRound:
                    break
                else:
                    lastRound = True
            else:
                lastRound = False
                    
            time.sleep(1)
    
    except Exception as e:
        print "Some error occurred, but I have to wait for the rest of the workers before raising the exception.."
        print "While you are waiting, the error message is:"
        print str(e)
        print " and sorry for the inconvenience."
        
        processMaster.KillAll()
        processMaster.Block(None, False)
        time.sleep(5)
        
        raise e
    
    print "Stop", time.time()
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    print "#", "gorm galster, gorm@galster.dk - sep. 2011"
    #runNumbers = [189288, 189280, 189242, 189207, 189205, 189184]
    #runNumbers = [187552]    
    trp_to_cool()    
#----------------------------------------------------------------------------------------------------
