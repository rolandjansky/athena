#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Load the DQUtils dictionary

Exposes quick_retrieve and GIL releasing functions
"""

try:
    import cppyy as PyCintex
except:
    import PyCintex
    PyCintex.Cintex.Enable()
PyCintex.loadDictionary("DQUtilsDict")

import PyCool

quick_retrieve = PyCintex.gbl.quick_retrieve
browse_coracool = PyCintex.gbl.browse_coracool
get_coracool_payload_spec = PyCintex.gbl.get_coracool_payload_spec
make_fieldselection = PyCintex.gbl.make_fieldselection
make_selection_vector = PyCintex.gbl.make_selection_vector
