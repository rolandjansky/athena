import os
testdata = os.environ.get ('D3PDTESTDATA',
                           '/afs/cern.ch/atlas/maxidisk/d33/referencefiles')

def fix_infile (f):
    if os.path.exists (f):
        return f
    return os.path.join (testdata, f)

if type(infile) != type([]):
    infile = [infile]
infile = [fix_infile(f) for f in infile]

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock (infile)

# Turn on FPE trapping.
from RecExConfig.RecFlags import rec
rec.doFloatingPointException = True

if hasattr (rec, 'doEdmMonitor'):
    rec.doEdmMonitor.set_Value_and_Lock (False)

include (jo)

import ROOT
import cppyy
ROOT.errorcheck.ReportMessage.hideErrorLocus (True)

if not hasattr (globals(), 'd3pd_tests'):
    d3pd_tests = []
    
d3pd_tests = [(tupleFileOutput, reffile)] + d3pd_tests
def dumpit():
    import os
    for (tupleFileOutput, reffile) in d3pd_tests:
        refpath = '../test/%s' % reffile
        if not os.path.exists (refpath):
            refpath = '../../test/%s' % reffile
        os.system ('python -m D3PDMakerTest.difftuple_text %s %s' %
                   (tupleFileOutput, refpath))
import atexit
atexit.register (dumpit)

ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
