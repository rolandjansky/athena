#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: John Chapman (Cambridge), September 2010
# Usage:
#   -specify default inputs for transforms

#Default values of input/output types, for standard tests
DefaultInputs={
    'inputBSFile' : '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts',
    'inputRDOFile': '/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root',
    'inputESDFile': '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts.ESD.pool.root', 
    'inputAODFile': '/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts.AOD.pool.root', 
    'cosmicsBS'   : '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data',
    'topBS'       : '/afs/cern.ch/atlas/offline/test/user.jboyd.DiLeptonicTopSkim.RAW.Run183347.10events.dat',
    
    'inputEvgenFile': '/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E50_eta0-25-7000.evgen.pool.root',
    'inputHITSFile' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1',
    'NDMinbiasHitsFile': 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000034.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000043.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000060.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105001.pythia_minbias.merge.HITS.e577_s932_s952_tid170554_00/HITS.170554._000082.pool.root.1',
    #'SDMinbiasHitsFile': '/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E50_eta0-25-7000_ATLAS-GEO-11-00-00.hits.pool.root',
    #'DDMinbiasHitsFile': '/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E50_eta0-25-7000_ATLAS-GEO-11-00-00.hits.pool.root',
    'cavernHitsFile': 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000011.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000111.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000144.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000150.pool.root.1,root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.005008.CavernInput.merge.HITS.e4_e607_s951_s952_tid170551_00/HITS.170551._000151.pool.root.1',
    'beamHaloHitsFile': 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108852.BeamHaloInputs.merge.HITS.e4_e567_s949_s952_tid170552_00/HITS.170552._000001.pool.root.1',
    'beamGasHitsFile': 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108863.Hijing_beamgas.merge.HITS.e4_s950_s952_tid170553_00/HITS.170552._000087.pool.root.1'
    }


def getOutputFileName(fmt):
    value='my'+fmt
    fmt=fmt.split('_')[0]
    if fmt in ['RAW', 'DRAW', 'BS']:
        value += '.data'
    elif fmt in ['RDO', 'EVNT', 'TAG', 'HITS', 'ESD', 'DESD', 'D2ESD', 'DESDM', 'D2ESDM', 'AOD', 'DAOD', 'D2AOD', 'DAODM', 'D2AODM']:
        value += '.pool.root'
    elif fmt in ['HIST', 'NTUP']:
        value += '.root'
    return value


def getInputFileName(arg, tag=None):
    arg=arg.lstrip('-')
    if tag=="q120" and arg=='inputBSFile':
        return DefaultInputs["cosmicsBS"]                
    elif tag=="q126" and arg=='inputBSFile':
        return DefaultInputs["topBS"]
    datatype = arg.replace('input','').replace('File', '')
    return DefaultInputs.get(arg, getOutputFileName(datatype))

    

    
