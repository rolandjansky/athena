from __future__ import print_function

###############################################################
#
# Skeleton top job options for NTUP->NTUP
#
#
#==============================================================

from AthenaCommon.Logging import logging
msg = logging.getLogger('NTUP Reduction')
msg.info( '****************** STARTING NTUP Reduction *****************' )

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

## Automatically turn ON/OFF and set output file name of each possible DPD
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")
try:
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    listOfFlags.append( prodFlags )
except ImportError:
    print("WARNING D3PDProdFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))


if hasattr(runArgs, "reductionConf"):
    msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
else:
    msg.error('NTUP Reduction job started, but with no "reductionConf" array - aborting')
    raise RuntimeError("No reductions configured")

## max/skip events
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
else: athenaCommonFlags.SkipEvents=0
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
else: athenaCommonFlags.EvtMax=-1

## TreeName
from NTUPtoNTUPCore.NTUPtoNTUPProdFlags import prodFlags
if hasattr(runArgs,"tree_name"):
    prodFlags.TreeName.set_Value_and_Lock( runArgs.tree_name )
else:
    prodFlags.TreeName.set_Value_and_Lock( 'physics' )

## Input
from PATJobTransforms.Configuration import ConfigDic
from PyJobTransformsCore.trferr import TransformArgumentError
from PyJobTransformsCore.full_trfarg import InputNtupleFileArg
inFileArgs=0
for arg in dir(runArgs):
    if arg.startswith('inputNTUP') and arg.endswith('File'):
        inFile = getattr(runArgs,arg)
        athenaCommonFlags.FilesInput.set_Value_and_Lock(inFile)
        inFileArgs+=1
        print("Using argument ", arg, " = ",inFile, ", tree name = ", prodFlags.TreeName())
if inFileArgs!=1:
    raise TransformArgumentError(message='Wrong number of inputNTUPXXXFile arguments: {0:d} instead of 1. Stopping!'.format(inFileArgs))

## Automatically turn ON/OFF and set output file name of each possible NTUP
listOfFlags=[]
try:
    listOfFlags.append( prodFlags )
except ImportError:
    print("WARNING NTUPtoNTUPProdFlags.py is not available. Only OK if you're using job transforms without the AtlasAnalysis project.")

from NTUPtoNTUPCore.NTUPUtils import SetupOutputNTUPs
from AthenaCommon.JobProperties import jobproperties
prodFlags.NTUPScripts.append(SetupOutputNTUPs(runArgs,listOfFlags))

from PATJobTransforms.OutputsMgr import outputsMgr,magicKey
for key in dir(runArgs):
    if key.startswith(magicKey):
        outputsMgr.addOutput(key,getattr(runArgs,key))

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
else: include( "NTUPtoNTUPCore/NTUPtoNTUP_topOptions.py" )

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

