#DetDescrVersion="ATLAS-GEO-02-01-00" # Set your geometry here
#ConditionsTag="COMCOND-REPC-003-00" # Set the conditions tag here; not needed for AOD 

jetanalysis = True
zeeanalysis = False
PoolAODInput = []

# for the jet analysis
if jetanalysis:
    DetDescrVersion="ATLAS-GEO-02-01-00"
    datapath = '/afs/cern.ch/atlas/maxidisk/d36/topmix/user.RichardHawkings.0108175.topmix_Muon.AOD.v5'  # local
    nFiles = 1 # max 15
    for i in xrange(1,nFiles+1):
        PoolAODInput += ['%s/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._000%02i.pool.root' % (datapath, i) ]

# for the zee analysis
if zeeanalysis:
    DetDescrVersion="ATLAS-GEO-08-00-01"
    #datapath = 'valid1.105144.PythiaZee.recon.AOD.e380_s593_r824_tid094947'  # local
    datapath = '/afs/cern.ch/atlas/maxidisk/d36/valid1.105144.PythiaZee.recon.AOD.e380_s593_r824_tid094947'
    nFiles = 10 # max 10
    for i in xrange(1,nFiles+1):
        PoolAODInput += ['%s/AOD.094947._0000%02i.pool.root.1' % (datapath, i)]



doTrigger = False # Need to check
EvtMax=-1 # number of event to process

# include your algorithm job options here
UserAlgs = [ "TrigAnalysisExamples/TATAlg.py" ]

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
OutputLevel = INFO

# Read settings; for performance DPD set ESD to true
readRDO = False
readESD = False
readAOD = True

# Write settings; keep all of these to false.
# Control the writing of your own n-tuple in the alg's job options
doCBNT = False
doWriteESD = False
doWriteAOD = False
doWriteTAG = False

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")
