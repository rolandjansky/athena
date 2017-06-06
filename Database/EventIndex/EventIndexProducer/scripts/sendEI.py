#!/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import logging
import sys

from EventIndexProducer.sendEI_Lib import eimrun
from EventIndexProducer.sendEI_Lib import options as eioptions

def main():

    # logger
    logger = logging.getLogger('sendEI.py')
    global log
    log = logger 

    # analyze options
    opt = eioptions(sys.argv[1:])

    if opt.verbose > 0:
        logger.setLevel(logging.INFO)

    if opt.debug > 0:
        logger.setLevel(logging.DEBUG)

    eimrun(logger,opt)


if __name__ == '__main__':

    # logger setup
    root = logging.getLogger()
    ch = logging.StreamHandler(sys.stdout)
    ch.setLevel(logging.DEBUG)
    #formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    formatter = logging.Formatter('%(name)-15s %(levelname)9s %(message)s')
    ch.setFormatter(formatter)
    root.addHandler(ch)

    main()

    
    

