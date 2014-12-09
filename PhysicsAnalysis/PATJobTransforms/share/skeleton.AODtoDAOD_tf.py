# Skeleton file for AOD to DAOD (Reduction framework) job
#
# $Id: skeleton.AODtoDAOD_tf.py 632134 2014-11-29 12:28:25Z graemes $
#
from AthenaCommon.Logging import logging
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
msg = logging.getLogger('AODtoDAOD')
msg.info( '****************** STARTING AOD->DAOD MAKING *****************' )

if hasattr(runArgs, "reductionConf"):
    msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
else:
    msg.error('AOD Reduction job started, but with no "reductionConf" array - aborting')
    raise RuntimeError, "No reductions configured"

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")


if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    rec.doDPD.set_Value_and_Lock(True)
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
elif hasattr(runArgs,'inputEVNTFile') or hasattr(runArgs,'jobConfig'):
    # Assume that we're running from EVNT or straight through evgen
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    rec.AutoConfiguration.set_Value_and_Lock(['ProjectName','BeamType','RealOrSim','DoTruth','InputType'])
    rec.doInDet.set_Value_and_Lock(False)
    rec.doCalo.set_Value_and_Lock(False)
    rec.doMuon.set_Value_and_Lock(False)
    rec.doForwardDet.set_Value_and_Lock(False)
    rec.doFileMetaData.set_Value_and_Lock(False)
    rec.doTruth.set_Value_and_Lock( True )
    from AthenaCommon.DetFlags      import DetFlags
    DetFlags.detdescr.BField_setOff()
    athenaCommonFlags.FilesInput = runArgs.inputEVNTFile
    # Leave the remainder for the internal setup
else:
    msg.error('AOD Reduction job started, but with no AOD inputs - aborting')
    raise RuntimeError, "No AOD input"

listOfFlags=[]

try:
    from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
    listOfFlags.append(derivationFlags)
except ImportError:
    print "WARNING DerivationFrameworkProdFlags not available."  

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

passThroughMode = False
if hasattr(runArgs,"passThrough"):
    passThroughMode = runArgs.passThrough

if (passThroughMode==True):
    msg.warning("Pass through mode is ON: decision of derivation kernels will be IGNORED!")
    rec.doDPD.passThroughMode = True

## Pre-exec
if hasattr(runArgs,"preExec"):
    msg.info("transform pre-exec")
    for cmd in runArgs.preExec:
        msg.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )

# Intervene and strip algs from streams if pass through mode requested
if rec.doDPD.passThroughMode:
    for stream in MSMgr.StreamList:
        evtStream = stream.GetEventStream()
        evtStream.AcceptAlgs = []
        evtStream.RequireAlgs = []
        evtStream.VetoAlgs = []
    msgFresh = logging.getLogger('AODtoDAOD')
    msgFresh.info( 'Pass through mode was requested. Stream definitions have been revised. New stream definitions follow')
    MSMgr.Print()


## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)


## Post-exec
if hasattr(runArgs,"postExec"):
    msg.info("transform post-exec")
    for cmd in runArgs.postExec:
        msg.info(cmd)
        exec(cmd)

