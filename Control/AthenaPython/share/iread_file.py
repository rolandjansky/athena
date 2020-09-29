## @file: AthenaPython/iread_file.py
## @purpose: simple jobo to read any file, leveraging the auto-config fwk
## @date January 2010
## @author Sebastien Binet <binet@cern.ch>

__version__ = "$Revision: 273745 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = "simple jobo to read any file, leveraging the auto-config fwk"

## percolate through the auto-configuration

## input files configuration
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
acf = jp.AthenaCommonFlags
_input_files = globals().get('FNAME', [])
if isinstance(_input_files, str):
    _input_files = [_input_files]
acf.FilesInput = _input_files
del _input_files

# events to process
acf.EvtMax = EvtMax = globals().get('EVTMAX', -1)

# main jobos
include('AthenaPython/read_file.py')
