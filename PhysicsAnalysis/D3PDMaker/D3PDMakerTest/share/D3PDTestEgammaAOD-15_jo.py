# Test reading a v15 file.

infile = 'aod/AOD-15.6.4/AOD-15.6.4-full.pool.root'
reffile = 'egamma-aod-d3pd-15.ref'
tupleFileOutput = 'egamma-15.root'
jo = 'D3PDMakerConfig/AODToEgammaD3PD.py'
include ('D3PDMakerTest/run-d3pd-test.py')

