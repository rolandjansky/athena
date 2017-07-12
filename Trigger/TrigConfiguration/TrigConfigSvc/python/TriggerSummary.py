# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @author: Till Eifert <Till.Eifert@cern.ch>
# @date:   October 2008
#
# Note, parts of this file are copied from S. Binet's PoolFile.py

__version__ = "$Revision: 1.2 $"
__author__  = "Till Eifert <Till.Eifert@cern.ch>"

import sys, os
import shelve, whichdb


class PoolOpts(object):
    FAST_MODE   = False
    READ_MODE   = "READ"
    POOL_HEADER = "POOLContainer_"
    EVENT_DATA  = "CollectionTree"
    META_DATA   = "MetaData"
    HDR_FORMAT  = "%5s %5s     %-15s %s"
    ROW_FORMAT  = "%5i  %4s   %-30s   %5i     %5i          %5i"
    ROW_FORMAT_GLOBAL = "       %4s   %-30s   %5i"

    L1_GLOBAL   = "Global LVL1"
    L2_GLOBAL   = "Global LVL2"
    EF_GLOBAL   = "Global EF (LVL3)"

    level = ['None', 'LVL1', 'LVL2', 'EF']

    @classmethod
    def isData(cls, name):
        return ( len(name) >= len("##") and name[:2] != "##" ) and \
               ( name != PoolOpts.POOL_HEADER )

    @classmethod
    def isDataHeader(cls, name):
        return name == PoolOpts.POOL_HEADER+"DataHeader"

    @classmethod
    def isEventData(cls, name):
        return len(name) >= len(PoolOpts.EVENT_DATA) and \
               name[:len(PoolOpts.EVENT_DATA)] == PoolOpts.EVENT_DATA

    pass # class PoolOpts



class PoolRecord(object):
    """
    """
    class Sorter:
        name    = "name"
        counter = "counter"
        evtsBeforePS = "evtsBeforePS"
        evtsAfterPS = "evtsAfterPS"
        evtsAfterPT = "evtsAfterPT"

        @staticmethod
        def allowedValues():
            return [ PoolRecord.Sorter.name,
                     PoolRecord.Sorter.counter,
                     PoolRecord.Sorter.evtsBeforePS,
                     PoolRecord.Sorter.evtsAfterPS,
                     PoolRecord.Sorter.evtsAfterPT,]
        pass
    def __init__(self, name, counter, evtsBeforePS, evtsAfterPS, evtsAfterPT, level):
        object.__init__(self)
        self.name          = name
        self.counter       = counter
        self.evtsBeforePS  = evtsBeforePS
        self.evtsAfterPS   = evtsAfterPS
        self.evtsAfterPT   = evtsAfterPT
        self.level         = level
        return


class PoolFile(object):
    """
    A simple class to retrieve informations about the content of a POOL file.
    It should be abstracted from the underlying technology used to create this
    POOL file (Db, ROOT,...).
    Right now, we are using the easy and loosy solution: going straight to the
    ROOT 'API'.
    """

    def __init__(self, fileName):
        object.__init__(self)

        self._fileInfos = None
        self.keys       = None
        self.dataHeader = None
        # trigger stuff
        self.trigConf   = None
        self.l1_global  = None
        self.l1_items   = []
        self.l2_global  = None
        self.l2_chains  = []
        self.ef_global  = None
        self.ef_chains  = []

        self.poolFile = None
        dbFileName = whichdb.whichdb( fileName )
        if not dbFileName in ( None, '' ):
            print "## opening file [%s]..." % str(fileName)
            db = shelve.open( fileName, 'r' )
            print "## opening file [OK]"
            report = db['report']
            self._fileInfos = report['fileInfos']

            self.l1_items   = report['l1_data']
            self.l2_chains  = report['l2_data']
            self.ef_chains  = report['ef_data']
            self.l1_global   = report['l1_global']
            self.l2_global   = report['l2_global']
            self.ef_global   = report['ef_global']
        else:
            print "## opening file [%s]..." % str(fileName)
            self.__openPoolFile( fileName )
            print "## opening file [OK]"
            self.__processFile()

        return

    def __openPoolFile(self, fileName):
        # hack to prevent ROOT from loading graphic libraries and hence bother
        # our fellow Mac users
        oldArgs = sys.argv
        sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
        print "## importing ROOT..."
        import ROOT
        print "## importing ROOT... [DONE]"
        # prevent ROOT from being too verbose
        #rootMsg = ShutUp()
        #rootMsg.mute()
        ROOT.gErrorIgnoreLevel = ROOT.kFatal
        #rootMsg.unMute()
        sys.argv = oldArgs

        poolFile = None
        try:
            #ROOT.TFile.SetReadStreamerInfo(False)
            poolFile = ROOT.TFile.Open( fileName, PoolOpts.READ_MODE )
        except Exception, e:
            rootMsg.unMute()
            print "## Failed to open file [%s] !!" % fileName
            print "## Reason:"
            print e
            print "## Bailing out..."
            raise IOError, "Could not open file [%s]" % fileName

        #rootMsg.unMute()

        if poolFile == None:
            print "## Failed to open file [%s] !!" % fileName
            msg = "Could not open file [%s]" % fileName
            raise IOError, msg

        self.poolFile = poolFile
        assert self.poolFile.IsOpen() and not self.poolFile.IsZombie(), "Invalid POOL file or a Zombie one" 
        self._fileInfos = {
            'name' : self.poolFile.GetName(),
            'size' : self.poolFile.GetSize()
            }
        return

    def __setupDataStructure(self):
        self.l1_global = PoolRecord( name=PoolOpts.L1_GLOBAL, counter=-1, evtsBeforePS=0, evtsAfterPS=0,
                                     evtsAfterPT=0, level=PoolOpts.level[1] )

        self.l2_global = PoolRecord( name=PoolOpts.L2_GLOBAL, counter=-1, evtsBeforePS=0, evtsAfterPS=0,
                                     evtsAfterPT=0, level=PoolOpts.level[2] )

        self.ef_global = PoolRecord( name=PoolOpts.EF_GLOBAL, counter=-1, evtsBeforePS=0, evtsAfterPS=0,
                                     evtsAfterPT=0, level=PoolOpts.level[3] )

        # access to L1 TriggerItems through trigConf.L1Items
        for (name,item) in self.trigConf.L1Items.items():
            self.l1_items += [PoolRecord(name=name, counter=item.ctpId(), evtsBeforePS=0, evtsAfterPS=0,
                                         evtsAfterPT=0, level=PoolOpts.level[1]) ]

        # access to HLT chains through trigConf.L1Items
        for (name,chain) in self.trigConf.HLTChains.items():
            if chain.level() == 'L2':
                self.l2_chains += [PoolRecord(name=name, counter=chain.chain_counter(), evtsBeforePS=0, evtsAfterPS=0, \
                                                  evtsAfterPT=0, level=PoolOpts.level[2]) ]
            elif chain.level() == 'EF':
                self.ef_chains += [PoolRecord(name=name, counter=chain.chain_counter(), evtsBeforePS=0, evtsAfterPS=0, \
                                                  evtsAfterPT=0, level=PoolOpts.level[3]) ]
            else:
                print "will ignore chain (%s, %i) because the trigger level (=%s) is not set to either 'L2' or 'EF'" \
                    % ( name, chain.chain_counter(), chain.level())


        return

    def __processFile(self):

        ## first we try to fetch the DataHeader
        treeNames = [k.GetName() for k in self.poolFile.GetListOfKeys()]
        if "POOLContainer" in treeNames:
            name  = "POOLContainer" # new format
        else:
            name  = "POOLContainer_DataHeader" # old format
        dhKey = self.poolFile.FindKey( name )
        nEntries = dhKey.ReadObj().GetEntries()
        self._fileInfos['entries'] = nEntries

        ## next, try to get the trigger configuration from the MetaData TTree
        print "## Reading trigger configuration from MetaData ..."
        from TrigConfigSvc.TriggerConfigARA import TriggerConfigARA
        try:
            self.trigConf = TriggerConfigARA(self.poolFile)
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            pass
        print "## Reading trigger configuration from MetaData ... [DONE]"

        ## get the transient tree
        print "## Creating transient collection tree  ..."
        import AthenaROOTAccess.transientTree
        def filter(dhe):
            if 'HLTResult' in dhe.token() or 'TrigDecision' in dhe.token() or 'EventInfo' in dhe.token():
                print '+', dhe.token()
                return dhe
            return False




        branchNames = {}
        transientTree = AthenaROOTAccess.transientTree.makeTree(self.poolFile, branchNames = branchNames, dhfilt=filter)
        print "## Creating transient collection tree  ... [DONE]"

        # figure out the name of the EventInfo branch (this may change from MC to MC and DATA)
        eventInfoBranchName = 'McEventInfo'
        possible = [ (b.GetClassName(), b.GetName()) for b in transientTree.GetListOfBranches() if b.GetClassName() == 'EventInfo']
        if len(possible) == 1:
            eventInfoBranchName = possible[0][1]
        del possible

        # from TrigSteering.Chain import Chain  #MN
        Chain = ROOT.HLT.Chain

        ## Branches that we're interested in: 
        EventInfo_br    = transientTree.GetBranch(eventInfoBranchName)        
        TrigDecision_br = transientTree.GetBranch("TrigDecision")

        print "## Looping over all events now ..."
        isFirstEvent = True
        for i in xrange(transientTree.GetEntries()):
            if EventInfo_br.GetEntry(i) <= 0 or TrigDecision_br.GetEntry(i) <= 0 :
                print ":: could not get entry #%i" % i
                break
            # update the trig conf. with these numbers:
            
            #eventID = eval('transientTree.'+eventInfoBranchName+'.event_ID()')
            ei = getattr(transientTree, eventInfoBranchName)
            eventID  = ei.event_ID()
            self.trigConf.update(eventID.run_number(), eventID.lumi_block())

            ## FIRST EVENT -> setup data structure
            if isFirstEvent:
                isFirstEvent = False
                self.__setupDataStructure()

            ## global trigger status
            l1 = transientTree.TrigDecision.getL1Result()
            l2 = transientTree.TrigDecision.getL2Result()
            ef = transientTree.TrigDecision.getEFResult()

            self.l1_global.evtsAfterPT += l1.isAccepted()
            self.l2_global.evtsAfterPT += l2.isAccepted()
            self.ef_global.evtsAfterPT += ef.isAccepted()

            ## LVL1 items
            for l1_item in self.l1_items:
                l1_item.evtsBeforePS += l1.isPassedBeforePrescale( l1_item.counter )
                l1_item.evtsAfterPS  += l1.isPassedAfterPrescale ( l1_item.counter )
                l1_item.evtsAfterPT  += l1.isPassedAfterVeto     ( l1_item.counter )

            ## LVL2 chains
            l2_chains = l2.getChainResult()
            l2Chains = {}
            for i in range(l2_chains.size()):
                aChain = Chain( l2_chains[i] )
                l2Chains[aChain.getChainCounter()] =  aChain

            for chain in self.l2_chains:
                if not l2Chains.has_key(chain.counter):
                    continue
                if l2Chains[ chain.counter ].chainPassedRaw():
                    chain.evtsBeforePS += 1
                    if not l2Chains[ chain.counter ].isPrescaled():
                        chain.evtsAfterPS  += 1
                if l2Chains[ chain.counter ].chainPassed():
                    chain.evtsAfterPT  += 1

            ## EF chains
            ef_chains = ef.getChainResult()
            efChains = {}
            for i in range(ef_chains.size()):
                aChain = Chain( ef_chains[i] )
                efChains[aChain.getChainCounter()] =  aChain

            for chain in self.ef_chains:
                if not efChains.has_key(chain.counter):
                    continue
                if efChains[ chain.counter ].chainPassedRaw():
                    chain.evtsBeforePS += 1
                    if not efChains[ chain.counter ].isPrescaled():
                        chain.evtsAfterPS  += 1
                if efChains[ chain.counter ].chainPassed():
                    chain.evtsAfterPT  += 1




        print "## Looping over all events ... [DONE]"

        return


    def fileInfos(self):
        return os.linesep.join( [
            "File:" + self._fileInfos['name'],
            "Size: %12.3f kb" % (self._fileInfos['size'] / 1e3),
            "Nbr Events: %i" % self._fileInfos['entries']
            ] )

    def checkFile(self, sorting = PoolRecord.Sorter.name):
        print self.fileInfos()
        print ""
        print "Trigger configuration summary:"
        print self.trigConf

        ## sorting data
        if sorting in PoolRecord.Sorter.allowedValues():
            import operator
            data1 = self.l1_items
            data1.sort(key = operator.attrgetter(sorting) )
            data2 = self.l2_chains
            data2.sort(key = operator.attrgetter(sorting) )
            data3 = self.ef_chains
            data3.sort(key = operator.attrgetter(sorting) )

        print ""
        print "="*80
        print PoolOpts.HDR_FORMAT % ( "ID",
                                      "level",
                                      "Trigger name",
                                      "Passed events:   raw, after PS, after PT/Veto")
        print "="*80
        for item in [self.l1_global, self.l2_global, self.ef_global]:
            print PoolOpts.ROW_FORMAT_GLOBAL % (
                item.level,
                item.name,
                item.evtsAfterPT
                )

        print "-"*80
        for item in self.l1_items:
            print PoolOpts.ROW_FORMAT % (
                item.counter,
                item.level,
                item.name,
                item.evtsBeforePS, item.evtsAfterPS,
                item.evtsAfterPT
                )
        print "-"*80
        for chain in self.l2_chains:
            print PoolOpts.ROW_FORMAT % (
                chain.counter,
                chain.level,
                chain.name,
                chain.evtsBeforePS, chain.evtsAfterPS,
                chain.evtsAfterPT
                )
        print "-"*80
        for chain in self.ef_chains:
            print PoolOpts.ROW_FORMAT % (
                chain.counter,
                chain.level,
                chain.name,
                chain.evtsBeforePS, chain.evtsAfterPS,
                chain.evtsAfterPT
                )
        print "="*80

        return

    def saveReport (self, fileName):
        """
        Save all the gathered informations into a python shelve or a CSV file
        (depending on the @param `fileName` extension)
        """
        import os
        if os.path.splitext(fileName)[-1] == '.csv':
            return self._save_csv_report (fileName)
        return self._save_shelve_report (fileName)

    def _save_shelve_report(self, fileName):
        """
        Save all the gathered informations into a python shelve
        Data can then be read like so:
         >>> import shelve
         >>> db = shelve.open( 'myfile.dat', 'r' )
         >>> report = db['report']
         >>> print 'fileSize:',report['fileSize']
         >>> print 'dataHeader/memSize:',report['dataHeader'].memSize
         >>> for d in report['data']:
         ...   print 'data:',d.name,d.nEntries,d.memSize
        """
        import shelve, os
        if os.path.exists (fileName):
            os.unlink (fileName)
        db = shelve.open (fileName)
        db['report'] = {
            'fileInfos'  : self._fileInfos,
            'l1_global'       : self.l1_global,
            'l2_global'       : self.l2_global,
            'ef_global'       : self.ef_global,
            'l1_data'       : self.l1_items,
            'l2_data'       : self.l2_chains,
            'ef_data'       : self.ef_chains
            }
        db.close()
        return

    def _save_csv_report(self, fileName):
        """
        Save all the gathered informations into a CSV file
        """
        import csv, os
        if os.path.exists (fileName):
            os.unlink (fileName)
        o = csv.writer (open (fileName, 'w'))
        nentries = self._fileInfos['entries']
        map (o.writerow,
             [ ['file name', self._fileInfos['name']],
               ['file size', self._fileInfos['size']],
               ['nbr evts',  self._fileInfos['entries']],
               ['counter', 'level', 'name', 'evtsBeforePS',
                'evtsAfterPS', 'evtsAfterPT'],
               ])
        map (o.writerow,
             [ [d.counter, d.level, d.name,
                d.evtsBeforePS, d.evtsAfterPS, d.evtsAfterPT]
               for d in self.l1_items ])
        map (o.writerow,
             [ [d.counter, d.level, d.name,
                d.evtsBeforePS, d.evtsAfterPS, d.evtsAfterPT]
               for d in self.l2_chains ])
        map (o.writerow,
             [ [d.counter, d.level, d.name,
                d.evtsBeforePS, d.evtsAfterPS, d.evtsAfterPT]
               for d in self.ef_chains ])
        return



    def __del__(self):
        if self.poolFile and hasattr(self.poolFile, 'Close'):
            try:
                self.poolFile.Close()
                self.poolFile = None
            except Exception,err:
                print "WARNING:",err
                pass

    pass # class PoolFile
