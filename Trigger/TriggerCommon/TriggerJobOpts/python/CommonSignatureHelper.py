# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" helpers for signature flags manipulations """
class CommonSignatureHelper(object):
    def setAll(self):
        self.signatures.set_On()
        
    def unsetAll(self):
        self.signatures.set_Off()
        
    def enableSignature(self, chainInfo):
        """ Add signature defined by chainInfo array """
        self.enableSignatures([chainInfo])
        
    def enableSignatures(self, chainInfoList):
        """ Add list of signatures """
        current=self.signatures.get_Value()
        new = [sig for sig in chainInfoList if sig not in current]
        new.extend(current)
        self.signatures.set_Value(new)

    def disableSignature(self, l):
        """ Disable signature with name l """
        self.disableSignatures([l])
        
    def disableSignatures(self, list):
        """ Disable list of sigantures """
        current = self.signatures.get_Value()
        new = [sig for sig in current if sig[0] not in list ]
        self.signatures.set_Value(new)


class Bag(object):
    def __init__(self,l):
        self.allowedValues = l

    def __str__(self):
        return str(self.allowedValues)

    def __repr__(self):
        return "%r" % self.allowedValues

    def __contains__(self, val):
        return True in [ x == val for x in self.allowedValues]

    def __eq__(self, arg):
        if type(arg)==list or type(arg)==tuple or type(arg)==set:
            
            notContained = [a for a in arg if a not in self]

            if len(notContained)==0:
                return True

            from AthenaCommon.Logging import logging
            log = logging.getLogger( 'AllowedList' )
            log.error("the element %s is not in the list of allowed values: %s", notContained, self)
            return False
        return True


class AllowedList(list):
    def __init__(self, l):
        super(AllowedList,self).__init__([Bag(l)])
