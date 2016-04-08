# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id $
"""
Miscellaneous utilities related to get LHC-related information.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'


#
# Stupid dicts to store LHC info needed by scripts (e.g. for approved plots).
# How could we get this from COOL?
#
lhcFillData = {
    '142165': '907',
    '142193': '911',
    '142195': '912',
    '142308': '916',
    '142402': '923',
    '152166': '1005',
    '152409': '1022',
    '153565': '1058',
    '153599': '1059',
    '154813': '1089',
    '215456': '3351'
}

lhcEnergyData = {
    '142193': '900 GeV',
    '142308': '2.36 TeV',
    '142402': '2.36 TeV',
    '152166': '7 TeV',
    '152409': '7 TeV',
    '153565': '7 TeV',
    '153599': '7 TeV',
    '215456': '8 TeV'
}
