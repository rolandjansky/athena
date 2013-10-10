# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#

"""Shell execution environments for programs to play in"""

__all__ = [
    'LocalShell',
    'utils'
    ]

from .localshell import LocalShell
from . import utils

