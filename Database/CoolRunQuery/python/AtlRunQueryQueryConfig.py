# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from utils.AtlRunQueryUtils import Enumerate

import time

class QC:
    localtime = True
    timezone = ('CET','CEST')
    datapath = ''

    @classmethod
    def tzdesc(cls):
        return QC.timezone[1] if QC.localtime else 'UTC'

    @classmethod
    def settimezone(cls):
        time.tzname = QC.timezone

    @classmethod
    def localStr(cls):
        return "local" if QC.localtime else 'UTC'


