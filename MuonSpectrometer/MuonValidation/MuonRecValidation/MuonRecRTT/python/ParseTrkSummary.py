# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def getCollectionName(line):
    begin = line.find("=")
    collection = line[begin+2:]
    #print "Line:", line
    #print "collection: '%s'" % collection.strip()
    return collection.strip()

# To use this, do e.g.
# summary = ParseTrkSummary.parseSummaryFile()
# summary.get('MooreTracks').get('Trk/Evt')
def parseSummaryFile(filename='trkSummary.txt', makePlots=False):
    import os
    filename=os.path.expanduser(filename)
    if filename.endswith('.gz'):
        import gzip
        fh = gzip.open(filename,'r')
    else:
        fh = open(filename,'r')
    
    collectionName=""
    lineIt = iter(fh)
    summary = dict()
    while True:
        try:
            line = lineIt.next()
            if line[0]!='>' and line[0]!='|':#Ignore most lines with minimal overhead (safe since empty lines still have '\n')
                continue
            if line[0:4]==">>>>": 
                collectionName = getCollectionName(line)
                #print "collection: '%s'" % collectionName
                continue

            if line[0:2]=="||":
                firstline=line.strip().strip("||")
                # get rid of spaces around each 'key', where we get the keys by splitting the line at the '||' boundaries
                keys = [key.strip() for key in firstline.split("||")]
                nextLine = lineIt.next()
                secondline=nextLine.strip().strip("||")
                # get rid of spaces around each 'value', where we get the values by splitting the line at the '||' boundaries
                values = [float(value.strip()) for value in secondline.split("||")]
                info = dict(zip(keys,values))
                summary[collectionName]=info

        except StopIteration:
            break
    print "Found data for the following collections: ", summary.keys()
    if makePlots:
        from PmbUtils.ValuePerDomainCalcUtils import PlottableResult, JobListPlotter, TimeLabel
        title = 'MooreTracks ( %s events)' % int(summary['MooreTracks'].get('Events'))
        result = PlottableResult(summary['MooreTracks'].keys(), title=title)
        # We don't want the large values of the following to dominate the results (and anyway, they're redundant)
        exclusions = ["Events", "Tracks"]
        for key in summary['MooreTracks'].keys():
            if (key not in exclusions):
                result.addPoint(colname=key, value=summary['MooreTracks'].get(key), time=TimeLabel(label="Now",sortvalue=0))
        JobListPlotter().produceTimeDevelopmentPlot(result,"test.png")
    return summary
    
def plotSummariesForCollection(filename='test.png', collection='MooreTracks', summaries=[], times=[]):
    from PmbUtils.ValuePerDomainCalcUtils import PlottableResult, JobListPlotter, TimeLabel
    firstSummary = summaries[0]
    
    title = '%s ( %s events)' % (collection, int(firstSummary[collection].get('Events')) )
    print 'Making plot for %s with %s summaries' % (title, len(summaries))
    result = PlottableResult(firstSummary[collection].keys(), title=title)
    i=0
    for summary in summaries:
        i+=1
        # We don't want the large values of the following to dominate the results (and anyway, they're redundant)
        exclusions = ["Events", "Tracks"]
        for key in summary[collection].keys():
            if (key not in exclusions):
                print i
                result.addPoint(colname=key, value=summary[collection].get(key), time=TimeLabel(label=times[i],sortvalue=i))
    print "result.getNColumns()", result.getNColumns()
    print "result.getTimes()", result.getTimes()
    JobListPlotter().produceTimeDevelopmentPlot(result,filename)
