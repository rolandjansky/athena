# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# SkimNTUP_ProdFlags.py for NTUP->NTUP
#

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

listAllKnownSkimNTUP=[]


class SkimNTUP_ProdFlags (JobPropertyContainer):
    """ The SkimNTUP_ProdFlags flag/job property  container."""
jobproperties.add_Container(SkimNTUP_ProdFlags)

prodFlags = jobproperties.SkimNTUP_ProdFlags

# This function correctly determines the name of the output file.
# If the transform set the FileName to the "official" name and locked
# the jobProperty, that one will be used. Otherwise the user sets it.
def buildFileName( jobPropFlag ):
    if jobPropFlag.is_locked() :
        fileName = jobPropFlag.FileName
    else :
        fileName = (prodFlags.OutputDirectoryName() +
                    prodFlags.OutputPoolRootFileNamePrefix() +
                    jobPropFlag.StreamName +
                    prodFlags.OutputMiddleName() +
                    ".root")

    return fileName

def _importFlagsFromModule (mod):
    """Given the full name of a module MOD, import it.
Then look for objects in the module with names like Write*NTUP that
derive from JobProperty and add them to our list."""
    name = mod.rsplit('.', 1)[-1]
    mod = __import__ (mod, fromlist = [name])
    for k, v in mod.__dict__.items():
        if (k.startswith ('Write') and k.endswith ('NTUP') and
            issubclass (v, JobProperty)):
            jobproperties.SkimNTUP_ProdFlags.add_JobProperty (v)
            listAllKnownSkimNTUP.append (getattr (prodFlags, k))
    return

class TreeName(JobProperty):
    """ Tree name """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
prodFlags.add_JobProperty(TreeName)

from RecExConfig.RecoFunctions import AddValidItemToList,RemoveValidItemFromList
class SkimNTUPScripts(JobProperty):
    """ Setting SkimNTUPScripts+=['MyPackage/NTUPOptions.py'] will execute NTUPOptions.py
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
    def append(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            AddValidItemToList(item,self.StoredValue)
        return
    def remove(self,item):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            RemoveValidItemFromList(item,self.StoredValue)
        return
prodFlags.add_JobProperty(SkimNTUPScripts)

class WriteMySkimNTUP (JobProperty):
    """skimming NTUP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_MYSKIMNTUP'
    FileName = ''
    isVirtual = False
    SkimNTUPScript = "NTUPtoNTUPExample/MySkimNTUP_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']
prodFlags.add_JobProperty (WriteMySkimNTUP)
listAllKnownSkimNTUP.append (prodFlags.WriteMySkimNTUP)

class WriteMySkimNTUP2 (JobProperty):
    """skimming NTUP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_MYSKIMNTUP2'
    FileName = ''
    isVirtual = False
    SkimNTUPScript = "NTUPtoNTUPExample/MySkimNTUP2_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']
prodFlags.add_JobProperty (WriteMySkimNTUP2)
listAllKnownSkimNTUP.append (prodFlags.WriteMySkimNTUP2)


# Egamma D3PDs
_importFlagsFromModule ('EgammaN2N.EgammaN2NProdFlags')

# Exotics D3PDs
_importFlagsFromModule ('ExoticsN2N.ExoticsN2NProdFlags')

# Jet D3PDs
_importFlagsFromModule ('JetN2N.JetN2NProdFlags')

print 'Known NTUPs are', listAllKnownSkimNTUP
