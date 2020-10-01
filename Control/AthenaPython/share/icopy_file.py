## @file: AthenaPython/icopy_file.py
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
_input_files = globals()['FNAME']
if isinstance(_input_files, str):
    _input_files = [_input_files]
acf.FilesInput = _input_files
del _input_files

# events to process
acf.EvtMax = EvtMax = theApp.EvtMax = globals().get('EVTMAX', -1)

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name='copy-file',
                   input_files=acf.FilesInput(),
                   output_file=globals().get('OFNAME', 'copy_file.pool'))
cfg.configure_job()

if cfg.is_rdo() or cfg.is_esd() or cfg.is_aod():
    # main jobos
    include ('RecExCond/RecExCommon_flags.py')
    include ('RecExCommon/RecExCommon_topOptions.py')


