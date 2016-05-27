#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Temporary support for old ESDtoDPD workflow - should update to RecJobTransforms version
# in next release... (Captain's Log, Stardate 2016-04-08)

from RecExConfig.RecoFunctions import AddValidItemToList

def SetupOutputDPDs(runArgs,flagContainerList):
    DPDMakerScripts=[]
    for flagContainer in flagContainerList:
        for flagName in flagContainer.__dict__.keys():
            flag=getattr(flagContainer,flagName)
            if hasattr(flag,"StreamName"):
                dpdName=flag.StreamName.lstrip("Stream")
                argName='output'+dpdName+'File'
                if hasattr(runArgs,argName):
                    if hasattr(flag,"FileName"):
                        flag.FileName=getattr(runArgs,argName)
                        flag.set_Value_and_Lock( True )
                        if hasattr(flag,"DPDMakerScript"):
                            AddValidItemToList(flag.DPDMakerScript,DPDMakerScripts)
                        elif hasattr (flag, 'configure'):
                            DPDMakerScripts.append(flag.configure)
                        else:
                            raise RuntimeError("No athena script attached to argument '%s'"%argName)
                    else:
                        raise RuntimeError("%s is not conform with the job transform convention, hence unusable. Please fix this."%dpdName)

    return DPDMakerScripts

# These functions are still needed for old Run1 NTUP infrastructure
def SetupDPDIncludes(runArgs,flagContainerList,includeType):
    """This function can be used to include all the pre-include scripts
       defined for the requested DPD types.

       Returns a list of output types in which a pre/post-include was used."""
    # We must not produce multiple output types when pre/post-include
    # scripts are involved internally. This variable keeps track of
    # the output types which have used the pre/postInclude functionality
    includeTypeList = []
    # Make a logger for the function:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "SetupDPDIncludes" )
    # Loop over all DPD job properties:
    for flagContainer in flagContainerList:
        for flagName in flagContainer.__dict__.keys():
            flag=getattr(flagContainer,flagName)
            # Check if this DPD type has been requested in the job:
            if hasattr(flag,"StreamName"):
                dpdName=flag.StreamName.lstrip("Stream")
                argName='output'+dpdName+'File'
                if hasattr(runArgs,argName):
                    # We found a match for an output, however if there was already a script 
                    # included, then we may be in trouble (note that we print an error even when
                    # the current output type does not itself have an include, because the include 
                    # that already exists can even mess up this new output type).
                    if len(includeTypeList) > 0:
                        logger.error( "Multiple output DPD types requested with "
                                      "pre/post-includes present: {0} after includes from {1}".format(argName, includeTypeList) )
                        logger.error( "This will most probably lead to bad output" )
                        pass
                    # Only some DPD types define pre/post-include scripts.
                    # It's okay if this one doesn't define any,
                    if not hasattr(flag,includeType): 
                        continue
                    includeTypeList.append(argName)
                    # Make sure that we have a list of scripts, not just one:
                    includes = getattr(flag,includeType)
                    if type(includes) != list:
                        includes = [includes]
                        pass
                    # Now include all the specified scripts:
                    for incfile in includes:
                        logger.warning( "Including script: {0} for {1}".format(incfile, argName) )
                        from AthenaCommon.Include import include
                        include( incfile )
                        pass
                    pass
                pass
            pass
        pass
    # Tell the caller which types used pre/postIncludes:
    return includeTypeList

def SetupDPDPreIncludes(runArgs,flagContainerList):
    # Let the generic function do the work:
    return SetupDPDIncludes(runArgs,flagContainerList,"PreIncludeScripts")

def SetupDPDPostIncludes(runArgs,flagContainerList):
    # Let the generic function do the work:
    return SetupDPDIncludes(runArgs,flagContainerList,"PostIncludeScripts")
