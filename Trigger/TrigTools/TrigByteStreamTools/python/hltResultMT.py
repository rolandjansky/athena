#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Methods to deserialise HLTResultMT using Python bindings of C++ classes
and reimplementing some logic from TriggerEDMDeserialiserAlg
'''

from ROOT import vector, StringSerializer
from functools import lru_cache

# Global StringSerialiser to avoid constructing per call
_string_serialiser = StringSerializer()

# Copy of variables defined in TrigOutputHandling/src/TriggerEDMDeserialiserAlg.h
SizeWord = 0
CLIDOffset = 1
NameLengthOffset = 2
NameOffset = 3


class EDMCollection:
    '''A simple representation of an EDM collection storing just its names and size'''

    def __init__(self, name_vec, size_words):
        self.name_persistent = name_vec[0]
        self.name_key = name_vec[1]
        self.size_words = size_words
        self.size_bytes = size_words*4
        self.parent = None

    def __str__(self):
        return '{:s}, Size: {:d} bytes'.format(self.name(), self.size_bytes)

    def name(self):
        if not self.parent:
            return '{:s}#{:s}'.format(self.name_persistent, self.name_key)
        else:
            return '{:s}#{:s}{:s}'.format(self.name_persistent, self.parent.name_key, self.name_key)

    def is_xAOD_interface_container(self):
        return self.name_persistent.startswith('xAOD') and not self.name_key.endswith('Aux.')

    def is_xAOD_aux_container(self):
        return self.name_persistent.startswith('xAOD') and self.name_key.endswith('Aux.')

    def is_xAOD_decoration(self):
        return 'vector' in self.name_persistent and \
                    not self.is_xAOD_interface_container() and \
                    not self.is_xAOD_aux_container()

    def is_TP_container(self):
        return '_p' in self.name_persistent


@lru_cache(maxsize=2048)
def deserialise_name(name_words):
    '''Use Python bindings of C++ StringSerialiser to deserialise collection type and name'''

    name_raw_vec = vector['unsigned int'](name_words)
    name_str_vec = vector['string']()
    _string_serialiser.deserialize(name_raw_vec, name_str_vec)
    return name_str_vec


@lru_cache(maxsize=4096)
def get_collection_name(raw_data_words):
    '''Extract type+name words from the full collection raw data and convert to string'''

    nw = raw_data_words[NameLengthOffset]
    name_words = raw_data_words[NameOffset:NameOffset+nw]
    name_str_vec = deserialise_name(tuple(name_words))
    return [str(s) for s in name_str_vec]


def get_collections(raw_data_words):
    '''
    Extract a list of EDMCollection objects from raw data
    (ROD data from an HLT ROBFragment)
    '''

    start = SizeWord
    collections = []
    last_aux_cont = None
    while start < len(raw_data_words):
        size = raw_data_words[start+SizeWord]
        coll_name = get_collection_name(tuple(raw_data_words[start:start+size]))
        coll = EDMCollection(coll_name, size)
        if coll.is_xAOD_aux_container():
            last_aux_cont = coll
        if coll.is_xAOD_decoration():
            coll.parent = last_aux_cont
        collections.append(coll)
        start += size
    return collections
