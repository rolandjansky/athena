#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# TileBchTools.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-12-17
#
#
# Edition:
# Andrey Kamenshchikov, 23-10-2013 (akamensh@cern.ch)
################################################################
"""
Python module for managing TileCal ADC status words.

"""

import PyCintex
from TileCalibBlobObjs.Classes import *
from TileCalibBlobPython import TileCalibTools
from TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger

#
#______________________________________________________________________
class TileBchMgr(TileCalibLogger):
    """
    This class manages updates to the Tile Calorimeter bad channel database.
    The usual mode of operation should start with initializing this manager
    with a current set of bad channels from an existing database.
    The status of individual ADCs can then be modified using the setAdcStatus,
    updateFromFile or updateFromDb methods.
    In a final step, the changes are commited to the database using the commit
    method.
    """
    #____________________________________________________________________
    def __init__(self):

        #=== initialize base class
        TileCalibLogger.__init__(self,"TileBchMgr")

        #=== initialize all channel status to "good"
        self.__newStat = (self.__getAdcIdx(4,63,47,1) + 1) * [TileBchStatus()]
        self.__oldStat = (self.__getAdcIdx(4,63,47,1) + 1) * [TileBchStatus()]
        self.__comment = ""


    #____________________________________________________________________
    def __getAdcIdx(self, ros, drawer, channel, adc):
        """
        Private function, calculating the index of a given ADC
        for the internal cache.
        """
        return TileCalibUtils.getAdcIdx(ros,drawer,channel,adc)

    #____________________________________________________________________
    def __updateFromDb(self, db, folderPath, tag, runLumi, fillNewStat=True):
        """
        Updates the internal bad channel cache with the content
        found in the database. An open database instance (db) has to
        be provided. Tag and runNum are used to locate the set of
        bad channels to be read from the database.
        """

        #=== try to open the db
        try:
            if not db.isOpen():
                raise "DB not open: ", db.databaseId()
        except Exception, e:
            self.log().critical( e )
            return

        #=== print status information
        reader = TileCalibTools.TileBlobReader(db,folderPath,tag)
        self.log().info("Updating dictionary from \'%s\'" % db.databaseName())
        self.log().info("... using tag \'%s\', run-lumi=%s" % (tag,runLumi))
        self.__comment = reader.getComment(runLumi)
        self.log().info("... comment: %s" % self.__comment)

        #=== loop over the whole detector
        for ros in xrange(0,TileCalibUtils.max_ros()):
            for mod in xrange(TileCalibUtils.getMaxDrawer(ros)):
                bch = reader.getDrawer(ros, mod, runLumi)
                if bch is None:
                    self.log().warning("Missing IOV in condDB: ros=%i mod=%i runLumi=%s" % (ros,mod,runLumi))
                    continue
                bchDecoder = TileBchDecoder(bch.getBitPatternVersion())
                for chn in xrange(TileCalibUtils.max_chan()):
                    for adc in xrange(TileCalibUtils.max_gain()):
                        #=== adc bits
                        adcBits = bch.getData(chn,adc,0)
                        #=== channel bits (works always due to default policy)
                        chnBits = bch.getData(chn,  2,0)
                        #=== build status from both adc and channel bits
                        status = TileBchStatus( bchDecoder.decode(chnBits,adcBits) )
                        if fillNewStat:
                            self.__newStat[self.__getAdcIdx(ros,mod,chn,adc)] = status
                        else:
                            self.__oldStat[self.__getAdcIdx(ros,mod,chn,adc)] = status

    #____________________________________________________________________
    def getComment(self):                            
        return self.__comment

    #____________________________________________________________________
    def updateFromDb(self, db, folderPath, tag, runLumi):
        self.__updateFromDb(db, folderPath, tag, runLumi)

    #____________________________________________________________________
    def initialize(self, db, folderPath, tag="", runLumi=(MAXRUN,MAXLBK-1)):
        """
        Initializes the internal bad channel cache. Any changes applied to the
        cache previous to calling this function are lost. Typically this function
        is called once in the beginning to initialize the cache with a set of
        current bad channels.
        """
        self.log().info("Initializing from database, resetting all changes!")
        self.updateFromDb(db,folderPath,tag,runLumi)
        #=== initialize reference to current status
        for idx in xrange(len(self.__newStat)):
            self.__oldStat[idx] = self.__newStat[idx]

        #=== update TileBchStatus::isBad() definition from DB
        self.log().info("Updating TileBchStatus::isBad() definition from DB")
        status = self.__newStat[self.__getAdcIdx(0,1,0,0)]
        if status.isGood():
            self.log().info("No TileBchStatus::isBad() definition found in DB, using defaults")
        else:    
            TileBchStatus.defineBad(status)

    #____________________________________________________________________
    def getAdcStatus(self, ros, drawer, channel, adc):
        """
        Get TileBchStatus for a given ADC.
        """
        return self.__newStat[self.__getAdcIdx(ros,drawer,channel,adc)]

    #____________________________________________________________________
    def setAdcStatus(self, ros, drawer, channel, adc, status):
        """
        Set TileBchStatus for a given ADC.
        """
        self.__newStat[self.__getAdcIdx(ros,drawer,channel,adc)] = status

    #____________________________________________________________________
    def getAdcProblems(self, ros, drawer, channel, adc):
        """
        Returns a dictionary with { problemEnum : 'Description'}
        """
        prbDescDict = {}
        status = self.getAdcStatus(ros,drawer,channel,adc)
        if not status.isGood():
            prbs = status.getPrbs()
            for prb in prbs:
                prbDescDict[prb] = TileBchPrbs.getDescription(prb)
        return prbDescDict


    #____________________________________________________________________
    def setAdcProblems(self, ros, drawer, channel, adc, problems):
        """
        Expects a list of TileBchPrbs::PrbS as input
        """
        status = TileBchStatus()
        for prb in problems:
            status += prb
        self.setAdcStatus(ros,drawer,channel,adc,status)

    #____________________________________________________________________
    def addAdcProblem(self, ros, drawer, channel, adc, problem):
        """
        Sets a specific problem
        """
        status = self.getAdcStatus(ros,drawer,channel,adc)
        status += problem
        self.setAdcStatus(ros,drawer,channel,adc,status)

    #____________________________________________________________________
    def delAdcProblem(self, ros, drawer, channel, adc, problem):
        """
        Removes a specific problem
        """
        status = self.getAdcStatus(ros,drawer,channel,adc)
        status -= problem
        self.setAdcStatus(ros,drawer,channel,adc,status)

    #____________________________________________________________________
    def listBadAdcs(self, rosBeg=0, modBeg=0, rosEnd=5, modEnd=64):
        """
        Print a formatted list of all ADCs with problems.
        """
        self.log().info("==============================================================")
        self.log().info("                 Current list of affected ADCs               " )
        self.log().info("==============================================================")
        for ros in xrange(rosBeg,rosEnd):
            for mod in xrange(modBeg, min(modEnd,TileCalibUtils.getMaxDrawer(ros))):
                modName = TileCalibUtils.getDrawerString(ros,mod)        
                for chn in xrange(TileCalibUtils.max_chan()):
                    chnName = "channel %2i" % chn
                    for adc in xrange(TileCalibUtils.max_gain()):
                        gainName = "LG:"
                        if adc:
                            gainName = "HG:"
                        prbs = self.getAdcProblems(ros,mod,chn,adc)
                        for prbCode in sorted(prbs.keys()):
                            prbDesc = prbs[prbCode]
                            msg = "%s %s %s %2i (%s)" % (modName,chnName,gainName,prbCode,prbDesc)
                            self.log().info( msg )
                            modName  = " " *  5 
                            chnName  = " " * 10
                            gainName = " " *  3 
        self.log().info("==============================================================")

    #____________________________________________________________________
    def checkModuleForChanges(self, ros, drawer):
        """
        Returns:
        - if nothing changed                                    :  0 
        - if something changed and complete drawer is now good  : -1
        - if something changed but drawer is not completely good: >0 
        """
        diffCnt = 0
        allGood = True
        for chn in xrange(TileCalibUtils.max_chan()):
            for adc in xrange(TileCalibUtils.max_gain()):
                idx = self.__getAdcIdx(ros,drawer,chn,adc)
                newStatus = self.__newStat[idx]
                #=== count differences between old and new
                if newStatus!=self.__oldStat[idx]:
                    diffCnt+=1
                #=== invalidate allGood if one ADC is not good
                if not newStatus.isGood(): allGood = False
        if diffCnt>0 and allGood:
            return -1
        return diffCnt
        
                        
    #____________________________________________________________________
    def updateFromFile(self, fileName):
        """
        Updates the internal bad channel cache with the content
        found in the file. The layout of the file has to follow the
        TileConditions ASCII file layout.
        
        NGO: change this at some point. In a file, not the status word (which
        depends on the bit pattern version) should be encoded, but the individual problems (enums).
        For this we need one line per ADC... this requires some modification in the reader.
        """
        parser = TileCalibTools.TileASCIIParser(fileName,'Bch')
        dict = parser.getDict()
        self.log().info("Updating dictionary from file with %i entries" % len(dict))
        self.log().info("... filename: %s" % fileName )
        for key, stat in dict.iteritems():
            ros = key[0]
            mod = key[1]
            chn = key[2]
            for adc in xrange(2):
                status = TileBchStatus()
                status+=self.getAdcStatus(ros,mod,chn,adc)
                if adc < len(stat):
                    statInt = int(stat[adc])
                else:
                    statInt=0
                #=== temporary convention
                if statInt==0: pass
                elif statInt==1: status += TileBchPrbs.IgnoredInHlt
                else             : status += int(stat[adc])
                self.setAdcStatus(ros,mod,chn,adc,status)

    #____________________________________________________________________
    def commitToDb(self, db, folderPath, tag, bitPatVer, author, comment,
                   since, until=(MAXRUN,MAXLBK)):
        """
        Commits the differences compared to the set of bad channels read in with the
        initialze function to the provided database.
        - author  : author name (string)
        - comment : a comment (string)
        - sinceRun, sinceLbk : start of IOV for which bad channels are valid
        - untilRun, untilLbk : end   of IOV for which bad channels are valid
        """
        #=== check db status
        try:
            if not db.isOpen():
                raise "DB not open: ", db.databaseId()
        except Exception, e:
            raise( e )

        #=== get latest state from db
        if since != (MINRUN,MINLBK):
            justBefore = list(since)
            if justBefore[1]>MINLBK:
                justBefore[1] = justBefore[1]-1
            else:
                justBefore[0] = justBefore[0]-1
                justBefore[1] = MAXLBK                                
            justBefore = tuple(justBefore)
            self.log().info("Reading db state just before %s, i.e. at %s" % (since,justBefore))
            self.log().info("Reading db state at %s, i.e. at %s" % (since,justBefore))
            self.__updateFromDb(db, folderPath, tag, justBefore, False)
        else:
            self.log().info("Filling db from %s, resetting old status cache" % since)
            self.__oldStat = len(self.__oldStat) * [TileBchStatus()]

        #=== print status information
        self.log().info("Committing changes to DB \'%s\'" % db.databaseId())
        self.log().info("... using tag \'%s\' and [run,lumi] range: [%i,%i] - [%i,%i]"
                 % (tag,since[0],since[1],until[0],until[1]))
        self.log().info("... author : \'%s\'" % author  )
        self.log().info("... comment: \'%s\'" % comment )

        #=== default for drawer initialization
        loGainDefVec = PyCintex.gbl.std.vector('unsigned int')()
        loGainDefVec.push_back(0)
        hiGainDefVec = PyCintex.gbl.std.vector('unsigned int')()
        hiGainDefVec.push_back(0)
        comChnDefVec = PyCintex.gbl.std.vector('unsigned int')()
        comChnDefVec.push_back(0)
        PyCintex.makeClass('std::vector<unsigned int>')
        defVec = PyCintex.gbl.std.vector('std::vector<unsigned int>')()
        defVec.push_back(loGainDefVec)
        defVec.push_back(hiGainDefVec)
        defVec.push_back(comChnDefVec)
        
        #=== loop over the whole detector
        reader = TileCalibTools.TileBlobReader(db,folderPath,tag)
        writer = TileCalibTools.TileBlobWriter(db,folderPath,'Bch',reader.folderIsMultiVersion())
        writer.setComment(author, comment)
        bchDecoder = TileBchDecoder(bitPatVer)
        nUpdates = 0
        for ros in xrange(0,TileCalibUtils.max_ros()):
            for mod in xrange(TileCalibUtils.getMaxDrawer(ros)):
                nChange = self.checkModuleForChanges(ros,mod)
                if nChange == 0:
                    #=== do nothing if nothing changed
                    continue
                if nChange==-1:
                    nUpdates += 1
                    self.log().debug("Drawer (ros,mod) = (%i,%2i) reset to GOOD"%(ros,mod)) 
                    writer.zeroBlob(ros,mod)
                else:
                    nUpdates += 1
                    self.log().debug("Applying %2i changes to drawer (ros,mod) = (%i,%2i)"
                                    %(nChange,ros,mod)) 
                    drawer = writer.getDrawer(ros,mod)
                    drawer.init(defVec,TileCalibUtils.max_chan(),bitPatVer)
                    for chn in xrange(TileCalibUtils.max_chan()):
                        #=== get low gain bit words
                        wordsLo = bchDecoder.encode(self.getAdcStatus(ros,mod,chn,0))
                        chBits = wordsLo[0]
                        loBits = wordsLo[1]
                        #=== get high gain bit words
                        wordsHi = bchDecoder.encode(self.getAdcStatus(ros,mod,chn,1))
                        chBits = wordsHi[0] | chBits
                        hiBits = wordsHi[1]
                        #=== set low, high and common channel word in calibDrawer
                        drawer.setData(chn,0,0, loBits)
                        drawer.setData(chn,1,0, hiBits)
                        drawer.setData(chn,2,0, chBits)

        #=== register
        if nUpdates>0:
            self.log().info("Attempting to register %i modified drawers..." % nUpdates)
            writer.register(since,until,tag)
        else:
            self.log().warning("No drawer modifications detected, ignoring commit request")
