# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


from xml.dom import minidom
import os, re, sys
from builtins import range
from past.builtins import cmp

from CoolConvUtilities.AtlCoolLib import indirectOpen
from PyCool import cool

class CurrentMapping:
    def __init__(self, fn, levels):
        self.filename = fn
        self.levels = levels # can be 'L1', 'HLT', ''
        if not fn:
            self.firstrun = 0
            self.lastrun  = 0 
            self.l1map    = {}
            self.l2map    = {}
            self.efmap    = {}
        else:
            m = re.match(r'%sChainTagMap_(\d*)-(\d*).xml$' % self.levels,fn)
            self.firstrun = int(m.group(1))
            self.lastrun  = int(m.group(2))
            self.readFromFile(fn)

    def deleteFile(self):
        if self.filename:
            os.remove(self.filename)
            self.filename=None
            
    def writeToFile(self):
        # delete self.filename()
        doc = minidom.Document()
        tagmap = doc.createElement('TAGMAP')
        doc.appendChild(tagmap)
        if self.levels == 'L1':
            lvlname = ['L1']
            lvlmap  = [self.l1map]
        elif self.levels == 'HLT':
            lvlname = ['L2','EF']
            lvlmap  = [self.l2map,self.efmap]
        else:
            lvlname = ['L1','L2','EF']
            lvlmap  = [self.l1map,self.l2map,self.efmap]
            
        for l in range(len(lvlname)):
            lvl = doc.createElement('LEVEL')
            tagmap.appendChild(lvl)
            lvl.setAttribute('level',lvlname[l])
            items = lvlmap[l].items()
            items.sort(lambda x,y: cmp(x[1],y[1]))
            for item,chan in items:
                me = doc.createElement('MAPELEMENT')
                lvl.appendChild(me)
                me.setAttribute('chainname',item)
                me.setAttribute('tagindex',str(chan))
        FH = open('%sChainTagMap_%08i-%08i.xml' % (self.levels, self.firstrun, self.lastrun),'w')
        FH.write(doc.toprettyxml())
        doc.unlink()

    def readFromFile(self, fn):
        doc = minidom.parse(fn)
        for l in doc.getElementsByTagName('LEVEL'):
            level = l.attributes['level'].value
            if level == 'L1':   themap = self.l1map = {}
            elif level == 'L2': themap = self.l2map = {}
            elif level == 'EF': themap = self.efmap = {}
            for e in l.getElementsByTagName('MAPELEMENT'):
                themap[e.attributes['chainname'].value] = int(e.attributes['tagindex'].value)
        doc.unlink()




class ChainTagMapTool:
    def __init__(self, options):
        self.currentMapping = None
        self.opt = options
        self.dir = self.opt.dir
        self.verbose = self.opt.verbosity
        self.split = self.opt.split
        self.mode = 'default'
        
    @staticmethod
    def GetConnection(dbconn,verbosity=0):
        connection = None
        if dbconn=="COMP":   connection = 'COOLONL_TRIGGER/COMP200'
        elif dbconn=="OFLP": connection = 'COOLONL_TRIGGER/OFLP200'
        else: raise RuntimeError ("Can't connect to COOL db %s" % dbconn)
        try:
            openConn = indirectOpen(connection,readOnly=True,oracle=True,debug=(verbosity>0))
        except Exception:
            import traceback
            traceback.print_exc()
            sys.exit(-1)
        return openConn


    def getL1MapsForRuns(self,db, runrange):
        limmin=runrange[0]<<32
        limmax=(runrange[1]+1)<<32
        f = db.getFolder( "/TRIGGER/LVL1/Menu" )
        chansel=cool.ChannelSelection.all(cool.ChannelSelection.sinceBeforeChannel)
        objs = f.browseObjects( limmin,limmax,chansel)
        l1map = {}
        while objs.goToNext():
            obj=objs.currentRef()
            runNr = obj.since()>>32
            if runNr not in l1map: l1map[runNr] = {}
            itemname = obj.payload()['ItemName']
            l1map[runNr][itemname] = obj.channelId()
        return l1map

    def getHLTMapsForRuns(self,db, runrange):
        limmin=runrange[0]<<32
        limmax=(runrange[1]+1)<<32
        f = db.getFolder( "/TRIGGER/HLT/Menu" )
        chansel=cool.ChannelSelection.all(cool.ChannelSelection.sinceBeforeChannel)
        objs = f.browseObjects( limmin,limmax,chansel)
        l2map = {}
        efmap = {}
        while objs.goToNext():
            obj=objs.currentRef()
            pl = obj.payload()
            chainCounter = pl['ChainCounter']
            if chainCounter>1023: continue
            runNr = obj.since()>>32
            if runNr not in l2map: l2map[runNr] = {}
            if runNr not in efmap: efmap[runNr] = {}
            itemname = pl['ChainName']
            if pl['TriggerLevel'] == 'L2':
                l2map[runNr][itemname] = chainCounter
            else:
                efmap[runNr][itemname] = chainCounter
        return l2map, efmap

    @staticmethod
    def mergelists(l1,l2):
        l = l1 + [x for x in l2 if x not in l1]
        l.sort()
        return l




    def buildMap(self):
        if self.split:
            self.buildMapByLevel('L1')
            self.buildMapByLevel('HLT')
        else:
            self.buildMapByLevel('')

    def buildMapByLevel(self,levels):
        filename = self.getLastFilename(levels)
        self.currentMapping = CurrentMapping(filename,levels)

        lastNewRun = 100000000
        if self.opt.run: lastNewRun = int(self.opt.run)
        rr = [self.currentMapping.lastrun+1,lastNewRun]
        #rr = [112927,112965]


        if self.verbose>0:
            print ("Analyzing runs %i to %i" % (rr[0],rr[1]))

        # get the maps from COOL
        if self.verbose>0:
            print ("Reading from COOL DB")
        dbconn = ChainTagMapTool.GetConnection('COMP')
        if levels=='' or levels=='L1':
            l1maps = self.getL1MapsForRuns(dbconn,rr)
        if levels=='' or levels=='HLT':
            (l2maps, efmaps) = self.getHLTMapsForRuns(dbconn,rr)
        dbconn.closeDatabase()
        
        if self.verbose>0:
            print ("Analyzing runs")
        if levels=='L1':
            runs = l1maps.keys()
            runs.sort()
        elif levels=='HLT':
            runs = l2maps.keys()
            runs.sort()
        else:
            runs = ChainTagMapTool.mergelists(l1maps.keys(),l2maps.keys())

        for run in runs:
            if self.verbose>1:
                if self.verbose>2:
                    print ("Analyzing run %8i" % run)
                elif run%5000==0:
                    print ("Analyzing run %8i" % run)

            l1map=None
            l2map=None
            efmap=None
            if levels=='' or levels=='L1':
                l1map = l1maps[run] if run in l1maps else {}
            if levels=='' or levels=='HLT':
                l2map = l2maps[run] if run in l2maps else {}
                efmap = efmaps[run] if run in efmaps else {}

            self.checkConsistency(levels, l1map, l2map, efmap)

            self.updateMaps(l1map, l2map, efmap)

            self.currentMapping.lastrun = run
            if self.currentMapping.firstrun == 0:
                self.currentMapping.firstrun = run
        self.closeCurrentMap(levels)

    def checkConsistency(self, levels, newl1map, newl2map, newefmap):
        """Checks if the currentmap and the new map are consistent.
        If an inconsistency is found, the current map is closed (written to file and set to None)
        """
        isConsistent  = True
        if levels=='' or levels=='L1':
            isConsistent &= self.checkMapConsistency(self.currentMapping.l1map,newl1map)
        if levels=='' or levels=='HLT':
            isConsistent &= self.checkMapConsistency(self.currentMapping.l2map,newl2map)
            isConsistent &= self.checkMapConsistency(self.currentMapping.efmap,newefmap)
        if not isConsistent:
            self.closeCurrentMap(levels)


    def checkMapConsistency(self,oldmap,newmap):
        if not newmap: return True
        if self.mode == 'default' or self.mode == 'loose':
            for name,counter in newmap.items():
                samecounter = [(c,n,name) for n,c in oldmap.items() if (c==counter and n!=name)]
                if samecounter:
                    print ("Need to close current map '%i-%i', because counter %i has changed trigger (%s -> %s)" %
                           ((self.currentMapping.firstrun,self.currentMapping.lastrun) + samecounter[0]))
                    return False
            return True
        return True
                
    def updateMaps(self, newl1map, newl2map, newefmap):
        if newl1map:
            self.currentMapping.l1map.update(newl1map)
        if newl2map:
            self.currentMapping.l2map.update(newl2map)
        if newefmap:
            self.currentMapping.efmap.update(newefmap)

    def closeCurrentMap(self,levels):
        self.currentMapping.deleteFile()
        self.currentMapping.writeToFile()
        self.currentMapping = CurrentMapping(None,levels)



    def getLastFilename(self, levels):
        files = os.listdir(self.dir)
        lastrun = 0
        lastfile = None
        for fn in files:
            m = re.match(r'%sChainTagMap_\d*-(\d*).xml$' % levels,fn)
            if not m: continue
            endrun = int(m.group(1))
            if endrun>lastrun:
                lastrun=endrun
                lastfile=fn
        return lastfile
