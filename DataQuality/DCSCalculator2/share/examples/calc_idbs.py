#! /usr/bin/env python

from DQUtils import fetch_iovs, write_iovs
from DQUtils.iov_arrangement import inverse_lblb
from DQUtils.utils import pprint_objects

from DCSCalculator2.lib import dcsofl_cool_record
from DCSCalculator2.subdetectors import IDBS

def calculate_idbs(run_range):
    # List of lumiblocks to run over
    lbtime = inverse_lblb(fetch_iovs("LBLB", runs=run_range))
    
    idbs = IDBS()
    idbs.input_db = "COOLOFL_INDET/CONDBR2"
    
    result_iovs = idbs.run(lbtime)
    
    print "Will write the following:"
    pprint_objects(result_iovs)
    for entry in result_iovs:
        print entry.since.run, entry.since.lumi, "->", entry.until.lumi, entry.Code
        
    write_iovs("idbs_test.db/CONDBR2::/GLOBAL/DETSTATUS/DCSOFL",
               result_iovs, dcsofl_cool_record())

if __name__ == "__main__":
    print "Querying single run:",
    calculate_idbs(165703)
    
    print "Querying with run range:"
    calculate_idbs((165703, 165817))
