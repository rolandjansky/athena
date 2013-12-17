#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from xml.dom import minidom
import matplotlib.pyplot as plt
from pylab import *

from optparse import OptionParser
p = OptionParser()
p.add_option( '-d','--debug',   action='store_true', default=False,                                dest="debug",   help="turn on global debug option")
p.add_option( '--printEvt',     action='store_true', default=False,                                dest="printEvt",help="print the event")
p.add_option( '--input',  type="string", default="/Trigger/TrigCost/TrigCostExec/data/event.xml",  dest="input",   help="path to input  file")
p.add_option( '--output', type="string", default="out.root",                                       dest="output",  help="path to output root file")
(options, args) = p.parse_args()

def ReadFile():
    
    testArea = str(os.environ.get("TestArea"))
    if os.path.exists(testArea+options.input):
        pres=minidom.parse(options.input).childNodes
        for pre in pres:
            evts=pre.childNodes
            for evt in evts:
                if evt.localName=="event":
                    return evt

    elif os.path.exists(options.input):
        pres=minidom.parse(options.input).childNodes
        for pre in pres:
            evts=pre.childNodes
            for evt in evts:
                if evt.localName=="event":
                    return evt

    print "ERROR: No input file"
    return minidom.parse(options.input)

def ReadEvent(evts):
    info=[]
    var={}
    var['id'            ]=int(evts.getAttribute('id'))
    var['run'           ]=int(evts.getAttribute('run'))           
    var['lb'            ]=int(evts.getAttribute('lb'))          
    var['bcid'          ]=int(evts.getAttribute('bcid'))          
    var['L1evtTime'     ]=str(evts.getAttribute('L1evtTime'))     
    var['L1evtTimens'   ]=int(evts.getAttribute('L1evtTimens'))   
    var['L2PUid'        ]=int(evts.getAttribute('L2PUid'))        
    var['L2PUName'      ]=str(evts.getAttribute('L2PUName'))      
    var['algTime'       ]=float(evts.getAttribute('algTime'))       
    var['steerTime'     ]=float(evts.getAttribute('steerTime'))     
    var['ROBTime'       ]=float(evts.getAttribute('ROBTime'))       
    var['ActiveAlgCalls']=int(evts.getAttribute('ActiveAlgCalls'))
    var['CachedAlgCalls']=int(evts.getAttribute('CachedAlgCalls'))
    var['ROBDataCollCalls' ]=int(evts.getAttribute('ROBDataCollCalls' ))
    var['dataCollretrieved']=int(evts.getAttribute('dataCollretrieved'))
    var['dataCollcached'   ]=int(evts.getAttribute('dataCollcached'   ))
    var['reqROBSize'       ]=float(evts.getAttribute('reqROBSize'       ))
    var['retROBSize'       ]=float(evts.getAttribute('retROBSize'       ))
    var['cachedROBSize'    ]=float(evts.getAttribute('cachedROBSize'    ))
    var['otherROBSize'     ]=float(evts.getAttribute('otherROBSize'     ))
    var['nROSReq'          ]=int(evts.getAttribute('nROSReq'          ))
    var['RetnROSReq'       ]=int(evts.getAttribute('RetnROSReq'       ))
    var['cachednROSReq'    ]=int(evts.getAttribute('cachednROSReq'    ))
    var['othernROSReq'     ]=int(evts.getAttribute('othernROSReq'     ))
    info.append(info)       
    return info             

def GetAlgCalls(evts):
    algs=[]
    calls = evts.childNodes
    for call in calls:
        if call.localName=="AlgCalls":
            cs = call.childNodes
            for c in cs:
                if c.localName == "algCall":
                    var={}
                    var['startTime'] =float(c.getAttribute("startTime"))
                    var['chnName']   =str(c.getAttribute("chnName"))
                    var['seqName']   =str(c.getAttribute("seqName"))
                    var['algName']   =str(c.getAttribute("algName"))
                    var['sigStep']   =int(c.getAttribute("sigStep"))
                    var['roiId']     =str(c.getAttribute("roiId"))
                    var['algStatus'] =str(c.getAttribute("algStatus"))
                    var['algTime']   =float(c.getAttribute("algTime"))
                    var['rob_nreq_retrv']=int(c.getAttribute("rob_nreq_retrv"))
                    var['rob_nids']  =int(c.getAttribute("rob_nids"))
                    var['rob_nids_retrv']=int(c.getAttribute("rob_nids_retrv"))
                    var['robTime']   =float(c.getAttribute("robTime"))
                    var['robSizeReq']=float(c.getAttribute("robSizeReq"))
                    var['robSizeRet']=float(c.getAttribute("robSizeRet"))
                    var['rosCallsRet']=str(c.getAttribute("rosCallsRet"))
                    
                    algs.append(var)
    return algs


def GetROBCalls(evt):    
    robs=[]
    calls = evt.childNodes
    for call in calls:
        if call.localName=="ROBCalls":
            cs = call.childNodes
            for c in cs:
                if c.localName == "ROBCall":
                    var={}
                    var['chnName']   =str(c.getAttribute("chnName"))
                    var['seqName']   =str(c.getAttribute("seqName"))
                    var['algName']   =str(c.getAttribute("algName"))
                    var['algType']   =str(c.getAttribute("algType"))
                    var['nRoi']      =int(c.getAttribute("nRoi"))
                    var['roiIds']    =str(c.getAttribute("roiIds"))
                    var['nDataReq']  =int(c.getAttribute("nDataReq"))
                    var['callStatus']=str(c.getAttribute("callStatus"))
                    var['reqTime']   =float(c.getAttribute("reqTime"))
                    var['wordReq']   =int(c.getAttribute("wordReq"))
                    var['sizeReq']   =float(c.getAttribute("sizeReq"))
                    robs.append(var)
    return robs

    # can do .childNodes to get the data requests inside the ROBCall
def GetROBDatas(evt):
    robs=[]
    calls = evt.childNodes
    for call in calls:
        if call.localName=="ROBCalls":
            cs = call.childNodes
            for c in cs:
                if c.localName == "ROBCall":
                    das=c.childNodes
                    for da in das:
                        if da.localName=="ROBData":
                            var={}
                            var['chnName']   =str(da.getAttribute("chnName"))
                            var['algName']   =str(da.getAttribute("algName"))
                            var['robId']     =str(da.getAttribute("robId"))
                            var['rosName']   =str(da.getAttribute("rosName"))
                            var['detName']   =str(da.getAttribute("detName"))
                            var['robReqStat']=str(da.getAttribute("robReqStat"))
                            var['robReqSize']=float(da.getAttribute("robReqSize"))
                            var['robReqWord']=int(da.getAttribute("robReqWord"))
                            var['roiIds']    =str(da.getAttribute("roiIds"))
                            robs.append(var)
    return robs


if __name__=="__main__":

    print "Reading Event"
    fil = ReadFile()

    evts     = ReadEvent(fil)
    algCalls = GetAlgCalls(fil)
    robCalls = GetROBCalls(fil)
    robDatas = GetROBDatas(fil)

    for evt in evts:
        print "Event statics:",evt

    print "Number of AlgCalls:",len(algCalls)
    print "Number of ROBCalls:",len(robCalls)
    print "Number of ROBCalls:",len(robDatas)

    if options.printEvt:
        print "Event"
        print "AlgCalls",algCalls
        print "robCalls",robCalls
    
#figures
    fig =plt.figure()    
    fig2=plt.figure()
    fig3=plt.figure()

#define plots
    halgTime    = fig.add_subplot(111)
    halgTime.set_title('alg times')

    hrobReqWord = fig2.add_subplot(111)
    hrobReqWord.set_title('words requested')

    hmuAlgTime  = fig3.add_subplot(111)
    hmuAlgTime.set_title('time for muon algs')

#calculate variables

    algTimes=[]
    for algCall in algCalls:
        algTimes.append(algCall['algTime'])

    muAlgTimes=[]
    for algCall in algCalls:
        if algCall['chnName'].count('mu'):
            muAlgTimes.append(algCall['algTime'])

    robTimes=[]
    for rob in robCalls:
        robTimes.append(rob['wordReq'])

#fill plots
    halgTime.hist(algTimes,bins=200,log=True)
    xlabel('time (ms)')
    ylabel('# algs')
    hmuAlgTime.hist(muAlgTimes,bins=200,log=True)
    xlabel('# words')
    ylabel('# ROB Requests')
    hrobReqWord.hist(robTimes,bins=200)
    xlabel('time (ms)')
    ylabel('# Events')

#
# Show plots
#
    plt.show()
