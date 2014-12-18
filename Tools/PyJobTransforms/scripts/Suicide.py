#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: Suicide.py 634986 2014-12-10 12:43:21Z graemes $
# Script which will kill itself (useful for testing transforms)
#
import signal
import os

os.kill(os.getpid(), signal.SIGKILL)
