#! /usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

"""
Run EventIndex on POOL file, produce EI file and send Events to brokers
"""

import argparse
import sys
import time
import traceback
from itertools import cycle, izip

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in {}'.format(sys.argv[0]))

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is {}'.format(sys.argv[0]))
    
    trf = getTransform()    
    trf.parseCmdLineArgs(sys.argv[1:])

    # update datasetname if inputfile has the form "datasetname#filename"
    # and it has not been specified within the options.
    args = trf.argdict
    if not args.has_key("eidsname"):
        dsname=None
        for filetype in ('POOL', 'AOD', 'ESD', 'EVNT', 'HITS', 'RDO'):
            if args.has_key('input'+filetype+'File'):
                inputFile = args['input'+filetype+'File']
                dsname = inputFile.dataset
        if dsname is not None:
            obj=trfArgClasses.argString(dsname)
            args['eidsname'] = obj

    # execute transformation
    trf.execute()

    # process EI output file
    (oname,oname_spb) = getOutputNames(args)
    msg.info('Output files: {} , {}'.format(oname, oname_spb))

    if oname is not None and args.has_key("sendtobroker") and args["sendtobroker"]:
        # old format
        from EventIndexProducer.sendEI_Lib import eimrun
        from EventIndexProducer.sendEI_Lib import options as eioptions

        # set default options
        import base64
        argv2 = []
        argv2.append("-v")                # verbose
        argv2.append("--trigger")         # include trigger
        # add the connect string
        if args.has_key("testbrk") and args["testbrk"]:
            # test
            earg =  "fWIqIhAfKiAxAEoxOyMtB0IxLCwASz0tYS8RHStnPBxcZn5/fUdDaGQqBwMiYy44GAozPTYQHiQ8O2BZQjUoLAcFPysqYDMJ" + \
                "cT09QRISABg1MwcXPQk="
        else:
            # prod
            earg =  "fWIqIhAfKiAxAEoxOyMtB0IoK3EXAyIhYS8cVXN4b0VVfGJiOQcKN2U+AAo1OTslEBcxOitYS30/Lj8HDCotOlghNns7LkEb" + \
                "BwYIDSE4HTsa"
        eargs = ''.join(chr(ord(c)^ord(k)) for c,k in izip(base64.b64decode(earg), cycle(trf.name))).split(",")
        argv2.extend(eargs)
        argv2.append(oname)
        eiopts = eioptions(argv2)

        # transfer file
        eimrun(msg,eiopts)

    if oname_spb is not None and args.has_key("sendtobroker") and args["sendtobroker"]:
        # SPB
        from EventIndexProducer.sendEI_SPB_Lib import eimrun
        from EventIndexProducer.sendEI_SPB_Lib import options as eioptions
        
        # set default options
        import base64
        argv2 = []
        argv2.append("-v")                # verbose
        # add the connect string
        if args.has_key("testbrk") and args["testbrk"]:
            # test
            earg = "fWIqIhAfKiAxAEoxOyMtB0IxLCwASz0tYS8RHStnPBxcZn5/fUdDaGQqBwMiYy44GAozPTYQHiQ8O2BZQjUoLAcFPysqYDMJ" + \
                "cT09QRISABg1MwcXPQlYS308fCkaCzUmNhoSfCw8f1oMIDsxWgU4dXt4R0NoZD4DFTEsLCkHHGl/FjItCAAdGy4/dH0GRzJm" + \
                "HXsDPUNoZD4DFSMqLD4RG2kYKQVQFCsfOTUmFnlmPxApHAQNGCczcQc/LigFAC8kAHYdayFUCAIkYFlCICYsEgc8Iy0tFwRp" + \
                "PS0BAw=="
        else:
            # prod
            earg = "fWIqIhAfKiAxAEoxOyMtB0IoK3EXAyIhYS8cVXN4b0VVfGJiOQcKN2U+AAo1OTslEBcxOitYS30/Lj8HDCotOlghNns7LkEb" + \
                "BwYIDSE4HTsaWEJoOmwRCDQ/ICUaG2kqLEdIMyo9IloMLXNrQFV8YmItAxwkKjwRFSNjeQUyJB0GDSM8AH57FUc7cxtrOy98" + \
                "YmItAxw2LDwGAyRjHjoFWQEtDwEnGRx/dT8ZPBoUNQoYOXcUPyc9AxAXNj98G3ghXR0ENFhLfSogPxIOKSU9FQU7Yzs+AQo="
        eargs = ''.join(chr(ord(c)^ord(k)) for c,k in izip(base64.b64decode(earg), cycle(trf.name))).split(",")
        argv2.extend(eargs)
        argv2.append(oname_spb)
        eiopts = eioptions(argv2)

        # transfer file
        eimrun(msg,eiopts)

    trf.generateReport()

    msg.info("{} stopped at {}, trf exit code {:d}".format(sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

## Get the base transform with all arguments added
def getTransform():
    executorSet = set()
    from EventIndexProducer.EITransformUtils import addEI_Substep,  addEI_arguments
    addEI_Substep(executorSet)

    trf = transform(executor = executorSet, description = 'EventIndex transform. Input must be a POOL file.')
    addAthenaArguments(trf.parser)
    addEI_arguments(trf.parser)
    return trf


## Build output file names
def getOutputNames(args):

    if args.has_key("eifmt"):
        eifmt = args['eifmt'].value
    else:
        eifmt = 0
    if args.has_key("outputEIFile"):
        outputEIFile = args['outputEIFile'].value[0]
    else:
        outputEIFile = None

    if eifmt <= 0:
        if outputEIFile is not None:
            oname = outputEIFile
        else:
            oname = "output.ei.pkl"
        oname_spb = None
    elif eifmt == 1:
        if outputEIFile is not None:
            oname_spb = outputEIFile
        else:
            oname_spb = "output.ei.spb"
        oname = None
    else:
        # both outputs
        if outputEIFile is None:
            oname     = "output.ei.pkl"
            oname_spb = "output.ei.spb"
        else:
            if outputEIFile.endswith(".pkl"):
                oname     = outputEIFile
                oname_spb = oname[:-4] + ".spb"
            elif outputEIFile.endswith(".spb"):
                oname_spb = outputEIFile
                oname     = oname_spb[:-4] + ".pkl"
            else:
                oname_spb = outputEIFile + ".spb"
                oname     = outputEIFile
    
    return (oname,oname_spb)

if __name__ == '__main__':
    main()
