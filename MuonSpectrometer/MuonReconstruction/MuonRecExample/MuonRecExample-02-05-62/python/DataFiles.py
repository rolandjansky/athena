# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file DataFiles.py A list of standard datasets to choose from when running MuonReco in standalone mode.
#
# Each dataset is identified with a key (the key in the dictionary), and it contains in addition to
# the list of files, the type of file (HITS,RDO,...) and the DetDescrVersion to use.
# A dataset is chosen by setting muonRecFlags.InputData to one of the keys.

## @namespace python::DataFiles @copydoc DataFiles.py

## ATLAS-GEO-02-01-00

## Zmumu
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/HITS/valid1.105145.PythiaZmumu.simul.HITS.e380_s494_tid040060/
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105145.PythiaZmumu.digit.RDO.e380_s494_tid040060/
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105145.PythiaZmumu.recon.ESD.e380_s494_r702_tid070477
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/AOD/valid1.105145.PythiaZmumu.recon.AOD.e380_s494_r702_tid070477

## ttbar
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e380_s494_tid040030/

## ttbar cavBG_2e33SF05
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e380_s494_d153_tid040636/

## H->4l (130 GeV)
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.005300.PythiaH130zz4l.digit.RDO.e322_s461_d156_tid044400/

## ZPrime
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.006601.Pythia_ZHuu2000.digit.RDO.e322_s461_d156_tid044407/

## mu 10GeV
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007211.singlepart_mu10.digit.RDO.e322_s461_d156_tid044413/

## mu 100 GeV
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007233.singlepart_mu100.digit.RDO.e322_s461_d156_tid044414/

## mu 200 GeV
## /castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007234.singlepart_mu200.digit.RDO.e322_s461_d156_tid044415/



from MuonRecExample.MuonRecUtils import DataInfo

DataFilesInfoMap = {
    'Zmumu' :
    DataInfo("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/Zmumu_15616/",
             'RDO',maxFiles=10) ,

    'Zmumu_HITS' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/HITS/valid1.105145.PythiaZmumu.simul.HITS.e380_s559_tid071852/',
             'HITS',maxFiles=10) ,

    'Zmumu_pile1sf5_old' :
    DataInfo('root://castoratlas//castor/cern.ch/user/r/rtt/RTTdata/pile1sf05_misal1_csc11.005145.PythiaZmumu.digit.RDO.v12000605/pile1sf05_misal1_csc11.005145.PythiaZmumu.digit.RDO.v12000605_tid008668._[00001-10].pool.root.1',
             'RDO', 'ATLAS-CSC-01-02-00') ,

    'ttbar' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e380_s632_tid097896/',
             'RDO',maxFiles=10) ,

    'ttbar cavBG_2e33SF05' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e380_s494_d153_tid040636/',
             'RDO', 'ATLAS-GEO-02-01-00',maxFiles=10) ,


    'H130ZZ4l' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.005300.PythiaH130zz4l.digit.RDO.e322_s461_d156_tid044400/',
             'RDO', 'ATLAS-GEO-02-01-00',maxFiles=10) ,


    'ZPrime' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.006601.Pythia_ZHuu2000.digit.RDO.e322_s461_d156_tid044407/',
             'RDO', 'ATLAS-GEO-02-01-00',maxFiles=10) ,

    'p10' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007211.singlepart_mu10.digit.RDO.e322_s461_d156_tid044413/',
             'RDO', 'ATLAS-GEO-02-01-00',maxFiles=10) ,

    'p100' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007233.singlepart_mu100.digit.RDO.e322_s461_d156_tid044414/',
             'RDO', 'ATLAS-GEO-02-01-00',maxFiles=10) ,

    'p200' :
    DataInfo('root://castoratlas//castor/cern.ch/grid/atlas/atlasmcdisk/valid1/RDO/valid1.007234.singlepart_mu200.digit.RDO.e322_s461_d156_tid044415/',
             'RDO', 'ATLAS-GEO-02-01-00') ,

    
    }

