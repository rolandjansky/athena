#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Interface to the general ART configuration."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import logging
import yaml

MODULE = "art.configuration"


class ArtConfiguration(object):
    """Class to interface to the ART configuration."""

    ALL = 'All'
    SEPARATOR = '/'

    def __init__(self, config_file=None):
        """Init."""
        log = logging.getLogger(MODULE)
        if config_file is None:
            config_file = 'art-configuration.yml'
        try:
            f = open(config_file, "r")
            self.config = yaml.load(f)
            f.close()
        except IOError:
            log.critical("Cannot read %s", config_file)
            exit(2)

    def release_key(self, nightly_release, project, platform):
        """
        Return release key.

        Format is: /21.0/Athena/x86_64-slc6-gcc62-opt
        """
        return ArtConfiguration.SEPARATOR + ArtConfiguration.SEPARATOR.join((nightly_release, project, platform))

    def release_key_compare(self, x, y):
        """Compare two release keys."""
        xa = x.split(ArtConfiguration.SEPARATOR)
        ya = y.split(ArtConfiguration.SEPARATOR)

        for index, item in sorted(enumerate(xa), None, None, True):
            if xa[index] < ya[index]:
                return -1
            elif xa[index] > ya[index]:
                return +1
        return 0

    def keys(self, nightly_release, project, platform, package=None):
        """Return all keys for all matching patterns for one specific package."""
        if self.config is None:
            return []

        if package is None:
            package = ArtConfiguration.ALL

        if package not in self.config:
            return []

        keys = []
        for pattern in self.config[package]:
            if fnmatch.fnmatch(self.release_key(nightly_release, project, platform), pattern):
                for key in self.config[package][pattern].keys():
                    if key not in keys:
                        keys.append(key)

        return keys

    def get(self, nightly_release, project, platform, package, key, default_value=None):
        """Return most specific value for specified key and matching pattern.

        By specifying more specific release_keys in the file [/21.0/*...]
        one can override less specific keys [/*/*...]
        (order in the file is not important):

        Tier0ChainTests:
            /*/*/*:
                dst: /yourlocaldirectory
            /21.0/*/*:
                dst: /eos/atlas/atlascerngroupdisk/data-art/grid-output

        """
        log = logging.getLogger(MODULE)
        if self.config is None:
            log.debug("No configuration")
            return default_value

        if package is None:
            log.debug("%s used for package", ArtConfiguration.ALL)
            package = ArtConfiguration.ALL

        if package not in self.config:
            log.debug("%s not in config", package)
            return default_value

        value = default_value
        for pattern in sorted(self.config[package], self.release_key_compare):
            release_key = self.release_key(nightly_release, project, platform)
            log.debug("release_key %s", release_key)
            # print key, pattern
            if fnmatch.fnmatch(release_key, pattern):
                log.debug("matched %s", pattern)
                release = self.config[package][pattern]
                if key in release:
                    value = release[key]

        return value

    def get_option(self, nightly_release, project, platform, package, key, option_key):
        """TBD."""
        value = self.get(nightly_release, project, platform, package, key)
        return option_key + value if value is not None else ''
