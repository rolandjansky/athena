#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## David Cote, 17 July 2009
## Put the GetCool method in a python import, to ensure it is queried only once.

__all__ = [
    "cool",
    ]

### globals ----------------------------------------------------------------
def _setup():
    from AthenaCommon.Logging import logging
    logGetCool = logging.getLogger( 'GetCool' )
    logGetCool.debug("Getting CoolConvUtilities object...")  

    from RecExConfig.AutoConfiguration import GetRunNumber
    from RecExConfig.AutoConfiguration import GetLBNumber
    rn=GetRunNumber()
    lb=GetLBNumber()
        
    from CoolConvUtilities.MagFieldUtils import getFieldForRun

    if lb==None:
        print rn
        cool=getFieldForRun(rn)
    else:
        # MagFieldDCSInfo
        cool=getFieldForRun(run=rn,lumiblock=lb)
    return cool

# execute function at module import
cool=_setup()            
# clean-up
del _setup
