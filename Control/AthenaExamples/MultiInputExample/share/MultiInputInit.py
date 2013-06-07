#=======================================================================
# File:   MultiInputInit.py
#=======================================================================

class inputObject:
	pass;
	#Should have inputCollections, objectName 


inputObjectList=[]; #List of inputObjcet

if not (theApp.EventLoop=="PileUpEventLoopMgr") :
    raise "Please use the flag -p Digitization/pileUpBootstrap.py"

from PileUpComps.PileUpCompsConf import PileUpEventLoopMgr

if not hasattr(ServiceMgr, 'EventSelector'):
    import AthenaPoolCnvSvc.ReadAthenaPool;

from PileUpComps.PileUpCompsConf import BkgStreamsCache;
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool;

def initMultiInput():
	global inputObjectList, pileUpEventLoopMgr, ServiceMgr, SkipEvents

	i=0;
	for iObj in inputObjectList:
		if i==0: #The Primary stream in PileUp
			#Selector of the first one has to be this name?
			pileUpEventLoopMgr.OrigSelector="EventSelector"
			ServiceMgr.EventSelector.InputCollections = iObj.inputCollections;
			pileUpEventLoopMgr.firstXing=0;
			pileUpEventLoopMgr.lastXing=0;
			ServiceMgr.EventSelector.SkipEvents = SkipEvents
		else:
			pileUpEventLoopMgr.bkgCaches += [BkgStreamsCache(iObj.objectName+"_Cache")];
			tmpCache=pileUpEventLoopMgr.bkgCaches[len(pileUpEventLoopMgr.bkgCaches)-1];
			tmpCache.CollPerXing=1;
			tmpCache.CollDistribution="Fixed";
			tmpCache.ReadDownscaleFactor=1;

			tmpEvtSel=EventSelectorAthenaPool(iObj.objectName+"_EventSelector");
			tmpEvtSel.InputCollections = iObj.inputCollections;
			tmpEvtSel.KeepInputFilesOpen = True;
			tmpEvtSel.SkipEvents = SkipEvents

			ServiceMgr += tmpEvtSel;
			tmpCache.EventSelector=iObj.objectName+"_EventSelector";

			
			
		print iObj.objectName;
		print iObj.inputCollections;
		i=i+1;
		
