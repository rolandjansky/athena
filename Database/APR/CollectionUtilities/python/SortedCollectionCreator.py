# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ = "Marcin Nowak"
__doc__ = """
Create a sorted collection of Event references from a set of Athnea files containing EventInfoTags
(or from other compatible APR Event collections)
"""


class SortedCollectionCreator:
   """Creates a sorted (APR) collection of event references from input files"""
   
   def __init__(self, name= "sortEvents"):
      from AthenaCommon import Logging
      Logging.log.name = name
      self.info = Logging.log.info
      self.debug = Logging.log.debug
      self.verbose = Logging.log.verbose
      self.collDescription = None
      self.allRows = []
      self.attrNames = []
      self.attrTypes = {}
      self.tokenName = None
      self.collSvc = None

   def loadRoot(self):
      """import ROOT, create CollectionSvc"""
      import ROOT
      self.collSvc = ROOT.pool.CollectionService()

   def readCollectionDescription(self, collection):
      """read Collection Description and remember it"""
      desc =  collection.description()
      self.debug("Reading Collection Description from {}".format(desc.name()))
      import ROOT
      # read attributes' names and types from the description and remember them
      for an in range(0, desc.numberOfAttributeColumns()):
         attr = desc.attributeColumn(an,0)
         name = attr.name()
         self.attrNames.append(name)
         self.attrTypes[name] = attr.type()
      self.tokenName = desc.eventReferenceColumnName()
      # make a local copy of the description
      self.collDescription = ROOT.pool.CollectionDescription( desc )
      # prevent overwriting input by accident
      self.collDescription.setName("")


   def readInputCollections(self, inputCollections):
      """read all input collections into memore"""
      self.collDescription = None
      for inFileName in inputCollections:
         self.debug("Opening {}".format(inFileName))
         iColl = self.collSvc.open( "PFN:"+inFileName, "RootCollection")
         self.debug("{} opened".format(inFileName))
         if self.collDescription is None:
            self.readCollectionDescription(iColl)
         self.info("Reading Events from {}".format(inFileName))

         query = iColl.newQuery()
         query.selectAll()
         cursor = query.execute()
      
         while cursor.next():
            row = cursor.currentRow()
            # put the token first in the attribute list, for convenience
            t = [ row.tokenList()[self.tokenName].toString() ]
            for nam in self.attrNames:
               t.append( row.attributeList()[nam].data[ self.attrTypes[nam] ]() )
            self.allRows.append(t)   
       
         iColl.close()

      for t in self.allRows:
         self.verbose( t[1:], t[0])
      self.verbose('='*80)
      self.info("Finished reading input collections, total events read: {}".format(len(self.allRows)) )

   def sortEvents(self, sortAttrName):
      """sort the events based on an attribute name"""
      self.info("Sorting on attribute {}".format(sortAttrName))
      # +1 because the Ref is first
      attrPos = self.attrNames.index(sortAttrName)+1
      self.allRows.sort( key=lambda t: t[attrPos] )

      for t in self.allRows:
         self.verbose( t[1:], t[0] )
      self.verbose('='*80)

   def writeCollection(self, outputCollection):
      """write sorted collection into a Collection file"""
      self.info("Writing Event collection {} ".format(outputCollection+".root"))
      self.collDescription.setName(outputCollection)
      self.collDescription.setType("RootCollection")
      # create the output collection (file)
      dstColl = self.collSvc.create(self.collDescription)

      for t in self.allRows:
         row = dstColl.dataEditor().rowBuffer()
         row.tokenList()[0].fromString( t[0] )
         for idx,nam in enumerate(self.attrNames):
            type = self.attrTypes[nam]
            row.attributeList()[nam].setValue[type]( t[idx+1] )
         dstColl.dataEditor().insertRow( row )
      
      dstColl.commit()
      dstColl.close()
       
   def execute(self, inputCollections, outputCollection="PFN:collection.root", sortAttribute="LumiBlockN"):
      self.info("Executing SortedCollectionCreator, inputs={}, output={}, sort on: {}"
                .format(inputCollections, outputCollection, sortAttribute))
      if isinstance(inputCollections, str):
         inputs = [inputCollections]
      else:
         inputs = inputCollections
      self.loadRoot()
      self.readInputCollections(inputs)
      self.sortEvents(sortAttribute)
      self.writeCollection(outputCollection)
      

