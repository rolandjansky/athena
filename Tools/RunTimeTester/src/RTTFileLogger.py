# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logging, os, logging.handlers

class RTTFileHandler(logging.handlers.MemoryHandler):
    def __init__(self, ofn):
        logging.handlers.MemoryHandler.__init__(self, 1000, logging.DEBUG)
        self.ofn = ofn
        try:
            os.remove(ofn)
        except:
            pass
        
    def emit(self, record):
        of = open(self.ofn, 'a')
        of.write(self.formatter.format(record)+'\n')
        of.close()
    def shouldFlush(self, record): return True

def getLogger(name, ofn):
    msgfmt = '%(asctime)s %(filename)s %(lineno)s %(levelname)s %(message)s'
    timefmt   = '%d%b%y %H:%M:%S'
    formatter=logging.Formatter(msgfmt, timefmt)



    handler = RTTFileHandler(ofn)
    handler.setFormatter(formatter)
    handler.setLevel(logging.DEBUG)
    
    logger = logging.getLogger(name)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)

    return logger

def useLogger(s, logger):
    logger.debug(s)

def testFunction():
    logger = getLogger('rttlogger', 'logtest.txt')
    logger.debug('aaa')
    logger.error('bbb\nccc')

