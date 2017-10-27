# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: Configurables.py
# @purpose: customized Configurable classes for AthenaServices
# @author: Sebastien Binet <binet@cern.ch>

## import the automatically generated Configurables
from AthenaServicesConf import ThinningSvc as _ThinningSvc
from AthenaServicesConf import CoreDumpSvc as _CoreDumpSvc
from AthenaServicesConf import EvtIdModifierSvc as _EvtIdModifierSvc

## import configurables module facade
from AthenaCommon import CfgMgr

class ThinningSvc( _ThinningSvc ):

    __slots__ = ( )      # enforce no new properties

    def __init__(self, name = "ThinningSvc", **kwargs ):

        # have to call base init
        kwargs['name'] = name
        super( ThinningSvc, self ).__init__( **kwargs )

        # initialize the 'Streams' property with the default value
        # to prevent people messing up with the 'Streams' array
        if not 'Streams' in kwargs:
            self.Streams = self.getDefaultProperty('Streams')[:]
            
        return

    def setDefaults( cls, handle ):

        ## continue, only if it is our Configurable
        if not isinstance(handle, ThinningSvc):
            return

        from AthenaCommon.AlgSequence import AlgSequence
        ## get a handle on the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc as toolSvc
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        from AthenaCommon.Logging import logging
        msg = logging.getLogger( handle.name() )

        from CLIDComps.clidGenerator import clidGenerator
        cliddb = clidGenerator(db=None)

        def _clid_from_string(s):
            """return the clid (integer) from a string (which can either
            contain the integer or the clid-name associated with it.)
            """
            try:
                clid = int(s)
            except ValueError:
                clid = cliddb.getClidFromName(s)
                if clid is None or clid == 'None':
                    msg.warning('could not infer clid-name for: "%s"', s)
                    clid = None
            return clid

        from PyUtils.Decorators import memoize
        @memoize
        def _retrieve_items_from_input():
            items = []
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            import PyUtils.PoolFile as _pf
            # only inspect the first input file
            # that should be enough as we don't really support varying schema
            # shapes anymore (as ROOT doesn't)
            items = _pf.extract_items(svcMgr.EventSelector.InputCollections[0])
            input_items = []
            for item in items:
                if item[0] is None:
                    continue
                clid = _clid_from_string(item[0])
                if clid is None:
                    msg.warning('could not infer clid for: "%s"', item[0])
                    clid = item[0] # put back the original string, then.
                input_items.append([clid, item[1]])
            return input_items
        
        def _doScheduleTool(outStream, streamNames):
            outStreamName = outStream.name()
            return outStreamName in streamNames

        def _build_proxy_list(outStream):
            """helper to access the 'ForceRead' and 'TakeItemsFromInput'
            properties of the output stream to build a list of pairs (clid,key)
            the thinning tool will forcingly read during the pre-execute step.
            """
            from collections import defaultdict
            proxies = defaultdict(list)
            props = outStream.properties()
            
            from_input = outStream.properties()['TakeItemsFromInput']
            if from_input == outStream.propertyNoValue:
                from_input = outStream.getDefaultProperty('TakeItemsFromInput')

            ## fetch the items to be written out
            itemlist = outStream.properties()['ItemList'][:]
            for item in itemlist:
                clid_or_name, sg_key = item.split('#')
                clid_or_name = clid_or_name.strip()
                if clid_or_name.endswith('!'): # Strip exact flag.
                    clid_or_name = clid_or_name[:-1]
                sg_key = sg_key.strip()
                clid_or_name = _clid_from_string(clid_or_name)
                if clid_or_name is None:
                    msg.warning('could not infer clid for: "%s"', item)
                else:
                    if len(sg_key) == 0:
                        msg.warning('invalid key for item: "%s"', item)
                    else:
                        proxies[clid_or_name].append(sg_key)

            if from_input:
                input_items = _retrieve_items_from_input()
                for clid,sg_key in input_items:
                    proxies[clid].append(sg_key)
            d = proxies
            proxies = []
            for clid,sg_keys in d.iteritems():
                sg_keys = list(set(sg_keys))
                proxies.extend([[str(clid),sg_key] for sg_key in sg_keys])
            return proxies
        
        # filter out duplicates
        streams = [o for o in set(handle.Streams[:])]
        
        ## list of streams we know we will apply thinning on
        outstreams = []
        
        ## connect @c ThinningOutputTool into the HelperTools slot
        AthenaOutputStream = CfgMgr.AthenaOutputStream

        ## first loop over TopAlg (as output stream can be located
        ## into that sequence)
        for o in AlgSequence("TopAlg"):
            if (isinstance(o, AthenaOutputStream) and
                hasattr(o, 'HelperTools') and
                _doScheduleTool(o, streams)):
                outstreams.append (o)
                pass
        ## then loop over OutStream sequence
        if AlgSequence.configurables.has_key('Streams'):
            for o in AlgSequence("Streams"):
                if (isinstance(o, AthenaOutputStream) and
                    hasattr(o, 'HelperTools') and
                    _doScheduleTool(o, streams)):
                    outstreams.append (o)
                    pass

        ## set the streams we found
        setattr(handle, 'Streams', [o.name() for o in outstreams])

        if len(outstreams) <= 0:
            handle.Streams = []
            msg.warning("No output stream will be thinned !")
            msg.warning("Check your jobOptions configuration")
            msg.warning("...performing hara-kiri...")
            delattr (svcMgr, handle.name())
            del handle
            return
        
        _lvl = handle.properties()['OutputLevel']
        if _lvl == handle.propertyNoValue:
            _lvl = handle.getDefaultProperty('OutputLevel')
            pass
        ## get and install the ThinningOutputTool configurable
        from AthenaServicesConf import ThinningOutputTool
        toolName = "ThinningTool_%s" % handle.name()
        tool = ThinningOutputTool (toolName,
                                   OutputLevel=_lvl,
                                   ThinningSvc=handle)
        tool.Proxies = []
        for o in outstreams:
            tool.Proxies += _build_proxy_list(o)
            o.HelperTools.insert(0, tool)
            ## XXX FIXME XXX
            ## see https://savannah.cern.ch/bugs/index.php?40823
            o.HelperTools += [] # just to work-around bug #40823
        ##
        return
    pass # class ThinningSvc

def createThinningSvc(svcName = "ThinningSvc", outStreams = []):
    """Helper method to create a completely configured ThinningSvc.
    It will take care of adding the helper ThinningOutputTool to the list of
    'outStreams' it has been given.
    Note that 'outStreams' elements have to be AthenaOutputStreams !
    """
    from AthenaCommon.Logging import logging
    msg = logging.getLogger( svcName )
    if len(outStreams) == 0:
        msg.warning("Empty list of (to be thinned) output streams !")
        msg.warning("Check your jobOptions configuration")
        return

    # filter out duplicates
    outStreams = [ o for o in set(outStreams) ]
    
    allGood = True
    AthenaOutputStream = CfgMgr.AthenaOutputStream
    for i,o in enumerate(outStreams):
        if not isinstance(o, AthenaOutputStream):
            msg.error("output stream #%i (n='%s') is not an instance of type "
                      "AthenaOutputStream [type: %r]" % (i,o.name(),type(o)))
            msg.error("check your configuration !")
            allGood = False
    if not allGood:
        return
    del allGood

    outStreams = [o.name() for o in outStreams]
    svc = ThinningSvc(svcName, Streams=outStreams)
    return svc


class CoreDumpSvc( _CoreDumpSvc ):
   
   __slots__ = ()  # enforce no new properties

   def __init__(self, name = "CoreDumpSvc", **kwargs):
      
      # have to call base init
      kwargs['name'] = name
      super( CoreDumpSvc, self ).__init__( **kwargs )
      
      # make sure the application manager explicitly creates the service
      from AthenaCommon.AppMgr import theApp
      handleName = self.getFullJobOptName()
      if not handleName in theApp.CreateSvc:
         theApp.CreateSvc += [ handleName ]
         
      return

   pass # class CoreDumpSvc

###
class EvtIdModifierSvc(_EvtIdModifierSvc):
    __slots__ = ()
    def __init__(self, name="EvtIdModifierSvc", **kwargs):
        kwargs['name'] = name
        _EvtIdModifierSvc.__init__(self, **kwargs)

    def add_modifier(self,
                     run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None, 
                     nevts=1):
        if run_nbr is None:
            modify_run_nbr = 0
            run_nbr = 0
        else:
            modify_run_nbr = 1
            
        if evt_nbr is None:
            modify_evt_nbr = 0
            evt_nbr = 0
        else:
            modify_evt_nbr = 1
            
        if time_stamp is None:
            modify_time_stamp = 0
            time_stamp = 0
        else:
            modify_time_stamp = 1

        if lbk_nbr is None:
            modify_lbk_nbr = 0
            lbk_nbr = 0
        else:
            modify_lbk_nbr = 1

        mod_bit = int(0b0000
                      | (modify_run_nbr << 0)
                      | (modify_evt_nbr << 1)
                      | (modify_time_stamp << 2)
                      | (modify_lbk_nbr << 3))

        self.Modifiers += [run_nbr, evt_nbr, time_stamp, lbk_nbr,
                           nevts, mod_bit]
        return
    
