#!/usr/bin/env python 
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import gzip
import time
import uuid
import os
import sys

try:
    import google.protobuf
except:
    sys.path.insert(1, '/cvmfs/atlas.cern.ch/repo/sw/external/protobuf/2.3.0/lib/python2.7/site-packages/')
    import google.protobuf

import EventIndexProducer.EventIndex_pb2 as EventIndex_pb2
from EventIndexProducer.EIPBof import EIPBof

import struct
import argparse


class RunEvtRanges():

    def __init__(self):
        self.runs={}

    def update(self,run_number,evt_number):
        if run_number not in self.runs:
            self.runs[run_number] = (sys.maxint,-1)
        (min, max) = self.runs[run_number]
        if evt_number > max:
            max = evt_number
        if evt_number < min:
            min = evt_number
        self.runs[run_number] = (min, max)

    def getRanges(self):
        return self.runs

class SpbFile():

    def __init__(self, filename):
        
        self.info={}
        self.info['size'] = os.path.getsize(filename)

        try:
            self.eispb = gzip.open(filename, 'rb')
        except Exception, e:
            raise Exception(e)

        # buffer to read file
        self.buf   = ""   # read buffer
        self.blen  = 0    # buffer length
        self.bpos  = 0    # buffer current read position
        self.usize = 0    # aproximate uncompressed size
        try:
            self.version = self.readMagicVersion()
        except Exception, e:
            raise Exception(e)
        
    def getVersion(self):
        return self.version

    def readMagicVersion(self):
        self.growBufToSize(8)
        magic = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        version = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        if magic != 0x6e56c8c7:
            raise Exception("Invalid magic on SPB file")
        return version
        
    def growBufToSize(self, size):
        # throw bytes away until current read position
        self.buf  = self.buf[self.bpos:]
        self.bpos = 0
        self.blen = len(self.buf)
        # more bytes needed ?
        if self.blen < size:
            self.buf += self.eispb.read(size-self.blen)
            self.usize += size-self.blen
            self.blen = len(self.buf)
        
    def getMsg(self):
        self.growBufToSize(4)
        mtype = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4

        self.growBufToSize(4)
        mlen = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4

        self.growBufToSize(mlen)
        msg = self.buf[self.bpos:self.bpos+mlen]
        self.bpos += mlen

        #print mtype, mlen
        return (mtype,mlen,msg)

    def close(self):
        self.eispb.close()
        
    def rewind(self):
        self.eispb.rewind()
        self.buf   = ""   # read buffer
        self.blen  = 0    # buffer length
        self.bpos  = 0    # buffer current read position
        self.usize = 0    # aproximate uncompressed size
        # skip magic and version
        self.growBufToSize(8)
        self.bpos += 8

    def tell(self):
        self.eispb.tell()

    def getInfo(self):

        self.info['guids']=[]
        fileno=0
        tot_nevents = 0
        tot_nuevents = 0
        tot_nbytes = 0
        
        """ 
        SPB file format is a stream of google's protocol buffer messages 
        Reading is sequential, no random read of messages is possible
        """
        eipbof = EIPBof(self.version)
        self.info['version'] = self.version

        while True:
            (mtype_ver,mlen,msg) = self.getMsg()
            mtype = (mtype_ver & 0x000fff00) >> 8
            ver   = (mtype_ver & 0x000000ff)

            if mtype == 1:                                   # Header
                header = eipbof.Header()
                header.ParseFromString(msg)
                self.info['jobID']         = header.jobID
                self.info['taskID']        = header.taskID
                self.info['startProcTime'] = header.startProcTime
                self.info['inputDsName']   = header.inputDsName
                self.info['provenanceRef'] = header.provenanceRef
                self.info['triggerInfo']   = header.triggerInfo

            elif mtype == 3:                                 # Begin GUID
                beginGUID = eipbof.BeginGUID()
                beginGUID.ParseFromString(msg)
                currentGUID = {}
                currentGUID['fileStartProcTime'] = beginGUID.startProcTime
                currentGUID['guid']              = beginGUID.guid
                currentGUID['fileno']            = fileno
                fileno  += 1
                nevents  = 0
                nuevents = 0
                nbytes   = 0
                # run-evt, used to calculate unique events
                runevt={}
                # to calculate run evt ranges
                runevtRanges = RunEvtRanges()
                #
                currentGUID['AMITag'] = beginGUID.AMITag
                currentGUID['trigStream'] = beginGUID.trigStream
                currentGUID['projName'] = beginGUID.projName
            elif mtype == 4:                                 # End GUID
                endGUID = eipbof.EndGUID()
                endGUID.ParseFromString(msg)
                currentGUID['nevents']           = nevents
                currentGUID['nuevents']          = nuevents
                currentGUID['fileEndProcTime']   = endGUID.endProcTime
                currentGUID['runevtRanges']      = runevtRanges.getRanges()
                self.info['guids'].append(currentGUID)
                tot_nevents  += nevents
                tot_nuevents += nuevents
                tot_nbytes   += nbytes
            elif mtype == 6:                                 # event
                eventPB = eipbof.EIEvent()
                eventPB.ParseFromString(msg)
                nevents += 1
                nbytes  += mlen
                runevtRanges.update(int(eventPB.runNumber),int(eventPB.eventNumber))
                runevtkey = "{0:08d}-{1:011d}".format(int(eventPB.runNumber),int(eventPB.eventNumber))
                if runevtkey not in runevt:
                    runevt[runevtkey] = 1
                    nuevents += 1
            elif mtype == 5:                                 # Trigger menu
                triggerMenu = eipbof.TriggerMenu()
                triggerMenu.ParseFromString(msg)
            elif mtype == 2:                                 # Trailer
                trailer = eipbof.Trailer()
                trailer.ParseFromString(msg)
                self.info['endProcTime'] = trailer.endProcTime
                self.info['nevents']     = trailer.nentries
                self.info['nfiles']      = trailer.nfiles
                break
            else:
                break

        self.info['usize']    =  self.usize
        self.info['nuevents'] = tot_nuevents
        self.rewind()

        # checkings
        if self.info['nfiles'] != fileno:
            log.error("ERROR nfiles {} {}".format(fileno, self.info['nfiles']))
        if self.info['nevents'] != tot_nevents:
            log.error("ERROR tot_nevents {} {}".format(tot_nevents, self.info['nevents']))
            
            
        return self.info


def options(argv):

    parser = argparse.ArgumentParser(description='Unpick Event Index File')
    parser.add_argument('-e','--evtnumber', default=0, type=int, help='event number to dump')
    parser.add_argument('-r','--runnumber', default=0, type=int, help='run number to dump')
    parser.add_argument('-m','--evtmax', default=1, type=int, help='Max number of events to dump')
    parser.add_argument('--decodetrig0',action='count', help='decode trigger. only bits fired')
    parser.add_argument('--decodetrig1',action='count', help='decode trigger. Full bit stream')
    parser.add_argument('--decodetrig2',action='count', help='decode trigger. bit map')
    parser.add_argument('eispbfile', help="EventIndex SPB file")

    return parser.parse_args()

def getTriggerList(s):

    tl=[]
    idx=0
    count=""
    for k in range(0,len(s)):
        c = s[k]
        if c == "!":
            if len(count)>0:
                idx += _fromB64(count)
                count=""
            tl.append(idx)
            idx+=1
        else:
            count+=c
            
    return tl

def getL1TriggerList(s):

    TAV=[]
    TAP=[]
    TBP=[]

    tl = getTriggerList(s);
    for pos in tl:
        #print pos
        if (pos > 512):
            TAV.append(pos-512)
            #print " TAV: ",pos-512
        elif (pos > 256):
            TAP.append(pos-256)
            #print " TAP: ",pos-256
        else:
            TBP.append(pos)
            #print " TBP: ",pos

    l1tl={'TAV':TAV,'TAP':TAP,'TBP':TBP}
    
    return l1tl
            
def main():
    
    # analyze options
    opt = options(sys.argv)

    fname = opt.eispbfile
    allGUIDS=[]    
    producerID = uuid.uuid4().hex
    fileno=0
    tot_nevents = 0
    tot_nuevents = 0
    tot_nbytes = 0

    try:
        spbf = SpbFile(fname)
        info = spbf.getInfo()
    except Exception, e:
        print "Error reading SPB file. Please check file: " + str(e)
        sys.exit(1)


    print ""
    print " version: ", info['version']
    print " #input files: ", info['nfiles']
    print "  number of events: ", info['nevents']
    print "  StartProcTime: ", info['startProcTime']
    print "  EndProcTime: ", info['endProcTime']
    print "  TaskID: ", info['taskID']
    print "  JobID: ", info['jobID']
    print "  InputDsName: ", info['inputDsName']
    print "  Includes Provenance: ", info['provenanceRef']
    print "  Includes Trigger: ", info['triggerInfo']
    
    for f in info['guids']:
        print ""
        print "File {:d}".format(f['fileno'])
        print "  Events in this file:             {:8d}".format(f['nevents'])
        print "  Unique events in this file:      {:8d}".format(f['nuevents'])
        print "  StartProcTime:              {:13d}".format(f['fileStartProcTime'])
        print "  EndProcTime:                {:13d}".format(f['fileEndProcTime'])
        print "  AMITag:          {}".format(f['AMITag'])
        print "  TrigStream:      {}".format(f['trigStream'])
        print "  ProjName:        {}".format(f['projName'])
        print "  RunEvtRanges:    {}".format(f['runevtRanges'])
        print "  GUID:            {}".format(f['guid'])


    triggerinfo = info['triggerInfo']
    provenanceinfo = info['provenanceRef']

    eipbof = EIPBof(spbf.getVersion())


    print ""
    print "Summary: (info for {:d} events max)".format(opt.evtmax)
    print ""

    
    nevt_shown=0
    while True:
        (mtype_ver,mlen,msg) = spbf.getMsg()
        mtype = (mtype_ver & 0x000fff00) >> 8
        ver   = (mtype_ver & 0x000000ff)
        if mtype == 1:
            header = eipbof.Header()
            header.ParseFromString(msg)
            #print "> header"
        elif mtype == 3:
            beginGUID = eipbof.BeginGUID()
            beginGUID.ParseFromString(msg)
            #print "> beginGUID"
        elif mtype == 4:
            endGUID = eipbof.EndGUID()
            endGUID.ParseFromString(msg)
            #print "> endGUID"
        elif mtype == 6:
            eventPB = eipbof.EIEvent()
            eventPB.ParseFromString(msg)
            if nevt_shown < opt.evtmax:
                print "======================================================"
                print "> eventPB"
                print eventPB
                nevt_shown += 1
            else:
                break
        elif mtype == 5:
            triggerMenu = eipbof.TriggerMenu()
            triggerMenu.ParseFromString(msg)
            #print "  > triggerMenu"
        elif mtype == 2:
            trailer = eipbof.Trailer()
            trailer.ParseFromString(msg)
            #print "> trailer"
            break
        else:
            print mtype
            break

    spbf.close()


if __name__ == '__main__':
    main()
