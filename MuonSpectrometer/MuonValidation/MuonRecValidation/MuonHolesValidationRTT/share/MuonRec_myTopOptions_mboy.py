from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

if 'EvtMax' not in dir():
    EvtMax = 5000 # maximum number of events to process

# add some more flags available for standalone running
import MuonRecExample.MuonRecStandaloneFlags

# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup


#--------------------------------------------------------------------------------
# Input
#--------------------------------------------------------------------------------
### choose one of the pre-defined data files from DataFiles.py
#muonRecFlags.InputData = 'p20'
#muonRecFlags.InputData = 'FDR1_BS_RTT'

### To run on your private input files, uncomment the following lines:
# from MuonRecExample.DataFiles import DataInfo,DataFilesInfoMap
# DataFilesInfoMap['myData'] = DataInfo(["/path/to/my/datafile1","/path/to/my/datafile2","path/to/mydir/myprefix.[00001-6].mypostfix"],
#                                       'RDO', # type of input data: 'BS','RDO','ESD','AOD' etc
#                                       'ATLAS-CSC-01-02-00' , # geometry tag
#                                       'OFLCOND-CSC-00-00-00' ) # optional conditions tag
# muonRecFlags.InputData = 'myData'

#--------------------------------------------------------------------------------
# Output
#--------------------------------------------------------------------------------
#recFlags.doCBNT=False
#recFlags.doWriteESD = True
#muonRecFlags.doCalib = False
#muonCalibFlags.Mode = 'ntuple' # segments only

###
#muonRecFlags.doMuonboy = False  # switch on or off Muonboy
muonRecFlags.doMoore = False    # switch on or off Moore

#recFlags.doNameAuditor = True
#muonRecFlags.doVP1 = True    # Decide whether to run Virtual Point1 graphical event display
#recFlags.doTruth=True

#recFlags.doTrigger = True

# Read geometry alignment corrections from DB
#muonRecFlags.useAlignmentCorrections = True
##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####


    ###### put any user finetuning before this line #####
except:
    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # but still exit with error
    raise
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
