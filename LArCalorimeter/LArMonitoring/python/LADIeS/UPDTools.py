# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#############################################################################
# Authors : Emmanuel Sauvan, sauvan@lapp.in2p3.fr
#         : Julien Morel,    julien.morel@cern.ch
#         : Walter Lampl,    Walter.Lampl@cern.ch 
#############################################################################
#...Import
import time
import os
import PyCintex as PyLCGDict
import ECalDBLib

from array import array
from PyCool import cool,coral
from CoolConvUtilities.AtlCoolLib import indirectOpen

PyLCGDict.loadDict('BadChanDict')
import ROOT
from ROOT import LArBadChannelDBTools,AthenaAttributeList
from CoolTools import *

#############################################################################
#...General definition
bct        = LArBadChannelDBTools()
A          = AthenaAttributeList(bct.createCoolSpec())
log        = bct.getDefaultMsgStream()
Tags       = {'UPD1':'ES1','UPD4':'BLK'}
connect    = 0
folderName = 0
folder     = 0
dbSvc      = 0
db         = 0
option     = ''
#############################################################################
def DateToIOV(dateSince,dateUntil):
    #...Get the run-lumi corresonding to the dates
    IOVSince = 0
    IOVUntil = 0
    if ':' in str(dateSince) and ':' in str(dateUntil):
        runlumi = GetRunNumberLumiblock(dateSince,dateUntil)
        IOVSince = int(runlumi[0][0]<<32) + int(runlumi[0][1]<<0)
        IOVUntil = int(runlumi[1][0]<<32) + int(runlumi[1][1]<<0)
    elif ':' in str(dateSince) and int(dateUntil)==-1:
        runlumi = GetRunNumberLumiblock(dateSince,'2099-07-13:14:27:00')
        IOVSince = int(runlumi[0][0]<<32) + int(runlumi[0][1]<<0)
        IOVUntil = (cool.ValidityKeyMax>>32)<<32 
    elif int(dateUntil)!=-1:
        IOVSince = int(dateSince)<<32
        IOVUntil = int(dateUntil)<<32
    else:
        IOVSince = int(dateSince)<<32
        IOVUntil = (cool.ValidityKeyMax>>32)<<32

    return IOVSince, IOVUntil

#############################################################################
def GetBadChanByIOV(IOV,list='UPD4'):
    res={}
    
    #...Get the info in the database
    fld = db.getFolder(folderName)
    if list=='UPD3':
        tag = 'LARBadChannelsOflBadChannels-UPD3-00'
    elif list=='BULK-00':
        tag = 'LARBadChannelsOflBadChannels-Bulk-00'
    else:
        tag = fld.resolveTag('COMCOND-%sPST-004-05'%(Tags[list])) #...New tag since 6 april 2011
    objs = fld.browseObjects(IOV,IOV,cool.ChannelSelection.all(),tag)

    #...Loop and store the channels
    for obj in objs:
        p = obj.payload()
        for i in ['Blob','ChannelSize','StatusWordSize','Endianness','Version']: A[i] = p[i]
        badChanList = bct.readBadChan(A,log)
        updatetime = obj.insertionTime()
        since      = obj.since()
        for i in range(badChanList.size()):
            badchan   = badChanList.at(i)
            channelId = badchan[0].get_compact()
            badChan   = badchan[1].packedData() 
            res[channelId] = {'Id':channelId, 'badchanNew':badChan, 'badchanOld':badChan, 'badchanStart':badChan, 'updateTime':updatetime, 'IOVSince':since}

    return res

#############################################################################
def GetBadChanChangeLog(dateSince,dateUntil,list='UPD4'):

    chantest = 0

    resNew     = {}
    resUp      = {}
    resRemoved = {}
    DBContent  = {}

    def PreviousSince(channelID,badChan,IOVSince):
        IOVKeys = DBContent.keys()
        IOVKeys.sort()
        IOVKeys.reverse()
        for IOV in IOVKeys:
            if IOVSince<=IOV: continue
            list = DBContent[IOV]
            for blob in list:
                if channelID == blob[0]:
                    blob.append(IOV)
                    return blob
        return -1

    def NextSince(channelID,IOVSince):
        IOVKeys = DBContent.keys()
        IOVKeys.sort()
        for IOV in IOVKeys:
            if IOVSince>=IOV: continue
            for blob in DBContent[IOV]:
                if channelID == blob[0]:
                    if IOV == IOVKeys[0]: return -1
                    blob.append(IOV)
                    return blob
        return -1

    def IsModified(channelId,since):
        if since in DBContent.keys(): 
            for i in range(len(DBContent[since])):
                blob = DBContent[since][i]
                if channelId==blob[0]:
                    return i
        return -1
            
    
    #...Get the run-lumi corresonding to the dates
    IOVSince, IOVUntil = DateToIOV(dateSince,dateUntil)

    #...Get channel at the start and the end
    channelStart = GetBadChanByIOV(IOVSince,list)
    channelEnd   = GetBadChanByIOV(IOVUntil,list)

    #...Get the info in the database
    fld = db.getFolder(folderName)
    if list=='UPD3':
        tag = 'LARBadChannelsOflBadChannels-UPD3-00'
    elif list=='BULK-00':
        tag = 'LARBadChannelsOflBadChannels-Bulk-00'
    else:
        tag = fld.resolveTag('COMCOND-%sPST-004-05'%(Tags[list])) #...New tag since 6 april 2011
    objs = fld.browseObjects(IOVSince,IOVUntil,cool.ChannelSelection.all(),tag)

    #...Store DB content in a python dict
    for obj in objs:
        #...Get the UPD content
        plCurrent = obj.payload()
        for i in ['Blob','ChannelSize','StatusWordSize','Endianness','Version']: A[i] = plCurrent[i]
        badChanListCurrent = bct.readBadChan(A,log)

        updatetime = obj.insertionTime()
        since      = obj.since()
        until      = obj.until()

        for j in range(badChanListCurrent.size()):
            currentChan = badChanListCurrent.at(j)
            channelId = currentChan[0].get_compact()
            badChan   = currentChan[1].packedData()

            #...if already modified update the status
            if since!=cool.ValidityKeyMax and (since>=IOVSince and since<=IOVUntil):
                if not(since in DBContent.keys()): DBContent[since] = []
                index = IsModified(channelId,since)
                if index!=-1:
                    DBContent[since][index][1]=badChan
                    DBContent[since][index][2]=updatetime
                    if channelId == chantest:
                        print '-00-',chantest,'->',since>>32,until>>32, [channelId,badChan,updatetime]
                #...else add the modification
                else:
                    DBContent[since].append([channelId,badChan,updatetime])
                    if channelId == chantest:
                        print '-01-',chantest,'->',since>>32,until>>32, [channelId,badChan,updatetime]
            if until!=cool.ValidityKeyMax and (until>=IOVSince and until<=IOVUntil):
                #...in any case add the end of the modification    
                if not(until in DBContent.keys()): DBContent[until] = []
                if channelId in channelStart.keys():
                    DBContent[until].append([channelId,channelStart[channelId]['badchanNew'],updatetime])
                    if channelId == chantest:
                        print '-1-',chantest,'->',since>>32,until>>32, [channelId,channelStart[channelId]['badchanNew'],updatetime]
                else:
                    DBContent[until].append([channelId,0,updatetime])
                    if channelId == chantest:
                        print '-2-',chantest,'->',since>>32,until>>32, [channelId,0,updatetime]
                
    #...Loop and compute the differences
    keylist = DBContent.keys()
    keylist.sort()
    for indice in range(len(keylist)):
        since = keylist[indice]

        resUp     [since]=[]
        resNew    [since]=[]
        resRemoved[since]=[]

        
        if chantest != 0:
            print '                       ',since>>32
        for chan in DBContent[since]:                
            channelId  = chan[0]
            badChan    = chan[1]
            updatetime = chan[2]
            badChanStart = 0
            if channelId in channelStart.keys():
                badChanStart = channelStart[channelId]['badchanStart']


            if channelId == chantest:
                print '--',chantest,'->',indice,channelId,badChan,since>>32
                
            #...Count the updated and new channels
            previousIOV = PreviousSince(channelId,badChan,since)
            if previousIOV != -1:
                if channelId == chantest:
                    print previousIOV[0], previousIOV[1], previousIOV[3]>>32
                    print 'comparaison ', badChan,'!=',previousIOV[1]
                if badChan!=previousIOV[1]:
                    resUp[since].append({'Id':channelId, 'badchanNew':badChan, 'badchanOld':previousIOV[1], 'badchanStart':badChanStart, 'updateTime':updatetime, 'IOVSince':since})
                    if channelId == chantest:
                        print '--up->',indice,channelId,'badchanNew',badChan,'badchanOld',previousIOV[1],since>>32

            elif not(channelId in channelStart.keys()):
                resNew[since].append({'Id':channelId, 'badchanNew':badChan, 'badchanOld':0, 'badchanStart':badChanStart, 'updateTime':updatetime, 'IOVSince':since})
                resUp [since].append({'Id':channelId, 'badchanNew':badChan, 'badchanOld':0, 'badchanStart':badChanStart, 'updateTime':updatetime, 'IOVSince':since})
                if channelId == chantest:
                    print '-new->',indice,channelId,badChan,since>>32


            nextIOV = NextSince(channelId,since)
            if nextIOV == -1 and not(channelId in channelEnd.keys()):
                resRemoved[since].append({'Id':channelId, 'badchanNew':0, 'badchanOld':badChan, 'badchanStart':badChanStart, 'updateTime':updatetime, 'IOVSince':since})
                if channelId == chantest:
                    print '--rm->',indice,channelId,badChan,since>>32
                    
                    
    return resNew, resUp, resRemoved



#############################################################################
def DisplayBadChanChangeLog(dateSince,dateUntil,list='UPD4'):

    #...Get the run-lumi corresonding to the dates
    IOVSince, IOVUntil = DateToIOV(dateSince,dateUntil)

    #...shape the date for the display
    DispDate1 = ''
    if ':' in str(dateSince):
        DispDate1 = dateSince +'('+str(IOVSince>>32)+' '+str(IOVSince&0xFFFFFFFF)+')'
    else:
        DispDate1 = str(dateSince)

    DispDate2 = ''
    if ':' in str(dateUntil):
        DispDate2 = dateUntil +'('+str(IOVUntil>>32)+' '+str(IOVUntil&0xFFFFFFFF)+')'
    elif int(dateUntil)==-1:
        DispDate2 = 'cool.ValidityKeyMax'
    else:
        DispDate2 = str(dateUntil)

    print ' '
    print ' '
    print ' '
    print ' '
    print '**********************************************************************************************************************'
    print '       ',list,'changes beetwen',DispDate1,'and',DispDate2
    print '**********************************************************************************************************************'

    ListNew, ListUp, ListRemoved = GetBadChanChangeLog(dateSince,dateUntil,list)
    channelStart = GetBadChanByIOV(IOVSince,list)

    if len(ListUp)!=0:
        print ' '
        print '*********************************************************************'
        print 'Summary of modified channels:'
        DisplayLogSummary(ListUp)
        
    if len(ListUp)!=0:
        print ' '
        print '*********************************************************************'
        print 'Log for modified channels:'
        keylist = ListUp.keys()
        keylist.sort()
        for since in keylist:
            list=ListUp[since]
            if len(list)==0: continue

            time   = list[0]['updateTime']            
            UpDate = " %i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())
            print UpDate,'(IOV',since>>32,str(since&0xFFFFFFFF)+')  -> ', len(list),'updated channels :'

            for Blob in list:
                Id        = Blob['Id']
                badchNew  = Blob['badchanNew']            
                time      = Blob['updateTime']            
                badchOld  = Blob['badchanOld']            
                UpDate = " %i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())
                chan = GetChannelByOnlineId(Id)
                print '   ','channel=0x%x  %7s FT%2.0d SL%2.0d CH%3.0d - %s (0x%x) --> %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badchOld),badchOld,BadChanInterpreter(badchNew),badchNew)


    if len(ListUp)>1:
        if 'Histo' in option:
            DisplayHisto(IOVSince, IOVUntil,ListUp,channelStart)

##     if len(ListNew)!=0:
##         print ' '
##         print '*********************************************************************'
##         print 'Summary for new channels:'
##         DisplayLogSummary(ListNew)
##     if len(ListRemoved)!=0:
##         print ' '
##         print '*********************************************************************'
##         print 'Summary for removed channels:'
##         DisplayLogSummary(ListRemoved)
         
##     if len(ListNew)!=0:
##         print ' '
##         print '*********************************************************************'
##         print 'Log for new channels:'
##         keylist = ListNew.keys()
##         keylist.sort()
##         for since in keylist:
##             list=ListNew[since]
##             if len(list)==0: continue
            
##             time   = list[0]['updateTime']            
##             UpDate = " %i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())
##             print UpDate,'(IOV',since>>32,str(since&0xFFFFFFFF)+')  -> ', len(list),'new channels :'
##             for Blob in list:
##                 Id     = Blob['Id']
##                 badch  = Blob['badchanNew']            
##                 time   = Blob['updateTime']
##                 UpDate = " %i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())
##                 chan = GetChannelByOnlineId(Id)
##                 print '   ','channel=0x%x  %7s FT%2.0d SL%2.0d CH%3.0d - %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badch),badch)

                
##     if len(ListRemoved)!=0:
##         print ' '
##         print '*********************************************************************'
##         print 'Log for removed channeld:'
##         keylist = ListRemoved.keys()
##         keylist.sort()
##         for since in keylist:
##             list=ListRemoved[since]
##             if len(list)==0: continue
            
##             time   = list[0]['updateTime']            
##             UpDate = " %i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())
##             print UpDate,'(IOV',since>>32,str(since&0xFFFFFFFF)+')  -> ', len(list),'removed channels :'
            
##             for Blob in list:
##                 Id     = Blob['Id']
##                 badch  = Blob['badchanNew']            
##                 chan = GetChannelByOnlineId(Id)
##                 print '   ','channel=0x%x  %7s FT%2.0d SL%2.0d CH%3.0d - %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badch),badch)

#############################################################################
def DisplayLogSummary(ListUp):
    """
    Display the number of different channels in the list ListUp.
    Display the channel and corresponding IOVs if more then 1 IOV is associate to the channel in the list.
    """
    def FillHisto(since, Blob,Hist):
        Id           = Blob['Id']
        badChan      = BadChanInterpreter(Blob['badchanNew'])
        badChanStart = BadChanInterpreter(Blob['badchanStart'])
        chan         = GetChannelByOnlineId(Id)

        #...count the number of channels in different part of the detec
        if badChan!=badChanStart:
            Hist['ALL']['all'] +=1
            if   'highNoiseHG'        in badChan: Hist['ALL']['highNoiseHG']        +=1
            elif 'sporadicBurstNoise' in badChan: Hist['ALL']['sporadicBurstNoise'] +=1            
            else                                : Hist['ALL']['other'] +=1
        for Part in Partitions:
            if   chan['subSubPartition'][0:-1]==Part:
                if badChan!=badChanStart:
                    Hist[Part]['all'] +=1
                    if   'highNoiseHG'        in badChan: Hist[Part]['highNoiseHG']        +=1
                    elif 'sporadicBurstNoise' in badChan: Hist[Part]['sporadicBurstNoise'] +=1
                    else                                : Hist[Part]['other'] +=1
        
    #...Count the number of different channel (separate for EMB, EMBPS, EMEC, EMECPS, HEC and FCAL) and store IOVSince for channel with more then one occurence
    channelDict = {} #...channelDict[ChannelId] = [IOV1,IOV2,...]
    Partitions  = ['ALL', 'EMB', 'EMBPS', 'EMEC', 'EMECPS', 'HEC', 'FCAL']
    BadChannels = ['highNoiseHG','sporadicBurstNoise','other','all']

    countHist   = {}
    for part in Partitions:
        countHist[part]={}
        for bc in BadChannels:
            countHist[part][bc] = 0


    iovSet = set()
    for since,list in ListUp.iteritems():        
        if len(list)==0: continue
        for Blob in list:
            FillHisto(since,Blob,countHist)                        
            iovSet.add(since>>32)
            #...Fill the list of channels
            Id           = Blob['Id']
            if not(Id in channelDict.keys()):
                channelDict[Id] = []
                channelDict[Id].append(Blob)
            else:
                channelDict[Id].append(Blob)
    #...count the number of good run
    numberOfGoodRun = 0
    GoodRun = []
    for iov in iovSet:
        if len(GoodLB(iov))>0:
            GoodRun.append(iov)
            numberOfGoodRun += 1

    #...Display the summary
    ecalDB = ECalDBLib.ECalDBLib()
    ecalDB.Open('sqlite://'+ECalDBLib.dbBasePath+'/ECalDB.db')

    
    print '   '
    print '   ',' This report is done over %d AtlasReady runs : ' % numberOfGoodRun ,GoodRun
    print '   ','/---------------------------------------------------------------------------------\\'
    print '   ','| Partition        | highNoiseHG | sporadicBurstNoise | Other       | Total       |'
    print '   ','|------------------|-------------|--------------------|-------------|-------------|'
    print '   ','| in EMB  (EMBPS)  | %4i (%4i) | %4i (%4i)        | %4i (%4i) | %4i (%4i) |' % (countHist['EMB' ]['highNoiseHG']+countHist['EMBPS' ]['highNoiseHG'] , countHist['EMBPS' ]['highNoiseHG'], countHist['EMB' ]['sporadicBurstNoise']+countHist['EMBPS' ]['sporadicBurstNoise'] , countHist['EMBPS' ]['sporadicBurstNoise'], countHist['EMB' ]['other']+countHist['EMBPS' ]['other'] , countHist['EMBPS' ]['other'], countHist['EMB' ]['all']+countHist['EMBPS' ]['all'] , countHist['EMBPS' ]['all'])
    print '   ','| in EMEC (EMECPS) | %4i (%4i) | %4i (%4i)        | %4i (%4i) | %4i (%4i) |' % (countHist['EMEC']['highNoiseHG']+countHist['EMECPS']['highNoiseHG'] , countHist['EMECPS']['highNoiseHG'], countHist['EMEC']['sporadicBurstNoise']+countHist['EMECPS']['sporadicBurstNoise'] , countHist['EMECPS']['sporadicBurstNoise'], countHist['EMEC']['other']+countHist['EMECPS']['other'] , countHist['EMECPS']['other'], countHist['EMEC']['all']+countHist['EMECPS']['all'] , countHist['EMECPS']['all'])
    print '   ','| in HEC           | %4i        | %4i               | %4i        | %4i        |'     % (countHist['HEC' ]['highNoiseHG'], countHist['HEC' ]['sporadicBurstNoise'], countHist['HEC' ]['other'], countHist['HEC' ]['all'])
    print '   ','| in FCAL          | %4i        | %4i               | %4i        | %4i        |'     % (countHist['FCAL']['highNoiseHG'], countHist['FCAL']['sporadicBurstNoise'], countHist['FCAL']['other'], countHist['FCAL']['all'])
    print '   ','|------------------|-------------|--------------------|-------------|-------------|'
    print '   ','| Total            | %4i        | %4i               | %4i        | %4i        |'     % (countHist['ALL' ]['highNoiseHG'], countHist['ALL' ]['sporadicBurstNoise'], countHist['ALL' ]['other'], countHist['ALL' ]['all'])
    print '   ','\\---------------------------------------------------------------------------------/'
    print '   '
    print ' List of channels with many occurences:'

    #...Print the log
    keylist = channelDict.keys()
    keylist.sort()
    occDict = {}
    for Id in keylist:
        taille = len(channelDict[Id])
        if not(taille in occDict.keys()):
            occDict[taille] = []
            occDict[taille].append(Id)
        else:
            occDict[taille].append(Id)

    occList = occDict.keys()
    occList.sort()
    occList.reverse()
    for nb in occList:
        occDict[nb].sort()
        occDict[nb].reverse()
        for Id in occDict[nb]:
            list=channelDict[Id]
            if len(list)<3 : continue
            badch = channelDict[Id][0]['badchanNew']
            chan  = GetChannelByOnlineId(Id)
            list.sort()

            outline = []
            count   = 0
            for i in range(len(list)):
                Blob =  list[i]
                    
                badchNew  = Blob['badchanNew']
                badchOld  = Blob['badchanOld']
                since     = Blob['IOVSince']
                time      = Blob['updateTime']            

                #...Do not display the 2 line add and remove for the same channel
                disp = 1
                if i>0:
                    prevBlob =  list[i-1]
                    prevSince     = prevBlob['IOVSince']
                    prevBadchNew  = prevBlob['badchanNew']
                    prevBadchOld  = prevBlob['badchanOld']

##                     if int(since>>32) == int(prevSince>>32)+1:
##                         if badchNew==prevBadchOld:
##                             disp=0
                             
                UpDate = "%i-%.2i-%.2i %.2i:%.2i:%.2i" % (time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second())

                action = 'Updated'
                if badchOld == 0:
                    action = 'Added'
                if badchNew == 0:
                    action = 'Removed'

                if disp==1:
                    if since>>32 in GoodRun:
                        outline.append('          '+UpDate+' (IOV %6i* %10i '%(since>>32,since&0xFFFFFFFF)+')  '+' %s (0x%x)  -->  %s (0x%x)'%(BadChanInterpreter(badchOld),badchOld,BadChanInterpreter(badchNew),badchNew)+' '+action)
                        count += 1
                    else:
                        outline.append('          '+UpDate+' (IOV %6i  %10i '%(since>>32,since&0xFFFFFFFF)+')  '+' %s (0x%x)  -->  %s (0x%x)'%(BadChanInterpreter(badchOld),badchOld,BadChanInterpreter(badchNew),badchNew)+' '+action)
            print ' ','channel=0x%x  %7s FT%2.0d SL%2.0d CH%3.0d  - %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badch),badch)
            print '      ','Observed in %d AtlasReady IOV* over %d (~%2.0d per cent):' % (count, numberOfGoodRun, 100 * count/ numberOfGoodRun)
            for i in outline:
                print i
                
            #...Print history from ECal
            idMax = ecalDB.GetCampaignId()-1
            idMin = 1
            historyList = ecalDB.GetChannelHistoryByCampaingId(chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'], idMin, idMax)
            histSummuray = {}

            for type in ECalDBLib.typeRun.keys():
                for history in historyList:
                    if type in history.keys():
                        if type in histSummuray.keys():
                            histSummuray[type].append([history['id'],history['date']])
                        else:
                            histSummuray[type]=[]
                            histSummuray[type].append([history['id'],history['date']])


            if len(histSummuray)>0:
                print '      ','Calibration deviations :'
                for type in histSummuray.keys():
                    if len(histSummuray[type])>0:
                        displayedCamp = ''
                        for i in histSummuray[type]:
                            displayedCamp += str(i[0])+' '
                        print '         ',type+' -> '+str(len(histSummuray[type]))+' times in campaigns: '+displayedCamp
        
    ecalDB.Close()
#############################################################################
def DisplayHisto(min,max,ListUp,channelStart={}):
    """
    Display the number of different channels in the list ListUp.
    Display the channel and corresponding IOVs if more then 1 IOV is associate to the channel in the list.
    """

    def FillHisto(since, Blob,Hist,binList):
        Id           = Blob['Id']
        badChanNew   = BadChanInterpreter(Blob['badchanNew'])
        badChanOld   = BadChanInterpreter(Blob['badchanOld'])
        badChanStart = BadChanInterpreter(Blob['badchanStart'])
        chan         = GetChannelByOnlineId(Id)
        
        def FilluptoEnd(Histo,since,weight):
            for i in range(Histo.GetNbinsX()):
                if binList[i] >= since:
                    Histo.SetBinContent(i+1,weight+Histo.GetBinContent(i+1))
                    
        def FindBad(list):
            res = []
            list = list.split('/')
            for i in list:
                if i=='': continue
                if i in BadChannels:
                    res.append(i)
                elif not('other' in res):
                    res.append('other')
            return res
                    
        #...count the number of channels in different part of the detec
        listOld = FindBad(badChanOld)
        for bad in listOld:
            if bad=='' :continue
            if 'Debug' in option: print since>>32, 'Remove '+bad+' : ',badChanOld,'->',badChanNew            
            FilluptoEnd(Hist['ALL'][bad]       ,since>>32,-1)

        listNew = FindBad(badChanNew)
        for bad in listNew:
            if bad=='' :continue
            if 'Debug' in option: print since>>32, 'Add '+bad+' : ',badChanOld,'->',badChanNew            
            FilluptoEnd(Hist['ALL'][bad]       ,since>>32,1)

        for Part in Partitions:
            if   chan['subSubPartition'][0:-1]==Part:
                listOld = FindBad(badChanOld)
                for bad in listOld:
                    if bad=='' :continue
                    if 'Debug' in option: print since>>32, 'Remove '+bad+' : ',badChanOld,'->',badChanNew            
                    FilluptoEnd(Hist[Part][bad]       ,since>>32,-1)

                listNew = FindBad(badChanNew)
                for bad in listNew:
                    if bad=='' :continue
                    if 'Debug' in option: print since>>32, 'Add '+bad+' : ',badChanOld,'->',badChanNew            
                    FilluptoEnd(Hist[Part][bad]       ,since>>32,1)
        
    #...Count the number of different channel (separate for EMB, EMBPS, EMEC, EMECPS, HEC and FCAL) and store IOVSince for channel with more then one occurence
    channelDict = {} #...channelDict[ChannelId] = [IOV1,IOV2,...]
    Partitions  = ['ALL', 'EMB', 'EMBPS', 'EMEC', 'EMECPS', 'HEC', 'FCAL']
    color       = {'ALL':1, 'EMB':2, 'EMBPS':3, 'EMEC':4, 'EMECPS':5, 'HEC':6, 'FCAL':7}
    BadChannels = ['highNoiseHG','sporadicBurstNoise','other','all']

    #...Get the bin list
    binList = [min>>32]
    for i,blob in ListUp.iteritems():        
        if (i>min and i<max):
            binList.append(i>>32)

    
    if max>>32!=cool.ValidityKeyMax>>32: binList.append(max>>32)
    binList.sort()

    print 'bins are ',binList

    def SetXLabel(h,binlist):
       for i in range(len(binlist)-1):
           if binlist[i]==binlist[i+1]-1:
               h.GetXaxis().SetBinLabel(i+1,str(binlist[i]))
            

    countHist   = {}
    startHist   = {}
    for part in Partitions:
        countHist[part]={} 
        startHist[part]={}
        for bc in BadChannels:
            if 'FixedSizeBin' in option:
                countHist[part][bc] = ROOT.TH1I('countHist_'+part+'_'+bc,part,len(binList)-1,binList[0],binList[-1])
                startHist[part][bc] = ROOT.TH1I('startHist_'+part+'_'+bc,part,len(binList)-1,binList[0],binList[-1])
            else:
                countHist[part][bc] = ROOT.TH1I('countHist_'+part+'_'+bc,part,len(binList)-1,array('d',binList))
                startHist[part][bc] = ROOT.TH1I('startHist_'+part+'_'+bc,part,len(binList)-1,array('d',binList))
            SetXLabel(countHist[part][bc],binList)
            SetXLabel(startHist[part][bc],binList)
                    
    for since,list in ListUp.iteritems():        
        if len(list)==0: continue
        for Blob in list:
            FillHisto(since,Blob,countHist,binList)
            
            #...Fill the list of channels
            Id           = Blob['Id'] 
            if not(Id in channelDict.keys()):
                channelDict[Id] = []
                channelDict[Id].append(Blob)
            else:
                channelDict[Id].append(Blob)

    #...Display the summary
    if channelStart != {}:
        #...Get the status at the starting point
        def FillStartHisto(histo):
            for i in range(histo.GetNbinsX()):
                histo.SetBinContent(i+1,1+histo.GetBinContent(i+1))

        for Id, Blob in channelStart.iteritems():            
            Id           = Blob['Id']
            badChan      = BadChanInterpreter(Blob['badchanNew'])
            chan         = GetChannelByOnlineId(Id)
            
            #...count the number of channels in different part of the detec
            FillStartHisto(startHist['ALL']['all'])
            if   'highNoiseHG'        in badChan: FillStartHisto(startHist['ALL']['highNoiseHG'])
            elif 'sporadicBurstNoise' in badChan: FillStartHisto(startHist['ALL']['sporadicBurstNoise'])
            else                                : FillStartHisto(startHist['ALL']['other'])
            for Part in Partitions:
                if   chan['subSubPartition'][0:-1]==Part:
                    FillStartHisto(startHist[Part]['all'])
                    if   'highNoiseHG'        in badChan: FillStartHisto(startHist[Part]['highNoiseHG']       )
                    elif 'sporadicBurstNoise' in badChan: FillStartHisto(startHist[Part]['sporadicBurstNoise'])
                    else                                : FillStartHisto(startHist[Part]['other'])
        #...Add the modified channels
        for part in Partitions:
            for bc in BadChannels:
                countHist[part][bc].Add(startHist[part][bc])
                
    #...cumulate the partition
    listPart = Partitions
    listPart.reverse()
    listPart.pop()
    for part1 in listPart:
        for part2 in listPart:
            if part1==part2: continue
            if listPart.index(part1)>listPart.index(part2): continue
            for bc in BadChannels:
                countHist[part1][bc].Add(countHist[part2][bc])
            

    #...Draw the graph
    countFig   = {}
    leg = ROOT.TLegend(0.15,0.15,0.3,0.5)
    texte = ROOT.TText()
    countFig = {}
    
    for bc in BadChannels:
        countFig[bc] = ROOT.TCanvas()
        ROOT.gPad.SetGridy()
        countHist[listPart[0]][bc].Draw()
        countHist[listPart[0]][bc].SetXTitle("IOV")
        countHist[listPart[0]][bc].SetYTitle(bc)
        for part in listPart:
            countHist[part][bc].SetLineColor(1)
            countHist[part][bc].SetFillColor(color[part])
            if bc==BadChannels[0]:
                leg.AddEntry(countHist[part][bc],countHist[part][bc].GetTitle(),"f")
            countHist[part][bc].Draw('same')
#        texte.DrawTextNDC(0.2,0.95,bc)
        max = countHist[listPart[0]][bc].GetBinContent(countHist[listPart[0]][bc].GetMaximumBin())
        max = 1.1*max
        countHist[listPart[0]][bc].GetYaxis().SetRangeUser(0,max)
        countHist[listPart[0]][bc].GetYaxis().SetNdivisions(4 + 100*0 + 10000*0)
        ROOT.gPad.RedrawAxis()
        
        leg.Draw()
    raw_input("Press Enter To Continue: ")                



#####################################################################################
def GetBadChanBetweenDates(dateSince,dateUntil,list='UPD4'):
    resNew={}
    resUp={}
    resRemoved={}

    #...Get the run-lumi corresonding to the dates
    IOVSince, IOVUntil = DateToIOV(dateSince,dateUntil)

    listSince = GetBadChanByIOV(IOVSince,list)
    listUntil = GetBadChanByIOV(IOVUntil,list)

    #...Count the updated and new channels
    for chanUntil in listUntil.keys():
        if chanUntil<IOVSince or chanUntil>IOVUntil: continue
        if chanUntil in listSince.keys():
            if listUntil[chanUntil]['badchanNew']!=listSince[chanUntil]['badchanNew']:
                listUntil[chanUntil]['badchanOld'] = listSince[chanUntil]['badchanNew']
                resUp[chanUntil]=listUntil[chanUntil]
            else: continue
        else:
            resNew[chanUntil]=listUntil[chanUntil]

    #...Count the removed channels
    countRemoved = 0
    for chanSince in listSince.keys():
        if chanSince<IOVSince or chanSince>IOVUntil: continue
        if not(chanSince in listUntil.keys()):
            resRemoved[chanSince]=listSince[chanSince]

    return resNew, resUp, resRemoved

#####################################################################################
def DisplayBadChanBetweenDates(dateSince,dateUntil,list='UPD4'):

    #...Get the run-lumi corresonding to the dates
    IOVSince, IOVUntil = DateToIOV(dateSince,dateUntil)

    #...shape the date for the display
    DispDate1 = ''
    if ':' in str(dateSince):
        DispDate1 = dateSince +'('+str(IOVSince>>32)+' '+str(IOVSince&0xFFFFFFFF)+')'
    else:
        DispDate1 = str(dateSince)

    DispDate2 = ''
    if ':' in str(dateUntil):
        DispDate2 = dateUntil +'('+str(IOVUntil>>32)+' '+str(IOVUntil&0xFFFFFFFF)+')'
    elif int(dateUntil)==-1:
        DispDate2 = 'cool.ValidityKeyMax'
    else:
        DispDate2 = str(dateUntil)

    print ' '
    print ' '
    print ' '
    print ' '
    print '**********************************************************************************************************************'
    print '       ',list,'differences beetwen',DispDate1,'and',DispDate2
    print '**********************************************************************************************************************'

    ListNew, ListUp, ListRemoved = GetBadChanBetweenDates(dateSince,dateUntil,list)

    if len(ListNew)!=0:
        print ' '
        print '*********************************************************************'
        print 'List of new channels:'
        for Id,Blob in ListNew.iteritems():
            if Id in ListUp.keys() : continue
            badch  = Blob['badchanNew']            
            chan = GetChannelByOnlineId(Id)
            DispIOV = '(IOV '+str(Blob['IOVSince']>>32)+' '+str(Blob['IOVSince']&0xFFFFFFFF)+')'
            print '   ','channel=0x%x  %7s FT%d SL%d CH%d - %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badch),badch) +'  '+DispIOV

    if len(ListUp)!=0:
        print ' '
        print '*********************************************************************'
        print 'List of updated channels:'
        for Id,Blob in ListUp.iteritems():
            badchNew  = Blob['badchanNew']
            badchOld  = Blob['badchanOld']
            chan = GetChannelByOnlineId(Id)
            DispIOV = '(IOV '+str(Blob['IOVSince']>>32)+' '+str(Blob['IOVSince']&0xFFFFFFFF)+')'
            print '   ','channel=0x%x  %7s FT%d SL%d CH%d - %s (0x%x) --> %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badchOld),badchOld,BadChanInterpreter(badchNew),badchNew)+'  '+DispIOV
          
    if len(ListRemoved)!=0:
        print ' '
        print '*********************************************************************'
        print 'List of removed channels:'
        for Id,Blob in ListRemoved.iteritems():
            badch  = Blob['badchanNew']
            chan = GetChannelByOnlineId(Id)
            DispIOV = '(IOV '+str(Blob['IOVSince']>>32)+' '+str(Blob['IOVSince']&0xFFFFFFFF)+')'
            print '   ','channel=0x%x  %7s FT%d SL%d CH%d - %s (0x%x)' % (Id,chan['subSubPartition'],chan['FT'],chan['SL'],chan['CH'],BadChanInterpreter(badch),badch)+'  '+DispIOV


    print ' '
    print '  ','Between',DispDate1,'and',DispDate2,'in',list,':'
    print '  ','There is',len(ListNew)+len(ListUp),'channels in the list.'
    print '  ',len(ListNew),'are new,'
    print '  ',len(ListUp),'were updated,'
    print '  ',len(ListRemoved),'were removed.'


#####################################################################
#...main
if __name__ == "__main__":

    def PrintHelp():
        print ' '
        print ' '
        print ' '
        print '   **************************************************************************************************************************************************** '
        print '   Print the changes in List between the two dates.'
        print '   Use : '
        print '   python '+sys.argv[0]+' dateSince dateUntil line list option'
        print '   dateSince and dateUntil = IOV or dates with the format : YYYY-MM-DD:hh:mm:ss'
        print '   line   = OFFLINE or ONLINE '
        print '   list   = UPD1 / UPD3 / UPD4 / BULK-00 '
        print '   option = Histo to draw histograms '
        print '            FixedSizeBin to have fixed size bin'
        print ' '
        print '   Default values are:'        
        print '   dateSince = No default value'
        print '   dateUntil = cool.ValidityKeyMax (use -1 if you want cool.ValidityKeyMax and other arguments)'
        print '   line      = OFFLINE'
        print '   list      = UPD4'
        print '   option    = ""'
        print ' '
        print '   Ex : python UPDTools.py 2011-02-16:00:00:00 -1 OFFLINE UPD4'
        print '   **************************************************************************************************************************************************** '
        print ' '
        print ' '
        print ' '

    if len(sys.argv)<2:
        PrintHelp()
        sys.exit()
        

    #...Get the argument
    dateUntil = -1
    line      = 'OFFLINE'
    list      = 'UPD4'
    if len(sys.argv)>1:
        dateSince = sys.argv[1]
    if len(sys.argv)>2:
        dateUntil = sys.argv[2]
    if len(sys.argv)>3:
        line = sys.argv[3]
    if len(sys.argv)>4:
        list = sys.argv[4]
    if len(sys.argv)>5:
        option = sys.argv[5]

    #...Connect to the COOL database
    if line=='ONLINE':
        connect="oracle://ATLR;schema=ATLAS_COOLONL_LAR;dbname=COMP200"
        folderName="/LAR/BadChannels/BadChannels"
    elif line=='OFFLINE':
        connect="oracle://ATLR;schema=ATLAS_COOLOFL_LAR;dbname=COMP200"
        folderName="/LAR/BadChannelsOfl/BadChannels"
    else:
        PrintHelp()

    dbSvc  = cool.DatabaseSvcFactory.databaseService()
    db     = dbSvc.openDatabase(connect+";user=ATLAS_COOL_READER;password=COOLRED4PRO")

        
    #...Diplay the log and the summuray
    DisplayBadChanBetweenDates(dateSince,dateUntil,list)
    DisplayBadChanChangeLog(dateSince,dateUntil,list)
#############################################################################
