#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#====================================================================================================================

__author__  = "Peter Radloff"
__version__ = "$Revision: 1.0$"
__doc__     = "Fast Rate plotter from COOL database"

#====================================================================================================================

from PyCool import cool
from multiprocessing import Queue, cpu_count, Process
from time import sleep
import argparse,sys,logging,time,os
from math import log10
import signal
from Queue import Empty

logger = logging.getLogger(__name__)
ch = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
ch.setFormatter(formatter)
logger.addHandler(ch)

if __name__=='__main__':

	parser = argparse.ArgumentParser(prog='COOLRates.py',description='Plots rate information from COOL')
	parser.add_argument(dest='RATES',help='String specifing rates to plot, comma seperated with a bar seperating groups.  Separate rate and alias with colon: "L1_L1A:L1_total", add -0,-1 for before prescale/after prescale [default is output]\nExample:\nCOOLQuery.py "L1_L1A:L1_total,L2_total,EF_total|L1_EM10VH,L1_EM6_2MU6,L1_2EM6_MU6,L1_EM6,L1_EM12|EF_2g40_loose,EF_g100_loose,EF_mu18_tight_e7_medium1,EF_e60_medium1" -r 201257 201269')
	parser.add_argument('-r','--run',default=None,dest='RUNNUMBER', metavar='R',type=int, nargs='+',help='Run number(s) for which to plot rates')
	parser.add_argument('-f','--fill',default=None,dest='FILLNUMBER', metavar='F',type=int, nargs='+',help='Fill number(s) for which to plot rates')
	parser.add_argument('-t','--timerange',default=None,dest='TIMERANGE',help='Time range for which to plot rates YYYY-MM-DD:HH:MM:SS;YYYY-MM-DD:HH:MM:SS')
	parser.add_argument('-n','--nofilter',dest='FILTER',action='store_false',help='Turns off filtering for dead time, stable beams and Atlas ready.')
	parser.add_argument('-b','--batch',dest='BATCH',action='store_true',help='Enables batch mode, automatically saves canvases instead of displaying')
	parser.add_argument('-x','--xunit',default='time',dest='XUNIT',help='X axis unit to plot rates against, ["time","luminosity","noBX"]')
	parser.add_argument('-v','--verbose',dest='VERBOSE',action='store_true',help='Sets verbosity to TRUE, default is FALSE')
	parser.add_argument('-l','--label',dest='LABEL',action='store_true',help='Adds Atlas Trigger Operations label')
	parser.add_argument('-a','--linear',dest='LINEAR',action='store_false',help='Sets linear scale for plotting')
	parser.add_argument('-s','--bunches',default=None,type=int,dest='LUMINOUSBUNCHES',help='Required luminous bunches')
	parser.add_argument('-e','--envelope',dest='ENVELOPE',action='store_true',help='Draws filled error envelope, recommended for large fluctuations compared average rate')
	parser.add_argument('-p','--points',dest='POINTS',action='store_true',help='Draws standard profile points')
	parser.add_argument('-m','--processes',dest='PROCESSES',default=2,type=int,help='Sets number of processes for multiprocessing retrieval of COOL data')	
	args = parser.parse_args()

from ROOT import TDatime, TProfile, TCanvas, TLegend, TGaxis, gROOT, SetOwnership, TColor, TLatex

#====================================================================================================================

def waitForReturn(fd):
	logger.critical('Press Return to safely exit')
	signal.signal(signal.SIGINT, signal.SIG_IGN)
	while(1):
		ch = os.read(fd,1)
		if ch == '\n': break
	logger.critical('Received signal, exiting at first chance')
	return

#====================================================================================================================

def commonKeys(dictionaries):
	if not len(dictionaries): return []
	elif not len(dictionaries) >= 1: return dictionaries[0].keys()
	keys = set(dictionaries[0].keys())
	for dictionary in dictionaries[1:]:
		keys = set(dictionary.keys())&keys
	return list(keys)

#====================================================================================================================

class COOLQueryWorker():
	def __init__(self,timeConverter = {}):
		self.db_svc = cool.DatabaseSvcFactory.databaseService()
		self.timeConverter=timeConverter
		self.db = {}
		self.folder = {}

	def getdb(self,dbName):
		if dbName not in self.db:
			self.db[dbName] = self.db_svc.openDatabase(dbName,True)
		return self.db.get(dbName)
	
	def getFolder(self,folderName):
		if folderName not in self.folder:
			dbName = self.getdbName(folderName)
			db = self.getdb(dbName)
			self.folder[folderName] = db.getFolder(folderName)
		return self.folder.get(folderName)
		
	def getdbName(self,folderName):
		if folderName.startswith('/LHC/DCS/'):
			return 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;'
		if folderName.startswith('/TRIGGER/HLT/') and 'Rates' in folderName:
			return "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;"
		if not folderName.startswith('/'): folderName = '/'+folderName
		return "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_{0};dbname=COMP200;".format(folderName.split("/")[1])

	def convertTime(self,timeStart,timeEnd,timeConverter):
		validRunLbs = [runLb for runLb,time in timeConverter.items() if timeStart<=time<=timeEnd]
		return validRunLbs

	def processQuery(self,queryBundle):
		folder = self.getFolder(queryBundle.folderName)
		folder.setPrefetchAll(False)
		result = {}		
		for payloadName,payloadAlias,cast in queryBundle.payloadRequests: result[payloadAlias]={}
		folderIterator = folder.browseObjects(queryBundle.IoVStart,queryBundle.IoVEnd,queryBundle.channelSelection)
		if queryBundle.timeBased:
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				for payloadName,payloadAlias,cast in queryBundle.payloadRequests:
					payload = cast(currentSlice.payloadValue(payloadName))				
					if payloadName in queryBundle.payloadRequirements:
						if not eval(queryBundle.payloadRequirements[payloadName].format(repr(payload))): continue
					runLbs = self.convertTime(int(currentSlice.since()),int(currentSlice.until()),queryBundle.timeConverter)		
					result[payloadAlias].update(dict.fromkeys(runLbs,payload))
		else:
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				for payloadName,payloadAlias,cast in queryBundle.payloadRequests:
					payload = cast(currentSlice.payloadValue(payloadName))
					if payloadName in queryBundle.payloadRequirements:
						if not eval(queryBundle.payloadRequirements['payloadName'].format(repr(payload))): continue
					runLbs = range(max(currentSlice.since(),queryBundle.IoVStart),min(currentSlice.until(),queryBundle.IoVEnd))
					result[payloadAlias].update(dict.fromkeys(runLbs,payload))

		folderIterator.close()
		return result
	
	def close(self):
		for db in self.db.values(): db.closeDatabase()
	
	def __call__(self,queueIn,queueOut):
		signal.signal(signal.SIGINT, signal.SIG_IGN)
		try:
			while True:
				try:
					queryBundle = queueIn.get(False,.001)
				except Empty:
					sleep(.001)
					continue
				if queryBundle is True:
					break
				resultBundle = self.processQuery(queryBundle)
				queueOut.put(resultBundle)
		except Exception as exception:
			logger.critical(exception)
		
		self.close()
		queueOut.put(True)
		return

#====================================================================================================================		

class COOLQueryHandler():
	def __init__(self,mySignal,numProc=None,timeConverter={},verbose=False):
		self.troughQueue = Queue()
		self.resultQueue = Queue()
		self.QueryProcesses = []
		self.result = {}
		self.counter=0
		self.verbose = verbose
		self.timeConverter = timeConverter
		self.mySignal = mySignal
		if numProc is None: numProc = cpu_count()
		self.numProc = numProc

	def createProcesses(self):
		self.QueryProcesses = []
		for i in range(self.numProc):		
			process = Process(target = COOLQueryWorker(timeConverter=self.timeConverter), args = (self.troughQueue,self.resultQueue,))
			self.QueryProcesses.append(process)
			
	def addQueryBundle(self,queryBundle):
		self.counter+=1
		self.troughQueue.put(queryBundle)
	
	def beginQuery(self):
		self.createProcesses()
		for i in range(self.numProc):
			self.troughQueue.put(True)
		logger.info('{0} queries distributed over {1} workers'.format(self.counter,len(self.QueryProcesses)))
		for process in self.QueryProcesses: process.start()
		try:
			self.collectResults()
		except KeyboardInterrupt:
			logger.critical('Caught signal, terminating processes')
			self.terminateProcesses()
			return
		for process in self.QueryProcesses: process.join()
		return
	
	def processResults(self,resultBundle):
		for resultAlias in resultBundle:
			if resultAlias not in self.result: self.result[resultAlias]={}
			self.result[resultAlias].update(resultBundle[resultAlias])	
	
	def terminateProcesses(self):
		while 1:
			try:self.troughQueue.get(False)
			except Empty: break
		logger.critical('Queue is empty')
		for i in range(self.numProc): self.troughQueue.put(True)
		time.sleep(1)
		for process in self.QueryProcesses:
			process.terminate()
			process.join()
		logger.critical('Processes are joined')
		raise KeyboardInterrupt

	def collectResults(self):
		finished = 0
		startingQueries = self.counter
		target=0.
		while True:
			if not self.mySignal.is_alive():
				raise KeyboardInterrupt
				break
				#sys.exit(0)
			try: resultBundle = self.resultQueue.get(False,0.001)
			except Empty:
				sleep(.001)
				continue
			if resultBundle is True:
				finished+=1
				if finished == self.numProc: break
				continue
			self.counter-=1
			percentComplete = min(100,int(round(float(startingQueries-self.counter)/startingQueries*100+.5)))
			if percentComplete>=target:
				target+=5.
				logger.info('{0}% complete'.format(percentComplete))
			self.processResults(resultBundle)
#		except KeyboardInterrupt:
#			logger.critical('Caught signal, terminating processes')
#			self.terminateProcesses()
		return

#====================================================================================================================

def createTimeConverter(runLbRanges):
	timeConverter = {}
	worker = COOLQueryWorker()
	
	if runLbRanges is not None:
		folder = worker.getFolder('/TRIGGER/LUMI/LBLB')
		folder.setPrefetchAll(False)
		for runLbStart,runLbEnd in runLbRanges:
			timeConverter[(runLbStart,runLbEnd)]={}
			folderIterator = folder.browseObjects(runLbStart,runLbEnd,cool.ChannelSelection())
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				timeConverter[(runLbStart,runLbEnd)][int(currentSlice.since())] = int(currentSlice.payloadValue('StartTime'))
			folderIterator.close()
			
	return timeConverter

#====================================================================================================================

class QueryBundle():
	def __init__(self,IoVStart,IoVEnd,payloadRequests,folderName,channelSelection=cool.ChannelSelection(),timeBased=False,payloadRequirements={},timeConverter={}):
		self.IoVStart = IoVStart
		self.IoVEnd = IoVEnd
		for request in payloadRequests:
			if not len(request)==3:
				print(payloadRequests)
				sys.exit(0)
		self.payloadRequests = payloadRequests
		self.payloadRequirements = payloadRequirements
		self.folderName = folderName
		self.timeBased=timeBased
		self.timeConverter=timeConverter
		self.channelSelection = channelSelection
		
#====================================================================================================================

##########################################
#                                        #
#  For trigger type (0,1,2,3) defines:   #
#				folder name                      #
#				payload name                     #
#				turn counts (if necessary)       #
#                                        #
##########################################

folderLocations = {}

folderLocations[0] = {}
folderLocations[0]['Input'] = ('/TRIGGER/LUMI/LVL1COUNTERS','BeforePrescale','/TRIGGER/LVL1/CTPCORELBDATA','BeforePrescaleTurnCounter')
folderLocations[0]['AfterPrescale'] = ('/TRIGGER/LUMI/LVL1COUNTERS','AfterPrescale','/TRIGGER/LVL1/CTPCORELBDATA','AfterPrescaleTurnCounter')
folderLocations[0]['Output'] = ('/TRIGGER/LUMI/LVL1COUNTERS','L1Accept','/TRIGGER/LVL1/CTPCORELBDATA','L1AcceptTurnCounter')

folderLocations[1] = {}
folderLocations[1]['Input'] = ('/TRIGGER/HLT/L2Rates','Prescale',None,None)
folderLocations[1]['AfterPrescale'] = ('/TRIGGER/HLT/L2Rates','Input',None,None)
folderLocations[1]['Output'] = ('/TRIGGER/HLT/L2Rates','Output',None,None)

folderLocations[2] = {}
folderLocations[2]['Input'] = ('/TRIGGER/HLT/EFRates','Prescale',None,None)
folderLocations[2]['AfterPrescale'] = ('/TRIGGER/HLT/EFRates','Input',None,None)
folderLocations[2]['Output'] = ('/TRIGGER/HLT/EFRates','Output',None,None)

folderLocations[3] = {}
folderLocations[3]['Input'] = ('/TRIGGER/HLT/TotalRates','Input',None,None)
folderLocations[3]['AfterPrescale'] = ('/TRIGGER/HLT/TotalRates','Input',None,None)
folderLocations[3]['Output'] = ('/TRIGGER/HLT/TotalRates','Output',None,None)

def rateNameInfo(runLbRanges,mySignal,numProc=1):
	def rateNameWorker(queueIn,queueOut):
		worker = COOLQueryWorker()
		while(1):
			try: run = queueIn.get(False,0.001)
			except Empty:
				sleep(.001)
				continue
			if run is True:
				break
			runLbStart=run<<32
			runLbEnd=runLbStart+1
			nameToChannel={}
			nameToFolder={}
			#L1 Triggers
			folder = worker.getFolder("/TRIGGER/LVL1/Menu")
			folder.setPrefetchAll(False)
			folderIterator = folder.browseObjects(runLbStart,runLbEnd,cool.ChannelSelection())
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				nameToChannel[currentSlice.payloadValue('ItemName')] = currentSlice.channelId()
				nameToFolder[currentSlice.payloadValue('ItemName')] = 0
			folderIterator.close()			
			#HLT Triggers
			folder = worker.getFolder("/TRIGGER/HLT/Menu")
			folder.setPrefetchAll(False)
			folderIterator = folder.browseObjects(runLbStart,runLbEnd,cool.ChannelSelection())
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				if currentSlice.payloadValue('ChainName').startswith('L2'):
					nameToChannel[currentSlice.payloadValue('ChainName')] = int(currentSlice.payloadValue('ChainCounter'))
					nameToFolder[currentSlice.payloadValue('ChainName')] = 1
				elif currentSlice.payloadValue('ChainName').startswith('EF'):
					nameToChannel[currentSlice.payloadValue('ChainName')] = int(currentSlice.payloadValue('ChainCounter'))
					nameToFolder[currentSlice.payloadValue('ChainName')] = 2
			folderIterator.close()			
			#Totals/Streams
			folder = worker.getFolder("/TRIGGER/HLT/TotalRates")
			folder.setPrefetchAll(False)
			for channel in folder.listChannels():
				nameToChannel[folder.channelName(channel)] = channel
				nameToFolder[folder.channelName(channel)] = 3
			queueOut.put(({run:nameToChannel},{run:nameToFolder}))
		queueOut.put(True)
		return	

	def terminateProcesses(QueryProcesses,troughQueue):
		while 1:
			try:troughQueue.get(False)
			except Empty: break
		logger.critical('Queue is empty')
		for i in range(numProc): troughQueue.put(True)
		time.sleep(1)
		for process in QueryProcesses: 
			process.terminate()
			process.join()
		logger.critical('Processes are joined')
		raise KeyboardInterrupt

	nameToChannel = {}
	nameToFolder = {}

	troughQueue = Queue()
	resultQueue = Queue()

	processes = [Process(target=rateNameWorker,args=(troughQueue,resultQueue,)) for i in range(numProc)]
	for process in processes: process.start()
	
	counter = 0
	target = 0.
	finished = 0
	
	runs = list(set([runLbStart>>32 for runLbStart,runLbEnd in runLbRanges]))

	for run in runs:
		counter+=1
		troughQueue.put(run)
		
	startingQueries = counter

	logger.info('{0} queries distributed over {1} workers'.format(startingQueries,numProc))

	for i in range(numProc): troughQueue.put(True)
	
	try:
		while(1):
			if not mySignal.is_alive():
				raise KeyboardInterrupt
				break
			try: result = resultQueue.get(False,0.001)
			except Empty:
				time.sleep(.001)
				continue
			if result is True:
				finished+=1
				if finished==numProc: break
				continue
			tempNTC,tempNTF = result
			nameToChannel.update(tempNTC)
			nameToFolder.update(tempNTF)
			counter-=1
			percentComplete = min(100,int(round(float(startingQueries-counter)/startingQueries*100+.5)))
			if percentComplete>=target:
				target+=5.
				logger.info('{0}% complete'.format(percentComplete))
	except KeyboardInterrupt:
		logger.critical('Caught signal, terminating processes')
		terminateProcesses(processes,troughQueue)
		
	for process in processes: process.join()

	return nameToChannel,nameToFolder

#====================================================================================================================

class RateQueryWorker(COOLQueryWorker):
	def __init__(self):
		COOLQueryWorker.__init__(self)

	def processQuery(self,rateBundle):
		result = {}
		result[rateBundle.alias] = {}

		#Get first result
		firstResult = {}
		folder = self.getFolder(rateBundle.folderName)
		folder.setPrefetchAll(False)
		folderIterator = folder.browseObjects(rateBundle.IoVStart,rateBundle.IoVEnd,rateBundle.channelSelection)
		while folderIterator.goToNext():
			currentSlice = folderIterator.currentRef()
			firstResult[currentSlice.since()]=float(currentSlice.payloadValue(rateBundle.payloadName))
		folderIterator.close()
		
		#Get second result (if counts based)
		if rateBundle.turnCountFolderName is not None:
			secondResult = {}
			folder = self.getFolder(rateBundle.turnCountFolderName)
			folder.setPrefetchAll(False)
			folderIterator = folder.browseObjects(rateBundle.IoVStart,rateBundle.IoVEnd,cool.ChannelSelection())
			while folderIterator.goToNext():
				currentSlice = folderIterator.currentRef()
				secondResult[currentSlice.since()]=float(currentSlice.payloadValue(rateBundle.turnCountPayloadName))
			folderIterator.close()
			result[rateBundle.alias].update(dict((key,11246.0*firstResult.get(key)/secondResult.get(key)) for key in commonKeys([firstResult,secondResult])))		
		else: 
			result[rateBundle.alias].update(firstResult)
		return result

#====================================================================================================================

class RateQueryBundle():
	def __init__(self,IoVStart,IoVEnd,alias,folderName,payloadName,turnCountFolderName,turnCountPayloadName,channel):
		self.IoVStart = IoVStart
		self.IoVEnd = IoVEnd
		self.alias = alias
		self.folderName = folderName
		self.payloadName = payloadName
		self.turnCountFolderName = turnCountFolderName
		self.turnCountPayloadName = turnCountPayloadName
		self.channelSelection = cool.ChannelSelection(channel)
				
#====================================================================================================================

def generateRateQueryBundles(name,level,runLbRanges,nameToChannel,nameToFolder,alias=None):
	queryBundles = []
	if alias is None: alias = name
	for runLbStart,runLbEnd in runLbRanges:
		run = runLbStart>>32
		if run not in nameToChannel:continue
		if name not in nameToChannel[run]:continue
		channel = nameToChannel[run][name]
		if run not in nameToFolder:continue
		if name not in nameToFolder[run]:continue
		rateType = nameToFolder[run][name]
		if rateType not in folderLocations:continue
		if level not in folderLocations[rateType]:continue
		folderName,payloadName,turnCounterFolder,turnCountPayloadName = folderLocations[rateType][level]			
		queryBundle = RateQueryBundle(runLbStart,runLbEnd,alias,folderName,payloadName,turnCounterFolder,turnCountPayloadName,channel)
		queryBundles.append(queryBundle)
	return queryBundles	
	
#====================================================================================================================

class RateQueryHandler(COOLQueryHandler):
	def __init__(self,mySignal,numProc=None,verbose=False):
		COOLQueryHandler.__init__(self,mySignal,numProc=numProc,verbose=verbose)	
		self.QueryWorkers = []
		for i in range(self.numProc):
			worker = RateQueryWorker()
			self.QueryWorkers.append(worker)

	def createProcesses(self):
		self.QueryProcesses = []
		for i in range(self.numProc):		
			process = Process(target = RateQueryWorker(), args = (self.troughQueue,self.resultQueue,))
			self.QueryProcesses.append(process)
#====================================================================================================================

def parseRatesString(ratesString):
	groups_ = []
	groups = ratesString.split('|')
	for group in groups:
		if group == '': continue
		groups_.append([])
		for rate in group.split(','):
			if rate == '': continue
			if ':' in rate:
				rateName,alias = tuple(rate.split(':'))
			else:
				rateName,alias = (rate,rate)
			if rateName.endswith('-0'):
				level = 'Input'
				rateName = rateName[:-2]
			elif rateName.endswith('-1'):
				level = 'AfterPrescale'
				rateName = rateName[:-2]
			else:
				level = 'Output'
			groups_[-1].append((rateName,alias,level))
	return groups_

#====================================================================================================================

def runNumberToRunLbRange(runNumbers):
	runLbRanges = []
	worker = COOLQueryWorker()
	folder = worker.getFolder('/TRIGGER/LUMI/LBLB')
	folder.setPrefetchAll(False)
	channelSelection = cool.ChannelSelection()
	for runNumber in runNumbers:
		folderIterator = folder.browseObjects(runNumber<<32,((runNumber+1)<<32)-1,channelSelection)
		if folderIterator.isEmpty(): 
			folderIterator.close()
			continue
		runLbs = []
		while folderIterator.goToNext():
			currentSlice = folderIterator.currentRef()
			runLbs.append(currentSlice.since())
		folderIterator.close()	
		runLbRanges.append((min(runLbs),max(runLbs)))
	worker.close()
	return runLbRanges

#====================================================================================================================

def timeRangeToRunLbRange(timeRange):

	if len(timeRange.split(';'))!=2:
		logger.critical('Time range "{0}" does not match YYYY-MM-DD:HH:mm:ss;YYYY-MM-DD:HH:mm:ss'.format(timeRange))
		sys.exit(0)
	timeStart,timeEnd = tuple(timeRange.split(';'))
	try:
		timeStart = time.strptime(timeStart,'%Y-%m-%d:%H:%M:%S')
		timeEnd = time.strptime(timeEnd,'%Y-%m-%d:%H:%M:%S')
	except Exception:
		logger.critical('Time range "{0}" does not match YYYY-MM-DD:HH:mm:ss;YYYY-MM-DD:HH:mm:ss'.format(timeRange))
		sys.exit(0)

	timeStart = int(time.mktime(timeStart))*10**9
	timeEnd = int(time.mktime(timeEnd))*10**9

	return nanoTimeRangesToRunLbRanges([(timeStart,timeEnd)])

#====================================================================================================================

def nanoTimeRangesToRunLbRanges(nanoTimeRanges):
	runLbRanges = []
	worker = COOLQueryWorker()
	folder = worker.getFolder('/TRIGGER/LUMI/LBTIME')
	folder.setPrefetchAll(False)
	channelSelection = cool.ChannelSelection()
	for timeStart,timeEnd in nanoTimeRanges:
		folderIterator = folder.browseObjects(timeStart,timeEnd,channelSelection)
		if folderIterator.isEmpty():
			folderIterator.close()
			continue
		temp = {}	
		while	folderIterator.goToNext():
			currentSlice = folderIterator.currentRef()
			run = int(currentSlice.payloadValue('Run'))
			lb = int(currentSlice.payloadValue('LumiBlock'))
			runLb = (run<<32)+lb
			if run not in temp: temp[run]=[]
			temp[run].append(runLb)
		for run,runLbs in temp.items():
			runLbRanges.append((min(runLbs),max(runLbs)))
		folderIterator.close()
	worker.close()
	return runLbRanges

#====================================================================================================================

def fillNumberToRunLbRange(fillNumbers):
	minFill = min(fillNumbers)-1
	worker = COOLQueryWorker()
	result = {}
	folder = worker.getFolder('/LHC/DCS/FILLSTATE')
	folder.setPrefetchAll(False)
	timeEnd = int(time.mktime(time.localtime()))*10**9
	channelSelection = cool.ChannelSelection()
	for i in range(25):
		timeStart = timeEnd-259200000000000
		#load information in 3 day chunks until lowest fill is less than or equal to minFill (break after loading full year)
		folderIterator = folder.browseObjects(timeStart,timeEnd,channelSelection)
		while folderIterator.goToNext():
			currentSlice = folderIterator.currentRef()
			fillNumber = currentSlice.payloadValue('FillNumber')
			if not fillNumber.isdigit(): continue
			fillNumber = int(fillNumber)
			if fillNumber not in result: result[fillNumber]=[]
			result[fillNumber].append(currentSlice.since())
			result[fillNumber].append(currentSlice.until())
		folderIterator.close()
		timeEnd = timeStart	
		if not len(result): continue
		if min(result.keys())<=minFill: break
	nanoTimeRanges = []
	for fillNumber in list(set(fillNumbers)&set(result.keys())):
		nanoTimes = result[fillNumber]
		nanoTimeRanges.append((min(nanoTimes),max(nanoTimes)))
	worker.close()
	return nanoTimeRangesToRunLbRanges(nanoTimeRanges)

#====================================================================================================================
"""
This marks the beginning of plotting related tools
"""
#====================================================================================================================

def setAtlasStyle():
	from ROOT import TStyle
		
	atlasStyle = TStyle("ATLAS","Atlas style")

	atlasStyle.SetFrameBorderMode(0)
	atlasStyle.SetFrameFillColor(0)
	atlasStyle.SetCanvasBorderMode(0)
	atlasStyle.SetCanvasColor(0)
	atlasStyle.SetPadBorderMode(0)
	atlasStyle.SetPadColor(0)
	atlasStyle.SetStatColor(0)	
	atlasStyle.SetPaperSize(20,26)
	atlasStyle.SetPadTopMargin(0.07)
	atlasStyle.SetPadRightMargin(0.05)
	atlasStyle.SetPadBottomMargin(0.16)
	atlasStyle.SetPadLeftMargin(0.16)
	atlasStyle.SetTitleXOffset(1.4)
	atlasStyle.SetTitleYOffset(1.2)
	font=43
	tsize=0.05
	atlasStyle.SetTextFont(font)
	atlasStyle.SetTextSize(tsize)
	atlasStyle.SetLabelFont(font,"x")
	atlasStyle.SetTitleFont(font,"x")
	atlasStyle.SetLabelFont(font,"y")
	atlasStyle.SetTitleFont(font,"y")
	atlasStyle.SetLabelFont(font,"z")
	atlasStyle.SetTitleFont(font,"z")
	atlasStyle.SetLabelSize(tsize,"x")
	atlasStyle.SetTitleSize(tsize,"x")
	atlasStyle.SetLabelSize(tsize,"y")
	atlasStyle.SetTitleSize(tsize,"y")
	atlasStyle.SetLabelSize(tsize,"z")
	atlasStyle.SetTitleSize(tsize,"z")
	atlasStyle.SetMarkerStyle(20)
	atlasStyle.SetMarkerSize(1.2)
	atlasStyle.SetHistLineWidth(2)
	atlasStyle.SetLineStyleString(2,"[12 12]")
	atlasStyle.SetEndErrorSize(0.)
	atlasStyle.SetOptTitle(0)
	atlasStyle.SetOptStat(0)
	atlasStyle.SetOptFit(0)
	atlasStyle.SetPadTickX(1)
	atlasStyle.SetPadTickY(1)

	gROOT.SetStyle("ATLAS")
	gROOT.ForceStyle()
	
#====================================================================================================================

######################################################
#                                                    #
# Set default color, styles and sizes (14 available) #
#                                                    #
######################################################

MarkerColors = [TColor.GetColor(hexColor) for hexColor in ['#000000','#2956B2','#7DBD00','#FF542E','#AA0078','#FFCC00','#D01A55','#A6AF7A','#DCF600','#FF5B00','#659CEF','#F2BC00','#FFF0BA']]
MarkerStyles = [20,21,22, 29,31, 33, 34, 23,24,25,26,27,28, 30]
MarkerSizes =	 [1.,1.,1.4,1.8,1.,1.8,1.4,1.4,1.,1.,1.,1.,1.,1.8]

def fillProfile(profileName,numBins,xMin,xMax,xValues,yValues):
	profile = TProfile(profileName,profileName,int(numBins),xMin,xMax)
	for x,y in zip(xValues,yValues):
		profile.Fill(x,y)
	return profile

#====================================================================================================================

def fillRateProfiles(data):
	rates = []
	if 'profiles' not in data: data['profiles'] = {}
	for group in data['groups']:
		for rate in group:
			rates.append(rate)
	xOffset = data['plotConfig']['xOffset']
	xScale = data['plotConfig']['xScale']
	for rateName,alias,level in rates:
		if data['xUnit'] == 'noBX':
			if 'filter' in data:
				runLbs = commonKeys([data[data['xUnit']],data['filter'],data['rates'][alias],data['luminosity']])
			else:
				runLbs = commonKeys([data[data['xUnit']],data['rates'][alias],data['luminosity']])
		else:
			if 'filter' in data:
				runLbs = commonKeys([data[data['xUnit']],data['filter'],data['rates'][alias]])
			else:
				runLbs = commonKeys([data[data['xUnit']],data['rates'][alias]])		
		xValues = []
		yValues = []
		if data['xUnit'] == 'noBX':
			for runLb in runLbs:
				luminosity = data['luminosity'].get(runLb)
				if luminosity==0.: continue
				yValue = data['rates'][alias].get(runLb)/luminosity
				xValues.append(data[data['xUnit']].get(runLb)/xScale - xOffset)
				yValues.append(yValue)
		else:
			for runLb in runLbs:
				xValues.append(data[data['xUnit']].get(runLb)/xScale - xOffset)
				yValues.append(data['rates'][alias].get(runLb))
		data['profiles'][alias] = fillProfile(alias,data['plotConfig']['numBins'],data['plotConfig']['xMin'],data['plotConfig']['xMax'],xValues,yValues)
	return data

#====================================================================================================================

####################################
#                                  #
#		Fills plotting data:           #
#			1) xMin                      #
#			2) xMax                      #
#			3) xLabel                    #
#			4) xScale                    #
#			5) xOffset                   #
#			6) xFormat                   #
#			7) numBins                   #
#			8) validPoints               #
#			8) yLabel                    #
#                                  #
####################################	

def configurePlotting(data):
	xUnit = data['xUnit']
	if xUnit == 'time':
		data.update(configureTime(data))
	elif xUnit == 'luminosity':
		data.update(configureLuminosity(data))
	elif xUnit == 'noBX':
		data.update(configureNoBX(data))
	return data

#====================================================================================================================
	
def configureTime(data):
	xScale = 1.0*10**9
	validPoints = len(data['time'])
	if validPoints:
		xOffset = min(data['time'].values())/xScale
		xMin = 0.
		xMax = max(data['time'].values())/xScale-xOffset
		maxBins = xMax/300
		minBins = xMax/1200
		if maxBins <= 160: 
			numBins = maxBins
		elif maxBins/2 <= 160: 
			numBins = maxBins/2
		elif maxBins/3 <= 160: 
			numBins = maxBins/3
		else: 
			numBins = minBins
		if xMax > 48*60*60: xFormat = "%d/%m"
		elif xMax > 12*60*60: xFormat = "%d-%Hh"
		else: xFormat = "%H:%M"
	else:
		xOffset = 0.
		xMin = 0.
		xMax = 1.
		numBins = 1.
		xFormat = "%d/%m"
	xLabel = time.strftime("%Z Time")
	if 'filter' in data:
		validPoints = len(commonKeys([data['time'],data['filter']]))
	data['plotConfig'] = {'xMin':xMin,'xMax':xMax,'xLabel':xLabel,'xScale':xScale,'xOffset':xOffset,'xFormat':xFormat,'numBins':numBins,'validPoints':validPoints,'yLabel':'Rate [Hz]'}
	return data

#====================================================================================================================
	
def configureLuminosity(data):
	if 'filter' in data:
		validPoints = len(commonKeys([data['luminosity'],data['filter']]))
		if not validPoints:
			xMin = 0
			xMax = 0
		else:
			xMin = min([data['luminosity'][runLb] for runLb in commonKeys([data['luminosity'],data['filter']])])
			xMax = max([data['luminosity'][runLb] for runLb in commonKeys([data['luminosity'],data['filter']])])
	else:
		validPoints = len(data['luminosity'])
		if not validPoints:
			xMin = 0
			xMax = 0
		else:
			xMin = min(data['luminosity'].values())
			xMax = max(data['luminosity'].values())
	if xMin and xMax:
		xScale=round(log10(xMax)-.5)
		xLabel = 'Inst. Luminosity [10^{'+str(int(30+xScale))+'} cm^{-2} s^{-1}]'
		xScale = float(10**xScale)
	else:
		xScale = 1.0
		xLabel = 'Inst. Luminosity [10^{'+str(int(xScale+30))+'} cm^{-2} s^{-1}]'
	xMin/=xScale
	xMax/=xScale
	xOffset = 0.0
	numBins = int(min(validPoints/3.,100))
	xFormat = None
	data['plotConfig'] = {'xMin':xMin,'xMax':xMax,'xLabel':xLabel,'xScale':xScale,'xOffset':xOffset,'xFormat':xFormat,'numBins':numBins,'validPoints':validPoints,'yLabel':'Rate [Hz]'}
	return data

#====================================================================================================================

def configureNoBX(data):
	if 'filter' in data:
		validPoints = len(commonKeys([data['noBX'],data['filter']]))
		if not validPoints:
			xMin = 0
			xMax = 0
		else:
			xMin = min([data['noBX'][runLb] for runLb in commonKeys([data['noBX'],data['filter']])])
			xMax = max([data['noBX'][runLb] for runLb in commonKeys([data['noBX'],data['filter']])])
	else:
		validPoints = len(data['noBX'])
		if not validPoints:
			xMin = 0
			xMax = 0
		else:
			xMin = min(data['noBX'].values())
			xMax = max(data['noBX'].values())
	xScale = 1.0
	xLabel = 'Average Interactions per Bunch Crossing'
	xOffset = 0.0
	numBins = int(min(validPoints/3.,100))
	xFormat = None
	data['plotConfig'] = {'xMin':xMin,'xMax':xMax,'xLabel':xLabel,'xScale':xScale,'xOffset':xOffset,'xFormat':xFormat,'numBins':numBins,'validPoints':validPoints,'yLabel':'Cross-section [outhouse]'}
	return data

#====================================================================================================================

def getNiceCanvas(name,aliases,textSize=25):
	canvas = TCanvas(name, name, 4, 45, 700, 500)
	canvas.SetGridx(1)
	canvas.SetGridy(1)
	canvas.SetFillColor(0)
	canvas.SetBorderMode(0)
	canvas.SetBorderSize(0)
	canvas.SetFrameFillColor(0)
	#Stretch width and add subpad for legend
	width = int(max([len(nm) for nm in aliases])*textSize*.50)+70
	oldWidth = canvas.GetWindowWidth()
	newWidth = oldWidth+width
	canvas.SetCanvasSize(newWidth,canvas.GetWindowHeight())
	canvas.Divide(2,1)
	subPad1 = canvas.GetPad(1)
	subPad1.SetPad(.005,.005,float(oldWidth)/newWidth,.995)
	subPad2 = canvas.GetPad(2)
	subPad2.SetPad(float(oldWidth)/newWidth,.005,.995,.995)
	SetOwnership(canvas, 0)
	return canvas

#====================================================================================================================

def configureCanvasLegend(canvas,aliases,textSize=25):
	height = 1.2*len(aliases)*textSize/canvas.GetWh()
	width = int(max([len(name) for name in aliases])*textSize*.50)+70
	canvas.SetLeftMargin(canvas.GetLeftMargin()*textSize/14.)
	canvas.SetBottomMargin(canvas.GetBottomMargin()*textSize/14.)
	legend = TLegend(0.00,1-canvas.GetTopMargin(),150./width,1-canvas.GetTopMargin()-height)
	legend.SetTextSize(textSize)
	legend.SetFillColor(0)
	legend.SetShadowColor(0)
	legend.SetBorderSize(0)
	SetOwnership(legend, 0)

	return canvas,legend

#====================================================================================================================

def plotRateProfiles(data,label=False,runNumber=None,fillNumber=None,log=True,minY=None,maxY=None,luminousBunches=None,drawOption='H'):
	
	TGaxis.SetMaxDigits(3)

	data['canvases']={}

	for groupNum,group in enumerate(data['groups']):

		if minY is None: minY = 0.
		if log: minY = max([10**-9,minY])
		if maxY is None: maxY = 10.**10

		currentMin = maxY
		currentMax = minY

		aliases = []
		for rateName,alias,level in group:
			aliases.append(alias)
			temp = [data['profiles'][alias].GetBinContent(i) for i in range(data['profiles'][alias].GetNbinsX()+1) if minY<data['profiles'][alias].GetBinContent(i)<maxY]
			if not len(temp): continue
			thisMin = min(temp)
			thisMax = max(temp)
			currentMin = min(currentMin,thisMin)
			currentMax = max(currentMax,thisMax)

		canvas = getNiceCanvas('canvas{0}'.format(('000'+str(groupNum))[-3:]),aliases)

		if currentMin>=currentMax:
			logger.warning('No data in group, {0} is empty'.format(canvas.GetName()))		
			data['canvases'].update({canvas.GetName():canvas})
			continue
			
		scale = 1.1
		if log: scale = 1.5
		thisMinY = currentMin/scale
		thisMaxY = currentMax*scale

		subPad1 = canvas.GetPad(1)
		if log: subPad1.SetLogy()

		canvas,legend = configureCanvasLegend(canvas,aliases)

		legendOption = 'L'
		if 'P' in drawOption: legendOption = 'p'
		
		canvas.cd(1)
		for i,(rateName,alias,level) in enumerate(group):

			profile = data['profiles'][alias]
			profile = configureProfile(profile,i,thisMinY,thisMaxY,drawOption,data,log)
			if drawOption:
				if not i: profile.Draw(drawOption)
				else: profile.Draw('{0} SAME'.format(drawOption))
			else:
				if not i: profile.Draw()
				else: profile.Draw('SAME')
			if log:
				yAxis = profile.GetYaxis()
				if int(log10(minY)) == int(log10(maxY)): yAxis.SetMoreLogLabels(True)
				else: yAxis.SetMoreLogLabels(False)
				yAxis.Draw()
			legend.AddEntry(profile,alias,legendOption)
			
		canvas.cd(2)
		legend.Draw()

		canvas.cd(1)
		if label:
			scale = 1.0*10**9
			timeStart = min(data['time'].values())/scale
			timeEnd = max(data['time'].values())/scale
			timeLabel = fetchTimeLabel(timeStart,timeEnd)
			#startLumi,endLumi = getLumiEndPoints(data)
			canvas = generateAtlasLabel(canvas,timeLabel,luminousBunches,runNumber=runNumber,fillNumber=fillNumber)
		data['canvases'].update({canvas.GetName():canvas})
		
	return data

#====================================================================================================================

def configureProfile(profile,i,minY,maxY,drawOption,data,log,textSize=25):
	profile.SetMarkerColor(MarkerColors[i])
	profile.SetMarkerStyle(MarkerStyles[i])
	profile.SetMarkerSize(MarkerSizes[i])
	profile.SetLineColor(MarkerColors[i])
	if drawOption!='P': profile.SetMarkerSize(0.0)
	if drawOption=='E3':
		#profile.SetFillStyle(3001)
		profile.SetFillColor(MarkerColors[i])
	if not i:
		profile.GetXaxis().SetLabelSize(textSize)
		profile.GetXaxis().SetTitleSize(textSize)
		profile.GetYaxis().SetLabelSize(textSize)
		profile.GetYaxis().SetTitleSize(textSize)
		profile.SetMinimum(minY)
		profile.SetMaximum(maxY)
		profile.GetXaxis().SetTitle(data['plotConfig']['xLabel'])
		profile.GetYaxis().SetTitle(data['plotConfig']['yLabel'])
	if data['xUnit'] == 'time':
		profile.GetXaxis().SetTimeDisplay(1)
		profile.GetXaxis().SetTimeFormat(data['plotConfig']['xFormat'])
		dh = TDatime()
		dh.Set(int(data['plotConfig']['xOffset']), False)
		profile.GetXaxis().SetTimeOffset(dh.Convert(False))
		profile.GetXaxis().SetNdivisions(507)
	else:
		profile.GetXaxis().SetTimeDisplay(0)
	return profile

#====================================================================================================================

def getLumiEndPoints(data):
	if 'filter' in data:
		validPoints = len(commonKeys([data['luminosity'],data['filter']]))
		if not validPoints:
			lumiStart = 0.
			lumiEnd = 0.
		else:
			lumiStart = data['luminosity'][min(commonKeys([data['luminosity'],data['filter']]))]
			lumiEnd = data['luminosity'][max(commonKeys([data['luminosity'],data['filter']]))]
	else:
		if not validPoints:
			lumiStart = 0.
			lumiEnd = 0.
		else:
			lumiStart = data['luminosity'][min(data['luminosity'].keys())]
			lumiEnd = data['luminosity'][max(data['luminosity'].keys())]
	return (lumiStart,lumiEnd)
	
#====================================================================================================================

def generateAtlasLabel(canvas,timeLabel,luminousBunches,runNumber = None,fillNumber = None):
	canvas.cd(1)
	
	xOffset = .30
	yOffset = .27
	
	AtlasLabel = []

	AtlasLabel.append(TLatex())
	AtlasLabel[-1].SetNDC()	
	AtlasLabel[-1].SetTextFont(42)	

	if runNumber is None: runNumber = []
	if fillNumber is None: fillNumber = []
	
	if len(runNumber)==1:
		AtlasLabel[-1].DrawLatex(xOffset+0.115,yOffset,'Run {0} {1}'.format(runNumber[0],timeLabel))
		AtlasLabel.append(TLatex())
		AtlasLabel[-1].SetNDC()
		AtlasLabel[-1].SetTextFont(72)
		AtlasLabel[-1].DrawLatex(xOffset,yOffset,"ATLAS")
	elif len(fillNumber)==1:
		AtlasLabel[-1].DrawLatex(xOffset,yOffset,'LHC Fill {0} {1}'.format(fillNumber[0],timeLabel))
	else:
			AtlasLabel[-1].DrawLatex(xOffset,yOffset,'Running {0}'.format(timeLabel))		

	AtlasLabel.append(TLatex())
	AtlasLabel[-1].SetNDC()
	AtlasLabel[-1].SetTextFont(72)
	AtlasLabel[-1].DrawLatex(xOffset,yOffset+.07,"ATLAS")

	AtlasLabel.append(TLatex())
	AtlasLabel[-1].SetNDC()	
	AtlasLabel[-1].SetTextFont(42)
	AtlasLabel[-1].DrawLatex(xOffset+0.115,yOffset+.07,'Trigger Operations')

	if luminousBunches is not None:
		AtlasLabel.append(TLatex())
		AtlasLabel[-1].SetNDC()	
		AtlasLabel[-1].SetTextFont(42)
		AtlasLabel[-1].DrawLatex(xOffset,yOffset-.07,'{0} Luminous Bunches'.format(luminousBunches))	
	
	return canvas

#====================================================================================================================

def fetchTimeLabel(timeStart,timeEnd):
	timeTupleStart = time.localtime(timeStart)
	timeTupleEnd = time.localtime(timeEnd)

	if timeTupleStart.tm_year != timeTupleEnd.tm_year:
		timeLabel = '{0} - {1}'.format(time.strftime('%b. %e %Y',timeTupleStart),time.strftime('%b. %e %Y',timeTupleEnd))
	else:
		if timeTupleStart.tm_mon != timeTupleEnd.tm_mon:
			timeLabel = '{0} - {1} {2}'.format(time.strftime('%b. %e',timeTupleStart),time.strftime('%b. %e',timeTupleEnd),timeTupleEnd.tm_year)	
		else:
			if timeTupleStart.tm_mday != timeTupleEnd.tm_mday:
				timeLabel = '{0}-{1} {2}'.format(time.strftime('%b. %e',timeTupleStart),time.strftime('%e',timeTupleEnd),timeTupleEnd.tm_year)	
			else:
				timeLabel = time.strftime('%b. %e %Y',timeTupleStart)					
	return timeLabel

#====================================================================================================================
"""
This marks the beginning of the script
"""
#====================================================================================================================


if __name__=='__main__':
	try:
		mySignal = Process(target = waitForReturn, args=(sys.stdin.fileno(),))
		mySignal.start()
	
		logger = logging.getLogger(__name__)
		if args.VERBOSE: logger.setLevel(logging.INFO)

		if args.RUNNUMBER is not None:
			logger.info('Getting runLb range for run(s) {0}'.format(args.RUNNUMBER))
			runLbRanges = runNumberToRunLbRange(args.RUNNUMBER)
			args.FILLNUMBER = None
			args.TIMERANGE = None
		elif args.FILLNUMBER is not None:
			logger.info('Getting runLb range for fill(s) {0}'.format(args.FILLNUMBER))	
			runLbRanges = fillNumberToRunLbRange(args.FILLNUMBER)
			args.RUNNUMBER = None
			args.TIMERANGE = None
		elif args.TIMERANGE is not None:
			logger.info('Getting runLb range for time range {0}'.format(args.TIMERANGE))	
			runLbRanges = timeRangeToRunLbRange(args.TIMERANGE)
			args.RUNNUMBER = None
			args.FILLNUMBER = None
		else:
			logger.critical('No range given for which to gather data')
			sys.exit(0)

		if not (args.XUNIT in ['time','luminosity','noBX']):
			logger.critical('Cannot plot against x-axis unit {0}'.format(args.XUNIT))
			sys.exit(0)

		logger.info('Creating time converter')
		timeConverter = createTimeConverter(runLbRanges)
		
		coolQueryHandler = COOLQueryHandler(mySignal,numProc=args.PROCESSES,verbose=args.VERBOSE)
		rateQueryHandler = RateQueryHandler(mySignal,numProc=args.PROCESSES,verbose=args.VERBOSE)

		logger.info('Creating rate name lookups')
		nameToChannel,nameToFolder = rateNameInfo(runLbRanges,mySignal,numProc=args.PROCESSES)
	
		if not mySignal.is_alive(): sys.exit(0)
		
		"""
		folderName										|	payloadName			|	timeBased	| Special 
		====================================================================================>
		/TRIGGER/LUMI/LBLB						|	StartTime				|	False			|	None
		/TRIGGER/LUMI/LBLB						|	EndTime					|	False			|	None
		/TRIGGER/LUMI/LBLESTONL				|	LBAvInstLumi		|	False			|	0
		/TDAQ/OLC/LHC/FILLPARAMS			|	LuminousBunches	|	True			|	None
		/TRIGGER/LUMI/LBLESTONL				|	LBAvEvtsPerBX		|	False			|	0
		/TDAQ/RunCtrl/DataTakingMode	|	ReadyForPhysics	|	False			|	None
		/LHC/DCS/FILLSTATE						|	StableBeams			|	True			|	None
		/LHC/DCS/FILLSTATE						|	FillNumber			|	True			|	None
		====================================================================================>
		"""

		coolQueryHandler.result['stable'] = {}
		coolQueryHandler.result['ready'] = {}
		coolQueryHandler.result['noBX'] = {}
		coolQueryHandler.result['luminosity'] = {}
		coolQueryHandler.result['time'] = {}
		coolQueryHandler.result['luminousBunches'] = {}

		if args.FILTER:
			for runLbStart,runLbEnd in runLbRanges:
				coolQueryHandler.addQueryBundle(QueryBundle(timeConverter[(runLbStart,runLbEnd)][runLbStart],timeConverter[(runLbStart,runLbEnd)][runLbEnd],[('StableBeams','stable',str)],'/LHC/DCS/FILLSTATE',timeBased=True,payloadRequirements={'StableBeams':'{0}==\'TRUE\''},timeConverter=timeConverter[(runLbStart,runLbEnd)]))
				coolQueryHandler.addQueryBundle(QueryBundle(runLbStart,runLbEnd,[('ReadyForPhysics','ready',int)],'/TDAQ/RunCtrl/DataTakingMode'))
			for rateName,alias,level in [('L1_RD0_FILLED','deadTime1','Output'),('L1_RD0_FILLED','deadTime2','AfterPrescale')]:
				for rateQueryBundle in generateRateQueryBundles(rateName,level,runLbRanges,nameToChannel,nameToFolder,alias=alias):
					rateQueryHandler.addQueryBundle(rateQueryBundle)
		if args.XUNIT == 'noBX':
			for runLbStart,runLbEnd in runLbRanges:
				coolQueryHandler.addQueryBundle(QueryBundle(runLbStart,runLbEnd,[('LBAvInstLumi','luminosity',float),('LBAvEvtsPerBX','noBX',float)],'/TRIGGER/LUMI/LBLESTONL',channelSelection = cool.ChannelSelection(0)))
		else:
			for runLbStart,runLbEnd in runLbRanges:
				coolQueryHandler.addQueryBundle(QueryBundle(runLbStart,runLbEnd,[('LBAvInstLumi','luminosity',float)],'/TRIGGER/LUMI/LBLESTONL',channelSelection = cool.ChannelSelection(0)))
		for runLbStart,runLbEnd in runLbRanges:
			coolQueryHandler.addQueryBundle(QueryBundle(runLbStart,runLbEnd,[('StartTime','time',int),('EndTime','timeEnd',int)],'/TRIGGER/LUMI/LBLB'))
			coolQueryHandler.addQueryBundle(QueryBundle(timeConverter[(runLbStart,runLbEnd)][runLbStart],timeConverter[(runLbStart,runLbEnd)][runLbEnd],[('LuminousBunches','luminousBunches',int)],'/TDAQ/OLC/LHC/FILLPARAMS',timeBased=True,timeConverter=timeConverter[(runLbStart,runLbEnd)]))

		logger.info('Retrieving general data')
		
		coolQueryHandler.beginQuery()
		
		rateQueryHandler.result['deadTime1']={}
		rateQueryHandler.result['deadTime2']={}	

		for group in parseRatesString(args.RATES):
			for rateName,alias,level in group:
				rateQueryHandler.result[alias]={}
				for rateQueryBundle in generateRateQueryBundles(rateName,level,runLbRanges,nameToChannel,nameToFolder,alias=alias):
					rateQueryHandler.addQueryBundle(rateQueryBundle)

		if not mySignal.is_alive(): sys.exit(0)

		logger.info('Retrieving rate data')
		rateQueryHandler.beginQuery()
		
		Data = {}
		Data.update(coolQueryHandler.result)
		Data['rates']={}
		Data['rates'].update(rateQueryHandler.result)
		
		Data['xUnit']=args.XUNIT

		Data['groups']=parseRatesString(args.RATES)

		if not mySignal.is_alive(): sys.exit(0)
		
		if args.FILTER:
			Data['lowDeadTime']={}
			for runLb in commonKeys([Data['rates']['deadTime1'],Data['rates']['deadTime2']]):
				rateOutput = Data['rates']['deadTime1'].get(runLb)
				rateAfterPrescale = Data['rates']['deadTime2'].get(runLb)
				if rateAfterPrescale == 0.:
					continue
				deadTime = 1.0-rateOutput/rateAfterPrescale
				if deadTime < 0.05:
					Data['lowDeadTime'][runLb] = 1
			Data['filter']={}
			logger.info('Creating filter')
			if args.LUMINOUSBUNCHES is not None:
				Data['filter'].update(dict((key,1) for key in commonKeys([Data['lowDeadTime'],Data['ready'],Data['stable'],Data['luminousBunches']]) if Data['ready'].get(key) == 1 and Data['stable'].get(key) == 'TRUE' and Data['luminousBunches'].get(key) == args.LUMINOUSBUNCHES))			
			else:
				Data['filter'].update(dict((key,1) for key in commonKeys([Data['lowDeadTime'],Data['ready'],Data['stable']])                         if Data['ready'].get(key) == 1 and Data['stable'].get(key) == 'TRUE'))					

		setAtlasStyle()
		gROOT.SetBatch(args.BATCH)
		logger.info('Configuring plots')
		if not mySignal.is_alive(): sys.exit(0)		
		Data.update(configurePlotting(Data))
		logger.info('Filling profiles')
		if not mySignal.is_alive(): sys.exit(0)		
		Data.update(fillRateProfiles(Data))
		logger.info('Plotting rates')
		if not mySignal.is_alive(): sys.exit(0)		
		
		drawOption = 'H'
		if args.ENVELOPE: drawOption = 'E3'
		if args.POINTS: drawOption = 'P'
		
		Data.update(plotRateProfiles(Data,label=args.LABEL,runNumber=args.RUNNUMBER,fillNumber=args.FILLNUMBER,luminousBunches=args.LUMINOUSBUNCHES,log=args.LINEAR,drawOption=drawOption))
		if mySignal.is_alive(): 
			mySignal.terminate()
			mySignal.join()
		if not args.BATCH:
			logger.info('Drawing canvases')
			for canvasName,canvas in Data['canvases'].items(): canvas.Draw()
			import code
			code.interact(local=globals())
		else:
			for canvasName,canvas in Data['canvases'].items(): canvas.Print('{0}.eps'.format(canvasName))
	except KeyboardInterrupt:
		logger.critical('Caught signal, exiting')
		if mySignal.is_alive(): 
			mySignal.terminate()
			mySignal.join()
