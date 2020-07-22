#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import sys
import eformat
from collections import defaultdict

filename = sys.argv[1]
bsfile = eformat.istream(filename)
print("Read file %s with %i events" % (filename,bsfile.total_events))

event = bsfile[0]


rob_by_subdet = defaultdict(list)
subdets = set()

subdetsData = [
    'LAR_EM_BARREL_A_SIDE', 'LAR_EM_BARREL_C_SIDE', 'LAR_EM_ENDCAP_A_SIDE', 'LAR_EM_ENDCAP_C_SIDE', 'LAR_FCAL_A_SIDE', 'LAR_FCAL_C_SIDE', 'LAR_HAD_ENDCAP_A_SIDE', 'LAR_HAD_ENDCAP_C_SIDE',
    'MUON_MDT_BARREL_A_SIDE', 'MUON_MDT_BARREL_C_SIDE', 'MUON_MDT_ENDCAP_A_SIDE', 'MUON_MDT_ENDCAP_C_SIDE', 'MUON_RPC_BARREL_A_SIDE', 'MUON_RPC_BARREL_C_SIDE', 'MUON_TGC_ENDCAP_A_SIDE', 'MUON_TGC_ENDCAP_C_SIDE',
    'PIXEL_BARREL', 'PIXEL_B_LAYER', 'PIXEL_DISK_SIDE',
    'SCT_BARREL_A_SIDE', 'SCT_BARREL_C_SIDE', 'SCT_ENDCAP_A_SIDE', 'SCT_ENDCAP_C_SIDE',
    'TDAQ_CALO_CLUSTER_PROC_DAQ', 'TDAQ_CALO_CLUSTER_PROC_ROI', 'TDAQ_CALO_JET_PROC_DAQ', 'TDAQ_CALO_JET_PROC_ROI', 'TDAQ_CALO_PREPROC',
    'TDAQ_CTP',
    #'TDAQ_EVENT_FILTER', 'TDAQ_LVL2',
    'TDAQ_MUON_CTP_INTERFACE',
    'TILECAL_BARREL_A_SIDE', 'TILECAL_BARREL_C_SIDE', 'TILECAL_EXT_A_SIDE', 'TILECAL_EXT_C_SIDE',
    'TRT_BARREL_A_SIDE', 'TRT_BARREL_C_SIDE', 'TRT_ENDCAP_A_SIDE', 'TRT_ENDCAP_C_SIDE'
    ]


first = True
for rob in event:
    if first:
        first = False
        #help(rob)
        #print dir(rob)
        #print "ROB version",rob.version(),type(rob.version())
        #print "ROB minor version",rob.minor_version()
        #print "ROD version",rob.rod_version()
        #print "ROB source sub det ID", rob.rob_source_id().subdetector_id()
        #print "ROB source sub det ID", dir(rob.rob_source_id().subdetector_id())
        #print "ROB header",rob.header_size_word(),rob.header()
        #print "ROD header",len(rob.rod_header()), '[' + ", ".join(["0x%08x" % x for x in rob.rod_header()])+"]"

    # define the key for the dictionary in which we store ROD versions by subdetector
    det = rob.rob_source_id().subdetector_group().name
    split_index = 10
    if det in ['PIXEL','SCT','TRT','LAR','TILECAL']:
        split_index = 1
    if det in ['MUON']:
        split_index = 4
    if det in ['TDAQ']:
        split_index = 10

    subdet = rob.rob_source_id().subdetector_id().name
    subdets.add(subdet)
    key = '_'.join( subdet.split('_')[:split_index] )

    rob_by_subdet[ key ] += [ rob ]



# print

width = max([len(s) for s in rob_by_subdet.keys()]+[15]) + 5

print("Detector%sROD version%s#ROBs" % (' ' * (width-8), ' ' * (14) ))
print("--------%s-----------%s-----" % ('-' * (width-8), '-' * (14) ))

for subdet in sorted(rob_by_subdet.keys()):

    roblist = rob_by_subdet[subdet]

    robset = set(["%s" % rob.rod_version() for rob in roblist])

    print("%-*s%-*s%i" % (width, subdet, 25, ", ".join(robset), len(roblist) ))

subdet_missing = set(subdetsData) - subdets
if subdet_missing:
    print("Missing the following sub detectors in the file:")
    for sd in sorted(subdet_missing):
        print("    ",sd)
