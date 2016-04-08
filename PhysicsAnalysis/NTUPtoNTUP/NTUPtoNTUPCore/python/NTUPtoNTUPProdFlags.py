# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# NTUPtoNTUPProdFlags.py for NTUP->NTUP
#

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

listAllKnownNTUPtoNTUP=[]


class NTUPtoNTUPProdFlags (JobPropertyContainer):
    """ The NTUPtoNTUPProdFlags flag/job property  container."""
jobproperties.add_Container(NTUPtoNTUPProdFlags)

prodFlags = jobproperties.NTUPtoNTUPProdFlags

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

# Allows prodflags from elsewhere to be defined 
def _importFlagsFromModule (mod):
    """Given the full name of a module MOD, import it.
Then look for objects in the module with names like WriteNTUP_RED that
derive from JobProperty and add them to our list."""
    name = mod.rsplit('.', 1)[-1]
    mod = __import__ (mod, fromlist = [name])
    for k, v in mod.__dict__.items():
        if (k.startswith ('WriteNTUP_RED') and
            issubclass (v, JobProperty)):
            if hasattr (v, 'NTUPScript') :
                prodFlags.add_JobProperty (v)
                listAllKnownNTUPtoNTUP.append (getattr (prodFlags, k))
    return
   
class TreeName(JobProperty):
    """ Tree name """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
prodFlags.add_JobProperty(TreeName)

from RecExConfig.RecoFunctions import AddValidItemToList,RemoveValidItemFromList
class NTUPScripts(JobProperty):
    """ Setting NTUPScripts+=['MyPackage/NTUPOptions.py'] will execute NTUPOptions.py
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
prodFlags.add_JobProperty(NTUPScripts)

class WriteMyNTUP (JobProperty):
    """skimming NTUP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_MYNTUP'
    FileName = ''
    isVirtual = False
    NTUPScript = "NTUPtoNTUPExample/MyNTUP_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']
prodFlags.add_JobProperty (WriteMyNTUP)
listAllKnownNTUPtoNTUP.append (prodFlags.WriteMyNTUP)

class WriteMyNTUP2 (JobProperty):
    """test NTUP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamNTUP_MYNTUP2'
    FileName = ''
    isVirtual = False
    NTUPScript = "NTUPtoNTUPExample/MyNTUP2_prodJobOFragment.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']
prodFlags.add_JobProperty (WriteMyNTUP2)
listAllKnownNTUPtoNTUP.append (prodFlags.WriteMyNTUP2)

################################################################
# Following block of stream names for the Derivation Framework #

_importFlagsFromModule('DerivationFrameworkCore.DerivationFrameworkProdFlags')

# End of Derivation Framework formats #
#######################################
