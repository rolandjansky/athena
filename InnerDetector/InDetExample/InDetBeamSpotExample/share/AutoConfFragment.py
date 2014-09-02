# $Id: AutoConfFragment.py 226960 2009-11-15 23:14:39Z beringer $
#
# Job option fragment for JobRunner templates to automatically configure
# job parameters based on information obtained from the input file using
# PyUtils.AthFile. The primary use case for this is DetDescrVersion.
#
# Written by Juerg Beringer in November 2009.
#
print "InDetBeamSpotExample INFO Using $Id: AutoConfFragment.py 226960 2009-11-15 23:14:39Z beringer $"


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'autoconfparams' in jobConfig:                jobConfig['autoconfparams'] = ''
if not 'inputfiles' in jobConfig:                    jobConfig['inputfiles'] = [ ]


# Utility function to translate job parameter names to names used in file metadata
def metaDataName(p):
    transTable = { 'DetDescrVersion': 'geometry',
                   'GlobalTag': 'conditions_tag'
                  }
    if p in transTable:
        return transTable[p]
    else:
        return p


# Get list of parameters to extract that are not already defined in jobConfig
autoconfparams = [ ]
for p in [ w.strip() for w in jobConfig['autoconfparams'].split(',') ]:
    if p and not p in jobConfig:
        autoconfparams.append(p)


# Extract parameters
if autoconfparams:
    print "InDetBeamSpotExample INFO Automatically configuring parameters: ", autoconfparams
    import PyUtils.AthFile
    try:
        athFile = PyUtils.AthFile.fopen(jobConfig['inputfiles'][0])
    except:
        if len(jobConfig['inputfiles'])>0:
            print "InDetBeamSpotExample ERROR Unable to autoconfigure from input file",jobConfig['inputfiles'][0]
        else:
            print "InDetBeamSpotExample ERROR Unable to autoconfigure - no input files in jobConfig['inputfiles']"
    else:
        for p in autoconfparams:
            try:
                # Below, we might also take this out of athFile.fileinfos['tag_info'] (w/different names)
                jobConfig[p] = athFile.fileinfos[metaDataName(p)]
                print "InDetBeamSpotExample INFO %s --> %s" % (p,jobConfig[p])
            except:
                print "InDetBeamSpotExample ERROR Unable to determine", p
else:
    print "InDetBeamSpotExample INFO Nothing to autoconfigure"
