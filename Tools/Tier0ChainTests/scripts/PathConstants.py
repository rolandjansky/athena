"""Provide hardcoded path related strings shared by a number of modules.

Trivial (value = name) strings:
cmt
share
bin
installArea

Ex-Variables - names tht could once take more than one value:
releaseType
runType

names that take one value
cmtConfigFileMacroName

RTT project renaming anomally:
projectDict

Require updates
branches list of legal branches

Dangerous Kludges
projectMap - list of sub projects - use by findJobOptions - FIXME
"""

import os.path


share = 'share'
cmt = 'cmt'

# historical - still ocurs in summary overview. Alternative was monolithic (?)
releaseType = 'project'

# historical - still ocurs in summary overview and results paths.
# Alternative was kit. Note there is a PathNameBuilder.runType
# (dev,prod,batch,shell) that is not the same thing as this relic.
runType = 'build'

installArea = 'InstallArea'

# actual name of the macro is <package>_TestConfiguration
cmtConfigFileMacroName = '_TestConfiguration'

distArea = '/afs/cern.ch/atlas/software/builds'

eosProjSitRTTPath = '/eos/atlas/atlascerngroupdisk/proj-sit/rtt'

eosBinDir = '/afs/cern.ch/project/eos/installation/atlas/bin' # not needed any more? HAZ
eosLibDir = '/afs/cern.ch/project/eos/installation/atlas/lib64'# not needed any more? HAZ
#eos = os.path.join(eosBinDir, 'eos.select')  # alias to the EOS command
eos = 'eos'


# The path to the private file containing RTT connection params for AMI.
# This file is sourced before launching the user job.
amiConnectionFile = '/afs/cern.ch/user/r/rtt/private/amiConnection.sh'

rttExternals = '/afs/cern.ch/atlas/project/RTT/Externals'

# 31/5/2012. DCubeRunner uses dcubeRefBaseDir as a base dir for data files
dcubeRefBaseDir = '/afs/cern.ch/atlas/project/RTT/DCubeReference'

# 31/5/2012. DCubeRunner uses dcubeCfgBaseDir as a base dir for cfg files
dcubeCfgBaseDir = '/afs/cern.ch/atlas/project/RTT/Results/reference/'

rttBases = {'prod':  '/afs/cern.ch/atlas/project/RTT/prod',
            'dev':   '/afs/cern.ch/atlas/project/RTT/dev'
            }

projectDict = {'AtlasHLT':        'hlt',
               'AtlasProduction': 'offline',
               'AtlasPoint1':     'point1',
               'AtlasTier0':      'tier0',
               'AtlasP1HLT':      'p1hlt'}

invProjectDict = {}
for k, v in projectDict.items():
    invProjectDict[v] = k

resultsBaseURL = 'https://atlas-rtt.cern.ch/'

projects_2017 = [
    'Athena',
    'AthenaP1',    
    ]

projects = [
    'AtlasAnalysis',
    'AtlasLogin',
    'AtlasSetup',
    'AtlasConditions',
    'AtlasOffline',
    'AtlasSimulation',
    'AtlasCore',
    'AtlasProduction',
    'AtlasTrigger',
    'AtlasEvent',
    'AtlasReconstruction',
    'AtlasHLT',
    'AtlasPoint1',
    'AtlasTier0',
    'AtlasP1HLT',
    'AtlasCAFHLT',
    'TopPhys',
    'TAGProd',
    'WZBenchmarks',
    'TrigMC',
    'AtlasDerivation',
    'AtlasProd1',
    'AtlasProd2',
    'AtlasP1MON',
    'AthAnalysisBase',
    'AtlasTestHLT',
    'AtlasPhysics',
    'Athena']

branches_2017 = ['master',
                 '21.0',
                 '21.0.X',]


branches = [
    'cmake',
    'dev',
    'devval',
    '19.0.X',
    '19.0.X.Y',
    '19.0.X-VAL',
    '19.0.X.Y-VAL',
    '19.0.X.Y-VAL2',
    '19.1.X',
    '19.1.X.Y',
    '19.1.X.Y-VAL',
    '19.2.X-VAL',
    '19.2.X.Y',
    '19.2.X.Y-VAL',
    '19.1.X-VAL',
    '19.1.0.Y-VAL',
    '19.1.1.Y-VAL',
    '19.1.3.Y-VAL',
    '19.3.X.Y',
    '19.3.X.Y-VAL',
    '20.X.0',
    '20.X.0-VAL',
    '20.0.X.Y',
    '20.0.X.Y-VAL',
    '20.1.0.Y',
    '20.1.0.Y-VAL',
    '20.1.0.Y-VAL2',
    '20.1.1.Y',
    '20.1.1.Y-VAL',
    '20.1.1.Y-VAL2',
    '20.1.2.Y-VAL',
    '20.1.2.Y-VAL2',
    '20.1.3.Y-VAL',
    '20.1.3.Y-VAL2',
    '20.1.4.Y',
    '20.1.4.Y-VAL',
    '20.1.5.Y',
    '20.1.5.Y-VAL',
    '20.1.5.Y.Z-VAL',
    '20.1.X',
    '20.1.X-VAL',
    '20.1.X.Y',
    '20.1.X.Y-VAL',
    '20.1.X.Y.Z',
    '20.1.X.Y.Z-VAL',
    '20.2.X',
    '20.2.X-VAL',
    '20.2.X.Y',
    '20.2.X.Y-VAL',
    '20.2.X.Y.Z-VAL',
    '20.2.X.Y.Z-VAL2',
    '20.3.X',
    '20.3.X-VAL',
    '20.3.X.Y',
    '20.3.X.Y-VAL',
    '20.3.3.Y-VAL',
    '20.3.4.Y-VAL',
    '20.3.0.Y-VAL',
    '20.6.X',
    '20.6.X-VAL',
    '20.6.X.Y',
    '20.6.X.Y-VAL',
    '20.7.X',
    '20.7.X-VAL',
    '20.7.X.Y',
    '20.7.X.Y-VAL',
    '20.7.X.Y.Z-VAL',
    '20.7.1.Y-VAL',
    '20.7.2.Y-VAL',
    '20.7.3.Y',
    '20.7.3.Y-VAL',
    '20.7.3.Y.Z-VAL',
    '20.7.4.Y',
    '20.7.4.Y-VAL',
    '20.7.5.Y-VAL',
    '20.7.5.Y.Z-VAL',
    '20.7.6.Y-VAL',
    '20.7.8.Y-VAL',
    '20.7.9.Y-VAL',
    '20.99.Y-VAL',
    '20.11.X.Y-VAL',
    '20.11.X.Y.Z-VAL',
    '20.20.X',
    '20.20.X-VAL',
    '20.20.X.Y-VAL',
    '21.0.X',
    '21.0.X-VAL',
    '21.0.X.Y-VAL',
    '21.9.X-VAL',
    '22.0.X-master',
    '2.0.X',
    '2.1.X',
    '2.3.X',
    '2.4.X',
    'EXP',
    'mig0',
    'mig1',
    'mig2',
    'mig3',
    'mig4',
    'mig5',
    'mig6',
    'mig7',
    'mig8',
    'mig9',
    'mig10',
    'mig11',
    'mig12',
    'mig13',
    'mig14',
    'mig15',
    'devmig1',
    'devmig2',
    'devmig3',
    'devmig4',
    'devmig5',
    'devmig6',
    'lcg4']


_dbconnectionDir = '/afs/cern.ch/atlas/project/RTT/dbconnection/'
cooldozer = '/afs/cern.ch/atlas/project/RTT/prod/Results/dozer/api/latest'
db_location = {
    ('rtt', 'prod'): os.path.join(_dbconnectionDir, 'oracle.prod'),
    ('rttdev', 'dev'): os.path.join(_dbconnectionDir, 'oracle.dev')
}

branchdir_2017 = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'
