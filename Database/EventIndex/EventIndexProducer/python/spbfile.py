
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os
import sys
import gzip
import struct
from EventIndexProducer.EIPBof import EIPBof
import EventIndexProducer.EIconstants as eic
import logging as log


class RunEvtRanges():

    def __init__(self):
        self.runs = {}

    def update(self, run_number, evt_number):
        if run_number not in self.runs:
            self.runs[run_number] = (sys.maxsize, -1)
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

        self.info = {}
        self.info['size'] = os.path.getsize(filename)

        try:
            self.eispb = gzip.open(filename, 'rb')
        except Exception:
            raise

        # buffer to read file
        self.buf = b""    # read buffer
        self.blen = 0     # buffer length
        self.bpos = 0     # buffer current read position
        self.usize = 0    # aproximate uncompressed size
        try:
            self.version = self.readMagicVersion()
        except Exception:
            raise

    def getVersion(self):
        return self.version

    def readMagicVersion(self):
        self.growBufToSize(8)
        magic = struct.unpack('<I', self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        version = struct.unpack('<I', self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4
        if magic != 0x6e56c8c7:
            raise Exception("Invalid magic in EISPB file")
        return version

    def growBufToSize(self, size):
        if self.bpos + size < self.blen:
            return
        # throw bytes away until current read position
        self.buf = self.buf[self.bpos:]
        self.bpos = 0
        self.blen = len(self.buf)
        # more bytes needed ?
        if self.blen < size:
            # read at least 40960
            toread = size-self.blen
            if toread < 40960:
                toread = 40960
            self.buf += self.eispb.read(toread)
            self.usize += toread
            self.blen = len(self.buf)

    def getMsg(self):
        self.growBufToSize(4)
        mtype = struct.unpack('<I', self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4

        self.growBufToSize(4)
        mlen = struct.unpack('<I', self.buf[self.bpos:self.bpos+4])[0]
        self.bpos += 4

        self.growBufToSize(mlen)
        msg = self.buf[self.bpos:self.bpos+mlen]
        self.bpos += mlen

        return (mtype, mlen, msg)

    def close(self):
        self.eispb.close()

    def rewind(self):
        self.eispb.rewind()
        self.buf = b""    # read buffer
        self.blen = 0     # buffer length
        self.bpos = 0     # buffer current read position
        self.usize = 0    # aproximate uncompressed size
        # skip magic and version
        self.growBufToSize(8)
        self.bpos += 8

    def tell(self):
        self.eispb.tell()

    def getInfo(self):

        self.info['guids'] = []
        fileno = 0
        tot_nevents = 0
        tot_nuevents = 0
        tot_nbytes = 0
        nevents = 0
        nuevents = 0
        nbytes = 0
        runevtRanges = RunEvtRanges()
        runevt = {}
        currentGUID = {}

        """
        SPB file format is a stream of google's protocol buffer messages
        Reading is sequential, no random read of messages is possible
        """

        eipbof = EIPBof(self.version)
        self.info['version'] = self.version

        while True:
            (mtype_ver, mlen, msg) = self.getMsg()
            mtype = (mtype_ver & 0x000fff00) >> 8
            ver = (mtype_ver & 0x000000ff)
            if (ver != eic.EI_PROTO_MSGVER):
                raise Exception("Invalid message version found in EISPB file")

            if mtype == eic.EI_PROTO_HEADER:                 # Header
                header = eipbof.Header()
                header.ParseFromString(msg)
                self.info['jobID'] = header.jobID
                self.info['taskID'] = header.taskID
                self.info['startProcTime'] = header.startProcTime
                self.info['inputDsName'] = header.inputDsName
                self.info['provenanceRef'] = header.provenanceRef
                self.info['triggerInfo'] = header.triggerInfo
            elif mtype == eic.EI_PROTO_BEGINGUID:            # Begin GUID
                beginGUID = eipbof.BeginGUID()
                beginGUID.ParseFromString(msg)
                currentGUID = {}
                currentGUID['fileStartProcTime'] = beginGUID.startProcTime
                currentGUID['guid'] = beginGUID.guid
                currentGUID['fileno'] = fileno
                fileno += 1
                nevents = 0
                nuevents = 0
                nbytes = 0
                # run-evt, used to calculate unique events
                runevt = {}
                # to calculate run evt ranges
                runevtRanges = RunEvtRanges()
                #
                currentGUID['AMITag'] = beginGUID.AMITag
                currentGUID['trigStream'] = beginGUID.trigStream
                currentGUID['projName'] = beginGUID.projName
            elif mtype == eic.EI_PROTO_ENDGUID:              # End GUID
                endGUID = eipbof.EndGUID()
                endGUID.ParseFromString(msg)
                currentGUID['nevents'] = nevents
                currentGUID['nuevents'] = nuevents
                currentGUID['fileEndProcTime'] = endGUID.endProcTime
                currentGUID['runevtRanges'] = runevtRanges.getRanges()
                self.info['guids'].append(currentGUID)
                tot_nevents += nevents
                tot_nuevents += nuevents
                tot_nbytes += nbytes
            elif mtype == eic.EI_PROTO_EIEVENT:              # event
                eventPB = eipbof.EIEvent()
                eventPB.ParseFromString(msg)
                nevents += 1
                nbytes += mlen
                runevtRanges.update(int(eventPB.runNumber),
                                    int(eventPB.eventNumber))
                runevtkey = "{0:08d}-{1:011d}".format(int(eventPB.runNumber),
                                                      int(eventPB.eventNumber))
                if runevtkey not in runevt:
                    runevt[runevtkey] = 1
                    nuevents += 1
            elif mtype == eic.EI_PROTO_TRIGGERMENU:          # Trigger menu
                triggerMenu = eipbof.TriggerMenu()
                triggerMenu.ParseFromString(msg)
            elif mtype == eic.EI_PROTO_TRAILER:              # Trailer
                trailer = eipbof.Trailer()
                trailer.ParseFromString(msg)
                self.info['endProcTime'] = trailer.endProcTime
                self.info['nevents'] = trailer.nentries
                self.info['nfiles'] = trailer.nfiles
                break
            else:
                break

        self.info['usize'] = self.usize
        self.info['nuevents'] = tot_nuevents
        self.rewind()

        # checkings
        if self.info['nfiles'] != fileno:
            log.info("ERROR nfiles {} {}".format(fileno, self.info['nfiles']))
        if self.info['nevents'] != tot_nevents:
            log.info("ERROR tot_nevents {} {}".format(tot_nevents,
                                                      self.info['nevents']))

        return self.info
