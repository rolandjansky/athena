#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Methods to deserialise HLTResultMT using Python bindings of C++ classes
and reimplementing some logic from TriggerEDMDeserialiserAlg
'''

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


def get_collection_name(raw_data_words):
    '''Use Python bindings of C++ StringSerialiser to deserialise collection names'''

    from ROOT import string, vector, StringSerializer
    nw = raw_data_words[NameLengthOffset]
    name_raw = raw_data_words[NameOffset:NameOffset+nw]
    name_raw_vec = vector('unsigned int')()
    for w in name_raw:
        name_raw_vec.push_back(w)
    name_str_vec = vector(string)()
    ss = StringSerializer()
    ss.deserialize(name_raw_vec, name_str_vec)
    name_list = []
    for s in name_str_vec:
        name_list.append(str(s))
    return name_list


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
        coll_raw = raw_data_words[start:start+size]
        coll = EDMCollection(get_collection_name(coll_raw), size)
        if coll.is_xAOD_aux_container():
            last_aux_cont = coll
        if coll.is_xAOD_decoration():
            coll.parent = last_aux_cont
        collections.append(coll)
        start += size
    return collections
