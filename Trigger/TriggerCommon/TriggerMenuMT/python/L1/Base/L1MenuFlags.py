# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__="Level 1 specific configuration for L1 Run 3"

from .Limits import Limits
from collections import Iterable, OrderedDict
import six


class FlagWrapper:
    """
    this is to support the old functionality of calling and setting statusOn for flags
    """
    def __init__(self, name, value, cls):
        self.name = name
        self.value = value
        self.cls = cls
    def __call__(self):
        return self.value
    def statusOn(self):
        return self.name in self.cls.statusOn
    def setStatusOn(self, on = True):
        if on:
            self.cls.statusOn.add(self.name)
        else:
            self.cls.statusOn.remove(self.name)

class L1MenuFlagsCont(object):

    class FlagArgs:
        def __init__( self, val_type, val_default = None, val_check = None, action = None ):
            self.val_type = val_type
            self.val_default = val_default
            self.val_check = val_check
            self.action = action
    
    statusOn = set()

    __slots__ = {
        "MenuSetup"               :  FlagArgs( six.string_types ),
        "CTPVersion"              :  FlagArgs( int, 4,   val_check = lambda x: x in range(5), action = lambda x: Limits.setLimits(x) ),
        "BunchGroupPartitioning"  :  FlagArgs( Iterable, val_check = lambda x: len(list(filter(lambda y: y not in range(16), x)))==0 ),
        "BunchGroupNames"         :  FlagArgs( Iterable, val_check = lambda x: len(list(filter(lambda y: not isinstance(y, six.string_types), x)))==0),
        "MenuPartitioning"        :  FlagArgs( Iterable, val_check = lambda x: len(list(filter(lambda y: y not in range(512), x)))==0 ),
        "items"                   :  FlagArgs( Iterable, val_check = lambda x: len(list(filter(lambda y: not isinstance(y, six.string_types), x)))==0),
        "boards"                  :  FlagArgs( OrderedDict, OrderedDict() ),
        "legacyBoards"            :  FlagArgs( OrderedDict, OrderedDict() ),
        "prescales"               :  FlagArgs( dict, dict() ),
        "RemapThresholdsAsListed" :  FlagArgs( bool, False ),
        "CtpIdMap"                :  FlagArgs( dict, dict() ),
    }

    def __setattr__(self, attr, value):
        # set the object
        object.__setattr__(self, attr, value)
        d = L1MenuFlagsCont.__slots__[attr]
        # check the type
        if not isinstance(value, d.val_type):
            raise TypeError("L1MenuFlags.%s type check failed for %r. Type needs to be '%s'" % ( attr, value, d.valtype.__name__))
        # check the values
        if d.val_check and not d.val_check(value):
            raise ValueError("L1MenuFlags.%s value check failed for %r" % ( attr, value))

        if d.action:
            d.action(value)
        L1MenuFlagsCont.statusOn.add(attr)

    def __getattribute__(self,attr):
        if attr in L1MenuFlagsCont.__slots__:
            try:
                object.__getattribute__(self,attr)
            except AttributeError:
                if L1MenuFlagsCont.__slots__[attr].val_default is not None:
                    object.__setattr__(self, attr, L1MenuFlagsCont.__slots__[attr].val_default)
            return FlagWrapper(attr, object.__getattribute__(self,attr), L1MenuFlagsCont)
        else:
            return object.__getattribute__(self,attr)

        
L1MenuFlags = L1MenuFlagsCont() 

