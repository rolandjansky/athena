#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import os
# from ROOT import PathResolver
from PathResolver import PathResolver


def getScalefactor(tool_options):

    print('EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile '+tool_options+' 2>&1')
    result = os.popen('EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile ' +
                      tool_options+' 2>&1').read().strip()
    print(result)
    return result


stype = 'FullSim'
runno = 334842
model = 'TOTAL'
pT = 41212.1
eta = -0.94
other = '| grep SF'
eleid = 'Medium'

thesfs = set()
# first check: go through correlation models and see if the results are the same
print(' ==== CHECK I:  CORRELATION MODELS ==== ')
# we don't test COMBMCTOYS because it's subject to stat fluctuations
for thismodel in [model, 'SIMPLIFIED', 'FULL']:
    sf = getScalefactor(' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' %
                        (stype, runno, thismodel, eta, pT, eleid, other)).split(':')[5]
    if sf not in thesfs:
        thesfs.add(sf)
if len(thesfs) > 1:
    print('--------------------------------------------------')
    print(' the uncertainty models yield inconsistent results ', thesfs)
    print('--------------------------------------------------')
    exit()

# second check: check that are map file doesn't contain duplicates
themap = getScalefactor(' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' %
                        (stype, runno, model, eta, pT, eleid, '-l 0 | grep map')).split("'")[1]
thekeys = set()
print(' ==== CHECK II:  DEFAULT MAP ==== ')
for line in open(PathResolver.FindCalibFile(themap)).readlines():
    key = line.rstrip().split('=')[0]
    if key not in thekeys:
        thekeys.add(key)
    else:
        print('--------------------------------------------------')
        print(' there are duplicate keys in your map ', key)
        print('--------------------------------------------------')
        exit()
print('    ---> map looks good! ')

# third check: print all ID levels
print(' ==== CHECK III:  ID LVL ==== ')
for thisid in [eleid, 'LooseBLayer', 'Tight']:
    getScalefactor(' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' %
                   (stype, runno, model, eta, pT, thisid, other))

# fourth check: check a couple of pT, eta, runno and make sure they're different
print(' ==== CHECK IV:  PT/ETA/RUN ==== ')
for thissim in ['FullSim', 'AtlFast2']:
    for thisrunno in [270000, 300000, 334320, 352183]:
        for thispT in [7421.4, pT, 12128482.9]:
            for thiseta in [-2.42, 0.94]:
                flags = (thissim, thisrunno, model,
                         thiseta, thispT, eleid, other)
                sf = getScalefactor(
                    ' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' % flags).split(':')[5]
                if sf not in thesfs:
                    thesfs.add(sf)
                else:
                    # we don't exit since this can be on purpose
                    print(
                        " we got a duplicate scale factor! "
                        "are you sure it\'s supposed to be there?")

# fifth check: also run a couple of reco, iso and trigger scale factors
print(' ==== CHECK IV:  RECO+ISO+TRIGGER ==== ')
getScalefactor(' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' %
               (stype, runno, model, eta, pT, 'Reconstruction', other))
for thisother in [
    '-i FixedCutTight',
    '-i FixedCutTight -g SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2017_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0'
]:
    for thissim in ['FullSim']:  # ,'AtlFast2'] : # to be added later
        for thisrunno in [270000, 300000, 334320]:  # ,352183] : # to be added later
            flags = (thissim, thisrunno, model, eta,
                     pT, eleid, thisother+other)
            getScalefactor(
                ' -t %s -r %d -c %s -e %1.2f -p %1.2f -d %s %s ' % flags)

print(' ==== DONE ==== ')
