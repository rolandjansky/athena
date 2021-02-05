# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import



def makeMsg(args, sep=' '):
    s = ''
    for a in args:
        s += str(a) + sep
    return s


class DebugPrinter:
    def __init__(self, prefix = '', debug=False):
        self.prefix = prefix + ' '
        self.debug = debug

    def __call__(self, *args):

        if self.debug:
            m = makeMsg(args)
            print (self.prefix, m)

if __name__ == '__main__':
    printer = DebugPrinter(debug=True, prefix = 'testprefix')
    printer('a')
    printer ('text', (1, 2, 5.9))
    printer ('text', 1, 2, 5.9)
