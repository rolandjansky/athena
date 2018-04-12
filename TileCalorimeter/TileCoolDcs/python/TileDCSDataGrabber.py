#!/bin/env python
# Author: nils.gollub@cern.ch

import sys, os
import time
import logging
from PyCool import cool
import ROOT
from array import array
from sets import Set
import copy
import TileDCSDataInfo
from CoolConvUtilities.AtlCoolTool import connect
from ProgressBar import progressBar

#====================================================================================================
class IOVDict:
    """
    Class that stores time stamps and associated data. If data is requested for a time
    between registered time stamps, then the data at the smaller time stamp is returned.
    For overflow time request, the last data is returned.
    For underflow time requests, the first data is returned.

    It is mandatory that data is entered with strictly growing iovSince!
    """

    def __init__(self, putDuplicates=False):
        self.iovDict = {}
        self.iovList = []
        self.putDuplicates = putDuplicates

    def addData(self, iovSince, data):
        """
        Only add data with new iovSince if the list of data values changed
        """
        #=== check strict iovSince ordering
        if len(self.iovList):
            if iovSince < self.iovList[-1]:
                raise "ERROR: Data must be entered by strict iov order"
        #=== check if data has changed since last entry
        oldData = self.getData(iovSince)
        if data != oldData or self.putDuplicates:
            #print "Adding new data to store...: ", iovSince,"(",time.ctime(iovSince/1000000000),")", data
            self.iovDict[iovSince] = data
            if iovSince not in self.iovList:
                self.iovList.append(iovSince)


    def getData(self, iovTime):
        if len(self.iovList):
            key = self.__floorsearch(self.iovList, iovTime)
            return self.iovDict[key]
        else:
            return None

    def getIOVs(self):
        if not self.isSorted:
            self.iovList.sort()
            self.isSorted = True
        return iovList;

    def __binsearch(self, seq, search):
        right = len(seq)
        left = 0
        previous_center = -1
        if search < seq[0]:
            return -1
        while True:
            center = (left + right) / 2
            candidate = seq[center]
            if search == candidate:
                return center
            if center == previous_center:
                return - 2 - center
            elif search < candidate:
                right = center
            else:
                left = center
            previous_center = center

    def __floorsearch(self, seq, search):
        ind = self.__binsearch(seq,search)
        if ind>=0:
            return search
        elif ind==-1:
            #=== catch left out of bounds case
            return seq[0]
        else:
            return seq[-ind-2]



#====================================================================================================
class FolderVarSet:
    """
    Class that stored a set of variables. Groups of sets belonging to the same
    drawer and IOV are saved.
    """

    def __init__(self, variables, putDuplicates=False):

        #=== list to store the variable values
        self.val = len(variables) * [ 0 ]
        #=== dictionary of variable name to index and modified
        self.index = {}
        for ind in xrange(len(variables)):
            self.index[variables[ind]] = [ ind, False ]
        #=== dictionary of drawers to IOVDict objects
        self.iovDictStore = {}
        self.putDuplicates = putDuplicates


    def setVariable(self, variable, value):
        ind, mod = self.index[variable]
        self.val[ind] = value
        self.index[variable][1] = True


    def getVariable(self, variable):
        ind, mod = self.index[variable]
        return self.val[ind]


    def registerInIOVStore(self, drawer, iovSince):

        #=== check if all variables have been set
        notSet = []
        for var, stat in self.index.iteritems():
            mod = stat[1]
            if mod==False: notSet.append(var)
        if len(notSet):
            print "WARNING: Not all variables set: "
            for var in notSet: print var

        #=== register in IOVDict
        if drawer not in self.iovDictStore:
            self.iovDictStore[drawer] = IOVDict(self.putDuplicates)
        valueCopy = copy.deepcopy(self.val)
        self.iovDictStore[drawer].addData(iovSince, valueCopy)

        #=== reset all values and modification flags
        for var in self.index.iterkeys():
            self.index[var][1] = False
            self.val[ self.index[var][0] ] = 0


    def getFromIOVStore(self, drawer, iovSince):

        if drawer not in self.iovDictStore:
            raise "Drawer not known: ", drawer
        iovDict = self.iovDictStore[drawer]
        self.val = iovDict.getData(iovSince)


    def getUniqueIOVs(self):

        uniqueIOVList = []
        for iovDict in self.iovDictStore.itervalues():
            uniqueIOVList.extend( iovDict.iovList )
        return list(set(uniqueIOVList))

    def getUniqueVariables(self):

        uniqueVariableList = []
        for drawer in self.iovDictStore.iterkeys():
            for variable in self.index.iterkeys():
                uniqueVariableList.append( (drawer,variable) )
        return uniqueVariableList;

    def getVariables(self):
        return self.index.keys()

    def getDrawers(self):
        return self.iovDictStore.keys()



#====================================================================================================
class TileDCSDataGrabber:

    """
    Class to extract DCS data from the COOL database in two different ways:
    - selected variables will be extracted to a TTree in synchronization with
      time stamps in an input ntuple (getSyncDCSTree)
    - selected variables for a selection of drawers will be dumped to a TTree for
      a speciefied time range. The data between the different selected drawers is
      synchronized.
    """

    #________________________________________________________________________________________________
    def __init__( self, dbSource="COOL", logLevel=logging.WARNING, dbstring=None, putDuplicates=False, iovStartUnix=None):

        #=== setup loggging
        logging.basicConfig(level=logLevel, format='TileDCSDataGrabber %(levelname)-8s %(message)s')

        #=== connect to DB, depending on technology
        self.db = 0
        self.dbV = []
        self.info = TileDCSDataInfo.TileDCSDataInfo(dbstring)
        if dbSource=="COOL":
            if dbstring==None:
                self.dbV.append(0)
                self.dbV.append(0)
                self.dbV.append(0)
                self.dbV[1], connectString = connect( self.info.get_dbstring(dbSource,1) )
                self.dbV[2], connectString = connect( self.info.get_dbstring(dbSource,2) )
                self.db = self.dbV[2]
            else:
                self.db, connectString = connect( dbstring )
                self.dbV.append(self.db)
                self.dbV.append(self.db)
                self.dbV.append(self.db)
        elif dbSource=="ORACLE":
            self.iovStart = self.getOracleTimeString(iovStartUnix)
            self.oldTableBoundary = 131231230000
            if dbstring==None:
                dbstring = self.info.get_dbstring(dbSource, 1 if int(self.iovStart) < self.oldTableBoundary else 2)
            self.db = ROOT.TSQLServer.Connect( dbstring[0], dbstring[1], dbstring[2])
        elif dbSource=="TESTBEAM":
            if dbstring==None:
                dbstring = self.info.get_dbstring(dbSource,0)
            self.db = ROOT.TSQLServer.Connect( dbstring[0], dbstring[1], dbstring[2])
        else:
            raise("Unknown dbSource: %s, please use either COOL or ORACLE or TESTBEAM" % dbSource)

        self.dbCounter = 0
        self.queryCounter = 0
        self.maxQueryCounter = 100
        self.dbString = dbstring
        self.dbSource = dbSource
        self.unix2cool = 1000000000
        self.R2minTime = 1404165600 # 01-jul-2014 00:00:00 CEST
        self.putDuplicates = putDuplicates
        print """
        ##############################################################################
        #
        #     TileDCSDataGrabber   --   Access to TileCal DCS data in COOL / ORACLE
        #
        #         problems, suggestions, etc... :  andrey.ryzhov@cern.ch
        #
        ##############################################################################

        """
        print "Reading from",dbSource

    #________________________________________________________________________________________________
    def check_db(self, iovStartUnix, iovEndUnix, ar):

        if self.dbCounter==0:
            if iovStartUnix<self.R2minTime:
                self.dbCounter=1
                if iovEndUnix>self.R2minTime:
                    ar[0] = cool.ValidityKey(iovStartUnix*self.unix2cool)
                    ar[1] = cool.ValidityKey(self.R2minTime*self.unix2cool)
            else:
                self.dbCounter=2
        elif self.dbCounter==1:
            if iovEndUnix<=self.R2minTime:
                self.dbCounter=0
            else:
                self.dbCounter=2
                if iovStartUnix<self.R2minTime:
                    ar[0] = cool.ValidityKey(self.R2minTime*self.unix2cool)
                    ar[1] = cool.ValidityKey(iovEndUnix*self.unix2cool)
        else:
            self.dbCounter=0

        self.db = self.dbV[self.dbCounter]
        return self.dbCounter


    #________________________________________________________________________________________________
    def reconnect(self):

        if self.queryCounter == self.maxQueryCounter:
            if self.dbSource=="ORACLE" or self.dbSource=="TESTBEAM":
                print "Max connect counter reached, reconnecting to Databaase Server"
                self.db.Close()
                self.db = ROOT.TSQLServer.Connect( self.dbString[0], self.dbString[1], self.dbString[2] )
            self.queryCounter=1
        else:
            self.queryCounter+=1



    #________________________________________________________________________________________________
    def getSyncDCSTree(self, h2000, drawer, variables=[], timeVarName="EvTime"):
        """
        This function returns a tree containing the DCS variables in <variables>
        for the drawer <drawer> synchronized with the input tree <h2000>. The
        name of the time stamp used for synchronization is <timeVarName>.
        Input:  - h2000      : a TTree containing at least the variable <timeVarName>
                - drawer     : The drawer of interst in string from, i.e. \"LBC02\"
                - variables  : A python list containing all the variable names of
                               interest. By default all variables are attached.
                - timeVarName: name of the time stamp in h2000 tree
        """

        #=== avoid variable duplication
        if not len(variables):
            variables = self.info.get_all_variables()
        variables = list(set(variables))

        #=== sort variables by directories
        folders = self.info.get_variables_by_folder(variables, drawer)

        #=== prepare input tree for fast reading
        evTime = array('i', [0])
        h2000.SetBranchStatus("*",0)
        h2000.SetBranchStatus(timeVarName,1)
        h2000.SetBranchAddress(timeVarName, evTime)
        nEntries = h2000.GetEntries()

        #=== create the friend
        friendName=drawer
        t = ROOT.TTree(friendName,friendName)
        t.SetDirectory(0)
        buffer = []
        nVars = len(variables)
        previousVal = {}
        for iVar in xrange(nVars):
            var = variables[iVar]
            arrayType, treeType = self.info.get_variable_type(var)
            previousVal[var] = array(arrayType, [0])
            buffer.append(     array(arrayType, [0]) )
            t.Branch( var , buffer[iVar], var+treeType )


        #=== loop over input tree h2000
        timeBeg = -1
        timeEnd = -1
        bar = progressBar(0, nEntries, 78)
        for jentry in xrange( nEntries ):

            #=== load current entry
            ientry = h2000.LoadTree( jentry )
            if ientry < 0: break
            nb = h2000.GetEntry( jentry )
            if nb <= 0: continue
            bar.update(jentry)

            #=== access cool only if IOV changed
            if timeEnd==-1: timeBeg = evTime[0]
            if evTime[0]!=timeEnd:
                timeEnd=evTime[0]
                valKey = cool.ValidityKey(evTime[0]*self.unix2cool)
                for folder, varlist in folders.iteritems():
                    coolFolder = self.db.getFolder(folder)
                    channel = self.info.get_channel(folder,drawer)
                    obj = coolFolder.findObject(valKey,channel)
                    #payload = obj.payload()
                    for var in varlist:
                        if '/HVSET' in folder:
                            coolVarName = var.split(".")[1]
                        elif '/AI' in folder:
                            coolVarName = "AI_"+var+"_VALUE"
                        else:
                            coolVarName = var.upper()
                        #previousVal[var][0]  = payload[coolVarName]
                        sval = obj.payloadValue(coolVarName)
                        if sval=="NULL": sval="0"
                        previousVal[var][0] = float(sval)

            #=== fill the buffers with current value
            for iVar in xrange(len(variables)):
                var = variables[iVar]
                buffer[iVar][0] = previousVal[var][0]
            #=== fill friend tree
            t.Fill()

        #=== reset trees
        h2000.SetBranchStatus("*",1)
        t.ResetBranchAddresses();
        bar.done()

        #=== print out stats
        print "IOV start: ", time.ctime(timeBeg), "\t COOL time stamp: ", (timeBeg*self.unix2cool)
        print "IOV end  : ", time.ctime(timeEnd), "\t COOL time stamp: ", (timeEnd*self.unix2cool)

        return t


    #________________________________________________________________________________________________
    def getDCSTree(self, iovStartUnix, iovEndUnix, drawers, variables, lastOnly=False, firstAndLast=False):
        """
        Returns a TTree containing the specified variables for the specified drawers between
        iovStart and iovEnd. The variables in the output tree are called <drawer.variable>.
        Input: - iovStartUnix: start time stamp in unix time
               - iovEndUnix  :   end time stamp in unix time
               - drawers     : list of drawers, i.e. ['LBC13','LBC15']
               - variables   : list of variables, i.e. ['AI_5VMB_OUTPUT_V_VALUE','AI_15VMB_OUTPUT_I_VALUE']
        Output: TTree with the requested variables. A new entry is created in the tree if any
                of the variables changes its value. A weight called \"weight\" is calculated for each
                entry in the tree proportional to the lenght of the IOV.
        """

        #=== catch duplications
        drawers   = list(set(drawers))
        if not len(variables):
            variables = self.info.get_all_variables()
        variables = list(set(variables))
        logging.debug("Will extract the following variables:")
        for var in variables:
            logging.debug("\t- %s"%var)

        #=== build a list of folders and
        #=== 1) associated channel numbers  and
        #=== 2) associated variables in the folder
        #=== that need to be accessed
        folderChannelDict = {}
        folderVariableDict = {}
        for drawer in drawers:
            for variable in variables:
                folder, channel = self.info.get_folder_and_channel(variable, drawer)

                if folder not in folderChannelDict:
                    folderChannelDict[folder] = [ channel ]
                else:
                    if channel not in folderChannelDict[folder]:
                        folderChannelDict[folder].append(channel)

                if folder not in folderVariableDict:
                    folderVariableDict[folder] = [ variable ]
                else:
                    if variable not in folderVariableDict[folder]:
                        folderVariableDict[folder].append(variable)


        #===============================================================
        #=== Extract data from DB
        #===============================================================

        folderVarSetList = []
        if self.dbSource=="COOL":
            folderVarSetList = self.readFromCool(iovStartUnix, iovEndUnix, folderChannelDict, folderVariableDict)
        elif self.dbSource=="ORACLE":
            folderVarSetList = self.readFromOracle(iovStartUnix, iovEndUnix, folderChannelDict, folderVariableDict)
        elif self.dbSource=="TESTBEAM":
            folderVarSetList = self.readFromOracle(iovStartUnix, iovEndUnix, folderChannelDict, folderVariableDict)


        #=== extract list of unique iovSince
        uniqueIOVs = []
        uniqueVariables = []
        for folderVarSet in folderVarSetList:
            uniqueIOVs.extend( folderVarSet.getUniqueIOVs() )
            uniqueVariables.extend( folderVarSet.getUniqueVariables() )
        uniqueIOVs = sorted(list(set(uniqueIOVs)))
        print "===> Found Number of unique IOVs: " , len(uniqueIOVs)
        print "===> Will store the following variables:" , uniqueVariables

        #=== calculate normalized iov weights
        uniqueIOVs.append(iovEndUnix*self.unix2cool)
        iovWeight={}
        sumDiff = 0.
        for i in xrange(len(uniqueIOVs)-1):
            iov = uniqueIOVs[i]
            diff = float(uniqueIOVs[i+1]-iov)
            iovWeight[iov] = diff
            sumDiff = sumDiff + diff
        uniqueIOVs.pop(-1)
        for iov in uniqueIOVs:
            iovWeight[iov] = iovWeight[iov]/sumDiff

        #=== create tree structure
        treeName="DCS_tree"
        t = ROOT.TTree(treeName,treeName)
        t.SetDirectory(0)
        evTime = array('l',[0])
        t.Branch('EvTime', evTime, 'EvTime/I')
        weight = array('f',[0])
        t.Branch('weight', weight, 'weight/F')
        buffer = {}
        for drawer, variable in uniqueVariables:
            varName = drawer+"."+variable
            arrayType, treeType = self.info.get_variable_type(variable)
            buffer[varName] = array(arrayType, [0])
            t.Branch( varName , buffer[varName], varName+treeType )

        if firstAndLast:
            uniqueIOVs = uniqueIOVs[:1] + uniqueIOVs[-1:]
        if lastOnly:
            uniqueIOVs = uniqueIOVs[-1:]
        #=== fill tree ordered by iov
        bar = progressBar(0, len(uniqueIOVs), 78)
        iov = 0
        for iovSince in uniqueIOVs:
            bar.update(iov)
            iov+=1
            evTime[0] = int(iovSince/self.unix2cool)
            weight[0] = iovWeight[iovSince]
            for folderVarSet in folderVarSetList:
                for drawer in folderVarSet.getDrawers():
                    folderVarSet.getFromIOVStore(drawer,iovSince)
                    for variable in folderVarSet.getVariables():
                        varName = drawer+"."+variable
                        if variable=="FORDAQ_MBHV":
                            buffer[varName][0] = int(folderVarSet.getVariable(variable))
                        else:
                            buffer[varName][0] = folderVarSet.getVariable(variable)
            t.Fill()
        bar.done()
        t.ResetBranchAddresses()
        return t



    #________________________________________________________________________________________________
    def getEntry(self, drawer, variable, unixTime):
        """
        """

        #=== sort variables by directories
        folder, channel = self.info.get_folder_and_channel(variable, drawer)

        #NGO quick hack to fix ./checkCoolLatestUpdate.py --> need to introduce some varname mapping
        if '/HVSET' in folder:
            variable = variable.split(".")[1]
        elif '/AI' in folder:
            variable = "AI_"+variable+"_VALUE"
        else:
            variable = variable.upper()
        valKey = cool.ValidityKey(unixTime*self.unix2cool)
        coolFolder = self.db.getFolder(folder)
        obj = coolFolder.findObject(valKey,channel)
        iovSince = obj.since()
        iovUntil = obj.until()
        sval     = obj.payloadValue(variable)
        if sval=="NULL": sval="0"
        val      = float(sval)
        inTime   = obj.insertionTime()

        return (val, iovSince, iovUntil, inTime)



    #________________________________________________________________________________________________
    def readFromCool(self, iovStartUnix, iovEndUnix, folderChannelDict, folderVariableDict):
        """
        """

        self.reconnect()

        #=== verify and translate times, maximum is current time
        #=== need iovStart/End as cool::ValidityKey
        if not (iovStartUnix>0):
            iovStartUnix=self.R2minTime
        if not (iovEndUnix>iovStartUnix and iovEndUnix>0):
            iovEndUnix=int(time.time())

        iovS=cool.ValidityKey(iovStartUnix*self.unix2cool)
        iovE=cool.ValidityKey(iovEndUnix*self.unix2cool)

        print "IOV start: ", time.ctime(iovS/self.unix2cool), "\t COOL time stamp: ", iovS
        if iovStartUnix<self.R2minTime and iovEndUnix>self.R2minTime:
            print "R1->R2 tm: ", time.ctime(self.R2minTime), "\t COOL time stamp: ", self.R2minTime*self.unix2cool
        print "IOV end  : ", time.ctime(iovE/self.unix2cool), "\t COOL time stamp: ", iovE

        #=== initialize return list
        folderVarSetList = []

        #=== loop over all different folders and build list of FolderVarSets
        for folder, channels in folderChannelDict.iteritems():
            logging.debug( "treating folder: %s" % folder )
            varsInFolder =  folderVariableDict[folder]
            logging.debug( "---> variables in this folder: %s" % varsInFolder )
            folderVarSet = FolderVarSet(varsInFolder,self.putDuplicates)

            ar=[iovS,iovE]
            #=== loop over different databases
            while self.check_db(iovStartUnix, iovEndUnix,ar):
                iovStart=ar[0]
                iovEnd=ar[1]
                coolFolder = self.dbV[self.dbCounter].getFolder(folder)
                coolFolder.setPrefetchAll(False) #=== save some RAM
                for channel in channels:
                    drawer = self.info.get_drawer(folder,channel)
                    logging.debug( "---> treating channel %s,    drawer %s" % ( channel, drawer ) )
                    channelSelection = cool.ChannelSelection(channel)
                    objs = coolFolder.browseObjects(iovStart, iovEnd, channelSelection)
                    bar = progressBar(iovStart,iovEnd,78,drawer)
                    while objs.goToNext():
                        obj = objs.currentRef()
                        iovSince = max(iovStart,obj.since())
                        for var in varsInFolder:
                            #=== pre- and append stuff for AI variables
                            if '/HVSET' in folder:
                                coolVarName = var.split(".")[1]
                            elif '/AI' in folder:
                                coolVarName = "AI_"+var+"_VALUE"
                            else:
                                coolVarName = var.upper()
                            sval=obj.payloadValue(coolVarName)
                            if sval=="NULL": sval="0"
                            val=float(sval)
                            logging.debug( "-------------> %s (%s)  %s : %f" % ( iovSince, time.ctime(iovSince/self.unix2cool), var, val ) )
                            folderVarSet.setVariable(var, val)
                        folderVarSet.registerInIOVStore(drawer,iovSince)
                        bar.update(iovSince)
                    bar.done()
                    #=== release server resources
                    objs.close()
            folderVarSetList.append(folderVarSet)
        return folderVarSetList


    #________________________________________________________________________________________________
    def readFromOracle(self, iovStartUnix, iovEndUnix, folderChannelDict, folderVariableDict):
        """
        """

        #=== get logging status
        isDebug = (logging.getLogger().getEffectiveLevel() <= logging.DEBUG)

        #=== verify and translate times, maximum is current time
        now = int(time.time())
        iovStart = self.getOracleTimeString(iovStartUnix)
        iovEnd   = self.getOracleTimeString(now)
        if iovEndUnix>iovStartUnix and iovEndUnix>0:
            iovEnd = self.getOracleTimeString(iovEndUnix)
        print "IOV start: ", time.ctime(iovStartUnix), " (in local time)\t ORACLE time string: ", iovStart, " (in UTC)"
        print "IOV end  : ", time.ctime(iovEndUnix  ), " (in local time)\t ORACLE time string: ", iovEnd  , " (in UTC)"

        #=== initialize return list
        folderVarSetList = []

        #=== get all tables spanned by query
        tableRange = self.getEventHistoryTables( iovStart, iovEnd)
        tables = tableRange.keys()
        tables.sort()
        print "===> Going to access the following oracle table(s):"
        for table in tables:
            print "     * ", table, " ---> validity range: ", tableRange[table]

        for folder, channels in folderChannelDict.iteritems():
            logging.debug( "treating folder: %s" % folder )
            varsInFolder = folderVariableDict[folder]
            logging.debug( "---> variables in this folder: %s" % varsInFolder )
            for channel in channels:
                drawer = self.info.get_drawer(folder,channel)
                logging.debug( "channel: %s" % channel )
                nCrap = 0;
                for var in varsInFolder:
                    varType = self.info.get_variable_type(var)
                    logging.debug( "variable: %s" % var )
                    folderVarSet = FolderVarSet([var],self.putDuplicates)
                    bar = progressBar(iovStartUnix,iovEndUnix,78,drawer,var)
                    for table in tables:
                        tableRangeStart, tableRangeEnd = tableRange[table]
                        logging.debug( "Processing table: %s" % table )
                        oracleString = self.getOracleString(folder, drawer, var, table, tableRangeStart, tableRangeEnd)
                        logging.debug( "Oralce string: %s" % oracleString )
                        stmt = self.db.Statement(oracleString)
                        if stmt.Process():
                            stmt.StoreResult()
                            #=== read all values
                            value = 0
                            while stmt.NextResultRow():
                                if   varType==self.info.type_int:    value = stmt.GetInt(   1)
                                elif varType==self.info.type_float:  value = stmt.GetDouble(1)
                                #=== catch crap in oracle
                                if value < -10000:
                                    nCrap += 1
                                    continue
                                #=== extract time stamp
                                Y     = stmt.GetYear(0)
                                M     = stmt.GetMonth(0)
                                D     = stmt.GetDay(0)
                                H     = stmt.GetHour(0)
                                Min   = stmt.GetMinute(0)
                                Sec   = stmt.GetSecond(0)
                                tuple = (Y,M,D,H,Min,Sec,0,0,0)
                                #=== time.mktime interprets tuple as local standard (==winter) time
                                #=== example: Oracle time stamp is 15:00
                                #=== mktime makes seconds for 15:00 local winter time, i.e. 14:00 UTC
                                #=== --> need to add 3600 seconds (time.timezone == -3600)
                                unixTime = time.mktime(tuple) - time.timezone
                                iovSince = int(unixTime*self.unix2cool)
                                folderVarSet.setVariable(var, value)
                                folderVarSet.registerInIOVStore(drawer,iovSince)
                                if not isDebug: bar.update(unixTime)
                                logging.debug( "%s %s: %s (%s)\t%f" % (drawer,var,iovSince,time.ctime(unixTime),value) )
                        if not isDebug: bar.done()
                    folderVarSetList.append(folderVarSet)
                if nCrap > 0:
                    logging.warning("Found %i \"crap\" entries for %s, ignored those!"%(nCrap,var))
        return folderVarSetList


    #________________________________________________________________________________________________
    def getOracleTimeString(self, unixTime):
        """
        Convert input unixTime to Oracle time string yymmddhh24miss
        """

        tuple  = time.gmtime(unixTime)
        year   = str(tuple[0])[-2:]
        month  = ('00'+str(tuple[1]))[-2:]
        day    = ('00'+str(tuple[2]))[-2:]
        hour   = ('00'+str(tuple[3]))[-2:]
        minute = ('00'+str(tuple[4]))[-2:]
        second = ('00'+str(tuple[5]))[-2:]
        return year+month+day+hour+minute+second



    #________________________________________________________________________________________________
    def getOracleString(self, folder, drawer, var, table, tableRangeStart, tableRangeEnd):
        """
        iovStart and iovEnd need to be a string, formated in yymmddhh24miss
        """

        self.reconnect()
        key = ( folder , drawer)
        if key not in self.info.folderDrawer_to_oracleId:
            logging.error( "Can not resolve key: %s , known keys are:" % key )
            for key, val in self.info.folderDrawer_to_oracleId.iteritems():
                logging.error( "%s %s" % (key, val) )
            return None
        oracleId = self.info.folderDrawer_to_oracleId[key]

        masterTableName = table.split('.')[0]
        statement  = "select TS, VALUE_NUMBER, ELEMENT_NAME from "
        sysId = self.info.systemID[drawer[:3]]
        #=== DAQ STATE has system id from SCS
        if self.info.vars[var][0] == self.info.VARS_DAQ:
            sysId = self.info.systemID["SCS"]
        statement += "(select element_id, element_name from %s.elements where sys_id = %i and event = 1 and " % (masterTableName,sysId)
        statement += "ELEMENT_NAME = \'"+oracleId

        #=== special treatment for STATES
        if self.info.vars[var][0] == self.info.LVPS_STATES:
            logging.debug( "STATES VAR DETECTED: %s" % var )
            if   var == "FORDAQ_HV"  : var = ".ForDAQ_HV"
            elif var == "FORDAQ_MB"  : var = ".ForDAQ_MB"
            elif var == "FORDAQ_MBHV": var = ".ForDAQ_MBHV"
        #=== special treatment for HV
        elif self.info.vars[var][0] == self.info.VARS_HV:
            logging.debug( "HV VAR DETECTED: %s" % var )
            var = "."+var+".value"
        #=== special treatment for HV
        elif self.info.vars[var][0] == self.info.VARS_HVSET:
            logging.debug( "HVSET VAR DETECTED: %s" % var )
            var = "."+var.split(".")[1]+".order"
        #=== special treatment for DAQ STATE
        elif self.info.vars[var][0] == self.info.VARS_DAQ:
            var = ""
        else:
            var = "/"+var+".value"


        statement += var
        statement +="\') el, "
        statement += "(select element_id, ts, value_number from %s " % table
        statement += "where ts between to_date(\'"+tableRangeStart+"\', \'yymmddhh24miss\') AND to_date(\'"+tableRangeEnd+"\', \'yymmddhh24miss\')) "
        statement += "ev where el.element_id = ev.element_id"
        return statement


    #________________________________________________________________________________________________
    def getEventHistoryTables(self, iovStart, iovEnd):
        """
        This function returns a list of the oracle eventhistory tables spanned
        by the range iovStart to iovEnd.
        - iovStart : start time in the format yymmddhh24miss
        - iovEnd   : end   time in the format yymmddhh24miss
        """

        evhistNames = {}

        #=== last entry in ATLAS_PVSS_TILE.EVENTHISTORY_00007001:  06/25/2007, 15:06:48 (oracle time, UTC)
        #=== ---> table name changed at oracle time 070625150648 (yymmddhh24miss)
        #=== last entry in ATONR_ADG.EVENTHISTORY:  May 25 00:00:00 2011 (oracle time, UTC)
        #=== ---> table name changed at oracle time 070625150648 (yymmddhh24miss)
        oldTableBoundary = 70625150648
        oldTableBoundary2 = 131231230000
        if int(iovStart) < oldTableBoundary:
            if int(iovEnd) > oldTableBoundary:
                raise ("Can not generate plots of table boundary at 2007-06-25 15:06:48, sorry...")
            rangeEnd = min(iovEnd , "070625150648" )
            validityRange = ( iovStart, rangeEnd )
            evhistNames["ATLAS_PVSS_TILE.EVENTHISTORY_00007001"] = validityRange
        elif int(iovStart) < oldTableBoundary2 and int(iovStart) >= oldTableBoundary and int(iovEnd)>oldTableBoundary2:
            raise ("Can not generate plots of table boundary at 2014-01-01 00:00:00, sorry...")
            rangeEnd = min(iovEnd , "131231230000" )
            validityRange = ( iovStart, rangeEnd )
            evhistNames["ATLAS_PVSS_TILE.EVENTHISTORY_00007001"] = validityRange

        else:
            statement  = "select archive#, "
            statement += "to_char(start_time, 'yymmddhh24miss') as s_time, "
            statement += "to_char(end_time,   'yymmddhh24miss') as e_time "
            statement += "from( "
            statement += "select archive#, start_time, end_time from ATLAS_PVSSTIL.arc_archive "
            statement += "where group_name = 'EVENT' and "
            statement += "(end_time > to_date('%s', 'yymmddhh24miss') or end_time is null) " % iovStart
            statement += "intersect "
            statement += "select archive#, start_time, end_time from ATLAS_PVSSTIL.arc_archive "
            statement += "where group_name = 'EVENT' and "
            statement += "start_time < to_date('%s', 'yymmddhh24miss') " % iovEnd
            statement += ")"

            stmt = self.db.Statement(statement)
            if stmt.Process()==True:
                stmt.StoreResult()
                while stmt.NextResultRow():
                    rangeStart = stmt.GetString(1)
                    if iovStart > rangeStart:
                        rangeStart = iovStart
                    rangeEnd = stmt.GetString(2)
                    if rangeEnd=="" or iovEnd<rangeEnd:
                        rangeEnd=iovEnd

                    validityRange = ( rangeStart , rangeEnd )
                    evhistNum = (8*'0'+stmt.GetString(0))[-8:]
                    evhistNames["ATLAS_PVSSTIL.EVENTHISTORY_"+evhistNum] = validityRange

        return evhistNames


