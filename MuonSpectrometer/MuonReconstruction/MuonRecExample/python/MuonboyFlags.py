# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file MuonboyFlags.py Flags for configuring the %Muonboy muon reconstruction algorithm

## @namespace python::MuonboyFlags @copydoc MuonboyFlags.py



from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.Logging import logging
from MuonRecExample.MuonRecUtils import fillJobPropertyContainer

# message logger
log = logging.getLogger( __name__ )


## if True, TGC and CSC are not used in the reconstruction
class BarrelOnly(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    
    ## Check consistency of flags
    def _do_action(self):
        _check_flags_consistency()


## if True, segments and tracks reconstruction are switched off
class DumpOnly(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

    ## Check consistency of flags
    def _do_action(self):
        _check_flags_consistency()

    
## if True add 2 TopAlg, MboyChecks and MboySegmentCheck, doing dumps
class AllChecks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

    ## check constistency of flags
    def _do_action(self):
        _check_flags_consistency()

## stop reconstruction at segment level
class doSegmentsOnly(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    



## Container with flags to steer Muonboy
class Muonboy(JobPropertyContainer):

    def setDefaults(self):
        pass


def _check_flags_consistency():
    global muonboyFlags
    # check consistency between DumpOnly and AllChecks
    if muonboyFlags.DumpOnly() and muonboyFlags.AllChecks():
        log.warning("Setting AllChecks to False because DumpOnly is True")
        muonboyFlags.AllChecks = False



# add muonboy container
jobproperties.add_Container(Muonboy)

## shortcut to access flags
muonboyFlags = jobproperties.Muonboy

# add all properties in this module
fillJobPropertyContainer(muonboyFlags,__name__)

#
# Backwards compatibility
#
oldtonew = { 'doMuonboyBarrelOnly' : 'muonboyFlags.BarrelOnly' ,
             'doMuonboyDumpOnly'   : 'muonboyFlags.DumpOnly'   ,
             'doMuonboyAllChecks'  : 'muonboyFlags.AllChecks'  }
import __main__
varlist = vars(__main__)
for oldflag,newflag in oldtonew.items():
    try:
        oldValue = varlist[oldflag]
    except KeyError:
        pass
    else:
        # set new flag to old flag value
        exec newflag + ' = %r' % oldValue
        # print a deprecated warning
        log.warning( '%s flag is deprecated. Use %s instead (%s)',
                     oldflag, newflag,
                     'from ' + __name__ + ' import muonboyFlags' )
