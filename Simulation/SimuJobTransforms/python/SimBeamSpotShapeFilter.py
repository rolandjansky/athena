# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id $
"""
This library defines a class for filtering events based on the beamspot size in athena.
"""
__author__  = 'Anthony Morley'
__version__ = '$Id $'


# General setup
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from AthenaCommon.AppMgr import ServiceMgr

#
# Event filtering based on signal vertex position
#
import ROOT
import math

class SimBeamSpotShapeFilter( PyAthena.AthFilterAlgorithm ):

    """
    This class is a short algorithm to change the beamspot size.
    By default it will obatain the simulated beamspot from tag associated to the file unless an 
    initialBStag is specified.

    Target BS size will be taken from either the sepcified values or a supplied BS tag.

    Possible inputs are
    targetSigmaX
    targetSigmaY
    targetSigmaZ
    targetBStag
    initialBStag
    """ 

    # Constructor
    def __init__(self,name='SimBeamSpotShapeFilter', **kw):
        kw['name'] = name
        super(SimBeamSpotShapeFilter,self).__init__(**kw)
        self.targetSigmaX = kw.get('targetSigmaX', -999. )
        self.targetSigmaY = kw.get('targetSigmaY', -999. )
        self.targetSigmaZ = kw.get('targetSigmaZ', -999. )
        self.targetBStag  = kw.get('targetBStag', '' )
        self.initialBStag = kw.get('initialBStag', '' )
        self.initialSigmaX = -999.
        self.initialSigmaY = -999.
        self.initialSigmaZ = -999.
        self.initialPosX   = -999.
        self.initialPosY   = -999.
        self.initialPosZ   = -999.
        self.initialTiltXZ = -999.
        self.initialTiltYZ = -999.

        return

    # Initialize function --  get services  
    def initialize(self):
        self.msg.info( '==> Initialize  %s...' % self.name() )

        ## Initialize the counters
        self.nProcessed = 0
        self.nEventPassed  = 0

        if self.initialBStag == '' :
          self.msg.info('Initial width will be taken from associated BS tag'  )
        else:
          self.msg.info('Initial width will be taken from BS tag:  %s' % ( self.initialBStag ) )

        if self.targetBStag == '' :
          self.msg.info('Target width (%f, %f, %f)' % (self.targetSigmaX,self.targetSigmaY,self.targetSigmaZ) )
        else: 
          self.msg.info('Target width will be taken from BS tag:  %s' % ( self.targetBStag ) )
          
        self.sg = PyAthena.py_svc('StoreGateSvc')
        self.bs = PyAthena.py_svc('BeamCondSvc', iface='IBeamCondSvc' )
        return StatusCode.Success

    # Calculate the prob a event falling in the window given the original and target widths
    def calcScale(self, sigmaO, sigmaT, x):
        #if the beamspot are the same to 1 nm don't reweight
        if math.fabs( sigmaO - sigmaT ) < 1e-6:
          return 1.
        if sigmaO < sigmaT:
          self.msg.error( 'This will not work target width larger than original width: %f <  %f '  %(sigmaO, sigmaT) )
          return 1.
        value =   math.exp( -0.5*(x*x)*(1./(sigmaT*sigmaT) - 1./(sigmaO*sigmaO)))
        #print 'Targetn Original Prob ',x, ' ', sigmaT, ' ',  sigmaO, ' ', value
        return value

    # Event execute
    def execute(self):

        initialPosX = self.bs.beamPos().x()
        initialPosY = self.bs.beamPos().y()
        initialPosZ = self.bs.beamPos().z()
        initialSigmaX = self.bs.beamSigma(0)
        initialSigmaY = self.bs.beamSigma(1)
        initialSigmaZ = self.bs.beamSigma(2)
        initialTiltXZ = self.bs.beamTilt(0)
        initialTiltYZ = self.bs.beamTilt(1)       

        self.msg.verbose('Intial position from DB (%f, %f, %f)' % ( initialPosX, initialPosY, initialPosZ ) )
        self.msg.verbose('Intial width from DB (%f, %f, %f)' % ( initialSigmaX, initialSigmaY, initialSigmaZ ) )

        if self.targetBStag != '' or self.initialBStag != '' :
          #Get Event info object
          eventInfo = self.sg.retrieve( 'EventInfo',"McEventInfo")
          iov = eventInfo.event_ID().run_number() << 32 | ( eventInfo.event_ID().lumi_block() & 0xFFFFFFFF ) 
          
          #Get BS from database
          from CoolConvUtilities import AtlCoolLib
          from PyCool import cool
          cooldbBS = AtlCoolLib.indirectOpen('COOLOFL_INDET/OFLP200', True, True, False)
          folderBS = cooldbBS.getFolder('/Indet/Beampos')

          if self.initialBStag != '': 
            self.msg.info('Taking initial beamspot information from conditions database: %s' % self.initialBStag )
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
            self.msg.info('Intial BS position from tag  (%f, %f, %f)' % (self.initialPosX,self.initialPosY,self.initialPosZ ) )
            self.msg.info('Intial BS width from tag (%f, %f, %f)' % (self.initialSigmaX,self.initialSigmaY,self.initialSigmaZ ) ) 

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
   
        targetSigmaX = self.targetSigmaX if self.targetSigmaX > 0. else initialSigmaX
        targetSigmaY = self.targetSigmaY if self.targetSigmaY > 0. else initialSigmaY
        targetSigmaZ = self.targetSigmaZ if self.targetSigmaZ > 0. else initialSigmaZ
 
        initialSigmaX = self.initialSigmaX if self.initialSigmaX > 0. else initialSigmaX
        initialSigmaY = self.initialSigmaY if self.initialSigmaY > 0. else initialSigmaY
        initialSigmaZ = self.initialSigmaZ if self.initialSigmaZ > 0. else initialSigmaZ
        initialPosX = self.initialPosX if self.initialPosX > -999. else initialPosX
        initialPosY = self.initialPosY if self.initialPosY > -999. else initialPosY
        initialPosZ = self.initialPosZ if self.initialPosZ > -999. else initialPosZ
        initialTiltXZ = self.initialTiltXZ if self.initialTiltXZ > -999. else initialTiltXZ
        initialTiltYZ = self.initialTiltYZ if self.initialTiltYZ > -999. else initialTiltYZ


        self.msg.verbose('Intial position used (%f, %f, %f)' % ( initialPosX, initialPosY, initialPosZ ) )
        self.msg.verbose('Intial width used (%f, %f, %f)' % ( initialSigmaX, initialSigmaY, initialSigmaZ ) )
        self.msg.verbose('Target width used (%f, %f, %f)' % ( targetSigmaX,  targetSigmaY,  targetSigmaZ ) )

        #Get Signa event
        truthEventCollection = self.sg.retrieve( "McEventCollection", "TruthEvent")
        # get GenEvent
        genEvent  = truthEventCollection[0]
        # get Signal Vertex
        sigVtx  = genEvent.signal_process_vertex()

        deltaZ = initialPosZ - sigVtx.point3d().z()
        deltaX = initialPosX + deltaZ * initialTiltXZ - sigVtx.point3d().x()
        deltaY = initialPosY + deltaZ * initialTiltYZ - sigVtx.point3d().y()
        # Calculate prob of keeping this event
        weight =  self.calcScale( initialSigmaX, targetSigmaX, deltaX) \
                  * self.calcScale( initialSigmaY, targetSigmaY, deltaY) \
                  * self.calcScale( initialSigmaZ, targetSigmaZ, deltaZ)

        # Decide if you keep 
        accept =  weight > ROOT.gRandom.Rndm()
        self.setFilterPassed(accept)
        
        self.nProcessed += 1
        if accept:
          self.nEventPassed  += 1
        
        return StatusCode.Success

    # Finilize
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
        self.msg.info( '**********************************************************************' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events accepted:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassed, effiEvents, effiErrEvents ) )
        self.msg.info( '**********************************************************************' )

        return StatusCode.Success


