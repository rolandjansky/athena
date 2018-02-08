#!/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import sys
import os
import json
import stomp
import socket
import uuid
import time
import logging
import gzip
from urlparse import urlparse

try:
    import google.protobuf
except:
    import sys
    sys.path.insert(1, '/cvmfs/atlas.cern.ch/repo/sw/external/protobuf/2.3.0/lib/python2.7/site-packages/')
    import google.protobuf

# import after google.protobuf to avoid warnings
import argparse

#import EventIndexProducer.EventIndex_pb2 as EventIndex_pb2
from EventIndexProducer.EIPBof import EIPBof
import struct

# import boto libraries
try:
    import boto
except:
    sys.path.insert(1, '/cvmfs/atlas.cern.ch/repo/sw/external/boto/lib/python2.6/site-packages/')
    import boto
import boto.s3.connection
from boto.s3.key import Key


class MSG(object):
    
    def __init__(self, opt):

        self.brokers = opt.endpoint
        self.user = opt.user
        self.passcode = opt.passcode
        self.queue = opt.queue
        self.verbose = opt.verbose
        self.dummy = opt.dummy

        self.conn = None

    def isConnected(self):
        if self.conn is None:
            return False
        else:
            return True
        
    def connect(self):

        # prepare for connection
        self.conn = None

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

    def sendMSG(self,msg):

        log.debug("Sending message. Len: {}".format(len(msg)))

        NON_PERSISTENT="1"
        PERSISTENT="2"

        if not self.dummy:
            self.conn.send(message=msg, destination=self.queue, JMSDeliveryMode=PERSISTENT, JMSExpiration=0)
        if self.verbose > 2:
            print >> sys.stderr, msg
        

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
        self.version = self.readMagicVersion()

    def getVersion(self):
        return self.version

    def readMagicVersion(self):
        self.growBufToSize(8)
        magic = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        version = struct.unpack('<I',self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        if magic != 0x6e56c8c7:
            raise IOError("Invalid magic in EISPB file")
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
        return (mtype, mlen, msg)

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
            log.info("ERROR nfiles {} {}".format(fileno, self.info['nfiles']))
        if self.info['nevents'] != tot_nevents:
            log.info("ERROR tot_nevents {} {}".format(tot_nevents, self.info['nevents']))
            
            
        return self.info


def osCopy(logger, opt, info):

    # logger
    global log
    log=logger

    fname = opt.eifile
    objID =  info['uuid']

    # build connection
    try:
        (s3host,s3port) = opt.s3endpoint.split(":")
    except:
        s3host = opt.s3endpoint
        s3port = 443
    try:
        s3port = int(s3port)
    except:
        log.info("Invalid AWS port {}".format(s3port))
        sys.exit(0)

    # resolve host name. detect invalid connection endpoint
    try:
        (h, a, ip)=socket.gethostbyname_ex(s3host)
    except:
        log.info("S3 host can not be resolved {}".format(s3host))
        raise Exception("Invalid host {}".format(s3host))

    # define s3 connection
    try:
        conn = boto.connect_s3(
            aws_access_key_id = opt.awsaccess,
            aws_secret_access_key = opt.awssecret,
            host = s3host, port = s3port,
            is_secure = opt.s3secure)
    except Exception, e:
        log.info("Unable to connect to object store " + str(e))
        raise Exception(e)


    # copy to object store
    bucket_name = opt.s3bucket
    try:
        bucket = conn.create_bucket(bucket_name)
    except Exception, e:
        log.info("Unable to create bucket " + str(e))
        raise Exception(e)

    try:
        taskID_1,taskID_2 = info['taskID'].split('.')
        if taskID_2 == 'G':
            flavor = 'panda'
        elif taskID_2 == 'T':
            flavor = 'tier0'
        elif taskID_2 == 'X':
            flavor = 'test'
        else:
            flavor = 'unknown'
    except:
        flavor="unknown"
    
    keyname = "{0}/{1}/{2}_{3}_{4}.ei.spb".format(flavor,info['taskID'].split('.')[0],info['taskID'], info['jobID'], objID)
    

    # convert metada values to strings (numbers make generate_url fail)
    # do not include guids
    info2={}
    for k, v in info.iteritems():
        if k != 'guids':
            info2[k]=v.__str__()
    
    try:
        kb = Key(bucket)
        kb.key      = keyname
        kb.metadata = info2
        kb.set_contents_from_filename(fname)
        kb.set_acl('public-read')
        if opt.http:
            url = kb.generate_url(expires_in=0, query_auth=False, force_http=True)
    except Exception, e:
        log.info("Unable to store object " + str(e))
        raise Exception(e)
    
    if opt.http:
        u = urlparse(url)
        if u.port is not None:
            urlx = "{}://{}:{}{}".format(u.scheme, u.hostname, u.port, u.path)
        else:
            urlx = "{}://{}{}".format(u.scheme, u.hostname, u.path)
    else:
        urlx = "s3://{}:{}/{}/{}".format(s3host,s3port,bucket_name,keyname)
        
    return urlx


def eosCopy(logger, opt, info):

    # logger
    global log
    log=logger

    fname = opt.eifile
    objID =  info['uuid']

    keyname = "{0}_{1}_{2}.ei.spb".format(info['taskID'], info['jobID'], objID)
    rfile = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-evind/Consumer/data/{}".format(keyname)

    try:
        os.system("xrdcp -s {0} {1}".format(fname,rfile))
    except Exception, e:
        log.info("Unable to xrdcp file" + str(e))
        raise Exception(e)
        
    return rfile

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
            log.info("Invalid port {}".format(port))
            raise Exception("Invalid port {}".format(port))
        try:
            (h, a, ip)=socket.gethostbyname_ex(host)
        except:
            log.info("Host can not be resolved {}".format(host))
            raise Exception("Invalid host {}".format(host))
        for addr in ip:
            result.append((addr,port))

    return result
    

def options(argv):

    parser = argparse.ArgumentParser(description='SendEI_SPB to object store and notify EI supervisor.')
    parser.add_argument('-e','--endpoint', default='localhost:61613', type=endpointV,
                        help="broker name and port")
    parser.add_argument('-q','--queue',  default='/queue/atlas.eventindex.supervisor', help="broker queue name")
    parser.add_argument('-u','--user',default=None, help="Stomp user name")
    parser.add_argument('-k','--passcode',default=None,help="Stomp passcode")
    parser.add_argument('-v','--verbose', action='count', help='Verbosity level')
    parser.add_argument('-d','--debug', action='count', help='Debug')
    parser.add_argument("-n","--dummy", action='store_true', default=False, help="Do not send messages and do not connect")
    parser.add_argument("--s3endpoint", default="localhost:443", help="S3 AWS server name and port")
    parser.add_argument("--awsaccess", default=None, help="AWS access key. (default from AWSACCESS env variable)")
    parser.add_argument("--awssecret", default=None, help="AWS secret key. (default from AWSSECRET env variable)")
    parser.add_argument("--s3secure", default=True, help="Secure connection to S3 AWS server")
    parser.add_argument("--s3bucket", default="atlas_eventindex", help="S3 bucket name")
    parser.add_argument("--eosfallback", default=False, help="Copy file into EOS if S3 fails")
    parser.add_argument("-x","--http", action='store_true', default=False, help="Generate object http public link")

    parser.add_argument('eifile', help="EventIndex file")

    opt=parser.parse_args(args=argv)

    if opt.user is None  or opt.passcode is None:
        if not(opt.user is None  and opt.passcode is None):
            log.error("Both, user and passcode must be specified or none")
            sys.exit(0)

    if opt.awsaccess is None or opt.awssecret is None:
        opt.awsaccess=os.getenv('AWSACCESS', None)
        opt.awssecret=os.getenv('AWSSECRET', None)

    if opt.awsaccess is None or opt.awssecret is None:
        log.error("Both ACCESS KEY and SECRET KEY must exists")
        sys.exit(0)

    return opt


def eimrun(logger,opt):

    # logger
    global log
    log=logger

    # open EI file and get info
    fname = opt.eifile
    if not (os.path.isfile(fname) and os.access(fname, os.R_OK)):
        log.info("Event Index SPB file {} does not exists or is not readble".format(fname))
        sys.exit(0)
    try:
        spbf = SpbFile(fname)
        info = spbf.getInfo()
    except:
        log.info("Unable to get info from EI SPB file {}".format(fname))
        sys.exit(0)

    # prepare to copy
    objID  = uuid.uuid4().hex
    info['uuid'] = objID

    
    urlx = None
    try:
        urlx = osCopy(logger, opt, info)
    except Exception, e:
        log.info("Error trying to copy file to ObjectStore: "+str(e))
        if opt.eosfallback:
            urlx = eosCopy(logger, opt, info)

    if urlx is None:
        log.info("Unable to send SPB file")
        sys.exit(0)
        

    # attach timestamp and file url into file info.
    # it will be sent to the broker messaging
    ts = int(time.time() * 1000)
    info['timestamp']  = ts
    info['url']        = urlx


    mbroker=MSG(opt)
    try:
        mbroker.connect()
    except stomp.exception.ConnectFailedException, e:
        log.info("Unable to connect to stomp broker")
        sys.exit(0)

    msg={}
    msg['type']   = 1      # PROD -> SUP
    msg['data'] = info     # payload
    msg['ts']   = ts       # timestamp
    msg['id']   = objID    # instance id
    mbroker.sendMSG(json.dumps(msg))
        
    if opt.verbose > 0:
        log.info("=========== sendEI SPB summary ==========")
        log.info(" endpoint: {}".format(opt.endpoint))
        log.info(" queue: {}".format(opt.queue))
        log.info(" url: {}".format(urlx))
        for g in info['guids']:
            log.info(" guid[{:03d}]: {} {:7d} {:7d}".format(g['fileno'],g['guid'],g['nevents'],g['nuevents'],))
        log.info(" number of guids:        {:10d}".format(info['nfiles']))
        log.info(" number of events:       {:10d}".format(info['nevents']))
        log.info(" number of unique evt:   {:10d}".format(info['nuevents']))
        log.info(" file size:              {:10d} bytes".format(info['size']))
        log.info(" uncompressed file size: {:10d} bytes".format(info['usize']))
        if int(info['nevents']) != 0:
            log.info(" mean size per evt:      {:10.2f} bytes".format(float(info['size'])/int(info['nevents'])))
            log.info(" mean usize per evt:     {:10.2f} bytes".format(float(info['usize'])/int(info['nevents'])))
            
    # wait 1 second to let messaging processing
    time.sleep(1)
    mbroker.close()

