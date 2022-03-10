# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys
import os
import json
import stomp
import socket
import uuid
import time
from urllib.parse import urlparse

from EventIndexProducer.spbfile import SpbFile

import argparse
import boto3

from botocore.config import Config


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

        # connect
        conn = stomp.Connection(self.brokers)
        conn.set_listener('', MyListener())
        try:
            if self.user is not None and self.passcode is not None:
                log.debug("Conecting broker using user and password")
                conn.connect(self.user, self.passcode, wait=True)
            else:
                conn.connect(wait=True)
        except stomp.exception.ConnectFailedException:
            # pass error to caller
            raise

        self.conn = conn

    def close(self):
        if self.conn is not None:
            self.conn.disconnect()
        self.conn = None

    def sendMSG(self, msg):

        log.debug("Sending message. Len: {}".format(len(msg)))

        # NON_PERSISTENT = "1"
        PERSISTENT = "2"

        if not self.dummy:
            self.conn.send(self.queue, msg,
                           JMSDeliveryMode=PERSISTENT,
                           JMSExpiration=0)
        if self.verbose > 2:
            print(msg, file=sys.stderr)


# ----------------------
# listener
# ----------------------
class MyListener(object):
    def on_error(self, headers, message):
        print('received an error', file=sys.stderr)
        print(" headers: ", headers, file=sys.stderr)
        print(" message: ", message, file=sys.stderr)

    def on_message(self, headers, message):
        print('received a message', file=sys.stderr)
        print(" headers: ", headers, file=sys.stderr)
        print(" message: ", message, file=sys.stderr)


def osCopy(logger, opt, info):

    # logger
    global log
    log = logger

    fname = opt.eifile
    objID = info['uuid']

    # build connection
    try:
        (s3host, s3port) = opt.s3endpoint.split(":")
    except Exception:
        s3host = opt.s3endpoint
        if opt.s3secure:
            s3port = 443
        else:
            s3port = 80
    try:
        s3port = int(s3port)
    except Exception:
        log.info("Invalid AWS port {}".format(s3port))
        sys.exit(0)

    # resolve host name. detect invalid connection endpoint
    try:
        (h, a, ip) = socket.gethostbyname_ex(s3host)
    except Exception:
        log.info("S3 host can not be resolved {}".format(s3host))
        raise Exception("Invalid host {}".format(s3host))

    if opt.s3secure:
        s3endpoint = "https://{}:{}".format(s3host, s3port)
    else:
        s3endpoint = "http://{}:{}".format(s3host, s3port)

    s3Config = None

    # should we use a proxy ?
    # eg.: http_proxy=squid.grid.sinica.edu.tw:3128
    http_proxy = os.getenv('http_proxy')
    if http_proxy is not None:
        # take the first proxy if there are several
        http_proxy0 = http_proxy.split(',')[0]
        try:
            (proxy, proxy_port) = http_proxy0.split(":")
        except Exception:
            proxy = http_proxy0
            proxy_port = 3128
        try:
            proxy_port = int(proxy_port)
        except Exception:
            log.info("Invalid proxy port {}".format(proxy_port))
            sys.exit(0)
        http_proxy1 = "{}:{}".format(proxy, proxy_port)
        log.info("osCopy using proxy {}".format(http_proxy1))

        # use this proxy for both http and https
        s3Config = Config(proxies={'http': http_proxy1, 'https': http_proxy1})

    # get s3 resource
    try:
        log.debug("osCopy: try to get s3 resoruce")
        s3 = boto3.resource(
            service_name='s3',
            aws_access_key_id=opt.awsaccess,
            aws_secret_access_key=opt.awssecret,
            endpoint_url=s3endpoint,
            verify=True,
            config=s3Config
        )
    except Exception as e:
        log.info("Unable to connect to object store " + str(e))
        raise

    # copy to object store
    bucket_name = opt.s3bucket

    try:
        taskID_1, taskID_2 = info['taskID'].split('.')
        if taskID_2 == 'G':
            flavor = 'panda'
        elif taskID_2 == 'T':
            flavor = 'tier0'
        elif taskID_2 == 'X':
            flavor = 'test'
        else:
            flavor = 'unknown'
    except Exception:
        flavor = "unknown"

    keyname = "{0}/{1}/{2}_{3}_{4}.ei.spb".format(flavor,
                                                  info['taskID'].split('.')[0],
                                                  info['taskID'],
                                                  info['jobID'],
                                                  objID)

    # convert metada values to strings
    # do not include guids
    eiMetadata = {k: info[k].__str__() for k in info if k != 'guids'}

    # upload eiFile to s3 storage
    try:
        log.debug("osCopy: try to build object and upload eiFile")
        object = s3.Object(bucket_name=bucket_name, key=keyname)
        object.upload_file(fname,
                           ExtraArgs={"Metadata": eiMetadata,
                                      'ContentType':
                                      'application/octet-stream'})
        object_acl = object.Acl()
        object_acl.put(ACL='public-read')
        if opt.http:
            # get s3 client from resouce
            s3client = s3.meta.client
            url = s3client.generate_presigned_url('get_object',
                                                  Params={'Bucket':
                                                          bucket_name,
                                                          'Key': keyname},
                                                  HttpMethod="http")
    except Exception as e:
        log.info("Unable to store object " + str(e))
        raise

    if opt.http:
        u = urlparse(url)
        if u.port is not None:
            urlx = "{}://{}:{}{}".format(u.scheme, u.hostname, u.port, u.path)
        else:
            urlx = "{}://{}{}".format(u.scheme, u.hostname, u.path)
    else:
        urlx = "s3://{}:{}/{}/{}".format(s3host, s3port, bucket_name, keyname)

    log.debug("osCopy: eiFile successfully uploaded")
    return urlx


def eosCopy(logger, opt, info):

    # logger
    global log
    log = logger

    fname = opt.eifile
    objID = info['uuid']

    keyname = "{0}_{1}_{2}.ei.spb".format(info['taskID'], info['jobID'], objID)
    rfile = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/" \
            "proj-evind/Consumer/data/{}".format(keyname)

    try:
        log.debug("eosCopy: using xrdcp -s {0} {1}".format(fname, rfile))
        os.system("xrdcp -s {0} {1}".format(fname, rfile))
    except Exception as e:
        log.info("Unable to xrdcp file" + str(e))
        raise

    return rfile


def endpointV(endpoint):
    # default port
    dport = 60013

    # enpoint should be broker1[:port1],broker2:[port2],...
    lbroker = endpoint.split(',')
    result = []
    for b in lbroker:
        try:
            (host, port) = b.split(":")
        except Exception:
            host = b
            port = dport
        try:
            port = int(port)
        except Exception:
            log.info("Invalid port {}".format(port))
            raise Exception("Invalid port {}".format(port))
        try:
            (h, a, ip) = socket.gethostbyname_ex(host)
        except Exception:
            log.info("Host can not be resolved {}".format(host))
            raise Exception("Invalid host {}".format(host))
        for addr in ip:
            result.append((addr, port))

    return result


def options(argv):

    parser = argparse.ArgumentParser(description='SendEI_SPB to object store '
                                     'and notify EI supervisor.')
    parser.add_argument('-e', '--endpoint', default='localhost:61613',
                        type=endpointV,
                        help="broker name and port")
    parser.add_argument('-q', '--queue',
                        default='/queue/atlas.eventindex.supervisor',
                        help="broker queue name")
    parser.add_argument('-u', '--user',
                        default=None,
                        help="Stomp user name")
    parser.add_argument('-k', '--passcode',
                        default=None,
                        help="Stomp passcode")
    parser.add_argument('-v', '--verbose',
                        default=0,
                        action='count',
                        help='Verbosity level')
    parser.add_argument('-d', '--debug',
                        default=0,
                        action='count',
                        help='Debug')
    parser.add_argument("-n", "--dummy",
                        action='store_true',
                        default=False,
                        help="Do not send messages and do not connect")
    parser.add_argument("--s3endpoint",
                        default="localhost:443",
                        help="S3 AWS server name and port")
    parser.add_argument("--awsaccess",
                        default=None,
                        help="AWS access key. "
                        "(default from AWSACCESS env variable)")
    parser.add_argument("--awssecret",
                        default=None,
                        help="AWS secret key. "
                        "(default from AWSSECRET env variable)")
    parser.add_argument("--s3secure",
                        default=True,
                        help="Secure connection to S3 AWS server")
    parser.add_argument("--s3bucket",
                        default="atlas_eventindex",
                        help="S3 bucket name")
    parser.add_argument("--eosfallback",
                        default=False,
                        help="Copy file into EOS if S3 fails")
    parser.add_argument("-x", "--http",
                        action='store_true',
                        default=False,
                        help="Generate object http public link")
    parser.add_argument("--objID",
                        default=None,
                        help="Object ID")

    parser.add_argument('eifile', help="EventIndex file")

    opt = parser.parse_args(args=argv)

    if opt.user is None or opt.passcode is None:
        if not(opt.user is None and opt.passcode is None):
            log.error("Both, user and passcode must be specified or none")
            sys.exit(0)

    if opt.awsaccess is None or opt.awssecret is None:
        opt.awsaccess = os.getenv('AWSACCESS', None)
        opt.awssecret = os.getenv('AWSSECRET', None)

    if opt.awsaccess is None or opt.awssecret is None:
        log.error("Both ACCESS KEY and SECRET KEY must exists")
        sys.exit(0)

    return opt


def eimrun(logger, opt):

    # logger
    global log
    log = logger

    # open EI file and get info
    fname = opt.eifile
    if not (os.path.isfile(fname) and os.access(fname, os.R_OK)):
        log.info("Event Index SPB file {} does not "
                 "exists or is not readble".format(fname))
        sys.exit(0)

    try:
        spbf = SpbFile(fname)
        info = spbf.getInfo()
    except Exception:
        log.info("Unable to get info from EI SPB file {}".format(fname))
        sys.exit(0)

    # remove keys not to be included
    if 'provenanceRef' in info:
        del info['provenanceRef']
    if 'triggerInfo' in info:
        del info['triggerInfo']

    # prepare to copy
    if opt.objID is None:
        objID = uuid.uuid4().hex
    else:
        objID = opt.objID
    info['uuid'] = objID

    urlx = None
    try:
        log.debug("call osCopy to transfer file")
        urlx = osCopy(logger, opt, info)
    except Exception as e:
        log.info("Error trying to copy file to ObjectStore: "+str(e))
        if opt.eosfallback:
            log.info("osCopy failed. Trying with eosCopy")
            urlx = eosCopy(logger, opt, info)

    if urlx is None:
        log.info("Unable to send SPB file")
        sys.exit(0)

    # attach timestamp and file url into file info.
    # it will be sent to the broker messaging
    ts = int(time.time() * 1000)
    info['timestamp'] = ts
    info['url'] = urlx

    mbroker = MSG(opt)
    try:
        mbroker.connect()
    except stomp.exception.ConnectFailedException:
        log.info("Unable to connect to stomp broker")
        sys.exit(0)

    msg = {}
    msg['type'] = 1      # PROD -> SUP
    msg['data'] = info   # payload
    msg['ts'] = ts       # timestamp
    msg['id'] = objID    # instance id
    mbroker.sendMSG(json.dumps(msg))

    if opt.verbose > 0:
        log.info("=========== sendEI SPB summary ==========")
        log.info(" endpoint: {}".format(opt.endpoint))
        log.info(" queue: {}".format(opt.queue))
        log.info(" url: {}".format(urlx))
        for g in info['guids']:
            log.info(" guid[{:03d}]: {} {:7d} {:7d}".format(
                g['fileno'], g['guid'], g['nevents'], g['nuevents']))
        log.info(" number of guids:        {:10d}".format(info['nfiles']))
        log.info(" number of events:       {:10d}".format(info['nevents']))
        log.info(" number of unique evt:   {:10d}".format(info['nuevents']))
        log.info(" file size:              {:10d} bytes".format(info['size']))
        log.info(" uncompressed file size: {:10d} bytes".format(info['usize']))
        if int(info['nevents']) != 0:
            log.info(" mean size per evt:      {:10.2f} bytes".format(
                float(info['size'])/int(info['nevents'])))
            log.info(" mean usize per evt:     {:10.2f} bytes".format(
                float(info['usize'])/int(info['nevents'])))

    # wait 1 second to let messaging processing
    time.sleep(1)
    mbroker.close()
