# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################
##  AugmentedStreams & MultipleStreamManager classes  ##
##                                                    ##
##  creation: David Cote (May 2008)                   ##
########################################################

class AugmentedStreamBase:
    """This class manages common methods of AugmentedPoolStream and AugmentedByteStream."""
    def  __init__(self, StreamName):
        self.Name = StreamName
        self.OtherAlgs=[]
        #the following attributes must exist in all child classes:
        #self.Stream
        #self.Print()
        #self.SetOutputFileName()
        #self.Add/Remove(MetaData)Item()
        return

    #Basic list helper functions
    def _ItemInList(self,item,aList):
        #Make sure aList is a list
        if not isinstance(aList, list):
            raise TypeError("AugmentedPoolStream._ItemInList() does not support aList of type %s"%type(aList))

        isInList=False
        for i in aList:
            if i==item:
                isInList=True
        return isInList

    def _AddValidItemToList(self,item,aList):
        #Recursive loop if item is a list
        if isinstance(item,list):
            for i in item:
                self._AddValidItemToList(i,aList)
        #Add to list if item is a string and not already in the list
        elif isinstance(item, str):
            if not self._ItemInList(item,aList):
                aList += [item]
        else:
            raise TypeError("AugmentedPoolStream._AddValidItemToList() does not support item of type %s"%type(item))
        return

    def _RemoveValidItemFromList(self,item,aList):
        #Recursive loop if item is a list
        if isinstance(item,list):
            for i in item:
                self._RemoveValidItemFromList(i,aList)
        #Remove from list if item is a string and already in list
        elif isinstance(item, str):
            if self._ItemInList(item,aList):
                aList.remove(item)
            else:
                print self.Name,": WARNING you asked to remove item '%s' but this item is not present"%item
        else:
            raise TypeError("AugmentedPoolStream._RemoveValidItemFromList() does not support item of type %s"%type(item))
        return

    #########################################
    #Accept Algs
    def AddAcceptAlgs(self, algs):
        self._AddValidItemToList(algs, self.Stream.AcceptAlgs)
        return

    def RemoveAcceptAlgs(self, algs):
        self._RemoveValidItemFromList(algs, self.Stream.AcceptAlgs)
        return

    def GetAcceptAlgs(self):
        return self.Stream.AcceptAlgs

    #########################################
    #Require Algs
    def AddRequireAlgs(self, algs):
        self._AddValidItemToList(algs, self.Stream.RequireAlgs)
        return

    def RemoveRequireAlgs(self, algs):
        self._RemoveValidItemFromList(algs, self.Stream.RequireAlgs)
        return

    def GetRequireAlgs(self):
        return self.Stream.RequireAlgs

    #########################################
    #Veto Algs (ignored by prescaler for now)
    def AddVetoAlgs(self, algs):
        self._AddValidItemToList(algs, self.Stream.VetoAlgs)
        return

    def RemoveVetoAlgs(self, algs):
        self._RemoveValidItemFromList(algs, self.Stream.VetoAlgs)
        return

    def GetVetoAlgs(self):
        return self.Stream.VetoAlgs

    #########################################
    #Other Algs (e.g. those used by a FilterCombiner, but ignored by everybody else)
    def AddOtherAlgsToBookkeep(self, algs):
        self._AddValidItemToList(algs, self.OtherAlgs)
        return

    def RemoveOtherAlgsToBookkeep(self, algs):
        self._RemoveValidItemFromList(algs, self.OtherAlgs)
        return

    def GetOtherAlgsToBookkeep(self):
        return self.OtherAlgs

    #########################################
    #Skim decisions writer
    def GetSkimDecisionsWriter(self):
        from EventBookkeeperTools.BookkeepingInfoWriter import SkimDecisionsWriter
        sdw=SkimDecisionsWriter(self.Name+"_SkimDecisionsWriter")
        newContainerName=self.Name+"_"+sdw.SkimDecisionsContainerName
        sdw.SkimDecisionsContainerName=newContainerName
        #sdw.OutputLevel=2
        for a in self.GetRequireAlgs():
            sdw.addRequireAlg(a)
        for a in self.GetAcceptAlgs():
            sdw.addAcceptAlg(a)
        for a in self.GetVetoAlgs():
            sdw.addVetoAlg(a)
        for a in self.GetOtherAlgsToBookkeep():
            sdw.addOtherAlg(a)
        return sdw

    #########################################
    #Master prescale
    def SetPrescale(self,value):
        #Set a master prescale to given value. Create the Prescaler tool if necessary.
        if not hasattr(self,"_Prescaler"):
            from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
            prescalerName="MasterPrescale_"+self.Name
            self._Prescaler = PrimaryDPDPrescaler(prescalerName)
            #Deep copies of the Require and Accept algs lists considered by the Master prescaler
            #Notes:
            # 1) deep copy is necessary because the Master prescaler itself will be added to the RequireAlgs list\
            # 2) this implies that SetPrescale must be the *last* algorithm added to the stream
            self._Prescaler.RequireAlgs = self.Stream.RequireAlgs[:]
            self._Prescaler.AcceptAlgs = self.Stream.AcceptAlgs[:]

            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            topSequence += self._Prescaler
            self.AddRequireAlgs(prescalerName)

        self._Prescaler.Prescale=value
        return

    def GetPrescale(self):
        if hasattr(self,"_Prescaler"):
            return self._Prescaler.Prescale
        else:
            return "NONE"



#############################################################
class AugmentedPoolStream( AugmentedStreamBase ):
    """This class manages the associated event-by-event and metadata AthenaOutputStreams as a single object."""
    def  __init__(self, StreamName, FileName, asAlg, isVirtual):
        AugmentedStreamBase.__init__(self,StreamName)
        #event-by-event stream
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
        self.Stream = AthenaPoolOutputStream( StreamName, FileName, asAlg )
        self.Stream.ForceRead=True;  #force read of output data objs
        if isVirtual == True:
            self.Stream.WriteOnExecute=False
            self.Stream.WriteOnFinalize=False

        return

    def GetEventStream(self):
        #This is the stream that's written at each event
        return self.Stream

    # FIXME: This function is obsolete and should be remove.
    def GetMetaDataStream(self):
        #This is the stream that's written once per multiple events
        return self.Stream

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
        self._AddValidItemToList(item, self.Stream.MetadataItemList)
        return

    def RemoveMetaDataItem(self, item):
        self._RemoveValidItemFromList(item, self.Stream.MetadataItemList)
        return

    def GetMetaDataItems(self):
        return self.Stream.MetadataItemList

    #########################################
    #Accept Algs
    def AcceptAlgs(self, AlgsList):
        #Keep this function for now... but we may get rid of it.
        self.Stream.AcceptAlgs=AlgsList
        return

    #########################################
    #Require Algs
    def RequireAlgs(self, AlgsList):
        #Keep this function for now... but we may get rid of it.
        self.Stream.RequireAlgs=AlgsList
        return

    #########################################
    #Veto Algs (ignored by prescaler for now)
    def VetoAlgs(self, AlgsList):
        #Keep this function for now... but we may get rid of it
        self.Stream.VetoAlgs=AlgsList
        return

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
        print "MetaData ItemList:"
        print self.Stream.MetadataItemList
        return

##############################################
# See: Event/ByteStreamCnvSvc/share/BSFilter_test_jobOptions.py
# requires external: include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
class AugmentedByteStream( AugmentedStreamBase ):
    def  __init__(self, StreamName,FileName):
        AugmentedStreamBase.__init__(self,StreamName)

        #General setup
        from AthenaCommon.AppMgr import theApp
        svcMgr = theApp.serviceMgr()
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # BS OutputStream Tool
        from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc,ByteStreamOutputStreamCopyTool
        self.bsCopyTool = ByteStreamOutputStreamCopyTool("OutputStreamBSCopyTool")
        svcMgr.ToolSvc += self.bsCopyTool

        #default output directory
        import commands
        cmd='pwd'
        (statusCode,outDir) = commands.getstatusoutput(cmd)
        outDir+="/"

        # BS output Svc
        from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
        self.bsOutputSvc=ByteStreamEventStorageOutputSvc("BSESOutputSvc"+StreamName,
                                                    OutputDirectory=outDir,
                                                    SimpleFileName=FileName )
        svcMgr += self.bsOutputSvc

        # Set BS OutputSvc/InputSvc to BS OutputStream Tool
        self.bsCopyTool.ByteStreamOutputSvc=self.bsOutputSvc
        self.bsCopyTool.ByteStreamInputSvc=svcMgr.ByteStreamInputSvc

        # create AthenaOutputStream for BS Copy and add it to topSequence
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        self.Stream = AthenaOutputStream( StreamName, WritingTool=self.bsCopyTool )

        #topSequence += self.Stream #<-- coherent with asAlg=False in OutputStreamAthenaPool.py
        theApp.addOutputStream( self.Stream )
        return

    def SetOutputFileName(self, name):
        #Method name for coherence with Pool stream, but warning: it's only a file name suffix, not the full name
        self.bsOutputSvc.SimpleFileName = name
        return

    def SetSimpleFileName(self, name):
        self.bsOutputSvc.SimpleFileName = name
        return

    def SetOutputDirName(self, name):
        self.bsOutputSvc.OutputDirectory = name
        return

    def AddItem(self, item): pass
    def RemoveItem(self, item): pass
    def AddMetaDataItem(self, item): pass
    def RemoveMetaDataItem(self, item): pass

    def Print(self):
        print "**** AugmentedByteStream",self.Name,"****"
        print "Output file:"
        print self.bsOutputSvc.OutputDirectory+self.bsOutputSvc.SimpleFileName
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
        print "Not available for bytestream"
        return

#############################################################
class _RootStreamFilterHelper:
    """This class is used to help the AugmentedRootStream class in handling
       filter algorithms in the same way as they behaved with the 'old' way of
       setting up the D3PD::MakerAlg  algorithm.
    """
    def __init__( self, stream, seq ):
        self.stream = stream
        self.seq = seq
        return
    def __iadd__( self, alg ):
        self.seq += alg
        self.stream.AddRequireAlgs( alg.getName() )
        return

class AugmentedRootStream( AugmentedStreamBase ):
    """This class is used to handle output ROOT (D3PD) streams in Athena.
       It inherits from the AugmentedStreamBase class, so implements its
       interface, but at the same time it behaves for all intents and
       purposes like a configurable for the D3PD::MakerAlg class.
    """
    def  __init__( self, StreamName, FileName, TreeName = None, asAlg = False ):
        """Constructor for the D3PD stream object.

           Arguments:
              StreamName: Logical name of the D3PD stream. Note that beside
                          using it to define the stream in THistSvc, this
                          name is also used as the name of the TTree in the
                          output file in case one is not specified explicitly.
              FileName: Name of the file to write the D3PD TTree into.
              TreeName: Name of the TTree in the output file. If it's not
                        specified, the stream name is used as the tree name.
              asAlg: If set to True, the D3PD::MakerAlg algorithm is added
                     to the job as a regular algorithm. When set to False
                     (default), the D3PD algorithm is added to the application
                     manager as an output stream.
        """
        # Initialize the base class:
        AugmentedStreamBase.__init__( self, StreamName )

        # Check if the user specified a tree name or not:
        if TreeName == None:
            TreeName = StreamName

        # Remember the file and tree names just for bookkeeping:
        self.fileName = FileName
        self.treeName = TreeName

        # We need to add some stuff to the main algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # Create a sequence where the pre-D3PD-making algorithms are run:
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName(),
                              StopOverride = True )
        if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
            topSequence += [ preseq ]
            pass
        

        # Add the AANT algorithm for making it possible to back navigate
        # from D3PD events:
        ParentStreamName = StreamName.split( ':' )[ 0 ]
        if StreamName.count( ':' ) != 0:
            if StreamName.count( ':' ) == 1:
                StreamName = StreamName.split( ':' )[ 1 ]
            else:
                raise AttributeError( "Stream name '%s' can't be used!" % StreamName )
        if not hasattr( topSequence, ParentStreamName + "AANTStream" ):
            try:
                from AnalysisTools.AnalysisToolsConf import AANTupleStream
                topSequence += AANTupleStream( ParentStreamName + "AANTStream",
                                               ExtraRefNames = ['StreamRDO',
                                                                'StreamRAW',
                                                                'StreamESD',
                                                                'StreamAOD'],
                                               OutputName = FileName,
                                               WriteInputDataHeader = True,
                                               StreamName = ParentStreamName )
                pass
            except ImportError:
                print self.Name,": INFO didn't find AnalysisTools.AnalysisToolsConf in release."
                pass
            pass
        
        # Make sure that THistSvc exists.
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, 'THistSvc' ):
            from GaudiSvc.GaudiSvcConf import THistSvc
            ServiceMgr += THistSvc()

        # Check if the requested stream is already defined in THistSvc:
        streamExists = False
        for s in ServiceMgr.THistSvc.Output:
            stream = s.split()[ 0 ]
            if stream == StreamName:
                streamExists = True
                break

        # Add the stream if it's not defined yet:
        if not streamExists:
            ServiceMgr.THistSvc.Output += [ "%s DATAFILE='%s' OPT='RECREATE' CL='%i'" %
                                            ( StreamName, FileName,
                                              D3PDMakerFlags.CompressionLevel() ) ]

        # Finally, create the D3PD::MakerAlg algorithm and add it to the job.
        # Note that here we're specifying that the D3PDMaker code should use
        # ROOT output.
        #
        # If we're adding as an algorithm directly, then pass the parent sequence
        # into MakerAlg(...). MakerAlg(...) will then add itself to the sequence
        # and also set up the accompanying filter sequence. Otherwise, we add it
        # as a stream; in that case we set up backwards compatibility for
        # 'filterSeq'.
        try:
            import D3PDMakerCoreComps
            if asAlg:
                theseq = topSequence
            else:
                theseq = None
                pass
            self.Stream = D3PDMakerCoreComps.MakerAlg( StreamName + "D3PDMaker", seq = theseq,
                                                       file = FileName, stream = ParentStreamName,
                                                       tuplename = TreeName,
                                                       D3PDSvc = "D3PD::RootD3PDSvc" )

            if not asAlg:
                from AthenaCommon.AppMgr import theApp
                theApp.addOutputStream( self.Stream )
                # Backwards compatibility for the filter algoirthm:
                self.filterSeq = _RootStreamFilterHelper( self, topSequence )
                pass
            pass
        except ImportError:
            print self.Name,": INFO didn't find D3PDMakerCoreComps in release."
            pass

        return

    def SetOutputFileName(self, name):
        """It's not impossible to change the file name after the stream has been created,
           but I didn't want to write this code unless there's actual need for it.
        """
        print "**** ERROR: Can't change the name of the output ROOT file! ****"
        return

    #
    # The following functions are not applicable for this type of stream:
    #
    def AddItem(self, item): pass
    def RemoveItem(self, item): pass
    def AddMetaDataItem(self, item): pass
    def RemoveMetaDataItem(self, item): pass

    def Print(self):
        print "**** AugmentedRootStream",self.Name,"****"
        print "Output file:"
        print "  " + self.fileName
        print "TTree name:"
        print "  " + self.treeName
        print "AcceptAlgs:"
        print "  " + str( self.Stream.AcceptAlgs )
        print "RequireAlgs:"
        print "  " + str( self.Stream.RequireAlgs )
        print "VetoAlgs:"
        print "  " + str( self.Stream.VetoAlgs )
        print "OtherAlgs to bookkeep (but not directly used by the Stream):"
        print "  " + str( self.OtherAlgs )
        print "Master prescale:"
        print "  " + str( self.GetPrescale() )
        print "ItemList:"
        print "  Not available for ROOT (D3PD) stream"
        return

    def __iadd__( self, configs ):
        """This function makes it possible to add D3PDObject-s to this object in the same
           way as they are added to D3PDMakerCoreComps.MakerAlg objects..__setattr__( name, value )
        """
        self.Stream += configs
        return self

    def __getattr__( self, attr ):
        """This function forwards attribute requests which don't exist in this object to
           the D3PDMakerCoreComps.MakerAlg object.
        """
        if self.__dict__.has_key( 'Stream' ) and hasattr( self.Stream, attr ):
            return getattr( self.Stream, attr )
        raise AttributeError( "D3PD Maker algorithm doesn't have property '%s'." % attr )
        return

    def __setattr__( self, name, value ):
        """This function forwards attribute setting requests to the D3PDMakerCoreComps.MakerAlg
           object.
        """
        if hasattr( self, "Stream" ):
            if hasattr( self.Stream, name ):
                return self.Stream.__setattr__( name, value )
        # The following doesn't work... :-/
        # super( AugmentedRootStream, self ).__setattr__( name, value )
        # So instead I'm using this:
        self.__dict__[ name ] = value
        return

#############################################################
class MultipleStreamManager:
    """This class helps managing multiple streams.
    Normal users only manipulate their own streams with functions like NewStream or GetStream,
    while commands like Lock, AddItemToAllStreams or RenameAllStreams are for the real manager
    (e.g. RecExCommon or a job transform class)"""
    def __init__(self):
        self.StreamList=[]
        self.nStream=0
        self.StreamDict={}
        self._Locked=False
        return

    def NewByteStream(self,StreamName,FileName="default"):
        if FileName=="default":
            FileName=StreamName
        return self.NewStream(StreamName,FileName,type='bytestream')

    def NewPoolStream(self,StreamName,FileName="default", asAlg=False):
        return self.NewStream(StreamName,FileName,type='pool',asAlg=asAlg)

    def NewPoolRootStream(self,StreamName,FileName="default", asAlg=False):
        theStream = self.NewStream(StreamName,FileName,type='pool',asAlg=asAlg)
        from AthenaCommon.AppMgr import theApp
        svcMgr = theApp.serviceMgr()
        theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
        theStream.AddMetaDataItem("xAOD::EventFormat#EventFormat")

        from AthenaCommon import CfgMgr
        streamMarkUpTool = CfgMgr.xAODMaker__FileMetaDataMarkUpTool( StreamName + "_FileMetaDataMarkUpTool" )
        streamMarkUpTool.Key = StreamName
        theStream.Stream.HelperTools += [ streamMarkUpTool ]
        theStream.Stream.WritingTool.SubLevelBranchName = "<key>"
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + FileName + "'; COMPRESSION_LEVEL = '5'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + FileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '-10000000'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + FileName + "'; ContainerName = 'TTree=CollectionTree'; CONTAINER_SPLITLEVEL = '1'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + FileName + "'; ContainerName = 'TTree=Aux.'; CONTAINER_SPLITLEVEL = '1'"]
        return theStream


    def NewVirtualStream(self,StreamName,FileName="default", asAlg=False):
        return self.NewStream(StreamName,FileName,type='virtual',asAlg=asAlg)

    def NewRootStream(self,StreamName,FileName=None,TreeName=None,asAlg=False):
        """Function used to create a new D3PD 'stream'. The object returned by
           it behaves both as an AugmentedStreamBase, and as a D3PD::MakerAlg
           configurable.

           Arguments:
              StreamName: Logical name of the D3PD stream. Used also as the D3PD
                          TTree name in case a tree name is not specified.
              FileName: Name of the output file into which the D3PD should be
                        written. If not specified, '<StreamName>.root' is used.
              TreeName: Name of the TTree in the created file. If not specified,
                        StreamName is used for the TTree name as well.
              asAlg: Selects whether the 'stream' should be configured as an
                     actual stream, or an algorithm.
        """
        # Check if a file name was specified or not:
        if FileName == None:
            FileName = StreamName + ".root"
        # Use the common function for creating the stream:
        return self.NewStream( StreamName, FileName, type='root', asAlg = asAlg,
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
            if type=='pool':
                self.StreamList += [ AugmentedPoolStream(StreamName,FileName,asAlg,isVirtual=False) ]
            elif type=='bytestream':
                self.StreamList += [ AugmentedByteStream(StreamName,FileName) ]
            elif type=='virtual':
                self.StreamList += [ AugmentedPoolStream(StreamName,FileName,asAlg,isVirtual=True) ]
            elif type=='root':
                self.StreamList += [ AugmentedRootStream(StreamName,FileName,TreeName,asAlg) ]
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
                raise IndexError("ERROR: No stream with index %i is defined in MultipleStreamManager."%NameOrIndex)

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
        print "**** MultipleStreamManager INFOS ****"
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
# Create one instance of MultipleStreamManager (MSMgr) if not already done.
# Otherwise, do dothing (avoid overwriting MSMgr!).
if vars().has_key('MSMgr'):
    raise RuntimeError("MSMgr already exists?!? This will almost certainly create erroneous results.")
MSMgr=MultipleStreamManager()
