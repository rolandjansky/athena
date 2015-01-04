# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LumiGRLParser.py
#
# Utility to parse GRL XML file and provide access to useful information in python
#
import xml.etree.cElementTree as ET

class LumiGRLParser:

    def __init__(self, grlfile=None):

        # Holds the element tree for the parsed XML file
        self.tree = None

        # Name of NamedLumiRange to consider (None works if only one NamedLumiRange is present) 
        self.lumiRangeName = None
        self.lumiRange = None

        # List of (integer) run numbers
        self.runList = []

        # List of COOL IOV ranges (start, end)
        # run << 32 & LB
        self.iovList = []

        # Dictionary keyed by runnum containing a list of IOV ranges
        self.iovDict = dict()
        
        if grlfile != None:
            self.parseFile(grlfile)


    def parseFile(self, filename):

        self.tree = ET.parse(filename)

        self.runList = []
        self.iovList = []
        self.iovDict = dict()
        
        # Return list of runs in GRL
        if not self.selectRange():
            print 'LumiGRLParser.parseFile(%s) - Error selecting valid range!' % filename
            return

        # Now, go through this range and find run numbers
        for lbc in self.lumiRange.findall('LumiBlockCollection'):
            runnum = int(lbc.findtext('Run', '-1'))
            if runnum < 0:
                print "LumiGRLParser.parseFile(%s) - Couldn't find Run in valid LumiBlockCollection!" % filename
                continue

            self.runList.append(runnum)
            self.iovDict[runnum] = []
            
            for lbr in lbc.findall('LBRange'):
                lbstart = int(lbr.get('Start', '-1'))
                lbend = int(lbr.get('End', '-1'))

                if lbstart < 0 or lbend < 0:
                    print "LumiGRLParser.parseFile(%s) - Couldn't find LBRange attributes for run %d!" % (filename, runnum)
                    continue

                # Must add one to make IOVRange exclusive at end
                lbend += 1
                self.iovDict[runnum].append((lbstart, lbend))

                iovstart = (runnum<<32)+lbstart
                iovend = (runnum<<32)+lbend
                self.iovList.append((iovstart, iovend))
                
        # Finally sort these
        self.runList.sort()
        self.iovList.sort()
        
    def selectRange(self):

        self.lumiRange = None
        
        if self.tree == None:
            print 'LumiGRLParser.selectRange() - no tree found!'
            return False

        lumiRangeList = self.tree.findall('NamedLumiRange')
        if len(lumiRangeList) == 1:

            if self.lumiRangeName != None:
                name = lumiRangeList[0].findtext('Name', '')

                if name != self.lumiRangeName:
                    print "LumiGRLParser.selectRange() - Can't find %s in GRL, only %s!" % (self.lumiRangeName, name)
                    return False
                
            self.lumiRange = lumiRangeList[0]
            return True

        elif len(lumiRangeList) == 0:

            print 'LumiGRLParser.selectRange() - No NamedLumiRange object found!'
            return False

        # More than one range found
        if self.lumiRangeName == None:
            print 'LumiGRLParser.selectRange() - %d NamedLumiRange objects found, but no lumiRangeName specified!' % len(lumiRangeList)
            return False
        
            
        # More than one, match by name
        found = False
        for lbr in lumiRangeList:
            name = lbr.findtext('Name', '')
            
            if self.lumiRangeName == name:
                found = True
                self.lumiRange = lbr
                break

        if not found:
            print "LumiGRLParser.selectRange() - Couldn't find %s in available NamedLumiRange objects!" % self.lumiRangeName
            return False

        return True

    # Return sorted list of lumi blocks present in specified run 
    def lumiBlockList(self, runnum):

        lbList = []
        for iov in self.iovDict.get(runnum, []):
            lbList.extend(range(iov[0], iov[1]))

        return lbList    
        
