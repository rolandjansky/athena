# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from GaudiKernel.GaudiHandles import PrivateToolHandleArray,PrivateToolHandle

from AthenaConfiguration.ComponentFactory import CompFactory
class dummyService(CompFactory.StoreGateSvc):
  __slots__ = {
    'SomeTools': PrivateToolHandleArray([]),
    'AList' : [],
    'OneTool' : PrivateToolHandle(),
    'AString': '',
  }


class dummyTool(CompFactory.HelloTool):
  __slots__ = {
    'BString' : '', #str
    'BList' : [ ]
  }

