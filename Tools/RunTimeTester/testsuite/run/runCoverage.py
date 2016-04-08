# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import glob
import os
import coverage
import sys
import run

def doit():
    srcdir = os.environ['RTTSrcDir']
    pyfiles = glob.glob(os.path.join(srcdir, '*.py'))

    coverage.erase()
    coverage.start()

    run.TestFramework()

    coverage.stop()
    coverage.analysis(run)

    coverage.report(pyfiles)

    coverage.erase()

if __name__ == '__main__':
    doit()
