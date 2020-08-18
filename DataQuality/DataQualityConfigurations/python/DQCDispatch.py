# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This holds the method that returns an appropriate DataQualityConfiguration
# module for the specified input string
# Algorithm: exact match to input if exists, otherwise dispatch based on end
# of string, or to pattern data[0-9]{2}
#
# 2012-12-05 Peter Onyisi

def getmodule(modname):
    import six
    assert isinstance(modname, six.string_types), 'Argument to getmodule must be a string'

    # Local file?
    try:
        mod = __import__(modname)
        components = modname.split('.')
        for comp in components[1:]:
            mod = getattr(mod,comp)
        return mod
    except ImportError:
        pass
    
    # Does it exist already?
    try:
        return getattr(__import__('DataQualityConfigurations.'+modname), modname)
    except ImportError:
        pass

    # Does it match a pattern?
    if (modname.endswith('_900GeV')
        or modname.endswith('_2p76TeV')
        or modname.endswith('_5TeV')
        or modname.endswith('_7TeV')
        or modname.endswith('_8TeV')
        or modname.endswith('_13TeV')
        or modname.endswith('_calib')
        or modname.endswith('_comm')):
        from . import base_data; return base_data
    if (modname.endswith('_hi')
        or modname.endswith('_hip')):
        from . import base_data_hi; return base_data_hi
    elif (modname.endswith('_1beam') ):
        from . import base_data_1beam; return base_data_1beam
        #return __import__('base_data_1beam')
    elif (modname.endswith('_cos')
          or modname.endswith('_calocomm')
          or modname.endswith('_idcomm')
          or modname.endswith('_larcomm')
          or modname.endswith('_muoncomm')
          or modname.endswith('_tilecomm')):
        from . import base_data_cos; return base_data_cos

    import re
    # Does it look like datann_hip?
    m = re.match('data\d{2}_hip', modname)
    if m:
        from . import base_data_hi; return base_data_hi
    # Does it look like datann?
    m = re.match('data\d{2}$', modname)
    if m:
        from . import base_data; return base_data

    raise ValueError('Unable to find a valid configuration for %s' % modname)
