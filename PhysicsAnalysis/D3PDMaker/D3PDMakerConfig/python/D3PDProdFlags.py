# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# @file D3PDMakerConfig/python/D3PDProdFlags.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief Define flags and configuration settings used when making D3PDs
#        from production transforms.
#

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

"""Define flags and configuration settings used when making D3PDs
from production transforms.
"""


listAllKnownD3PD=[]


class D3PDProdFlags (JobPropertyContainer):
    """ The D3PDProdFlags flag/job property  container."""
jobproperties.add_Container(D3PDProdFlags)

class OldD3PDProdFlags( JobPropertyContainer ):
    """ The old style flag/job property container."""
jobproperties.add_Container( OldD3PDProdFlags )

prodFlags = jobproperties.D3PDProdFlags
oldProdFlags = jobproperties.OldD3PDProdFlags


def _importFlagsFromModule (mod, catchFail = False):
    """Given the full name of a module MOD, import it.
Then look for objects in the module with names like Write*D3PD that
derive from JobProperty and add them to our list."""
    name = mod.rsplit('.', 1)[-1]
    if catchFail:
        try:
            mod = __import__ (mod, fromlist = [name])
        except ImportError:
            return
    else:
        mod = __import__ (mod, fromlist = [name])
    for k, v in mod.__dict__.items():
        if (k.startswith ('Write') and k.endswith ('D3PD') and
            issubclass (v, JobProperty)):
            if hasattr (v, 'DPDMakerScript') :
                jobproperties.D3PDProdFlags.add_JobProperty (v)
                listAllKnownD3PD.append (getattr (prodFlags, k))
            else:
                jobproperties.OldD3PDProdFlags.add_JobProperty (v)
                listAllKnownD3PD.append (getattr (oldProdFlags, k))
    return
    


class OutputDirectoryName(JobProperty):
    """ Name of the directory where the output files are written. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D3PDProdFlags.add_JobProperty(OutputDirectoryName)

class OutputMiddleName(JobProperty):
    """ Keyword added in the middle of the output files names. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D3PDProdFlags.add_JobProperty(OutputMiddleName)

class OutputPoolRootFileNamePrefix(JobProperty):
    """ File name prefix for the pool.root output files."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D3PDProdFlags.add_JobProperty(OutputPoolRootFileNamePrefix)


class WriteMuonD3PD (JobProperty):
    """Produce the MCP/muon D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_MUON'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "MuonD3PDMaker/MuonD3PD_prodJobOFragment.py"
    TreeNames = ['muon']
    SubSteps = ['a2d', 'e2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteMuonD3PD)
listAllKnownD3PD.append (prodFlags.WriteMuonD3PD)

# egamma D3PDs.
_importFlagsFromModule ('egammaD3PDMaker.egammaD3PDProdFlags')
