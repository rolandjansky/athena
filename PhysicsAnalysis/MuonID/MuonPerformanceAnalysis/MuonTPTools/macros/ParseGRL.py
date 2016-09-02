#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Parse a GRL in pyROOT

import xml.etree.ElementTree as ET

class GRLHandler:
    def __init__ (self, GRLs):
        self.RunDict = {}
        if len(GRLs)>1:
            print 'Currently only one GRL supported! Using first one...'
        tree = ET.parse(GRLs[0])
        root = tree.getroot()
        for athing in root:
            #print athing.tag
            if athing.tag == "NamedLumiRange":
                for thing in athing:
                    if thing.tag == "LumiBlockCollection":
                        #print "Found a LumiBlockCollection"
                        #for entry in thing
                        current_run = -1
                        for entry in thing:
                            #print entry.tag
                            if entry.tag == "Run":
                                current_run = int(entry.text)
                                if not current_run in self.RunDict.iterkeys():
                                    self.RunDict[current_run] = []
                            elif entry.tag == "LBRange":
                                if current_run < 0:
                                    #print " Error: no run number parsed yet"
                                    continue
                                else:
                                    startblock = int(entry.attrib['Start'])
                                    endblock = int(entry.attrib['End'])
                                    self.RunDict[current_run]+=range(startblock,endblock+1)
                                    #self.RunDict[current_run]+=[[startblock,endblock+1]]
                                    #print "Found run %i: %i to %i "%(current_run,startblock,endblock)
        #from pprint import pprint
        #pprint (self.RunDict)
        print "\nCustom GRL reader is ready! Found valid LBs for runs "+", ".join(str(thing) for thing in self.RunDict.iterkeys())
        
    def HasRunLumiBlock(self,run,LB):
        try:
            if LB in self.RunDict[run]:
                return True
        except KeyError:
            pass
        return False
                
        
if __name__ == "__main__":
    handler = GRLHandler(["/ptmp/mpp/niko/MCP/temp/grl.xml"])
    print handler.HasRunLumiBlock(271595,333)
    print handler.HasRunLumiBlock(271595,1)
        