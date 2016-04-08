# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import replace
def formatCollection(c):
    return replace(str(c),',',',\n  ')
