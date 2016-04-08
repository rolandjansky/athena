# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"run validation against a DTD. The DTD location is written into the xml file."

from xml.parsers.xmlproc      import xmlproc, xmlval
import os.path

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------

def validateXMLFile(fileName):
    # logger.critical('Hacked version of validateXMLFile !!!! always returns True!!!!')
    # logger.critical('UCL is down - no DTD!!!')
    # return True

    try:
        validator = xmlval.XMLValidator()
        validator.parse_resource(fileName)
        logger.debug(fileName+' is validated against DTD')
    except SystemExit, e:
        logger.error(fileName+' is NOT validated against DTD ')
        msg  = 'caught SytemExit: maybe xml file does not refer to a '
        msg += 'DTD or is invalid against DTD'
        logger.error(msg)
        raise Exception, str(os.path.basename(fileName)) + " not valid against DTD"
    except Exception, e:
        logger.error(fileName+' is NOT validated against DTD ')
        msg =  'Unknown exception: '+sys.exc_info()
        logger.error(e)
        logger.error(msg)
        raise Exception, str(os.path.basename(fileName)) + ' not valid against DTD'

if __name__ == '__main__':

    import getopt, os, sys

    try:
        opts,args = getopt.getopt(sys.argv[1:],'')
    except getopt.GetoptError:
        print '1 usage validateXMLFile fn'
        sys.exit(0)


    if len(args) != 1:
        print '2 usage validateXMLFile fn'
        sys.exit(0)

    file = args[0]
    if not os.path.exists(file):
        print 'unknown file', file
        sys.exit(0)

    validateXMLFile(file)
