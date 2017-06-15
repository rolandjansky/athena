# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

"""Switch stdout on, off"""

class StdOutController(object):
    def __init__(self):
        self.old_out = sys.stdout

    def off(self):

        # for some reason setting stdout to os.devnull goes into
        # into an inifinte loop on isx, anaconda distribution.
        sys.stdout = open('/dev/null', 'w')

    def on(self):
        sys.stdout = self.old_out
