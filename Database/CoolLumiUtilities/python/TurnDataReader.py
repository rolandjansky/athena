# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TurnDataReader
#
# Contents:
# TurnDataReader - horrible hack to read in turn data for luminosity scan data from text files
#                  provided by Nitesh.  In time this will be replaced by a correct storage of this
#                  data in COOL, but at the moment this is the best we have...
#

import re

class TurnDataReader:

    def __init__(self, infile=None):
        
        self.verbose = False
        
        # File to read data from
        self.infile = infile
        
        # dict keyed by pseudoLB number containing dict by algorithm with data Tuple
        # (turns, counts) = self.data[lb][algName] 
        self.data = None

    def readData(self):

        self.data = None
        if self.infile == None:
            print 'TurnDataReader.readData() called with infile == None!'
            return self.data

        # Open input file
        try:
            f = open(self.infile, 'r')
        except IOError, e:
            print 'Error opening file', self.infile
            print e
            return self.data

        # Initialize our new dictionary
        self.data = dict()
        
        # Must parse lines looking for either LB records or algorithm type
        
        # pseudoLB record (number followed by two text records
        matchlb = re.compile('^([0-9]+) (\S+) (\S+)$')
        
        # algorithm name (only thing on the line)
        matchalg = re.compile('^(\S+)$')

        # Current algorithm read in file
        algId = None
        
        for line in f.readlines():

            if self.verbose: print line,

            m = matchlb.search(line)
            if m:
                if self.verbose: print 'Found data record match:', m.group(1), m.group(2), m.group(3)
                lb = int(m.group(1))
                # Hack to realign these properly.  This will probably bite us in the ass someday
                lb -= 1
                
                if m.group(2) == 'nan':
                    counts = 0.
                else:
                    counts = float(m.group(2))
                    
                if m.group(3) == 'nan':
                    turns = 0.
                else:
                    turns = float(m.group(3))

                # Do we have a valid algorithm?
                if algId == None:
                    print 'TurnDataReader.readData() found data record with no algorithm defined in', self.infile
                    print line
                    sys.exit()
                
                # save data
                if not lb in self.data:
                    self.data[lb] = dict()

                self.data[lb][algId] = (turns, counts)

                if self.verbose:
                    print 'Alg:', algId, 'LB:', lb, 'Turns:', turns, 'Counts:', counts
                continue
            
            m = matchalg.search(line)
            if m:
                if self.verbose: print 'Found new algorithm:', m.group(1)

                # Decode Nitesh's into my values, just hardcode here
                algId = m.group(1)
                if algId == None:
                    print 'TurnDataReader.readData() found unrecognized algorithm name in', self.infile
                    print line
                    sys.exit()

        f.close()
        return self.data
    
# Test by running from command line
if __name__ == '__main__':

    dr = TurnDataReader('fill1386.txt')
    dr.verbose = True
    dr.readData()
