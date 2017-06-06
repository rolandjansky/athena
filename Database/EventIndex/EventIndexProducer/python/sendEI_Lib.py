#!/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import sys
import re
import PyUtils.dbsqlite as dbsqlite
from EventIndexProducer.compressB64 import decompressB64
import json
import stomp
import argparse
import socket
import uuid
#import pdb
import time
import logging
from os import path, access, R_OK


class Stats(object):
    pass

stats=Stats()
gstats=Stats()

# logger
log=None

class EIFile(object):

    _db=None
    _nentries=0

    def __init__(self,fname):
        try:
            self._db = dbsqlite.open(fname,flags='r')
        except:
            log.error("Unable to open Event Index file {}".format(fname))
            raise

        try:
            self._nentries=self._db['Nentries'] 
        except:
            log.error("Unable to get nentries from Event Index file {}".format(fname))
            raise

    def __getitem__(self,key):
        if isinstance( key, slice ) :
            l=[]
            start=key.start
            stop=key.stop
            if stop > self._nentries:
                stop = self._nentries
            if key.step is not None:
                step=key.step
            else:
                step=1
            for i in range(start, stop, step):
                l.append(self._db["Entry_{:d}".format(i)])
            return l
        elif isinstance( key, int ) :
            if key < 0 : #Handle negative indices
                key += self._nentries
            if key >= self._nentries :
                raise IndexError, "The index ({:d}) is out of range.".format(key)
            return self._db["Entry_{:d}".format(key)]
        else:
            if key in self._db:
                return self._db[key]
            else:
                raise TypeError(key)

    def keys(self):
        return self._db.keys()

    def __contains__(self,key):
        return self._db.__contains__(key)

    def __iter__(self):
        return EIIterator(self._db)

    def iterE(self,max=None):
        return EIIterator(self._db,max)

    def close(self):
        self._db.close()

    def getEventsPerFile(self):
        nfiles=self._db['Nfiles']
        nevents=[]
        for nf in xrange(nfiles):
            nevents.append(self._db['Nentries_{:d}'.format(nf)])
        return nevents

class EIIterator(object):

    def __init__(self, db, max=None):
        self.db=db
        self.current = 0
        if max is not None:
            self.high = int(max)
            if self.high > self.db['Nentries']:
                self.high = self.db['Nentries']
        else:
            self.high = self.db['Nentries']

    def __iter__(self):
        return self

    def next(self):
        if self.current >= self.high:
            self.current = 0  # prepare for anoter iteration
            raise StopIteration
        else:
            #print "==", self.current
            evt=self.db["Entry_{:d}".format(self.current)]
            self.current += 1
            return evt

    def end(self):
        self.current = self.high

class EI5(object):

    _schema=[]
    _evtlist=[]
    _lastevt=[]
    _ec={}
    _ec_next=None
    _inctrigger=False
    _incprovenance=False

    def __init__(self,schema):
        self._schema=schema
        pass

    def _tk2components(self,tk):
        # pool token has the format:
        # [DB=dbID][CNT=cntID][CLID=classID][TECH=technology][OID=oid.first-oid.second]
        # eg.:
        #   [DB=FEAD2AD8-111F-11E2-8080-1CCD8E80BEEF][CNT=POOLContainer(DataHeader)]
        #     [CLID=D82968A1-CF91-4320-B2DD-E0F739CBC7E6][TECH=00000202][OID=00000123-00000008]
        pool_token = re.compile(r'[[]DB=(?P<db>.*?)[]]' \
                                    r'[[]CNT=(?P<cnt>.*?)[]]' \
                                    r'[[]CLID=(?P<clid>.*?)[]]' \
                                    r'[[]TECH=(?P<tech>.*?)[]]' \
                                    r'[[]OID=(?P<oid1>.*?)-(?P<oid2>.*?)[]]' \
                                    ).match
        if tk is None:
            return None
        match = pool_token(tk)
        if not match:
            return None
        d = match.groupdict()
        r=[]
        r.append(d['db'].replace('-',''))
        r.append(d['cnt'])
        r.append(d['clid'].replace('-',''))
        r.append("{:x}".format(int(d['tech'],16)))
        r.append("{:x}".format(int(d['oid1'],16)))
        r.append("{:x}".format(int(d['oid2'],16)))
        return r

    
    def setEvtCommon(self,ec):
        ecnames=[('JobID','a'),('TaskID','b'),("AMITag",'c'),("ProjName",'d'),("TrigStream",'e'),('InputDsName','f'),('GUID','g')]
        ecn = dict(ecnames)
        self._ec_next={}              # valid starting from next append
        for k,v in ec.iteritems():
            self._ec_next[ecn[k]]=v


    def append(self,e):

        if self._ec_next is not None:
            self._ec = self._ec_next
            self._ec_next = None

        if e is None:
            # empty event
            self._lastevt={}
            self._evtlist.append({})
            return 0

        evt=dict(zip(self._schema, e))

        # fill and translate field names
        e={}
        e["r"]=int(evt['RunNumber'])
        e["e"]=int(evt['EventNumber'])
        e['l']=int(evt['LumiBlockN'])
        e['t']=int(evt['EventTime'])
        e['u']=int(evt['EventTimeNanoSec'])
        e['w']=float(evt['EventWeight'])
        e['j']=int(evt['McChannelNumber'])
        e['i']=int(evt['BunchId'])

        e['i0']=int(evt['IsSimulation'])
        e['i1']=int(evt['IsCalibration'])
        e['i2']=int(evt['IsTestBeam'])

        # trigger
        if self._inctrigger:
            e['d']=int(evt['Lvl1ID'])
            e["t1"]=evt['L1PassedTrigMask']
            e["t2"]=evt['L2PassedTrigMask']
            e["te"]=evt['EFPassedTrigMask']
            if 'SMK' in evt and evt['SMK'] is not None:
                e['m']=int(evt['SMK'])
            else:
                e['m']=0
            if 'HLTPSK' in evt and evt['HLTPSK'] is not None:
                e['n']=int(evt['HLTPSK'])
            else:
                e['n']=0
            if 'L1PSK' in evt and evt['L1PSK'] is not None:
                e['o']=int(evt['L1PSK'])
            else:
                e['o']=0

        # get refrences from SnamN, SrefN
        for idx in range(4):   
            if idx == 0 or self._incprovenance:
                ref="Sref{:d}".format(idx)
                nam="Snam{:d}".format(idx)
                if evt[ref] is not None:
                    tkc = self._tk2components(evt[ref])
                    if tkc is None:
                        continue
                    for letter,component in zip (['a','b','c','d','e','f'],tkc):
                        e[letter+str(idx)] = component
                    e['x{:d}'.format(idx)]=evt[nam]


        
        # substitute None by 0
        for k,v in e.iteritems():
            if v is None:
                e[k]=0
        
        # discard redundant fields
        e2=e.copy()
        for k,v in e.iteritems():
            if k in self._lastevt and v == self._lastevt[k] == v:
                e2.pop(k,None)
        self._lastevt=e
        self._evtlist.append(e2)

        # return lenght 
        return len(json.dumps(self._evtlist,separators=(',', ':')))
    

    def setIncTrigger(self,trig):
        self._inctrigger=trig

    def setIncProvenance(self,provenance):
        self._incprovenance=provenance

    def getEvtBlock(self):

        if len(self._evtlist) == 0:
            return None

        # block: header,options,data
        #    header: type=10,version=1
        #    options: empty now
        #    data: evt_common, evt_list
        
        bh={'t':10,'v':1}  # block header
        bo={}              # block options
        bd={}              # block data
        
        # fill data
        bd["ec"]=self._ec
        if len(self._evtlist[0]) != 0:
            bd['el']=self._evtlist
        else:
            bd['el']=[]                    # empty event

        # build block
        eb={}
        eb['h']=bh   # header
        eb['o']=bo   # options
        eb['d']=bd   # data

        self._evtlist=[]  # empty list
        self._lastevt=[]  
        return eb
    

class MSG(object):
    
    def __init__(self, opt):

        self.brokers = opt.endpoint
        self.user = opt.user
        self.passcode = opt.passcode
        self.ssl = opt.ssl
        self.keyfile = opt.keyfile
        self.certfile = opt.certfile
        self.queue = opt.queue
        self.queue2 = opt.queue2
        self.verbose = opt.verbose
        self.dummy = opt.dummy

        self.conn = None
        self.seq = 0
        self.blocks = []
        self.transactionID = None

    def isConnected(self):
        if self.conn is None:
            return False
        else:
            return True
        
    def connect(self):
        #print "MSG::connect"

        # prepare for connection
        self.conn = None
        self.blocks = []
        self.seqid = None


        if self.dummy:
            return

        log.debug("Broker IPs and ports to connect:")
        for broker in self.brokers:
            log.debug("   {:15s}   {}".format(*broker))

        conn=None
        # build options
        copts={'host_and_ports':self.brokers,'wait_on_receipt':True}
        if self.user is not None and self.passcode is not None:
            log.debug("Conecting broker using user and password")
            copts['user'] = self.user
            copts['passcode'] = self.passcode
        if self.ssl:
            log.debug("Conecting broker using SSL")
            copts['use_ssl'] = True
            copts['ssl_key_file'] = self.keyfile
            copts['ssl_cert_file'] = self.certfile

        # connect
        conn = stomp.Connection(**copts)
        conn.set_listener('', MyListener())
        conn.start()
        try:
            conn.connect(wait=True)
        except stomp.exception.ConnectFailedException, e:
            # pass error to caller
            raise
        self.conn=conn


    def close(self):
        if self.conn is not None:
            self.conn.stop()
        self.conn = None

    def addBlock(self,block):
        self.blocks.append(block)

    def sendMSGstats(self,msg):

        texp=int(time.time() * 1000) + 259200000   # 72 hours from now

        log.debug("Sending stats message: {}".format(msg))

        if not self.dummy:
            self.conn.send(message=msg, destination=self.queue2, JMSExpiration=texp)
        if self.verbose > 2:
            print >> sys.stderr, msg
        
    def sendMSG(self,last=False):

        if self.seq == 0:
            self.seqid = uuid.uuid4().hex   # define a new identifier for this sequence

        ## build message
        tnow=int(time.time() * 1000)
        mh={}  # header
        mo={}  # options
        md=[]  # data

        mh["v"] = 1           # version
        mh["s"] = self.seq    # header
        mh["t"] = tnow        # time
        mh["i"] = self.seqid  # sequence id
        if last:
            mh['e']  = True  # last message in sequence
        
        # data (payload)
        md=self.blocks
        
        # message
        oo = {"h":mh,"o":mo,"d":md}
        msg = json.dumps(oo,separators=(',', ':'))


        # send message
        log.debug("Sending message: {}:{:<5d}  len: {:d}".format(self.seqid,self.seq,len(msg)))

        NON_PERSISTENT="1"
        PERSISTENT="2"

        if not self.dummy:
            if self.seq == 0:
                self.transactionID=self.conn.begin()   # start transaction before first message is sent

            if last:
                # close group
                self.conn.send(message=msg, destination=self.queue, transaction=self.transactionID, 
                               JMSXGroupID=self.transactionID, JMSXGroupSeq="-1", JMSDeliveryMode=PERSISTENT)
            else:
                self.conn.send(message=msg, destination=self.queue, transaction=self.transactionID, 
                               JMSXGroupID=self.transactionID, JMSDeliveryMode=PERSISTENT)

        if self.verbose > 2:
            print >> sys.stderr, msg

        self.seq += 1  # increment sequence for nex message

        if last:
            if self.conn is not None:
                self.conn.commit(transaction=self.transactionID)     # commit transaction
            self.seq = 0                                             # restart sequence 

        self.blocks=[]

        return len(msg)

    def getSeqID(self):
        return self.seqid

###########################
# listener 
class MyListener(object):
    def on_error(self, headers, message):
        print >> sys.stderr, 'received an error'
        print >> sys.stderr, " headers: ", headers
        print >> sys.stderr, " message: ", message

    def on_message(self, headers, message):
        print >> sys.stderr, 'received a message'
        print >> sys.stderr, " headers: ", headers
        print >> sys.stderr, " message: ", message


def endpointV(endpoint):
    # default port
    dport=60013

    # enpoint should be broker1[:port1],broker2:[port2],...
    lbroker=endpoint.split(',')
    result=[]
    for b in lbroker:
        try:
            (host,port)=b.split(":")
        except:
            host=b
            port=dport
        try:
            port=int(port)
        except:
            log.error("Invalid port {}".format(port))
            continue
            #raise Exception("Invalid port {}",port)
        try:
            (h, a, ip)=socket.gethostbyname_ex(host)
        except:
            log.error("Host can not be resolved {}".format(host))
            #raise Exception("Invalid host {}",host)
            continue
        for addr in ip:
            result.append((addr,port))

    return result
    

def options(argv):

    parser = argparse.ArgumentParser(description='SendEI to STOMP broker.')
    parser.add_argument('-e','--endpoint', default='localhost:61613', type=endpointV,
                        help="broker name and port")
    parser.add_argument('-m','--evtmax', default=0, type=int, help='Max events to process')
    parser.add_argument('-s','--msize', default=10000, type=int, help='message size')
    parser.add_argument('-q','--queue',  default='/queue/atlas.eventindex', help="broker queue name")
    parser.add_argument(     '--queue2', default='/topic/atlas.eventindex.stats', help="broker queue name for statistics")
    parser.add_argument('-u','--user',default=None, help="Stomp user name")
    parser.add_argument('-k','--passcode',default=None,help="Stomp passcode")
    parser.add_argument('-v','--verbose', action='count', help='Verbosity level')
    parser.add_argument('-d','--debug', action='count', help='Debug')
    parser.add_argument("-t","--trigger", action='store_true', help="Include trigger information (default: true)")
    parser.add_argument("-p","--provenance", action='store_true', help="Include provenance information (default: true)")
    parser.add_argument("--no-trigger", action='store_false', dest='trigger', help="Do not include trigger information")
    parser.add_argument("--no-provenance", action='store_false', dest='provenance', help="Do not include provenance information")
    parser.add_argument("-n","--dummy", action='store_true', default=False, help="Do not send messages and do not connect")
    parser.add_argument("--ssl", action='store_true', default=False,help="SLL connection")
    parser.add_argument('--keyfile',default=None,help="SSL Private Key file")
    parser.add_argument('--certfile',default=None,help="SSL Cert file")
    parser.add_argument('-g','--guids', default=[], nargs='+', help="GUID to send")

    parser.add_argument('eifile', help="EventIndex file")

    # default for trigger and provenance
    parser.set_defaults(trigger=True)
    parser.set_defaults(provenance=True)

    # parse args
    opt=parser.parse_args(args=argv)

    if opt.ssl:
        if opt.keyfile is None or opt.certfile is None:
            log.error("keyfile and certfile must be specified to use SSL")
            sys.exit(1)

    if opt.user is None  or opt.passcode is None:
        if not(opt.user is None  and opt.passcode is None):
            log.error("Both, user and passcode must be specified or none")
            sys.exit(1)

    return opt


def eimrun(logger,opt):

    # logger
    global log
    log=logger

    # producerID
    producerID = uuid.uuid4().hex

    # open EI file
    fname = opt.eifile
    if not (path.isfile(fname) and access(fname, R_OK)):
        log.error("Event Index file {} does not exists or is not readble".format(fname))
        sys.exit(1)
    try:
        eif = EIFile(fname)
    except:
        log.error("Unable to get info from EI file {}".format(fname))
        sys.exit(1)


    # grand total stats
    gstats.ntot=0
    gstats.uniq_ntot=0
    gstats.tot_size=0
    gstats.start_time=0
    gstats.end_time=0
    gstats.nmsg=0

    # check supported versions of EI file
    if  int(eif['Version']) > 1:
        log.error("Unable to process EI file version {}".format(eif['Version']))
        sys.exit(1)


    # dump info if verbose
    if opt.verbose > 0:
        log.info("===================== sendEI ====================")
        log.info(" endpoint: {}".format(opt.endpoint))
        log.info(" queue: {}".format(opt.queue))
        log.info("Event Index file contents")
        log.info("-------------------------")
        log.info(" Version: {}".format(eif['Version']))
        log.info(" Schema: {}".format(eif['Schema']))
        log.info(" #input files: {}".format(eif['Nfiles']))
        log.info(" Total number of events: {}".format(eif['Nentries']))
        log.info(" StartProcTime: {}".format(eif['StartProcTime']))
        log.info(" EndProcTime: {}".format(eif['EndProcTime']))
        log.info(" TaskID: {}".format(eif['TaskID']))
        log.info(" JobID: {}".format(eif['JobID']))
        log.info(" InputDsName: {}".format(eif['InputDsName']))
        log.info(" Includes Provenance: {}".format(eif['ProvenanceRef']))
        log.info(" Includes Trigger: {}".format(eif['TriggerInfo']))
        

        for i in xrange(eif['Nfiles']):
            log.info("")
            log.info("File {:d}".format(i))
            if 'Nentries_{:d}'.format(i) in eif:
                log.info("  Events in this file: {}".format(eif['Nentries_{:d}'.format(i)]))
            log.info("  StartProcTime: {}".format(eif['StartProcTime_{:d}'.format(i)]))
            if 'EndProcTime_{:d}'.format(i) in eif:
                log.info("  EndProcTime: {}".format(eif['EndProcTime_{:d}'.format(i)]))
            else:
                log.info("  EndProcTime: {}".format(eif['EndProcTime']))
            if "AMITag_{:d}".format(i) in eif:
                log.info("  AMITag: {}".format(eif['AMITag_{:d}'.format(i)]))
            if "TrigStream_{:d}".format(i) in eif:
                log.info("  TrigStream: {}".format(eif['TrigStream_{:d}'.format(i)]))
            if "ProjName_{:d}".format(i) in eif:
                log.info("  ProjName: {}".format(eif['ProjName_{:d}'.format(i)]))
            if "GUID_{:d}".format(i) in eif:
                log.info("  GUID: {}".format(eif['GUID_{:d}'.format(i)]))

        log.info("")


    # check compatibility of options
    if opt.provenance and not eif['ProvenanceRef']:
        log.info("Unable to send provenance information since it is missing from EI file")
        opt.provenance = False

    if opt.trigger and not eif['TriggerInfo']:
        log.info("Unable to send trigger information since it is missing from EI file")
        opt.trigger = False


    schema = eif['Schema']
    run_index = schema.index('RunNumber')
    evt_index = schema.index('EventNumber')
    ei5=EI5(schema)

    #pass options 
    ei5.setIncTrigger(opt.trigger)
    ei5.setIncProvenance(opt.provenance)

    # connect to broker
    gstats.start_time=int(time.time() * 1000)
    mbroker=MSG(opt)
    try:
        mbroker.connect()
    except stomp.exception.ConnectFailedException, e:
        log.error("Unable to connect to stomp broker")
        sys.exit(1)
 

    nFiles = eif['Nfiles']
    nEventsPerFile = eif.getEventsPerFile()
    if opt.evtmax != 0:                                 # truncate list if max events <> 0
        nEvtTot = 0
        for nf in xrange(nFiles):
            nEvtTot += nEventsPerFile[nf]
            if nEvtTot >= opt.evtmax:
                nEventsPerFile[nf] = nEventsPerFile[nf] - nEvtTot + opt.evtmax
                nFiles = nf + 1
                nEventsPerFile = nEventsPerFile[0:nf+1]
                break

    proc_events_tot = 0                                 # total events procesed
    nEvtFrom = nEvtTo = 0                               # ranges to process
    fileno = 0                                          # file number sent

    for nf in xrange(nFiles):                           # loop over files
        log.info("Processing file {:d}".format(nf))

        # stats
        stats.ntot=0
        stats.uniq_ntot=0
        stats.tot_size=0
        stats.nmsg=0
        stats.start_time=int(time.time() * 1000)
        
        # run-evt, use to calculate unique events
        runevt={}

        # new file, new common values
        evtcommon={}
        if "AMITag_{:d}".format(nf) in eif:
            evtcommon['AMITag']=eif['AMITag_{:d}'.format(nf)]
        if "ProjName_{:d}".format(nf) in eif:
            evtcommon['ProjName']=eif['ProjName_{:d}'.format(nf)]
        if "TrigStream_{:d}".format(nf) in eif:
            evtcommon['TrigStream']=eif['TrigStream_{:d}'.format(nf)]
        if "GUID_{:d}".format(nf) in eif:
            evtcommon['GUID']=eif["GUID_{:d}".format(nf)]

        fileStartProcTime = 0
        fileEndProcTime = 0
        if 'StartProcTime_{:d}'.format(nf) in eif:
            fileStartProcTime = eif['StartProcTime_{:d}'.format(nf)]
        if 'EndProcTime_{:d}'.format(nf) in eif:
            fileEndProcTime = eif['EndProcTime_{:d}'.format(nf)]

        # send only selected GUIDs if present on arguments 
        if len(opt.guids) != 0 and evtcommon['GUID'] not in opt.guids:
            log.info("SKIP FILE {}".format(evtcommon['GUID']))
            nevt = nEventsPerFile[nf]
            nEvtTo += nevt
            nEvtFrom = nEvtTo
            continue
            
        evtcommon['TaskID']=eif['TaskID']
        evtcommon['JobID']=eif['JobID']
        evtcommon['InputDsName']=eif['InputDsName']
        ei5.setEvtCommon(evtcommon)

        nevt = nEventsPerFile[nf]
        nEvtTo += nevt
        fileno += 1

        for i in xrange(nEvtFrom, nEvtTo):     # loop over events in file
            evt=eif[i]

            # unique events
            runevtkey="{0:08d}-{1:011d}".format(int(evt[run_index]),int(evt[evt_index]))
            if runevtkey not in runevt:
                runevt[runevtkey] = 1
                stats.uniq_ntot += 1

            #print nf, i, evt[0],evt[1]
            sz = ei5.append(evt)    # append event to block
            stats.ntot += 1
            # send block if greather enough or last event
            if sz >= opt.msize or i == (nEvtTo - 1):
                #print "sendig buffer", nf, i, evt[0],evt[1]
                blk = ei5.getEvtBlock()
                mbroker.addBlock(blk)
                if i == (nEvtTo - 1):
                    #print "LAST"
                    msz = mbroker.sendMSG(last=True)
                else:
                    msz = mbroker.sendMSG()
                stats.nmsg += 1
                stats.tot_size += msz

        # patch: if file has no events at all, send empty message
        if nEvtFrom == nEvtTo:
            ei5.append(None)            # add empty event
            blk = ei5.getEvtBlock()
            mbroker.addBlock(blk)
            msz = mbroker.sendMSG(last=True)
            stats.nmsg += 1
            stats.tot_size += msz
            
        stats.end_time=int(time.time() * 1000)

        # send stats to alternate queue
        #     JobID,TaskID,start_time,end_time,#evts,#msg,totsize
        msgst="EISTATS0;{};{};{};{};{};{};{};{};{};{};{};{};{};{}".format(
            producerID,
            eif["GUID_{:d}".format(nf)],
            eif['JobID'],eif['TaskID'],
            mbroker.getSeqID(),
            stats.start_time,stats.end_time,
            fileStartProcTime,fileEndProcTime,fileno,
            stats.ntot,stats.uniq_ntot,stats.nmsg,stats.tot_size)
        mbroker.sendMSGstats(msgst)

        log.info("Last event in file {:d}".format(nf))

        if opt.verbose > 0:
            log.info("=========== sendEI summary ==========")
            log.info(" number of events:     {:10d}".format(stats.ntot))
            log.info(" number of unique evt: {:10d}".format(stats.uniq_ntot))
            log.info(" Sun of message sizes: {:10d} bytes".format(stats.tot_size))
            if int(stats.ntot) != 0:
                log.info(" mean size per evt:    {:10.1f} bytes".format((float(stats.tot_size)/int(stats.ntot))))
            log.info(" number of messages:   {:10d} ".format(stats.nmsg))
            if int(stats.nmsg) != 0:
                log.info(" mean message size:    {:10.2f} Kbytes".format((float(stats.tot_size)/int(stats.nmsg)/1000.)))
                log.info(" mean evts per msg:    {:10.2f}".format((float(stats.ntot)/int(stats.nmsg))))
            dt = int(stats.end_time - stats.start_time)
            log.info(" connected time:   {:d} ms".format(dt))
            if dt != 0:
                log.info(" BW {:10.2f} KB/s".format((float(stats.tot_size)/(dt))))
                log.info(" BW {:10.2f} msg/s".format((float(stats.nmsg)/(dt)*1000)))
                log.info(" BW {:10.2f} evt/s".format((float(stats.ntot)/(dt)*1000)))


        # grand stats
        gstats.ntot += stats.ntot
        gstats.uniq_ntot += stats.uniq_ntot
        gstats.tot_size += stats.tot_size
        gstats.nmsg += stats.nmsg

        # reset stats
        stats.ntot=0
        stats.uniq_ntot=0
        stats.tot_size=0
        stats.nmsg=0
        stats.start_time=stats.end_time
        
        nEvtFrom = nEvtTo                          # next file


    # at this point, buffer should be empty
    blk = ei5.getEvtBlock()
    if blk is not None:
        log.error("Buffer is not empty")

    gstats.end_time=int(time.time() * 1000)

    if fileno > 1:
        # send stats to alternate queue
        #     JobID,TaskID,start_time,end_time,#evts,#msg,totsize
        msgst="EISTATS1;{};{};{};{};{};{};{};{};{};{};{};{};{};{}".format(
            producerID,
            '00000000-0000-0000-0000-000000000000',
            eif['JobID'],eif['TaskID'],
            "00000000000000000000000000000000",
            gstats.start_time,gstats.end_time,
            0,0,fileno,
            gstats.ntot,gstats.uniq_ntot,gstats.nmsg,gstats.tot_size)
        mbroker.sendMSGstats(msgst)

    mbroker.close()

    if nFiles > 1 and opt.verbose > 0:
        log.info("=========== sendEI summary grand total ==========")
        log.info(" number of events:     {:10d}".format(gstats.ntot))
        log.info(" number of unique evt: {:10d}".format(gstats.uniq_ntot))
        log.info(" Sun of message sizes: {:10d} bytes".format(gstats.tot_size))
        if int(gstats.ntot) != 0:
            log.info(" mean size per evt:    {:10.1f} bytes".format((float(gstats.tot_size)/int(gstats.ntot))))
        log.info(" number of messages:   {:10d} ".format(gstats.nmsg))
        if int(gstats.nmsg) != 0:
            log.info(" mean message size:    {:10.2f} Kbytes".format((float(gstats.tot_size)/int(gstats.nmsg)/1000.)))
            log.info(" mean evts per msg:    {:10.2f}".format((float(gstats.ntot)/int(gstats.nmsg))))
        dt = int(gstats.end_time - gstats.start_time)
        log.info(" connected time:   {:d} ms".format(dt))
        if dt != 0:
            log.info(" BW {:10.2f} KB/s".format((float(gstats.tot_size)/(dt))))
            log.info(" BW {:10.2f} msg/s".format((float(gstats.nmsg)/(dt)*1000)))
            log.info(" BW {:10.2f} evt/s".format((float(gstats.ntot)/(dt)*1000)))


