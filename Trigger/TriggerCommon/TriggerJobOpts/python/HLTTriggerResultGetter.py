# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging

from RecExConfig.Configured import Configured

from RecExConfig.RecFlags import rec

from TrigRoiConversion.TrigRoiConversionConf import RoiWriter
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper


class xAODConversionGetter(Configured):
    def configure(self):
        from TriggerJobOpts.TriggerRecoConfig import Run1xAODConversionCfg
        CAtoGlobalWrapper(Run1xAODConversionCfg, ConfigFlags)
        return True
    
        

class ByteStreamUnpackGetter(Configured):
    def configure(self):
        log = logging.getLogger("ByteStreamUnpackGetter")

        # Define the decoding sequence
        from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
        from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg
        from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
        from AthenaCommon.CFElements import seqAND
        decoder = HLTResultMTByteStreamDecoderAlg()
        deserialiser = TriggerEDMDeserialiserAlg("TrigDeserialiser")
        nav_collection = getRun3NavigationContainerFromInput(ConfigFlags)
        deserialiser.ExtraOutputs += [('xAOD::TrigCompositeContainer' , 'StoreGateSvc+%s' % nav_collection)]  
        decodingSeq = seqAND("HLTDecodingSeq")
        decodingSeq += decoder  # BS -> HLTResultMT
        decodingSeq += deserialiser  # HLTResultMT -> xAOD

        # Append the decoding sequence to topSequence
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += decodingSeq

        log.debug("Configured HLT result BS decoding sequence")
        return True

class ByteStreamUnpackGetterRun1or2(Configured):
    def configure(self):
        from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
        from TriggerJobOpts.TriggerRecoConfig import Run1Run2BSExtractionCfg
        CAtoGlobalWrapper(Run1Run2BSExtractionCfg, ConfigFlags)

        return True


class TrigDecisionGetter(Configured):
    def configure(self):
        log = logging.getLogger("TrigDecisionGetter")
        from TrigDecisionMaker.TrigDecisionMakerConfig import Run3DecisionMakerCfg
        CAtoGlobalWrapper( Run3DecisionMakerCfg, ConfigFlags)
        log.info('xTrigDecision writing enabled')

        return True

class TrigDecisionGetterRun1or2(Configured):
    #class to setup the writing or just making of TrigDecisionObject
    def configure(self):
        if ( rec.doWriteESD() or rec.doWriteAOD() or rec.doESD() or rec.doAOD() ) and \
               ( not ( rec.readAOD() or rec.readESD() or rec.doWriteBS()) ):
            log = logging.getLogger("TrigDecisionGetterRun1or2")
            from TrigDecisionMaker.TrigDecisionMakerConfig import Run1Run2DecisionMakerCfg
            CAtoGlobalWrapper(Run1Run2DecisionMakerCfg, ConfigFlags)
            log.info('xTrigDecision writing enabled')
        return True
    
    
class HLTTriggerResultGetter(Configured):

    log = logging.getLogger("HLTTriggerResultGetter.py")

    def configure(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")
        from RecExConfig.ObjKeyStore import objKeyStore

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        log.info("BS unpacking (ConfigFlags.Trigger.readBS): %d", ConfigFlags.Trigger.readBS )
        if ConfigFlags.Trigger.readBS:
            if ConfigFlags.Trigger.EDMVersion == 1 or \
               ConfigFlags.Trigger.EDMVersion == 2:
                bs = ByteStreamUnpackGetterRun1or2()  # noqa: F841
            elif ConfigFlags.Trigger.EDMVersion >=3:
                bs = ByteStreamUnpackGetter()  # noqa: F841
            else:
                raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)

        xAODContainers = {}

        if ConfigFlags.Trigger.EDMVersion == 1:
            xAODConversionGetter()
            # xAODContainers = xaodcnvrt.xaodlist

        if ConfigFlags.Trigger.EDMVersion == 1 or \
           ConfigFlags.Trigger.EDMVersion == 2:
            if rec.doTrigger():
                tdt = TrigDecisionGetterRun1or2()  # noqa: F841
        elif ConfigFlags.Trigger.EDMVersion >= 3:
            if ConfigFlags.Trigger.readBS:
                tdt = TrigDecisionGetter()  # noqa: F841
        else:
            raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)

        # Temporary hack to add Run-3 navigation to ESD and AOD
        if (rec.doESD() or rec.doAOD()) and ConfigFlags.Trigger.EDMVersion == 3:
            # The hack with wildcards is needed for BS->ESD because we don't know the exact keys
            # of HLT navigation containers before unpacking them from the BS event.
            objKeyStore._store['streamESD'].allowWildCard(True)
            objKeyStore._store['streamAOD'].allowWildCard(True)
            objKeyStore.addManyTypesStreamESD(['xAOD::TrigCompositeContainer#HLTNav*',
                                               'xAOD::TrigCompositeAuxContainer#HLTNav*'])
            objKeyStore.addManyTypesStreamAOD(['xAOD::TrigCompositeContainer#HLTNav*',
                                               'xAOD::TrigCompositeAuxContainer#HLTNav*'])

        # TrigJetRec additions
        if rec.doWriteESD():
            objKeyStore.addStreamESD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamESD("JetMomentMap","TrigJetRecMomentMap")

        if rec.doWriteAOD():
            objKeyStore.addStreamAOD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamAOD("JetMomentMap","TrigJetRecMomentMap")
                    
        if rec.doAOD() or rec.doWriteAOD():
            # schedule the RoiDescriptorStore conversion
            # log.warning( "HLTTriggerResultGetter - setting up RoiWriter" )
            roiWriter = RoiWriter()
            # Add fictional input to ensure data dependency in AthenaMT
            roiWriter.ExtraInputs += [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]
            topSequence += roiWriter
            # write out the RoiDescriptorStores
            from TrigEDMConfig.TriggerEDMRun2 import TriggerRoiList
            objKeyStore.addManyTypesStreamAOD( TriggerRoiList )




        # ESD objects definitions
        _TriggerESDList = {}

        from TrigEDMConfig.TriggerEDM import getTriggerEDMList 
        # we have to store xAOD containers in the root file, NOT AOD,
        # if the xAOD container list is not empty
        if(xAODContainers):
            _TriggerESDList.update( xAODContainers )
        else:
            _TriggerESDList.update( getTriggerEDMList(ConfigFlags.Trigger.ESDEDMSet,  ConfigFlags.Trigger.EDMVersion) )
        
        log.info("ESD content set according to the ESDEDMSet flag: %s and EDM version %d", ConfigFlags.Trigger.ESDEDMSet, ConfigFlags.Trigger.EDMVersion)

        # AOD objects choice
        _TriggerAODList = {}
        
        #from TrigEDMConfig.TriggerEDM import getAODList    
        _TriggerAODList.update( getTriggerEDMList(ConfigFlags.Trigger.AODEDMSet,  ConfigFlags.Trigger.EDMVersion) )

        log.info("AOD content set according to the AODEDMSet flag: %s and EDM version %d", ConfigFlags.Trigger.AODEDMSet, ConfigFlags.Trigger.EDMVersion)

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


        def _addSlimmingRun2(stream, edm):
            from TrigNavTools.TrigNavToolsConfig import navigationThinningSvc

            edmlist = list(y.split('-')[0] for x in edm.values() for y in x) #flatten names
          
            # TimM Sep 2021: In MT the 'reload' slimming option in the R2 navigation thinning service was found to be creating
            # AODs which would crash when trying to return features. We therefore remove this option by using the added 'cleanup_noreload'
            # configuration, see ATR-24141 for details. 
            svc = navigationThinningSvc ({'name':'HLTNav_%s'%stream, 'mode':'cleanup_noreload', 
                                          'result':'HLTResult_HLT',
                                          'features':edmlist})

            from OutputStreamAthenaPool.CreateOutputStreams import registerTrigNavThinningSvc
            registerTrigNavThinningSvc (stream, svc)

            log.info("Configured slimming of HLT for %s", stream)
            print(svc)  # noqa: ATL901
            del edmlist


        if ConfigFlags.Trigger.EDMVersion == 1 or ConfigFlags.Trigger.EDMVersion == 2:

            # Run 1, 2 slimming
            if ConfigFlags.Trigger.doNavigationSlimming and rec.readRDO() and rec.doWriteAOD():
                _addSlimmingRun2('StreamAOD', _TriggerESDList ) #Use ESD item list also for AOD!
                log.info("configured navigation slimming for AOD output")
                
            if ConfigFlags.Trigger.doNavigationSlimming and rec.readRDO() and rec.doWriteESD():
                _addSlimmingRun2('StreamESD', _TriggerESDList )                
                log.info("configured navigation slimming for ESD output")
            if ConfigFlags.Trigger.doEDMVersionConversion and 'HLTNav_R2ToR3Summary' not in ConfigFlags.Input.Collections:
                from TrigNavTools.NavConverterConfig import NavConverterCfg
                CAtoGlobalWrapper( NavConverterCfg, ConfigFlags)
                log.info("Configured Run 1/3 -> Run3 Navigation conversion")

        if ConfigFlags.Trigger.EDMVersion >= 3:
            # Change in the future to 'if EDMVersion >= 3 or doEDMVersionConversion:'

            # Run 3 slimming
            if ConfigFlags.Trigger.doNavigationSlimming: 
                from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import TrigNavSlimmingMTCfg
                from AthenaCommon.Configurable import Configurable
                Configurable.configurableRun3Behavior += 1
                from AthenaConfiguration.ComponentAccumulator import appendCAtoAthena
                appendCAtoAthena( TrigNavSlimmingMTCfg(ConfigFlags) )
                Configurable.configurableRun3Behavior -= 1
            else:
                log.info("doNavigationSlimming is False, won't schedule run 3 navigation slimming")

        objKeyStore.addManyTypesStreamESD( _TriggerESDList )
        objKeyStore.addManyTypesStreamAOD( _TriggerAODList )        
            
        return True





