# $Id $
"""
This library defines different classes for filtering events in athena.
"""
__author__  = 'Anthony Morley'
__version__ = '$Id $'


# General setup
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena

#
# Event filtering based on z of primary vertex position
#
import ROOT
import math

class SimBeamSpotShapeFilter( PyAthena.AthFilterAlgorithm ):

    """
    This class is a short algorithm to change the beamspot size

    Possible inputs are
    initialSigmaX
    initialSigmaY
    initialSigmaZ
    initialPosX
    initialPosY
    initialPosZ
    targetSigmaX
    targetSigmaY
    targetSigmaZ
    initialBStag
    targetBStag
    """ 

    def __init__(self,name='SimBeamSpotShapeFilter', **kw):
        kw['name'] = name
        super(SimBeamSpotShapeFilter,self).__init__(**kw)
        self.initialSigmaX = kw.get('initialSigmaX', 0.010 )
        self.initialSigmaY = kw.get('initialSigmaY', 0.010 )
        self.initialSigmaZ = kw.get('initialSigmaZ', 42.00 )
        self.initialPosX = kw.get('initialPosX', -0.50 )
        self.initialPosY = kw.get('initialPosY', -0.50 )
        self.initialPosZ = kw.get('initialPosZ',  0.00 )
        self.initialTiltXZ = kw.get('initialTiltXZ',  0.0 )
        self.initialTiltYZ = kw.get('initialTiltYZ',  0.0 )
        self.targetSigmaX = kw.get('targetSigmaX', self.initialSigmaX )
        self.targetSigmaY = kw.get('targetSigmaY', self.initialSigmaY )
        self.targetSigmaZ = kw.get('targetSigmaZ', self.initialSigmaZ )
        self.initialBStag = kw.get('initialBStag', '' )
        self.targetBStag = kw.get('targetBStag', '' )

        return

    def initialize(self):
        self.msg.info( '==> Initialize  %s...' % self.name() )

        ## Initialize the counters
        self.nProcessed = 0
        self.nEventPassed  = 0

        if self.initialBStag == '':
          self.msg.info('Intial position (%f, %f, %f)' % (self.initialPosX,self.initialPosY,self.initialPosZ ) ) 
          self.msg.info('Intial width (%f, %f, %f)' % (self.initialSigmaX,self.initialSigmaY,self.initialSigmaZ ) ) 
        else:
          self.msg.info('Initial width will be taken from BS tag:  %s' % ( self.initialBStag ) )

        if self.targetBStag == '' :
          self.msg.info('Target width (%f, %f, %f)' % (self.targetSigmaX,self.targetSigmaY,self.targetSigmaZ) )
        else: 
          self.msg.info('Target width will be taken from BS tag:  %s' % ( self.targetBStag ) )
          
        self.sg = PyAthena.py_svc('StoreGateSvc')
        return StatusCode.Success

    def calcScale(self, sigmaO, sigmaT, x):
        if sigmaO < sigmaT:
          self.msg.error( 'This will not work target width larger than original width: %f <  %f'  %(sigmaO, sigmaT) )
          return 1.
        if sigmaO == sigmaT:
          return 1.
        value =   math.exp( -0.5*(x*x)*(1./(sigmaT*sigmaT) - 1./(sigmaO*sigmaO)))
        #print 'Targetn Original Prob ',x, ' ', sigmaT, ' ',  sigmaO, ' ', value
        return value

    def execute(self):
        
        if self.initialBStag != '' or self.targetBStag != '':
          #Get Event info object
          eventInfo = self.sg.retrieve( 'EventInfo',"McEventInfo")
          iov = eventInfo.event_ID().run_number() << 32 | ( eventInfo.event_ID().lumi_block() & 0xFFFFFFFF ) 
          
          #Get BS from database
          from CoolConvUtilities import AtlCoolLib
          cooldbBS = AtlCoolLib.indirectOpen('COOLOFL_INDET/OFLP200', True, True, False)
          if self.intialBStag != '': 
            self.msg.info('Taking initial beamspot information from conditions database: %s' % self.intialBStag )
            itrBS = folderBS.browseObjects(iov, iov, cool.ChannelSelection.all(), self.initialBStag )
            while itrBS.goToNext():
              obj = itrBS.currentRef()
              self.initialSigmaX = float(obj.payloadValue("sigmaX"))
              self.initialSigmaY = float(obj.payloadValue("sigmaY"))
              self.initialSigmaZ = float(obj.payloadValue("sigmaZ"))
              self.initialTiltXZ = float(obj.payloadValue("tiltX"))
              self.initialTiltYZ = float(obj.payloadValue("tiltY"))
              self.initialPosX = float(obj.payloadValue("posX"))
              self.initialPosY = float(obj.payloadValue("posY"))
              self.initialPosZ = float(obj.payloadValue("posZ"))
              break
            self.initialBStag = ''
            self.msg.info('Intial position (%f, %f, %f)' % (self.initialPosX,self.initialPosY,self.initialPosZ ) )
            self.msg.info('Intial width (%f, %f, %f)' % (self.initialSigmaX,self.initialSigmaY,self.initialSigmaZ ) )  

          if self.targetBStag != '': 
            self.msg.info('Taking target beamspot information from conditions database: %s' % self.targetBStag )
            self.msg.info('Only widths are considered not positions!!!')
            itrBS = folderBS.browseObjects(iov, iov, cool.ChannelSelection.all(), self.targetBStag )
            while itrBS.goToNext():
              obj = itrBS.currentRef()
              self.targetSigmaX = float(obj.payloadValue("sigmaX"))
              self.targetSigmaY = float(obj.payloadValue("sigmaY"))
              self.targetSigmaZ = float(obj.payloadValue("sigmaZ"))
              break
            self.targetBStag = ''
            self.msg.info('Target width (%f, %f, %f)' % (self.targetSigmaX,self.targetSigmaY,self.targetSigmaZ ) )  
 

       
        #Get Signa event
        truthEventCollection = self.sg.retrieve( "McEventCollection", "TruthEvent")
        # get GenEvent
        genEvent  = truthEventCollection[0]
        # get Signal Vertex
        sigVtx  = genEvent.signal_process_vertex()


        accept = False
        deltaZ = self.initialPosZ - sigVtx.point3d().z()
        deltaX = self.initialPosX + deltaZ * self.initialTiltXZ - sigVtx.point3d().x()
        deltaY = self.initialPosY + deltaZ * self.initialTiltYZ - sigVtx.point3d().y()
        # Calculate prob of keeping this event
        weight =  self.calcScale( self.initialSigmaX, self.targetSigmaX, deltaX) \
                  * self.calcScale( self.initialSigmaY, self.targetSigmaY, deltaY) \
                  * self.calcScale( self.initialSigmaZ, self.targetSigmaZ, deltaZ)

        # Decide if you keep 
        accept =  weight > ROOT.gRandom.Rndm()
        self.setFilterPassed(accept)
        
        self.nProcessed += 1
        if accept:
          self.nEventPassed  += 1
        
        return StatusCode.Success

    def finalize(self):
        effiEvents    = 0.0
        effiErrEvents = 0.0
        try :
          effiEvents    = self.nEventPassed / float(self.nProcessed)
          effiErrEvents = 100.0 * math.sqrt(effiEvents*(1.-effiEvents)/float(self.nProcessed))
          effiEvents   *= 100.0 


        except ZeroDivisionError :
          self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
        
        
        self.msg.info( '==> Finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events accepted:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassed, effiEvents, effiErrEvents ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success


