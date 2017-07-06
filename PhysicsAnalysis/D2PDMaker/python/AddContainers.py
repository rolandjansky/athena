# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        AddContainers
##
## Author:      Karsten Koeneke (DESY)
## Created:     November 2009
##
## Description: This short algorithm can add a list of input containers of the
##              SAME TYPE together and record the resulting container back into
##              StoreGate.
##=============================================================================

__doc__ = """This short algorithm can add a list of input containers of the
SAME TYPE together and record the resulting container back into StoreGate.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


# Import the needed modules
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode


class AddContainers( PyAthena.Alg ):
    """
    This short algorithm can add a list of input containers of the
    SAME TYPE together and record the resulting container back into StoreGate.
    """



    def __init__ ( self, name = "AddContainers", **kw ):
        ## initialize base class
        kw['name'] = name
        super(AddContainers, self).__init__(**kw)

        ## Define the cuts
        self.inputContainerType   = kw.get( 'inputContainerType',  "" )
        self.inputContainerParticleType = kw.get( 'inputContainerParticleType', self.inputContainerType )
        self.inputContainerNames  = kw.get( 'inputContainerNames', [] )
        self.outputContainerName  = kw.get( 'outputContainerName', "" )
        self.printInternalCutflow = kw.get( 'printInternalCutflow', False )
        self.minNumberPassed      = kw.get( 'minNumberPassed', -999999999999 )
        self.maxNumberPassed      = kw.get( 'maxNumberPassed', 999999999999 )
        self.removeIdentical      = kw.get( 'removeIdentical', True )
        self.removeNearby         = kw.get( 'removeNearby',    False )
        self.maxDeltaR            = kw.get( 'maxDeltaR', 0.01 )
        self.attribute            = kw.get( 'useAttribute',  "" ) # This could be, e.g., for egamma objects "cluster"; then, the cluster object will be used for deltaR overlap checks instead of the egamma object
        ## Get the storgate handle
        self.storeGateSvc = None

        return


    

    def initialize(self):
        self.msg.debug( '************************************' )
        self.msg.debug( '==> initialize %s...',       self.name() )
        self.msg.debug( '  inputContainerType = %r',  self.inputContainerType )
        self.msg.debug( '  inputContainerNames = %r', self.inputContainerNames )
        self.msg.debug( '  outputContainerName = %r', self.outputContainerName )
        self.msg.debug( '  removeIdentical = %r',     self.removeIdentical )
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
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }
        
        return StatusCode.Success



    def getFunctor(self, obj):
        if self.attribute != "":
            if hasattr(obj, self.attribute):
                self.msg.debug( "Will use new object of type %s for further computation" % self.attribute )
                return getattr(obj, self.attribute)()
            pass
        return obj
    


    def isOverlapping(self, obj, objCont):
        obj = self.getFunctor(obj)
        for targetObj in objCont:
            targetObj = self.getFunctor(targetObj)
            if self.utils['deltaR'](obj, targetObj) <= self.maxDeltaR :
                return True
            pass
        return False



    def execute(self):
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
        


        # Create the new output container and reserve some memory for it
        try :
            VIEW_ELEMENTS = 1
            outCont = PyAthena.DataVector(getattr( PyAthena.xAOD, self.inputContainerParticleType ) )(VIEW_ELEMENTS)
        except TypeError :
            outCont = PyAthena.DataVector(getattr( PyAthena.xAOD, self.inputContainerParticleType ) )()
            pass
        outCont.reserve( nInputElements )
        

        #Then... here we go!

        # Get the input collections from StoreGate
        for inCont in inContList :
            self.msg.verbose( 'Processing input container with %s elements...' % len(inCont) )
        
            # Loop over all objects in the input container and add them to the output
            for obj in inCont :
                self.nObjectsProcessed += 1
                self.msg.verbose( '  Processing object in input container...' )
                if self.removeIdentical :
                    if not obj in outCont :
                        outCont.push_back( obj )
                        pass
                    pass
                else :
                    if self.removeNearby: 
                        if not self.isOverlapping( obj, outCont ):
                            outCont.push_back( obj )
                            pass
                        pass
                    else:
                        outCont.push_back( obj )
                        pass
                    pass
                pass
            
            pass

        # Determine if this events passes the minimum/maximum number of required objects
        nPassedObject = outCont.size()
        self.nObjectsPassed += nPassedObject
        if nPassedObject >= self.minNumberPassed and nPassedObject <= self.maxNumberPassed :
            self.msg.debug( 'Found %s objects in this event... passing it.' % nPassedObject )
            self.nEventAccepted += 1
            self.setFilterPassed(True)
            pass
        else:
            self.setFilterPassed(False)
            pass
        

        # Record the merged output container into StoreGate so that they can be retrieved by other algorithms
        if self.storeGateSvc.record( outCont, self.outputContainerName ) != StatusCode.Success :
            self.msg.error( 'Could not record the output container into StoreGate with the key = ' % self.outputContainerName )
            pass

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
