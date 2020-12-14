# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from collections import defaultdict
import numpy as np
def tree(key_wrapper = None):
    return lambda : TreeDict({}, key_wrapper)
def directory_like(*key):
    if len(key) == 1:
        return key[0].split('/',1)
    else:
        return key

def get_array(hist):
    arr = hist.GetArray()
    arr.SetSize(hist.GetNbinsX() + 2)
    return np.fromiter(arr, np.float)

TAIL   = lambda s: u' └── {} '.format(s)  # noqa: E731
BRANCH = lambda s: u' ├── {} '.format(s)  # noqa: E731
LINE   = lambda s: u' │   {} '.format(s)  # noqa: E731
SPACE  = lambda s: u'     {} '.format(s)  # noqa: E731
class TreeDict(defaultdict):
    def __init__(self, dic = {}, key_wrapper=None):
        super(defaultdict,self).__init__(dic)
        self.default_factory = tree(key_wrapper)
        self.key_wrapper = key_wrapper
    def __missing__(self, key, wrap=True):
        if self.default_factory is None: raise KeyError((key,))
        if wrap and self.key_wrapper:
            key = self.key_wrapper(key)
            if len(key) == 1:
                return self.__missing__(key[0], wrap=False)
            else:
                return self[key[0]][key[1]]
        self[key] = value = self.default_factory()
        return value
    def __setitem__(self, key, value, wrap=True):
        if wrap and self.key_wrapper:
            key = self.key_wrapper(key)
            if len(key) == 1:
                self.__setitem__(key[0], value, wrap=False)
            else:
                self[key[0]][key[1]] = value
        else:
            super(defaultdict, self).__setitem__(key, value)
    def to_dict(self):
        return dict((key, val.to_dict()) if isinstance(val, TreeDict) else (key, val) for key, val in self.items())
    def __repr__(self, level=0):
        ret = u''
        items = list(self.items())
        len_key = 0
        for i, (key, val) in enumerate(items):
            LS = BRANCH if i < len(items)-1 else TAIL
            ret += SPACE(' '*len_key)*level + LS(key.ljust(len_key)) + '\n'
            if isinstance(val, TreeDict):
                ret += val.__repr__(level+1)
            else:
                ret += SPACE(' '*len_key)*(level+1) + LS(str(val).ljust(len_key)) + '\n' 
        return ret
    def _repr_pretty_(self, p, cycle):
        return p.text(repr(self))
