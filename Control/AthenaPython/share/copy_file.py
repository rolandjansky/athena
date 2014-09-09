## @file: AthenaPython/copy_file.py
## @purpose: simple jobo to copy any file, leveraging the auto-config fwk
## @date January 2010
## @author Sebastien Binet <binet@cern.ch>

__version__ = "$Revision: 293864 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = "simple jobo to copy any file, leveraging the auto-config fwk"

## percolate through the auto-configuration

## input files configuration
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
acf = jp.AthenaCommonFlags
assert len(acf.FilesInput()) != 0, \
       "this jobo fragment needs the autoconfig-fwk." \
       "FilesInput needs to be filled"

import AthenaPython.ConfigLib as apcl
cfg = apcl.copy_file(src=acf.FilesInput(),
                     dst="copy_file.pool")

if cfg.is_rdo() or cfg.is_esd() or cfg.is_aod():
    # main jobos
    include ('RecExCond/RecExCommon_flags.py')
    include ('RecExCommon/RecExCommon_topOptions.py')

