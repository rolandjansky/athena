# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from Tools2                  import getFileOrDirDateStamp
from RTTSException           import RTTCodingError

#----------------------------------------------------
class InValidNICOSDateError(Exception): pass
def getReleaseDate(pathNameBuilder):
    """For a given release, get the stamp date from the directory on AFS,
    or, for nightlies, from the link date if it is today's release."""
    
    fileWithDate = str(pathNameBuilder.pathToFindReleaseDate())
    date = getFileOrDirDateStamp(fileWithDate)
    if not date.isValid():
        m = 'InValidNICOSDateError %s' % str(date)
        raise RTTCodingError(m)
    return date
