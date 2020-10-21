# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import collections

def namedtuple_with_defaults(typename, field_names, default_values=()):
    T = collections.namedtuple(typename, field_names)
    T.__new__.__defaults__ = (None,) * len(T._fields)
    if isinstance(default_values, collections.Mapping):
        prototype = T(**default_values)
    else:
        prototype = T(*default_values)
    T.__new__.__defaults__ = tuple(prototype)
    return T


# namedtuple class with some defaults set for those entries that do not require settings
ChainProp = namedtuple_with_defaults("ChainProp", 
                                     ['name', 'l1SeedThresholds', 'stream', 'groups', 'mergingStrategy', 'mergingOrder', 'mergingOffset', 'topoStartFrom'],
                                     {'stream':['Main'],
                                      'l1SeedThresholds': [], 
                                      'mergingStrategy':'auto', 
                                      'mergingOrder': [], 
                                      'mergingOffset': -1, 
                                      'topoStartFrom': False})


