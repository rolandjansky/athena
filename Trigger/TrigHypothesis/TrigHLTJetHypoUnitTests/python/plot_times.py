# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import pylab as pl
import sys
import os





def times(fn):
    result = []
    with open(fn) as iff:
        for l in iff:
            if 'tav' in l and 'node: 0 parent: 0' in l:
                result.append(float(l[:-1].split()[-1]))

    return result


if __name__ == '__main__':
    
    if len(sys.argv) < 2:
        print('Please supply an input file name on the command line')
        sys.exit(0)


    fn = sys.argv[1]
    if not os.path.exists(fn):
        print('unknown file. giving up: ', fn)
        sys.exit(0)
    x =  times(fn)
    # print x
    pl.hist(x)
    pl.suptitle('Execution times for %s' % fn)
    pl.xlabel('time (ns)')
    pl.ylabel('no of calls')
    pl.savefig('%s_times.pdf' % fn[:-4])
    pl.show()
