# $Id: AutoConfFragment.py 226960 2009-11-15 23:14:39Z beringer $
#
# Job option fragment for JobRunner templates to automatically configure
# job parameters based on information obtained from the input file using
#
# Written by Juerg Beringer in November 2009.
#
printfunc ("InDetBeamSpotExample INFO Using $Id: AutoConfFragment.py 226960 2009-11-15 23:14:39Z beringer $")


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'autoconfparams' in jobConfig:                jobConfig['autoconfparams'] = ''
if not 'inputfiles' in jobConfig:                    jobConfig['inputfiles'] = [ ]


# Utility function to translate job parameter names to names used in file metadata
def metaDataName(p):
    transTable = { 'DetDescrVersion': 'GeoAtlas',
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
    printfunc ("InDetBeamSpotExample INFO Automatically configuring parameters: ", autoconfparams)
    from PyUtils.MetaReader import read_metadata
    try:
        input_file = jobConfig['inputfiles'][0]
        metadata = read_metadata(input_file)
        metadata = metadata[input_file]  # promote keys stored under input filename key one level up to access them directly

    except:
        if len(jobConfig['inputfiles'])>0:
            printfunc ("InDetBeamSpotExample ERROR Unable to autoconfigure from input file",jobConfig['inputfiles'][0])
        else:
            printfunc ("InDetBeamSpotExample ERROR Unable to autoconfigure - no input files in jobConfig['inputfiles']")
    else:
        for p in autoconfparams:
            try:
                jobConfig[p] = metadata[metaDataName(p)]
                printfunc ("InDetBeamSpotExample INFO %s --> %s" % (p,jobConfig[p]))
            except:
                printfunc ("InDetBeamSpotExample ERROR Unable to determine", p)
else:
    printfunc ("InDetBeamSpotExample INFO Nothing to autoconfigure")
