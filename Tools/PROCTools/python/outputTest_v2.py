#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
import commands
from optparse import OptionParser



physicsStreams = ["physics_Main","physics_MinBias","physics_MinBiasOverlay","physics_ZeroBias","physics_HardProbes",
                  "physics_TauOverlay","physics_CosmicCalo", "physics_CosmicMuons","physics_IDCosmic",
                    "physics_HLT_IDCosmic","physics_Standby","physics_Late","physics_L1Calo","physics_L1Topo",
                    "express_express"] 


monitoringStreams = ["calibration_Background","calibration_beamspot","calibration_IBLLumi","calibration_PixelBeam", 
                         "caibration_vdM", "calibration_PIxelNoise", "calibration_SCTNoise", "calibration_IDTracks",
                         "calibration_LArCells","calibration_LArCellsEmpty","calibration_LArNoiseBurst","calibration_Tile"
                         "calibration_MuonAll","calibration_CostMonitoring"]

otherStream = ["calibration_IDFwd","calibration_ALFA","calibration_DataScoutingXX","debugrec_hltacc","debugrec_hltrej","debugrec_crashed"] 

def readJson(filename):
    import json
    s = open(filename, 'r').read()
    outputs =  eval(s)
    #print outputs
    return outputs
    
import re

#Dictionary

def_outputs = {'outputDESDM_CALJETFile': {'dstype': 'DESDM_CALJET', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(?!.*physics_Main.*)'}, 'outputDESDM_SGLELFile': {'dstype': 'DESDM_SGLEL', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputDESDM_SLTTMUFile': {'dstype': 'DESDM_SLTTMU', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputDESDM_RPVLLFile': {'dstype': 'DESDM_RPVLL', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputDAOD_IDTRKVALIDFile': {'dstype': 'DAOD_IDTRKVALID', 'ifMatch': 'data[0-9][0-9]_(cos|1beam|.*eV|comm)(?!.*DRAW.*)(.*MinBias.*|.*IDCosmic.*)'}, 'outputDRAW_ZMUMUFile': {'dstype': 'DRAW_ZMUMU', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(?!.*DRAW.*)(.*physics_Main\..*)'}, 'outputDAOD_IDTIDEFile': {'dstype': 'DAOD_IDTIDE', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputDRAW_TAUMUHFile': {'dstype': 'DRAW_TAUMUH', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(?!.*DRAW.*)(.*physics_Main\..*)'}, 'outputDESDM_MSPerfFile': {'dstype': '!replace RAW DESDM_MCP DDESDM_MCP_ZMUMU DESDM_ZMCP', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(.*physics_Main\..*)(.*\.RAW\.*|.*DRAW_ZMUMU.*)'}, 'outputDESDM_EGAMMAFile': {'dstype': 'DESDM_EGAMMA', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputDRAW_EMUFile': {'dstype': 'DRAW_EMU', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(?!.*DRAW.*)(.*physics_Main\..*)'}, 'outputDRAW_EGZFile': {'dstype': 'DRAW_EGZ', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(?!.*DRAW.*)(.*physics_Main\..*)'}, 'outputDESDM_PHOJETFile': {'dstype': 'DESDM_PHOJET', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_Main.*)'}, 'outputAODFile': {'dstype': '!replace RAW AOD', 'ifMatch': 'data[0-9][0-9]_(cos|1beam|.*eV|comm)'}, 'outputDESDM_EOVERPFile': {'dstype': 'DESDM_EOVERP', 'ifMatch': 'data[0-9][0-9]_.*eV(?!.*DRAW.*)(.*physics_MinBias.*)'}, 'outputDESDM_ALLCELLSFile': {'dstype': '!replace RAW ESDM', 'ifMatch': 'data[0-9][0-9]_(1beam|.*eV|comm)(?!.*DRAW_TAUMUH.*)(.*DRAW.*)'}, 'outputHISTFile': {'dstype': 'HIST', 'ifMatch': '(?!.(.*DRAW.*|.*debugrec.*))'}}



parser=OptionParser(usage="\n ./outputTest_v2 -s allStreams  [-f <filename of file containing output dict>] \n")
parser.add_option("-s","--streams"      ,type="string"        ,dest="streams"    ,default="physics_Main,express_express"    ,help="comma seperated list of streams")
parser.add_option("-p","--project"      ,type="string"        ,dest="project"    ,default="data15_13TeV"                    ,help="projects")
parser.add_option("-r","--runnumber"    ,type="string"        ,dest="runnumber"  ,default="00267639"                    ,help="run number")
parser.add_option("-f","--filename"    ,type="string"        ,dest="filename"    ,default="default_outputs.dict"                    ,help="run number")

(options,args)=parser.parse_args()


if options.filename:
    try:
        outputs = readJson(options.filename)
    except:
        print
        print "WARNING: file with dict %s not available... using default outputs dict" %(options.filename)
        print
else:   
    outputs = def_outputs

streams = []
project = options.project
runnumber = options.runnumber
inStreams =  options.streams.split(",")

""""
  define list of streams
"""
if "physicsStreams" in inStreams:
    streams.extend(physicsStreams)
    inStreams.remove("physicsStreams")
if "monitoringStreams" in inStreams:
    streams.extend(monitoringStreams)
    inStreams.remove("monitoringStreams")
if "otherStreams" in inStreams:
    streams.extend(monitoringStreams)
    inStreams.remove("otherStreams")

if "allStreams" in inStreams:
    streams.extend(physicsStreams)
    streams.extend(monitoringStreams)
    streams.extend(monitoringStreams)
    inStreams.remove("allStreams")
  


if  inStreams:
    for stream in inStreams:
        if stream in physicsStreams or stream in monitoringStreams or stream in otherStream:
            streams.append(stream)
        else:
            print "WARNING: %s is not defined in known stream" %(stream)



"""
  print output types per stream
"""
for stream in streams:
    inputds = project+"."+runnumber+"."+stream+".merge.RAW"
    print "Primary outputs %s RAW \n" %(stream)
    print "\t inputDS: %s \n" %(inputds) 
    for k in outputs.keys() :
        if re.match(outputs[k]['ifMatch'], inputds) :
            print "- %s" %(k)
    print 
