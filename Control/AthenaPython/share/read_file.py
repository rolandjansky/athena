## @file: AthenaPython/read_file.py
## @purpose: simple jobo to read any file, leveraging the auto-config fwk
## @date November 2009
## @author Sebastien Binet <binet@cern.ch>

__version__ = "$Revision: 279865 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = "simple jobo to read any file, leveraging the auto-config fwk"

## percolate through the auto-configuration

## input files configuration
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
acf = jp.AthenaCommonFlags
assert len(acf.FilesInput()) != 0, \
       "this jobo fragment needs the autoconfig-fwk." \
       "FilesInput needs to be filled"

from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = acf.FilesInput()

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name='read-file',
                   input_files=acf.FilesInput())
cfg.configure_job()

if not cfg.is_evgen():
    # main jobos
    include ('RecExCond/RecExCommon_flags.py')
    include ('RecExCommon/RecExCommon_topOptions.py')
