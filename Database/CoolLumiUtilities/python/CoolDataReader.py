# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# CoolDataReader
#
# Eric Torrence - October 2010
#
# Contents:
# CoolDataReader - utility object to handle reading of COOL DB folders.
#                 The benefit over just using AtlCoolLib directly is that each DB connection is
#                 cached, so multiple connections to the same DB will not be made.
#
#                 CoolDataReader.readData() returns a list the full IObject for maximal flexibility
#
# General usage example
# myReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLESTONL')
# myReader.setIOVRange(startIOV, endIOV)
# myReader.readData()
# for obj in myReader.data:
#   ...
#
# One can specify specific channels or IOV ranges if desired, but by default all data will be loaded
#
# The CoolDataReader uses the LumiDBHandler internally to cache multiple CoolConnections
#

from PyCool import cool

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler


class CoolDataReader:

    def __init__(self, dbstr=None, folderstr=None):

        self.verbose = False

        # Defined variables
        self.dbstr = None
        self.folderstr = None
        self.channelIdList = []
        self.tag = ''
        self.iovstart = None
        self.iovend = None

        self.folder = None
        self.data = []
        
        # Initialize to default values
        self.setChannel()
        self.setTag()
        self.setFolder(dbstr, folderstr)
        self.setIOVRange()
            
    def setFolder(self, dbstr, folderstr):
        # Force re-opening connection if these are different
        if (dbstr != self.dbstr) or (folderstr != self.folderstr):
            self.folder = None
            
        self.dbstr = dbstr
        self.folderstr = folderstr

    def setTag(self, tagstr=''):
        self.tag = tagstr

    def setChannel(self, channelIdList=[]):
        self.channelIdList = channelIdList
        
    def setChannelAll(self):
        self.setChannel()

    def setChannelId(self, channelId):
        self.setChannel([channelId])
        
    def setIOVRange(self, iovstart=cool.ValidityKeyMin, iovend=cool.ValidityKeyMax):
        self.iovstart = iovstart
        self.iovend = iovend

    def setIOVRangeFromRun(self, runnum):
        self.iovstart = runnum << 32
        self.iovend = ((runnum+1) << 32) - 1

    # Call to get data after all other parameters are properly set
    # Data is returned as a list of IObject values, one per DB entry.
    # This gives maximal flexibility to manipulate the items
    def readData(self):

        self.data = []

        # Open the DB connection here if needed
        if self.folder == None:
            dbHandler = LumiDBHandler()
            self.folder = dbHandler.getFolder(self.dbstr, self.folderstr)
            
            if self.folder == None:
                print "Can't access DB", self.dbstr, 'folder', self.folderstr, '!'
                return self.data

        # Create the channel list
        if len(self.channelIdList) == 0:
            channels = cool.ChannelSelection.all()
            self.readChannelList(channels)

        else:
            # Build the channel list here
            self.channelIdList.sort()  # Must be sorted!

            # Must read channels 50 at a time due to COOL limit...
            ichan = 0
            while (ichan < len(self.channelIdList)) :

                jchan = 0
                channels = None
                firstChan = True
            
                for channelId in self.channelIdList[ichan:]:
                    jchan += 1
                    if firstChan:
                        firstChan = False
                        channels = cool.ChannelSelection(channelId)
                    else:
                        channels.addChannel(channelId)
                    if jchan == 50: break 

                # Remeber how many we have read for next time
                if self.verbose:
                    print 'CoolDataReader.readData() - loaded %d channels from %d' % (jchan, ichan)
                ichan += jchan

                if self.verbose:
                    print 'CoolDataReader.readData() - browsing', self.iovstart, self.iovend, 'with channel', channels, 'and tag', self.tag

                self.readChannelList(channels)

            # End of loop building channel list and reading

        # End of if statement reading data
        return self.data

    def readChannelList(self, channels):

        # Open iterator over our defined IOV range
        try:
            itr = self.folder.browseObjects(self.iovstart, self.iovend, channels, self.tag)
        except Exception, e:
            print 'CoolDataReader.readData() - exception reading folder:', self.folderstr
            print e
            print 'CoolDataReader.readData() - will try to reconnect (once)'

            # Force re-opening connection
            dbHandler = LumiDBHandler()
            dbHandler.verbose = True
            self.folder = dbHandler.getFolder(self.dbstr, self.folderstr, force=True)
            
            if self.folder == None:
                print 'CoolDataReader.readData() - forced re-opening failed!'
                return self.data

            # OK, lets try reading this again
            print 'CoolDataReader.readData() - trying to re-read re-opened folder!'
            try:
                itr = self.folder.browseObjects(self.iovstart, self.iovend, channels, self.tag)
            except Exception, e:
                print 'CoolDataReader.readData() - exception reading folder:', self.folderstr
                print e
                return self.data
                
        while itr.goToNext():
            obj = itr.currentRef()
            # print obj.payload()
            self.data.append(obj.clone())
            
        itr.close()


        
