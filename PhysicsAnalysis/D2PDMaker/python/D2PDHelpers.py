# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        D2PDHelpers
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2009
##
## Description: Here, some helper functions are defined
##
##=============================================================================

# Import the flags for this package
from D2PDMaker.D2PDFlags import D2PDFlags


# This function correctly determines the name of the output file.
# If the transform set the FileName to the "official" name and locked
# the jobProperty, that one will be used. Otherwise the user sets it.
def buildFileName( jobPropFlagOrString ):
    fileName = ""
    if type(jobPropFlagOrString) == str :
        fileName = D2PDFlags.OutputDirectoryName()+D2PDFlags.OutputPoolRootFileNamePrefix()+jobPropFlagOrString+D2PDFlags.OutputMiddleName()+".pool.root"
        pass
    else :
        if jobPropFlagOrString.is_locked() :
            fileName = jobPropFlagOrString.FileName
            pass
        else :
            fileName = D2PDFlags.OutputDirectoryName()+D2PDFlags.OutputPoolRootFileNamePrefix()+jobPropFlagOrString.StreamName+D2PDFlags.OutputMiddleName()+".pool.root"
            pass
        pass
    return fileName
