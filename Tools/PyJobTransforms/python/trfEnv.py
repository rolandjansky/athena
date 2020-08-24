from future.utils import iteritems

from builtins import object
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @Package PyJobTransforms.trfEnv
#  @brief Support for environemnt variable manipulation in the transforms
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfEnv.py 726697 2016-02-28 16:52:54Z uworlika $

import os
import os.path as path

import logging
msg = logging.getLogger(__name__)

## @brief Class holding the update to an environment that will be passed on to 
#  an executor
class environmentUpdate(object):

    def __init__(self):
        self._envdict = {}
        
    ## @brief Setup the standard execution environment according to the 
    #  switches in the transform configuration
    def setStandardEnvironment(self, argdict, name='all', substep=''):
        self.probeIMFSettings(argdict, name=name, substep=substep)
        self.probeTcmallocSettings(argdict, name=name, substep=substep)
        self.probeOtherSettings(argdict, name=name, substep=substep)
        
    ## @brief Add the libimf maths library to the setup
    def probeIMFSettings(self, argdict, name='all', substep=''):
        # If imf=True/False then follow its lead, but otherwise try to detect the release
        # and enable if we have a release >= 17.7
        if 'imf' in argdict:
            if argdict['imf'].returnMyValue(name=name, substep=substep) is False:
                msg.info('Skipping inclusion of imf libraries: --imf is set to False')
            else:
                msg.info('Enabling inclusion of imf libraries: --imf is set to True')
                self._addIMFSettings()
            return

        self._addIMFSettings()


    def _addIMFSettings(self):
        if 'ATLASMKLLIBDIR_PRELOAD' not in os.environ:
                msg.warning('"ATLASMKLLIBDIR_PRELOAD" not found in the current environment'
                            ' - no setup of MKL is possible')
                return
        
        if 'ATLASMKLLIBDIR_PRELOAD' in os.environ:
            if "LD_PRELOAD" not in self._envdict:
                self._envdict["LD_PRELOAD"] = pathVar("LD_PRELOAD")
            self._envdict["LD_PRELOAD"].add(path.join(os.environ['ATLASMKLLIBDIR_PRELOAD'], "libimf.so"))
            self._envdict["LD_PRELOAD"].add(path.join(os.environ['ATLASMKLLIBDIR_PRELOAD'], "libintlc.so.5"))


    ## @brief Add TCMALLOC to the setup
    def probeTcmallocSettings(self, argdict, name='all', substep=''):
        # We only do this if  tcmalloc=True in atgdict
        if 'tcmalloc' not in argdict or argdict['tcmalloc'].returnMyValue(name=name, substep=substep) is False:
            msg.info('Skipping inclusion of tcmalloc')
            return
    
        if 'TCMALLOCDIR' not in os.environ:
            msg.warning('"TCMALLOCDIR" not found in the current environment'
                        ' - no setup of tcmalloc is possible')
            return

        # For now we support the minimal version (it's the default)
        if "LD_PRELOAD" not in self._envdict:
            self._envdict["LD_PRELOAD"] = pathVar("LD_PRELOAD")
        self._envdict["LD_PRELOAD"].add(path.join(os.environ['TCMALLOCDIR'], "libtcmalloc_minimal.so"))
    

    ## @brief Add other settings
    def probeOtherSettings(self, argdict, name='all', substep=''):
        if 'env' not in argdict:
            return
        
        myEnv = argdict['env'].returnMyValue(name=name, substep=substep)
        if myEnv is None:
            return
        
        for setting in myEnv:
            try:
                k, v = setting.split('=', 1)
                self._envdict[k] = v
            except ValueError:
                msg.warning('Environment setting "{0}" seems to be invalid (must be KEY=VALUE)')
                
    ## @brief Return the value for a key, string converted
    def value(self, key):
        return str(self._envdict[key])
                
    ## @brief Return a list of KEY=VALUE pairs for this environment
    @property
    def values(self):
        return [ "{0}={1}".format(k, v) for k, v in iteritems(self._envdict) ]
    
    ## @brief Count the number of environment items that need to be updated
    @property
    def len(self):
        return len(self._envdict)
    
        
## @brief Helper class for environment variables using colon separated paths 
class pathVar(object):
    _currentEnvironmentValue = "+++CURRENT+++"

    def __init__(self, varname, separator=":", testForExistance=True):
        self._name = varname
        self._separator = separator
        self._testExistance = testForExistance
        
        # Note the special value @c _currentEnvironmentValue that will be expanded to the current
        # setting in the environment (i.e., @c os.environ['self._name']), if it exists
        self._value = [self._currentEnvironmentValue]
        
    ## @brief Add a new element to the variable 
    def add(self, value, prepend=True):
        msg.debug('Adding new value "{0}" to envvar {1} (currently {2}'.format(value, self._name, self._value))
        if value in self._value:
            msg.warning('Attempt to add environment element {0} twice to {1}'.format(value, self._name))
        if self._testExistance:
            if not os.access(value, os.R_OK):
                msg.warning("Path to {0} is not readable - will not add it to {1}".format(value, self._name))
                return
        if prepend:
            self._value[0:0] = [value]
        else:
            self._value.append(value)
 
    ## @brief Return the correct string representing the value for the shell
    def __str__(self):
        valStr = ""
        for v in self._value:
            if v == self._currentEnvironmentValue:
                if self._name in os.environ:
                    valStr += "$" + self._name + self._separator
            else:
                valStr += v + self._separator
        valStr = valStr[:-1]
        return valStr
