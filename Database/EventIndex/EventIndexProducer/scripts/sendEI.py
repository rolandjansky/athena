#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

# logger
log=None

class EIFile(object):

    _db=None
    _nentries=0

    def __init__(self,fname):
        try:
            self._db = dbsqlite.open(fname,flags='r')
        except:
            log.error("Unable to open EI  file %s" % fname)
            raise

        try:
            self._nentries=self._db['Nentries'] 
        except:
            log.error("Unable to get nentries from EI file %s" % fname)
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
                l.append(self._db["Entry_%d"%i])
            return l
        elif isinstance( key, int ) :
            if key < 0 : #Handle negative indices
                key += self._nentries
            if key >= self._nentries :
                raise IndexError, "The index (%d) is out of range."%key
            return self._db["Entry_%d"%key]
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
            evt=self.db['Entry_%d'%self.current]
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
        r.append("%x"%int(d['tech'],16))
        r.append("%x"%int(d['oid1'],16))
        r.append("%x"%int(d['oid2'],16))
        return r

    
    def setEvtCommon(self,ec):
        ecnames=[('PandaID','a'),('PanDA_TaskID','b'),("AMITag",'c'),("ProjName",'d'),("TrigStream",'e')]
        ecn = dict(ecnames)
        self._ec_next={}              # valid starting from next append
        for k,v in ec.iteritems():
            self._ec_next[ecn[k]]=v


    def append(self,e):

        if self._ec_next is not None:
            self._ec = self._ec_next
            self._ec_next = None

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
        e['d']=int(evt['Lvl1ID'])

        e['i0']=int(evt['IsSimulation'])
        e['i1']=int(evt['IsCalibration'])
        e['i2']=int(evt['IsTestBeam'])

        # trigger
        if self._inctrigger:
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
                ref="Sref%d"%idx
                nam="Snam%d"%idx
                if evt[ref] is not None:
                    tkc = self._tk2components(evt[ref])
                    if tkc is None:
                        continue
                    for letter,component in zip (['a','b','c','d','e','f'],tkc):
                        e[letter+str(idx)] = component
                    e['x%d'%idx]=evt[nam]


        
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
        bd['el']=self._evtlist

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
        self.seqid = uuid.uuid4().hex  # define a new identifier for this connection


        if self.dummy:
            return

        log.debug("Broker IPs and ports to connect:")
        for broker in self.brokers:
            log.debug("   %15s   %s"%broker)

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

        log.debug("Sending stats message: %s", msg)
        if not self.dummy:
            self.conn.send(message=msg, destination=self.queue2)
        if self.verbose > 2:
            print >> sys.stderr, msg
        
    def sendMSG(self,last=False):


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
        log.debug("Sending message: %s:%-5d  len: %d",self.seqid,self.seq,len(msg))

        if not self.dummy:
            if self.seq == 0:
                self.transactionID=self.conn.begin()   # start transaction before first message is sent
            self.conn.send(message=msg, destination=self.queue, transaction=self.transactionID, 
                           JMSXGroupID=self.seq)

        if self.verbose > 2:
            print >> sys.stderr, msg

        self.seq += 1  # increment sequence for nex message

        if last:
            if self.conn is not None:
                self.conn.commit(transaction=self.transactionID)     # commit transaction

        self.blocks=[]

        return len(msg)

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
            log.error("Invalid port %s",port)
            continue
            #raise Exception("Invalid port %s",port)
        try:
            (h, a, ip)=socket.gethostbyname_ex(host)
        except:
            log.error("Host can not be resolved %s",host)
            #raise Exception("Invalid host %s",host)
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
    parser.add_argument('-q','--queue', default='/queue/atlas.eventindex', help="broker queue name")
    parser.add_argument(     '--queue2', default=None, help="broker queue name for statistics")
    parser.add_argument('-u','--user',default=None, help="Stomp user name")
    parser.add_argument('-k','--passcode',default=None,help="Stomp passcode")
    parser.add_argument('-v','--verbose', action='count', help='Verbosity level')
    parser.add_argument('-d','--debug', action='count', help='Debug')
    parser.add_argument("-t","--trigger", action='store_true', help="Include trigger information (default: false)")
    parser.add_argument("-p","--provenance", action='store_true', help="Include provenance information (default: true)")
    parser.add_argument("--no-trigger", action='store_false', dest='trigger', help="Do not include trigger information")
    parser.add_argument("--no-provenance", action='store_false', dest='provenance', help="Do not include provenance information")
    parser.add_argument("-n","--dummy", action='store_true', default=False, help="Do not send messages and do not connect")
    parser.add_argument("--ssl", action='store_true', default=False,help="SLL connection")
    parser.add_argument('--keyfile',default=None,help="SSL Private Key file")
    parser.add_argument('--certfile',default=None,help="SSL Cert file")

    parser.add_argument('eifile', help="EventIndex file")

    # default for trigger and provenance
    parser.set_defaults(trigger=False)
    parser.set_defaults(provenance=True)

    # parse args
    opt=parser.parse_args()

    if opt.queue2 is None:
        opt.queue2 = opt.queue+"2"

    if opt.ssl:
        if opt.keyfile is None or opt.certfile is None:
            log.error("keyfile and certfile must be specified to use SSL")
            sys.exit(1)

    if opt.user is None  or opt.passcode is None:
        if not(opt.user is None  and opt.passcode is None):
            log.error("Both, user and passcode must be specified or none")
            sys.exit(1)

    return opt



def main():    

    # logger
    global log
    log = logging.getLogger('sendEI.py')

    # analyze options
    opt = options(sys.argv)

    if opt.verbose > 0:
        log.setLevel(logging.INFO)

    if opt.debug > 0:
        log.setLevel(logging.DEBUG)
        
    # open EI file
    fname = opt.eifile
    if not (path.isfile(fname) and access(fname, R_OK)):
        log.error("Event Index file %s does not exists or is not readble"%fname)
        sys.exit(1)
    try:
        eif = EIFile(fname)
    except:
        log.error("Unable to get info from EI file %s"%fname)
        sys.exit(1)


    # stats
    stats.ntot=0
    stats.tot_size=0
    stats.start_time=0
    stats.end_time=0
    stats.nmsg=0

    # check supported versions of EI file
    if  int(eif['Version']) > 1:
        log.error("Unable to process EI file version %s" % eif['Version'])
        sys.exit(1)


    # dump info if verbose
    if opt.verbose >0:
        log.info("Event Index file contents")
        log.info("-------------------------")
        log.info(" Version: %s" % eif['Version'])
        log.info(" Schema: %s" % eif['Schema'])
        log.info(" #input files: %s" % eif['Nfiles'])
        log.info(" Total number of events: %s" % eif['Nentries'])
        log.info(" StartProcTime: %s" % eif['StartProcTime'])
        log.info(" EndProcTime: %s" % eif['EndProcTime'])
        log.info(" PandaID: %s" % eif['PandaID'])
        log.info(" PanDA_TaskID: %s" % eif['PanDA_TaskID'])
        log.info(" Includes Provenance: %s" % eif['ProvenanceRef'])
        log.info(" Includes Trigger: %s" % eif['TriggerInfo'])
        

        for i in xrange(eif['Nfiles']):
            log.info("")
            log.info("File %d"%i)
            if 'Nentries_%d'%i in eif:
                log.info("  Events in this file: %s" % eif['Nentries_%d'%i])
            log.info("  StartProcTime: %s" % eif['StartProcTime_%d'%i])
            if 'EndProcTime_%d'%i in eif:
                log.info("  EndProcTime: %s" % eif['EndProcTime_%d'%i])
            else:
                log.info("  EndProcTime: %s" % eif['EndProcTime'])
            if "AMITag" in eif:
                log.info("  AMITag: %s" % eif['AMITag_%d'%i])
            if "TrigStream_%d"%i in eif:
                log.info("  TrigStream: %s" % eif['TrigStream_%d'%i])
            if "ProjName_%d"%i in eif:
                log.info("  ProjName: %s" % eif['ProjName_%d'%i])

        log.info("")


    # check compatibility of options
    if opt.provenance and not eif['ProvenanceRef']:
        log.error("Unable to send provenance information since it is missing from EI file")
        sys.exit(1)
    if opt.trigger and not eif['TriggerInfo']:
        log.error("Unable to send trigger information since it is missing from EI file")
        sys.exit(1)


    schema = eif['Schema']
    ei5=EI5(schema)

    #pass options 
    ei5.setIncTrigger(opt.trigger)
    ei5.setIncProvenance(opt.provenance)

    # connect to broker
    stats.start_time=int(time.time() * 1000)
    mbroker=MSG(opt)
    try:
        mbroker.connect()
    except stomp.exception.ConnectFailedException, e:
        log.error("Unable to connect to stomp broker")
        sys.exit(1)
 

    nfiles = eif['Nfiles']
    # new event iterator
    if opt.evtmax != 0:
        eifiter=eif.iterE(opt.evtmax)    # limit number of events to get
    else:
        eifiter=eif.iterE()              # all events


    events_tot = eif['Nentries']  # total number of events
    proc_events_tot = 0           # total events procesed
    proc_events_bunch = 0         # events procesed in bunch

    # get list of files
    fdata=[]
    for nf in xrange(nfiles):
        if 'Nentries_%d'%nf in eif:
            nevents = eif['Nentries_%d'%nf]
        else:
            nevents = events_tot
            
        # new common values
        evtcommon={}
        if "AMITag_%d"%nf in eif:
            evtcommon['AMITag']=eif['AMITag_%d'%nf]
        if "ProjName_%d"%nf in eif:
            evtcommon['ProjName']=eif['ProjName_%d'%nf]
        if "TrigStream_%d"%nf in eif:
            evtcommon['TrigStream']=eif['TrigStream_%d'%nf]
            
        evtcommon['PandaID']=eif['PandaID']
        evtcommon['PanDA_TaskID']=eif['PanDA_TaskID']
        fdata.append((nevents,evtcommon))

    i = 0
    nf = 0
    log.info("Processing file %d"%nf)
    (nevents,evtcommon) = fdata.pop(0)
    ei5.setEvtCommon(evtcommon)
    last_in_file = nevents
    if opt.evtmax != 0:
        last_event = opt.evtmax
    else:
        last_event = events_tot
    if last_in_file > last_event:
        last_in_file = last_event
    force_send = False
    while True:
        # get event
        try:
            evt = eifiter.next()
        except:
            evtlast=True  # no more events
            break

        i += 1
        stats.ntot += 1

        sz = ei5.append(evt)    # append event to block
        
        # last event in input file ?
        if i == last_in_file:
            log.info("Last event in file %d"%nf)
            nf += 1
            # prepare for next file
            try:
                (nevents,evtcommon) = fdata.pop(0)
            except:
                (nevents,evtcommon) = (0,{})
            last_in_file += nevents
            if last_in_file > last_event:
                last_in_file = last_event
            ei5.setEvtCommon(evtcommon)
            force_send=True

        # send block is greather enough or force_send
        if sz >= opt.msize or force_send:
            #print "sendig buffer"
            force_send = False
            blk = ei5.getEvtBlock()
            mbroker.addBlock(blk)
            if i == last_event:
                #print "LAST"
                msz = mbroker.sendMSG(last=True)
            else:
                msz = mbroker.sendMSG()
            stats.nmsg += 1
            stats.tot_size += msz


    # at this point, buffer should be empty
    blk = ei5.getEvtBlock()
    if blk is not None:
        log.error("Buffer is not empty")
    stats.end_time=int(time.time() * 1000)

    # send stats to alternate queue
    #     PandaID,PanDA_TaskID,start_time,end_time,#evts,#msg,totsize
    msgst="EISTATS0;%s;%s;%s;%s;%s;%s;%s"%(
        eif['PandaID'],eif['PanDA_TaskID'],
        stats.start_time,stats.end_time,
        stats.ntot,stats.nmsg,stats.tot_size)
    mbroker.sendMSGstats(msgst)

    mbroker.close()

    if opt.verbose > 0:
        log.info("======== summary")
        log.info(" number of events:     %10d" % stats.ntot)
        log.info(" Sun of message sizes: %10d bytes" % stats.tot_size)
        log.info(" mean size per evt:    %10.1f bytes" % (float(stats.tot_size)/int(stats.ntot)))
        log.info(" number of messages:   %10d " % stats.nmsg)
        log.info(" mean message size:    %10.2f Kbytes" % (float(stats.tot_size)/int(stats.nmsg)/1000.))
        log.info(" mean evts per msg:    %10.2f" % (float(stats.ntot)/int(stats.nmsg)))
        dt = int(stats.end_time - stats.start_time)
        log.info(" connected time:   %d ms" % dt)
        log.info(" BW %10.2f KB/s" % (float(stats.tot_size)/(dt)))
        log.info(" BW %10.2f msg/s" % (float(stats.nmsg)/(dt)*1000))
        log.info(" BW %10.2f evt/s" % (float(stats.ntot)/(dt)*1000))


if __name__ == '__main__':

    # logger setup
    root = logging.getLogger()
    ch = logging.StreamHandler(sys.stdout)
    ch.setLevel(logging.DEBUG)
    #formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    formatter = logging.Formatter('%(name)-15s %(levelname)9s %(message)s')
    ch.setFormatter(formatter)
    root.addHandler(ch)

    main()

    
    

