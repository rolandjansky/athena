# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon.Utils import unixtools

if not 'FNAMES' in dir():
    paths = os.getenv('DATAPATH').split(os.pathsep) + [os.getenv('ATLAS_REFERENCE_DATA','')]
    testdir = unixtools.find_datafile(os.getenv('ATLAS_REFERENCE_TAG'), paths)
    FNAMES = [os.path.join(testdir, f) for f in ('ntuple.0.root', 'ntuple.1.root')]

if not isinstance(FNAMES, (list,tuple)):
    FNAMES = [FNAMES]
    pass

if not 'TUPLENAME' in dir():
    TUPLENAME = 'egamma'

import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = FNAMES
svcMgr.EventSelector.TupleName = TUPLENAME

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

import AthenaRootComps.AthenaRootCompsConf as arcc
if not 'USEVARHANDLE' in dir():
    USEVARHANDLE=1
if USEVARHANDLE: job += arcc.Athena__RootAsciiDumperAlgHandle("rootdumper")
else:            job += arcc.Athena__RootAsciiDumperAlg      ("rootdumper")

if not 'EVTMAX' in dir():
    EVTMAX=-1
theApp.EvtMax = EVTMAX

if not 'DOWRITE' in dir():
    DOWRITE=1
if DOWRITE:
    svcMgr += CfgMgr.DecisionSvc()
    import AthenaRootComps.WriteAthenaRoot as arcw
    out = arcw.createNtupleOutputStream("StreamD3PD", "d3pd.root", "egamma")
    out.ItemList += [
        "el_n",
        "el_eta",
        "el_jetcone_dr",
        ]
    out.ForceRead = True
    if DOWRITE>1:
        # another output stream... with the same content.
        out = arcw.createNtupleOutputStream("StreamD3PD_2",
                                            "d3pd_2.root",
                                            "egamma")
        out.ItemList += [
            "el_n",
            "el_eta",
            "el_jetcone_dr",
            ]
        out.ForceRead = True
        
    pass

