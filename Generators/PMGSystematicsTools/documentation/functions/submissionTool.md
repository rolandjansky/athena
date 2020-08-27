Help on module submissionTool

# NAME
submissionTool

# FILE
/afs/cern.ch/user/l/lcorpe/work/private/testPMGSystTools_13Sept18/systematics-tools/local/bin/submissionTool.py

# DESCRIPTION
This module is a wrapper to pathena, which does the following useful things:
- Retrieves the list of matrix-elememt weights stored in a given ATLAS MC samples, either
-- using the DSID_database
-- or by automatically downloading a test sample and checking manually which weights are
available when initialising the Rivet_i instances
- Produces Job options from templates stored in /data/RivetAnalysis*JO*py
- Automatic download of test samples if needed

Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch

# FUNCTIONS
## downloadTestFile(testSampleDir, thisSampleName)
`testSampleDir` String (look in this directory to try to find a matching test file)
`thisSampleName` String (name of sample to download)
`result` String or None (String if a matching file name was found, None otherwise)

download a tst file for the specified dataset name into the specified dir name

## findTestFile(testSampleDir, identifier)
`testSampleDir` String (look in this directory to try to find a matching test file)
`identifier` String (A string which uniquely defined the sample you are interested in
for example, the DSID of the sample of interest)
`result` String or None (String if a matching file name was found, None otherwise)

check the specified directory for a test EVNT file for a specified DISD

## main(argv)
This module can also be run as a standalone executable. 
For info about the options try:
submissionTool.py -h

This tool is used to submit samples to the GRID, when once instance of Rivet_i per Matrix-Element
weight is initialised and run. Template Job Options are available in /data/RivetAnalysis_*JO*.py

# DATA
## args = ['submissionTool']
## opts = <Values at 0x7f94f76be2d8: {'inputSamples': 'exa...s': '', 'nJo...
## parser = <optparse.OptionParser instance>
## submissionTemplatesDir = '/afs/cern.ch/user/l/lcorpe/work/private/test...


