# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: IOVDbSvc/python/ConditionsContainerAccess.py
# Author: RD Schaffer (R.D.Schaffer@cern.ch)

# The ConditionsContainerAccess provides access to the objects
# referenced from COOL and stored in POOL.
#
# Usage:
#   1) Create a ConditionsContainerAccess providing theApp from
#      athena.py:
#      # Get Conditions Container Accessor
#      accessor = ConditionsContainerAccess(theApp)
#
#   2) Provide with folder description obtained from a COOL folder:
#      accessor.setDescription(descr)
#
#   3) For single channel containers:
#      myCont =  accessor.getSingleCondContainer()
#
#      or for multichannels, one must provide three lists: channel
#      numbers, a CondAttrListCollection filled with the collection of
#      AttributeLists for each multichannel, and the list of IOVs:
#
#      myCont = accessor.getMultiCondContainer(chans, attrListColl, iovs)
#
#      where filling the attrListColl can be done as:
#
#      CondAttrListCollection = PyLCGDict.makeClass('CondAttrListCollection')
#      hasRunEventTime = 1
#      attrListColl = CondAttrListCollection(hasRunEventTime)
#      i = 0
#      for a in folderAttributeList :
#        # Add in attribute list
#        attrListColl.add(chans[i], attrList)
#        i += 1
#      i = 0
#      for iov in folderIOVList :
#        # Add in the IOVs for each channel
#        attrListColl.add(chans[i], iov)
#        i += 1


import cppyy as PyLCGDict
import os, sys, string


class InterfaceRDS(object) :
  def __init__(self, t ) :
    if type(t) is str : t = PyLCGDict.makeClass(t)
    #  print "type is string: ",t
    #  t = PyLCGDict.makeClass(t)
    #t = PyLCGDict.makeClass(t)
    self.type = t
  def cast(self, obj) :
    if obj :
      ip = PyLCGDict.libPyROOT.MakeNullPointer(self.type)
      if obj._isvc.queryInterface(self.type.interfaceID(), ip).isSuccess() : 
        return ip
    return None

class DescriptionDecoder(object) :
  def __init__(self, descr):
    print descr
    self.descr = descr
  def extract(self, start, stop) :
    print 'descr, size', self.descr, len(self.descr)
    begin = string.find(self.descr, start)
    print 'begin', begin
    begin += len(start)
    end = string.find(self.descr, stop)
    print 'end', end
    if begin < len(self.descr) and end < len(self.descr) :
      result = self.descr[begin:end]
    else :
      result = "Unable to find: " + start + ", " + stop
    return result


class ConditionsContainerAccess(object) :
  def __init__(self, theApp) :
    # PersistencySvc
    pers = theApp.service('EventPersistencySvc')

    # PersistencySvc via its IConverter interface
    iConverter = InterfaceRDS('IConverter')
    self.icnv = iConverter.cast(pers)

    # PersistencySvc via its IAddressCreator interface
    iAddrCreator = InterfaceRDS('IAddressCreator')
    self.iaddr = iAddrCreator.cast(pers)

    # Create instance of cast object
    self.dbCast = PyLCGDict.makeClass('DataBucketCast')

  def pyroot_typedef_bug_workaround(self):
    # This method is temporary until we migrate to ROOT 
    # delete iterator types to prevent spurious typedeffing
    import libPyROOT

    try:
      del libPyROOT.const_iterator
    except AttributeError:
      pass

    try:
      del libPyROOT.iterator
    except AttributeError:
      pass


  def setDescription(self, descr) :
    self.descr = descr
    # extract type name and type
    decoder = DescriptionDecoder(descr)
    self.typeName = decoder.extract('<typeName>','</typeName>')
    self.type     = PyLCGDict.makeClass(self.typeName)
    # extract the address header
    self.header   = decoder.extract('<addrHeader>','</addrHeader>')
    self.header   += 'POOLContainer_CondAttrListCollection][CLID=x'

    print "Type name, type, header ",self.typeName,self.type,self.header

  def getSingleCondContainer(self) :
    # Get IOpaqueAddress
    ioa = PyLCGDict.libPyROOT.MakeNullPointer('IOpaqueAddress')

    # Get DataBucketBase pointer
    dbb = PyLCGDict.libPyROOT.MakeNullPointer('DataBucketBase')
    
    # Create IOA from string address
    self.iaddr.createAddress( 0, 0, self.header, ioa )

    # Read in object as DataObject (or DatabucketBase)
    self.icnv.createObj(ioa, dbb)

    # Cast to correct type and return it
    result = PyLCGDict.libPyROOT.MakeNullPointer(self.type)
    self.dbCast.castObject(self.typeName, dbb, result)

    return result

  def getMultiCondContainer(self, channels, attrListColl, iovs) :

    # Args:
    #   channels     - list of channels from COOL
    #   attrListColl - collection of AttributeLists with string address from COOL for each channel
    #   iovs         - list of IOVs for each channel
    #   cont         - the LArConditionsContainer object

    # Print out channels and IOV
    print "Channels, IOV, and string addresses"
    i = 0
    for chan in channels :
      iov = iovs[i]
      print "added chan/iov: ", chan, iov.iovPrint()
      i += 1
      
    # print out collection
    #attrListColl.dump()

    # Must set dummy string address to include type name for
    # AthenaPoolCnvSvc statistics gathering
    strAddress = self.header

    # Get IOpaqueAddress
    print "Create IOpaqueAddress pointer"
    ioa = PyLCGDict.libPyROOT.MakeNullPointer('GenericAddress')

    # Get DataBucketBase pointer
    print "Create DataBucketBase pointer"
    dbb = PyLCGDict.libPyROOT.MakeNullPointer('DataBucketBase')
    
    # Create IOA from string address
    print "Create IOpaqueAddress for address list"
    sc = self.iaddr.createAddress( 0, 0, strAddress, ioa )
    print "Status code: ", sc

    # Create CondAttrListCollAddress and add in attribute list
    CondAttrListCollAddress = PyLCGDict.makeClass('CondAttrListCollAddress')
    collAddr = CondAttrListCollAddress(ioa)
    collAddr.setAttrListColl(attrListColl)


    # Read in object as DataObject (or DatabucketBase)
    print "Retrieve data object for IOA"
    sc = self.icnv.createObj(collAddr, dbb)
    print "Status code: ", sc
    if sc.isFailure(): raise RuntimeError("Cannot read object")

    # Cast to correct type and return it
    print "cast data object to correct type: ",self.typeName
    result = PyLCGDict.libPyROOT.MakeNullPointer(self.type)
    self.dbCast.castObject(self.typeName, dbb, result)

    # Reset iterator to allow a new type - bug in pyroot
    #self.pyroot_typedef_bug_workaround()

    return result

