# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
