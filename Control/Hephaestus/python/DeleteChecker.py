# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: Hephaestus/DeleteChecker.py
# Author Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '1.1.0'
__author__ = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

def setup():
   from MemoryTracker import DeleteChecker as ddcheck

   ddcheck.save()
   ddcheck.install()

   return ddcheck.save, ddcheck.install, ddcheck.start, ddcheck.stop, ddcheck.uninstall

save, install, start, stop, uninstall = setup()
del setup
