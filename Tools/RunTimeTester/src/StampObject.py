# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools     import isDateStampedToday
from Tools2    import extractReleaseFromNightlyLink
import os.path

# ---------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# ---------------------------------------------------

class StampObject:
    'Responsible for returning the nightly release'

    def __init__(self, stampFile):
        self.stampFile = stampFile
        self.nicosNewDayHour = '19:00' # midnight "is" 19:00
        
    def giveUpWaitingForNightly(self):
        return False # for now

    def findNightlyRelease(self): 
         found = False
         release = ''

         if not os.path.exists(self.stampFile):
             logger.warning('Non-existent stamp file %s' % str(self.stampFile))
             return (found,release)

         logger.debug("Stamp file: %s" %str(self.stampFile))

         if isDateStampedToday(self.stampFile, self.nicosNewDayHour):
             release = extractReleaseFromNightlyLink(self.stampFile)
             found = True
         else:
             msg = 'StampFile is not from today %s' % str(self.stampFile)
             logger.warning(msg)
             
         return (found,str(release))
    
    
