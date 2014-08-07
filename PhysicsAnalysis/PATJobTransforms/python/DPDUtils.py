#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (DESY), 1st July 2008
# Usage: Utility functions for DPD transforms

from PATJobTransforms.ArgDicTools import SysArgsExceptionCatcher
from RecExConfig.RecoFunctions import AddValidItemToList

def outputBSNameStripper(fullName):
    if not fullName.endswith('._0001.data'):
        raise RuntimeError("Illegal output bytestream name '%s'. The name must ends with '._0001.data'."%fullName)
    
    strippedName=fullName.strip('._0001.data')
    return strippedName

# This function interprets sysArgs and returns the number of output DPD
def nDPDGetter(sysArgs,key,defaultPosition):
    #First look for known exceptions
    if SysArgsExceptionCatcher(sysArgs) is "Help":
        return 0

    #Then look for the given key (e.g. nbOutputDPD= )
    for ar in sysArgs:
        if isinstance(ar,str) and ar.startswith(key):
            return int(ar[len(key):])
        
    #If key not found, use the defaultPosition argument
    try:
        nDPD=int(sysArgs[defaultPosition])
    except:
        raise RuntimeError("Unable to find nDPD with default key '%s' or default position "%key+str(defaultPosition))

    return nDPD


#---------------------------------------------------------
# This function interprets sysArgs and returns the input file name according to key or defaultPosition
def inputFileGetter(sysArgs,key,defaultPosition):
    #First look for known exceptions
    if SysArgsExceptionCatcher(sysArgs) is "Help":
        return "dummyFileName"

    #Then look for the given key (e.g. inputBSFile=, inputESDFile= , inputAODFile=)
    for ar in sysArgs:
        if isinstance(ar,str) and ar.startswith(key):
            return ar[len(key):]

    #If key not found, use the defaultPosition argument
    fileName=sysArgs[defaultPosition]
    if not isinstance(fileName,str):
        raise RuntimeError("Unable to find input file with arg key '%s' or default position "%key+str(defaultPosition))

    return fileName

#---------------------------------------------------------
# This function returns a dictionary with DPD configuration from the inputESD name.
# The DPD configuration is taken from PrimaryDPDMaker.
try:
    from PrimaryDPDMaker.JobTransformConfiguration import *
except ImportError:
    print "WARNING PrimaryDPDMaker not available. Only OK if you're using job transforms without the AtlasAnalysis project."

def DPDConfigFromInputFile(inputFile):
    if inputFile.endswith("ESD.TCT.pool.root") or inputFile.endswith("AOD.TCT.pool.root"):
        print "Configuring DPD for TCT...\n"
        return TCTConfiguration()
    
    elif inputFile.find(".physics.Express.") >= 0:
        print "Configuring DPD for physics Express stream...\n"
        return TCTConfiguration()
    
    elif inputFile.find(".physics_Egamma.") >= 0 or inputFile.find(".physics.Egamma.") >= 0 :
        print "Configuring DPD for EGamma trigger stream...\n"
        return EGammaStreamConfiguration()

    elif inputFile.find(".physics_Muon.") >= 0 or inputFile.find(".physics.Muon.") >= 0:
        print "Configuring DPD for Muon trigger stream...\n"
        return MuonStreamConfiguration()

    elif inputFile.find(".physics_Jet.") >= 0 or inputFile.find(".physics.Jet.") >= 0:
        print "Configuring DPD for Jet trigger stream...\n"
        return JetStreamConfiguration()

    elif inputFile.find(".physics_Minbias.") >= 0 or inputFile.find(".physics.Minbias.") >= 0:
        print "Configuring DPD for MinBias trigger stream...\n"
        return MinBiasConfiguration()

    #elif inputFile.find(".physics_Bphys.") >= 0 or inputFile.find(".physics.Bphys.") >= 0:
    #    print "Configuring DPD for Bphys trigger stream...\n"
    #    return BphysConfiguration()

    else:
        print "WARNING no DPD configuration known for input file named %s"%inputFile
        print "***** No DPD will be produced in this job *****\n"
        conf={}
        conf['nDPD']=0
        return conf
        #raise RuntimeError("Unable to setup DPD from unknown input file name %s"%inputFile)
    return -1


#-------------------------------------------
# This function interprets sysArgs and returns the Tier-0 DPD names according to key 
def GetDPDNamesAndRemoveFromSysArgs(sysArgs,key,nPotentialDPD,conf):

    nDPD=conf['nDPD']
    if nDPD>0:
        T0ArgPositions = conf['T0ArgPositions']
        DefaultDPDNames = conf['DefaultDPDNames']

    #First get *potential* DPD names from the command line and remove them from sys.argv
    #Example: DPDName1=MainEGamma.pool.root DPDName4=SingleElectron.pool.root DPDName7=SinglePhoton.pool.root 
    T0Names=[]
    T0Positions=[]
    i=1    
    while i<=nPotentialDPD:
        defPos=0 # positional argument not yet supported
        try:
            tmp=T0ArgPositions.index(i-1)
            msgSuffix="(DPD #%i)"%tmp
        except:
            msgSuffix="(potential DPD ignored)"

        iKey=key+str(i)+"="
        tmpT0Name=inputFileGetter(sysArgs,iKey,defPos)
        zeArg=iKey+tmpT0Name
        try:
            sysArgs.remove(zeArg)
            print "Removed %s from sys.argv. Will use this name instead of default."%zeArg,msgSuffix
            T0Names.append( tmpT0Name )
            T0Positions.append( i-1 )
        except:
            print "%s not specified. Will use default name."%iKey,msgSuffix
        
        i+=1

    #Now get final DPD names, either from T0Names or DefaultNames
    nDef=0
    nT0=0
    DPDNames=[]
    for i in xrange(nDPD):
        try:
            t0Ind=T0Positions.index(T0ArgPositions[i])
            DPDNames.append(T0Names[t0Ind])
            nT0+=1
        except:
            DPDNames.append(DefaultDPDNames[i])
            nDef+=1

    #final coherence check...
    if nDef>0 and nT0==0:
        print "Auto-configuration completely from default DPD names."
    elif nDef==0 and nT0>0: 
        print "File names completely specified by DPDName1 ... DPDName%i."%nPotentialDPD
    elif nDef>0 and nT0>0:
        print "WARNING job configured with a mixture of default and T0 names... is it really what you want?"
    if nDPD != (nDef+nT0):
        raise RuntimeError("Error in auto-configuration: nDPD !=(nDef+nT0)")
    
    return DPDNames

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
