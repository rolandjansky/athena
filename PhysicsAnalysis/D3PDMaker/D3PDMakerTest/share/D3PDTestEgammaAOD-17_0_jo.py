# Test reading a v17.0 file.

import os
testdata = os.environ.get ('D3PDTESTDATA',
                           '/afs/cern.ch/atlas/maxidisk/d33/referencefiles')

# Testing changing trigger menu.
infile1 = 'aod/AOD-17.0.2/AOD-17.0.2-munged.pool.root'
#infile1 = os.path.join (testdata, infile1)
#infile1a = 'AOD-17.0.2-munged.pool.root'
infile2 = 'aod/AOD-17.0.4/AOD-17.0.4-full.pool.root'

#os.system ('python -m D3PDMakerTest.changerun 123456 %s %s' %
#           (infile1, infile1a))

infile = [infile1, infile2]

reffile = 'egamma-aod-d3pd-17_0.ref'
tupleFileOutput = 'egamma-17_0.root'
jo = 'D3PDMakerConfig/AODToEgammaD3PD.py'

# Force AODFix off, to get repeatible results across releases.
from RecExConfig.RecFlags import rec
rec.doApplyAODFix.set_Value_and_Lock(False)

include ('D3PDMakerTest/run-d3pd-test.py')

