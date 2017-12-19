#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Class to handle art-headers."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import logging
import re

from types import IntType
from types import ListType
from types import StringType

MODULE = "art.header"


class ArtHeader(object):
    """Class to handle art-headers."""

    ART_CI = 'art-ci'
    ART_DESCRIPTION = 'art-description'
    ART_INCLUDE = 'art-include'
    ART_INPUT = 'art-input'
    ART_INPUT_NFILES = 'art-input-nfiles'
    ART_INPUT_SPLIT = 'art-input-split'
    ART_OUTPUT = 'art-output'
    ART_TYPE = 'art-type'

    def __init__(self, filename):
        """Keep arguments, setup patterns for re-use, define possible art-header definitions."""
        self.header_format = re.compile(r'#\s(art-[\w-]+):\s+(.+)$')
        self.header_format_error1 = re.compile(r'#(art-[\w-]*):\s*(.+)$')
        self.header_format_error2 = re.compile(r'#\s\s+(art-[\w-]*):\s*(.+)$')
        self.header_format_error3 = re.compile(r'#\s(art-[\w-]*):\S(.*)$')

        self.filename = filename

        self.header = {}

        # general
        self.add(ArtHeader.ART_DESCRIPTION, StringType, '')
        self.add(ArtHeader.ART_TYPE, StringType, None, ['build', 'grid'])
        self.add(ArtHeader.ART_INCLUDE, ListType, ['*'])

        # "build" type only
        self.add(ArtHeader.ART_CI, ListType, [])

        # "grid" type only
        self.add(ArtHeader.ART_OUTPUT, ListType, [])
        self.add(ArtHeader.ART_INPUT, StringType, None)
        self.add(ArtHeader.ART_INPUT_NFILES, IntType, 1)
        self.add(ArtHeader.ART_INPUT_SPLIT, IntType, 0)

        self.read(filename)

    def add(self, key, value_type, default_value=None, constraint=None):
        """Add a single header definition."""
        self.header[key] = {}
        self.header[key]['type'] = value_type
        self.header[key]['default'] = default_value
        self.header[key]['constraint'] = constraint
        self.header[key]['value'] = None    # e.g. the value was never set

    def is_list(self, key):
        """Return true if key exists and is of ListType."""
        return self.header[key]['type'] is ListType if key in self.header else False

    def read(self, filename):
        """Read all headers from file."""
        log = logging.getLogger(MODULE)
        for line in open(filename, "r"):
            line_match = self.header_format.match(line)
            if line_match:
                try:
                    key = line_match.group(1)
                    value = line_match.group(2)
                    if key in self.header:
                        if self.header[key]['type'] == StringType:
                            value = value.strip()
                        elif self.header[key]['type'] == IntType:
                            value = int(value)

                    if self.is_list(key):
                        # handle list types
                        if self.header[key]['value'] is None:
                            self.header[key]['value'] = []
                        self.header[key]['value'].append(value)
                    else:
                        # handle values
                        if key not in self.header:
                            log.warning("Unknown art-header %s: %s in file %s", key, value, filename)
                            self.header[key] = {}
                            self.header[key]['value'] = None
                        if self.header[key]['value'] is None:
                            self.header[key]['value'] = value
                        else:
                            log.warning("key %s: already set to %s in file %s", key, self.header[key]['value'], filename)
                except ValueError:
                    log.error("Invalid value in art-header %s: %s in file %s", key, value, filename)

    def get(self, key):
        """
        Get the value of a header by key.

        Return default if header not specified.
        Warn and return None if header is not defined.
        """
        log = logging.getLogger(MODULE)
        if key not in self.header:
            log.warning("Art seems to look for a header key %s which is not in the list of defined headers.", key)
            return None

        if self.header[key]['value'] is None:
            return self.header[key]['default']

        return self.header[key]['value']

    def print_it(self):
        """Print content of the headers for this file."""
        log = logging.getLogger(MODULE)
        for key in self.header:
            log.info("%s: %s %s %s %s", key, self.header[key]['type'], self.header[key]['default'], self.header[key]['value'], self.header[key]['constraint'])

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
        log = logging.getLogger(MODULE)
        for line in open(self.filename, "r"):
            if self.header_format_error1.match(line):
                log.error("LINE: %s", line.rstrip())
                log.error("Header Validation - invalid header format, use space between '# and art-xxx' in file %s", self.filename)
                log.error("")
            if self.header_format_error2.match(line):
                log.error("LINE: %s", line.rstrip())
                log.error("Header Validation - invalid header format, too many spaces between '# and art-xxx' in file %s", self.filename)
                log.error("")
            if self.header_format_error3.match(line):
                log.error("LINE: %s", line.rstrip())
                log.error("Header Validation - invalid header format, use at least one space between ': and value' in file %s", self.filename)
                log.error("")

        for key in self.header:
            if 'type' not in self.header[key]:
                log.error("Header Validation - Invalid key: %s in file %s", key, self.filename)
                log.error("")
                continue
            if type(self.header[key]['value']) != self.header[key]['type']:
                if not isinstance(self.header[key]['value'], type(None)):
                    log.error("Header Validation - value_type: %s not valid for key: %s, expected value_type: %s in file %s", type(self.header[key]['value']), key, self.header[key]['type'], self.filename)
                    log.error("")
            if self.header[key]['constraint'] is not None and self.header[key]['value'] not in self.header[key]['constraint']:
                if self.header[key]['value'] is None:
                    log.error("Header Validation - missing key: %s in file %s", key, self.filename)
                else:
                    log.error("Header Validation - value: %s for key: %s not in constraints: %s in file %s", self.header[key]['value'], key, self.header[key]['constraint'], self.filename)
                log.error("")

        return 0
