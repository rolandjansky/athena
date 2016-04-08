###############################################################
#
# Skeleton top job options for SkimNTUP_trf
#
#==============================================================

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('ntup_to_ntup')
recoLog.info( '****************** STARTING NTUP->NTUP MAKING *****************' )

## max/skip events
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
else: athenaCommonFlags.SkipEvents=0
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
else: athenaCommonFlags.EvtMax=-1

## TreeName
from NTUPtoNTUPCore.SkimNTUP_ProdFlags import prodFlags
if hasattr(runArgs,"tree_name"): prodFlags.TreeName.set_Value_and_Lock( runArgs.tree_name )
else:
    raise TransformArgumentError(message='tree_name is not defined!')

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
        print "Using argument ", arg, " = ",inFile, ", tree name = ", prodFlags.TreeName()
if inFileArgs!=1:
    raise TransformArgumentError(message='Wrong number of inputNTUPXXXFile arguments: {0:d} instead of 1. Stopping!'.format(inFileArgs))

## Automatically turn ON/OFF and set output file name of each possible NTUP
listOfFlags=[]
try:
    listOfFlags.append( prodFlags )
except ImportError:
    print "WARNING SkimNTUPProdFlags.py is not available. Only OK if you're using job transforms without the AtlasAnalysis project."

from NTUPtoNTUPCore.NTUPUtils import SetupOutputSkimNTUPs
from AthenaCommon.JobProperties import jobproperties
prodFlags.SkimNTUPScripts.append(SetupOutputSkimNTUPs(runArgs,listOfFlags))

from PATJobTransforms.OutputsMgr import outputsMgr,magicKey
for key in dir(runArgs):
    if key.startswith(magicKey):
        outputsMgr.addOutput(key,getattr(runArgs,key))

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "NTUPtoNTUPCore/SkimNTUP_topOptions.py" )

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)

