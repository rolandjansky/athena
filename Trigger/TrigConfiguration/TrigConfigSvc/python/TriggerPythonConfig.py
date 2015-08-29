# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



# moved to TriggerMenuPython
# here only a forward is kept


from AthenaCommon.Logging import logging
msg = logging.getLogger( 'TrigConfigSvc/TriggerPythonConfig.py' )

msg.info('ATTENTION: TriggerPythonConfig.py has been moved from package TrigConfigSvc to package TriggerMenuPython')
msg.info('           Pleace change your code according to the description in:')
msg.info('           https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigConfiguration/TrigConfigSvc/trunk/python/TriggerPythonConfig.py')

from TriggerMenuPython.TriggerPythonConfig import *

## Moving to the new source is quite easy:

# instead of
#
#    from TrigConfigSvc.TriggerPythonConfig import *
#
# use
#
#    from TriggerMenuPython.TriggerPythonConfig import *
#
# and instead of
#
#    import TrigConfigSvc.TriggerPythonConfig
#    c = TrigConfigSvc.TriggerPythonConfig.CTPInfo
#
# use
#
#    import TriggerMenuPython.TriggerPythonConfig
#    c = TriggerMenuPython.TriggerPythonConfig.CTPInfo
#


