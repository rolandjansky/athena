# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from .Logic import Logic, LogicType

class ThrCondition(Logic):
    def __init__(self, threshold, multiplicity=1):
        super(ThrCondition, self).__init__( logicType = LogicType.THRESHOLD, 
                                            content = { "threshold" : threshold,
                                                        "multiplicity" : multiplicity } )

    def x(self, multiplicity):
        return ThrCondition(self.threshold(), multiplicity) # provide a copy

    def __str__(self):
        return str(self.threshold()) + '[x' + str(self.multiplicity())+']'

    def threshold(self):
        return self.content["threshold"]

    def multiplicity(self):
        return self.content["multiplicity"]        

    def name(self):
        return "%s_x%i" % (self.threshold().name, self.multiplicity())

    def thresholdNames(self, include_bgrp=False):
        return [ self.threshold().name ]


class InternalTrigger(Logic):
    def __init__(self, name):
        super(InternalTrigger, self).__init__( logicType = LogicType.INTERNAL, content = name )
        
    def __str__(self):
        return self.name()

    def name(self):
        return str(self.content)

    def thresholdNames(self, include_bgrp=False):
        if include_bgrp:
            return [ self.name() ]
        else:
            return []

