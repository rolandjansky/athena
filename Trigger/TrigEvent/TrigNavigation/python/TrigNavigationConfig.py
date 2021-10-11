# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigNavigation.TrigNavigationConf import HLT__Navigation



class HLTNavigation ( HLT__Navigation ):
    __slots__ = []
    
    def __init__(self, name = 'Navigation' ):# , offline=None):
        super( HLTNavigation, self ).__init__( name )  #
        #        self.Serializer = Serializer()
        from TrigSerializeResult.TrigSerializeResultConf import TrigTSerializer
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TrigTSerializer()

    def appendToPayload(self, classname ):
        if classname not in  self.ClassesToPayload:
            self.ClassesToPayload += [ classname ]

    def removeFromPayload(self, classname ):
        self.ClassesToPayload.remove ( classname )



class HLTNavigationOffline ( HLTNavigation ):
    __slots__ = []
    def __init__(self, name = 'Navigation' ):
        super( HLTNavigationOffline, self ).__init__( name )

    def setDefaults(self, handle):
        handle.ClassesToPayload = []




class HLTNavigationOnline ( HLTNavigation ):
    __slots__ = []
    def __init__(self, name = 'Navigation' ):
        super( HLTNavigationOnline, self ).__init__( name )

    def setDefaults(self, handle):
        handle.ClassesToPayload = []



class TestingHLTNavigationOffline ( HLTNavigation ):
    __slots__ = []
    def __init__(self, name = 'Navigation' ):
        super( TestingHLTNavigationOffline, self ).__init__( name )

    def setDefaults(self, handle):
        handle.ClassesToPayload = []
        from AthenaCommon.Constants import VERBOSE
        handle.OutputLevel = VERBOSE
