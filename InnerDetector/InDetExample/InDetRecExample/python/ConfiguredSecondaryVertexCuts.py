# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
# ConfiguredSecondaryVertexCuts class
#########################################################################
# Python Setup Class for finding V0s
#
#########################################################################

import AthenaCommon.SystemOfUnits as Units

class ConfiguredSecondaryVertexCuts :
   
  def __init__ (self, mode = "PileUp", minPt = 0.0):

    self.__mode      = mode
    self.__extension = ""
    self.__set_indetflags()    # pointer to InDetFlags, don't use them directly
 			       # to allow sharing this code with the trigger
    
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.BeamFlags import jobproperties
    from RecExConfig.RecFlags import rec

    # ------------------------------------------------
    # --- secondary vertexing setup of cuts
    # ------------------------------------------------

    if mode == "PileUp" or mode == "StartUp":

      #InDetSecVxFitterTool
      self.__Fitter_IterationNumber            = 10
      self.__Fitter_MakeExtendedVertex         = False
      self.__Fitter_FirstMeasuredPoint         = True
      self.__Fitter_Robustness                 = 6
      self.__Fitter_InputParticleMasses        = [139.57018,139.57018]
      self.__Fitter_VertexForConstraint        = [0.,0.,0.]
      self.__Fitter_CovVrtForConstraint        = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.]
      self.__Fitter_FirstMeasuredPointLimit    = False
      self.__Fitter_usePhiCnst                 = False
      self.__Fitter_useThetaCnst               = False

      # Track selector tool
      self.__TrkSel_maxSiD0                    = 80.0
      self.__TrkSel_maxTrtD0                   = 10000.
      self.__TrkSel_maxSiZ0                    = 10000.
      self.__TrkSel_maxTrtZ0                   = 10000.
      self.__TrkSel_minPt                      = minPt
      self.__TrkSel_RatioCut1                  = 0.0
      self.__TrkSel_RatioCut2                  = 0.0
      self.__TrkSel_RatioCut3                  = 0.0
      self.__TrkSel_RatioTRT                   = 0.0
      self.__TrkSel_IsConversion               = False
      self.__TrkSel_significanceD0_Si          = 2.   
      self.__TrkSel_RatioV0                    = 0.95  
      
      # Track pairs selector
      self.__TrkPairSel_MaxFirstHitRadius      = 510.
      self.__TrkPairSel_MaxDistBetweenTracks   = [4.,90.,30.] 
      self.__TrkPairSel_MaxEta                 = [1.2,0.9,0.4]
      self.__TrkPairSel_MaxInitDistance        = [100000.,640.,80.]
      self.__TrkPairSel_MinTrackAngle          = 0.6  
      
      # Vertex point estimator
      self.__VtxPt_MinDeltaR                   = [-10000.,-10000.,-10000.]
      self.__VtxPt_MaxDeltaR                   = [10000.,10000.,10000.]
      self.__VtxPt_MaxPhi                      = [10000., 10000., 10000.]
      
      # Secondary Vertex post selector
      self.__SecVtxPost_MaxChi2Vtx             = [15.,50.,25.]
      self.__SecVtxPost_MaxInvariantMass       = [10000.,50.,25.]
      self.__SecVtxPost_MinFitMomentum         = [1100.,2000.,2000.]
      self.__SecVtxPost_MinRadius              = [0.,35.,350.] 
      self.__SecVtxPost_MinPt                  = 0.0       # off
      self.__SecVtxPost_MaxdR                  = -10000.   # off
      self.__SecVtxPost_MaxPhiVtxTrk           = 10000.
    
      # Single track secondary vertex tool
      self.__SingleTrk_MaxBLayerHits              = 0
      self.__SingleTrk_MinInitialHitRadius        = 0.0
      self.__SingleTrk_MinInitialHitRadius_noBlay = 0.0
      self.__SingleTrk_MinRatioOfHLhits           = 0.0
    
      #  InDetSecVtxFinderTool 
      self.__Finder_RemoveTrtTracks            = True
      self.__Finder_IsConversion               = False
      self.__Finder_MinDistVtxHit              = -200.
      self.__Finder_MaxDistVtxHit              = 50.
      self.__Finder_MinFlightAngle             = -0.5
      self.__Finder_MinInitVtxR                = 5.
    
      if mode == "StartUp": #Loose selection for 900GeV (minbias) collisions
        # Track selector tool
        self.__TrkSel_maxSiD0                  = 10000.
        self.__TrkSel_significanceD0_Si        = 3.
        self.__TrkSel_RatioV0                  = 1.
        
        # Track pairs selector
        self.__TrkPairSel_MaxDistBetweenTracks = [10.,10.,10.]
        self.__TrkPairSel_MaxEta               = [4.,4.,4.] 
        self.__TrkPairSel_MinTrackAngle        = -10000.
        
        # Secondary Vertex post selector
        self.__SecVtxPost_MinFitMomentum       = [0.,0.,0.]
        self.__SecVtxPost_MinRadius            = [5.,5.,5.]
        
        #  InDetSecVtxFinderTool 
        self.__Finder_MinDistVtxHit            = -10000.
        self.__Finder_MaxDistVtxHit            = 10000.
        self.__Finder_MinInitVtxR              = 0.
      pass
      
    elif mode == "ConversionStartUp" or mode == "ConversionPileUp" or mode == "EGammaStartUp" or mode == "EGammaPileUp" :

      #InDetSecVxFitterTool
      self.__Fitter_IterationNumber            = 30    # huge number of iterations ?
      self.__Fitter_MakeExtendedVertex         = True
      self.__Fitter_FirstMeasuredPoint         = True
      self.__Fitter_Robustness                 = 6
      self.__Fitter_InputParticleMasses        = [0.511,0.511]
      self.__Fitter_VertexForConstraint        = [0.,0.,0.]
      self.__Fitter_CovVrtForConstraint        = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.]
      self.__Fitter_FirstMeasuredPointLimit    = True
      self.__Fitter_usePhiCnst                 = True
      self.__Fitter_useThetaCnst               = True            
    
      # Track selector tool
      self.__TrkSel_maxSiD0                    = 10000. # off ?
      self.__TrkSel_maxTrtD0                   = 10000. # off ?
      self.__TrkSel_maxSiZ0                    = 10000. # off ?
      self.__TrkSel_maxTrtZ0                   = 10000. # off ?
      self.__TrkSel_minPt                      = minPt
      self.__TrkSel_RatioCut1                  = 0.0    # e-prob: Ntrt < 15
      self.__TrkSel_RatioCut2                  = 0.0    # e-prob: 15 < Ntrt < 25  
      self.__TrkSel_RatioCut3                  = 0.0    # e-prob: Ntrt > 25 
      self.__TrkSel_RatioTRT                   = 0.0    # e-prob cut for TRT only
      self.__TrkSel_IsConversion               = True
      self.__TrkSel_significanceD0_Si          = -1.    # V0 only cuts
      self.__TrkSel_RatioV0                    = -1.    # V0 only cuts
      
      # Track pairs selector
      self.__TrkPairSel_MaxFirstHitRadius      = 500.                         # hacky way to determine if TRT only of SI
      self.__TrkPairSel_MaxDistBetweenTracks   = [10.,50.,50.]                # minimum distance cut at vertex
      self.__TrkPairSel_MaxEta                 = [0.3,0.5,0.5]                # delta cotan(theta) cuts, no cut in barrel for TRT only in code
      self.__TrkPairSel_MaxInitDistance        = [10000.0, 10000.0, 10000.0]  # off
      self.__TrkPairSel_MinTrackAngle          = 0.0                          # off, V0 cut
      
      # Vertex point estimator
      self.__VtxPt_MinDeltaR                   = [-5.,-25.,-50.]              # D-R1-R2 min cut
      self.__VtxPt_MaxDeltaR                   = [5.,10.,10.]                 # D-R1-R2 max cut
      self.__VtxPt_MaxPhi                      = [0.05, 0.5, 0.5]             # dphi cut at vertex
      
      # Secondary Vertex post selector
      self.__SecVtxPost_MaxChi2Vtx             = [50.,50.,50.]                  # was [40.,100.,80.]
      self.__SecVtxPost_MaxInvariantMass       = [10000.,10000.,10000.]         # off, was [60.,60.,30.]
      self.__SecVtxPost_MinFitMomentum         = [0.,0.,0.]                     # off, was [2000.,2000.,2000.]
      self.__SecVtxPost_MinRadius              = [-10000.0, -10000.0, -10000.0] # off
      self.__SecVtxPost_MinPt                  = 0.                             # off
      self.__SecVtxPost_MaxdR                  = -10000.                        # off, -250.
      self.__SecVtxPost_MaxPhiVtxTrk           = 10000.                         # off, was 0.2
    
      # Single track secondary vertex tool
      self.__SingleTrk_MaxBLayerHits              = 0
      self.__SingleTrk_MinInitialHitRadius        = 70.
      self.__SingleTrk_MinInitialHitRadius_noBlay = 70.
      self.__SingleTrk_MinRatioOfHLhits           = 0.95
    
      #  InDetSecVtxFinderTool 
      self.__Finder_RemoveTrtTracks            = False
      self.__Finder_IsConversion               = True
      self.__Finder_MinDistVtxHit              = -350.0     # off
      self.__Finder_MaxDistVtxHit              = 250.0      # off
      self.__Finder_MinFlightAngle             = 0.0        # off
      self.__Finder_MinInitVtxR                = 0.0        # off

      if mode == "ConversionPileUp" or mode == "EGammaPileUp":
        # ------------------------------------
        # --- cut tuning for 17.2
        # ------------------------------------

        # Track selector tool
        self.__TrkSel_minPt                      = minPt
        self.__TrkSel_RatioCut1                  = 0.0    # UNCHANGED, e-prob: Ntrt < 15
        self.__TrkSel_RatioCut2                  = 0.1    # UNCHANGED, e-prob: 15 < Ntrt < 25  
        self.__TrkSel_RatioCut3                  = 0.1    # UNCHANGED, e-prob: Ntrt > 25
        self.__TrkSel_RatioTRT                   = 0.7    # new e-prob cut for TRT only from Bruno
        
        # Track pairs selector
        self.__TrkPairSel_MaxDistBetweenTracks   = [10.*Units.mm,50.*Units.mm,50.*Units.mm]       # UNCHANGED, minimum distance cut at vertex (Mauro 10,100,50)
        self.__TrkPairSel_MaxEta                 = [0.3,0.5,0.5]                                  # UNCHANGED, is delta cotan(theta) cuts, no cut in barrel for TRT only in code
        # ----> replace this with a d(eta) cut by Bruno ?
        # ----> we need a dphi cut at perigee 

        # Vertex point estimator
        self.__VtxPt_MinDeltaR                   = [-5.*Units.mm,-25.*Units.mm,-50.*Units.mm]     # UNCHANGED, D-R1-R2 min cut
        self.__VtxPt_MaxDeltaR                   = [ 5.*Units.mm, 10.*Units.mm, 10.*Units.mm]     # UNCHANGED, D-R1-R2 max cut
        self.__VtxPt_MaxPhi                      = [0.05, 0.2, 0.2]                               # dphi cut at vertex tune for SI+TRT, TRT+TRT
      
        # Secondary Vertex post selector
        self.__SecVtxPost_MaxChi2Vtx             = [50.,50.,50.]                               # UNCHANGED, (Mauro 80,50,50)
        self.__SecVtxPost_MaxInvariantMass       = [10000.,10000.,10000.]                      # UNCHANGED, off
        self.__SecVtxPost_MinFitMomentum         = [0.,0.,0.]                                  # UNCHANGED, off
        self.__SecVtxPost_MinRadius              = [20.*Units.mm,70.*Units.mm,250.*Units.mm]   # new vertex R cuts to remove backgrounds
        self.__SecVtxPost_MinPt                  = 0.                                          # UNCHANGED, off
        self.__SecVtxPost_MaxdR                  = -250.*Units.mm                              # remove vertices way behind last hit, should not cut much
        self.__SecVtxPost_MaxPhiVtxTrk           = 0.2                                         # back to Mauro's cut
    
        # Single track secondary vertex tool
        self.__SingleTrk_MaxBLayerHits              = 0                # UNCHANGED, no blayer hits on single track conversion
        self.__SingleTrk_MinInitialHitRadius        =  70.*Units.mm    # UNCHANGED, start conversion in layer 1
        self.__SingleTrk_MinInitialHitRadius_noBlay = 120.*Units.mm    # but allow for conversion tracks starting in layer 2 if blayer is dead        
        self.__SingleTrk_MinRatioOfHLhits           = 0.7              # e-prob cut, Bruno prefered 0.8

    else:
      print "*********************** ERROR, wrong configuration for Configured Secondary Vertex Cuts !!!"
      print " mode given : ", mode
      
# ----------------------------------------------------------------------------
# --- private method
  def __set_indetflags(self):
    from InDetRecExample.InDetJobProperties import InDetFlags
    self.__indetflags = InDetFlags

# ----------------------------------------------------------------------------
# --- return methods for the cut values - the main purpose of this class
# ----------------------------------------------------------------------------

  def mode( self ) :
    return self.__mode

  def extension( self ) :
    return self.__extension

  def Fitter_IterationNumber( self ) :
    return self.__Fitter_IterationNumber

  def Fitter_MakeExtendedVertex( self ) :
    return self.__Fitter_MakeExtendedVertex

  def Fitter_FirstMeasuredPoint( self ) :
    return self.__Fitter_FirstMeasuredPoint

  def Fitter_Robustness( self ) :
    return self.__Fitter_Robustness

  def Fitter_InputParticleMasses( self ) :
    return self.__Fitter_InputParticleMasses

  def Fitter_VertexForConstraint( self ) :
    return self.__Fitter_VertexForConstraint

  def Fitter_CovVrtForConstraint( self ) :
    return self.__Fitter_CovVrtForConstraint

  def FirstMeasuredPointLimit( self ) :
    return self.__Fitter_FirstMeasuredPointLimit

  def usePhiCnst( self ) :
    return self.__Fitter_usePhiCnst

  def useThetaCnst( self ) :
    return self.__Fitter_useThetaCnst

  def TrkSel_maxSiD0( self ) :
    return self.__TrkSel_maxSiD0

  def TrkSel_maxTrtD0( self ) :
    return self.__TrkSel_maxTrtD0

  def TrkSel_maxSiZ0( self ) :
    return self.__TrkSel_maxSiZ0

  def TrkSel_maxTrtZ0( self ) :
    return self.__TrkSel_maxTrtZ0

  def TrkSel_minPt( self ) :
    return self.__TrkSel_minPt

  def TrkSel_RatioCut1 ( self ) :
    return self.__TrkSel_RatioCut1 

  def TrkSel_RatioCut2( self ) :
    return self.__TrkSel_RatioCut2

  def TrkSel_RatioCut3( self ) :
    return self.__TrkSel_RatioCut3

  def TrkSel_RatioTRT( self ) :
    return self.__TrkSel_RatioTRT

  def TrkSel_IsConversion( self ) :
    return self.__TrkSel_IsConversion

  def TrkSel_significanceD0_Si( self ) :
    return self.__TrkSel_significanceD0_Si

  def TrkSel_RatioV0( self ) :
    return self.__TrkSel_RatioV0

  def TrkPairSel_MaxFirstHitRadius( self ) :
    return self.__TrkPairSel_MaxFirstHitRadius

  def TrkPairSel_MaxDistBetweenTracks( self ) :
    return self.__TrkPairSel_MaxDistBetweenTracks

  def TrkPairSel_MaxEta( self ) :
    return self.__TrkPairSel_MaxEta

  def TrkPairSel_MaxInitDistance( self ) :
    return self.__TrkPairSel_MaxInitDistance

  def TrkPairSel_MinTrackAngle( self ) :
    return self.__TrkPairSel_MinTrackAngle

  def VtxPt_MinDeltaR( self ) :
    return self.__VtxPt_MinDeltaR

  def VtxPt_MaxDeltaR( self ) :
    return self.__VtxPt_MaxDeltaR

  def VtxPt_MaxPhi( self ) :
    return self.__VtxPt_MaxPhi

  def SecVtxPost_MaxChi2Vtx( self ) :
    return self.__SecVtxPost_MaxChi2Vtx

  def SecVtxPost_MaxInvariantMass( self ) :
    return self.__SecVtxPost_MaxInvariantMass

  def SecVtxPost_MinFitMomentum( self ) :
    return self.__SecVtxPost_MinFitMomentum

  def SecVtxPost_MinPt( self ) : 
    return self.__SecVtxPost_MinPt
  
  def SecVtxPost_MinRadius( self ) :
    return self.__SecVtxPost_MinRadius

  def SecVtxPost_MaxdR( self ) :
    return self.__SecVtxPost_MaxdR

  def SecVtxPost_MaxPhiVtxTrk( self ) :
    return self.__SecVtxPost_MaxPhiVtxTrk

  def SingleTrk_MaxBLayerHits( self ) :
    return self.__SingleTrk_MaxBLayerHits

  def SingleTrk_MinInitialHitRadius( self ) :
    return self.__SingleTrk_MinInitialHitRadius

  def SingleTrk_MinInitialHitRadius_noBlay( self ) :
    return self.__SingleTrk_MinInitialHitRadius_noBlay

  def SingleTrk_MinRatioOfHLhits( self ) :
    return self.__SingleTrk_MinRatioOfHLhits

  def Finder_RemoveTrtTracks( self ) :
    return self.__Finder_RemoveTrtTracks

  def Finder_IsConversion( self ) :
    return self.__Finder_IsConversion

  def Finder_MinDistVtxHit( self ) :
    return self.__Finder_MinDistVtxHit

  def Finder_MaxDistVtxHit( self ) :
    return self.__Finder_MaxDistVtxHit

  def Finder_MinFlightAngle( self ) :
    return self.__Finder_MinFlightAngle

  def Finder_MinInitVtxR( self ) :
    return self.__Finder_MinInitVtxR

  def printInfo( self ) :
    print '****** Inner Detector Secondary Vertex Reconstruction Cuts ****************************'
    print '*'
    print '* secondary vertex mode               : ',self.__mode 
    print '*'
    print '* Vertex Fitter settings'
    print '* - Iteration Number                  : ',self.__Fitter_IterationNumber 
    print '* - Make Extended Vertex              : ',self.__Fitter_MakeExtendedVertex  
    print '* - First Measured Point              : ',self.__Fitter_FirstMeasuredPoint  
    print '* - Robustness                        : ',self.__Fitter_Robustness          
    print '* - Input Particle Masses             : ',self.__Fitter_InputParticleMasses 
    print '* - Vertex for constraint             : ',self.__Fitter_VertexForConstraint 
    print '* - Cov for constraint                : ',self.__Fitter_CovVrtForConstraint 
    print '* - First measured point limit        : ',self.__Fitter_FirstMeasuredPointLimit
    print '* - use Phi constraint                : ',self.__Fitter_usePhiCnst
    print '* - use Theta constraint              : ',self.__Fitter_useThetaCnst
    print '*'    
    print '* cuts for Track selector tool'
    print '* - Track max Si  d0                  : ',self.__TrkSel_maxSiD0           
    print '* - Track max TRT d0                  : ',self.__TrkSel_maxTrtD0          
    print '* - Track max Si  Z0                  : ',self.__TrkSel_maxSiZ0           
    print '* - Track max TRT Z0                  : ',self.__TrkSel_maxTrtZ0          
    print '* - Track min pt                      : ',self.__TrkSel_minPt 
    print '* - Track Ratio Cut1                  : ',self.__TrkSel_RatioCut1         
    print '* - Track Ratio Cut2                  : ',self.__TrkSel_RatioCut2         
    print '* - Track Ratio Cut3                  : ',self.__TrkSel_RatioCut3         
    print '* - Track Ratio TRT                   : ',self.__TrkSel_RatioTRT
    print '* - Track IsConverion                 : ',self.__TrkSel_IsConversion      
    print '* - track Si sig d0                   : ',self.__TrkSel_significanceD0_Si 
    print '* - Track Ratio V0                    : ',self.__TrkSel_RatioV0           
    print '*'    
    print '* Track pairs selector [SI+SI,SI+TRT,TRT+TRT]'
    print '* - Track Pair Max First Hit Radius   : ',self.__TrkPairSel_MaxFirstHitRadius    
    print '* - Track Pair Max Dist btw Tracks    : ',self.__TrkPairSel_MaxDistBetweenTracks 
    print '* - Track Pair Max Eta                : ',self.__TrkPairSel_MaxEta               
    print '* - Track Pair Max Init Distance      : ',self.__TrkPairSel_MaxInitDistance      
    print '* - Track Pair Min Track Angle        : ',self.__TrkPairSel_MinTrackAngle        
    print '*'
    print '* Vertex point estimator'
    print '* - Vertex Point Min Delta R          : ',self.__VtxPt_MinDeltaR
    print '* - Vertex Point Max Delta R          : ',self.__VtxPt_MaxDeltaR
    print '* - Vertex Point Max Phi              : ',self.__VtxPt_MaxPhi   
    print '*'
    print '* Secondary Vertex post selector'
    print '* - SVPostSel Max Chi2 Vtx            : ',self.__SecVtxPost_MaxChi2Vtx      
    print '* - SVPostSel Max Inv Mass            : ',self.__SecVtxPost_MaxInvariantMass
    print '* - SVPostSel Max Min Fit P           : ',self.__SecVtxPost_MinFitMomentum
    print '* - SVPostSel Min Pt                  : ',self.__SecVtxPost_MinPt
    print '* - SVPostSel Min Radius              : ',self.__SecVtxPost_MinRadius       
    print '* - SVPostSel Max dR                  : ',self.__SecVtxPost_MaxdR
    print '* - SVPostSel Max Phi Vtx             : ',self.__SecVtxPost_MaxPhiVtxTrk    
    print '*'
    print '* Single track secondary vertex tool'
    print '* - Max Blayer hits for conversions   : ',self.__SingleTrk_MaxBLayerHits  
    print '* - Single tr. Min Initial Hit Radius : ',self.__SingleTrk_MinInitialHitRadius
    print '*     - same for no blayer            : ',self.__SingleTrk_MinInitialHitRadius_noBlay
    print '* - Single tr. Min Ratio HL hits      : ',self.__SingleTrk_MinRatioOfHLhits   
    print '*'
    print '* InDetSecVtxFinderTool '
    print '* - Finder Remove TRT Tracks          : ',self.__Finder_RemoveTrtTracks   
    print '* - Finder IsConversions              : ',self.__Finder_IsConversion      
    print '* - Finder Min Dist Vtx Hit           : ',self.__Finder_MinDistVtxHit     
    print '* - Finder Max Dist Vtx Hit           : ',self.__Finder_MaxDistVtxHit     
    print '* - Finder Min Flight Angle           : ',self.__Finder_MinFlightAngle    
    print '* - Finder Min Init Vtx R             : ',self.__Finder_MinInitVtxR       
    print '*'
    print '************************ General Configuration *************************************'
    if self.__mode == "StartUp": 
      print 'No luminosity mode: single collision, loosen cuts to get as many V0 as possible'  
       
    if self.__mode == "PileUp": 
      print 'Pile Up luminosity mode: multiple collisions'   
   
    print '************************************************************************************'

