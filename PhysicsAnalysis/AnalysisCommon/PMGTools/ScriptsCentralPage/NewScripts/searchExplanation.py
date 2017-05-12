#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import pyAMI.client
import time,datetime
from pytz import timezone

client = pyAMI.client.Client(
  'atlas'
  #'atlas-replica'
)


def main():

    cern_time = timezone('UCT')
    current_time = datetime.datetime.fromtimestamp(time.time(),cern_time).strftime('%Y-%m-%d %H:%M:%S') 
    
    
    # parse options from command line
    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog arguments", version="%prog")
    parser.add_option("-v", "--verbose", dest="verbose", action="store_true",  help="print more information (default: %default)")
    parser.add_option("-d", "--dataset", dest="dataset", action="store",  help="dataset to search (default: %default)")
    parser.add_option("-r", "--reference", dest="reference", action="store",  help="k-factor explanation reference to search (default: %default)")
    parser.add_option("-t", "--time", dest="time", action="store",  help="time to search from (default: %default)")
    
    parser.set_defaults(verbose=False, dataset="mc15_13TeV.410200%", reference="%CrossSectionNLOttV#NEW_Updated_ttW_and_ttZ_k_factor%", time=current_time)
    (options, args) = parser.parse_args()

    print "Options:"    
    print options
    
    datasetsToQuery=options.dataset
    explanationSearch=options.reference
    current_time=options.time
    
    res = client.execute(['GetPhysicsParamsForDataset',"--logicalDatasetName=%s"% datasetsToQuery,"--timestamp='%s'"%current_time,"--explanation=%s"% explanationSearch], format='dom_object')

    print "Results:"    
    for r in res.get_rows():
        for k,v in r.iteritems():
            print k,":",v," , ",
            
        print



#=====================================================================
# __name__
#=====================================================================
if __name__ == '__main__':
    main()

