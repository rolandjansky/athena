# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging

from PyUtils.Decorators import memoize
from os.path import exists, join, abspath


@memoize
def getLogger(name):
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)

    # create console handler and set level to debug
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    
    # create formatter
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    # add formatter to ch
    ch.setFormatter(formatter)
    
    # add ch to logger
    logger.addHandler(ch)

    return logger



@memoize
def findFileInXMLPATH(filename):
    """ Use XMLPATH to find files (cache result through memoize decorator)"""

    if filename=="NONE":
        return filename

    filename = str(filename)

    mlog = getLogger(__name__)
    mlog.debug("Searching for XML file %s", filename)
    if filename.find('./')==0: ## this expected to be local file, name starts from ./
        return filename
    else:
        mlog.debug("XML file is not in local directory")
        from os import environ
        ## even if ./ not as file name prefix look first in PWD
        if exists(filename):
            mlog.info(filename+" XML configuration file taken from working directory")
            return filename

        ## search XMLPATH path
        if 'XMLPATH' not in environ: ## XMLPATH is not known ... no search is performed
            mlog.info("XML file: "+filename + " not found and XMLPATH not given" )
            return filename

        xmlpath = environ['XMLPATH']
        paths = str.split(xmlpath, ":")
        for path in paths:

            test = join(path, filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

            test = join(path, "TrigConfMuctpi",filename)
            if exists(test):
                mlog.info("Found XML file: " + abspath(test))
                return abspath(test)

        return filename
