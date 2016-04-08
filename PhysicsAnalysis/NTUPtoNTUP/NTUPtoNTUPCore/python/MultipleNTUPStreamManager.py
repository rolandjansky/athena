# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from OutputStreamAthenaPool.MultipleStreamManager import *
from AthenaCommon.AppMgr import theApp

class AugmentedNTUPStream( AugmentedStreamBase ):
    def  __init__(self, StreamName, FileName, tupleName = None, asAlg = False ):

        # Initialize the base class:
        AugmentedStreamBase.__init__(self,StreamName)

        # Check if the user specified a tree name or not:
        if tupleName == None:
            tupleName = StreamName

        # event-by-event stream
        import AthenaCommon.CfgMgr as CfgMgr
        from AthenaRootComps.WriteAthenaRoot import createNtupleOutputStream
        from AthenaRootComps.AthenaRootCompsConf import Athena__RootOutputStreamTool as AthenaRootOutputStreamTool
        from AthenaRootComps.AthenaRootCompsConf import Athena__RootNtupleOutputMetadataTool as RootNtupleOutputMetadataTool

        self.Stream = createNtupleOutputStream( StreamName, FileName, tupleName, asAlg)
        metadataTool = RootNtupleOutputMetadataTool( StreamName + "Meta" )
        metadataTool.OutputFile = FileName
        metadataTool.StreamName = StreamName
        metadataTool.TupleName = tupleName
        theApp.serviceMgr().ToolSvc += metadataTool
        self.Stream.HelperTools = [metadataTool]
        self.Stream.ForceRead=True;

        return

    def SetOutputFileName(self, name):
        self.Stream.OutputFile = name
        return

    #########################################
    #Items & MetaDataItems
    def AddItem(self, item):
        self._AddValidItemToList(item, self.Stream.ItemList)
        return

    def RemoveItem(self, item):
        self._RemoveValidItemFromList(item, self.Stream.ItemList)
        return

    def GetItems(self):
        return self.Stream.ItemList

    def AddMetaDataItem(self, item):
        #self._AddValidItemToList(item, self.Stream_FH.ItemList)
        return

    def RemoveMetaDataItem(self, item):
        #self._RemoveValidItemFromList(item, self.Stream_FH.ItemList)
        #return
        pass

    def GetMetaDataItems(self):
        #return self.Stream_FH.ItemList
        pass

    #########################################
    def Print(self):
        print "**** AugmentedPoolStream",self.Name,"****"
        print "Output file:"
        print self.Stream.OutputFile
        print "AcceptAlgs:"
        print self.Stream.AcceptAlgs
        print "RequireAlgs:"
        print self.Stream.RequireAlgs
        print "VetoAlgs:"
        print self.Stream.VetoAlgs
        print "OtherAlgs to bookkeep (but not directly used by the Stream):"
        print self.OtherAlgs
        print "Master prescale:"
        print self.GetPrescale()
        print "ItemList:"
        print self.Stream.ItemList
        #print "MetaData ItemList:"
        #print self.Stream_FH.ItemList
        return
#############################################################
class MultipleNTUPStreamManager:
    def __init__(self):
        self.StreamList=[]
        self.nStream=0
        self.StreamDict={}
        self._Locked=False
        return

    def NewNTUPStream(self,StreamName,FileName=None,TreeName=None,asAlg=False):
        # Check if a file name was specified or not:
        if FileName == None:
            FileName = StreamName + ".root"
        # Use the common function for creating the stream:
        return self.NewStream( StreamName, FileName, type='ntup', asAlg = asAlg,
                               TreeName = TreeName )
    def NewStream(self,StreamName,FileName="default",type='pool',asAlg=False,TreeName=None):
        if FileName=="default":
            FileName=StreamName+".pool.root"
        try:
            #Check wheter a stream with the same name already exists
            index=self.StreamDict[StreamName]
        except KeyError:
            #The stream doesn't already exist. Register it and set it up.
            #(This is expected, not actually an error.)
            index=self.nStream
            if type=='ntup':
                self.StreamList += [ AugmentedNTUPStream(StreamName,FileName,TreeName,asAlg) ]
            else:
                raise RuntimeError("Unknown type '%s'"%type)

            self.StreamDict[StreamName]=index
            self.nStream+=1
        else:
            #This is the real error case...
            raise NameError("Stream %s already exists"%StreamName)
        return self.StreamList[index]
    
    def GetStream(self, NameOrIndex):
        #If NameOrIndex is an int, treat it as an index
        if isinstance(NameOrIndex, int):
            if NameOrIndex < self.nStream:
                return self.StreamList[NameOrIndex]
            else:
                raise IndexError("ERROR: No stream with index %i is defined in MultipleNTUPStreamManager."%NameOrIndex)

        #else treat NameOrIndex as a name in the Stream Dictionary
        try:
            #Check wheter a stream with the same name already exists
            index=self.StreamDict[NameOrIndex]
        except KeyError:
            raise NameError("Stream %s undefined!"%NameOrIndex)
        
        return self.StreamList[index]
    
    def StreamExists(self, StreamName):        
        try:
            index=self.StreamDict[StreamName]
        except KeyError:
            return False
        
        return True
    
    def Print(self):
        print "**** MultipleNTUPStreamManager INFOS ****" 
        print "Number of streams:", self.nStream
        i=0
        for Stream in self.StreamList:
            print "----------------------- Stream #",i," -----------------------"
            Stream.Print()
            i+=1
        return

    #Commands for the real manager (normal users only manipulate their own streams with the functions above)
    def Lock(self):
        self._Locked=True
        return

    def Unlock(self):
        self._Locked=False
        return
    
    def AddItemToAllStreams(self, item):
        if self._Locked is True:
            raise AssertionError("MSMgr is locked. AddItemToAllStreams cannot be used.")
        for Stream in self.StreamList:
            Stream.AddItem(item)
        return
    
    def RemoveItemFromAllStreams(self, item):
        if self._Locked is True:
            raise AssertionError("MSMgr is locked. RemoveItemFromAllStreams cannot be used.")
        for Stream in self.StreamList:
            Stream.RemoveItem(item)
        return
    
    def AddMetaDataItemToAllStreams(self, item):
        if self._Locked is True:
            raise AssertionError("MSMgr is locked. AddMetaDataItemToAllStreams cannot be used.")
        for Stream in self.StreamList:
            Stream.AddMetaDataItem(item)
        return

    def RemoveMetaDataItemFromAllStreams(self, item):
        if self._Locked is True:
            raise AssertionError("MSMgr is locked. AddMetaDataItemFromAllStreams cannot be used.")
        for Stream in self.StreamList:
            Stream.RemoveMetaDataItem(item)
        return

    def RenameAllStreams(self, NameList):
        if self._Locked is True:
            raise AssertionError("MSMgr is locked. RenameAllStreams cannot be used.")
        if not isinstance(NameList, list):
            raise TypeError("RenameAllStreams does not accep arguments of type %s"%type(NameList))
        if len(NameList) != self.nStream:
            raise IndexError("NameList needs to have the same length as self.StreamList.")
        
        i=0
        while i<self.nStream:
            self.StreamList[i].SetOutputFileName(NameList[i])
            i+=1            
        return

    def WriteSkimDecisionsOfAllStreams(self):
        if self._Locked:
            raise AssertionError("MSMgr is locked. WriteSkimDecisionsOfAllStreams cannot be used.")
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        for Stream in self.StreamList:
            if Stream.GetAcceptAlgs() or Stream.GetOtherAlgsToBookkeep() or Stream.GetRequireAlgs() or Stream.GetVetoAlgs():
                sdw=Stream.GetSkimDecisionsWriter()
                topSequence+=sdw
                if isinstance(Stream,AugmentedPoolStream):
                    Stream.AddItem("SkimDecisionCollection#"+sdw.SkimDecisionsContainerName)
        return

    def CreateEventBookkeepersWriterForAllFilters(self,doMCTruth=False,cycle_number=0):        
        from EventBookkeeperTools.BookkeepingInfoWriter import EventBookkeepersWriter
        ebw=EventBookkeepersWriter()
        ebw.setDoMC( doMCTruth )
        ebw.setCycle(cycle_number)
        
        #Loop over all streams and add all associated algorithms to ebw
        for Stream in self.StreamList:
            for a in Stream.GetRequireAlgs():
                ebw.addRequireAlg(a,StreamName=Stream.Name)
            for a in Stream.GetAcceptAlgs():
                ebw.addAcceptAlg(a,StreamName=Stream.Name)
            for a in Stream.GetVetoAlgs():
                ebw.addVetoAlg(a,StreamName=Stream.Name)
            for a in Stream.GetOtherAlgsToBookkeep():
                ebw.addOtherAlg(a,StreamName=Stream.Name)

        return ebw
    


############################################################################
# Create one instance of MultipleNTUPStreamManager (MNSMgr) if not already done.
# Otherwise, do dothing (avoid overwriting MSMgr!).
if vars().has_key('MNSMgr'):
    raise RuntimeError("MNSMgr already exists?!? This will almost certainly create erroneous results.")
MNSMgr=MultipleNTUPStreamManager()

