#!/bin/env python

from __future__ import print_function

#Import datasets from RTT--------------------------------------------------------------------------                                                   
def importRTTdatasets(jobID):
    from AthenaCommon.Utils.unixtools import find_datafile
    xmlFile = find_datafile("TrigInDetValidation_TestConfiguration.xml")
    import xml.etree.ElementTree as elemTree
    tree = elemTree.parse(xmlFile)
    namespace = "{http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt}"
    datasetList = []
    root = tree.getroot()
    for job in root.iter(namespace + 'athena'):
        if job.get('userJobId') == jobID:
            for dataset in job.findall(namespace + 'dataset'):
                eosDataset = "root://eosatlas.cern.ch/" + dataset.text
                datasetList.append(eosDataset)
    return datasetList
#--------------------------------------------------------------------------------------------------                                                   

def main(argv):
    for x in argv[1:]:
        for ds in importRTTdatasets( x ):
            print (ds)

if __name__ == "__main__":
    import sys
    main(sys.argv)

