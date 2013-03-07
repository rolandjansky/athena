# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from cPickle import dumps, loads
from functools import wraps
from os import environ
from pkg_resources import resource_string

def invariant(test_data_name):
    def wrapper(func):
        testfile = "%s.%s" % (func.__name__, test_data_name)
        @wraps(func)
        def check_invariant(*args, **kwargs):
            result = func(*args, **kwargs)
            if "DQU_UPDATE_TEST_DATA" in environ:
                with open(test_data_name, "w") as fd:
                    fd.write(dumps(result))
                print "Wrote updated test data"
            else:
                test_data = loads(resource_string("testdata", testfile))
                assert result == test_data, "Data considered 'invariant' has changed"
        return check_invariant           
            
    return wrapper
