#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

#import pdb
import eformat
import sys
import os
import re
import time
import logging
from libpyeformat import FullEventFragment, convert_old
import libpyeformat_helper as helper
import libpyevent_storage as EventStorage

import argparse
import PyUtils.dbsqlite as dbsqlite
from EventIndexProducer.compressB64 import compressB64
from EventIndexProducer.EI_Lib import EIrecord, IOV


# logger
log=None


class blob_istream2(object):
    """Returns useless blobs of data, as defined by the EventStorage i/f."""
    
    def __init__(self, f):  
        
        #we leave completely to the data reader to defined "valid files".
        self.total_events = 0
        dr = EventStorage.pickDataReader(f)
        if dr is None:
            raise IOError("Invalid file or format at '{}'".format(f))
        self.total_events = dr.eventsInFile()

        self.file = f

        def __len__(self):
            """Returns the number of events available in this stream"""
            return self.total_events

    def __iter__(self):
        dr = EventStorage.pickDataReader(self.file)
        for k in range(dr.eventsInFile()):
            yield (dr.getPosition(), dr.getData())

    def __getitem__(self, key):
        """Returns a specific blob in the stream. The 'key' parameter maybe
        either a single valued integer or a slice object, in which case many
        fragments are returned in one shot."""
    
        if type(key) is int or type(key) is long:
            if key >= len(self):
                raise IndexError, "Index ({:d}) is bigger than my length ({:d})".format(key, self.total_events)
            if key < 0:
                if (-key) > len(self):
                    raise IndexError, "Index ({:d}) is too small for my length ({:d})".format(key, self.total_events)
                key = len(self) + key
        
            f = None
            for f in self:
                key -= 1
                if key < 0: break
            return f

        elif type(key) is slice:
            (start, stop, stride) = key.indices(self.total_events)
            valid = range(start, stop, stride)
            retval = []
            counter = 0
            for f in self:
                if counter in valid:
                    retval.append(f)
                counter += 1
            return retval

class istream2(blob_istream2):
    """The istream class allows the user to read file streams.
    """

    def __init__(self, f):
        blob_istream2.__init__(self, f)

    def __iter__(self):
        dr = EventStorage.pickDataReader(self.file)
        for k in range(dr.eventsInFile()):
            offset = dr.getPosition()
            blob = dr.getData()

            # check for people trying old versions and convert it on the spot
            fragment_version = helper.Version(blob[3])
            if fragment_version.major_version() != helper.MAJOR_DEFAULT_VERSION:
                current_version = helper.Version()
                logging.debug("Converting from version {} to {}".format(
                        fragment_version.human_major(), current_version.human_major()))
                blob = convert_old(blob)

            if blob[0] == helper.HeaderMarker.FULL_EVENT:
                yield (offset,FullEventFragment(blob))
            else:
                raise SyntaxError, "Expecting event marker, not 0x{:08x}".format(blob[0])

    def iter_raw(self):
        dr = EventStorage.pickDataReader(self.file)
        for k in range(dr.eventsInFile()):
            yield (dr.getPosition(), dr.getData())


def processRAW(input_file, eif, nfile, nfirst, evtmax):    
  
    log.info("Opening data file: {}".format(input_file))
    eif['StartProcTime_{:d}'.format(nfile)] = int(time.time() * 1000)

    dr = EventStorage.pickDataReader(input_file)
    if dr is None:
        raise IOError("Invalid file or format at '{}'".format(input_file))

    log.info("total_events: {}".format(dr.eventsInFile()))
    log.debug("dataMB: {}".format(dr.dataMB_InFile()))
    log.debug("LumiBlock: {}".format(dr.lumiblockNumber()))
    log.debug("Stream: {}".format(dr.stream()))
    log.debug("App Name: {}".format(dr.appName()))
    log.debug("beamEnergy: {}".format(dr.beamEnergy()))
    log.debug("beamType: {}".format(dr.beamType()))
    log.debug("detectorMask: {}".format(dr.detectorMask()))
    log.debug("Core Name: {}".format(dr.fileNameCore()))
    log.debug("projectTag: {}".format(dr.projectTag()))
    log.debug("GUID: {}".format(dr.GUID()))
    log.debug("runNumber: {}".format(dr.runNumber()))
    log.debug("stream: {}".format(dr.stream()))
    log.debug("triggerType: {}".format(dr.triggerType()))

    GUID = dr.GUID()


    eif['ProjName_{:d}'.format(nfile)] = dr.projectTag()
    eif['TrigStream_{:d}'.format(nfile)] = dr.stream()
    eif['AMITag_{:d}'.format(nfile)] = ""                  # no tag for RAW data
    eif['GUID_{:d}'.format(nfile)] = GUID
    
    

    #input = eformat.istream([input_file])
    input = istream2(input_file)
    
    ###################################################################
    
    cntEvt=0
    cntEvtEI=nfirst

    for (offset,event) in input:

        if evtmax >0 and cntEvtEI >= evtmax :
            break;

        eirec = EIrecord()

        L1TBP=event.lvl1_trigger_info()[0:8]
        L1TAP=event.lvl1_trigger_info()[8:16]
        L1TAV=event.lvl1_trigger_info()[16:24]
        log.debug("--------------------------------")
        log.debug('Event: {}'.format(cntEvt))
        log.debug('Offset: {}'.format(offset))
        log.debug('RunNumber: {}'.format(event.run_no()))
        log.debug("EventNumber: {}".format( event.global_id()))
        log.debug('L1ID: {}'.format(event.lvl1_id()))
        log.debug("EventTime: {}".format(event.bc_time_seconds()))
        log.debug("EventTimeNanoSec: {}".format( event.bc_time_nanoseconds()))
        log.debug("LumiBlockN: {}".format(event.lumi_block()))
        log.debug("BunchId: {}".format(event.bc_id()))
        log.debug('L1 type: 0x{:02x}'.format(event.lvl1_trigger_type()))
        log.debug('L1 Before Prescale: {}'.format(L1TBP))
        log.debug('L1 After  Prescale: {}'.format( L1TAP))
        log.debug('L1 After  Veto: {}'.format(L1TAV))
        log.debug("RunType: {}".format(event.run_type()))
        

        eirec['RunNumber'] = event.run_no()
        eirec['EventNumber'] = event.global_id()
        eirec['LumiBlockN'] = event.lumi_block()
        eirec["BunchId"] = event.bc_id()
        eirec['EventTime'] = event.bc_time_seconds()
        eirec['EventTimeNanoSec'] = event.bc_time_nanoseconds()
        eirec['EventWeight'] = 1.0
        eirec['McChannelNumber'] = 0
        eirec['Lvl1ID'] = event.lvl1_id()


        #Run Type       Value
        #Physics     0x00000000
        #Calibration 0x00000001
        #Cosmics     0x00000002
        #Test        0x0000000f
        #Simulation  0x80000000

        runtype = event.run_type().__str__()
        eirec['IsSimulation'] = 1
        eirec['IsTestBeam'] = 0
        eirec['IsCalibration'] = 0
        if "PHYSICS" in runtype:
            eirec['IsSimulation'] = 0
        if "CALIBRATION" in runtype:
            eirec['IsCalibration'] = 1
        if "TEST" in runtype:
            eirec['IsTestBeam'] = 1

        eirec['SMK'] = 0
        eirec['L1PSK'] = 0
        eirec['HLTPSK'] = 0

        eirec['Snam0'] = "StreamRAW"
        offset_str = "{0:016X}".format(offset)
        offset_str1 = offset_str[:8]
        offset_str2 = offset_str[8:]
        tk_tmpl = "[DB={}][CNT=00000000][CLID=00000000-0000-0000-0000-000000000000][TECH=00001000][OID={}-{}]"
        eirec['Sref0'] = tk_tmpl.format(GUID,offset_str1,offset_str2)

        L1=event.lvl1_trigger_info()
        trigL1=""
        for l in L1:
            trigL1+="{0:032b}".format(l)[::-1]
        L2=event.lvl2_trigger_info()
        trigL2=""
        for l in L2:
            trigL2+="{0:032b}".format(l)[::-1]
        EF=event.event_filter_info()
        trigEF=""
        for l in EF:
            trigEF+="{0:032b}".format(l)[::-1]
        trigL1=compressB64(trigL1)
        trigL2=compressB64(trigL2)
        trigEF=compressB64(trigEF)
        eirec['L1PassedTrigMask'] = trigL1
        eirec['L2PassedTrigMask'] = trigL2
        eirec['EFPassedTrigMask'] = trigEF

        # write to db
        eif['Entry_{:d}'.format(cntEvtEI)] = eirec.getRec()
        cntEvt += 1
        cntEvtEI += 1

    eif['Nentries_{:d}'.format(nfile)] = cntEvt
    eif['EndProcTime_{:d}'.format(nfile)] = int(time.time() * 1000)

    return cntEvt


def options(argv):

    parser = argparse.ArgumentParser(description='RAW to EI processor')
    parser.add_argument('-v','--verbose', action='count', help='Verbosity level')
    parser.add_argument("-d","--debug", action='store_true', default=False,help="Debug messages")
    parser.add_argument("-m","--evtmax", type=int,default=0, help="Max events to read")
    parser.add_argument("--inputRAWFile", nargs='+', default=None, help="Input RAW file")
    parser.add_argument("--inputBSFile", nargs='+', default=None, help="Input BS file (same as RAW)")
    parser.add_argument("--outputFile", default="output.ei.pkl", help="Output EI file")
    parser.add_argument("--eidsname", default=None, help="DatasetName")
    
    opt=parser.parse_args()

    if opt.inputRAWFile is not None and opt.inputBSFile is not None:
        log.error("Only one of inputRAWFile or inputBSFile has to be specified")
        sys.exit(1)
       
    if opt.inputRAWFile is None and opt.inputBSFile is None:
        log.error("At least one input file has to be specified")
        sys.exit(1)

    if opt.inputRAWFile is not None:
        ifiles = opt.inputRAWFile
    if opt.inputBSFile is not None:
        ifiles = opt.inputBSFile
        
    itmp = ifiles
    ifiles=[]
    for f in itmp:
        ifiles += f.split(",")

    opt.input=ifiles
    opt.output=opt.outputFile
    
    return opt


def main():    
  
    # logger
    global log
    log = logging.getLogger('RAW2EI.py')

    # get options
    opt = options(sys.argv)
    
    if opt.verbose > 0:
        log.setLevel(logging.INFO)

    if opt.debug > 0:
        log.setLevel(logging.DEBUG)

    # open EI file
    if os.path.exists(opt.output):
        os.remove(opt.output)
    eif = dbsqlite.open(opt.output,flags='w')
    
    eif['StartProcTime'] = int(time.time() * 1000)
    eif['Schema'] = EIrecord().getRecSchema()
    eif['Version'] = EIrecord().getVersion()
    eif['TaskID'] = os.getenv('TaskID', 0)
    eif['JobID'] = os.getenv('JobID', 0)

    if opt.eidsname is not None:
        dsname = opt.eidsname
    else:
        dsname = os.getenv('INDS',"Unknown.Input.Dataset.Name")
    dsname = re.sub('_tid[0-9]{8}_[0-9]{2}', '', dsname)
    dsname = re.sub('_sub[0-9]{10}', '', dsname)
    dsname = re.sub('\/$', '', dsname)

    eif['InputDsName'] = dsname

    
    #processing options
    eif['ProvenanceRef'] = False
    eif['TriggerInfo'] = True


    ## process RAW files
    if type(opt.input) is str: 
        filelist = [opt.input]
    else:
        filelist = opt.input


    nfile=0;
    nevents=0;
    for f in filelist:
        nevents += processRAW(f,eif,nfile,nevents,opt.evtmax);
        nfile += 1;
        if opt.evtmax >0 and nevents >= opt.evtmax :
            break;


    eif['Nentries'] = nevents
    eif['Nfiles'] = nfile
    eif['EndProcTime'] = int(time.time() * 1000)
    eif.close()


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



                    
