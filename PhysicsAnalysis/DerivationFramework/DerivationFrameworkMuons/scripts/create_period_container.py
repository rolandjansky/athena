#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
######################
##
## Contact: Dongliang Zhang <dongliang.zhang@cern.ch>
##
## Setup:
##   setupATLAS
##   lsetup pyami
######################

from __future__ import print_function

import sys
import pyAMI.client
import pyAMI.atlas.api as AtlasAPI
from subprocess import call

client = pyAMI.client.Client(
  'atlas'
  #'atlas-replica'
)
AtlasAPI.init()


def getRuns(grl):
    listGRL = []
    for i in grl.split(','):
        import xml.etree.ElementTree
        listGRL += [atype.text.split(',') for atype in xml.etree.ElementTree.parse(grl).getroot().find('NamedLumiRange').findall('Metadata') if atype.get('Name') == 'RunList']
    return listGRL

class config1:
    def __init__(self, project, derivation, period, acceptTags, sTag, autoCreate=False, grl=None):
        self.project = project
        self.period = period
        self.sTag = sTag
        self.derivation = derivation
        self.acceptTags = acceptTags
        self.autoCreate = autoCreate
        self.grl = grl
        self.outScript=None

def makeContainer(c1):
    ### get run list
    x0 = AtlasAPI.list_datasets(client, patterns = [c1.project+'.%.physics_Main.merge.AOD%'], fields = ['run_number', 'ldn', 'events', 'total_size'], order=None, limit=None, show_archived=False, grl=c1.grl, data_period=c1.period)
    runs = set([a['run_number'] for a in x0])

    if len(runs)==0:
        print ('no run in period'+c1.period+' is listed in GRL:', c1.grl)
        return

    ### get AOD datasets
    x = AtlasAPI.list_datasets(client, patterns = [c1.project+'.%.physics_Main.merg%'+c1.derivation+'%'], fields = ['run_number', 'ldn', 'events', 'total_size'], order=None, limit=None, show_archived=False, grl=c1.grl, data_period=c1.period)

    ### get dataset info
    dic1={}
    for a in x:
        print (a['run_number'], a['ldn'])
        try:
            dic1[a['ldn'].split('_')[-1]].append((a['run_number'],a['ldn']))
        except KeyError:
            dic1[a['ldn'].split('_')[-1]] = [(a['run_number'],a['ldn'])]

    ### get the tags, sorted by popularity
    allTags = sorted(dic1.keys(), key=lambda k: len(dic1[k]),reverse=True)
    for tag in allTags:
        print (tag,':', end='')
        for xx in dic1[tag]:
            print (xx[0], end='')
        print()

    ### use the most popular tags if not spicified
    t_acceptTags = c1.acceptTags if c1.acceptTags else allTags

    #### get the list of dataset
    ds = []
    for t in t_acceptTags:
        for a in dic1.get(t,[]):
            found = False
            for d in ds:
                if d[0] == a[0]:
                    found = True
                    break
            if not found:
                ds.append(a)

    ### Warning when the given tag does not select any dataset
    if len(ds)==0:
        print ('No dataset in period',c1.period+', exiting...')
        return

    #### find any missing runs
    for d in ds:
        print (d[0],d[1])
        runs.remove(d[0])
    print (runs)

    #### Warn when there are any missing runs
    if len(runs) !=0:
        print ('!'*10)
        print ('MISSING ', ' '.join(runs))

    ### prepare commands
    dlist=','.join([d[1] for d in ds])
    superTag = 'period'+c1.period+','+c1.sTag+ds[0][1][-5:]
    comments = superTag+','+c1.derivation

    cmd='ami cmd COMAPopulateSuperProductionDataset -rucioRegistration="yes" -creationComment="'+comments+'" -selectionType="run_config" -superTag="'+superTag+'" -containedDatasets="'+dlist+'" -separator="," '
    print ('command:',cmd)

    ### write out script if asked
    if c1.outScript:
        with open(c1.outScript,'a') as f1:
            f1.write(cmd+'\n')
        return

    ### create the contianier imitiately if desired
    if not c1.autoCreate:
        while True:
            x = raw_input("create contianer: y[es]/N[o]/e[xit]")
            if x == 'e' or x=='N': return
            elif x=='y': break

    call(cmd, shell=True)


def doFirstTry():
    grl15 = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.xml'
    grl16 = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20160916/physics_25ns_20.7_ignoreToroidStatus.xml'
    c1 = config1('data16_13TeV', 'MUON0', 'A', sTag='grp16_v01_', acceptTags=['p2769','p2689','p2667'], autoCreate=False, grl=grl16)
    c1.outScript = 'createIt_15.sh'

#     periods = AtlasAPI.list_dataperiods(client, 2, 16)

    periods = AtlasAPI.list_dataperiods(client, 2, 15)
    c1.project = 'data15_13TeV'
    c1.sTag='grp15_v01_'
    c1.acceptTags=['p2667']
    c1.grl = grl15

    pds =[p['period'] for p in periods if p['status']=='frozen']
#     print (pds)
#     sys.exit(0)

    for p in pds:
        for d in ['MUON0','MUON1','MUON2','MUON3']:
#         for d in ['MUON0']:
            c1.period = p
            c1.derivation = d
            makeContainer(c1)

def doData15():
    grl15 = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.xml'
    c1 = config1('data15_13TeV', 'MUON0', 'A', sTag='grp15_v01_', acceptTags=None, autoCreate=False, grl=None)
    c1.outScript = 'createIt_15v2.sh'

    periods = AtlasAPI.list_dataperiods(client, 2, 15)
    c1.project = 'data15_13TeV'
    c1.sTag='grp15_v02_'
    c1.acceptTags=['p2889']
    c1.grl = grl15

    pds =[p['period'] for p in periods if p['status']=='frozen']
#     print (pds)
#     sys.exit(0)

    for p in pds:
        for d in ['MUON0','MUON1','MUON2','MUON3']:
            c1.period = p
            c1.derivation = d
            makeContainer(c1)


def do2016LaterPeriods():
    grl16 = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20161101/physics_25ns_20.7_ignoreToroidStatus.xml'
    c1 = config1('data16_13TeV', 'MUON0', 'A', sTag='grp16_v01_', acceptTags=['p2769','p2689','p2667'], autoCreate=False, grl=grl16)
    c1.outScript = 'createIt_16Later2.sh'

    periods = AtlasAPI.list_dataperiods(client, 2, 16)

    c1.project = 'data16_13TeV'
    c1.sTag='grp16_v01_'
    c1.acceptTags=None
    c1.grl = grl16

#     pds =[p['period'] for p in periods if p['status']=='frozen']
#     print (pds)
#     sys.exit(0)
    pds = ['J', 'K', 'L']

    for p in pds:
        for d in ['MUON0','MUON1','MUON2','MUON3']:
            c1.period = p
            c1.derivation = d
            makeContainer(c1)

if __name__ == '__main__':
    do2016LaterPeriods()
#     doData15()
