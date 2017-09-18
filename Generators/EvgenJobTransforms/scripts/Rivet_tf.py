#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#! /usr/bin/env python

"""
Take an input EVNT file and run Rivet on it
"""

import os, sys, time, shutil
from PyJobTransforms.trfLogger import msg
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from EvgenJobTransforms.evgenTrfArgs import addStdEvgenArgs
import PyJobTransforms.trfArgClasses as trfArgClasses


class RivetExecutor(athenaExecutor):
    "Specialised trf executor class for Rivet jobs"

    def __init__(self, skeleton):
        athenaExecutor.__init__(self, name="rivet", skeletonFile=skeleton, substep="evnt2yoda", tryDropAndReload=False, inData=["EVNT"], outData=["outNULL"])


def getTransform():
    exeSet = set()
    exeSet.add(RivetExecutor("EvgenJobTransforms/skeleton.EVGENtoRivet.py"))
    trf = transform(executor=exeSet)
    addAthenaArguments(trf.parser)
    addStdEvgenArgs(trf.parser)
    return trf


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    msg.info("This is %s" % sys.argv[0])
    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()
    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


if __name__ == "__main__":
    main()
