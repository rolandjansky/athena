#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

import sys
import argparse

from EventIndexProducer.EIPBof import EIPBof
from EventIndexProducer.compressB64 import _fromB64
from EventIndexProducer.spbfile import SpbFile
import EventIndexProducer.EIconstants as eic


def options(argv):

    parser = argparse.ArgumentParser(description='Unpick Event Index File')
    parser.add_argument('-e', '--evtnumber',
                        default=0,
                        type=int,
                        help='event number to dump')
    parser.add_argument('-r', '--runnumber',
                        default=0,
                        type=int,
                        help='run number to dump')
    parser.add_argument('-m', '--evtmax',
                        default=1,
                        type=int,
                        help='Max number of events to dump')
    parser.add_argument('--decodeL1trig',
                        action='count',
                        help='decode L1 trigger')
    parser.add_argument('--decodeL1trig0',
                        action='count',
                        help='decode L1 trigger, remove redundancies')
    parser.add_argument('--decodeL2trig',
                        action='count',
                        help='decode L2 trigger')
    parser.add_argument('--decodeEFtrig',
                        action='count',
                        help='decode EF trigger')
    parser.add_argument('--decodetrig',
                        action='count',
                        help='decode ALL triggers')
    parser.add_argument('eispbfile',
                        help="EventIndex SPB file")

    return parser.parse_args()


def getTriggerList(s):

    tl = []
    idx = 0
    count = ""
    for k in range(0, len(s)):
        c = s[k]
        if c == "!":
            if len(count) > 0:
                idx += _fromB64(count)
                count = ""
            tl.append(idx)
            idx += 1
        else:
            count += c

    if (len(count) > 0):
        idx += _fromB64(count)

    return (idx, tl)


def getL1TriggerList(s):

    TAV = []
    TAP = []
    TBP = []

    (nbits, tl) = getTriggerList(s)
    trigLen = nbits // 3

    for pos in tl:
        if (pos >= 2*trigLen):
            TAV.append(pos-2*trigLen)
        elif (pos >= trigLen):
            TAP.append(pos-trigLen)
        else:
            TBP.append(pos)

    l1tl = {'TAV': TAV, 'TAP': TAP, 'TBP': TBP}

    return l1tl


def dumpEvent(evt, opt):

    # similar to print(evt)
    # just to avoid format changes in protobuf printing
    # optional fields are checked for existence

    print('======================================================')
    print('> eventPB')
    print('runNumber:', evt.runNumber)
    print('eventNumber:', evt.eventNumber)
    print('lumiBlock:', evt.lumiBlock)
    print('timeStamp:', evt.timeStamp)
    print('timeStampNSOffset:', evt.timeStampNSOffset)
    print('bcid:', evt.bcid)
    if evt.HasField("extendedLevel1ID"):
        print('extendedLevel1ID:', evt.extendedLevel1ID)
    print('isSimulation: {}'.format('true' if evt.isSimulation else 'false'))
    print('isCalibration: {}'.format('true' if evt.isCalibration else 'false'))
    print('isTestBeam: {}'.format('true' if evt.isTestBeam else 'false'))
    if evt.HasField("L1PassedTrigMask"):
        print('L1PassedTrigMask: "{}"'.format(evt.L1PassedTrigMask))
    if evt.HasField("L2PassedTrigMask"):
        print('L2PassedTrigMask: "{}"'.format(evt.L2PassedTrigMask))
    if evt.HasField("EFPassedTrigMask"):
        print('EFPassedTrigMask: "{}"'.format(evt.EFPassedTrigMask))
    if evt.HasField("SMK"):
        print('SMK:', evt.SMK)
    if evt.HasField("HLTPSK"):
        print('HLTPSK:', evt.HLTPSK)
    if evt.HasField("L1PSK"):
        print('L1PSK:', evt.L1PSK)
    if evt.HasField("mcEventWeight"):
        print('mcEventWeight:', evt.mcEventWeight)
    if evt.HasField("mcChannelNumber"):
        print('mcChannelNumber:', evt.mcChannelNumber)
    for tk in evt.eitoken:
        print('eitoken {')
        print('  name: "{}"'.format(tk.name))
        print('  token: "{}"'.format(tk.token))
        print('}')
    print()

    if opt.decodeL1trig and evt.HasField("L1PassedTrigMask"):
        L1mask = evt.L1PassedTrigMask
        L1 = getL1TriggerList(L1mask)

        print('TBP: ', L1['TBP'])
        print('TAP: ', L1['TAP'])
        print('TAV: ', L1['TAV'])

    if opt.decodeL1trig0 and evt.HasField("L1PassedTrigMask"):
        L1mask = evt.L1PassedTrigMask
        L1 = getL1TriggerList(L1mask)

        TAV2 = L1['TAV']
        TAP2 = [k for k in L1['TAP'] if k not in L1['TAV']]
        TBP2 = [k for k in L1['TBP'] if k not in L1['TAP']]

        print('TBP*: ', TBP2)
        print('TAP*: ', TAP2)
        print('TAV*: ', TAV2)

    if opt.decodeL2trig and evt.HasField("L2PassedTrigMask"):
        L2mask = evt.L2PassedTrigMask
        if ';' in L2mask:
            L2mask = L2mask.split(';')
            L2 = {}
            nb, L2['PH'] = getTriggerList(L2mask[0])
            nb, L2['PT'] = getTriggerList(L2mask[1])
            nb, L2['RS'] = getTriggerList(L2mask[2])

            print('L2_PH: ', L2['PH'])
            print('L2_PT: ', L2['PT'])
            print('L2_RS: ', L2['RS'])
        else:
            nb, L2 = getTriggerList(L2mask)
            print('L2: ', L2)

    if opt.decodeEFtrig and evt.HasField("EFPassedTrigMask"):
        EFmask = evt.EFPassedTrigMask
        if ';' in EFmask:
            HLTmask = EFmask.split(';')
            HLT = {}
            nb, HLT['PH'] = getTriggerList(HLTmask[0])
            nb, HLT['PT'] = getTriggerList(HLTmask[1])
            nb, HLT['RS'] = getTriggerList(HLTmask[2])

            print('PH: ', HLT['PH'])
            print('PT: ', HLT['PT'])
            print('RS: ', HLT['RS'])
        else:
            nb, HLT = getTriggerList(EFmask)
            print('HLT: ', HLT)


def main():

    # analyze options
    opt = options(sys.argv)
    if opt.decodetrig:
        opt.decodeL1trig = True
        opt.decodeL2trig = True
        opt.decodeEFtrig = True

    fname = opt.eispbfile
    try:
        spbf = SpbFile(fname)
        info = spbf.getInfo()
    except Exception as e:
        print("Error reading SPB file. Please check file: " + str(e))
        sys.exit(1)

    print("")
    print(" version: ", info['version'])
    print(" #input files: ", info['nfiles'])
    print("  number of events: ", info['nevents'])
    print("  StartProcTime: ", info['startProcTime'])
    print("  EndProcTime: ", info['endProcTime'])
    print("  TaskID: ", info['taskID'])
    print("  JobID: ", info['jobID'])
    print("  InputDsName: ", info['inputDsName'])
    print("  Includes Provenance: ", info['provenanceRef'])
    print("  Includes Trigger: ", info['triggerInfo'])

    for f in info['guids']:
        print("")
        print("File {:d}".format(f['fileno']))
        print("  Events in this file:             {:8d}".format(f['nevents']))
        print("  Unique events in this file:      {:8d}".format(f['nuevents']))
        print("  StartProcTime:              {:13d}".format(
            f['fileStartProcTime']))
        print("  EndProcTime:                {:13d}".format(
            f['fileEndProcTime']))
        print("  AMITag:          {}".format(f['AMITag']))
        print("  TrigStream:      {}".format(f['trigStream']))
        print("  ProjName:        {}".format(f['projName']))
        print("  RunEvtRanges:    {}".format(f['runevtRanges']))
        print("  GUID:            {}".format(f['guid']))

    eipbof = EIPBof(spbf.getVersion())

    print("")
    print("Summary: (info for {:d} events max)".format(opt.evtmax))
    print("")

    nevt_shown = 0
    while True:
        (mtype_ver, mlen, msg) = spbf.getMsg()
        mtype = (mtype_ver & 0x000fff00) >> 8
        if mtype == eic.EI_PROTO_HEADER:
            header = eipbof.Header()
            header.ParseFromString(msg)
        elif mtype == eic.EI_PROTO_BEGINGUID:
            beginGUID = eipbof.BeginGUID()
            beginGUID.ParseFromString(msg)
        elif mtype == eic.EI_PROTO_ENDGUID:
            endGUID = eipbof.EndGUID()
            endGUID.ParseFromString(msg)
        elif mtype == eic.EI_PROTO_EIEVENT:
            eventPB = eipbof.EIEvent()
            eventPB.ParseFromString(msg)
            if nevt_shown < opt.evtmax:
                if (opt.evtnumber != 0 and
                        opt.evtnumber != eventPB.eventNumber):
                    continue
                if (opt.runnumber != 0 and
                        opt.runnumber != eventPB.runNumber):
                    continue
                dumpEvent(eventPB, opt)
                nevt_shown += 1
            else:
                break
        elif mtype == eic.EI_PROTO_TRIGGERMENU:
            triggerMenu = eipbof.TriggerMenu()
            triggerMenu.ParseFromString(msg)
        elif mtype == eic.EI_PROTO_TRAILER:
            trailer = eipbof.Trailer()
            trailer.ParseFromString(msg)
            break
        else:
            print(mtype)
            break

    spbf.close()


if __name__ == '__main__':
    main()
