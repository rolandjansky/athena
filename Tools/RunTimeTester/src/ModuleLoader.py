# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Class that loads an external module A.

Calling
A.getClassInstance(classname, parameters)
will return
classname(parameters)
for classname in A


Module name is without the .py extension.

"""
#------------------------------------------------
from exc2string2              import exc2string2
import imp, sys, os.path

import logging
logger = logging.getLogger('rtt')

#------------------------------------------------
class ModuleLoader:

    def __init__(self, modulename, pathToModule, logger):
        
        self.modulename   = modulename
        self.pathToModule = pathToModule
        self.logger       = logger

    def getClassInstance(self, classname, paramDict={}):
        """paramDict is a dictionary of argname-argvalue pairs
        that will be passed to the class constructor.
        See __main__ example below."""

        
        try:
            file,pathname,description = imp.find_module(self.modulename,
                                                        [self.pathToModule])
        except Exception, e:
            m = 'Failed to find module: \n%s %s' %(self.modulename,
                                                  self.pathToModule)
            self.logger.warning (m)
            self.logger.warning(exc2string2())
            self.logger.warning(str(e))
            return None

        try:
            klass                     = imp.load_module(classname,
                                                        file,
                                                        pathname,
                                                        description)
        except Exception, e:
            m  = 'Failed to load module:'
            m += '\n  class %s '         % classname
            m += '\n  file %s '          % file.name
            m += '\n  pathname %s '      % pathname
            if not os.path.exists(os.path.join(pathname, file.name)):
                m += '\npath to module does not exist'
            else:
                m += '\npath to module ok. Ensure class is present in module'
                                  
                                  
            self.logger.warning(m)
            self.logger.warning(exc2string2())
            self.logger.warning(str(e))
            return None

        if classname not in vars(klass).keys():
            m  = 'Failed to instantiate class:'
            m += '\n  class %s '         % classname
            m += '\n  from pathname %s \n'      % pathname

            self.logger.warning(m)
            return None
        
        return((vars(klass)[classname])(paramDict))  #instantiate class with paramDict


if __name__=="__main__":
    class Logger:
        def debug(self, s): print s
        def warning(self, s): print s
        def error(self, s): print s

    paramDict={'SimpleTest_key': 'SimpleTest_value'}
    ml = ModuleLoader('AtlasEnvSetup', '../share', Logger())
    pp = ml.getClassInstance('AtlasEnvSetup', paramDict)

    
        
