#! /usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

"""
Run EventIndex on POOL file, produce EI file and send Events to brokers
"""

import sys
import time
from itertools import cycle

# Setup core logging here
from PyJobTransforms.trfLogger import msg

from PyJobTransforms.transform import transform
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler
from PyJobTransforms.trfDecorators import sigUsrStackTrace

import PyJobTransforms.trfArgClasses as trfArgClasses

msg.info('logging set in {}'.format(sys.argv[0]))


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():

    msg.info('This is {}'.format(sys.argv[0]))

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])

    # update datasetname if inputfile has the form "datasetname#filename"
    # and it has not been specified within the options.
    args = trf.argdict
    if "eidsname" not in args:
        dsname = None
        for filetype in ('POOL', 'AOD', 'ESD', 'EVNT', 'HITS', 'RDO'):
            if 'input'+filetype+'File' in args:
                inputFile = args['input'+filetype+'File']
                dsname = inputFile.dataset
        if dsname is not None:
            obj = trfArgClasses.argString(dsname)
            args['eidsname'] = obj

    # execute transformation
    trf.execute()

    # process EI output file
    oname_spb = getOutputNames(args)
    msg.info('Output file: {}'.format(oname_spb))

    if (oname_spb is not None and "sendtobroker" in args
            and args["sendtobroker"].value):
        # SPB
        from EventIndexProducer.sendEI_SPB_Lib import eimrun
        from EventIndexProducer.sendEI_SPB_Lib import options as eioptions

        # set default options
        import base64
        argv2 = []
        argv2.append("-v")                # verbose
        # add the connect string
        if "testbrk" in args and args["testbrk"]:
            # test
            earg = "fWIqIhAfKiAxAEoxOyMtB0IxLCwASz0tYS8RHStnPBxcZn5/" \
                   "fUdDaGQqBwMiYy44GAozPTYQHiQ8O2BZQjUoLAcFPysqYDMJ" \
                   "cT09QRISABg1MwcXPQlYS308fCkaCzUmNhoSfDx8YhcKNydx" \
                   "Fw5qe3t/WEJoKCgHBzMsKj8HQ3MAGT8+Hx0YFiRecRBsIFAC" \
                   "ewAFWEJoKCgHFTUsPSkAQxQ/LkIiNB86DT08dXAUAh8DBA4g" \
                   "PBl9ERQ8HhoALBwbXBF9CkY+HSRjYVkKKjo5FQo8LS4vH0Mx" \
                   "OyoR"
        else:
            # prod
            earg = "fWIqIhAfKiAxAEoxOyMtB0IoK3EXAyIhYS8cVXN4b0VVfGJi" \
                   "OQcKN2U+AAo1OTslEBcxOitYS30/Lj8HDCotOlghNns7LkEb" \
                   "BwYIDSE4HTsaWEJoOmwRCDQ/ICUaG2k6bFoFNT0hYhcHf31r" \
                   "R0p9Yi47Bw4mKjoHFXx5Bgo/NwobCC42YXsWfyBZF30QPUp9" \
                   "Yi47BxwgKi0REnweOT1CKyEZKjUvA392BwIWFgIeGC4mdxcH" \
                   "PBcPBjwkCWMbexlGNwgic1lLNSA8KhUDKSs+Fw18Oz05EQ=="
        eargs = ''.join(chr(c ^ ord(k)) for c, k in
                        zip(base64.b64decode(earg),
                            cycle(trf.name))).split(",")
        argv2.extend(eargs)
        argv2.append(oname_spb)
        eiopts = eioptions(argv2)

        # transfer file
        eimrun(msg, eiopts)

    trf.generateReport()

    msg.info("{} stopped at {}, trf exit code {:d}".format(sys.argv[0],
                                                           time.asctime(),
                                                           trf.exitCode))
    sys.exit(trf.exitCode)


# Get the base transform with all arguments added
def getTransform():
    executorSet = set()
    from EventIndexProducer.EITransformUtils import addEI_Substep
    from EventIndexProducer.EITransformUtils import addEI_arguments
    addEI_Substep(executorSet)

    trf = transform(executor=executorSet,
                    description='EventIndex transform. '
                    'Input must be a POOL file.')
    addAthenaArguments(trf.parser)
    addEI_arguments(trf.parser)
    return trf


# Build output file names
def getOutputNames(args):

    if "eifmt" in args:
        eifmt = args['eifmt'].value
    else:
        eifmt = 0                            # noqa: F841
    if "outputEIFile" in args:
        outputEIFile = args['outputEIFile'].value[0]
    else:
        outputEIFile = None

    # ignore eifmt
    # keep eifmt format flag for compatibility and future usage
    if outputEIFile is not None:
        oname_spb = outputEIFile
    else:
        oname_spb = "output.ei.spb"

    return oname_spb


if __name__ == '__main__':
    main()
