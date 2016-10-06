# Test reading a v16.0 file.

infile = 'aod/AOD-16.0.2.3/AOD-16.0.2.3-full.pool.root'
reffile = 'egamma-aod-d3pd-16_0.ref'
tupleFileOutput = 'egamma-16_0.root'
jo = 'D3PDMakerConfig/AODToEgammaD3PD.py'

# Force AODFix off, to get repeatible results across releases.
from RecExConfig.RecFlags import rec
rec.doApplyAODFix.set_Value_and_Lock(False)

include ('D3PDMakerTest/run-d3pd-test.py')

