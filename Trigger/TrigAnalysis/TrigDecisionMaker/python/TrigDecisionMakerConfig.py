# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigDecisionMaker.TrigDecisionMakerConf import TrigDec__TrigDecisionMaker
#from TrigDecisionMaker.TrigDecisionMakerConf import TrigDec__TrigDecisionTest

from AthenaCommon.AppMgr import ToolSvc

class TrigDecisionMaker( TrigDec__TrigDecisionMaker ):
    __slots__ = []
    def __init__(self, name = "TrigDecMaker"):
        super( TrigDecisionMaker, self ).__init__( name )

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

    def setDefaults(self, handle):
        pass
        #from AthenaCommon.Constants import DEBUG
        #handle.OutputLevel = DEBUG
        #return
        



class TrigDecisionStream ( object) :
    def __init__ ( self, streamName = "Stream1", fileName = "HLT.root",
                   catalog = "xmlcatalog_file:Catalog1.xml",
                   store = None) :
        import AthenaPoolCnvSvc.WriteAthenaPool
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.PoolSvc.WriteCatalog = catalog

        from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
        svcMgr += AthenaPoolCnvSvc()
        svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;

        # revert later from OutputStreamAthenaPool.OutputStreamAthenaPool import createOutputStream
        # revert later self.stream = createOutputStream( streamName )

        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
        self.stream = AthenaPoolOutputStream( streamName )

        self.stream.OutputFile = fileName

        if store != None :
            self.stream.Store = store
        else :
            from StoreGate.StoreGateConf import StoreGateSvc
            self.stream.Store = StoreGateSvc( "StoreGateSvc" )

        TrigDecisionStream.setItemList(self.stream)

    def setItemList(stream) :
        stream.ItemList += [ "TrigDec::TrigDecision#TrigDecision" ]
    setItemList = staticmethod(setItemList)

    def stream(self) :
        return self.stream

class TrigConditionStream ( object) :
    def __init__ ( self, streamName = "Stream2", fileName = "HLT.root",
                   catalog = "xmlcatalog_file:Catalog1.xml",
                   store = None ) :

        import AthenaPoolCnvSvc.WriteAthenaPool
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from PoolSvc.PoolSvcConf import PoolSvc
        svcMgr += PoolSvc()
        svcMgr.PoolSvc.WriteCatalog = catalog

        from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
        svcMgr += AthenaPoolCnvSvc()
        svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;

        from OutputStreamAthenaPool.OutputStreamAthenaPool import AthenaPoolOutputConditionStream
        self.stream = AthenaPoolOutputConditionStream( streamName )

        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
        self.stream = AthenaPoolOutputStream( streamName )

        self.stream.OutputFile = fileName

        if store != None :
            self.stream.Store = store
        else :
            from StoreGate.StoreGateConf import StoreGateSvc
            self.stream.Store = StoreGateSvc( "DetectorStore" )

        TrigConditionStream.setItemList(self.stream)

    def setItemList(stream) :
        pass
    setItemList = staticmethod(setItemList)

    def stream(self) :
        return self.stream


class WriteTrigDecisionToFile ( object ) :
    def __init__ ( self, fileName = "TrigDec.root",
                   catalog = "xmlcatalog_file:Catalog1.xml" ) :

        from AthenaCommon.AlgSequence import AlgSequence
        TopAlg = AlgSequence()

        self.TrigDecMaker    = TrigDecisionMaker('TrigDecMaker')

        TopAlg += self.TrigDecMaker

        from StoreGate.StoreGateConf import StoreGateSvc
        sgStore = StoreGateSvc("StoreGateSvc")
        dtStore = StoreGateSvc("DetectorStore")

        self.TrigDecStream  = TrigDecisionStream ("Stream1", fileName, catalog, sgStore)


class WriteTrigDecisionToStream ( object ) :
    def __init__ ( self, decStream, condStream ) :

        from AthenaCommon.AlgSequence import AlgSequence
        TopAlg = AlgSequence()

        self.TrigDecMaker    = TrigDecisionMaker('TrigDecMaker')

        TopAlg += self.TrigDecMaker

        decStream.setItemList(decStream)
        condStream.setItemList(condStream)

        self.TrigDecStream  = decStream
        self.TrigCondStream = condStream


class WritexAODTrigDecision ( object ) :
    def __init__(self):
        from AthenaCommon.AlgSequence import AlgSequence
        TopAlg = AlgSequence()

        from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigDecisionCnvAlg
        alg = xAODMaker__TrigDecisionCnvAlg()

        # In order for the conversion to work we need to setup the TrigDecisionTool such that it uses the old decision
        ToolSvc.TrigDecisionTool.UseAODDecision = True
        ToolSvc.TrigDecisionTool.TrigDecisionKey = "TrigDecision"

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( 'WritexAODTrigDecision' )
        log.info('TrigDecisionTool setup to use old decision')
                
        alg.xAODKey = "xTrigDecision"
        TopAlg += alg
        
        from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigNavigationCnvAlg
        TopAlg += xAODMaker__TrigNavigationCnvAlg()

        log.info('TrigDecision writing to xAOD enabled')



class WriteTrigDecision ( object ) :
    def __init__ ( self, AODItemList = None, ESDItemList = None, doxAOD = True) :

        from AthenaCommon.AlgSequence import AlgSequence
        TopAlg = AlgSequence()

        self.TrigDecMaker    = TrigDecisionMaker('TrigDecMaker')

        TopAlg += self.TrigDecMaker

        if AODItemList != None : self.addItemsToList(AODItemList)
        if ESDItemList != None : self.addItemsToList(ESDItemList)

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( 'WriteTrigDecisionToAOD' )

        log.info('TrigDecision writing enabled')
        
        makexAOD = WritexAODTrigDecision()


    def addItemsToList(self, itemList) :
        itemList += [ "TrigDec::TrigDecision#*" ]





class ReadTrigDecisionFromFile ( object ) :
    def __init__ ( self, fileName = "TrigDec.root",
                   catalog = "xmlcatalog_file:Catalog1.xml" ) :

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.Constants import TRUE

        import AthenaPoolCnvSvc.ReadAthenaPool

        svcMgr.EventSelector.InputCollections = [ fileName ]
        svcMgr.EventSelector.BackNavigation = TRUE; # // switch on BackNavigation of AddressProvider
        svcMgr.PoolSvc.ReadCatalog = [ catalog ]


# class TrigDecisionTest( TrigDec__TrigDecisionTest ):
#     __slots__ = []
#     def __init__(self, name="TrigDecTest"):
#         super( TrigDecisionTest, self ).__init__( name )

#     def setDefaults(self, handle):
#         pass

#         if not hasattr( handle, 'TrigDecisionTool' ) :
#             from TrigDecision.TrigDecisionConfig import TrigDecisionTool
#             handle.TrigDecisionTool = TrigDecisionTool('TrigDecisionTool')
