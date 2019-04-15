# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""Utilities for flake8 plugins"""

def flake8_atlas(f):
   """Default decorator for flake8 plugins"""
   f.name = 'flake8_atlas'
   f.version = '1.0'
   return f

def off_by_default(f):
   """Decorator to disable plugin by default (use --enable-extensions)"""
   f.off_by_default = True
   return f
