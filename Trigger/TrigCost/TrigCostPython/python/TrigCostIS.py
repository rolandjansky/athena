# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Brian Petersen, CERN (2011)
#
# Read rates from online IS server (or "public" mirror)
#
# Results are one rate per lumiblock
#

#print 'Hello world TrigCostIS.py'

import dl
import math
import os
import sys
import time
#print '\tLoaded standard packages'

dl.open("libers.so", dl.RTLD_GLOBAL| dl.RTLD_NOW) #

try:
    import ipc;  #print '\tLoaded special ipc'
except:
    print 'WARNING -- TrigCostIS -- Cannot load module `ipc`. Set-up release for AtlasCAFHLT or AtlasP1HLT.'

try:
    from ispy import *;  #print '\tLoaded special ispy'
except:
    print 'WARNING -- TrigCostIS -- Cannot load module `ispy`. Set-up release for AtlasCAFHLT or AtlasP1HLT.'

from TrigCostPython import TrigCostAnalysis # print '\tLoaded special TrigCostPython.TrigCostAnalysis package'
from TrigCostPython import TrigCostXML      # print '\tLoaded special TrigCostPython.TrigCostXML package'
#print '\tDone prelim import for TrigCostIS'

#----------------------------------------------------------------------
def ReadISRates(beginTime,endTime=None,partition='rateTest'):
    server='ISS_TRP'
    if partition=='rateTest':
        #point to mirror instead of online server
        os.putenv('TDAQ_IPC_INIT_REF','file:/afs/cern.ch/user/a/aagaard/public/rateipc_root.ref')
        server='TRP'

    p = ipc.IPCPartition(partition)
    if not p.isValid():
        print 'Did not find rate partition'
        return None

    d = ISInfoDictionary(p)
    obj = ISInfoDynAny()

    samplingrate = 10

    collections={}

    for level in [('L1','TRP.L1_Rate','TAV'),
                ('L2','TRP.L2_Rate','raw'),
                ('EF','TRP.EF_Rate','raw'),
                #('Lumi','TRP.Lu_Rate_Luminosity','CalibLumi_Val')
                  ]:
        lvl     = level[0]
        object  = level[1]
        sfx_out = level[2]
        try:
            tags=d.getTags(object) # get time points - assume a maximum of 24 hours of data
            tagList=[]
            if lvl=='L1': #sort out time range
                tag,tval=tags[0] #latest measurement
                today=tval.day()
                if not endTime:
                    endTime='%2d:02d' % (tval.hour(),tval.min())
                start=int(beginTime.split(':')[0])*60+int(beginTime.split(':')[1])
                if start>tval.hour()*60+tval.min(): #starts in previous day
                    start-=24*60
                end=int(endTime.split(':')[0])*60+int(endTime.split(':')[1])
                if end>tval.hour()*60+tval.min(): #ends in previous day
                    end-=24*60
            for tag,tval in tags:
                ts=tval.hour()*60+tval.min()
                if tval.day()!=today:
                    ts-=24*60
                if ts>=start and ts<=end:
                    #print lvl,tag,tval.str()
                    tagList.append(tag)
                if ts<start:
                    break
            tagList.reverse()

            lblast=0 # Last LB number
            count =0 # Number of lb to average
            lbrate=0 # Initialize rate for one lb
            for tag in tagList:
                d.getValue(object,tag,obj)
                lb=obj['LumiBlock']
                run=obj['RunNumber']
                if not run in collections:
                    collections[run] = TrigCostAnalysis.CostResultCollection()
                    collections[run].lbbeg=lb
                    collections[run].lbend=lb
                collection=collections[run]
                collection.lbend=lb

                #print dir(obj)
                if lvl=='L1' and lb!=lblast:
                    collection.results[lb]=TrigCostAnalysis.CostResult()
#                if lvl=='Lumi':
#                    collection.lumi=obj['Data'][2] #ATLAS_PREFERRED CalibLumi
#                    continue
                triggers=obj['XLabels']
                ylabels=obj['YLabels']
                #print ylabels
                data=obj['Data']

                #-------------------------------------------------------
                def getCol(xidx,column):
                    ylen=len(ylabels)
                    yidx=ylabels.index(column)
                    return data[xidx*ylen+yidx]

                cpsval=False #don't have this in IS server ( I think)
                for xidx,bname in enumerate(triggers):
                    if bname=='L1A' or not bname.startswith(lvl):
                        bname=lvl+'_'+bname
#                    print lb,bname

                    if lb!=lblast:
                        ch = TrigCostAnalysis.CostChain()
                        count = 1
                        ch.SetName(bname)
                        ch.SetLevel(lvl)
                        if cpsval:
                            ch.SetRate(cpsval)
                        else:
                            rate = getCol(xidx,sfx_out)
                            ch.SetRate(rate)

                            # Add all rates, not just output rates
                            if lvl == 'L1':
                                ch.SetPrescale(getCol(xidx,'PS'))

                                # Reverse-engineer counts -- will be approximate
                                ch.SetTBPCnt (getCol(xidx,'TBP')*samplingrate)
                                ch.SetTAPCnt (getCol(xidx,'TAP')*samplingrate)
                                ch.SetTAVCnt (getCol(xidx,'TAV')*samplingrate)

                                # Rates are stored in TRP ntuples
                                ch.SetTBPRate(getCol(xidx,'TBP'))
                                ch.SetTAPRate(getCol(xidx,'TAP'))

                            else:
                                # Reverse-engineer counts -- will be approximate
                                ch.SetTBPCnt (getCol(xidx,'input'   )*samplingrate)
                                ch.SetTAPCnt (getCol(xidx,'prescale')*samplingrate)
                                ch.SetTAVCnt (getCol(xidx,'raw'     )*samplingrate)

                                # Rates are stored in TRP ntuples
                                ch.SetTBPRate(getCol(xidx,'input'   ))
                                ch.SetTAPRate(getCol(xidx,'prescale'))

                        collection.SetCostChain(lb,bname,ch)

                    else:
                        # Combine sample previous samples (#samples=count)
                        ch          = collection.GetCostChain(lb,bname)
                        pastrate    = ch.GetRate()
                        cumulative  = ch.GetRate()*count
                        lbrate      = getCol(xidx,sfx_out)
                        avgrate     = (cumulative+lbrate)/(count+1)
                        ch.SetRate(avgrate)


                        # err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
                        err2 = ch.GetRate() / ((count+1)*samplingrate)
                        ch.SetRateErr(math.sqrt(err2))

                        if lvl == 'L1':
                            ch.SetPrescale(getCol(xidx,'PS'))

                            # Reverse-engineer counts -- will be approximate
                            ch.SetTBPCnt (getCol(xidx,'TBP')*samplingrate+ch.GetTBPCnt())
                            ch.SetTAPCnt (getCol(xidx,'TAP')*samplingrate+ch.GetTAPCnt())
                            ch.SetTAVCnt (getCol(xidx,'TAV')*samplingrate+ch.GetTAVCnt())

                            # Rates are stored in TRP ntuples
                            ch.SetTBPRate((getCol(xidx,'TBP')*count+ch.GetTBPCnt())/(count+1))
                            ch.SetTAPRate((getCol(xidx,'TAP')*count+ch.GetTAPCnt())/(count+1))

                            # Rate errors
                            ch.SetTBPRateErr(math.sqrt(ch.GetTBPRate() / ((count+1)*samplingrate)))
                            ch.SetTAPRateErr(math.sqrt(ch.GetTAPRate() / ((count+1)*samplingrate)))

                        else:

                            # Reverse-engineer counts -- will be approximate
                            ch.SetTBPCnt (getCol(xidx,'input'   )*samplingrate+ch.GetTBPCnt())
                            ch.SetTAPCnt (getCol(xidx,'prescale')*samplingrate+ch.GetTAPCnt())
                            ch.SetTAVCnt (getCol(xidx,'output'  )*samplingrate+ch.GetTAVCnt())

                            # Rate are stored in TRP ntuples
                            ch.SetTBPRate((getCol(xidx,'input'   )*count+ch.GetTBPCnt())/(count+1))
                            ch.SetTAPRate((getCol(xidx,'prescale')*count+ch.GetTAPCnt())/(count+1))

                            # Rate errors
                            ch.SetTBPRateErr(math.sqrt(ch.GetTBPRate() / ((count+1)*samplingrate)))
                            ch.SetTAPRateErr(math.sqrt(ch.GetTAPRate() / ((count+1)*samplingrate)))

                # Increment
                count += 1
                lblast = lb

        except Exception:
            print 'Exception reading data from IS'
            raise
    return collections

