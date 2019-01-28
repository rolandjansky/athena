# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/AlgSequence.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)
# Author: Sebastien Binet (binet@cern.ch)

__version__ = '$Revision: 1.21 $'
__author__  = """Wim Lavrijsen (WLavrijsen@lbl.gov)
                 Sebastien Binet <binet@cern.ch>
                 """

__all__ = [ 'AlgSequence', 'AthSequencer',
            'iter_algseq',
            'dumpSequence',
            'dumpMasterSequence']

import GaudiSequencer.GaudiSequencerConf as GaudiSequencerConf


### sequence of Gaudi algorithms, to replace the generated configurable

# create a new base class type to replace the old configurable
class AthSequencer( GaudiSequencerConf.AthSequencer ):
    """Sequence of Gaudi algorithms"""

    def __init__( self, name = "AthSequencer", **kwargs ):
        # call base class __init__ to pass new name
        super( AthSequencer, self ).__init__( name, **kwargs )
 
    def getProperties( self ):
       ## call base class
       props = super( AthSequencer, self ).getProperties()

       ## correctly display the value of 'Members' by gathering children
       if 'Members' in props:
          props['Members'] = [ c.getFullName() for c in self.getChildren() ]
       return props

    def insert( self, index, item ):
        self.__iadd__( item, index = index )

    def setup( self ):

       ## synchronize the list of Members with our Configurable children
       self.Members = [ c.getFullName() for c in self.getChildren() ]

       import Logging
       msg = Logging.logging.getLogger( "AthSequencer" )
       msg.debug( 'setup of sequence: %s', self.getName() )
       if msg.isEnabledFor( Logging.logging.VERBOSE ):
          # call of __repr__ is relatively expensive
          msg.verbose( 'layout of sequence: %s\n%s', self.getName(), str(self) )

       ## delegate to base class...
       super( AthSequencer, self ).setup()

# store the new AthSequencer into CfgMgr to make it available
import CfgMgr
setattr( CfgMgr, 'AthSequencer', AthSequencer )
del CfgMgr

### AthRetrySequencer ----------------------------------------------------------
if hasattr(GaudiSequencerConf, 'AthRetrySequencer'):
    # create a new base class type to replace the old configurable
    class AthRetrySequencer( GaudiSequencerConf.AthRetrySequencer ):
        """Sequence of Gaudi algorithms"""

        def __init__( self, name = "AthRetrySequencer", **kwargs ):
            # call base class __init__ to pass new name
            super( AthRetrySequencer, self ).__init__( name, **kwargs )

        def getProperties( self ):
            ## call base class
            props = super( AthRetrySequencer, self ).getProperties()

            ## correctly display the value of 'Members' by gathering children
            if 'Members' in props:
                props['Members'] = [ c.getFullName() for c in self.getChildren() ]
            return props

        def insert( self, index, item ):
            self.__iadd__( item, index = index )

        def setup( self ):

            ## synchronize the list of Members with our Configurable children
            self.Members = [ c.getFullName() for c in self.getChildren() ]

            import Logging
            msg = Logging.logging.getLogger( "AthRetrySequencer" )
            msg.debug( 'setup of sequence: %s', self.getName() )
            if msg.isEnabledFor( Logging.logging.VERBOSE ):
                # call of __repr__ is relatively expensive
                msg.verbose( 'layout of sequence: %s\n%s', self.getName(), str(self) )

            ## delegate to base class...
            super( AthRetrySequencer, self ).setup()
        pass # AthRetrySequencer
    # store the new AthRetrySequencer into CfgMgr to make it available
    import CfgMgr
    setattr( CfgMgr, 'AthRetrySequencer', AthRetrySequencer )
    del CfgMgr
    pass # monkey-patching AthRetrySequencer


### sequence of Gaudi configurables
class AlgSequence( AthSequencer ):
    __slots__ = ()
    def __init__( self, name="TopAlg", **kwargs ):
        # call base class __init__ to pass new name
        super( AlgSequence, self ).__init__( name, **kwargs )


### helper functions ----------------------------------------------------------
def iter_algseq(seq):
    """iterate over a (possibly nested) ``seq`` AlgSequence.
    
    if the sequence contains nested sub-sequences, everything will be flatten
    out in-order.

    example:
    >>> import AthenaCommon.AlgSequence as acas
    >>> aaa = CfgMgr.AthSequencer('aaa')
    >>> aaa += CfgMgr.AthSequencer('aaa1')
    >>> aaa.aaa1 += CfgMgr.AthSequencer('aaa2')
    >>> aaa.aaa1.aaa2 += CfgMgr.AthSequencer('aaa3')
    >>> aaa.aaa1.aaa2.aaa3 += CfgMgr.AthSequencer('aaa4')
    >>> aaa.aaa1.aaa2.aaa3.aaa4 += CfgMgr.AthSequencer('aaa5')
    >>> aaa += CfgMgr.AthSequencer('aaa11')
    >>> aaa.aaa11 += CfgMgr.AthSequencer('aaa21')
    >>> print [c.getName() for c in acas.iter_algseq(aaa)]
    ['aaa', 'aaa1', 'aaa2', 'aaa3', 'aaa4', 'aaa5', 'aaa11', 'aaa21']
    """
    def _iter_algseq(seq):
        yield seq
        for c in seq.getChildren():
            for sub in _iter_algseq(c):
                yield sub
    return _iter_algseq(seq)

def dumpSequence (seq, indent=0):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():
            o.append( (c.getFullName(), indent) )
            o.extend( _dump (c, indent+1) )
        return o
    out = [(seq.getFullName(),indent)]
    out.extend( _dump( seq, indent=indent+1 ) )
    for n,i in out:
        print "  "*i,"+--",n

def dumpMasterSequence():
    """
    Helper function to display on screen the current master sequencer layout
    """
    # if the application manager has already been setup()'d we just have to
    # dump the AthMasterSeq as all the work has been done for us
    from AppMgr import theApp, AthAppMgr
    app_state = theApp.state()
    if app_state != AthAppMgr.State.OFFLINE:
        return dumpSequence( AthSequencer ("AthMasterSeq") )

    # otherwise, manually do it...
    dumpSequence( AthSequencer ("AthMasterSeq"), indent=0 )
    dumpSequence( AthSequencer ("AthFilterSeq"), indent=1 )
    dumpSequence( AthSequencer ("AthBeginSeq"),  indent=2 )
    dumpSequence( AthSequencer ("AthCondSeq"),   indent=3 )
    dumpSequence( AlgSequence  ("TopAlg"),       indent=3 )
    dumpSequence( AthSequencer ("AthEndSeq"),    indent=2 )
    dumpSequence( AthSequencer ("AthOutSeq"),    indent=1 )
    dumpSequence( AthSequencer ("AthRegSeq"),    indent=1 )
