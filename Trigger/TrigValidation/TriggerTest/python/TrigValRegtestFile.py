# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

class TrigValRegtestFile:
   def __init__(self, defaultFile):
      self.Default =  defaultFile

   def addFile(self, tag, file ):
      self.__dict__[tag] = file

   def getFile(self, tag='Default' ) :
      if( self.__dict__.has_key(tag) ):
          return self.__dict__[tag]
      else :
          #if( tag != "" ) :
          #   print "TrigValRegtestFile   INFO : use tag \"Default\" instead of \""+tag+"\""
          return self.__dict__['Default']
