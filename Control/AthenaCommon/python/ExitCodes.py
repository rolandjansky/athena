# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/ExitCodes.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Conventional exit codes, returned to shell."""


### successful run
ALL_OK                   = 0


### runtime/processing errors
BEGIN_RUN_EXCEPTION      = 0x10         # not implemented
BEGIN_RUN_FAILURE        = 0x11         # not implemented

INI_ALG_EXCEPTION        = 0x20         # no Gaudi support
INI_ALG_FAILURE          = 0x21

REINI_ALG_EXCEPTION      = 0x30         # not implemented
REINI_ALG_FAILURE        = 0x31         # not implemented

EXE_ALG_EXCEPTION        = 0x40
EXE_ALG_FAILURE          = 0x41

END_RUN_EXCEPTION        = 0x50         # not implemented
END_RUN_FAILURE          = 0x51

FIN_ALG_EXCEPTION        = 0x60         # no Gaudi support
FIN_ALG_FAILURE          = 0x61

### RESERVED RANGE:
###  0x81 - 0xff reserved as to not overlap with the Transforms'
###  report of signals causing exit

### miscellaneous error codes
OPTIONS_UNKNOWN          = 0x01
INCLUDE_ERROR            = 0x02
IMPORT_ERROR             = 0x03
DATA_INFILE_ERROR        = 0x04         # not implemented
DATA_OUTFILE_ERROR       = 0x05         # not implemented
HISTO_OUTFILE_ERROR      = 0x06         # not implemented
DATABASE_ERROR           = 0x07         # not implemented
UNKNOWN_EXCEPTION        = 0x08
CONFIGURATION_ERROR      = 0x09


### helper to get a human-readable string
codes = {
   ALL_OK              : "successful run",
   BEGIN_RUN_EXCEPTION : "exception raised during beginRun()",
   BEGIN_RUN_FAILURE   : "failure in beginRun()",
   INI_ALG_EXCEPTION   : "exception raised during initialization",
   INI_ALG_FAILURE     : "failure in initialization",
   REINI_ALG_EXCEPTION : "exception raised during an algorithm re-initialization",
   REINI_ALG_FAILURE   : "failure in an algorithm re-initialization",
   EXE_ALG_EXCEPTION   : "exception raised during an algorithm execute",
   EXE_ALG_FAILURE     : "failure in an algorithm execute",
   END_RUN_EXCEPTION   : "exception raised during endRun()",
   END_RUN_FAILURE     : "failure in endRun()",
   FIN_ALG_EXCEPTION   : "exception raised during finalization",
   FIN_ALG_FAILURE     : "failure in finalization",
   OPTIONS_UNKNOWN     : "unknown options supplied",
   INCLUDE_ERROR       : "an error occurred when including a file",
   IMPORT_ERROR        : "an error occurred when importing a file",
   DATA_INFILE_ERROR   : "an error occurred when reading a data file",
   DATA_OUTFILE_ERROR  : "an error occurred when writing a data file",
   HISTO_OUTFILE_ERROR : "an error occurred when writing a histogram file",
   DATABASE_ERROR      : "a database error occurred",
   UNKNOWN_EXCEPTION   : "an unknown exception occurred"
}

def what( code ):
   global codes
   try:
      return codes[ code ]
   except KeyError:
      return "unknown"
