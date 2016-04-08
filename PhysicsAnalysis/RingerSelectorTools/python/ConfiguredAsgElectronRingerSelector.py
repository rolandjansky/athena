# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredAsgElectronRingerSelector
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
## Created:     Dec. 2011
##
## Description: Apply the default configurations for the AsgPhotonIsEMSelector,
##              but allow for overwriting them with user-defined values.
##
##=============================================================================

from RingerSelectorTools.ElectronRingerSelectorMapping import ElectronRingerMap

def ConfiguredAsgElectronRingerSelector( quality, menu, cutIDConfDict = None, **kwargs ):
    """
    Configure the AsgElectronRingerSelector with the quality cuts
    and allow for setting its configurable properties through 
    cutIDConfDict and kwargs arguments.

    Use cutIDConfDict argument to set the properties from the CutIDSelector.
    It can also be set directly by setting the property CutIDSelector through
    kwargs. Beware not to set them together, as setting CutIDSelector
    through kwargs will overwrite cutIDConfDict configurations and, thus, it
    will be the only configuration used for the job.
    """
    try:
      RingerSelectorConfigurable = ElectronRingerMap[(quality, menu)]

      if cutIDConfDict is None:
        cutIDConfDict = {}
      # Configure it
      ringerSelectorTool = RingerSelectorConfigurable(cutIDConfDict = cutIDConfDict,
                                                      **kwargs)
      return ringerSelectorTool
    except KeyError:
      from AthenaCommon.Logging import logging
      import traceback
      mlog = logging.getLogger( 'ConfiguredAsgElectronRingerSelector.py' )
      mlog.error("There is no such configuration available:\n %s", traceback.format_exc())
      raise

