#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import re

from copy import copy
from types import ListType
from types import StringType


class ArtHeader(object):
    """TBD."""

    def __init__(self, filename):
        """TBD."""
        self.header = re.compile(r'#\s(art-[\w-]+):\s+(.+)$')
        self.header_format_error1 = re.compile(r'#(art-[\w-]+):\s*(.+)$')
        self.header_format_error2 = re.compile(r'#\s\s+(art-[\w-]+):\s*(.+)$')
        self.header_format_error3 = re.compile(r'#\s(art-[\w-]+):\S(.*)$')

        self.filename = filename

        self.type = {}
        self.defaultValue = {}
        self.constraint = {}
        self.value = {}

        self.add('art-description', StringType, '')
        self.add('art-type', StringType, None, ['build', 'grid'])
        self.add('art-ci', ListType, [])
        self.add('art-output', ListType, [])

        self.read(filename)

    def add(self, key, type, defaultValue=None, constraint=None):
        """TBD."""
        self.type[key] = type
        self.defaultValue[key] = defaultValue
        self.constraint[key] = constraint
        self.value[key] = copy(defaultValue)

    def is_list(self, key):
        """TBD."""
        return self.type[key] is ListType if key in self.type else False

    def read(self, filename):
        """Read all headers from file."""
        for line in open(filename, "r"):
            line_match = self.header.match(line)
            if line_match:
                key = line_match.group(1)
                value = line_match.group(2)
                if self.type[key] == StringType:
                    value = value.strip()

                if self.is_list(key):
                    self.value[key].append(value)
                else:
                    self.value[key] = value

    def get(self, key):
        """TBD."""
        return self.value[key] if key in self.value else None

    def print_it(self):
        """TBD."""
        for key in self.type:
            print key, self.type[key], self.defaultValue[key], self.value[key], self.constraint[key]

    def validate(self):
        """
        Validate the '# art-*' headers in the file.

        Validation fails if:
        - a header is spaced correctly (e.g. '#art-header: value')
        - a value in a header is not spaced correctly (e.g. '# art-header:value')
        - a key is found which is not defined
        - a value is found of the wrong type
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

        for key in self.value:
            if key not in self.type:
                print "ERROR: Header Validation - Invalid key: '" + key + "' in file", self.filename
                print
                continue
            if type(self.value[key]) != self.type[key]:
                if not isinstance(self.value[key], type(None)):
                    print "ERROR: Header Validation - type:", type(self.value[key]), "not valid for key:", key, "expected type:", self.type[key], "in file", self.filename
                    print
            if self.constraint[key] is not None and self.value[key] not in self.constraint[key]:
                if self.value[key] is None:
                    print "ERROR: Header Validation - missing key: '" + key + "' in file", self.filename
                else:
                    print "ERROR: Header Validation - value: '" + self.value[key] + "' for key: '" + key + "' not in constraints:", self.constraint[key], "in file", self.filename
                print

        return 0
