# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
import sys
from PyCool import cool,coral

from GaudiKernel.Constants import *
import logging

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

class LArDBFolderBrowser_BadChan():
    """My first python algorithm ( and most probably not my last... ;-)) )
    """
    
    def __init__(self,nspace_LArBadChannelDBTools,class_LArBadChanBitPacking,class_LArBadChannel,class_HWIdentifier,
                 onLineID,larCablingSvc,msg):

        self.nspace_LArBadChannelDBTools=nspace_LArBadChannelDBTools 
        self.class_LArBadChanBitPacking=class_LArBadChanBitPacking
        self.class_LArBadChannel=class_LArBadChannel
        self.class_HWIdentifier=class_HWIdentifier
        self.onlineID = onLineID
        self.larCablingSvc = larCablingSvc
        self.msg=msg

        self.dict_vectBadChanEntry={}
        self.dict_vectBadChanEntry_Status={}
        self.dict_vectBadChanEntry_Init={}
        self.channelNameDict={}
        self.BadChan_GetBadChannelProblemType()

        return

    def BadChan_SetChannelNameDict(self,channelNameDict):

        for key in channelNameDict.keys():
            self.channelNameDict[key]=channelNameDict[key]

        return

    
    def BadChan_SetBadChannelDataFromPayload(self,coolChan,payload):


        # Get components of payload object
        channelSize=payload['ChannelSize']
        statusWordSize=payload['StatusWordSize']
        endianness=payload['Endianness']
        version=payload['Version']
        blob=payload['Blob']

        
        # Decode blob content
        vect_BadChanEntry=self.nspace_LArBadChannelDBTools.decodeBlob(blob,channelSize,statusWordSize,endianness,version,
                                                                      self.nspace_LArBadChannelDBTools.getDefaultMsgStream())
        
        # Add entry in dictionnary
        sChannelKey=coolChan
        if not self.dict_vectBadChanEntry.has_key(sChannelKey):
            self.dict_vectBadChanEntry[sChannelKey]={}
            
        # Get channel name from first vect_BadChanEntry element 
        obj_HWid=vect_BadChanEntry.at(0).first
        
        # Transform vect_BadChanNetry into python dictionnary
        sChannelDict,sChannelDict_Status=self.BadChan_TransformEntryIntoDictionnary(vect_BadChanEntry)
        self.dict_vectBadChanEntry[sChannelKey]={}
        self.dict_vectBadChanEntry_Init[sChannelKey]={}
        for key in sChannelDict.keys():
            self.dict_vectBadChanEntry[sChannelKey][key]=sChannelDict[key]
            self.dict_vectBadChanEntry_Init[sChannelKey][key]=sChannelDict[key]
        self.dict_vectBadChanEntry_Status[sChannelKey]=sChannelDict_Status

        return (self.BadChan_GetCoolChannelNameFromHWIdentifier(obj_HWid))
    

    def BadChan_DisplayBadChanEntry(self,coolChan):
        """   """

        iChanCmpt=0
        listHWidKeys=[x for x in self.dict_vectBadChanEntry[coolChan].keys()]
        listHWidKeys.sort()

        print ""
        for sHWid in listHWidKeys:

            if self.dict_vectBadChanEntry_Init[coolChan].has_key(sHWid):
                sChanName,badChan_word,sValueInit=self.dict_vectBadChanEntry_Init[coolChan][sHWid]
                sPrefix=""
                if self.dict_vectBadChanEntry_Status[coolChan][sHWid]!=STATUS_INIT:sPrefix="->"
                self.BadChan_DisplayBadChanEntryLine(sPrefix,iChanCmpt,sHWid,sChanName,badChan_word,sValueInit)

            if self.dict_vectBadChanEntry_Status[coolChan][sHWid]!=STATUS_INIT:
                sChanName,badChan_word,sValueInit=self.dict_vectBadChanEntry[coolChan][sHWid]
                channelStatus=self.dict_vectBadChanEntry_Status[coolChan][sHWid]
                self.BadChan_DisplayBadChanEntryLine(channelStatus,iChanCmpt,sHWid,sChanName,badChan_word,sValueInit)

            iChanCmpt += 1


    def BadChan_DisplayBadChanEntryLine(self,sMessage,iChanCmpt,sHWid,sChanName,badChan_word,sValueInit):

        obj_HWid=self.class_HWIdentifier()
        obj_HWid.set(sHWid)
        
        cblSvc_id = self.larCablingSvc.cnvToIdentifier(obj_HWid)
        cblSvc_string = self.onlineID.print_to_string(cblSvc_id)

        barrel_ec=self.onlineID.barrel_ec(obj_HWid)
        pos_neg=self.onlineID.pos_neg(obj_HWid)
        feedthrough=self.onlineID.feedthrough(obj_HWid)
        slot=self.onlineID.slot(obj_HWid)
        channel=self.onlineID.channel(obj_HWid)
        
        larBadChannel=self.class_LArBadChannel(badChan_word)
        sStatusString=self.BadChan_GetBadChannelPb_String(larBadChannel)

        if sMessage=="":
            print "    %5d :  %-10s  %1d %1d %2d %2d %3d %40s   %s " % (iChanCmpt+1,obj_HWid.getString(),
                                                                    barrel_ec,pos_neg,feedthrough,slot,channel,
                                                                    sChanName,sStatusString)
        else:
            print "%3s %5d :  %-10s  %1d %1d %2d %2d %3d %40s   %s  %s" % (sMessage,iChanCmpt+1,obj_HWid.getString(),
                                                                        barrel_ec,pos_neg,feedthrough,slot,channel,
                                                                        sChanName,sStatusString,sValueInit)

            
            #            print "%5d :  %-10s  %1d %1d %2d %2d %3d %40s %s  //  %s" % (iChanCmpt,obj_HWid.getString(),
        #                                                                      barrel_ec,pos_neg,feedthrough,slot,channel,
        #                                                                      sChanName,sStatusString,sValueInit)

    

    def BadChan_DisplayBadChanEntryVector_FromAttrList(self,attrList):

        blob = attrList['Blob']
        channelSize = attrList['ChannelSize']
        statusWordSize = attrList['StatusWordSize']
        endianness = attrList['Endianness']
        version =  attrList['Version']
        vect_BadChanEntry=self.nspace_LArBadChannelDBTools.decodeBlob(blob,channelSize,statusWordSize,endianness,version,
                                                                      self.nspace_LArBadChannelDBTools.getDefaultMsgStream())
        print "BadChanEntry vector size : ",vect_BadChanEntry.size()
        self.BadChan_DisplayBadChanEntryVector(vect_BadChanEntry)

    
    def BadChan_DisplayBadChanEntryVector(self,vBadChanEntry):
        """   """

        sChannelDict,sChannelDict_Status=self.BadChan_TransformEntryIntoDictionnary(vBadChanEntry)
        listHWidKeys=[x for x in sChannelDict.keys()]
        listHWidKeys.sort()

        iCmpt=0
        for sHWid in listHWidKeys:

            (sChannelName,larBadChannelStatus,sStatusValue)=sChannelDict[sHWid]
            self.BadChan_DisplayBadChanEntryLine("",iCmpt,sHWid,sChannelName,larBadChannelStatus,sStatusValue)
            iCmpt +=1


    def BadChan_TransformEntryIntoDictionnary(self,vBadChanEntry):
        """ Transform the BadChanEntry decoded from blob object into a python dictionnary """
        
        print "--------------------------- Blob SIZE : ",vBadChanEntry.size()
        iNbBadChannel=vBadChanEntry.size()

        sChannelDict={}
        sChannelDict_Status={}
        for i in range(0,iNbBadChannel):
            larBadChannel=vBadChanEntry.at(i).second
            obj_HWid=vBadChanEntry.at(i).first
            sHexaString=obj_HWid.getString()

            sChannelName=self.onlineID.channel_name(vBadChanEntry.at(i).first)
            sInitialValues=sHexaString+" "+self.BadChan_GetBadChannelPb_String(larBadChannel)
            sChannelDict[sHexaString]=(sChannelName,larBadChannel.packedData(),sInitialValues)
            sChannelDict_Status[sHexaString]=STATUS_INIT

        return (sChannelDict,sChannelDict_Status)




    def BadChan_GetChannelHWIdentifierAndStatus(self,barrel_ec,pos_neg,feedthrough,slot,channel):
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

        # Check if HW identifier already corresponds to a bad channel
        for key in self.dict_vectBadChanEntry.keys():
            for sHWid in self.dict_vectBadChanEntry[key].keys():

                sChannelName,badChan_word,x=self.dict_vectBadChanEntry[key][sHWid]
                if sid.getString()==sHWid:
                    larBadChannel=self.class_LArBadChannel(badChan_word)
                    larBadChannelStatus=self.BadChan_GetBadChannelPb_String(larBadChannel)

                    return (0,sid.getString(),sChannelName,badChan_word)

        # default : new lar bad channel status
        sChannelName=self.onlineID.channel_name(sid)
        badChan_word=0
        return (1,sid.getString(),sChannelName,badChan_word)


    def BadChan_GetBadChannelPb_String(self,larBadChannel):
        """ Get bad channel ProblemType names """

        sBadChannelPb=""
        for i in range(0,self.nbProblemType):
            if larBadChannel.statusBad(i):
                sBadChannelPb=sBadChannelPb+self.channelProblemTypeList[i]+","
        
        return sBadChannelPb


    def BadChan_GetBadChannelPb_Int(self,larBadChannel):
        """ Get bad channel ProblemType indexes """
        
        iBadChannelPb=[]
        for i in range(0,self.nbProblemType):
            if larBadChannel.statusBad(i):
                iBadChannelPb.append(i)
        
        return iBadChannelPb


    def BadChan_GetCoolChannelNameFromHWIdentifier(self,sHWid):
        """ Get channel name from HW identifier (only for text menu purpose)"""

        self.class_LArBadChannelState=cppyy.makeClass('LArBadChannelState')
        inst_larBadChannelState=self.class_LArBadChannelState()
        
        sChannelName=""
        if self.onlineID.isEMBchannel(sHWid):
            if self.onlineID.pos_neg(sHWid)==1:
                iEnumChannel=inst_larBadChannelState.EMBA
            else:
                iEnumChannel=inst_larBadChannelState.EMBC
        if self.onlineID.isEMECchannel(sHWid):
            if self.onlineID.pos_neg(sHWid)==1:
                iEnumChannel=inst_larBadChannelState.EMECA
            else:
                iEnumChannel=inst_larBadChannelState.EMECC
        if self.onlineID.isHECchannel(sHWid):
            if self.onlineID.pos_neg(sHWid)==1:
                iEnumChannel=inst_larBadChannelState.HECA
            else:
                iEnumChannel=inst_larBadChannelState.HECC
        if self.onlineID.isFCALchannel(sHWid):
            if self.onlineID.pos_neg(sHWid)==1:
                iEnumChannel=inst_larBadChannelState.FCALA
            else:
                iEnumChannel=inst_larBadChannelState.FCALC
        
        sChannelName=inst_larBadChannelState.coolChannelName(iEnumChannel)+" ("+str(iEnumChannel)+")"
        return sChannelName


    def BadChan_GetBadChannelProblemType(self):
        """ Get list of problem type defines in LArBadChanBitPacking file """

        obj_larBadChanBitPacking=self.class_LArBadChanBitPacking()

        self.nbProblemType=int(obj_larBadChanBitPacking.numberOfProblemTypes())
        
        self.channelProblemTypeList=[]
        for i in range(0,self.nbProblemType):
            self.channelProblemTypeList.append(obj_larBadChanBitPacking.stringName(i))
            
        print "ENUM ProblemType : "
        print self.channelProblemTypeList




    def BadChan_ModifyLarBadChannel(self,coolChan):


        listHWidKeys=[x for x in self.dict_vectBadChanEntry[coolChan].keys()]
        listHWidKeys.sort()
        iNbBadChannel=len(self.dict_vectBadChanEntry[coolChan].keys())

        bEndOfCorrection=False
        while not bEndOfCorrection:

            print ""
            print ".. To select a channel : enter channel index or barrel_ec pos_neg feedthrough slot channel "
            print ".. To add a channel    : enter barrel_ec pos_neg feedthrough slot channel "
            print ".. To remove a channel : enter -(channel index) "
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
                barrel_ec,pos_neg,feedthrough,slot,channel=rep.split(' ')
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
                if iAbortConfirmation==1: return
                bEndOfCorrection=False

            # Refresh answer
            if sTxtAnswer=="refresh":
                self.BadChan_DisplayBadChanEntry(coolChan)
                bEndOfCorrection=False

            # Summary answer
            if sTxtAnswer=="summary":
                self.BadChan_ShowBadChannelCorrectionsSummary(coolChan)
                bEndOfCorrection=False

            # Refresh answer
            if sTxtAnswer=="save-quit":
                bEndOfCorrection=True

            # Channel selected by negative index => to remove
            if iSelectedIndex<0:
                sHWid=listHWidKeys[-iSelectedIndex-1]
                sChanName,badChan_word,sValueInit=self.dict_vectBadChanEntry[coolChan][sHWid]
                iDeleteConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to delete "+sChanName+" ? ")
                if iDeleteConfirmation==1:
                    self.dict_vectBadChanEntry_Status[coolChan][sHWid]=STATUS_REMOVED
                self.BadChan_ShowBadChannelCorrectionsSummary(coolChan)
                bEndOfCorrection=False
                
            # Channel selected by index or parameters
            if iSelectedIndex>0 or iCombinationAnswer==1:

                bChangeHWstatus=True
                bNewHWidentifier=False
                if iSelectedIndex>0:
                    sHWid=listHWidKeys[iSelectedIndex-1]
                    sChanName,badChan_word,sValueInit=self.dict_vectBadChanEntry[coolChan][sHWid]
                else:
                    iRes,sHWid,sChanName,badChan_word=self.BadChan_GetChannelHWIdentifierAndStatus(barrel_ec,pos_neg,feedthrough,slot,channel)
                    if iRes==-1:
                        print "An error occured while computing HW identifier -> computed HW identifier does not exist"
                        bChangeHWstatus=False
                    if iRes==1:
                        bNewHWidentifier=True
                        
                if bChangeHWstatus:
                    print""
                    for i in range(0,50): print "-",
                    print""
                    for i in range(0,50): print "-",
                    print""
                    self.BadChan_DisplayBadChanEntryLine("",0,sHWid,sChanName,badChan_word,"")
                    print "      ",self.BadChan_GetCablingServiceNameFromHWIdentifier(sHWid)

                    iRes,sNewStatus=self.BadChan_GetNewChannelStatus(badChan_word)

                    print "MODIFICATION STATUS : ",badChan_word," ",sNewStatus

                    if iRes==0 and sNewStatus!=badChan_word:
                        if bNewHWidentifier==False:
                            self.dict_vectBadChanEntry_Status[coolChan][sHWid]=STATUS_MODIFIED
                            self.dict_vectBadChanEntry[coolChan][sHWid]=(sChanName,sNewStatus,sValueInit)
                        else:
                            self.dict_vectBadChanEntry_Status[coolChan][sHWid]=STATUS_NEW
                            sValueInit="None"
                            self.dict_vectBadChanEntry[coolChan][sHWid]=(sChanName,sNewStatus,sValueInit)
                            self.BadChan_DisplayBadChanEntry(coolChan)
                    elif iRes==1:
                        self.dict_vectBadChanEntry_Status[coolChan][sHWid]=STATUS_INIT
                        

                self.BadChan_ShowBadChannelCorrectionsSummary(coolChan)
                bEndOfCorrection=False


        return StatusCode.Success

    def BadChan_GetChannelHWIdentifierAndStatus(self,barrel_ec,pos_neg,feedthrough,slot,channel):
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

        
        # Check if HW identifier already corresponds to a bad channel
        for key in self.dict_vectBadChanEntry.keys():
            for sHWid in self.dict_vectBadChanEntry[key].keys():

                sChannelName,badChan_word,x=self.dict_vectBadChanEntry[key][sHWid]
                if sid.getString()==sHWid:
                    larBadChannel=self.class_LArBadChannel(badChan_word)
                    larBadChannelStatus=self.BadChan_GetBadChannelPb_String(larBadChannel)

                    return (0,sid.getString(),sChannelName,badChan_word)

        # default : new lar bad channel status
        sChannelName=self.onlineID.channel_name(sid)
        badChan_word=0
        return (1,sid.getString(),sChannelName,badChan_word)


    def BadChan_GetBadChannelPb_String(self,larBadChannel):
        """ Get bad channel ProblemType names """

        sBadChannelPb=""
        for i in range(0,self.nbProblemType):
            if larBadChannel.statusBad(i):
                sBadChannelPb=sBadChannelPb+self.channelProblemTypeList[i]+","
        
        return sBadChannelPb


    def BadChan_GetBadChannelPb_Int(self,larBadChannel):
        """ Get bad channel ProblemType indexes """
        
        iBadChannelPb=[]
        for i in range(0,self.nbProblemType):
            if larBadChannel.statusBad(i):
                iBadChannelPb.append(i)
        
        return iBadChannelPb



    def BadChan_GetBadChannelProblemType(self):
        """ Get list of problem type defines in LArBadChanBitPacking file """

        obj_larBadChanBitPacking=self.class_LArBadChanBitPacking()

        self.nbProblemType=int(obj_larBadChanBitPacking.numberOfProblemTypes())
        
        self.channelProblemTypeList=[]
        for i in range(0,self.nbProblemType):
            self.channelProblemTypeList.append(obj_larBadChanBitPacking.stringName(i))
            
        print "ENUM ProblemType : "
        print self.channelProblemTypeList


    def BadChan_GetCablingServiceNameFromHWIdentifier(self, sHWid):
        """ Get Cabling Service param from HW identifier """
        
        obj_HWid=self.class_HWIdentifier()
        obj_HWid.set(sHWid)
        
        cblSvc_id = self.larCablingSvc.cnvToIdentifier(obj_HWid)
        cblSvc_string = self.onlineID.print_to_string(cblSvc_id)

        return cblSvc_string
        

    def BadChan_ShowBadChannelCorrectionsSummary(self,coolChan):
        
        listHWidKeys=[x for x in self.dict_vectBadChanEntry[coolChan].keys()]
        listHWidKeys.sort()
#                print listHWidKeys
        print ""

        print "Correction summary : "+self.channelNameDict[coolChan]
        iNbCorrection=0
        for index,sHWid in enumerate(listHWidKeys):
            if self.dict_vectBadChanEntry_Status[coolChan][sHWid]!=STATUS_INIT:
                sChanName,badChan_word,sValueInit=self.dict_vectBadChanEntry[coolChan][sHWid]
                channelStatus=self.dict_vectBadChanEntry_Status[coolChan][sHWid]
                self.BadChan_DisplayBadChanEntryLine(channelStatus,index,sHWid,sChanName,badChan_word,"")
                iNbCorrection += 1

        if iNbCorrection==0: print"-"

    
    def BadChan_GetNewChannelStatus(self,badChan_wordInit):

        badChan_word=badChan_wordInit

        inst_larBadChanBitPacking=self.class_LArBadChanBitPacking()

        sTmp="\n"
        for index,s in enumerate(self.channelProblemTypeList):
            sTmp=sTmp+"%2d %-20s" % (index,s)
            if (index+1)%5==0: sTmp=sTmp+"\n"
        print sTmp
        print ".. to add/remove a pb  : enter index/-index or sequence of indexes"
        print ".. other               :  a : abort /  c : cancel correction / r : reset to valid /  n : next channel"
        print ""

        bEndOfStatusCorrection=False
        larBadChannel=self.class_LArBadChannel(badChan_word)
        while not bEndOfStatusCorrection:

            iProblemList=self.BadChan_GetBadChannelPb_Int(larBadChannel)
            print "Status ",self.BadChan_GetBadChannelPb_String(larBadChannel)," ",iProblemList
            print ""
            print ".. > ",
            tty = open("/dev/tty", "r+")
            rep=tty.readline()
            rep=rep.strip()

            if rep=="a":
                iAbortConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to quit ? ")
                if iAbortConfirmation==1: return (-1,badChan_wordInit)
                bEndOfStatusCorrection=False
            elif rep=="n":
                badChan_word=larBadChannel.packedData()
                return (0,badChan_word)
                bEndOfStatusCorrection=True
            elif rep=="c":
                iCancelConfirmation=LArBadChannelBrowserTools.YesNoQuestion("Are you sure you want to cancel correction ? ")
                if iCancelConfirmation==1: return (1,STATUS_INIT)
                bEndOfStatusCorrection=True
            else:
                import re
                reSplitScheme=re.compile('[ ,;]+')
                sTmp=reSplitScheme.split(rep)
                sRepSeq=[x for x in sTmp if x !='']
                
                # reset
                if "r" in sRepSeq: iProblemList=[]
                
                # Indexes
                for index in sRepSeq:
                    try:
                        iNewPb=int(index)
                        if iNewPb in range(1,self.nbProblemType) and iNewPb not in iProblemList: iProblemList.append(iNewPb)
                        if iNewPb in range(-self.nbProblemType+1,0) and -iNewPb in iProblemList: iProblemList.remove(-iNewPb)
                    except:
                        continue

                larBadChannel=self.class_LArBadChannel()
                for iPb in iProblemList:
                    inst_larBadChanBitPacking.setBit(iPb,larBadChannel)
                badChan_word=larBadChannel.packedData()


        return (-1,badChan_word)
        


    def BadChan_SaveBadChannelCorrectionsToDatabase(self,dbstring,dbSvc,dbFolderName,selectedTag):

        listKeys=self.dict_vectBadChanEntry.keys()
        listKeys.sort()

        # Create LArBadChannel object for defined BadChanEntry vector and coolChan
        self.class_LArBadChannelState=cppyy.makeClass('LArBadChannelState')
        inst_larBadChannelState=self.class_LArBadChannelState()

        # Loop over cool channels
        bStoreNewCoolChannels=False
        bNewDBCreated=False
        for coolChan in listKeys:

            vect_BadChanEntry=cppyy.gbl.std.vector('std::pair<HWIdentifier,LArBadChannel>')()
            
            listHWidKeys=[x for x in self.dict_vectBadChanEntry[coolChan].keys()]
            listHWidKeys.sort()
            iNbCorrection=0
            for key in listHWidKeys:
                
                sHWid=key
                if self.dict_vectBadChanEntry_Status[coolChan][sHWid]==STATUS_INIT:
                    badChan_word=self.dict_vectBadChanEntry_Init[coolChan][sHWid][1]
                elif self.dict_vectBadChanEntry_Status[coolChan][sHWid]==STATUS_MODIFIED or self.dict_vectBadChanEntry_Status[coolChan][sHWid]==STATUS_NEW:
                    iNbCorrection += 1
                    badChan_word=self.dict_vectBadChanEntry[coolChan][sHWid][1]
                elif self.dict_vectBadChanEntry_Status[coolChan][sHWid]==STATUS_REMOVED:
                    iNbCorrection += 1
                    continue
            
                obj_HWid=self.class_HWIdentifier()
                obj_HWid.set(sHWid)
                larBadChannel=self.class_LArBadChannel(badChan_word)

                pair_BadChanEntry=cppyy.gbl.pair('HWIdentifier,LArBadChannel')(obj_HWid, larBadChannel)
                vect_BadChanEntry.push_back(pair_BadChanEntry)

            # if correction were made => store BadChanEntry vector
            if iNbCorrection>0:
                for sEntry in vect_BadChanEntry:
                    inst_larBadChannelState.add(sEntry,coolChan)
                bStoreNewCoolChannels=True
            else:
                continue

            # Create object based on new LArBadChannelState (via LArBadChannelDBTools python interface) 
            attrListSpec=cppyy.gbl.coral.AttributeListSpecification()
            athenaAttrList=cppyy.gbl.AthenaAttributeList()
        
            attrListSpec=self.nspace_LArBadChannelDBTools.createCoolSpec()
            athenaAttrList=self.nspace_LArBadChannelDBTools.createPayload(inst_larBadChannelState.coolChannel(coolChan), attrListSpec)

            # if save DB has not been created => do it
            if bNewDBCreated==False:
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

#                myfolder=dbSave.createFolder(dbFolderName, coolSpec, desc, cool.FolderVersioning.SINGLE_VERSION,True)
                myfolder=dbSave.createFolder(dbFolderName, coolSpec, desc, cool.FolderVersioning.MULTI_VERSION,True)

                import string
                IOVBeginEnd=["90","9999999"]
                beginRun = string.atoi(IOVBeginEnd[0]) << 32
                endRun   = string.atoi(IOVBeginEnd[1]) << 32

                bNewDBCreated=True

            # Create cool payload from AthenaAttributeList payload
            coolPayload=cool.Record(coolSpec)
            for iElemt in range(0,attrListSpec.size()):
                attrSpec=attrListSpec[iElemt]
                coolPayload[attrSpec.name()]=athenaAttrList[attrSpec.name()]

            # Store cool object to folder
            myfolder.storeObject(beginRun,endRun,coolPayload,coolChan,selectedTag)
            
        # Close new databse
        if bNewDBCreated==True:
            dbSave.closeDatabase()



    def BadChan_CheckSavedDatabaseContent(self,dbName,dbSvc,dbFolderName,selectedTag):

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
                statusWordSize=payload['StatusWordSize']
                endianness=payload['Endianness']
                version=payload['Version']
                blob=payload['Blob']

                # Decode blob content
                vect_BadChanEntry=self.nspace_LArBadChannelDBTools.decodeBlob(blob,channelSize,statusWordSize,endianness,version,
                                                                              self.nspace_LArBadChannelDBTools.getDefaultMsgStream())

                # Transform vect_BadChanNetry into python dictionnary
                sChannelDict,sChannelDict_Status=self.BadChan_TransformEntryIntoDictionnary(vect_BadChanEntry)

                sChannelKey=obj.channelId()
                HWidChecked={}
                for key in sChannelDict.keys(): HWidChecked[key]=0

                print " -> SQlite database content vs initial data : channel ", sChannelKey

                # Check all the saved data vs initial data
                listHWidKeys=[x for x in sChannelDict.keys()]
                for x in self.dict_vectBadChanEntry_Status[sChannelKey].keys():
                    if x not in listHWidKeys: listHWidKeys.append(x)
                listHWidKeys.sort()
                iChanCmpt=0
                for keyHWid in listHWidKeys:

                    try:
                        sChanName,badChan_word,sValueInit=sChannelDict[keyHWid]
                    except:
                        sChanName="UNDEFINED"
                        badChan_word=0

                    sSuffix="OK"
                    if self.dict_vectBadChanEntry_Status[sChannelKey][keyHWid]==STATUS_INIT:
                        if sChannelDict[keyHWid][1]!=self.dict_vectBadChanEntry_Init[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix="  no modification was done"+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix="  " 
                        self.BadChan_DisplayBadChanEntryLine(sPrefix,iChanCmpt,keyHWid,sChanName,badChan_word,sSuffix)

                    if self.dict_vectBadChanEntry_Status[sChannelKey][keyHWid]==STATUS_REMOVED:
                        if sChannelDict.has_key(keyHWid):
                            sPrefix="ERR"
                            sSuffix=" deletion not taken into accout"+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_REMOVED
                        sChanName,badChan_word,sValue_Init=self.dict_vectBadChanEntry[sChannelKey][keyHWid]
                        self.BadChan_DisplayBadChanEntryLine(sPrefix,iChanCmpt,keyHWid,sChanName,badChan_word,sSuffix)

                    if self.dict_vectBadChanEntry_Status[sChannelKey][keyHWid]==STATUS_NEW:
                        if sChannelDict[keyHWid][1]!=self.dict_vectBadChanEntry[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix=" error while saving new status "+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_NEW
                        self.BadChan_DisplayBadChanEntryLine(sPrefix,iChanCmpt,keyHWid,sChanName,badChan_word,sSuffix)

                    if self.dict_vectBadChanEntry_Status[sChannelKey][keyHWid]==STATUS_MODIFIED:
                        if sChannelDict[keyHWid][1]!=self.dict_vectBadChanEntry[sChannelKey][keyHWid][1]:
                            sPrefix="ERR"
                            sSuffix=" error while saving modified status "+keyHWid
                            bSavingProcessError=True
                        else:
                            sPrefix=STATUS_MODIFIED
                        self.BadChan_DisplayBadChanEntryLine(sPrefix,iChanCmpt,keyHWid,sChanName,badChan_word,sSuffix)

                    HWidChecked[keyHWid]=1
                    iChanCmpt += 1

                for key in self.dict_vectBadChanEntry_Status[sChannelKey].keys():
                    if not HWidChecked.has_key(key) and self.dict_vectBadChanEntry_Status[sChannelKey][key]!=STATUS_REMOVED:
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


            
    
