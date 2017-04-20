#######################################################
# 
# TrigHltRates Tool
# ----------------------------
# ATLAS Collaboration 
# 
# 06.04.2017 Package created 
# 
# Author: Cenk YILDIZ, UCI
# e-mail: cenk.yildiz@cern.ch
# 
#######################################################

#!/usr/bin/python

import sys
import os
import math
import traceback
import array
import datetime

from PyCool import cool


# Global variables

# COOL db connection
dbconnect = "COOLONL_TRIGGER/CONDBR2"

# COOL Folder names
rate_foldername  = "/TRIGGER/HLT/Rates"  # Folder with HLT Rates
chain_foldername = "/TRIGGER/HLT/Chains" # Folder with HLT Chain names

# All folders that are dealt with is single channel
coolchannel = 0

#Exceptions: These are only thrown internally to set correct return codes, user should not need to handle exceptions
class NoRateForTimestampInCache(Exception):
    pass

class CantAccessDB(Exception):
    pass

class NoChainData(Exception):
    pass

class NoRateData(Exception):
    pass

class NoIOVforTimestamp(Exception):
    pass

class TimestampOutOfRunBoundaries(Exception):
    pass

class TrigHltRates(object):
    """ Main class to read HLT Rates. This class allows to retrieve HLT rates for
        any HLT chain during a run. The class instance may be used by several
        users at once, thus it has an internal caching mechanism to allow faster
        access to rates once they are retrieved from COOL
        
        Some internal dictionaries use runnumber as key, in case that during the 
        lifetime of an instance, users ask rates for different runs
    """

    def __init__(self, ratetag = "TriggerHltRates-Physics-01", chaintag = "TriggerHltChains-Physics-01"):
        """ Constructor of TrigHltRates.

            Arguments:
                ratetag -- Cool tag for the rates folder (default TriggerHltRates-Physics-01)
                chaintag -- Cool tag for the chains folder  (default TriggerHltChains-Physics-01)
        """
        super(TrigHltRates, self).__init__()
        self.chains = {}  # Chain dictionary with key=runnumber, value=chain_name_array
        self.chainindex = {} # 2 dimensional chain index dictionary. 1st key: runno, 2nd key: chainname 
                             # To get index of chainname in run=runno: chainindex[runno][chainname]
        self.sor_eor = {} # Start/End of run time dictionary with key=runnumber, value=(SOR time,EOR time)
        self.iov_rates = {}  # Rates dictionary with key=iov (tuple of since,until), value=list of rates for that iov

        self.allrates_in_cache = []  # List of run numbers, where all rates of the run are cached (i.e. getAllRates or getRates methods are called)
        
        self.ratetag=ratetag   
        self.chaintag=chaintag   
    
    def getAllRates(self,runno):
        """ Retrieve and return all the chain names and rates for each IOV for a certain run.
    
            Arguments:
                runno -- Run Number

            Returns: (return_code, chainlist, ratelist, iov_list)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> No data for this run number in rates folder
                chainlist -- list of chain names, with size Nchains
                ratelist  -- 2d list of rates with size (NIOV,Nchains)
                    entry [i][j] refers to i'th IOV, c'th chain
                iovlist  -- list of iovs where each entry is (iovstart,iovend)
        """

        try:
            iovlist,ratelist = self.__getAllRates(runno)
        except CantAccessDB: 
            return -1,[],[],[]
        except NoChainData: 
            return -2,[],[],[]
        except NoRateData:
            return -3,self.__getChains(runno),[],[]

        return_code = len(self.__getChains(runno))
        return return_code, self.chains[runno], ratelist, iovlist

    def getAllRatesAtT(self,runno,timestamp):
        """Retrieve and return all the chain names and rates for a timestamp
            
            Arguments:
                runno -- Run Number
                timestamp -- unix time stamp
        
            Returns: (return_code, chainlist, ratelist)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> Timestamp out of run boundaries
                    -4      -> No IOV that contains this timestamp
                chainlist -- list of chain names, with size Nchains
                ratelist  -- List of rates with size Nchains
        """

        try:
            iov,rates = self.__getAllRatesAtT(runno,timestamp)
        except CantAccessDB: 
            return -1,[],[]
        except NoChainData: 
            return -2,[],[]
        except TimestampOutOfRunBoundaries:
            return -3,self.__getChains(runno),[]
        except NoRateData:
            return -4,self.__getChains(runno),[]

        return_code = len(self.__getChains(runno))
        return return_code, self.chains[runno], rates

    def getRates(self,runno,selected_chains):
        """Retrieve and return rates for selected chains for each IOV for a certain run
    
            Arguments:
                runno -- Run Number
                selected_chains -- list of requested chain names

            Returns: (return_code, ratelist, iov_list)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> No data for this run number in rates folder
                ratelist  -- 2d list of rates with size (NIOV,len(selected_chains))
                    entry [i][j] refers to i'th IOV, c'th chain
                iovlist  -- list of iovs where each entry is (iovstart,iovend)
        """
        try:
            iovlist,ratelist_all = self.__getAllRates(runno)
        except CantAccessDB: 
            return -1,[],[],[]
        except NoChainData: 
            return -2,[],[],[]
        except NoRateData:
            return -3,self.__getChains(runno),[],[]

        # Calculate the indices of selected chains
        i = 0       # index over selected chains
        nread = 0   # number of chains that are read
        indices = [-1] * len(selected_chains) #Initialize all indices to -1
        for chainname in selected_chains:
            if self.chainindex[runno].has_key(chainname):
                index = self.chainindex[runno][chainname]
                indices[i] = index
                nread+= 1
            i+= 1

        k = 0 #Index over iovs
        ratelist = []
        for iov in iovlist:
            rates = ratelist_all[k]
            selected_rates = [-1]*len(selected_chains) #Initialize all rates to -1

            i = 0 # index over selected chains
            for index in indices:
                if index != -1:
                    selected_rates[i] = rates[index]
                i+= 1

            ratelist.append(selected_rates) 
            k+= 1

        return_code = nread
        return return_code, ratelist, iovlist

    def getRatesAtT(self,runno,timestamp,selected_chains):
        """ Retrieve and return rates for selected chains for each IOV for a certain run

            Arguments:
                runno -- Run Number
                timestamp -- unix time stamp
                selected_chains -- list of requested chain names

            Returns: (return_code, ratelist)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> Timestamp out of run boundaries
                    -4      -> No IOV that contains this timestamp
                ratelist  -- List of rates with size len(selected_chains)
        """

        try:
            iov,rates = self.__getAllRatesAtT(runno,timestamp)
        except CantAccessDB: 
            return -1,[],[]
        except NoChainData: 
            return -2,[],[]
        except TimestampOutOfRunBoundaries:
            return -3,self.__getChains(runno),[]
        except NoRateData:
            return -4,self.__getChains(runno), []

        i = 0       # index over selected chains
        nread = 0   # number of chains that are read
        selected_rates = [-1]*len(selected_chains) #Initialize all rates to -1
        for chainname in selected_chains:
            if self.chainindex[runno].has_key(chainname):
                index = self.chainindex[runno][chainname]
                selected_rates[i] = rates[index]
                nread+= 1
            i+= 1

        return_code = nread
        return return_code, selected_rates

    def __get_sor_eor(self,runno):
        """ Return a tuple with run start time,end time for 'runno'

            If the run is still ongoing, end time will be set to cool.ValidityKeyMax
        """
        sor_foldername ="/TDAQ/RunCtrl/SOR"
        eor_foldername ="/TDAQ/RunCtrl/EOR"
        dbconnect = "COOLONL_TDAQ/CONDBR2"
        coolchannel = 0 #Folders are single channel

        # Check if it's already cached
        if self.sor_eor.has_key(runno):
            return self.sor_eor[runno]

        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            cfolder_sor=db.getFolder(sor_foldername)
            cfolder_eor=db.getFolder(eor_foldername)
        except Exception,e:
            print("Can't open DB or get SOR/EOR folders: {0}".format(dbconnect))
            print("Exception: {0}".format(e))
            sys.exit(0)

        # Find object at lb=2 of the run
        lb=2
        runlb = (runno << 32) + lb

        try:
            obj = cfolder_sor.findObject(runlb, coolchannel)
        except Exception,e:
            print("Can't get object from folder: {0}".format(sor_foldername))
            print("Exception: {0}".format(e))
            sys.exit(0)

        payload = obj.payload()
        sor_time = payload["SORTime"]/1E9
        #print("SOR: {0} ".format(sor_time))
                
        try:
            obj = cfolder_eor.findObject(runlb,coolchannel)
            payload = obj.payload()
            eor_time = payload["EORTime"]/1E9
            #print("End of Run reached, run is stopped! EOR: {0}".format(eor_time))
        except Exception,e:
            eor_time = cool.ValidityKeyMax
            print("End of Run NOT reached, run ongoing")

        # finish
        db.closeDatabase()
        self.sor_eor[runno] = (sor_time, eor_time)
        return sor_time,eor_time

    def __getChains(self,runno):
        """ Private method to get all chain names for a run number """
        if self.chains.has_key(runno):
            return self.chains[runno]

        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            chainfolder=db.getFolder(chain_foldername)
        except Exception,e:
            print("Can't open DB or get chain folder")
            print("Exception: {0}".format(e))
            raise CantAccessDB

        # Chain folder is run/lumi based, using lb=2 ensures we get the entry for right IOV
        lb=2
        runlb = (runno << 32) + lb

        try:
            chainobj = chainfolder.findObject(runlb, coolchannel, self.chaintag);
        except Exception,e:
            print("Can't get chains object from chain folder!")
            print("Exception: {0}".format(e))
            raise NoChainData

        payload = chainobj.payload()
        chains = payload["chains"].split(",")
        self.chains[runno]=chains

        self.chainindex[runno] = {}
        i = 0
        for c in chains:
            self.chainindex[runno][c] = i
            i += 1

        return chains

    def __getAllRates(self,runno, iov_start=None,iov_end=None):
        """ Get rates between 2 timestamps
            
            Arguments:
                runno -- run number
                iov_start -- start timestamp, if None, use run start time
                iov_end -- end timestamp, if None, use run end time

            Returns: (rates,iovs)
                iovs -- list of tuples, where each tuple is (since,until)
                rates -- list of lists where entry [i][j] is i'th iov, j'th chain
        """

        self.__getChains(runno)

        start,end = self.__get_sor_eor(runno)

        # iov_start, iov_end is implemented in case we add a method to get rates between 2 timestamps
        if iov_start == None:
            iov_start = start
        else:
            if iov_start < start or iov_start > end:
                raise TimestampOutOfRunBoundaries

        if iov_end == None:
            iov_end = end
        else:
            if iov_end < start or iov_end > end:
                raise TimestampOutOfRunBoundaries

        ratelist = []
        iovlist = []

        # If rates are already loaded to cache, don't access COOL
        if runno in self.allrates_in_cache:
            iovs = self.iov_rates.keys()
            iovs.sort()
            for iov in iovs:
                if iov[0] > iov_start and iov[1] < iov_end: # If entry is within the run boundaries
                    iovlist.append(iov)
                    ratelist.append(self.iov_rates[iov])
        else:
            dbSvc=cool.DatabaseSvcFactory.databaseService()

            readonly = True

            try:
                db=dbSvc.openDatabase(dbconnect,readonly)
                ratefolder=db.getFolder(rate_foldername)
                rateobjs = ratefolder.browseObjects(int(iov_start), int(iov_end),cool.ChannelSelection(int(coolchannel)),self.ratetag)
            except Exception,e:
                print("Can't open DB or get folders")
                print("Exception: {0}".format(e))
                raise CantAccessDB

            for obj in rateobjs:
                since = obj.since()
                until = obj.until()
                # Discard entries that may belong to previous/next runs, or first entry of the next run, which may fall in (iov_start,iov_end)
                # TODO(cyildiz) if iov_start and iov_end are passed as argument to this function, following may be not desirable
                if since < iov_start or until > iov_end:
                    continue

                payload = obj.payload()
                data = payload["rates"]

                ratedata = self.__blob2rates(data,runno)
                ratelist.append(ratedata)
                iov = (since,until)
                iovlist.append(iov)

                if not self.iov_rates.has_key(iov):
                    self.iov_rates[iov]=ratedata

            if ratelist==[]:
                raise NoRateData

            db.closeDatabase()

            # Add run number to the list,
            self.allrates_in_cache.append(runno)

        return iovlist,ratelist

    def __getAllRatesAtT(self,runno, timestamp):
        """ Get rates at a certain timestamp
            
            Arguments:
                runno -- run number
                timestamp -- timestamp

            Returns: (iov,rates)
                iov -- iov of the cool entry as a tuple: (since,until)
                rates -- list of floats with size Nchains
        """

        self.__getChains(runno)

        sor,eor=self.__get_sor_eor(runno)
        if timestamp < sor or timestamp > eor:
            raise TimestampOutOfRunBoundaries

        #Try getting the rates from the cache first
        for iov in self.iov_rates:
            if timestamp<iov[1] and timestamp>iov[0]:
                return (iov,self.iov_rates[iov])

        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            ratefolder=db.getFolder(rate_foldername)
        except Exception,e:
            print("Can't open DB or get folders")
            print("Exception: {0}".format(e))
            raise CantAccessDB

        try:
            rateobj = ratefolder.findObject(timestamp, coolchannel, self.ratetag)
        except Exception,e:
            print ("No rate entry for timestamp")
            raise NoRateData

        since = rateobj.since()
        until = rateobj.until()
        iov = (since,until)

        payload = rateobj.payload()
        data = payload["rates"]

        ratedata = self.__blob2rates(data,runno)

        db.closeDatabase()

        self.iov_rates[iov]=ratedata
        return iov,ratedata

    def __blob2rates(self,blob,runno):
        """ Convert COOL blob to list of floats using the version number
        
            Arguments
                blob: cool blob
                runno: run number, needed to get number of chains
            
            Returns
                rates: list of floats
        """
        nchains = len(self.__getChains(runno))
        datastr = blob.read()
        floatarr = array.array("f", datastr).tolist()
        version = floatarr[0]
        if version == 2.0:
            data = floatarr[1:1+nchains]
        return data


#TODO(cyildiz) In getRates() and getAllRates(), give option to user to access cool. COOL could be updated since last time method was run, and we may want to get new entries
