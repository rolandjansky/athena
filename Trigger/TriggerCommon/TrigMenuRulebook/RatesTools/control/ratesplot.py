# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ATLAS_PREFERRED_Inst_CalibLumi_Val
#----------------------------------------------------------------------------------------------------
from model.basics.Profiler import Profiler
from model.basics.TRPFile import TRPFile
profiler = Profiler("/pcatr-srv1/home/ggalster/ratesplot.txt")
#----------------------------------------------------------------------------------------------------
import os
def mkdir(path):
    if not os.path.exists(path):
        npath = path[0:path.rfind("/")]
        if not os.path.exists(npath):
            mkdir(npath)
        os.mkdir(path)  
#----------------------------------------------------------------------------------------------------
import time
#----------------------------------------------------------------------------------------------------
from model.basics.DataColumn import DataColumn
#----------------------------------------------------------------------------------------------------
from model.parallelization.ProcessMaster import ProcessMaster
from model.parallelization.ProcessJob import ProcessJob 
from model.parallelization.ProcessWorker import ProcessWorker
#----------------------------------------------------------------------------------------------------
from model.parser.ROOT.ROOTWorker import ROOTWorker
from model.parser.ROOT.ROOTJob import ROOTJob
from model.parser.COOLquery import COOLquery
from model.COOL.COOLTime import COOLTime as COOLtime
#----------------------------------------------------------------------------------------------------        
def joinDataColumns(inputData, key = None):
    outputData = {}
    
    
    
    if key == None:
        for column in inputData.values():
            for name in column.keys():
                if not outputData.has_key(name):
                    outputData[name] = DataColumn()
                outputData[name] += column[name]
            
    else:
        varNames = []
        orderedData = {}
        for column in inputData.values():
            if not column.has_key(key):
                continue
            
            columnNames = []
            for name in column.keys():  
                if key == name:
                    continue
                columnNames.append(name)
            varNames += columnNames
            
            for i in range(len(column[key])):
                keyVal = column[key][i]
                if not orderedData.has_key(keyVal):
                    orderedData[keyVal] = {}
                for name in columnNames:

                    if not orderedData[keyVal].has_key(name):
                        orderedData[keyVal][name] = DataColumn()
                                           
                    orderedData[keyVal][name].append(column[name][i])
        
        varNames = set(varNames)
        
        outputData[key] = DataColumn()
        for varName in varNames:
            outputData[varName] = DataColumn()
            
        for keyVal in orderedData.keys():
            
            #if orderedData[keyVal].has_key("ATLAS_PREFERRED_Inst_CalibLumi_Val"):
            #    print "Exists?",keyVal,len(orderedData[keyVal]["ATLAS_PREFERRED_Inst_CalibLumi_Val"])
            #else:
            #    print "Exists?",keyVal,"NONE"
                
            #if 400 < keyVal and keyVal < 600: print keyVal
            outputData[key].append(keyVal)
            for varName in varNames:
                val = -1
                if orderedData[keyVal].has_key(varName):
                        
                    #if 400 < keyVal and keyVal < 600: print "\tjo", varName
                    if len(orderedData[keyVal][varName]):
                        val = orderedData[keyVal][varName].Mean()
                        #if 400 < keyVal and keyVal < 600: print "\t\tjo", val
                
                if val == -1:
                    pass
                    #print "Assuming",varName,"==",val,"at",key,"==",keyVal,
                    
                outputData[varName].append(val)

        
    return outputData

#----------------------------------------------------------------------------------------------------        
class TRPDataJob(ROOTJob):
    def __init__(self, runNumber = None):
        ROOTJob.__init__(self)
        self.SetRunNumber(runNumber)
    
    __runNumber = int()
    def GetRunNumber(self): return self.__runNumber
    def SetRunNumber(self, val):
        self.__runNumber = val
        if val != None:
            #self.SetFileName("/pcatr-srv1/home/ggalster/TriggerRates_ATLAS_184169.root")
            #
            self.SetFileName(TRPFile(self.__runNumber).get_path())

    

#----------------------------------------------------------------------------------------------------
class TRPDataWorker(ROOTWorker):
    def __init__(self):
        ROOTWorker.__init__(self)
        self.SetType(TRPDataJob().GetType())
                 
#----------------------------------------------------------------------------------------------------
class SimpleDataJob(ProcessJob):
    def __init__(self, runNumber = None):
        ProcessJob.__init__(self)
        
        self.SetRunNumber(runNumber)
        self.SetData({})
        self.SetKey("")
    
    __runNumber = int()
    def GetRunNumber(self): return self.__runNumber
    def SetRunNumber(self, val): self.__runNumber = val
    
    __data = {}
    def GetData(self): return self.__data
    def SetData(self, val): self.__data = val
    
    __key = str()
    def GetKey(self): return self.__key
    def SetKey(self, val): self.__key = val
#----------------------------------------------------------------------------------------------------
class DataLimitJob(SimpleDataJob):
    def __init__(self, runNumber = None):
        SimpleDataJob.__init__(self, runNumber)

    def GetDeadtimeThreshold(self): return self.__threshold        
    def SetDeadtimeThreshold(self, val): self.__threshold = val
    
    def Test(self):
        pass        
#----------------------------------------------------------------------------------------------------
class DataLimitWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(DataLimitJob().GetType())
        
    def ExecuteJob(self, job):
        job.Test()
        
        query = COOLquery()
        query.SetIOVStart(COOLtime(job.GetRunNumber(), 0x0000))
        query.SetIOVStop(COOLtime(job.GetRunNumber(), 0xFFFF))
        
        
        physicsReadyList = []
        with profiler.new("Ready for physics..."):
            begin = -1
            for obj in query.GetFolder("/TDAQ/RunCtrl/DataTakingMode"):
                payload = obj.payload()
                
                if payload["ReadyForPhysics"] == 1 and begin == -1:
                    begin = COOLtime(obj.since()).GetLumiBlock()
                
                if payload["ReadyForPhysics"] == 0 and not begin == -1:
                    end = COOLtime(obj.since()).GetLumiBlock() - 1
                    
                    tup = (begin, end)
                    
                    physicsReadyList.append(tup)
                    
                    begin = -1
                
        
        # Get dead lumi blocks 
        deadtimeList = []
        with profiler.new("Get dead lumi blocks"):
            threshold = job.GetDeadtimeThreshold()
            """
            deadtimeJob = TRPDataJob(job.GetRunNumber())
            deadtimeJob.SetTree("L1_Rate")
            deadtimeJob.SetBranches(["L1_EM16_DT", "LumiBlock"])
            
            deadtimeWorker = TRPDataWorker()
            deadResult = deadtimeWorker.ExecuteJob(deadtimeJob)
            
            for i in range(len(deadResult["LumiBlock"])):            
                if threshold < deadResult["L1_EM16_DT"][i]:
                    deadtimeList.append(deadResult["LumiBlock"][i])
            """
        
            deadtimeId = None
            for obj in query.GetFolder("/TRIGGER/LVL1/Menu"):
                pl = obj.payload()
                if pl["ItemName"] == "L1_EM16":
                    deadtimeId = obj.channelId()
            
            if deadtimeId == None:
                raise Exception("channelId() not found in COOL for L1_EM16")
            
            query.SetChannelSelection(deadtimeId)
            
            for obj in query.GetFolder("/TRIGGER/LUMI/LVL1COUNTERS"):
                pl = obj.payload()
                
                try:
                    deadtime = 1.0-(float(pl["L1Accept"]) / float(pl["BeforePrescale"])) 
                except:
                    deadtime = 0.0
                 
                if deadtime > threshold:
                    deadtimeList.append(deadtime)
    
                    #print COOLtime(obj.since()).GetLumiBlock(), "\t", deadTime 
                    
            deadtimeList = set(deadtimeList)
        
        result = {}
        result["PhysicsReady"] = physicsReadyList
        result["Deadtime"] = deadtimeList
        
        return result
        
#----------------------------------------------------------------------------------------------------
class LuminosityJob(SimpleDataJob):
    def __init__(self, runNumber = None):
        SimpleDataJob.__init__(self, runNumber)
#----------------------------------------------------------------------------------------------------
class LuminosityWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(LuminosityJob().GetType())
        
    def ExecuteJob(self, job):
        start = COOLtime(job.GetRunNumber(), 0x0000)
        stop = COOLtime(job.GetRunNumber(), 0xFFFF)
        
        query = COOLquery()
        query.SetIOVStart(start)
        query.SetIOVStop(stop)
        
        prLumiBlock = {}
        
        for obj in query.GetFolder("/TRIGGER/LUMI/LBLESTONL"):
            payload = obj.payload()
            LumiBlock = COOLtime(obj.since()).GetLumiBlock()

            if obj.channelId() == 0:
                if not prLumiBlock.has_key(LumiBlock):
                    prLumiBlock[LumiBlock] = DataColumn()
                    
                prLumiBlock[LumiBlock].append(payload["LBAvInstLumi"])
                
        result = {}
        
        lumiBlockColumn = DataColumn()
        lumiColumn = DataColumn() 
        for lumiBlock, luminosity in prLumiBlock.items():
            lumiBlockColumn.append(lumiBlock)
            lumiColumn.append(luminosity.Mean())

        result["LumiBlock"] = lumiBlockColumn
        result["Luminosity"] = lumiColumn
        return result
#----------------------------------------------------------------------------------------------------
class PostProcessingJob(SimpleDataJob):
    def __init__(self, runNumber = None):
        SimpleDataJob.__init__(self, runNumber)
        self.SetDeadLumiBlocksList([])
        self.SetPhysicsRanges([])
        self.SetQualityRanges([])
    
    __deadLumiBlocks = []
    def GetDeadLumiBlocksList(self): return self.__deadLumiBlocks
    def SetDeadLumiBlocksList(self, val): self.__deadLumiBlocks = val  
    
    __qualityRanges = []
    def GetQualityRanges(self): return self.__qualityRanges
    def SetQualityRanges(self, val): self.__qualityRanges = val
            
    __physicsRanges = []
    def GetPhysicsRanges(self): return self.__physicsRanges
    def SetPhysicsRanges(self, val): self.____physicsRanges = val
#----------------------------------------------------------------------------------------------------        
class PostProcessingWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(PostProcessingJob().GetType())
    
    def ExecuteJob(self, job):        
        rawResult = job.GetData()
        """
        for tree in rawResult.keys():
            print tree
            for var in rawResult[tree].keys():
                print "\t",var
        """
        deadLumiBlocks = job.GetDeadLumiBlocksList()
        
        physicsRanges = job.GetPhysicsRanges()
        qualityRanges = job.GetQualityRanges()
        
        def isGoodLumiBlock(LumiBlock):
            if LumiBlock in deadLumiBlocks:
                return False
            
            inQualityRange = len(qualityRanges) == 0
            for p in qualityRanges:
                if p[0] < LumiBlock and LumiBlock < p[1]:
                    inQualityRange = True
            
            inPhysicsRange = len(physicsRanges) == 0
            for p in physicsRanges:
                if p[0] < LumiBlock and LumiBlock < p[1]:
                    inPhysicsRange = True
            
            if inQualityRange and inPhysicsRange:
                return True
            return False
        
        
        result = {}
        for columnName, column in rawResult.items():            
            goodLumiBlocks = []
            for i in range(len(column["LumiBlock"])):
                if isGoodLumiBlock(column["LumiBlock"][i]):
                    goodLumiBlocks.append(i)
                
            if len(goodLumiBlocks) == 0:
                raise Exception("Run %i is invalid. No good LumiBlocks found" % job.GetRunNumber())
            
            columnResult = {}
            for i in goodLumiBlocks:
                for varName, varColumn in column.items():
                    if not columnResult.has_key(varName):
                        columnResult[varName] = DataColumn()
                        
                    columnResult[varName].append(varColumn[i])
                
                  
                
                
            result[columnName] = columnResult
                  
                    
        joinResult = joinDataColumns(result, job.GetKey())
        
    
        for variableName in joinResult.keys():
            rIndex = max(variableName.rfind("output"), variableName.rfind("TAV")) 
            
            if rIndex == -1:
                continue 
            
            xsectionName = variableName[0:rIndex] + "CrossSection"
            
            joinResult[xsectionName] = DataColumn() 
            
            for i in range(len(joinResult["Luminosity"])):
                xsection = joinResult[variableName][i] / joinResult["Luminosity"][i] 
                joinResult[xsectionName].append(xsection)
                

        return joinResult
#----------------------------------------------------------------------------------------------------
class SimplePlotJob(SimpleDataJob):
    def __init__(self, runNumber = None):
        SimpleDataJob.__init__(self, runNumber)
        self.__dataColumns = {}
        self.__keyColumns = {}
        
    def Prepare(self):
        data = self.GetData()
        self.__keyColumns = {}
        self.__dataColumns = {}
        for columnName in data.keys():
            if not columnName == self.GetKey():
                self.__dataColumns[columnName] = data[columnName]
            else:
                self.__keyColumns[columnName] = data[columnName]

        if not len(self.__keyColumns):
            raise Exception("Key, %s, was not found for run " % (self.GetKey()) +  str(self.GetRunNumber()))

        if not len(self.__dataColumns):
            raise Exception("No data to plot for run %s" % (self.GetKey()))
        
    def GetKeyColumns(self):
        return self.__keyColumns
    
    def GetDataColumns(self):
        return self.__dataColumns    
    
    def GetVariableLabel(self, name):
        variableLabel = { \
            "LumiBlock": "Luminosity Block", \
            "TimeStamp": "Time", \
            "ATLAS_PREFERRED_Inst_CalibLumi_Val": "Luminosity", \
        }

        if variableLabel.has_key(name):
            return variableLabel[name]
        
        if "output" in name or "TAV" in name:
            return "Rate"
        
        return name
    
    def GetVariableUnits(self, name):
        variableUnits = {
            "LumiBlock": "", \
            "TimeStamp": "s", \
            "ATLAS_PREFERRED_Inst_CalibLumi_Val": "10^{33} s cm^{-2}", \
        }
    
        if variableUnits.has_key(name):
            return variableUnits[name]
        
        if "output" in name or "TAV" in name:
            return "Hz"
                
        return ""
    
    def GetVariableHumanName(self, name):
        variableHumanNames = { \
            "LumiBlock": "Luminosity Block", \
            "TimeStamp": "Time", \
            "ATLAS_PREFERRED_Inst_CalibLumi_Val": "Luminosity", \
        }
        
        if variableHumanNames.has_key(name):
            return variableHumanNames[name]
        
        if "output" in name or "TAV" in name:
            return name[0:2] + " Rate"
        
        return name
    
    def GetVariableFileName(self, name):
        variableFileNames = {
            "LumiBlock": "LB", \
            "TimeStamp": "t", \
            "ATLAS_PREFERRED_Inst_CalibLumi_Val": "LU", \
            "L1_L1A_TAV": "L1R", \
            "L2_recording_output": "L2R", \
            "L2_recording_physics_output": "L2pR", \
            "EF_recording_output": "EFR", \
            "EF_recording_physics_output": "EFpr", \
        }
        
        if variableFileNames.has_key(name):
            return variableFileNames[name]
        
        return name
#----------------------------------------------------------------------------------------------------
class PlotRatesJob(SimplePlotJob):
    def __init__(self, runNumber = None):
        SimplePlotJob.__init__(self, runNumber)
        pass
#----------------------------------------------------------------------------------------------------
class PlotRatesWorker(ProcessWorker):
    def __init__(self):
        ProcessWorker.__init__(self)
        self.SetType(PlotRatesJob().GetType())
    
    def ExecuteJob(self, job):
        userName = os.environ["USER"]
        path = "/tmp/%s/ratesplot/" % userName
        
        job.Prepare()
        
        
        variables = job.GetDataColumns()
        key = job.GetKeyColumns()
        keyName = job.GetKey()

        
        variableColor = { \
            "L1_L1A_TAV": 1, \
            "L2_recording_output": 2, \
            "L2_recording_physics_output": 2, \
            "EF_recording_output": 3, \
            "EF_recording_physics_output": 3, \
        }
        
        import ROOT
        ROOT.gROOT.Reset()
        ROOT.gROOT.SetStyle("Plain")
        ROOT.gStyle.SetCanvasColor(0)
        ROOT.gStyle.SetOptStat(000000)
        ROOT.gStyle.SetPalette(1)
        
        N = len(key.values()[0])
        
        canvas  = ROOT.TCanvas("", "", 888, 500)
        canvas.SetFillColor(0)
        canvas.SetLogy()
        canvas.SetGrid()        
        
        
        axisMin = 0.0
        axisMax = 0.0
        for column in variables.values():
            axisMin = min(column.Min(), axisMin)
            axisMax = max(column.Max(), axisMax)

        axisMax *= 1.3
        axisMin *= 0.0

        graphs = {}
        for varName in variables.keys():
            graph = ROOT.TGraph(N)
            
            for i in range(N):
                if key[keyName][i] > 0.0 and variables[varName][i] > 0.0 or 1:
                    graph.SetPoint(i, key[keyName][i], variables[varName][i])
            
            
            
            title = job.GetVariableLabel(varName) + " as function of " + job.GetVariableLabel(keyName).lower()
            graph.SetTitle(title)
            
            
            
            if variableColor.has_key(varName):
                graph.SetMarkerColor(variableColor[varName])
            else:
                graph.SetMarkerColor(2*len(graphs) + 1)
            
            
            
            title = job.GetVariableLabel(varName)
            if len(job.GetVariableUnits(varName)):
                title += " [%s]" % job.GetVariableUnits(varName)
            graph.GetYaxis().SetTitle(title)
            
            title = job.GetVariableLabel(keyName)
            if len(job.GetVariableUnits(keyName)):
                title += " [%s]" % job.GetVariableUnits(keyName)
            graph.GetXaxis().SetTitle(title)
            
            graph.GetYaxis().SetRangeUser(axisMin, axisMax)
            
            
            
            graphs[varName] = graph
        
         
        canvas.SetRightMargin(0.20) 
        firstPlot = True
        for graph in graphs.values():
            
            if firstPlot:
                firstPlot = False
                graph.Draw("A*")
            else:
                graph.Draw("* SAME")
        
        
        leg = ROOT.TLegend(0.81, 0.25, 1.0, 0.75);
        leg.SetLineWidth(0)
        leg.SetFillColor(0)
        leg.SetLineColor(0)
        
        for name in graphs.keys():
            leg.AddEntry(graphs[name], job.GetVariableHumanName(name), "lp")
        
        leg.Draw()



        fileNameList = []
        for names in variables.keys():
            fileNameList.append(job.GetVariableFileName(names)) 
        fileNameList = sorted(set(fileNameList))
        fileNameExt = ""
        for i in fileNameList:
            fileNameExt += i + "_" 
        fileNameExt = fileNameExt[0:-1]
        
        canvas.Print(path + "Rates_%s_%s_%s.ps" % (str(job.GetRunNumber()), str(job.GetVariableFileName(job.GetKey())), fileNameExt))
        canvas.Print(path + "Rates_%s_%s_%s.root" % (str(job.GetRunNumber()), str(job.GetVariableFileName(job.GetKey())), fileNameExt))
    
        
#----------------------------------------------------------------------------------------------------
def plotRuns(runNumbers, initCuts = None):

    rateSets = [ \
                    ["L1_L1A_TAV", "L2_recording_output", "EF_recording_output"], \
                    ["L1_L1A_TAV", "L2_recording_physics_output", "EF_recording_physics_output"], \
                ]
    
    multiSets = [ \
                    ["EF_e20_medium_output", "EF_mu20_output", "EF_xe60_noMu_output"], \
                ]
    
    
    onlyLoad = [ "EF_xe20_noMu_output", "EF_xe40_noMu_output", "EF_xe50_noMu_output", "EF_xe60_noMu_output"]
    
    branches = { \
#                "Lu_Rate_Luminosity": [],\
                "L1_Rate": [],\
                "L2_Rate": [],\
                "EF_Rate": [],\
                }
    
    for lists in ([["LumiBlock", "TimeStamp"]] + rateSets + multiSets + [onlyLoad]):
        for signature in lists:            
            if signature == "LumiBlock" or signature == "TimeStamp":
                for tree in branches.keys():
                    branches[tree].append(signature)
                continue
            sub = signature[0:2]
            if sub == "AT":
                branches["Lu_Rate_Luminosity"].append(signature)
            else:
                branches[sub + "_Rate"].append(signature)
    
    
    
    lumiTypes = ["LumiBlock", "TimeStamp", "Luminosity"]   
    
    rateSets.append(["EF_xe20_noMu_CrossSection", "EF_xe40_noMu_CrossSection", "EF_xe50_noMu_CrossSection", "EF_xe60_noMu_CrossSection"])
    
               
    for tree in branches.keys():
        branches[tree] = set(branches[tree]) 


    processMaster = ProcessMaster() 
    
    
    if runNumbers.__class__ != int and runNumbers.__class__ != list:
        raise Exception("Invalid argument, expected list or int")          
    
    
    if runNumbers.__class__ == int:
        runNumbers = [runNumbers]
    
    if runNumbers.__class__ == list:
        runNumbers = set(runNumbers)        
    
    for runNumber in runNumbers:
                     
        if runNumber.__class__ != int:
            raise Exception("Non-int in list of run numbers")
        
    
    if initCuts == None:
        initCuts = {}
    for runNumber in runNumbers:
        if not initCuts.has_key(runNumber):
            initCuts[runNumber] = []
    
    
    
    counterTRPDataJob = {}
    counterLuminosityJob = {}
    counterDataLimitJob = {}
    counterPostProcessingJob = {}
    for runNumber in runNumbers:
        counterTRPDataJob[runNumber] = 0
        counterLuminosityJob[runNumber] = 0
        counterDataLimitJob[runNumber] = 0


    for i in range(min(5,len(runNumbers))):
        for k in range(4):
            processMaster.AddWorker(TRPDataWorker())
            processMaster.AddWorker(PlotRatesWorker())
        
        processMaster.AddWorker(LuminosityWorker())
        
        processMaster.AddWorker(DataLimitWorker())
        processMaster.AddWorker(PostProcessingWorker())
     
    
    for runNumber in runNumbers:
                    
        
        for tree in branches.keys():
            dataJob = TRPDataJob(runNumber)
            dataJob.SetTree(tree)
            dataJob.SetBranches(branches[tree])
            processMaster.SubmitJob(dataJob)
            
            counterTRPDataJob[runNumber] += 1
        
        
        lumiJob = LuminosityJob(runNumber)
        processMaster.SubmitJob(lumiJob)
        counterLuminosityJob[runNumber] += 1


        limitJob = DataLimitJob(runNumber)
        limitJob.SetDeadtimeThreshold(0.05)
        processMaster.SubmitJob(limitJob)
        counterDataLimitJob[runNumber] += 1
        

    #processMaster.Block(None, True)
    data = {}  
    dataLimit = {}  
    while True:
        
        
        
        dataTRPResult =  processMaster.GetResult(TRPDataJob().GetType())
        if len(dataTRPResult):
            for dataJob in dataTRPResult:
                runNumber = dataJob.GetRunNumber()
                if not runNumber in runNumbers: continue
                
                print "TRPDataJob finished for", runNumber
                                
                tree = dataJob.GetTree()
                result = dataJob.GetResult()
                
                if not data.has_key(runNumber):
                    data[runNumber] = {}
                if not data[runNumber].has_key(tree):
                    data[runNumber][tree] = {}
                
                for columnName in result.keys():
                    data[runNumber][tree][columnName] = result[columnName]
                
                counterTRPDataJob[runNumber] -= 1


        dataLuminosityResult =  processMaster.GetResult(LuminosityJob().GetType())
        if len(dataLuminosityResult):
            for lumiJob in dataLuminosityResult:
                runNumber = lumiJob.GetRunNumber()
                if not runNumber in runNumbers: continue
                
                print "LuminosityJob finished for", runNumber
                
                if not data.has_key(runNumber):
                    data[runNumber] = {}
                             
                data[runNumber]["Luminosity"] = lumiJob.GetResult()
                
                counterLuminosityJob[runNumber] -= 1
                
                
        dataLimitResult =  processMaster.GetResult(DataLimitJob().GetType())
        if len(dataLimitResult):
            for limitJob in dataLimitResult:
                runNumber = limitJob.GetRunNumber()
                if not runNumber in runNumbers: continue
                
                print "DataLimitJob finished for", runNumber
                
                dataLimit[runNumber] = limitJob.GetResult()
                                
                counterDataLimitJob[runNumber] -= 1                
                


        for runNumber in runNumbers:
            if counterTRPDataJob[runNumber] == 0 and \
                            counterLuminosityJob[runNumber] == 0 and \
                            counterDataLimitJob[runNumber] == 0 and \
                            not counterPostProcessingJob.has_key(runNumber):
                
                counterPostProcessingJob[runNumber] = 0

                processingJob = PostProcessingJob(runNumber)
                """                
                for tree in data[runNumber].keys():
                    print tree
                    for var in data[runNumber][tree].keys():
                        print "\t", var, "(",len(data[runNumber][tree][var]),")"
                """             
                
                processingJob.SetData(data[runNumber])
                processingJob.SetKey("LumiBlock")
                processingJob.SetDeadLumiBlocksList(dataLimit[runNumber]["Deadtime"])
                processingJob.SetPhysicsRanges(dataLimit[runNumber]["PhysicsReady"])
                processingJob.SetQualityRanges(initCuts[runNumber])
                
                                
                processMaster.SubmitJob(processingJob)
                
                counterPostProcessingJob[runNumber] += 1
#                if jobCounterTRPData[runNumber] == 0:                    
#                    print "- Starting data merging job for run",runNumber
#                    joinJob = JoinDataJob(runNumber)
#                    joinJob.SetData(data[runNumber])
#                    joinJob.SetKey("LumiBlock")
#                    processMaster.SubmitJob(joinJob)
#                    #data[runNumber] = joinDataColumns(data[runNumber], "LumiBlock")
                               

        dataPostProcessingResult =  processMaster.GetResult(PostProcessingJob().GetType())
        if len(dataPostProcessingResult):
            for processingJob in dataPostProcessingResult:
                runNumber = processingJob.GetRunNumber()
                if not runNumber in runNumbers: continue
                
                print "PostProcessingJob finished for", runNumber
                
                
                data[runNumber] = processingJob.GetResult()
                counterPostProcessingJob[runNumber] -= 1
                
                for lumiType in lumiTypes:             
                    for rateSet in rateSets:
                        dataToPlot = {}
                        
                        plotRatesJob = PlotRatesJob(runNumber)                        
                        plotRatesJob.SetKey(lumiType)
                        
                        
                        for nam in data[runNumber].keys():
                            print nam
                        print
                        
                        try: 
                            dataToPlot[lumiType] = data[runNumber][lumiType]
                        except Exception as e:
                            raise Exception(str(e) + ", runNumber == " + str(runNumber))
                                        
                        for rateType in rateSet:
                            if rateType in data[runNumber].keys():
                                dataToPlot[rateType] = data[runNumber][rateType]

                        plotRatesJob.SetData(dataToPlot)
                        
                        processMaster.SubmitJob(plotRatesJob)
        
                                       
                        
        dataPlotRatesResult = processMaster.GetResult(PlotRatesJob().GetType())
        if len(dataPlotRatesResult):
            for plotRatesJob in dataPlotRatesResult:
                runNumber = plotRatesJob.GetRunNumber()
                if not runNumber in runNumbers: continue
                
                print "PlotRatesJob finished for", runNumber
        

        errors = processMaster.GetError()
        for type, failedJobs in errors.items():
            for failedJob in failedJobs:
                
                run = failedJob.GetRunNumber()
                
                isUnderstood = False
                isCritical = True
                
                if type == PostProcessingJob().GetType():
                    isUnderstood = True                                        
                    isCritical = True
                
                if type == TRPDataJob().GetType():
                    exception = failedJob.GetResult()
                    errType = exception.GetType()
                    
                    if errType == "Branch not found":
                        isUnderstood = True
                        
                        newBranches = failedJob.GetBranches()
                        print newBranches
                        newBranches.remove(exception.GetBranch())
                        
                        if not len(newBranches) <= 2:
                            print newBranches
                            isCritical = True
                        """
                        else:
                            print "-- Resubmitting trpjob"
                            isCritical = False
                            failedJob.SetResult(None)
                            failedJob.SetBranches(newBranches)
                            processMaster.SubmitJob(failedJob)
                        """
                    
                    if errType == "Tree not found":
                        isUnderstood = True
                        isCritical = True
                    
                    
                
                if isCritical:
                    print 
                    print "* A worker returned a critical error. Run %i can't be processed." % (run)
                    print "  (?) Worker type:",failedJob.GetType()
                    print "  (?) Reason:",failedJob.GetResult()
                    print "  (!) Run will be ignored"
                    print 
                    
                    if data.has_key(run):
                        data.pop(run)
                    if run in runNumbers:
                        runNumbers.remove(run)
                        
                    if run in counterTRPDataJob.keys():
                        counterTRPDataJob.pop(run)

                    if run in counterLuminosityJob.keys():
                        counterLuminosityJob.pop(run)                        

                    if run in counterDataLimitJob.keys():
                        counterDataLimitJob.pop(run)

                    if run in counterPostProcessingJob.keys():
                        counterPostProcessingJob.pop(run)                        
                       
                    continue
                                    
                if not isUnderstood:
                    print
                    print "* Whoooops.. A worker returned an error which I do not yet understand..."
                    print "  (?) Worker type:",type
                    print "  (?) Run number:",run
                    print "  (?) Worker result:", failedJob.GetResult()
                    print
                                                       
            
        
        if not processMaster.IsActive():
            print "Finished.."
            #pass
            break

        time.sleep(1)
    

    
    if len(data.keys()):
        if str("LumiBlock") in lumiTypes:
            lumiTypes.remove("LumiBlock")
            
        data = joinDataColumns(data) 
        
        for lumiType in lumiTypes:
            for varSet in multiSets:
                job = PlotRatesJob("MultiPlot")
                job.SetKey(lumiType)
                
                dataToPlot = {}
                dataToPlot[lumiType] = data[lumiType]
                for branch in varSet:
                    dataToPlot[branch] = data[branch]
                    
                job.SetData(dataToPlot)
                
                processMaster.SubmitJob(job)
        
        processMaster.Block(None, True)
        
        for failedJob in processMaster.GetError(PlotRatesJob().GetType()):
            print failedJob.GetResult()
        
    #else:
    #    print "Nothing left to plot.. Quitting.."

    processMaster.KillAll()
    return
        
    
    
    
#------------------------------------------------------------------------------------------
if __name__ == "__main__":
#    runNumber = [186049, 185998, 185976]
#    runNumber = [184169]
#    plotRuns(runNumber)

    from model.parser.XMLTag import XMLTag
    
    xml = XMLTag()
    xml.FromFile(os.environ["PWD"] + "/" + "data11_7TeV.periodAllYear_DetStatus-v19-pro08-05_CoolRunQuery-00-03-98_Susy.xml")
    
    
    runList = []
    runQuality = {}
    
    for NamedLumiRange in xml.GetChildrenNamed("NamedLumiRange"):
        for LumiBlockCollection in NamedLumiRange.GetChildrenNamed("LumiBlockCollection"):
            Run = LumiBlockCollection.GetChildNamed("Run")
            
            runNumber = int(Run.GetContent())
            runList.append(runNumber)
            runQuality[runNumber] = []
            
            for LBRange in LumiBlockCollection.GetChildrenNamed("LBRange"):
                start = int(LBRange.GetAttribute("Start"))
                end = int(LBRange.GetAttribute("End"))
    
                runQuality[runNumber].append(tuple((start, end)))
    
    
    for i in runList:
        try:
            TRPFile(i).get_path()
        except:
            runList.remove(i)
    
    
    
    testRun = [184169]
    testQuality = {}
    testQuality[testRun[0]] = []
    if runQuality.has_key(testRun[0]):
        testQuality[testRun[0]] = runQuality[testRun[0]] 
    plotRuns(testRun, testQuality)
    
    
    a = time.time()
    #plotRuns(runList, runQuality)
    print
    print time.time() - a, len(runList), (time.time() - a) /len(runList)  
    print
    """
    testRun = testRun[0]
    
    q = COOLquery()
    q.SetIOVStart(COOLtime(testRun, 0x0000))
    q.SetIOVStop(COOLtime(testRun, 0xFFFF))
    
    deadtimeId = None
    for obj in q.GetFolder("/TRIGGER/LVL1/Menu"):
        pl = obj.payload()
        if pl["ItemName"] == "L1_EM16":
            deadtimeId = obj.channelId()
    
    if deadtimeId == None:
        raise Exception("channelId() not found in COOL for L1_EM16")
    
    
    for obj in q.GetFolder("/TRIGGER/LUMI/LVL1COUNTERS"):
        pl = obj.payload()
        
        if obj.channelId() == deadtimeId:
            try:
                deadTime = 1.0-(float(pl["L1Accept"]) / float(pl["BeforePrescale"])) 
            except:
                deadTime = 0.0
                
            print COOLtime(obj.since()).GetLumiBlock(), "\t", deadTime
        
    """