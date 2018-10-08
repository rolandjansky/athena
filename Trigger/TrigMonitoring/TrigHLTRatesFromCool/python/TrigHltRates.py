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
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#######################################################

#!/usr/bin/python

import sys
import os
import math
import traceback
import array
import datetime
import re

from PyCool import cool

from AthenaCommon.Logging import logging
msg = logging.getLogger("TrigHltRates")
msg.setLevel(logging.INFO)
msg.setFormat("%(asctime)s  Py:%(name)-15s %(levelname)7s %(message)s")

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
        lifetime of an instance, users ask rates for different runs.
    """

    #TODO : Add global variable here as instance counter -> Check singleton in python

    def __init__(self, ratetag = "TriggerHltRates-Physics-01", chaintag = "TriggerHltChains-Physics-01"):
        """ Constructor of TrigHltRates.

            Arguments:
                ratetag -- Cool tag for the rates folder (default TriggerHltRates-Physics-01)
                chaintag -- Cool tag for the chains folder  (default TriggerHltChains-Physics-01)
        """
        super(TrigHltRates, self).__init__()

        # All members are private as we don't want to expose any of them to user
        self.__chains = {}  # Chain dictionary with key=runnumber, value=chain_name_array
        self.__sor_eor = {} # Start/End of run time dictionary with key=runnumber, value=(SOR time,EOR time)

        self.__iovs = {}  # iov dictionary. key=runnumber, value=list(iovs) where iovs=(iovstart,iovend)
                        # self.__iovs[runnumber][i] is a tuple (iovstart,iovend) for i'th iov of runnumber

        self.__chainsrates = {}   # 2 dimensional chain names and rates dictionary. 1st key=runnumber, 2nd key=chainname, value=list(rates)
                                # self.__chainsrates[runnumber][mychain][i] is the rate for mychain during i'th iov in run "runnumber"

        self.__allrates_in_cache = []  # List of run numbers, where all rates of the run are cached (i.e. getAllRates method is called)

        self.__ratetag=ratetag
        self.__chaintag=chaintag

    def getIOV(self,runno,timestamp):
        """ Get IOV index for a timestamp, return -1 if not found"""

        # If timestamp is out of run start/end time
        sor,eor = self.__get_sor_eor(runno)
        if timestamp < sor or timestamp > eor:
            msg.warning("Timestamp out of run boundaries")
            return -1

        if runno in self.__allrates_in_cache:
            iovlist = self.__iovs[runno]

            # If timestamp if before first IOV
            if timestamp < iovlist[0][0]:
                msg.warning("Timestamp before first IOV")
                return -1

            # See if timestamp falls into any IOV
            i = 0
            for iov in iovlist:
                if timestamp >= iov[0] and timestamp < iov[1]:
                    return i
                i+=1

            # If timestamp is within first and last IOVs, but doesn't fall in any IOV (can be cool writing problems)
            if timestamp >= iovlist[0][0] and timestamp < iovlist[-1][1]:
                msg.warning("Timestamp doesn't correspond to any IOV")
                return -1

        # If IOV of timestamp still not found, update data in cache and check again
        try:
            self.__getAllRates(runno,update=True)
        except:
            return -1

        iovlist = self.__iovs[runno]
        i = 0
        for iov in iovlist:
            if timestamp >= iov[0] and timestamp < iov[1]:
                return i
            i+=1

        # If IOV of timestamp still not found, return -1
        return -1

    def getTimestamp(self,runno,iov):
        """ Get start timestamp of IOVreturn -1 if IOV not found"""

        if iov < 0:
            return -1

        if runno in self.__allrates_in_cache:
            iovlist = self.__iovs[runno]

            if iov < len(iovlist):
                return iovlist[iov][0]

        # If IOV of timestamp still not found, update data in cache and check again
        try:
            self.__getAllRates(runno,update=True)
        except:
            return -1
        iovlist = self.__iovs[runno]
        if iov < len(iovlist):
            return iovlist[iov][0]

        # If IOV of timestamp still not found, return -1
        return -1

    def getRates(self,runno,chains=[".*"],iov=5):
        """ Retrieve and return matching the chain names and rates for a single IOV for a certain run.

            Arguments:
                runno -- Run Number
                chains -- List of regex patterns to match with chain names, default=[".*"] (all chains)
                iov -- index of IOV, default=5

            Returns: (return_code, dict_chains_rates)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> No data for this IOV
                dict_chains_rates: Dictionary with keys as matching chain names, values as a rate
                    dict_chains_rates[mychainname] is rate for mychainname
        """

        dict_chains_rates = {}

        #Following is needed in case one tried to get IOV using getIOV() function for a timestamp with no IOV
        if iov < 0:
            return -3,dict_chains_rates

        # Get rates for selected chains
        return_code,dict_chains_rates_alliovs = self.getAllRates(runno,chains=chains,update=False)

        # If there is a DB access issue or chain folder is empty, return immediately
        if return_code in [-1,-2]:
            return return_code, dict_chains_rates

        allchains = self.__getChains(runno)
        somechain = allchains[0]

        # If rates for asked iov not in cache, update cache (check COOL again)
        if len(self.__chainsrates[runno][somechain]) <= iov:
            returncode,dict_chains_rates_alliovs = self.getAllRates(runno,chains=chains,update=True)

        # If rates for asked iov still not cached, they don't exist
        if len(self.__chainsrates[runno][somechain]) <= iov:
            return -3, dict_chains_rates

        for chain in dict_chains_rates_alliovs.keys():
            dict_chains_rates[chain] = dict_chains_rates_alliovs[chain][iov]

        return_code = len(dict_chains_rates.keys())
        return return_code,dict_chains_rates

    def getAllRates(self,runno,chains=[".*"],update=False):
        """ Retrieve and return matching chain names and rates for all IOVs for a certain run.

            Arguments:
                runno -- Run Number
                chains -- List of regex patterns to match with chain names, default value will get all chains
                update -- If True, everything is retrieved from COOL, regardless of data in the cache.
                          It is useful if COOL was updated in the meanwhile

            Returns: (return_code, dict_chains_rates)
                return_code -- Integer with following possible values
                    Nchains -> Number of chains that were read if successful
                    -1      -> Cool access problem
                    -2      -> No such run number in COOL chain folder
                    -3      -> No data for this run number in rates folder
                dict_chains_rates: Dictionary with keys as matching chain names, values as list of rates for each IOV
                    dict_chains_rates[mychainname][j] is rate of j'th IOV for mychainname
        """

        dict_chains_rates = {}
        try:
            dict_chains_rates_all = self.__getAllRates(runno,update)
        except CantAccessDB:
            return -1,dict_chains_rates
        except NoChainData:
            return -2,dict_chains_rates
        except NoRateData:
            return -3,dict_chains_rates

        # Don't do any regex matching if there is a pattern that will match everything (".*")
        if ".*" in chains:
            return_code = len(dict_chains_rates_all.keys())
            return return_code, dict_chains_rates_all

        allchains = self.__getChains(runno)

        for pattern in chains:
            for chain in allchains:
                if chain in dict_chains_rates.keys(): #Chain name is already matched before
                    continue
                try:
                    matched = bool(re.match(pattern,chain))
                except Exception,e:
                    msg.error("Regex matching error: {0}".format(e))
                    matched = False
                if matched:
                    dict_chains_rates[chain] = dict_chains_rates_all[chain]

        return_code = len(dict_chains_rates.keys())
        return return_code, dict_chains_rates

    def __get_sor_eor(self,runno):
        """ Return a tuple with run start time,end time for 'runno'

            If the run is still ongoing, end time will be set to cool.ValidityKeyMax
        """
        sor_foldername ="/TDAQ/RunCtrl/SOR"
        eor_foldername ="/TDAQ/RunCtrl/EOR"
        dbconnect = "COOLONL_TDAQ/CONDBR2"
        coolchannel = 0 #Folders are single channel

        # Check if it's already cached
        if self.__sor_eor.has_key(runno):
            return self.__sor_eor[runno]

        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            cfolder_sor=db.getFolder(sor_foldername)
            cfolder_eor=db.getFolder(eor_foldername)
        except Exception,e:
            msg.error("Can't open DB or get SOR/EOR folders: {0}".format(dbconnect))
            msg.error("Exception: {0}".format(e))
            print  "Info : --- : ", sys.exc_info()
            raise e

        # Find object at lb=2 of the run
        lb=2
        runlb = (runno << 32) + lb

        try:
            obj = cfolder_sor.findObject(runlb, coolchannel)
        except Exception,e:
            msg.error("Can't get object from folder: {0}".format(sor_foldername))
            msg.error("Exception: {0}".format(e))
            raise e

        payload = obj.payload()
        sor_time = payload["SORTime"]/1000000000
        #print("SOR: {0} ".format(sor_time))

        try:
            obj = cfolder_eor.findObject(runlb,coolchannel)
            payload = obj.payload()
            eor_time = payload["EORTime"]/1000000000
            #print("End of Run reached, run is stopped! EOR: {0}".format(eor_time))
        except Exception,e:
            eor_time = cool.ValidityKeyMax
            msg.info("End of Run NOT reached, run ongoing")

        # finish
        db.closeDatabase()
        self.__sor_eor[runno] = (sor_time, eor_time)
        return sor_time,eor_time

    def __getChains(self,runno):
        """ Private method to get all chain names for a run number """
        if self.__chains.has_key(runno):
            return self.__chains[runno]

        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            chainfolder=db.getFolder(chain_foldername)
        except Exception,e:
            msg.error("Can't open DB or get chain folder")
            msg.error("Exception: {0}".format(e))
            raise CantAccessDB

        # Chain folder is run/lumi based, using lb=2 ensures we get the entry for right IOV
        lb=2
        runlb = (runno << 32) + lb

        try:
            chainobj = chainfolder.findObject(runlb, coolchannel, self.__chaintag);
        except Exception,e:
            msg.error("Can't get chains object from chain folder, Exception: {0}".format(e))
            raise NoChainData

        payload = chainobj.payload()
        chains = payload["chains"].split(",")
        self.__chains[runno]=chains

        return chains

    def __getAllRates(self,runno,update=False):
        """ Get rates for a run for all IOVs

            Arguments:
                runno -- run number
                update -- If True, everything is retrieved from COOL, regardless of data in the cache.
                          It is useful if COOL was updated in the meanwhile

            Returns: dict_chains_rates, dictionary with key: chainname, value:list(rates)
                dict_chains_rates[mychain][i] is rate of mychain during i'th IOV
        """

        allchains = self.__getChains(runno)

        iov_start,iov_end = self.__get_sor_eor(runno)

        # If rates are already loaded to cache and update=False don't access COOL
        if runno in self.__allrates_in_cache and not update:
            return self.__chainsrates[runno]

        if runno not in self.__chainsrates.keys():
            self.__chainsrates[runno] = {}

        # Reinitialize rate array for each chain
        for chain in allchains:
            self.__chainsrates[runno][chain] = []

        # reinitialize IOV list
        self.__iovs[runno] = []

        # Else, read from COOL
        dbSvc=cool.DatabaseSvcFactory.databaseService()

        readonly = True

        try:
            db=dbSvc.openDatabase(dbconnect,readonly)
            ratefolder=db.getFolder(rate_foldername)

            # 341649 is the last physics run of 2017
            # Starting 2018, the iov of rate folder is in ns instead of s.
            if runno > 341649:
                iov_start = iov_start*1000000000
                iov_end = iov_end*1000000000

            rateobjs = ratefolder.browseObjects(long(iov_start), long(iov_end),cool.ChannelSelection(int(coolchannel)),self.__ratetag)
        except Exception,e:
            msg.error("Can't open DB or get folders, Exception: {0}".format(e))
            raise CantAccessDB

        ratedataexist = 0
        for obj in rateobjs:
            since = obj.since()
            until = obj.until()
            # Discard entries that may belong to previous/next runs, or first entry of the next run, which may fall in (iov_start,iov_end)
            if since < iov_start or until > iov_end:
                continue

            ratedataexist = 1

            # 341649 is the last physics run of 2017
            # Starting 2018, the iov of rate folder is in ns instead of s.
            if runno > 341649:
                since = since /1000000000
                until = until /1000000000
            iov = (since,until)
            # print ("since,until = {},{}".format(since,until))

            payload = obj.payload()
            data = payload["rates"]
            ratedata = self.__blob2rates(data,runno)

            #Cache results
            self.__iovs[runno].append(iov)
            chainindex = 0
            for chain in allchains:
                self.__chainsrates[runno][chain].append(ratedata[chainindex])
                chainindex+=1

        if not ratedataexist:
            msg.warning("No rate entry for timestamp")
            raise NoRateData

        db.closeDatabase()

        # Add run number to the list,
        if runno not in self.__allrates_in_cache:
            self.__allrates_in_cache.append(runno)

        return self.__chainsrates[runno]

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
