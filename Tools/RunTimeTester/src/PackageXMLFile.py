# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path
from validateXMLFile import Validator
from Evaluate        import Evaluate
from exc2string2     import exc2string2
from RTTSException   import RTTInputError

class PackageXMLFile:
    """
    Do not stop the RTT if user input exceptions are thrown,
    but keep the information and create error objects
    (minders, test runs) later on for reporting to the user
    """
    
    def __init__(self, pathToFile):
        self.path    = pathToFile
        self.errMsg  = ''
        self._docEl  = None
        self.isValid = self.validate_()

    def exists(self):
        return os.path.exists(self.path)

    def validate_(self):
        if not self.exists():
            self.errMsg = 'User XML file %s does not exist' % self.path
            return False

        try:
            validator = Validator(self.path) # throws exception if parse fails
        except RTTInputError, e:
            self.errMsg = 'User xml file does not parse:\n%s\nTraceback:\n%s' % (str(e), exc2string2())
            return False

        self._docEl = validator._dom.documentElement

        try:
            validator.validate() # throws exception if invalid
        except RTTInputError, e:
            self.errMsg = 'User input file does not validate:\n%s\nTraceback:\n%s' % (str(e), exc2string2())
            return False
        except:
            return False
        
        return True

    def __getitem__(self, tag):    
        if not self.isValid: return None
        return Evaluate(tag, self._docEl)

    def __del__(self):
        if not self.isValid: return
        if self._docEl:
            self._docEl.unlink()
            del self._docEl
