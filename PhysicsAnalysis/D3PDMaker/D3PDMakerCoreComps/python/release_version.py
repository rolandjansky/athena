# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: release_version.py 485961 2012-03-01 14:29:38Z krasznaa $
#
# @file D3PDMakerCoreComps/python/release_version.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief Try to determine the release version for the current input file.
#


def release_version():
    """Return the release version of the current input file.
The version is returned as a list of integers; i.e., [15, 5, 3].
None is returned if no version string is present in the input file.
An empty list is returned if we don't recognize the format
of the version string.  We return [99, 0] if we're not reading
a pool file (eg, raw).
"""

    from RecExConfig.InputFilePeeker import inputFileSummary
    try:
        if inputFileSummary['file_type'] != 'pool':
            return [99, 0]
        release = inputFileSummary['metadata']['/TagInfo']['AtlasRelease']
    except KeyError:
        return None

    rlist = release.split ('-')
    if len(rlist) < 2:
        return []

    try:
        rlist = [int(x) for x in rlist[1].split ('.')]
    except ValueError:
        return []

    if len(rlist) < 2:
        return []

    return rlist


def _version_compare (rlist, s):
    """Helper for at_least_version.

>>> _version_compare ([1,2,3], "1.2.3")
True
>>> _version_compare ([1,2,3], "1.2.4")
False
>>> _version_compare ([1,2,3], "1.2.2")
True
>>> _version_compare (None, "1.2.3")
False
>>> _version_compare ([], "1.2.3")
True
>>> _version_compare ([1,2,3,4], "1.2.3")
True
>>> _version_compare ([1,2], "1.2.3")
False
"""
    slist = [int(x) for x in s.split ('.')]

    if rlist == None:
        return False
    if rlist == []:
        return True

    while len(slist) > 0 and len(rlist) > 0 and slist[0] == rlist[0]:
        del slist[0]
        del rlist[0]

    if len(slist) == 0 and len(rlist) == 0:
        return True
    if len(slist) > 0 and len(rlist) > 0:
        return rlist[0] >= slist[0]
    if len(slist) > 0:
        return False
    return True
    

def at_least_version (s):
    """Test to see if the version of the input file is at least S.

S is interpreted as a version number, like 16.6.3.
Test to see if the version recorded in the input file is at least this.
Special cases: If no version number is recorded, we return False
(perhaps the file was processed before version numbers were stored).
If we don't recognize the version format, we return True (could be
from a local test build).
"""
    return _version_compare (release_version(), s)
    
    
if __name__ == "__main__":
    import doctest
    doctest.testmod()
    
