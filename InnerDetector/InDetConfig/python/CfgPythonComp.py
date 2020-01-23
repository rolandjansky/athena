from __future__ import print_function
def log(*args) :
    print( *args )

import sys
if sys.version_info >= (3,) :
    str_type = str
else :
    str_type=basestring

def makeSet(a_list) :
    if sys.version_info >= (3,) :
        return set(a_list)
    else :
        from sets import Set
        return Set(a_list)

def delOrSetAttr(an_obj, attr, val) :
    if sys.version_info >= (3,) :
        delattr(an_obj,attr)
    else :
        setattr(an_obj,attr,val)

