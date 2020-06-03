# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging
from AthenaCommon.Include import include
from AthenaCommon.GlobalFlags import globalflags
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon.Include import include

from RecExConfig.Configured import Configured

from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
from TriggerJobOpts.TriggerFlags import TriggerFlags
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.RecFlags import rec

from TrigRoiConversion.TrigRoiConversionConf import RoiWriter


def  EDMDecodingVersion():

    log = logging.getLogger("EDMDecodingVersion.py")

    # change version only if not rerunning the trigger    
    TriggerFlags.EDMDecodingVersion = 2

    # run the AutoConfiguration
    from RecExConfig.AutoConfiguration import ConfigureInputType
    #ConfigureInputType()
    from RecExConfig.InputFilePeeker import inputFileSummary
    
    if globalflags.InputFormat.is_bytestream():
        # BYTESTREAM: decide Run1/Run2 based on Run number
        from RecExConfig.AutoConfiguration  import GetRunNumber
        runNumber = GetRunNumber()

        #Run1 data
        if runNumber > 0 and runNumber < 230000 :            
            TriggerFlags.EDMDecodingVersion = 1
            TriggerFlags.doMergedHLTResult = False
            log.info("decoding version set to 1, because running on BS file from Run1")
            pass
    else:
        #Pool files
        from RecExConfig.ObjKeyStore import cfgKeyStore
        ItemDic=inputFileSummary.get("eventdata_itemsDic")
        ItemList=inputFileSummary.get('eventdata_itemsList')

        if cfgKeyStore.isInInputFile( "HLTResult", "HLTResult_EF" ):          
            TriggerFlags.EDMDecodingVersion = 1
            TriggerFlags.doMergedHLTResult = False
            log.info("Decoding version set to 1, because HLTResult_EF found in pool file")
        elif cfgKeyStore.isInInputFile( "HLTResult", "HLTResult_HLT"):          
            TriggerFlags.EDMDecodingVersion = 2
            TriggerFlags.doMergedHLTResult = True
        else:
            log.warning("No HLTResult found in pool file")
        pass
    pass
                




    ## if not (rec.readESD() or rec.readAOD()):
    ##     log.info("FPP Found rec.RunNumber=%s and autoConfiguration.RunNumber=%s"# and svcMgr.EventSelector.RunNumber=%s"
    ##              %(str(rec.RunNumber()),str(GetRunNumber())))#, str(svcMgr.EventSelector.RunNumber())))
            
         
                
        ## inputIsSimulation = False
        ## if inputFileSummary.has_key("evt_type"):
        ##     eventTypeList = inputFileSummary.get("evt_type")
        ##     if eventTypeList.__contains__("IS_SIMULATION") :
        ##         print "FPP Detected that the input file is a simulated dataset"
        ##         inputIsSimulation = True
        ##         pass
        ##     pass

            #from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc

            #    from AthenaCommon.GlobalFlags  import globalflags
            ## from RecExConfig.AutoConfiguration  import GetProjectName
            ## print "FPP projectName=%s"%str(GetProjectName())
        # Run1 data  

    # ESD/AOD files:
    

    log.info("EDMDecoding set to %s"%TriggerFlags.EDMDecodingVersion )
  
    return  True
        


class xAODConversionGetter(Configured):
    def configure(self):
        log = logging.getLogger('xAODConversionGetter.py')
        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()

        #schedule xAOD conversions here
        from TrigBSExtraction.TrigBSExtractionConf import TrigHLTtoxAODConversion
        xaodconverter = TrigHLTtoxAODConversion()
        
        from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
        xaodconverter.Navigation = HLTNavigationOffline()

        from TrigEDMConfig.TriggerEDM import getPreregistrationList,getL2PreregistrationList,getEFPreregistrationList
        from TrigEDMConfig.TriggerEDM import getEFRun1BSList,getEFRun2EquivalentList,getL2Run1BSList,getL2Run2EquivalentList#,getHLTPreregistrationList
        xaodconverter.Navigation.ClassesToPreregister = getPreregistrationList(TriggerFlags.EDMDecodingVersion())
        ## if TriggerFlags.EDMDecodingVersion() == 2:
        ##     #        if TriggerFlags.doMergedHLTResult():
        ##     #if EDMDecodingVersion() =='Run2': #FPP
        ##     xaodconverter.Navigation.ClassesToPreregister = getHLTPreregistrationList()
        ## else:
        ##     xaodconverter.Navigation.ClassesToPreregister = list(set(getL2PreregistrationList()+getEFPreregistrationList()+getHLTPreregistrationList()))

        #we attempt to convert the entire old navigation (L2+EF)
        #xaodconverter.BStoxAOD.ContainersToConvert = list(set(getL2PreregistrationList()+getEFPreregistrationList()))
        # we want only containers from Run 1 with the BS tag
        xaodconverter.BStoxAOD.ContainersToConvert = getL2Run1BSList() + getEFRun1BSList()
        xaodconverter.BStoxAOD.NewContainers = getL2Run2EquivalentList() + getEFRun2EquivalentList()

        xaodconverter.HLTResultKey="HLTResult_EF"
        topSequence += xaodconverter

        # define list of HLT xAOD containers to be written to the output root file
        # (previously this was defined in HLTTriggerResultGetter def configure)
        from TrigEDMConfig.TriggerEDM import getTriggerEDMList
        self.xaodlist = {}
        self.xaodlist.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(), 2 ))

        return True
    
        

class ByteStreamUnpackGetter(Configured):
    def configure(self):

        log = logging.getLogger("ByteStreamUnpackGetter.py")
        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()
        
        #if TriggerFlags.readBS():
        log.info( "TriggerFlags.dataTakingConditions: %s" % TriggerFlags.dataTakingConditions() )
        # in MC this is always FullTrigger
        hasHLT = TriggerFlags.dataTakingConditions()=='HltOnly' or TriggerFlags.dataTakingConditions()=='FullTrigger'
        
        if hasHLT:
            from AthenaCommon.AppMgr import ServiceMgr
            # Decide based on the run number whether to assume a merged, or a
            # split HLT:
            if not TriggerFlags.doMergedHLTResult():
                ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
                    "HLT::HLTResult/HLTResult_L2",
                    "HLT::HLTResult/HLTResult_EF" ]
            else:
                ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
                    "HLT::HLTResult/HLTResult_HLT" ]
                pass
            pass

        # BS unpacking
        from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
        extr = TrigBSExtraction()
        
        if hasHLT:
            from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
            extr.Navigation = HLTNavigationOffline()

            from TrigEDMConfig.TriggerEDM import getEDMLibraries
            extr.Navigation.Dlls = getEDMLibraries()            

            from TrigEDMConfig.TriggerEDM import getPreregistrationList
            extr.Navigation.ClassesToPreregister = getPreregistrationList(TriggerFlags.EDMDecodingVersion())
            
            if TriggerFlags.doMergedHLTResult():
                extr.L2ResultKey=""
                extr.EFResultKey=""
            else:
                extr.HLTResultKey=""

            #
            # Configure DataScouting
            #
            from RecExConfig.InputFilePeeker import inputFileSummary
            if inputFileSummary['bs_metadata']['Stream'].startswith('calibration_DataScouting_') or TriggerFlags.doAlwaysUnpackDSResult():
                for stag in inputFileSummary['stream_tags']:
                    if (stag['stream_type'] == 'calibration') and (stag['stream_name'].startswith('DataScouting_')):
                        ds_tag = stag['stream_name'][0:15]
                        ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/"+ds_tag ]
                        extr.DSResultKeys += [ ds_tag ]

        else:            
            #if data doesn't have HLT info set HLTResult keys as empty strings to avoid warnings
            # but the extr alg must run
            extr.L2ResultKey=""
            extr.EFResultKey=""
            extr.HLTResultKey=""
            extr.DSResultKeys=[]

        topSequence += extr
        
        from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
        TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TrigSerToolTP
        from TrigEDMConfig.TriggerEDM import getTPList
        TrigSerToolTP.TPMap = getTPList((TriggerFlags.EDMDecodingVersion()))
        
        from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
        TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
        ToolSvc += TrigSerializeConvHelper

        #
        # Configure L1Topo validation data algorithm
        #
        if hasHLT and TriggerFlags.doMergedHLTResult() and TriggerFlags.writeL1TopoValData() :
            # make sure that CTP_RDO is known (see also ATR-14683)
            ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
                "CTP_RDO/CTP_RDO"
                ]
            from L1TopoValDataCnv.L1TopoValDataCnvConf import xAODMaker__L1TopoValDataCnvAlg
            L1TopoValDataCvnAlg = xAODMaker__L1TopoValDataCnvAlg()
            topSequence += L1TopoValDataCvnAlg

        return True


class TrigDecisionGetter(Configured):
    #class to setup the writing or just making of TrigDecisionObject
    def configure(self):
        
        log = logging.getLogger("TrigDecisionGetter")

        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()
        
        #if hasOnlyLVL1:
        #from RecExConfig.ObjKeyStore import objKeyStore
        #objKeyStore.addStreamESD('TrigDec::TrigDecision','TrigDecision')
        #objKeyStore.addStreamAOD('TrigDec::TrigDecision','TrigDecision')
        
        from RecExConfig.RecFlags import rec
        if ( rec.doWriteESD() or rec.doWriteAOD() or rec.doESD() or rec.doAOD() ) and \
               ( not ( rec.readAOD() or rec.readESD() or rec.doWriteBS()) ):
            log.info("Will write TrigDecision object to storegate")
            
            from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
            trigDecWriter = WriteTrigDecision()

#            from TrigDecisionMaker.TrigDecisionMakerConfig import WritexAODTrigDecision
#            trigDecWriter = WritexAODTrigDecision()

            # inform TD maker that some parts may be missing
            if TriggerFlags.dataTakingConditions()=='Lvl1Only':
                topSequence.TrigDecMaker.doL2=False
                topSequence.TrigDecMaker.doEF=False
                topSequence.TrigDecMaker.doHLT=False
            elif TriggerFlags.dataTakingConditions()=='HltOnly':
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence.TrigDecMaker.doL1=False
            # Decide based on the doMergedHLTResult to assume a merged, or a
            # split HLT:
            if not TriggerFlags.doMergedHLTResult():
                topSequence.TrigDecMaker.doHLT = False
            else:
                topSequence.TrigDecMaker.doL2 = False
                topSequence.TrigDecMaker.doEF = False
                pass
                
        else:
            log.info("Will not write TrigDecision object to storegate")
    
        return True
    
    
class HLTTriggerResultGetter(Configured):

    log = logging.getLogger("HLTTriggerResultGetter.py")

    def _AddOPIToESD(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")        
        
        if rec.doESD():
            from RecExConfig.InputFilePeeker import inputFileSummary
            if inputFileSummary.has_key('bs_metadata') and inputFileSummary['bs_metadata'].has_key('Stream'):
                stream=inputFileSummary['bs_metadata']['Stream']
                log.debug("the stream found in 'bs_metadata' is "+stream)
                if "express" in stream:
                    from TrigEDMConfig.TriggerEDM import getTypeAndKey,EDMDetails
                    type,key=getTypeAndKey("TrigOperationalInfo#HLT_EXPRESS_OPI_HLT")
                    if EDMDetails[type].has_key('collection'):
                        colltype = EDMDetails[type]['collection']
                        log.info("Adding HLT_EXPRESS_OPI_HLT to ESD for stream "+stream)                        
                        from RecExConfig.ObjKeyStore import objKeyStore
                        objKeyStore.addStreamESD(colltype, key)
                    return True
            else:
                log.warning("Could not determine stream of bytestream file, not adding HLT_EXPRESS_OPI_HLT to ESD.")
        return False

    def configure(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")
        from RecExConfig.ObjKeyStore import objKeyStore

        # set EDMDecodingVersion
        EDMDecodingVersion()

        # Set AODFULL for data unless it was set explicitly already
        if TriggerFlags.AODEDMSet.isDefault() and globalflags.DataSource()=='data':
            TriggerFlags.AODEDMSet = 'AODFULL'
            
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        log.info("BS unpacking (TF.readBS): %d" % TriggerFlags.readBS() )
        if TriggerFlags.readBS():
            bs = ByteStreamUnpackGetter()

        xAODContainers = {}
#        if not recAlgs.doTrigger():      #only convert when running on old data
        if TriggerFlags.EDMDecodingVersion()==1:
            xaodcnvrt = xAODConversionGetter()
            xAODContainers = xaodcnvrt.xaodlist

        if recAlgs.doTrigger() or TriggerFlags.doTriggerConfigOnly():
            tdt = TrigDecisionGetter()

        # TrigJetRec additions
        if rec.doWriteESD():
            objKeyStore.addStreamESD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamESD("JetMomentMap","TrigJetRecMomentMap")

        if rec.doWriteAOD():
            objKeyStore.addStreamAOD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamAOD("JetMomentMap","TrigJetRecMomentMap")
                    
        # ID truth
        if not rec.readESD() and (not rec.readAOD()) and TriggerFlags.doID() \
                and rec.doTruth():
            try:
                from TrigInDetTruthAlgs.TrigInDetTruthAlgsConfig import \
                    TrigIDTruthMaker
                topSequence += TrigIDTruthMaker()
            except Exception:
                log.warning( "Couldn't set up the trigger ID truth maker" )
                pass

        if rec.doESD() or rec.doAOD():
            from TrigEDMConfig.TriggerEDM import getTrigIDTruthList
            objKeyStore.addManyTypesStreamESD(getTrigIDTruthList(TriggerFlags.ESDEDMSet()))
            objKeyStore.addManyTypesStreamAOD(getTrigIDTruthList(TriggerFlags.AODEDMSet()))

        if (rec.doESD() or rec.doAOD()) and TriggerFlags.writeL1TopoValData():
            objKeyStore.addManyTypesStreamESD(['xAOD::TrigCompositeContainer#HLT_xAOD__TrigCompositeContainer_L1TopoValData',
                                               'xAOD::TrigCompositeAuxContainer#HLT_xAOD__TrigCompositeContainer_L1TopoValDataAux.'])
            objKeyStore.addManyTypesStreamAOD(['xAOD::TrigCompositeContainer#HLT_xAOD__TrigCompositeContainer_L1TopoValData',
                                               'xAOD::TrigCompositeAuxContainer#HLT_xAOD__TrigCompositeContainer_L1TopoValDataAux.'])
            log.debug("HLT_xAOD__TrigCompositeContainer_L1TopoValData(Aux.) for L1Topo validation added to the data.")

        if rec.doAOD() or rec.doWriteAOD():
            # schedule the RoiDescriptorStore conversion
            # log.warning( "HLTTriggerResultGetter - setting up RoiWriter" )
            topSequence += RoiWriter()
            # write out the RoiDescriptorStores
            from TrigEDMConfig.TriggerEDM import TriggerRoiList
            objKeyStore.addManyTypesStreamAOD( TriggerRoiList )

        #Are we adding operational info objects in ESD?
        added=self._AddOPIToESD()
        if added:
            log.debug("Operational Info object HLT_EXPRESS_OPI_HLT with extra information about express stream prescaling added to the data.")
        


        # ESD objects definitions
        _TriggerESDList = {}

        from TrigEDMConfig.TriggerEDM import getTriggerEDMList 
        # we have to store xAOD containers in the root file, NOT AOD,
        # if the xAOD container list is not empty
        if(xAODContainers):
            _TriggerESDList.update( xAODContainers )
        else:
            _TriggerESDList.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(),  TriggerFlags.EDMDecodingVersion()) ) 
        
        log.info("ESD content set according to the ESDEDMSet flag: %s and EDM version %d" % (TriggerFlags.ESDEDMSet() ,TriggerFlags.EDMDecodingVersion()) )

        # AOD objects choice
        _TriggerAODList = {}
        
        #from TrigEDMConfig.TriggerEDM import getAODList    
        _TriggerAODList.update( getTriggerEDMList(TriggerFlags.AODEDMSet(),  TriggerFlags.EDMDecodingVersion()) ) 

        log.info("AOD content set according to the AODEDMSet flag: %s and EDM version %d" % (TriggerFlags.AODEDMSet(),TriggerFlags.EDMDecodingVersion()) )

        log.debug("ESD EDM list: %s", _TriggerESDList)
        log.debug("AOD EDM list: %s", _TriggerAODList)
        
        # Highlight what is in AOD list but not in ESD list, as this can cause
        # the "different number of entries in branch" problem, when it is in the
        # AOD list but the empty container per event is not created
        # Just compares keys of dicts, which are the class names, not their string keys in StoreGate
        not_in = [ element for element in  _TriggerAODList if element not in _TriggerESDList ]
        if (len(not_in)>0):
            log.warning("In AOD list but not in ESD list: ")
            log.warning(not_in)
        else:
            log.info("AOD list is subset of ESD list - good.")


        def _addSlimming(stream, thinningSvc, edm):
            from AthenaCommon.AlgSequence import AlgSequence 
            topSequence = AlgSequence()
            from TrigNavTools.TrigNavToolsConf import HLT__StreamTrigNavSlimming, HLT__TrigNavigationSlimming
            from TrigNavTools.TrigNavToolsConfig import navigationSlimming

            edmlist = list(y.split('-')[0] for x in edm.values() for y in x) #flatten names
          
            # from HLT result drop unrecorded features
            # slimmerHLT = HLT__StreamTrigNavSlimming('HLTNavSlimmer_%s'%stream)
            slimmerHLT = HLT__TrigNavigationSlimming('TrigNavigationSlimmer_%s'%stream)
            tHLT = navigationSlimming({'name':'HLTNav_%s'%stream, 'mode':'cleanup', 
                                                          'ThinningSvc':thinningSvc, 'result':'HLTResult_HLT',
                                                          'features':edmlist})
            #tHLT.SlimmingTool.OutputLevel=DEBUG
            tHLT.ActInPlace=True
            slimmerHLT.ThinningTool = tHLT
            print slimmerHLT.ThinningTool
            topSequence += slimmerHLT
            log.info("Configured slimming of HLT")
            del edmlist


        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaServices.Configurables import ThinningSvc, createThinningSvc
        
        _doSlimming = True
        if _doSlimming and rec.readRDO() and rec.doWriteAOD():
            if not hasattr(svcMgr, 'ThinningSvc'): # if the default is there it is configured for AODs
                svcMgr += ThinningSvc(name='ThinningSvc', Streams=['StreamAOD'])             
            _addSlimming('StreamAOD', svcMgr.ThinningSvc, _TriggerESDList ) #Use ESD item list also for AOD!
            log.info("configured navigation slimming for AOD output")
            
        if _doSlimming and rec.readRDO() and rec.doWriteESD(): #rec.doWriteESD() and not rec.readESD(): 
            if not  hasattr(svcMgr, 'ESDThinningSvc'):
                svcMgr += ThinningSvc(name='ESDThinningSvc', Streams=['StreamESD']) # the default is configured for AODs
            _addSlimming('StreamESD', svcMgr.ESDThinningSvc, _TriggerESDList )                
            log.info("configured navigation slimming for ESD output")              
            



        objKeyStore.addManyTypesStreamESD( _TriggerESDList )                        
        objKeyStore.addManyTypesStreamAOD( _TriggerAODList )        
            
        return True





