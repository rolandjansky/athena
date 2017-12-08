#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import re

from types import IntType
from types import ListType
from types import StringType


class ArtHeader(object):
    """TBD."""

    def __init__(self, filename):
        """TBD."""
        self.header_format = re.compile(r'#\s(art-[\w-]+):\s+(.+)$')
        self.header_format_error1 = re.compile(r'#(art-[\w-]+):\s*(.+)$')
        self.header_format_error2 = re.compile(r'#\s\s+(art-[\w-]+):\s*(.+)$')
        self.header_format_error3 = re.compile(r'#\s(art-[\w-]+):\S(.*)$')

        self.filename = filename

        self.header = {}

        # general
        self.add('art-description', StringType, '')
        self.add('art-type', StringType, None, ['build', 'grid'])

        # "build" type only
        self.add('art-ci', ListType, [])

        # "grid" type only
        self.add('art-include', ListType, ['*'])
        self.add('art-output', ListType, [])
        self.add('art-input', StringType, None)
        self.add('art-input-file', ListType, [])
        self.add('art-input-nfiles', IntType, 0)
        self.add('art-input-nevents', IntType, 0)
        self.add('art-input-split', IntType, 0)

        self.read(filename)

    def add(self, key, value_type, default_value=None, constraint=None):
        """TBD."""
        self.header[key] = {}
        self.header[key]['type'] = value_type
        self.header[key]['default'] = default_value
        self.header[key]['constraint'] = constraint
        self.header[key]['value'] = None    # never set

    def is_list(self, key):
        """TBD."""
        return self.header[key]['type'] is ListType if key in self.header else False

    def read(self, filename):
        """Read all headers from file."""
        for line in open(filename, "r"):
            line_match = self.header_format.match(line)
            if line_match:
                key = line_match.group(1)
                value = line_match.group(2)
                if key in self.header and self.header[key]['type'] == StringType:
                    value = value.strip()

                if self.is_list(key):
                    if self.header[key]['value'] is None:
                        self.header[key]['value'] = []
                    self.header[key]['value'].append(value)
                else:
                    if key not in self.header:
                        self.header[key] = {}
                    self.header[key]['value'] = value

    def get(self, key):
        """TBD."""
        if key not in self.header:
            return None

        if self.header[key]['value'] is None:
            return self.header[key]['default']

        return self.header[key]['value']

    def print_it(self):
        """TBD."""
        for key in self.header:
            print key, self.header[key]['type'], self.header[key]['default'], self.header[key]['value'], self.header[key]['constraint']

    def validate(self):
        """
        Validate the '# art-*' headers in the file.

        Validation fails if:
        - a header is spaced correctly (e.g. '#art-header: value')
        - a value in a header is not spaced correctly (e.g. '# art-header:value')
        - a key is found which is not defined
        - a value is found of the wrong value_type
        - a value is found outside the constraint
        """
        for line in open(self.filename, "r"):
            if self.header_format_error1.match(line):
                print "LINE: ", line.rstrip()
                print "ERROR: Header Validation - invalid header format, use space between '# and art-xxx' in file", self.filename
                print
            if self.header_format_error2.match(line):
                print "LINE: ", line.rstrip()
                print "ERROR: Header Validation - invalid header format, too many spaces between '# and art-xxx' in file", self.filename
                print
            if self.header_format_error3.match(line):
                print "LINE: ", line.rstrip()
                print "ERROR: Header Validation - invalid header format, use at least one space between ': and value' in file", self.filename
                print

        for key in self.header:
            if 'type' not in self.header[key]:
                print "ERROR: Header Validation - Invalid key:", key, "in file", self.filename
                print
                continue
            if type(self.header[key]['value']) != self.header[key]['type']:
                if not isinstance(self.header[key]['value'], type(None)):
                    print "ERROR: Header Validation - value_type:", type(self.header[key]['value']), "not valid for key:", key, "expected value_type:", self.header[key]['type'], "in file", self.filename
                    print
            if self.header[key]['constraint'] is not None and self.header[key]['value'] not in self.header[key]['constraint']:
                if self.header[key]['value'] is None:
                    print "ERROR: Header Validation - missing key:", key, "in file", self.filename
                else:
                    print "ERROR: Header Validation - value:", self.header[key]['value'], "for key:", key, "not in constraints:", self.header[key]['constraint'], "in file", self.filename
                print

        return 0
