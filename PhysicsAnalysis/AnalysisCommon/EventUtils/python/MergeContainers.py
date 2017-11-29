# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        MergeContainers
##
## Author:      Carsten Burgard
## Created:     April 2015
##
## Description: This short algorithm can add a list of input
##              containers of the SAME TYPE together and record the
##              resulting container back into StoreGate. This is based
##              upon the defunct AddContainers algorithm from Karsten Koeneke.
## =============================================================================

__doc__ = """This short algorithm can add a list of input containers of the
SAME TYPE together and record the resulting container back into StoreGate.
"""
__version__ = "0.0.1"
__author__  = "Carsten Burgard <cburgard@cern.ch>"


# Import the needed modules
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import gc

class MergeContainers( PyAthena.Alg ):
    """
    This short algorithm can add a list of input containers of the
    SAME TYPE together and record the resulting container back into StoreGate.
    """

    def __init__ ( self, name = "MergeContainers", **kw ):
        ## initialize base class
        kw['name'] = name
        super(MergeContainers, self).__init__(**kw)

        # try to figure out the particle type
        ptype = kw.get( 'ParticleType', "xAOD::IParticle" )
        try:
            self.particleType         = getattr( PyAthena.xAOD, ptype )
            self.particleTypeName     = ptype
        except AttributeError:
            found = False
            d = dict(PyAthena.xAOD.__dict__)
            for key in d.__dict__.keys():
                print(key)
                if key.startswith(ptype+"_v"):
                    self.particleType = getattr( PyAthena.xAOD, key )
                    self.particleTypeName     = key
                    found = True
                    break
            if not found:
                raise RuntimeError("unknown particle type '{:s}'".format(ptype))

        ## Define the container types
        self.inputContainerType   = kw.get( 'InputContainerType',  "" )
        self.inputContainerNames  = kw.get( 'InputContainerNames', [] )
        self.outputContainerName  = kw.get( 'OutputContainerName', "" )

        self.printInternalCutflow = kw.get( 'PrintInternalCutflow', False )
        self.minNumberPassed      = kw.get( 'MinNumberPassed', float("-inf") )
        self.maxNumberPassed      = kw.get( 'MaxNumberPassed', float("inf") )
        ## Get the storgate handle
        self.storeGateSvc = None

        return

    def initialize(self):
        self.msg.debug( '************************************' )
        self.msg.debug( '==> initialize %s...',       self.name() )
        self.msg.debug( '  inputContainerType  = %r', self.inputContainerType  )
        self.msg.debug( '  particleType        = %r', self.particleTypeName    )
        self.msg.debug( '  inputContainerNames = %r', self.inputContainerNames )
        self.msg.debug( '  outputContainerName = %r', self.outputContainerName )
        self.msg.debug( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed        = 0
        self.nObjectsProcessed = 0
        self.nObjectsPassed    = 0
        self.nEventAccepted    = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer!")
            return StatusCode.Failure

        gc.disable()
        return StatusCode.Success

    def execute(self):
        self.outCont = None
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        # Get the input collections from StoreGate and add each to the list of containers for this event
        nInputElements = 0
        inContList = []
        for i in xrange( self.inputContainerNames.__len__() ) :
            inCont = None
            try:
                inCont = self.storeGateSvc.retrieve( self.inputContainerType, self.inputContainerNames[i] )
            except LookupError:
                if self.nProcessed < 100:
                    self.msg.warning( 'Collection of type %s with key %s not found' % (self.inputContainerType, self.inputContainerNames[i]) )
                    pass #continue
                pass
            if inCont:
                inContList.append( inCont )
                nInputElements += inCont.size()
            pass
        self.msg.debug( 'Found %s input containers.' % len(inContList) )
        
        self.msg.debug( 'allocating containers')
        # Create the new output container
        # for now, we only create view containers, because deep copies are not yet supported
        self.outCont = PyAthena.DataVector( self.particleType )(1)
        self.outCont.reserve( nInputElements )

        self.msg.debug( 'recording containers')
        # Record the merged output container into StoreGate so that they can be retrieved by other algorithms
        if self.storeGateSvc.record( self.outCont, self.outputContainerName ) != StatusCode.Success :
            self.msg.error( 'Could not record the output container into StoreGate with the key = ' % self.outputContainerName )
            pass

        #Then... here we go!

        # Get the input collections from StoreGate
        for inCont in inContList :
            self.msg.verbose( 'Processing input container with %s elements...' % len(inCont) )
        
            # Loop over all objects in the input container and add them to the output
            for obj in inCont :
                self.nObjectsProcessed += 1
                self.msg.verbose( '  Processing object in input container...' )
                # TODO: maybe reimplement removeIdentical/removeNearby functionality here
                self.outCont.push_back( obj )
            pass

        # Determine if this events passes the minimum/maximum number of required objects
        nPassedObject = self.outCont.size()
        self.nObjectsPassed += nPassedObject
        if nPassedObject >= self.minNumberPassed and nPassedObject <= self.maxNumberPassed :
            self.msg.debug( 'Found %s objects in this event... passing it.' % nPassedObject )
            self.nEventAccepted += 1
            self.setFilterPassed(True)
            pass
        else:
            self.setFilterPassed(False)
            pass

        self.msg.debug( 'returning')
        if gc.isenabled():
            self.msg.debug( 'garbage collection is enabled' )
        else:
            self.msg.debug( 'garbage collection is disabled' )
        return StatusCode.Success

    def finalize(self):
        ## Import needed modules
        import math
        import PyUtils.Logging

        # Only dump the summary if requested or in DEBUG/VERBOSE mode
        if self.printInternalCutflow or self.msg.isEnabledFor( PyUtils.Logging.logging.DEBUG ) :
            # First calcultate the ratios
            objRatio = 0.0
            if self.nObjectsProcessed != 0 : objRatio = self.nObjectsPassed / self.nObjectsProcessed
            evtRatio = 0.0
            if self.nProcessed != 0 : evtRatio = self.nEventAccepted / self.nProcessed

            # Now, do the printout
            self.msg.info( '================================================================== begin ===================================================================' )
            self.msg.info( '                                                                                           ::         objects       ::         events       ' )
            self.msg.info( 'Event filter description                                                                   :: N accepted : ratio[%] :: N accepted : ratio[%]' )
            self.msg.info( '--------------------------------------------------------------------------------------------------------------------------------------------' )
            self.msg.info( 'Total number of objects/events processed                                                   :: %10u :  100.000 :: %10u :  100.000'
                           % ( self.nObjectsProcessed, self.nProcessed ) )
            self.msg.info( 'Final count                                                                                :: %10u : %8.5g :: %10u : %8.5g'
                           % ( self.nObjectsPassed, objRatio, self.nEventAccepted, evtRatio ) )
            self.msg.info( '=================================================================== end ====================================================================' )
            pass

        return StatusCode.Success
