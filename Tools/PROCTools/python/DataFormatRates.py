# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script for checking the yield and skimming efficiency for derived data formats

# needed python imports
import sys, os, argparse, subprocess

# use (rather than copy) some useful functions
from PROCTools import ExtractEvents as ee

# handle the CL parameters
parser = argparse.ArgumentParser(description='Extract yields and skimming efficiencies for derived data formats using ARQ and AMI. Written by C. Ohm.')
parser.add_argument('-r', '--runs', required=True, type=str, help='Run number range, e.g. 123456 or 123456-199999')
parser.add_argument('-f', '--fileformat', type=ee.validFileFormat, help='File format: (D)RAW(_XYZ), (D)ESD(_XYZ), (D)AOD(_XYZ)', default='AOD')
parser.add_argument('-b', '--baselineformat', type=ee.validFileFormat, help='File format to use for reference yields for the efficiency calculation (defaults to AOD)', default='AOD')
parser.add_argument('-m', '--matchingstring', type=str, nargs='?', default='', help='String for matching the dataset to look in, useful when there are several processings available, or both merged and unmerged datasets, e.g. "merge.DESDM_RPVLL*f620_m*" will do what you expect')
parser.add_argument('-s', '--stream', type=str, nargs='?', help='Stream name, defaults to "physics_Main"', default='physics_Main')
parser.add_argument('-v', '--verbose', action='store_true', default=False, help='Verbose mode, prints out eos paths and commands, file names, etc')
parser.add_argument('-q', '--quick', action='store_true', default=False, help='Quick mode, skips ARQ call and uses local pickle file from previous query')

args = parser.parse_args()
#print args

if args.quick:
    print "Quick mode: will use pickle file on disk from previous query, will not call ARQ"
else:
    # run the query - all of the retrieved data is stored in a pickle file on disk
    cmd = "AtlRunQuery.py \"find run %s and ready and st %s 100k+ / show lumi\"" % (args.runs, args.stream)
    if args.verbose:
        print "Will now execute the following ARQ query:"
        print "   %s" % cmd
    env = os.environ.copy()
    output = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
    if args.verbose:
        for line in output.stdout.readlines():
            print "   %s" % line.rstrip()
    print "Retrieved ARQ results"

print "Will now open the pickle file containing the needed lumi info"

# read in the pickle file with the results
import pickle
f = open("data/atlrunquery.pickle")
d = pickle.load(f)

print "Loaded pickle file containing info for %d runs" % len(d['Run'])

lumiPerRun = {}
eventsPerRun = {}
selectedEventsPerRun = {}

for run in d['Run']:
    print "Will now sum up integrated luminosity for run %d" % run
    # calculate the integrated luminosity based on the length of the LBs and the inst lumi per LB
    lbDict = d[run]['#LB']
    lumiTag = [key for key in d[run] if 'ofllumi:0:' in key][0]
    print "  Using lumi tag: %s" % lumiTag
    lumiDict = d[run][lumiTag]
    print lumiDict
    integratedLumi = 0
    for lb in range(1,len(lbDict)):
        print "lb: %d" % lb
        print "lumiDict[lb-1]:"
        print lumiDict[lb-1]
        #print "Will now add lumi for LB %d with length %f and lumi %f" % (lb, lbDict[1][lb]-lbDict[1][lb-1], lumiDict[lb-1]['value'])
        #integratedLumi += lumiDict[lb-1]['value']*(lbDict[1][lb]-lbDict[1][lb-1]) # this was good for online lumi which comes in std query.. /CO
        integratedLumi += float(lumiDict[lb-1]['value']) # this already has integrated lumi per LB, I guess? /CO
        #print "  LB %d: integrated lumi = %f" % (lb, integratedLumi)
    print " => Done: %f x 10^30 cm^-2" % (integratedLumi)
    lumiPerRun[run] = integratedLumi

# now go to AMI to get the event yields for the datasets you're interested in
import pyAMI.client
import pyAMI.atlas.api as AtlasAPI
client = pyAMI.client.Client('atlas')
AtlasAPI.init()

for run in d['Run']:

    pattern = "data15_13TeV.%08d.physics_Main.merge.DESDM_RPVLL" % run
    pattern += '.f%_m%'
    dslist = AtlasAPI.list_datasets(client, patterns = pattern, fields = ['events'], type = 'DESDM_RPVLL')
    #print dslist
    if len(dslist) > 0:
        print dslist[0]['events']
        selectedEventsPerRun[run] = dslist[0]['events']
    pattern = "data15_13TeV.%08d.physics_Main.merge.AOD" % run
    pattern += '.f%_m%'
    dslist = AtlasAPI.list_datasets(client, patterns = pattern, fields = ['events'], type = 'AOD')
    #print dslist
    if len(dslist) > 0:
        print dslist[0]['events']
        eventsPerRun[run] = dslist[0]['events']

import ROOT as r
import math

bins = len(eventsPerRun)

yieldPerRunHisto = r.TH1F("YieldPerRun", "YieldPerRun", bins, 0, bins)
skimmingEffPerRunHisto = r.TH1F("SkimmingEfficiencyPerRun", "SkimmingEfficiencyPerRun", bins, 0, bins)
yieldPerLumiHisto = r.TH1F("YieldPerRunPerLumi", "YieldPerRunPerLumi", bins, 0, bins)
runs = 0
for run in eventsPerRun:
    events = float(selectedEventsPerRun[run])
    allevents = float(eventsPerRun[run])
    lumi = lumiPerRun[run]
    yieldPerRunHisto.SetBinContent(runs+1, events)
    yieldPerRunHisto.SetBinError(runs+1, math.sqrt(events))
    yieldPerRunHisto.GetXaxis().SetBinLabel(runs+1, str(run))
    skimmingEffPerRunHisto.SetBinContent(runs+1, events/allevents)
    skimmingEffPerRunHisto.SetBinError(runs+1, events/allevents*math.sqrt(1/events + 1/allevents))
    skimmingEffPerRunHisto.GetXaxis().SetBinLabel(runs+1, str(run))
    yieldPerLumiHisto.SetBinContent(runs+1, events/lumi)
    yieldPerLumiHisto.SetBinError(runs+1, math.sqrt(events)/lumi)
    yieldPerLumiHisto.GetXaxis().SetBinLabel(runs+1, str(run))
    runs += 1

c1 = r.TCanvas("PerRun", "PerRun")
yieldPerRunHisto.GetYaxis().SetRangeUser(0, yieldPerRunHisto.GetMaximum()*1.2)
yieldPerRunHisto.SetTitle("Event yield per run;Run;Events in DESDM_RPVLL stream")
yieldPerRunHisto.Draw()
c2 = r.TCanvas("EffPerRun", "EffPerRun")
skimmingEffPerRunHisto.GetYaxis().SetRangeUser(0, skimmingEffPerRunHisto.GetMaximum()*1.2)
skimmingEffPerRunHisto.SetTitle("Skimming efficiency per run;Run;Fraction of events in DESDM_RPVLL stream")
skimmingEffPerRunHisto.Draw()
c3 = r.TCanvas("PerLumi", "PerLumi")
yieldPerLumiHisto.GetYaxis().SetRangeUser(0, yieldPerLumiHisto.GetMaximum()*1.2)
yieldPerLumiHisto.SetTitle("Event yield per #mub;Run;Events in DESDM_RPVLL/#mub")
yieldPerLumiHisto.Draw()
