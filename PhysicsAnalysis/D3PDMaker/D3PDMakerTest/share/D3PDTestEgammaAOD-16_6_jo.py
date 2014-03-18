# Test reading a v16.6 file.

infile = 'aod/AOD-16.6.2.1/AOD-16.6.2.1-full.pool.root'
reffile = 'egamma-aod-d3pd-16_6.ref'
tupleFileOutput = 'egamma-16_6.root'
jo = 'D3PDMakerConfig/AODToEgammaD3PD.py'

# Force AODFix off, to get repeatible results across releases.
from RecExConfig.RecFlags import rec
rec.doApplyAODFix.set_Value_and_Lock(False)

include ('D3PDMakerTest/run-d3pd-test.py')

