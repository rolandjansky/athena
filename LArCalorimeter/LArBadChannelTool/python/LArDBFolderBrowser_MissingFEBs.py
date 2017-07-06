# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
import sys
import os
from PyCool import cool,coral

import LArBadChannelBrowserTools

import cppyy

STATUS_INIT="INIT"
STATUS_NEW="NEW"
STATUS_REMOVED="DEL"
STATUS_MODIFIED="MOD"

BAD_CHANNEL=0
MISSING_FEB=1

STATUS_NOT_MISSING_FEB="0K"
STATUS_MISSING_FEB="missing"

class LArDBFolderBrowser_MissingFEBs():
    
    def __init__(self,nspace_LArBadChannelDBTools,class_LArBadChanBitPacking,class_LArBadChannel,class_HWIdentifier,onLineID,larCablingSvc):

        self.nspace_LArBadChannelDBTools=nspace_LArBadChannelDBTools 
        self.class_LArBadChanBitPacking=class_LArBadChanBitPacking
        self.class_LArBadChannel=class_LArBadChannel
        self.class_HWIdentifier=class_HWIdentifier
        self.onlineID = onLineID
        self.larCablingSvc = larCablingSvc
        
        self.dict_vectMissingFebHWid={}
        self.dict_vectMissingFebHWid_Status={}
        self.dict_vectMissingFebHWid_Init={}

        return
    

    def MissingFeb_SetBadChannelDataFromPayload(self,coolChan,payload):

        # Get components of payload object
        channelSize=payload['ChannelSize']
        endianness=payload['Endianness']
        version=payload['Version']
        blob=payload['Blob']

        # Decode blob content
        vect_MissingFebHWid=self.nspace_LArBadChannelDBTools.decodeFebBlob(blob,channelSize,endianness,version,
                                                                           self.nspace_LArBadChannelDBTools.getDefaultMsgStream())

        
        print vect_MissingFebHWid.size()
        for s in vect_MissingFebHWid: print s,
        print ""
        
        # Add entry in dictionnary
        sChannelKey=coolChan
        if not self.dict_vectMissingFebHWid.has_key(sChannelKey):
            self.dict_vectMissingFebHWid[sChannelKey]={}
            
        # Transform vect_BadChanNetry into python dictionnary
        sChannelDict,sChannelDict_Status=self.MissingFeb_TransformEntryIntoDictionnary(vect_MissingFebHWid)
                        
        self.dict_vectMissingFebHWid[sChannelKey]={}
        self.dict_vectMissingFebHWid_Init[sChannelKey]={}
        for key in sChannelDict.keys():
            self.dict_vectMissingFebHWid[sChannelKey][key]=sChannelDict[key]
            self.dict_vectMissingFebHWid_Init[sChannelKey][key]=sChannelDict[key]
        self.dict_vectMissingFebHWid_Status[sChannelKey]=sChannelDict_Status

        return ("None")



    def MissingFeb_DisplayMissingFeb(self,coolChan):
        """   """

        iChanCmpt=0
        listHWidKeys=[x for x in self.dict_vectMissingFebHWid[coolChan].keys()]
        listHWidKeys.sort()

        print ""
        for sHWid in listHWidKeys:

            if self.dict_vectMissingFebHWid_Init[coolChan].has_key(sHWid):
                sChanName,sStatus,sValueInit=self.dict_vectMissingFebHWid_Init[coolChan][sHWid]
                sPrefix=""
                if self.dict_vectMissingFebHWid_Status[coolChan][sHWid]!=STATUS_INIT:sPrefix="->"
                self.MissingFeb_DisplayMissingFebLine(sPrefix,iChanCmpt,sHWid,sChanName,sStatus,"")

            if self.dict_vectMissingFebHWid_Status[coolChan][sHWid]!=STATUS_INIT:
                sChanName,sStatus,sValueInit=self.dict_vectMissingFebHWid[coolChan][sHWid]
                channelStatus=self.dict_vectMissingFebHWid_Status[coolChan][sHWid]
                self.MissingFeb_DisplayMissingFebLine(channelStatus,iChanCmpt,sHWid,sChanName,sStatus,"")

            iChanCmpt += 1

    def MissingFeb_DisplayMissingFebLine(self,sMessage,iChanCmpt,sHWid,sChanName,sStatusValue,sValueInit):

        obj_HWid=self.class_HWIdentifier()
        obj_HWid.set(sHWid)
        
        cblSvc_id = self.larCablingSvc.cnvToIdentifier(obj_HWid)
        cblSvc_string = self.onlineID.print_to_string(cblSvc_id)

        barrel_ec=self.onlineID.barrel_ec(obj_HWid)
        pos_neg=self.onlineID.pos_neg(obj_HWid)
        feedthrough=self.onlineID.feedthrough(obj_HWid)
        slot=self.onlineID.slot(obj_HWid)
        channel=self.onlineID.channel(obj_HWid)
        
        if sMessage=="":
            print "    %5d :  %-10s  %1d %1d %2d %2d %3d %40s   %s " % (iChanCmpt+1,obj_HWid.getString(),
                                                                    barrel_ec,pos_neg,feedthrough,slot,channel,
                                                                    sChanName,sStatusValue)
        else:
            print "%3s %5d :  %-10s  %1d %1d %2d %2d %3d %40s   %s %s" % (sMessage,iChanCmpt+1,obj_HWid.getString(),
                                                                        barrel_ec,pos_neg,feedthrough,slot,channel,
                                                                        sChanName,sStatusValue,sValueInit)
            
    def MissingFeb_ShowMissingFebCorrectionsSummary(self,coolChan):

        listHWidKeys=[x for x in self.dict_vectMissingFebHWid[coolChan].keys()]
        listHWidKeys.sort()

        print ""
        print "Correction summary : "
        iNbCorrection=0
        for index,sHWid in enumerate(listHWidKeys):
            if self.dict_vectMissingFebHWid_Status[coolChan][sHWid]!=STATUS_INIT:
                sChanName,badChan_word,sValueInit=self.dict_vectMissingFebHWid[coolChan][sHWid]
                channelStatus=self.dict_vectMissingFebHWid_Status[coolChan][sHWid]
                self.MissingFeb_DisplayMissingFebLine(channelStatus,index,sHWid,sChanName,badChan_word,"")
                iNbCorrection += 1

        if iNbCorrection==0: print"-"


    def MissingFeb_TransformEntryIntoDictionnary(self,vMissingFEBHWid):
        """ Transform the missing FEB decoded from blob object into a python dictionnary """
        
        print "--------------------------- Blob SIZE : ",vMissingFEBHWid.size()
        iNbMissingFEB=vMissingFEBHWid.size()

        sChannelDict={}
        sChannelDict_Status={}
        for i in range(0,iNbMissingFEB):

            obj_HWid=vMissingFEBHWid.at(i)
            sHexaString=obj_HWid.getString()

            sChannelName=self.onlineID.channel_name(vMissingFEBHWid.at(i))
            sInitialValues=sHexaString+" "+STATUS_MISSING_FEB
            sChannelDict[sHexaString]=(sChannelName,STATUS_MISSING_FEB,sInitialValues)
            sChannelDict_Status[sHexaString]=STATUS_INIT

        return (sChannelDict,sChannelDict_Status)


    def MissingFeb_ModifyMissingFeb(self,coolChan):


        listHWidKeys=[x for x in self.dict_vectMissingFebHWid[coolChan].keys()]
        listHWidKeys.sort()
        iNbBadChannel=len(self.dict_vectMissingFebHWid[coolChan].keys())

        bEndOfCorrection=False
        while not bEndOfCorrection:

            print ""
            print ".. To add a missing feb    : enter barrel_ec pos_neg feedthrough slot "
            print ".. To remove a missing feb : enter -(channel index) "
            print ".. Other :   s (summary)  /  r (refresh list)  /  a (abort)  / q (save and quit)      .. > ",
            tty = open("/dev/tty", "r+")
            rep=tty.readline()
            rep=rep.strip()
               
            bReadableAnswer=True
            barrel_ec=-1
            pos_neg=-1
            feedthrough=-1
            slot=-1
            channel=-1
            iSelectedIndex=0
            sTxtAnswer=""
            iCombinationAnswer=0
            try:
                barrel_ec,pos_neg,feedthrough,slot=rep.split(' ')
                channel=0
                bReadableAnswer=True
                iCombinationAnswer=1
            except: 
                if rep=="a": sTxtAnswer="abort"
                elif rep=="r": sTxtAnswer="refresh"
                elif rep=="s": sTxtAnswer="summary"
                elif rep=="q": sTxtAnswer="save-quit"
                else:
                    try:
                        iSelection=int(rep)
                        if iSelection in range(1,iNbBadChannel+1): iSelectedIndex=iSelection
                        if iSelection in range(-iNbBadChannel-1,0): iSelectedIndex=iSelection
                        if iSelectedIndex==-99999: bReadableAnswer=False
                    except:
                        iSelectedIndex=0
                        bReadableAnswer=False
                        continue

            if bReadableAnswer==False:
                print "could not decode answer... "
                bEndOfCorrection=False
                continue

            # Abort answer
            if sTxtAnswer=="abort":
                iAbortConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to quit ? ")
                if iAbortConfirmation==1: return 'a'
                bEndOfCorrection=False

            # Refresh answer
            if sTxtAnswer=="refresh":
                self.MissingFeb_DisplayMissingFeb(coolChan)
                bEndOfCorrection=False

            # Summary answer
            if sTxtAnswer=="summary":
                self.MissingFeb_ShowMissingFebCorrectionsSummary(coolChan)
                bEndOfCorrection=False

            # Refresh answer
            if sTxtAnswer=="save-quit":
                return 'q'


            # Channel selected by negative index => to remove
            if iSelectedIndex<0:
                sHWid=listHWidKeys[-iSelectedIndex-1]
                sChanName,sFebstatus,sValueInit=self.dict_vectMissingFebHWid[coolChan][sHWid]
                iDeleteConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to delete "+sChanName+" ? ")
                if iDeleteConfirmation==1:
                    self.dict_vectMissingFebHWid_Status[coolChan][sHWid]=STATUS_REMOVED
                    self.dict_vectMissingFebHWid[coolChan][sHWid]=(sChanName,STATUS_NOT_MISSING_FEB,sValueInit)
                bEndOfCorrection=False
                
            # Channel selected by index or parameters
            if iSelectedIndex>0 or iCombinationAnswer==1:

                if iSelectedIndex>0:
                    sHWid=listHWidKeys[iSelectedIndex-1]
                    sChanName,sFebStatus,sValueInit=self.dict_vectMissingFebHWid[coolChan][sHWid]
                else:
                    iRes,sHWid,sChanName,sFebStatus=self.MissingFeb_GetChannelHWIdentifierAndStatus(barrel_ec,pos_neg,feedthrough,slot,channel)
                    if iRes==0 :
                        print "FEB already defined as missing"
                        sValueInit=self.dict_vectMissingFebHWid[coolChan][sHWid][2]
                    else:
                        sValueInit="None"
                        self.dict_vectMissingFebHWid_Status[coolChan][sHWid]=STATUS_NEW
                        self.dict_vectMissingFebHWid[coolChan][sHWid]=(sChanName,STATUS_MISSING_FEB,sValueInit)
                bEndOfCorrection=False

            # Display correction summary
            self.MissingFeb_ShowMissingFebCorrectionsSummary(coolChan)


        return StatusCode.Success


    def MissingFeb_GetChannelHWIdentifierAndStatus(self,barrel_ec,pos_neg,feedthrough,slot,channel):
        """ Get channel HW identifier and its status """
        
        sid = self.onlineID.channel_Id(int(barrel_ec),int(pos_neg),int(feedthrough),int(slot),int(channel))
        print str(barrel_ec)+" "+str(pos_neg)+" "+str(feedthrough)+" "+str(slot)+" "+str(channel)+"  =>  ",sid.getString()

        # Check if HW identifier is valid
        bValidWHidentifier=False
        if self.onlineID.isEMBchannel(sid): bValidWHidentifier=True
        if self.onlineID.isEMECchannel(sid): bValidWHidentifier=True
        if self.onlineID.isHECchannel(sid): bValidWHidentifier=True
        if self.onlineID.isFCALchannel(sid): bValidWHidentifier=True
        if bValidWHidentifier==False: return (-1,sid.getString(),"",0)

        for key in self.dict_vectMissingFebHWid.keys():
            for sHWid in self.dict_vectMissingFebHWid[key].keys():
                sChannelName,sFebStatus,x=self.dict_vectMissingFebHWid[key][sHWid]
                if sid.getString()==sHWid:            
                    return (0,sid.getString(),sChannelName,sFebStatus)
                    
        return (1,sid.getString(),sChannelName,STATUS_MISSING_FEB)



    def MissingFeb_SaveMissingFebCorrectionsToDatabase(self,dbstring,dbSvc,dbFolderName,selectedTag):

        listKeys=self.dict_vectMissingFebHWid.keys()
        listKeys.sort()

        # Loop over cool channels
        bStoreNewCoolChannels=False
        bNewDBCreated=False

        vect_MissingFebHWid=cppyy.gbl.std.vector('HWIdentifier')()

        coolChan=0
        listHWidKeys=[x for x in self.dict_vectMissingFebHWid[coolChan].keys()]
        listHWidKeys.sort()
        iNbCorrection=0
        for key in listHWidKeys:

            sHWid=key
            if self.dict_vectMissingFebHWid_Status[coolChan][sHWid]==STATUS_INIT:
                sStatusFeb=self.dict_vectMissingFebHWid_Init[coolChan][sHWid][1]
            elif self.dict_vectMissingFebHWid_Status[coolChan][sHWid]==STATUS_MODIFIED or self.dict_vectMissingFebHWid_Status[coolChan][sHWid]==STATUS_NEW:
                iNbCorrection += 1
                sStatusFeb=self.dict_vectMissingFebHWid[coolChan][sHWid][1]
            elif self.dict_vectMissingFebHWid_Status[coolChan][sHWid]==STATUS_REMOVED:
                iNbCorrection += 1
                continue

            obj_HWid=self.class_HWIdentifier()
            obj_HWid.set(sHWid)

            vect_MissingFebHWid.push_back(obj_HWid)

        # if correction were made => store MissingFebHWid vector
        if iNbCorrection>0:
            bStoreNewCoolChannels=True
        else:
            print "No correcton to save"
            return

        # Create object based on new LArBadChannelState (via LArBadChannelDBTools python interface) 
        attrListSpec=cppyy.gbl.coral.AttributeListSpecification()
        athenaAttrList=cppyy.gbl.AthenaAttributeList()

        attrListSpec=self.nspace_LArBadChannelDBTools.createCoolSpec()
        athenaAttrList=self.nspace_LArBadChannelDBTools.createFebPayload(vect_MissingFebHWid)

        # new DB creation
        import os
        try:
            dbSave = dbSvc.createDatabase(dbstring)
        except Exception,e:
            print 'Problem opening database',e
            sys.exit(-1)
        print "Opened database",dbstring
            
        desc='<timeStamp>run-event</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'

        # Create cool spec from AttributeListSpec
        coolSpec=cool.RecordSpecification()
        for iElemt in range(0,attrListSpec.size()):
            attrSpec=attrListSpec[iElemt]
            typeName=attrSpec.typeName()
            if typeName=="unsigned int":
                coolSpec.extend(attrSpec.name(),cool.StorageType.UInt32)
            elif typeName=="blob":
                coolSpec.extend(attrSpec.name(),cool.StorageType.Blob64k)
            else:
                print "Undefined cool.StorageType "+typeName

        myfolder=dbSave.createFolder(dbFolderName, coolSpec, desc, cool.FolderVersioning.MULTI_VERSION,True)

        import string
        IOVBeginEnd=["90","9999999"]
        beginRun = string.atoi(IOVBeginEnd[0]) << 32
        endRun   = string.atoi(IOVBeginEnd[1]) << 32

        # Create cool payload from AthenaAttributeList payload
        coolPayload=cool.Record(coolSpec)
        for iElemt in range(0,attrListSpec.size()):
            attrSpec=attrListSpec[iElemt]
            coolPayload[attrSpec.name()]=athenaAttrList[attrSpec.name()]

        # Store cool object to folder
            myfolder.storeObject(beginRun,endRun,coolPayload,coolChan,selectedTag)
            
        # Close new databse
        dbSave.closeDatabase()



    def MissingFeb_CheckSavedDatabaseContent(self,dbName,dbSvc,dbFolderName,selectedTag):

        # Dump new database content to screen
        try:
            dbase = dbSvc.openDatabase(dbName,False)
        except Exception,e:
            print 'Problem opening database',e
            sys.exit(-1)
        print "Opened database",dbName

        # Get Folder
        try:
            f = dbase.getFolder(dbFolderName)
            print "Analysing Folder " + str(dbFolderName)
        except:
            print "Skipping " + str(dbFolderName)
            return

        # get tags
        tags  = f.listTags()

        # SES
        if tags.size()==0: tags.push_back("notag")

        print "for tags ",
        for tag in tags: print tag

        bTagFound=False
        bSavingProcessError=False

        for tag in tags:

            if tag!=selectedTag: continue

            bTagFound=True

            nobjs = f.countObjects( cool.ValidityKeyMin,cool.ValidityKeyMax,cool.ChannelSelection.all())
            objs = f.browseObjects( cool.ValidityKeyMin,cool.ValidityKeyMax,cool.ChannelSelection.all())
            i = 0
            while objs.hasNext():
                obj = objs.next()
                print "Found object", i,
                print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
                print "until [r,l]: [", obj.until() >> 32,',',obj.until()%0x100000000,']',
                print "payload", obj.payload(),
                print "chan",obj.channelId() 

                # Get components of payload object
                payload=obj.payload()
                channelSize=payload['ChannelSize']
                endianness=payload['Endianness']
                version=payload['Version']
                blob=payload['Blob']

                # Decode blob content
                vect_MissingFeb=self.nspace_LArBadChannelDBTools.decodeFebBlob(blob,channelSize,endianness,version,
                                                                               self.nspace_LArBadChannelDBTools.getDefaultMsgStream())


                # Transform vect_BadChanNetry into python dictionnary
                sChannelDict,sChannelDict_Status=self.MissingFeb_TransformEntryIntoDictionnary(vect_MissingFeb)

                sChannelKey=obj.channelId()
                HWidChecked={}
                for key in sChannelDict.keys(): HWidChecked[key]=0

                print " -> SQlite database content vs initial data : channel ", sChannelKey

                # Check all the saved data vs initial data
                listHWidKeys=[x for x in sChannelDict.keys()]
                for x in self.dict_vectMissingFebHWid_Status[sChannelKey].keys():
                    if x not in listHWidKeys: listHWidKeys.append(x)
                listHWidKeys.sort()
                iChanCmpt=0
                for keyHWid in listHWidKeys:

                    try:
                        sChanName,sFebStatus,sValueInit=sChannelDict[keyHWid]
                    except:
                        sChanName="UNDEFINED"
                        sFebStatus=0

                    sSuffix=" checked"
                    if self.dict_vectMissingFebHWid_Status[sChannelKey][keyHWid]==STATUS_INIT:
                        if sChannelDict[keyHWid][1]!=self.dict_vectMissingFebHWid_Init[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix="  no modification was done"+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix="  " 
                        self.MissingFeb_DisplayMissingFebLine(sPrefix,iChanCmpt,keyHWid,sChanName,sFebStatus,sSuffix)

                    if self.dict_vectMissingFebHWid_Status[sChannelKey][keyHWid]==STATUS_REMOVED:
                        if sChannelDict.has_key(keyHWid):
                            sPrefix="ERR"
                            sSuffix=" deletion not taken into accout"+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_REMOVED
                        sChanName,sFebStatus,sValue_Init=self.dict_vectMissingFebHWid[sChannelKey][keyHWid]
                        self.MissingFeb_DisplayMissingFebLine(sPrefix,iChanCmpt,keyHWid,sChanName,sFebStatus,sSuffix)

                    if self.dict_vectMissingFebHWid_Status[sChannelKey][keyHWid]==STATUS_NEW:
                        if sChannelDict[keyHWid][1]!=self.dict_vectMissingFebHWid[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix=" error while saving new status "+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_NEW
                        self.MissingFeb_DisplayMissingFebLine(sPrefix,iChanCmpt,keyHWid,sChanName,sFebStatus,sSuffix)

                    if self.dict_vectMissingFebHWid_Status[sChannelKey][keyHWid]==STATUS_MODIFIED:
                        if sChannelDict[keyHWid][1]!=self.dict_vectMissingFebHWid[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix=" error while saving modified status "+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_MODIFIED
                        self.MissingFeb_DisplayMissingFebLine(sPrefix,iChanCmpt,keyHWid,sChanName,sFebStatus,sSuffix)

                    HWidChecked[keyHWid]=1
                    iChanCmpt += 1

                for key in self.dict_vectMissingFebHWid_Status[sChannelKey].keys():
                    if not HWidChecked.has_key(key) and self.dict_vectMissingFebHWid_Status[sChannelKey][key]!=STATUS_REMOVED:
                        print "ERROR : initial ",key," has not been saved"
                    elif HWidChecked.has_key(key) and HWidChecked[key]==0:
                        print "ERROR : ",key," has not been checked"

                i += 1


            objs.close()

        if bTagFound==False:
            print "ERROR : tag "+selectedTag+" not found in saved SQlite file"
            
        if bSavingProcessError==True:
            print "ERROR : found while making comparison between corrected and saved datas" 


        dbase.closeDatabase()


            
    
