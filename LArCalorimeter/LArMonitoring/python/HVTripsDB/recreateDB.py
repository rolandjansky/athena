#!/usr/bin/env python2.6

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

os.remove("trips.db")

execfile("createDB.py")


