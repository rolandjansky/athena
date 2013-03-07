# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import division

from logging import getLogger; log = getLogger("DQUtils.sugar")

from .runlumi import IovKey, RunLumiType, TimestampType, RunLumi, IOVMIN, IOVMAX
from .iovtype import define_iov_type, make_iov_type, RANGEIOV_VAL, IOVType
from .iovset import IOVSet


# Backwards compatibility
# TODO: Make use of this throw warnings
iov_holder = IOVSet
