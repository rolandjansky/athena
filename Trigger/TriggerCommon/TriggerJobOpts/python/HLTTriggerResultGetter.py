# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging  # loads logger
from AthenaCommon.Include import include
from AthenaCommon.GlobalFlags import GlobalFlags
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon.Include import include

from RecExConfig.Configured import Configured

from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
from TriggerJobOpts.TriggerFlags import TriggerFlags
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.RecFlags import rec


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

        from TrigEDMConfig.TriggerEDM import getPreregistrationList,getL2PreregistrationList,getEFPreregistrationList,getEDMLibraries
        xaodconverter.Navigation.ClassesToPreregister = getPreregistrationList()        
        #we attempt to convert the entire old navigation (L2+EF)
        xaodconverter.BStoxAOD.ContainersToConvert = list(set(getL2PreregistrationList()+getEFPreregistrationList()))

        xaodconverter.HLTResultKey="HLTResult_EF"
        topSequence += xaodconverter

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
            from TrigEDMConfig.TriggerEDM import getPreregistrationList,getEDMLibraries
            extr.Navigation.ClassesToPreregister = getPreregistrationList()        
            extr.Navigation.Dlls = getEDMLibraries()            
            if not TriggerFlags.doMergedHLTResult():
                extr.HLTResultKey=""
            else:
                extr.L2ResultKey=""
                extr.EFResultKey=""
        else:            
            #if data doesn't have HLT info set HLTResult keys as empty strings to avoid warnings
            # but the extr alg must run
            extr.L2ResultKey=""
            extr.EFResultKey=""
            extr.HLTResultKey=""

        topSequence += extr
        
        from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
        TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TrigSerToolTP
        from TrigEDMConfig.TriggerEDM import getTPList
        TrigSerToolTP.TPMap = getTPList()
        
        from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
        TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
        ToolSvc += TrigSerializeConvHelper

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
               ( not ( rec.readAOD() or rec.readESD() ) ):
            log.info("Will write TrigDecision object to storegate")
            from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
            trigDecWriter = WriteTrigDecision()
            # inform TD maker that some parts may be missing
            if TriggerFlags.dataTakingConditions()=='Lvl1Only':
                topSequence.TrigDecMaker.doL2=False
                topSequence.TrigDecMaker.doEF=False
                topSequence.TrigDecMaker.doHLT=False
            elif TriggerFlags.dataTakingConditions()=='HltOnly':
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence.TrigDecMaker.doL1=False
            # Decide based on the run number whether to assume a merged, or a
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
                    type,key=getTypeAndKey("TrigOperationalInfo#HLT_EXPRESS_OPI_EF")
                    if EDMDetails[type].has_key('collection'):
                        colltype = EDMDetails[type]['collection']
                        log.info("Adding HLT_EXPRESS_OPI_EF to ESD for stream "+stream)                        
                        from RecExConfig.ObjKeyStore import objKeyStore
                        objKeyStore.addStreamESD(colltype, key)
                    return True
            else:
                log.warning("Could not determine stream of bytestream file, not adding HLT_EXPRESS_OPI_EF to ESD.")
        return False

    def configure(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")
        from RecExConfig.ObjKeyStore import objKeyStore


        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        log.info("BS unpacking (TF.readBS): %d" % TriggerFlags.readBS() )
        if TriggerFlags.readBS():
            bs = ByteStreamUnpackGetter()

        if recAlgs.doTrigger() or TriggerFlags.doTriggerConfigOnly():

            #only convert when running on old trigger
            if not TriggerFlags.doMergedHLTResult():
                xaodcnvrt = xAODConversionGetter()
            
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


        #Are we adding operational info objects in ESD?
        added=self._AddOPIToESD()
        if added:
            log.debug("Operational Info object HLT_EXPRESS_OPI_EF with extra information about express stream prescaling added to the data.")
        


        # ESD objects definitions
        _TriggerESDList = {}

        from TrigEDMConfig.TriggerEDM import getESDList
        _TriggerESDList.update( getESDList( TriggerFlags.ESDEDMSet() ) )
        log.info("ESD content set according to the ESDEDMSet flag: %s" % TriggerFlags.ESDEDMSet() )

        # AOD objects choice
        _TriggerAODList = {}
        
        from TrigEDMConfig.TriggerEDM import getAODList    
        _TriggerAODList.update( getAODList( TriggerFlags.AODEDMSet() ) )

        log.info("AOD content set according to the AODEDMSet flag: %s" % TriggerFlags.AODEDMSet() )
    
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
            from TrigNavTools.TrigNavToolsConf import HLT__StreamTrigNavSlimming

            edmlist = []
            for t,kset in edm.iteritems():
                for k in kset:
                    edmlist.append('%s#%s' % (t,k)) 

            # from AOD navigation drop L2 navigation part
            slimmerL2 = HLT__StreamTrigNavSlimming('L2NavSlimmer_%s'%stream)
            slimmerL2.ResultKey="HLTResult_L2"
            slimmerL2.SlimmingTool.ResultKey="HLTResult_L2"
            slimmerL2.SlimmingTool.DropNavigation=True
            slimmerL2.ThinningSvc = thinningSvc
            slimmerL2.SlimmingTool.ThinningSvc = thinningSvc            
            topSequence += slimmerL2

            # for EF drop the features *not* recorded

            slimmerEF = HLT__StreamTrigNavSlimming('EFNavSlimmer_%s' % stream)
                        
            slimmerEF.SlimmingTool.FeatureInclusionList = edmlist
            slimmerEF.ThinningSvc = thinningSvc
            slimmerEF.SlimmingTool.ThinningSvc = thinningSvc
            slimmerEF.ResultKey="HLTResult_EF"
            slimmerEF.SlimmingTool.ResultKey="HLTResult_EF"
            topSequence += slimmerEF


            # from HLT result drop unrecorded features
            slimmerHLT = HLT__StreamTrigNavSlimming('HLTNavSlimmer_%s'%stream)
            slimmerHLT.SlimmingTool.FeatureInclusionList = edmlist
            slimmerHLT.ThinningSvc = thinningSvc
            slimmerHLT.SlimmingTool.ThinningSvc = thinningSvc
            slimmerHLT.ResultKey="HLTResult_HLT"
            slimmerHLT.SlimmingTool.ResultKey="HLTResult_HLT"
            #slimmerHLT.SlimmingTool.Squeeze=True

            topSequence += slimmerHLT
            log.info("Configured slimming of HLT results L2 EF and HLT")
            del edmlist


        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaServices.Configurables import ThinningSvc, createThinningSvc
        
        _doSlimming = True
        if _doSlimming and rec.doWriteAOD(): #  and not rec.readAOD(): why not to run it when we read AOD??
            # from AOD navigation drop references to the objects related to *not* recorded contaiers
            try:
                svcMgr += ThinningSvc() # the default is configured for AODs
                _addSlimming('StreamAOD', svcMgr.ThinningSvc, _TriggerAODList )
            except Exception:
                log.info("could not configure navigation slimming for AOD output")
        if _doSlimming and rec.doWriteESD(): # and not rec.readESD(): why not to run it when ESD is a source            
            svcMgr += ThinningSvc(name='ESDThinningSvc', Streams=['StreamESD']) # the default is configured for AODs
            # this was recommended but does not work
            # from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
            # svcMgr += createThinningSvc(svcName="ESDThinningSvc", outStreams=[MSMgr.GetStream('StreamESD').GetEventStream()])
            try:
                _addSlimming('StreamESD', svcMgr.ESDThinningSvc, _TriggerESDList )
            except Exception:
                log.info("could not configure navigation slimming for ESD output")

        
        objKeyStore.addManyTypesStreamESD( _TriggerESDList )                        
        objKeyStore.addManyTypesStreamAOD( _TriggerAODList )        
            
        return True





