# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# $Id$
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


class WritePhysicsD3PD (JobProperty):
    """Produce the physics D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_PHYSICS'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "D3PDMakerConfig/PhysicsD3PD_prodJobOFragment.py"
    TreeNames = ['physics', 'physicsTrigDec']
    SubSteps = ['a2d', 'e2d']
jobproperties.D3PDProdFlags.add_JobProperty (WritePhysicsD3PD)
listAllKnownD3PD.append (prodFlags.WritePhysicsD3PD)


class WriteLongLivedParticleD3PD (JobProperty):
     """Produce the Long Lived Particle D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_SUSYLLP'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "LongLivedParticleD3PDMaker/LongLivedParticleD3PD_prodJob\
OFragment.py"
     TreeNames = ['llp']
     SubSteps = ['a2d', 'e2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteLongLivedParticleD3PD)
listAllKnownD3PD.append (prodFlags.WriteLongLivedParticleD3PD)


class WriteTopBoostJSD3PD (JobProperty):
     """Produce the BoostedTop D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_TOPBOOST'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "BoostedTopD3PDMaker/TopBoostJSD3PD_prodJobOFragment.py"
     TreeNames = ['physics']
     SubSteps = ['a2d', 'e2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteTopBoostJSD3PD)
listAllKnownD3PD.append (prodFlags.WriteTopBoostJSD3PD)




class WriteBackgroundD3PD (JobProperty):
    """Produce the Background D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_BKGD'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "BackgroundD3PDMaker/BackgroundD3PD_prodJobOFragment.py"
    TreeNames = ['background']
    SubSteps = ['e2d', 'a2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteBackgroundD3PD)
listAllKnownD3PD.append (prodFlags.WriteBackgroundD3PD)


#######################################################
#
#                     Trigger D3PDs

class WriteTriggerD3PD (JobProperty):
    """Produce the Trigger D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TRIG'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TriggerD3PDMaker/TriggerD3PD_prodJobOFragment.py"
    TreeNames = ['trigger']
    SubSteps = ['e2d', 'a2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteTriggerD3PD)
listAllKnownD3PD.append (prodFlags.WriteTriggerD3PD)


class WriteTrigMuonD3PD (JobProperty):
    """Produce the TrigMuon D3PD."""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_TRIGMU'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "TrigMuonD3PDMaker/TrigMuonD3PD_prodJobOFragment.py"
    TreeNames = ['trigger']
    SubSteps = ['e2d', 'a2d']
jobproperties.D3PDProdFlags.add_JobProperty (WriteTrigMuonD3PD)
listAllKnownD3PD.append (prodFlags.WriteTrigMuonD3PD)


#
#
#######################################################


# Higgs D3PDs
_importFlagsFromModule ('HSG2DPDUtils.HSG2D3PDProdFlags')
#_importFlagsFromModule ('HSG5DPDUtils.HSG5D3PDProdFlags')

# L1Calo D3PDs
#_importFlagsFromModule ('L1CaloD3PDMaker.L1CaloD3PDProdFlags')

# Top D3PDs
_importFlagsFromModule ('TopInputsD3PDMaker.TopInputsD3PDProdFlags', True)

# Standard Model D3PDs
_importFlagsFromModule ('PhysicsD3PDMaker.PhysicsD3PDProdFlags')

# Jet tag D3PDs
_importFlagsFromModule ('JetTagD3PDMaker.JetTagD3PDProdFlags')

# SUSY D3PDs
_importFlagsFromModule ('SUSYD3PDMaker.SUSYD3PDProdFlags', True)

# Tracking D3PDs.
_importFlagsFromModule ('InDetD3PDMaker.VertexD3PDAnalysisProdFlags', True)

# egamma D3PDs.
_importFlagsFromModule ('egammaD3PDMaker.egammaD3PDProdFlags')

# Heavy ion D3PDs.
_importFlagsFromModule ('HeavyIonD3PDMaker.HeavyIonD3PDProdFlags', True)

# Trigger D3PDs.
#_importFlagsFromModule ('TrigBJetD3PDMaker.TrigBJetD3PDProdFlags')

