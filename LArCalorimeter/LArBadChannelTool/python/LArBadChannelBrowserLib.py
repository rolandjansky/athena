# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
import sys,os
from PyCool import cool,coral
import cppyy

import LArBadChannelBrowserTools
import LArDBFolderBrowser_BadChan
import LArDBFolderBrowser_MissingFEBs

STATUS_INIT="INIT"
STATUS_NEW="NEW"
STATUS_REMOVED="DEL"
STATUS_MODIFIED="MOD"

BAD_CHANNEL=0
MISSING_FEB=1

STATUS_NOT_MISSING_FEB="0K"
STATUS_MISSING_FEB="missing"

class LArBadChannelBrowserAlg(PyAthena.Alg):
    """My first python algorithm ( and most probably not the last one... )
    """
    
    def __init__(self, name="LArBadChannelBrowserAlg", **kw):
        ## init the base class
        kw['name'] = name
        super(LArBadChannelBrowserAlg,self).__init__(**kw)
        
        return
    

    def initialize(self):
        ## note that we are using the python logging service
        ## and that the PyAthena.Alg base class has already initialized
        ## it for us

        # -----------------------------------------------------------
        # Store gate  service
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve StoreGateSvc pointer...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if self.sg is None:
            self.msg.error('Problem retrieving StoreGateSvc pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.sg.name())

        # -----------------------------------------------------------
        # Detector store service
        self.msg.debug('retrieve DetectorStoreSvc pointer...')
        self.det = PyAthena.py_svc('DetDescrCnvSvc')
        if self.det is None:
            self.msg.error('Problem retrieving DetectorStoreSvc pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.det.name())

        # -----------------------------------------------------------
        # Message  service
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve MessageSvc pointer...')
        self.msgSvc = PyAthena.py_svc('MessageSvc')
        if self.msgSvc is None:
            self.msg.error('Problem retrieving MessageSvc pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.msgSvc.name())

        # -----------------------------------------------------------
        # get database service and open database
        self.msg.debug('retrieve database service...')
        self.dbSvc = cool.DatabaseSvcFactory.databaseService()

        # -----------------------------------------------------------
        # Read local copy of condb => LarBadChannel folder 
#        dbstring="sqlite://;schema=./myCondLArDB.db;dbname=CONDBR2"
        dbstring="COOLONL_LAR/CONDBR2"
        try:
            self.db = self.dbSvc.openDatabase(dbstring,True)
        except Exception,e:
            print 'Problem opening database',e
            sys.exit(-1)
        print "Opened database",dbstring

        # -----------------------------------------------------------
        # Initialize onlineID from detector store
        self.msg.info('inlineID initialization...')
        from StoreGateBindings.Bindings import StoreGate
        detStore = StoreGate.pointer("DetectorStore")
        self.onlineID=detStore.retrieve("LArOnlineID","LArOnlineID") 

        # -----------------------------------------------------------
        # Initialize LArCabling service
        self.larCablingSvc=PyAthena.py_tool("LArCablingService")
        if self.larCablingSvc is None:
            self.msg.error('Problem retrieving LArCablingService pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.larCablingSvc.name())

        return StatusCode.Success

        
    def execute(self):
        self.msg.info('running execute...')
        self.msg.info('dumping [%s] content', self.sg.name())
        self.msg.info(self.sg.dump())


        listOfFolders=["/LAR/BadChannels/BadChannels","/LAR/BadChannels/MissingFEBs"]
        iSelectedFolder=LArBadChannelBrowserTools.ChooseOptionFromList("Folder list : ","","",listOfFolders,+1,False,{})

        if iSelectedFolder==1:
            self.dbFolderName="/LAR/BadChannels/BadChannels"
            self.folderId=BAD_CHANNEL
        else:
            self.dbFolderName="/LAR/BadChannels/MissingFEBs"
            self.folderId=MISSING_FEB


        # -----------------------------------------------------------
        # Browse database a first time to catch tags name and IOV
        listOfTags, dictIOV, self.iNbCoolChannel=self.GetTagsAndIOVFromDatabase(self.dbFolderName)

        # -----------------------------------------------------------
        # Create an objec instance for all interfaced python classes

        LArBadChannelDBTools=cppyy.makeNamespace('LArBadChannelDBTools')
        self.nspace_LArBadChannelDBTools=LArBadChannelDBTools()

        self.class_LArBadChannel=cppyy.makeClass('LArBadChannel')
        self.class_HWIdentifier=cppyy.makeClass('HWIdentifier')
        self.class_LArBadChanBitPacking=cppyy.makeClass("LArBadChanBitPacking")

        if self.folderId==BAD_CHANNEL:
            self.dbBrowserBadChan=LArDBFolderBrowser_BadChan.LArDBFolderBrowser_BadChan(self.nspace_LArBadChannelDBTools,
                                                                                        self.class_LArBadChanBitPacking,
                                                                                        self.class_LArBadChannel,
                                                                                        self.class_HWIdentifier,
                                                                                        self.onlineID, self.larCablingSvc,
                                                                                        self.msg
                                                                                        )
            
        elif self.folderId==MISSING_FEB:
            self.dbBrowserMissingFeb=LArDBFolderBrowser_MissingFEBs.LArDBFolderBrowser_MissingFEBs(self.nspace_LArBadChannelDBTools,
                                                                                                   self.class_LArBadChanBitPacking,
                                                                                                   self.class_LArBadChannel,
                                                                                                   self.class_HWIdentifier,
                                                                                                   self.onlineID, self.larCablingSvc,
                                                                                                   )


        # -----------------------------------------------------------
        # Select the tag 
        iSelectedTag=LArBadChannelBrowserTools.ChooseOptionFromList("Tag list : ","","",listOfTags,+1,False,{})
        if iSelectedTag<1: return
        self.selectedTag=listOfTags[iSelectedTag-1]

        # -----------------------------------------------------------
        # Construct IOV list based on selected tag
        self.tagIOVDict={}
        for iChannel in range(0,self.iNbCoolChannel):
            self.tagIOVDict[iChannel]=dictIOV[self.selectedTag][iChannel][-1][0]

        # -----------------------------------------------------------
        # Show database content

        self.channelNameDict=self.ReadObjectsFromDatabase(self.dbFolderName,self.selectedTag,self.tagIOVDict)
        if self.folderId==BAD_CHANNEL:
            self.dbBrowserBadChan.BadChan_SetChannelNameDict(self.channelNameDict)
#        sMessage="IOV : "+str(self.tagIOVDict)+"  /  channel : "+str(self.channelNameDict)
#        LArBadChannelBrowserTools.TypeEnterToContinue(sMessage)


        # -------------------------------------------------------------------------------------
        #
        # LOOP OVER COOL CHANNEL MODIFICATIONS
        #
        # -------------------------------------------------------------------------------------

        bEndOfCoolChannelModification=False
        while not bEndOfCoolChannelModification:

            # -----------------------------------------------------------
            # MISSING_FEB : go to correction menu
            if self.folderId==MISSING_FEB:#

                coolChan=0
                self.dbBrowserMissingFeb.MissingFeb_DisplayMissingFeb(coolChan)
                endOfModif=self.dbBrowserMissingFeb.MissingFeb_ModifyMissingFeb(coolChan)

                if endOfModif=='q': 
                    sDBName=os.environ["PWD"]+"/MissingFebUpdate.db"
                    if os.path.isfile(sDBName): os.remove(sDBName)        
                    dbstring="sqlite://;schema="+sDBName+";dbname=BADCHAN"
                    self.dbBrowserMissingFeb.MissingFeb_SaveMissingFebCorrectionsToDatabase(dbstring,self.dbSvc,self.dbFolderName,self.selectedTag)
                    self.dbBrowserMissingFeb.MissingFeb_CheckSavedDatabaseContent(dbstring,self.dbSvc,self.dbFolderName,self.selectedTag)

                return

            # -----------------------------------------------------------
            # Select Cool channel

            sCoolChannelMenuDict=[("s","(summary)"),("a","(abort)"),("q","(save and quit)")]
            listCoolChannel=[]
            for i in range(0,self.iNbCoolChannel):
                listCoolChannel.append(self.channelNameDict[i]+" (IOV "+str(self.tagIOVDict[i])+")")
            repCoolChan=LArBadChannelBrowserTools.ChooseOptionFromList("Cool channel list : ","","",listCoolChannel,
                                                                       0,False,sCoolChannelMenuDict)
            
            if repCoolChan=="a":
                iAbortConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to quit ? ")
                if iAbortConfirmation==1: return
            elif repCoolChan=="q":
                print " SAUVEGARDE FINALE"
                if self.folderId==BAD_CHANNEL:
                    sDBName=os.environ["PWD"]+"/BadChannelUpdate.db"
                    if os.path.isfile(sDBName): os.remove(sDBName)        
                    dbstring="sqlite://;schema="+sDBName+";dbname=BADCHAN"
                    self.dbBrowserBadChan.BadChan_SaveBadChannelCorrectionsToDatabase(dbstring,self.dbSvc,self.dbFolderName,self.selectedTag)
                    self.dbBrowserBadChan.BadChan_CheckSavedDatabaseContent(dbstring,self.dbSvc,self.dbFolderName,self.selectedTag)

                bEndOfCoolChannelModification=True
            elif repCoolChan=="s":
                for i in range(0,self.iNbCoolChannel):
                    self.dbBrowserBadChan.BadChan_ShowBadChannelCorrectionsSummary(i)
            else:
                # -----------------------------------------------------------
                # Display selected Cool channel 
                coolChan=int(repCoolChan)
                if self.folderId==BAD_CHANNEL:
                    self.dbBrowserBadChan.BadChan_DisplayBadChanEntry(coolChan)
                    self.dbBrowserBadChan.BadChan_ModifyLarBadChannel(coolChan)


    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success

            
    def GetTagsAndIOVFromDatabase(self,folderName):
        """ First database browsing => retrieve tag names, IOVs and number of channels """
        
        # Get Folder
        try:
            f = self.db.getFolder(folderName)
            print "Analysing Folder " + str(folderName)
        except:
            print "Skipping " + str(folderName)
            return

        # get tags
        tags  = f.listTags()

        iMaxChannelNumber=0
        sIOVBeginEnd={}
        for tag in tags:

            nobjs = f.countObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all(),
                                    tag)
      
            objs = f.browseObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all(),
                                    tag)
            iObjet = 0
            while objs.hasNext():
                obj = objs.next()

                if obj.channelId()>iMaxChannelNumber: iMaxChannelNumber=obj.channelId()

                keyTag=tag
                if not sIOVBeginEnd.has_key(keyTag): sIOVBeginEnd[keyTag]={}

                keyChan=obj.channelId()
                if not sIOVBeginEnd[keyTag].has_key(keyChan): sIOVBeginEnd[keyTag][keyChan]=[]

                sIOVBeginEnd[keyTag][keyChan].append((obj.since()>>32%0x100000000,obj.until()>>32%0x100000000))
#                print "IOV : ",obj.channelId()," :  ",obj.since()>>32," ",obj.until()>>32
                
            objs.close()

                                            
        return (tags,sIOVBeginEnd,iMaxChannelNumber+1)


    def ReadObjectsFromDatabase(self,folderName,tagName,tagIOVNumber):
        """ Second database browsing => read cool channel corresponding to selected tag """

        channelNameDict={}

        # Get Folder
        try:
            f = self.db.getFolder(folderName)
            print "Analysing Folder " + str(folderName)
        except:
            print "Skipping " + str(folderName)
            return

        # get tags
        tags  = f.listTags()
        
        for tag in tags:

            if tag!=tagName: continue

            print "-> tag : ",tag
               
            nobjs = f.countObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all(),
                                    tag)
      
            objs = f.browseObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all(),
                                    tag)
            iObjet = 0
            while objs.hasNext():
                obj = objs.next()

                # Select cool channel vs IOV
                if (obj.since()>>32%0x100000000)==tagIOVNumber[obj.channelId()]:

                    print "Found object", iObjet,
                    print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
                    print "until [r,l]: [", obj.until() >> 32,',',obj.until()%0x100000000,']',
                    print "payload", obj.payload(),
                    print "chan",obj.channelId()

                    payload=obj.payload()

                    if self.folderId==BAD_CHANNEL:
                        sChannelKey=obj.channelId()
                        sChannelName=self.dbBrowserBadChan.BadChan_SetBadChannelDataFromPayload(sChannelKey,payload)
                    elif self.folderId==MISSING_FEB:
                        sChannelKey=obj.channelId()
                        sChannelName=self.dbBrowserMissingFeb.MissingFeb_SetBadChannelDataFromPayload(sChannelKey,payload)

                    print sChannelKey,"  ",sChannelName
                    channelNameDict[sChannelKey]=sChannelName
                    print str(channelNameDict)

                iObjet=iObjet+1

            objs.close()

        return channelNameDict




    def DumpDatabasePayloadObjects(self,dbSvc,dbName,ff):

        payloadDict={}
        
        try:
            dbase = dbSvc.openDatabase(dbName,False)
        except Exception,e:
            print 'Problem opening database',e
            sys.exit(-1)
        print "Opened database",dbName

        # Loop over folders
        folders = dbase.listAllNodes()
        for ff in folders:
            # Get Folder
            try:
                f = dbase.getFolder(ff)
                print "Analysing Folder " + str(ff)
            except:
                print "Skipping " + str(ff)
                continue
        
            # get tags
            tags  = f.listTags()

            # SES
            if tags.size()==0: tags.push_back("notag")


            print "for tags ",
            for tag in tags: print tag


            for tag in tags:
                
                nobjs = f.countObjects( cool.ValidityKeyMin,
                                        cool.ValidityKeyMax,
                                        cool.ChannelSelection.all())
                
                print "number of objects", nobjs
            
                objs = f.browseObjects( cool.ValidityKeyMin,
                                        cool.ValidityKeyMax,
                                        cool.ChannelSelection.all())
                i = 0
                while objs.hasNext():
                    obj = objs.next()
                    print "Found object", i,
                    print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
                    print "until [r,l]: [", obj.until() >> 32,',',obj.until()%0x100000000,']',
                    print "payload", obj.payload(),
                    print "chan",obj.channelId() 
                    i += 1

#                    payloadDict[obj.channelId()]=obj.payload()
                
                objs.close()

        dbase.closeDatabase()

        return payloadDict

