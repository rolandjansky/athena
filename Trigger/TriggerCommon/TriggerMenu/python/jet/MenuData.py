# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class MenuData(object):
    """Container for menu data organized in a manner useful for jet chain
    configuration."""

    def __init__(self, scan_type, fex, hypo):
        self.scan_type = scan_type
        self.fex = fex
        self.hypo = hypo

    def __str__(self):
        s = ['%s\n%s\n' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)


