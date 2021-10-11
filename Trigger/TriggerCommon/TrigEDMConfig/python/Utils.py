# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from functools import lru_cache
from AthenaCommon.Logging import logging
log = logging.getLogger('TrigEDMConfig')

@lru_cache(maxsize=None)
def getEDMVersionFromBS(filename):
   """Determine Trigger EDM version based on the input ByteStream file.

   Run-3 EDM is indicated by HLT ROD version > 1.0. For Run 1 and 2 the
   HLT ROD version was 0.0 and the run number is used to disambiguate between them.
   """

   boundary_run12 = 230000
   boundary_run23 = 368000

   import eformat
   from libpyeformat_helper import SubDetector
   bs = eformat.istream(filename)

   rodVersionM = None
   rodVersionL = None
   # Find the first HLT ROBFragment in the first event
   for robf in bs[0]:
       if robf.rob_source_id().subdetector_id()==SubDetector.TDAQ_HLT:
           rodVersionM = robf.rod_minor_version() >> 8
           rodVersionL = robf.rod_minor_version() & 0xFF
           log.debug("HLT ROD minor version from input file is %d.%d", rodVersionM, rodVersionL)
           break

   # Case 1: failed to read ROD version
   if rodVersionM is None or rodVersionL is None:
       log.warning("Cannot determine HLT ROD version from input file, falling back to run-number-based decision")
   # Case 2: ROD version indicating Run 3
   elif rodVersionM >= 1:
       log.info("Determined EDMVersion to be 3, because running on BS file with HLT ROD version %d.%d",
                 rodVersionM, rodVersionL)
       return 3

   # Case 3: ROD version indicating Run 1 or 2 - use run number to disambiguate
   runNumber = bs[0].run_no()
   log.debug("Read run number %s", runNumber)

   if not runNumber or runNumber <= 0:
       log.warning("Cannot determine EDM version because run number %s is invalid. ", runNumber)
       return None
   elif runNumber < boundary_run12:
       # Run-1 data
       log.info("Determined EDMVersion to be 1 based on BS file run number (runNumber < %d)",
                 boundary_run12)
       return 1
   elif runNumber < boundary_run23:
       # Run-2 data
       log.info("Determined EDMVersion to be 2 based on BS file run number (%d < runNumber < %d)",
                 boundary_run12, boundary_run23)
       return 2
   else:
       # Run-3 data
       log.info("Determined EDMVersion to be 3 based on BS file run number (runNumber > %d)",
                 boundary_run23)
       return 3
