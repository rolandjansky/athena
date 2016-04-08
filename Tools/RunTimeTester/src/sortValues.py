# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def sortValues(dict):
    sortedKeys = dict.keys()
    values = []
    [values.append(dict[k]) for k in sortedKeys]
    return values
