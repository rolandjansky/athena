#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART  - ATLAS Release Tester - Share.

Usage:
  art-share.py [options] <data_directory>

Options:
  -h --help               Show this screen
  --version               Show version
  -q --quiet              Show less information, only warnings and errors
  --write                 Write to directory
  -v --verbose            Show more information, debug level

Arguments:
  data_directory          directory to scan for shared files

"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import collections
import hashlib
import logging
import os
import sys

try:
    import scandir as scan
except ImportError:
    import os as scan

from ART.docopt import docopt

MODULE = "art.share"


class ArtShare(object):
    """Class for copying input files.

    Files are copies to the .art area under their SHA1 sum. The orignal file is replaced
    by a symbolic link. Any duplicates will result in the same SHA1 sum and thus just
    be replaced by their symbiolic link. Removing a file results in removing a link.
    If the .art directory has files without links pointing to them, these files are also
    removed.
    """

    def __init__(self, data_directory, write):
        """Constructor of ArtShare."""
        log = logging.getLogger(MODULE)
        self.data_directory = data_directory
        self.write = write

        self.art_directory = '.art'
        self.extension = '.art'
        self.count = collections.defaultdict(int)   # key is sha1

        if not os.path.isdir(self.data_directory):
            log.critical("data_directory does not exist: %s", self.data_directory)
            sys.exit(1)

        if self.write:
            log.warning("NOTE - Changing File System")
        else:
            log.warning("NOT Changing File System, use --write to change the File System")

        self.share()

    def create_sha1sum(self, path):
        """Calculate SHA1 from file on path."""
        BUF_SIZE = 65536
        sha1 = hashlib.sha1()
        with open(path, 'rb') as f:
            while True:
                data = f.read(BUF_SIZE)
                if not data:
                    break
                sha1.update(data)
        return sha1.hexdigest()

    def sha1sum(self, artpath):
        """Retrieve SHA1 from artpath specification (in the filename)."""
        f = os.path.basename(artpath)
        return os.path.splitext(f)[0]

    def share(self):
        """Share the files by copying."""
        log = logging.getLogger(MODULE)
        art_root = os.path.join(self.data_directory, self.art_directory)
        if not os.path.isdir(art_root):
            log.info("NOTE - art_directory does not exist.")
            log.info("       creating... %s", art_root)
            if self.write:
                os.makedirs(art_root)

        if os.path.isdir(art_root):
            for f in os.listdir(art_root):
                sha1art = os.path.join(art_root, f)
                if os.path.isfile(sha1art):
                    sha1 = self.sha1sum(sha1art)
                    self.count[sha1] = 0

        i = 0
        for root, dirs, files in scan.walk(self.data_directory):
            for f in files:
                if os.path.basename(root) == self.art_directory:
                    continue

                path = os.path.join(root, f)
                i += 1
                if os.path.islink(path):
                    # link
                    if not os.path.exists(path):
                        log.warning("WARNING - Stale link/file, skipping")
                        log.warning("          path: %s", path)
                        continue
                    sha1 = self.sha1sum(os.path.realpath(path))
                    log.debug("Link %d path %s", i, path)
                    log.debug("SHA1 %s", sha1)
                    self.count[sha1] += 1
                else:
                    # file
                    byte_size = os.path.getsize(path)
                    if byte_size <= 0:
                        log.warning("WARNING - zero sized file, skipping")
                        log.warning("          path: %s", path)
                        continue

                    megabyte_size = byte_size / 1024 / 1024
                    log.debug("File %d %s", i, path)
                    log.debug("File size %d", megabyte_size)
                    sha1 = self.create_sha1sum(path) if self.write or megabyte_size < 100 else "????????????????????????????????????????"
                    log.debug("SHA1 %s", sha1)

                    art_path = os.path.join(art_root, sha1 + self.extension)
                    art_relpath = os.path.relpath(art_path, os.path.dirname(path))

                    if sha1 not in self.count.keys():
                        log.info("    Moving file from %s", path)
                        log.info("                  to %s", art_path)
                        if self.write:
                            os.rename(path, art_path)
                        self.count[sha1] = 0
                    else:
                        log.info("    Removing file from %s", path)
                        if self.write:
                            os.remove(path)

                    log.info("    Creating link from %s", path)
                    log.info("                    to %s", art_relpath)
                    if self.write:
                        os.symlink(art_relpath, path)
                    self.count[sha1] += 1

        for sha1, count in self.count.iteritems():
            if count <= 0:
                art_path = os.path.join(art_root, sha1 + self.extension)
                log.info("    Removing file  %s", art_path)
                if self.write:
                    os.remove(art_path)


if __name__ == '__main__':
    if sys.version_info < (2, 7, 0):
        sys.stderr.write("You need python 2.7 or later to run this script\n")
        exit(1)

    # NOTE: import should be here, to keep the order of the decorators (module first, art last and unused)
    from art import __version__

    logging.basicConfig()
    log = logging.getLogger(MODULE)

    arguments = docopt(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
    level = logging.DEBUG if arguments['--verbose'] else logging.WARN if arguments['--quiet'] else logging.INFO
    log.setLevel(level)
    ArtShare(arguments['<data_directory>'], arguments['--write'])
