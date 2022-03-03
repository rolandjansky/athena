#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
Methods to deserialise HLTResultMT using Python bindings of C++ classes
and reimplementing some logic from TriggerEDMDeserialiserAlg
'''

from ROOT import StringSerializer
from AthenaCommon.Logging import logging
log = logging.getLogger('hltResultMT')

# Global StringSerialiser to avoid constructing per call
_string_serialiser = StringSerializer()

# Copy of variables defined in TrigOutputHandling/src/TriggerEDMDeserialiserAlg.h
SizeWord = 0
CLIDOffset = 1
NameLengthOffset = 2
NameOffset = 3


class EDMCollection:
    '''A python representation of a serialised EDM collection'''

    def __init__(self, name_vec, size_words, words=None):
        self.name_persistent = name_vec[0]
        self.name_key = name_vec[1]
        self.size_words = size_words
        self.size_bytes = size_words*4
        self.words = words
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

    def deserialise(self):
        if not self.words:
            log.error('No payload stored, cannot deserialise')
            return None

        # Lazily import modules needed for deserialisation
        import cppyy
        import ROOT
        import array
        import struct
        from ctypes import c_uint

        # Reinterpret words: Python int list -> C unsigned int list -> C char (Python bytes) array
        cwords = [c_uint(w) for w in self.words]
        bwords = [struct.pack('@I',cw.value) for cw in cwords]
        bwords_merged = b''.join(bwords)
        bwords_array = array.array('b', bwords_merged)

        # ROOT deserialisation
        cltype = ROOT.RootType.ByNameNoQuiet(self.name_persistent)
        buffer = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(bwords_array), bwords_array, False)
        obj_ptr = buffer.ReadObjectAny(cltype.Class())
        obj = cppyy.bind_object(obj_ptr, self.name_persistent)
        return obj


def get_collection_name(raw_data_words):
    '''Extract type+name words from the full collection raw data and convert to string'''

    # Use Python bindings of C++ StringSerialiser to deserialise collection type and name.
    # Doing the tuple-conversion here has a big performance impact. Probably this could be
    # further improved if the eformat had better support for u32slice iterators.

    nw = raw_data_words[NameLengthOffset]
    name_words = tuple(raw_data_words[NameOffset:NameOffset+nw])
    return _string_serialiser.deserialize(name_words)


def get_collection_payload(raw_data_words):
    '''Extract the serialised collection payload from the full collection raw data'''

    name_size = raw_data_words[NameLengthOffset]
    payload_start = NameOffset + name_size + 1
    return raw_data_words[payload_start:]


def get_collections(rob, skip_payload=False):
    '''
    Extract a list of EDMCollection objects from the HLT ROBFragment.
    If skip_payload=True, only the information about type, name and size
    are kept but the payload is discarded to improve performance.
    '''

    start = SizeWord
    collections = []
    last_aux_cont = None
    rod_size = len(rob.rod_data())
    while start < rod_size:
        size = rob.rod_data()[start+SizeWord]
        coll_name = get_collection_name(rob.rod_data()[start:start+size])
        words = None if skip_payload else get_collection_payload(rob.rod_data()[start:start+size])
        coll = EDMCollection(coll_name, size, words)
        if coll.is_xAOD_aux_container():
            last_aux_cont = coll
        if coll.is_xAOD_decoration():
            coll.parent = last_aux_cont
        collections.append(coll)
        start += size
    return collections
