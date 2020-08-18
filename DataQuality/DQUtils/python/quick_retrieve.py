#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Load the DQUtils dictionary

Exposes quick_retrieve and GIL releasing functions
"""

import cppyy

import PyCool

cppyy.load_library("libDQUtilsDict.so")

quick_retrieve = cppyy.gbl.quick_retrieve
browse_coracool = cppyy.gbl.browse_coracool
get_coracool_payload_spec = cppyy.gbl.get_coracool_payload_spec
make_fieldselection = cppyy.gbl.make_fieldselection
make_selection_vector = cppyy.gbl.make_selection_vector

__all__ = ['PyCool', 'quick_retrieve', 'browse_coracool',
           'get_coracool_payload_spec', 'make_fieldselection',
           'make_selection_vector']
