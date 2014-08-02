#
# HLTConsistencyChecker.py
#
# author: Tina Potter, tinapotter@gmail.com
# 21/Jan/2012
#
# Check the consistency of the cuts made 
# at HLT for egamma triggers
#
# Takes the name of a trigger chain
#
# TO RUN
# python HLTConsistencyChecker.py -t 'EF_e24vhi_medium1' -l WARNING
# or
# python HLTConsistencyChecker.py -t 'EF_e24vhi_medium1' -u 'EF_e22vhi_medium1' -l INFO
# python HLTConsistencyChecker.py -t 'L2_e24vhi_medium1' -u 'L2_e22vhi_medium1' -l INFO


from lxml import objectify
from optparse import OptionParser
import re, string, sys, fnmatch, types
import numpy as np
import logging
from prettytable import *

# ---------------------------------------------------------------------------
# return all the elements and their hypo algorithms for a given trigger
def getElementsAndHypos(trigchain): 
	# get the number of signature counters
	Trigger_sigcounter = rootXMLConfig.xpath("//CHAIN[@chain_name='{0}']/SIGNATURE_LIST/SIGNATURE/@signature_counter".format(trigchain))
	# get list of trigger elements 
	Trigger_Elements = getTriggerElements(trigchain,Trigger_sigcounter)
	# get list of hypo algs
	Trigger_HypoAlgs = getHypoAlgs(Trigger_Elements, rootXMLConfig)
	# print all cuts
	printAllCuts(Trigger_HypoAlgs)
	
	return (Trigger_Elements, Trigger_HypoAlgs)

# ---------------------------------------------------------------------------
# return all the trigger elements for a given trigger
def getTriggerElements(chainname,sigcounter):
	triggerElements = []
	for sig in sigcounter:
		triggerEle = rootXMLConfig.xpath("//CHAIN[@chain_name='{0}']/SIGNATURE_LIST/SIGNATURE[@logic='1' and @signature_counter='{1}']/TRIGGERELEMENT/@te_name".format(chainname,sig))
		triggerElements.append(triggerEle)
	
	return zip(*triggerElements)

# ---------------------------------------------------------------------------
# return all the hypo algorithms for a given trigger element and a given xml file (L2 or EF)
def getHypoAlgs(TrigElemList, ConfigFile):
	algolist = []
	for mobj in TrigElemList:
		elemagolist = []
		for elem in mobj:
			algonames = ConfigFile.xpath("//SEQUENCE[@output='{0}']/@algorithm".format(elem))
			algonames_array = [item.split('/')[-1]  for item in algonames[0].split()]
			for item in algonames_array:
				if "Hypo" in item:
					elemagolist.append(item)
		algolist.append(elemagolist)
	return algolist

# ---------------------------------------------------------------------------
# print tables of the cuts applied for each hypo algorithm. Taken from the xml files. Only done for DEBUG
def printAllCuts( HypoAlgList ):
	if getattr(logging, options.log.upper(), None)<=10 :	#print DEBUG
		print  "Printing all cuts for ", CleanUp(HypoAlgList)
	else:
		return
	alglist = [algo for sublist in HypoAlgList for algo in sublist]
	# loop over all the hypo algorithms given
	for algo in alglist:
		thisrootXML = rootXMLl2
		if( "EF" in algo ):
			thisrootXML = rootXMLef
		# get a list of all the parameters in the xml file
		parameterlist = thisrootXML.xpath("//component[@alias='{0}']/parameter/@name".format(algo))
		# start the table
		AllTable = PrettyTable( [algo, "Value" ])
		AllTable.align[algo] = "l"
		# loop over the parameters. Add to the table if relevent
		for param in parameterlist:
			if any(word in param for word in BoringCutsList):  # drop boring cuts
				continue			
			
			if( param!='egammaElectronCutIDToolName' ): # if standard parameter (not the IDtool)
				value = FetchValue(algo,param)
				if value==None or (not isinstance( value, types.ListType) and value.shape==(0,) ):
					continue
				if (not isinstance( value, types.ListType)) and value.shape!=() and (value[0]=='[]' or value[0]=='{  }'):
					continue
				AllTable.add_row( [param, CleanUp(value)])  # add parameter and values to table
				
			else: # if parameter is the IDtool - get all the sub-parameters and add them too
				toolIDname = getEFIDTool(algo)
				paramTool = thisrootXML.xpath("//component[@alias='ToolSvc.{0}']/parameter/@name".format(toolIDname))
				AllTable.add_row( [toolIDname, ""])
				for paramt in paramTool:
					if any(word in paramt for word in BoringCutsList):
						continue 
					value = FetchValue(algo,paramt)
					if valuet==None or valuet.shape==(0,):
						continue
					if valuet.shape!=() and (valuet[0]=='[]' or valuet[0]=='{  }'):
						continue
					AllTable.add_row( ["    "+paramt, CleanUp(valuet)]) # add parameter and values to table
		print AllTable
	return
			
# ---------------------------------------------------------------------------
# compare relevant cuts at L2 and EF for a given trigger
def compareAlgsL2EF(t1chain,L2alg, EFalg):
	if getattr(logging, options.log.upper(), None)<=20 :	#print INFO
		print "Doing L2 vs EF comparison of hypo algorithms ", L2alg, EFalg
	# only 3 cuts are common to both L2 and EF for egamma objects
	L2EFmap ={'HADETthr': ['CutHadLeakage','max'],
		  'CARCOREthr': ['CutReta37', 'min'],
		  'CAERATIOthr': ['CutDEmaxs1', 'min'],		}
	# setup table
	L2EFTable = PrettyTable( [t1chain+" compare", L2alg+" and "+EFalg])
	L2EFTable.align[t1chain+" compare"] = "l"
	# loop over the three common cuts
	for key, value in L2EFmap.items(): 
		# find the value at L2 and EF for the given algorithms and cut
		valuel2 = getL2value(L2alg,key)
		valueef = getEFvalue(EFalg,value[0])
		if valuel2==None or valueef==None:
			continue
		L2EFTable.add_row( ["L2: "+key+" ("+value[1]+")", CleanUp(valuel2)]) # add entries to table
		L2EFTable.add_row( ["EF: "+value[0]+" ("+value[1]+")", CleanUp(valueef)])
		L2EFTable.add_row( ["",""])
		if CompareTwoArrays( valuel2, valueef, value[1] ):  # check if L2 is tighter than EF
			continue
		else:
			if getattr(logging, options.log.upper(), None)<=40 :	#print ERROR
				print "\n ************************************************************************************************************************************"
				print "**** ERROR *** L2 tighter than EF ({0} threshold) *** Trigger: {1}".format(value[1], t1chain)
				print "**************  L2 {0}:  {1}".format(L2alg, valuel2)
				print "**************  EF {0}:  {1}".format(EFalg, valueef)
				print "************************************************************************************************************************************ \n" 
	
	if getattr(logging, options.log.upper(), None)<=20 and getattr(L2EFTable, "rowcount")>0:  #print L2 vs EF table for INFO
		print L2EFTable
	return

# ---------------------------------------------------------------------------
# return the value of a parameter for a given algorithm		    
def FetchValue(algo,param):
	if( "EF" in algo ):
		value = getEFvalue(algo,param)
	else:
		value = getL2value(algo,param)
	return value

# ---------------------------------------------------------------------------
# return the value of a parameter for a given L2 algorithm		    
def getL2value(alg,varname):
	value = rootXMLl2.xpath("//component[@alias='{0}']/parameter[@name='{1}']/@value".format(alg,varname))
	if( value==[] ):
		#logging.debug('            No L2 value for {0} in {1}'.format(varname,alg))
		return None
	valuearray = np.array(eval(value[0])) # make value list into array
	if valuearray.shape==() or valuearray.shape==(0,):
		return valuearray
	valuearray = np.insert(valuearray, 0, valuearray[0]) # duplicate the 1st bin for L2 so we can compare same bins
	
	return valuearray

# ---------------------------------------------------------------------------
# return the value of a parameter for a given EF algorithm		    
def getEFvalue(alg,varname):
	toolIDname = getEFIDTool(alg) # get the IDtool name for the algorithm
	# get the parameter value - either from the IDtool or from the non-IDtool parameters
	value = rootXMLef.xpath("//component[@alias='ToolSvc.{0}']/parameter[@name='{1}']/@value".format(toolIDname,varname))
	if toolIDname==None or value==None or value==['[]'] or value==[] :
		value = rootXMLef.xpath("//component[@alias='{0}']/parameter[@name='{1}']/@value".format(alg,varname))
	if value==None or value==['[]'] or value==[] :
		return None
	# get the etbins and etcut used
	etbins = rootXMLef.xpath("//component[@alias='ToolSvc.{0}']/parameter[@name='CutBinET']/@value".format(toolIDname))
	etcut = getEtcutInHypo(alg)
	if etcut==[] or etcut==['[]']:
		return value
	etcut = np.array(eval(etcut))
	# grab relevant row from the array based on the etcut and etbins
	valuearrayEtrow=getEtRow(value[0], etbins[0], etcut)
		  
	return (valuearrayEtrow)

# ---------------------------------------------------------------------------
# return the relevant row from the value array based on the etcut and etbins
def getEtRow(value, etbins, etcut):
	if not isinstance( value, types.ListType): # make sure we are looking at an array
		value = eval(value)
	valuearray = np.array(value)
	if valuearray.size < 110:
		return (valuearray)
	valuearray = valuearray.reshape(11,10)   # shape into 2d array
	if not isinstance( etbins, types.ListType):
		etbins = eval(etbins)
	etbinsarray = np.array(etbins)
	etbinsarray = np.insert(etbinsarray, 0, 0)  # insert a 0 for the 1st bin 
	# pick out the et row needed based on the etcut	
	valuearrayEtrow=None
	for i in range(len(valuearray)):
		if( etcut > etbinsarray[i] ):   # make sure we only look at the relevant ET bin
			valuearrayEtrow = valuearray[i]
	return (valuearrayEtrow)

	
# ---------------------------------------------------------------------------
# return the name of the IDtool for a given EF algorithm
def getEFIDTool(alg):
	tool = rootXMLef.xpath("//component[@alias='{0}']/parameter[@name='egammaElectronCutIDToolName']/@value".format(alg))
	if( tool==[] ):
		return None
	toolname = tool[0].split("/")
	
	return toolname[1]

# ---------------------------------------------------------------------------
# compare any two algorithms at EF:EF level or L2:L2 level
def compareAlgs(alg1, alg2):
	if getattr(logging, options.log.upper(), None)<=20 :	#print INFO
		print "Doing same level comparison of hypo algorithms ", alg1, alg2
		
	thisrootXML = rootXMLl2
	if( "EF" in alg1 ):
		thisrootXML = rootXMLef
	# get all the parameters in the first algorithm
	parameterlist = thisrootXML.xpath("//component[@alias='{0}']/parameter/@name".format(alg1))
	# setup table
	SameLevelTable = PrettyTable( ["Different Cuts", alg1, alg2])
	SameLevelTable.align["Different Cuts"] = "l"
	# loop over all the paramters
	for param in parameterlist:
		if( any(word in param for word in BoringCutsList) ): # ignore boring cuts
			continue
		# grab values of the paramter for each algorithm
		value1 = thisrootXML.xpath("//component[@alias='{0}']/parameter[@name='{1}']/@value".format(alg1,param))
		value2 = thisrootXML.xpath("//component[@alias='{0}']/parameter[@name='{1}']/@value".format(alg2,param))
		if len(set(value1) & set(value2)) ==0 or value1==value2:
		       continue
		SameLevelTable.add_row( [param, value1, value1])    # add parameter and values to table
	if getattr(logging, options.log.upper(), None)<=40 and getattr(SameLevelTable, "rowcount")>0:  #print ERROR
		print SameLevelTable  # print table
	return

# ---------------------------------------------------------------------------
# check the Et cut specified in the trigger name is consistent with the Et cut applied
def etCutConsistency(trigchain, algs):
	# get the Et values as specified in the trigger name
	triggerEt = getEtvalues(trigchain)
	# loop over each algorithm used in the trigger and check the Et cut applied
	for i, obj in enumerate(algs):
		for alg in obj:
			consistentEt=True
			Etcut = getEtcutInHypo(alg)
			if Etcut==[]:
				continue
			if( "EF" in alg ):
				if( float(triggerEt[i])*1000!=float(Etcut) ):
					consistentEt=False
			else: #L2
				valuearray = np.array(eval(Etcut))
				for val in valuearray:
					if( float(triggerEt[i])*1000<val): # for L2, cuts must be within 3 GeV of trigger Et
						consistentEt=False
			if( consistentEt==False ):
				if getattr(logging, options.log.upper(), None)<=40 :	#print ERROR
					print "\n************************************************************************************************************************************"
					print "**** ERROR *** Et cut not consistent *** Trigger: {0},  {1} Et cut made in {2} ".format(trigchain, Etcut, alg)
					print "************************************************************************************************************************************\n"
	return

# ---------------------------------------------------------------------------
# return the Et cut applied in an alorithm
def getEtcutInHypo(alg):
	if( "EF" in alg ):
		Etcut = rootXMLef.xpath("//component[@alias='{0}']/parameter[@name='emEt']/@value".format(alg))
	else:
		Etcut = rootXMLl2.xpath("//component[@alias='{0}']/parameter[@name='ETthr']/@value".format(alg))
	if Etcut==[] or Etcut==['False']:
		return []
	else:
		return Etcut[0]
	return None
# ---------------------------------------------------------------------------
# return the Et cuts as specified in the trigger name
def getEtvalues(trigchain):
	trigchainspl = trigchain.split("_")
	triggerEt = []
	for item in trigchainspl:
		etcutitem = []
		if( item[0]=='e' or item[0]=='g' or ((item[1]=='e' or item[1]=='g') and item[0].isdigit() ) ):
			etcutitem = (item[:0] + item[(0+1):]).translate(None, string.letters)
			triggerEt.append(etcutitem)
	return triggerEt


# ---------------------------------------------------------------------------
# return a list of triggers matching the wildcard given in the user input
def DealWithWildcards():
	global wildmatches
	wildmatches = [options.t1chain]
	# get list of matching triggers in HLTchains
	if ("*" in options.t1chain ):
		wildmatches = [ch for ch in HLTchains if fnmatch.fnmatch(ch, options.t1chain)]
	return

# ---------------------------------------------------------------------------
# compare the cuts in hypo algorithms from two given triggers
def CompareHypos(wildchain, Trigger1_HypoAlgs, Trigger2_HypoAlgs):
	if getattr(logging, options.log.upper(), None)<=20 :	#print INFO
		print "Comparing cut values in Hypo algs"
	for i, obj in enumerate(Trigger1_HypoAlgs):
		# compare the values of cuts used for each Hypo alg
		for alg1 in obj:
			for alg2 in Trigger2_HypoAlgs[i]:
				# don't compare an algorithm with itself
				if alg1==alg2:
					if getattr(logging, options.log.upper(), None)<=20 :	#print INFO
						print "Identical Hypo Algos:  {0},  {1} -- moving on".format(alg1, alg2)
					continue
				# make comparisons depending on the level of each algorithm considered
				if( "EF" in alg1 and "EF" in alg2 ):
					compareAlgs(alg1,alg2)
				elif( "EF" not in alg1 and "EF" in alg2 ):
					compareAlgsL2EF(wildchain,alg1,alg2)
				elif( "EF" in alg1 and "EF" not in alg2 ):
					compareAlgsL2EF(wildchain,alg2,alg1)
				elif( "EF" not in alg1 and "EF" not in alg2 ):
					compareAlgs(alg1,alg2)
	return

# ---------------------------------------------------------------------------
# return the L2 trigger of a given EF trigger
def FindL2Trigger(wildchain):
	global secondtrigger
	# only return if a second trigger is not specified already
	if (options.t2chain==None and options.compare==False and "EF" in wildchain):
		# find the L2 trigger that seeded the EF
		triggerEF_lowerchainname = rootXMLConfig.xpath("//CHAIN[@chain_name='{0}']/@lower_chain_name".format(wildchain))
		secondtrigger = [triggerEF_lowerchainname[0]]
	return

# ---------------------------------------------------------------------------
# return similar electron (photon) triggers for a given photon (electron) trigger
def FindSimilarElePhoChain(wildchain):
	global secondtrigger
	if (options.compare==True):
		# get the Et value of the trigger
		triggerEt = getEtvalues(wildchain)
		# get similar chains, with similar Et
		if( IsElectronTrigger(wildchain) ):
			secondtrigger = GetMatchingChains(wildchain, "electron",  triggerEt[0])
		if( IsPhotonTrigger(wildchain) ):
			secondtrigger = GetMatchingChains(wildchain, "photon",  triggerEt[0])
		print " Similar chains found ", secondtrigger
	return	

# ---------------------------------------------------------------------------
# return chains in the same Et bin
def GetMatchingChains(triggername, chaintype, etcut):
	Etbins = [0, 10, 20, 30, 40, 80, 999]
	matchingchains=[]
	# find the corresponding photon/ele triggers in range 0--10, 10--20, 20--30, 30--40, 40--80, 80+
	for i, et in enumerate(Etbins):
		if( float(etcut) > float(Etbins[i]) and float(etcut) < float(Etbins[i+1]) ):
			if( chaintype=="electron" ):
				chainmatches = [ch for ch in HLTchains if IsPhotonTrigger(ch)]
			if( chaintype=="photon" ):
				chainmatches = [ch for ch in HLTchains if IsElectronTrigger(ch)]
			for chainmatch in chainmatches:
				matchEt = getEtvalues(chainmatch)
				if( float(matchEt[0]) > float(Etbins[i]) and float(matchEt[0]) < float(Etbins[i+1]) ):
					if( ("EF_" in triggername and "EF_" in chainmatch) or ("L2_" in triggername and "L2_" in chainmatch) ):
						    matchingchains.append(chainmatch)
	return matchingchains

# ---------------------------------------------------------------------------
# return whether electron trigger based on the trigger name
def IsElectronTrigger(triggerchain):
	trigchainspl = triggerchain.split("_")
	for item in trigchainspl:
		if( item=='etcut' ):
			continue
		if( item[0]=='e' ):
			return True
		if( len(item)>1 and item[1]=='e'):
			return True
	return False

# ---------------------------------------------------------------------------
# return whether photon trigger based on the trigger name
def IsPhotonTrigger(triggerchain):
	trigchainspl = triggerchain.split("_")
	for item in trigchainspl:
		if( item[0]=='g' and "g_nocut" not in triggerchain):
			return True
		if( len(item)>1 and item[1]=='g' and "g_nocut" not in triggerchain):
			return True
	return False


# ---------------------------------------------------------------------------
# compare the EF online trigger to the offline reco
def CompareToOffline( trigger, alg ):
	if "EF" not in alg: # only do this for EF triggers
		return
	toolIDname = getEFIDTool( alg )
	if( toolIDname==None ):
		return
	# import offline module
	if "loose" in alg:
		import egammaPIDTools.egammaLooseElectronCutIDToolBase as OfflineMod
		offline_cuts = OfflineMod.egammaLooseElectronCutIDToolBase()
	elif "medium" in alg:
		import egammaPIDTools.egammaMediumElectronCutIDToolBase as OfflineMod
		offline_cuts = OfflineMod.egammaMediumElectronCutIDToolBase()
	elif "tight" in alg:
		import egammaPIDTools.egammaTightElectronCutIDToolBase as OfflineMod
		offline_cuts = OfflineMod.egammaTightElectronCutIDToolBase()
	else:
		import egammaPIDTools.egammaElectronCutIDToolBase as OfflineMod
		offline_cuts = OfflineMod.egammaElectronCutIDToolBase()
	offline_cuts.LowLumi_2012() # use the LowLumi_2012  menu
	
	# get all the parameters from the online xml
	paramTool_on = rootXMLef.xpath("//component[@alias='ToolSvc.{0}']/parameter/@name".format(toolIDname))
	# get et cut and etbins
	Etcut = getEtcutInHypo(alg)
	Etbins = getattr(offline_cuts, "CutBinET")
	
	# loop over each parameter and compare
	for key, value in EFattrmap.items(): 
		if hasattr(offline_cuts, key):
			value_off = getattr(offline_cuts, key)
			value_onl = rootXMLef.xpath("//component[@alias='ToolSvc.{0}']/parameter[@name='{1}']/@value".format(toolIDname,key))
			value_on = value_onl[0]
			if( value_on=='[]' or value_on=='{  }' ):
				continue
			RowOn = getEtRow(value_on, Etbins, Etcut) 
			RowOff = getEtRow(value_off, Etbins, Etcut)
			# check if online is tighter than offline
			if CompareTwoArrays( RowOn, RowOff, value ):  
				continue
			elif getattr(logging, options.log.upper(), None)<=40 :	#print ERROR
				print "\n ************************************************************************************************************************************"
				print "**** ERROR *** Online tighter than Offline ({0} threshold) *** Trigger: {1}".format(value, trigger)
				print "**************  Online  {0}".format(RowOn)
				print "**************  Offline {0}".format(RowOff)
				print "************************************************************************************************************************************ \n" 
	return

# ---------------------------------------------------------------------------
# return the agreement between two arrays with a given min or max threshold
def CompareTwoArrays(array1, array2, thresh):
	# check if array1 is looser than array2
	if np.array_equal(array1,array2):
		return True
	for i in range(len(array1)):  # loop over the eta bins
		if array1[i]==array2[i]:  # ignore cases where the value is equal
			continue
		# compare values and print error where array1 is tighter
		if( array1[i] == -999 or array2[i] == -999 ):
			continue
		if (thresh=='max' and array1[i]<array2[i]) or (thresh=='min' and array1[i]>array2[i]):
			#print thresh, array1[i], array2[i]
			return False
	return True

# ---------------------------------------------------------------------------
# do an EF online vs reco offline comparison for a given trigger and its hypo algorithms
def OfflineComparison(trigger, hypos):
	if options.offline==False:
		return
	for obj in hypos:
		# compare the values of cuts used for each Hypo alg
		for alg in obj:
			CompareToOffline(trigger, alg)
	return
	
# ---------------------------------------------------------------------------
# cleanup a list/array to be used in the tables
def CleanUp( bar ):
	return str(bar).replace("[","").replace("]","").replace("(","").replace(")","").replace("'","")



# ---------------------------------------------------------------------------
# General setup
def Setup():
	# global variables
	global rootXMLConfig, rootXMLef, rootXMLl2, HLTchains, L2EFmap, EFattrmap, BoringCutsList
	# logging
	numeric_level = getattr(logging, options.log.upper(), None)
	if not isinstance(numeric_level, int):
		raise ValueError('Invalid log level: %s' % options.log)
	logging.basicConfig(level=numeric_level)
	# width and precision of output
	np.set_printoptions(precision=4,suppress=True, linewidth=110)
	
	# open the trigger config file and  make it into an object
	with open(options.HLTconfig) as F:
		rootXMLConfig = objectify.parse(F)
	# open the two trigger files (ef and l2) and make them into objects
	with open(options.EFsetup) as f:
		rootXMLef = objectify.parse(f)
	with open(options.L2setup) as g:
		rootXMLl2 = objectify.parse(g)
	
	# Get list of all HLT chains
	HLTchains = rootXMLConfig.xpath("//CHAIN/@chain_name")

	# Check if given trigger exists
	if ("*" not in options.t1chain  and  options.t1chain not in HLTchains  ):
		if getattr(logging, options.log.upper(), None)<=40 :	#print ERROR
			print "\n************************************************************************************************************************************"
			print "**** ERROR *** Trigger not found: {0}".format(options.t1chain)
			print "************************************************************************************************************************************\n"
		sys.exit()
	
	# make variables maps
	# L2 vs EF - only 3 common variables
	L2EFmap ={'HADETthr': ['CutHadLeakage','max'],
		  'CARCOREthr': ['CutReta37', 'min'],
		  'CAERATIOthr': ['CutDEmaxs1', 'min'],		}
	# EF vs offline variables with min/max thresholds
	EFattrmap ={
		'CutA0': 'max',
		'CutA0Tight': 'max',
		'CutBL': 'min',
		'CutClusterIsolation': 'max',
		'CutDEmaxs1': 'min',
		'CutDeltaE': 'max',
		'CutDeltaEmax2': 'max',
		'CutDeltaEta': 'max',
		'CutDeltaEtaTight': 'max',
		'CutF1': 'max',
		'CutF3': 'max',
		'CutFracm': 'max',
		'CutHadLeakage': 'max',
		'CutNumTRT': 'min',
		'CutPi': 'min',
		'CutReta37': 'min',
		'CutSi': 'min',
		'CutTRTRatio': 'min',
		'CutTRTRatio90': 'min',
		'CutWeta1c': 'max',
		'CutWeta2c': 'max',
		'CutWtot': 'max',
		'CutmaxDeltaPhi': 'max',
		'CutmaxEp': 'max',
		'CutminDeltaPhi': 'min',
		'CutminEp': 'min',		}
	# Boring cuts to ignore in the tables printed out
	BoringCutsList =['Monitor', 'True', 'False', 'StoreGate', 'AcceptAll', 'ApplyIsEM', 'ApplyIsolation', 'AthenaMonTools', 'AuditAlgorithms', 'AuditBeginRun', 'AuditEndRun', 'AuditExecute', 'AuditFinalize', 'AuditInitialize', 'AuditReinitialize', 'AuditRestart', 'AuditStart', 'AuditStop', 'AuditTools', 'CaloCutsOnly', 'EMConversionContainerName', 'DetStore','EMShowerContainerName', 'EMTrackMatchContainerName', 'EvtStore', 'LumiLevel', 'Enable', 'ErrorCodeMap', 'ErrorCount', 'ErrorMax', 'MonitorService', 'OutputLevel', 'RegisterForContextService', 'doMonitoring', 'doOperationalInfo', 'doTiming', 'forceAccept', 'histoPath', 'PIDName', 'trackToVertexTool', 'useClusETforCaloIso', 'useClusETforTrackIso']

	return



#======== MAIN PROGRAM HERE ============================
#------------------------------- options -------------------------------
parser = OptionParser()
version = "v0.0"
parser = OptionParser(usage="%prog [--t1chain] [--t2chain] [--log]", version="%prog " + version)
parser.add_option("-t", "--t1chain", dest="t1chain", type="string", help="trigger 1 chain name")
parser.add_option("-u", "--t2chain", dest="t2chain", type="string", help="trigger 2 chain name")
parser.add_option("-l", "--log", dest="log", type="string", help="logging level")
parser.add_option("-c", "--compare", action="store_true", dest="compare", help="compare photon/electron chains")
parser.add_option("-o", "--offline", action="store_true", dest="offline", help="compare EF to offline cuts")
parser.add_option("-x", "--HLTconfig", dest="HLTconfig", type="string", help="HLT config xml file")
parser.add_option("-y", "--L2setup", dest="L2setup", type="string", help="L2 setup xml file")
parser.add_option("-z", "--EFsetup", dest="EFsetup", type="string", help="EF setup xml file")
(options, args) = parser.parse_args()
# ----- check we have the input required
if (options.t1chain==None):
	print "ERROR: Please supply a first chain name e.g. -t1 EF_e24vhi_medium1 "
	sys.exit(1)
if (options.t2chain==None and options.compare==None):
	print "\n Only one chain specified, assuming EF:L2 comparison \n"
if (options.log==None):
	options.log='ERROR'
if (options.compare==None):
	options.compare=False
if (options.compare==True):
	print "\n Cross-chain comparison specified: electron vs photon chain comparison will be done \n"	
if (options.offline==None):
	options.offline=False
if (options.offline==True):
	print "EF vs Offline comparison specified: comparison will be done \n"	
if (options.HLTconfig==None):
	options.HLTconfig='AllPT_physicsV4_menu/outputHLTconfig_PT-1.xml'
if (options.L2setup==None):
	options.L2setup='AllMT_physicsV4_menu/l2_Default_setup.xml'
if (options.EFsetup==None):
	options.EFsetup='AllPT_physicsV4_menu/ef_Default_setup.xml'
#-----------------------------------------------------------------------
#

Setup()

DealWithWildcards()

# Loop over all the matches to the wildcard (if any)
for wildchain in wildmatches:
	
	secondtrigger = [options.t2chain]
	# return the L2 trigger as secondtrigger if no 2nd trigger is specified
	FindL2Trigger(wildchain) 
	# returns list of similar triggers in ele/pho chains as secondtrigger if -c is requested
	FindSimilarElePhoChain(wildchain) 
	
	for trigger2 in secondtrigger:
		
		# get trigger elements and hypo algorithms
		(Trigger1_Elements, Trigger1_HypoAlgs) = getElementsAndHypos(wildchain)
		(Trigger2_Elements, Trigger2_HypoAlgs) = getElementsAndHypos(trigger2)
		# general table of triggers, elements and algorithms being compared
		if getattr(logging, options.log.upper(), None)<=20 :
			TrigEleHyposTable = PrettyTable( ["Triggers Compared", "Elements", "Hypo Algorithms"])
			TrigEleHyposTable.align["Trigger"] = "l"
			TrigEleHyposTable.add_row( [wildchain, CleanUp(Trigger1_Elements), CleanUp(Trigger1_HypoAlgs)] )
			TrigEleHyposTable.add_row( [trigger2,  CleanUp(Trigger2_Elements), CleanUp(Trigger2_HypoAlgs)] )
			print "\n \n \n"
			print TrigEleHyposTable
		
		# check consistency of Et cuts made in the algs compared to trigger name
		etCutConsistency(wildchain, Trigger1_HypoAlgs)
		etCutConsistency(trigger2, Trigger2_HypoAlgs)

		# compare hypo algorithms (general print out or L2 vs EF)
		CompareHypos(wildchain, Trigger1_HypoAlgs, Trigger2_HypoAlgs)

		# if EF trigger, compare to offline cuts
		OfflineComparison(wildchain, Trigger1_HypoAlgs)
		OfflineComparison(trigger2, Trigger2_HypoAlgs)
