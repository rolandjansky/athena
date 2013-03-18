# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonAna/python/Rtt.py
# @purpose: a set of classes to interact with RTT
# @author: Sebastien Binet <binet@cern.ch>
# $Id: Rtt.py,v 1.2 2007-07-19 18:27:49 binet Exp $

""" a set of classes to interact with RTT
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.2 $"
__doc__     = "a set of classes to interact with RTT"

import os, sys

class RttHelper:
    """A set of helpers to deal with RTT XML summary files"""

    @staticmethod
    def getText(nodeList):
        return "".join( [ node.data for node in nodeList.childNodes \
                          if node.nodeType == node.TEXT_NODE ] )

    @staticmethod
    def val(path, element):
        paths = path.split('/')
        curGenEls = [element]
        for p in paths:
            nextGenEls = []
            if p == 'text()':
                texts = []
                [texts.extend(RttHelper.getText(e)) for e in curGenEls]
                text = reduce(lambda x,y:x+y, texts,'')
                return text
            else:
                [nextGenEls.extend(e.childNodes) for e in curGenEls]
                nextGenEls = [n for n in nextGenEls if n.nodeName == p]
            curGenEls = nextGenEls
        return curGenEls.strip()

    @staticmethod
    def makeDict(node):
        elements = [ elem for elem in node.childNodes \
                     if elem.nodeType == elem.ELEMENT_NODE ]
        d = { }
        for elem in elements:
            d[elem.tagName.strip()] = RttHelper.val('text()', elem)
        return d
            
class Rtt(object):
    Home = "/afs/cern.ch/atlas/project/RTT"
    Nightlies = [ 'bugfix', 'val', 'mig0', 'dev' ]
    Builds    = [ '10', '11', '12', '13' ]
    Platforms = [ 'slc3', 'slc4' ]

    RelNbrKey   = "rel_nbr"
    RelTypeKey  = "rel_type"
    PlatformKey = "platform"

    RelNbr   = "%("+RelNbrKey+")s"
    RelType  = "%("+RelTypeKey+")s"
    Platform = "%("+PlatformKey+")s"


    # template to generate a path to results
    path = os.path.join( Home, "Results", RelNbr, RelType, "build",
                         Platform, "offline" )
    
class RttDb(object):
    """A naive repository of RTT test packages"""

    def __init__(self):
        object.__init__(self)

        self.dbName  = os.path.expanduser(
            os.path.join( "~", ".perfrtt", "rttPkgs.db" )
            )

        self.updatePkgDb()

    def updatePkgDb(self):

        if not os.path.exists( os.path.dirname(self.dbName) ):
            os.mkdir( os.path.dirname(self.dbName) )
            return self.createPkgDb()

        if not os.path.exists( self.dbName ):
            return self.createPkgDb()

        rootXml = os.path.join( Rtt.Home, 'Results', 'page1.xml' )

        # compare last modification of our repository and the
        # master RTT file from which that repository was built
        if os.path.getmtime( self.dbName ) < os.path.getmtime( rootXml ):
            return self.createPkgDb()

        return
    
    def createPkgDb(self):
        if not os.path.exists( os.path.dirname(self.dbName) ):
            os.mkdir( os.path.dirname(self.dbName) )
            
        val = RttHelper.val
        
        # get list of releases, nightlies...
        rootXml = open( os.path.join( Rtt.Home, 'Results', 'page1.xml' ), 'r' )
        import xml.dom.minidom as Xml
        dom = Xml.parse(rootXml)
        rootXml.close()

        releases = []
        for rel in dom.getElementsByTagName('overview'):
            relDb = {
                'name'    : val('releaseName/text()',    rel).strip(),
                'type'    : val('originalBranch/text()', rel).strip(),
                'nbr'     : val('release/text()',        rel).strip(),
                'results' : val('resReleaseDir/text()',  rel).strip(),
                'platform': val('targetCMTCONFIG/text()',rel).strip(),
                }
            ## we want 'bugfix/rel_2' instead of 'rel_2/bugfix'
            ## so it better meshes with the Athena AFS install naming scheme
            if relDb['name'].count('/') > 0:
                relDb['name'] = relDb['name'].split('/')
                relDb['name'].reverse()
                relDb['name'] = "/".join( relDb['name'] )
                
            releases.append( relDb )

        pkgs = set()
        jobDb = {}
        idDups = {}
        releases.sort( lambda x,y: cmp(x['name'],y['name']) )
        for r in releases:
            print "%10s" % r['type'],"%10s" % r['nbr'],"\t\t",r['name'],
            print r['platform']
##             if r['type'] != 'bugfix':
##                 continue
            sumFileName = os.path.join(r['results'], 'RTTSummary.xml')
            summary = Xml.parse( open(sumFileName, 'r') )
            for pkgNode in summary.getElementsByTagName('package'):
                pkgName = val( 'packageName/text()', pkgNode ).strip()
                pkgs.add( pkgName )
                for m in pkgNode.getElementsByTagName('minder'):
                    pkg = {
                        'pkgName' : pkgName,
                        'jobId'   : int(val('jobID/text()',      m).strip()),
                        'idName'  : val('identifiedName/text()', m).strip(),
                        'results' : val('resultsPath/text()',    m).strip(),
                        'jobName' : val('jobName/text()',        m).strip(),
                        'docStr'  : val('jobDocString/text()',   m).strip(),
                        'status'  : val('status/text()',         m).strip(),
                        }
                    res = pkg['results']
                    res = res.replace( "/%s/" % r['nbr'],
                                       "/"+Rtt.RelNbr+"/" )
                    res = res.replace( "/%s/" % r['type'],
                                       "/"+Rtt.RelType+"/" )
                    res = res.replace( "/%s/" % r['platform'],
                                       "/"+Rtt.Platform+"/" )
                    pkg['path'] = res
                    jobId = pkg['jobId']
                    if res.count(Rtt.RelType) <= 0:
                        continue
                    
                    if jobDb.has_key(jobId):
                        o = jobDb[jobId]
                        if o['idName'] != pkg['idName']:
                            if not idDups.has_key(jobId):
                                idDups[jobId] = []
                            if pkg['results'] not in idDups[jobId]:
                                idDups[jobId] += [pkg['results']]
                            if o['results'] not in idDups[jobId]:
                                idDups[jobId] += [o['results']]
                    else:
                        jobDb[jobId] = pkg
                    
        print "pkgs:",len(pkgs)
        for id in idDups.keys():
            print ">>> warning...",id
            idDups[id].sort()
            for r in idDups[id]:
                print "    ",r
        print "dups: %i/%i" % (len(idDups.keys()), len(jobDb.keys()))
        return
    
    pass # RttDb

       
