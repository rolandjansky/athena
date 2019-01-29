# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################
# ConfiguredVertexingCuts class
#
# Python Setup Class for primary vertex reconstruction
#
# Author: Kirill Prokofiev
#
#
#  18-5-2010 Removing the Single collision flag and moving it to InDetFlags
#  The single/multiple collision mode is now independent on the general
#  numberOfCollisions flag. Update to the head version is made
#
#
#  7-9-2010 General clean up of the configurable file
#
#

import AthenaCommon.SystemOfUnits as Units

class ConfiguredVertexingCuts :
   
  def __init__ (self, mode = "offline"):

   self.__mode      = mode
   self.__extension = ""
   
#pointer to InDetFlags, don't use them directly
#to allow sharing this code with the trigger  
   self.__set_indetflags()     
 			      
   from AthenaCommon.GlobalFlags import globalflags
   from AthenaCommon.DetFlags import DetFlags
   from AthenaCommon.BeamFlags import jobproperties
   from RecExConfig.RecFlags import rec

#-----------------------------------------------------------------------
#General default cuts, which are modified according to the mode of
# operation selected. These cuts are used ONLY for the pre-selection
# of tracks for the vertex reconstruction.
#

# Cut level for the track selection tool.
   self.__TrackCutLevel             = "NoCut"

# this is cutLevel = 1
   self.__minPT                     = 400. * Units.MeV
   self.__IPd0Max                   = 10.
   self.__IPz0Max	            = 1000.
   self.__z0Max		            = 1000.
   self.__sigIPd0Max		    = 5.
   self.__sigIPz0Max		    = 10.
   self.__d0significanceMax	    = -1.
   self.__z0significanceMax	    = -1.
   self.__etaMax		    = 9999.
   
#cluster and track quality cuts   
   self.__useTrackSummaryInfo       = True
   self.__nHitInnermostLayer	    = 0
   self.__nHitPix		    = 1
   self.__nHolesPix                 = 1
   self.__nHitSct		    = 4
   self.__nHitSi		    = 6
   self.__nHitTrt		    = 0
   self.__nHitTrtHighEFractionMax   = 1
   self.__nHitTrtHighEFractionWithOutliersMax = 1
   self.__useSharedHitInfo	    = False
   self.__useTrackQualityInfo       = False 
   self.__fitChi2OnNdfMax	    = 3.5
   self.__TrtMaxEtaAcceptance       = 1.9
   
#cuts ONLY relevant to InDetPriVxFinder + Full or Fast or similar
   self.__chi2CutMethod             = 2
   self.__enableMultipleVertices    = True  

   self.__doMaxTracksCut = False
   self.__MaxTracks = 5000 #Not applied  anyway if above false
      
#-----------------------------------------------------------------------
#End of the default track pre-selection block
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Maximal chi2 per track value to be accepted in the vertex mixture.
# ATTENTION: this cut is ONLY used in InDetPriVxFinder; it has no 
# meaning for the other finders and used mostly during the commisionning 
# stage

   if rec.Commissioning():
      self.__maxChi2PerTrack = 15.

   else: 
      self.__maxChi2PerTrack = 5.

#-----------------------------------------------------------------------
#End of the InDetPriVxFinder tuning block
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Fine tuning of the opration mode depending on the machine conditions 
# degree of understanding of detector performance.
#
# "Start Up" - to be operated in the beginning of the LHC run when the 
#  parameters of the beam and detector performance are largely unknown,
#  energy and event mutiplicity are bot small.
#  Features the use of very low pT and relaxed requirements on the
#  impact parameters of the used tracks. The IP cuts are tighten 
#  if the beam spot information is available.
#
# "No Lumi" Intended for the stable running in the single vertex mode
#  using the beam constraint. Features tighter cuts on the impact parameters 
#  during the pre-selection of tracks. At present time is probably obsolete.
#
# "Pile Up" Intended for the stable running in the pile up conditions
#  using the beam constraint. High track multiplicities are supposed.
#  Thus the impact parameter cuts are tightened and the pT threschold
# is significantly raised.

   if (self.__indetflags.useBeamConstraint()):
     self.__IPd0Max = 4. 
  
   if (self.__indetflags.doRobustReco()):
     self.__nHitPix = 0

   if (self.__indetflags.doMinBias()):
     self.__minPT   = 100. * Units.MeV

   if not (self.__indetflags.useBeamConstraint()) and not (mode == "HeavyIon") and not (mode == "SLHC"):
     # Cut on multiplicity for IterativeFinder if no beamspot constraint, to avoid timeouts
     self.__doMaxTracksCut = True
     self.__MaxTracks = 3000
     
       
#--------------"Hight PileUp mode, setup for 2012 ?"---------------------
#  Same Pt cut of 400, higher requirements for the track quality
#
   if self.__indetflags.priVtxCutLevel() == 2 :
   
     self.__nHitSi     = 9
   
     self.__nHolesPix  = 0


#-------------- RUN2 TightPrimary Options---------------------
#  Additional TrackSelectionTool options for RUN2
#
   if self.__indetflags.priVtxCutLevel() == 3 :
     self.__TrackCutLevel = "TightPrimary"


#-------------"MinBias running mode"-----------------------------------
# Looser pT cut on input tracks; tighter cut on sigma IP d0
   if mode == "LowPt":
     self.__minPT = 100. * Units.MeV
     self.__sigIPd0Max = 0.9


#-------------"IBL running mode"-----------------------------------
# For the moment, same as above, but just explicitely request the special
# mode for the Fast(or Full)VertexFinder, since it is in the single vertex
# mode anyways

   if mode == "IBL":  
      
      self.__minPT               = 500. * Units.MeV
      self.__IPd0Max             = 1.
      self.__sigIPd0Max          = 0.35
      self.__sigIPz0Max          = 2.5
      self.__d0significanceMax   = 4.

     #cluster and track quality cuts   

      self.__nHitSct             = 5
      self.__nHitSi              = 7
      self.__useTrackQualityInfo = True 
      
#-------------"Heavy Ion running mode"-----------------------------------
# For the moment, same as above, but just explicitely request the special
# mode for the Fast(or Full)VertexFinder, since it is in the single vertex
# mode anyways

   if mode == "SLHC":  

      self.__minPT               = 900. * Units.MeV
      self.__IPd0Max             = 1.
      self.__sigIPd0Max          = 0.35
      self.__sigIPz0Max          = 2.5
      self.__d0significanceMax   = 4.

     #cluster and track quality cuts   
      self.__nHitPix             = 3
      self.__nHitSct             = 0
      self.__nHitSi              = 7
      self.__useTrackQualityInfo = True      

#-------------"Heavy Ion running mode"-----------------------------------
# For the moment, same as above, but just explicitely request the special
# mode for the Fast(or Full)VertexFinder, since it is in the single vertex
# mode anyways

   if mode == "HeavyIon":  
     self.__chi2CutMethod          = 1
     self.__enableMultipleVertices = False


#------------------------------------------------------------------------- 
#  Return methods for the cut values - the main purpose of this class
#
  def mode( self ) :
    return self.__mode

  def extension( self ) :
    return self.__extension

  def TrackCutLevel( self ) :
    return self.__TrackCutLevel

  def minPT( self ) :
    return self.__minPT
    
  def IPd0Max( self ) :  
    return self.__IPd0Max
    
  def IPz0Max( self ) : 
    return self.__IPz0Max
    
  def  z0Max( self ) :
    return self.__z0Max
    
  def  sigIPd0Max( self ) :  			
   return self.__sigIPd0Max
   
  def  sigIPz0Max( self ) : 
   return self.__sigIPz0Max
   
  def d0significanceMax( self ) : 
   return self.__d0significanceMax
   
  def  z0significanceMax( self ) : 
   return self.__z0significanceMax
 
  def  etaMax( self ) :  
   return self.__etaMax

  def  useTrackSummaryInfo( self ) :  
   return self.__useTrackSummaryInfo
   
  def  nHitInnermostLayer( self ) :  
   return self.__nHitInnermostLayer
   
  def  nHitPix( self ):
   return self.__nHitPix
   
  def  nHitSct( self ):
   return self.__nHitSct
   
  def  nHitSi( self ):
   return self.__nHitSi
   
  def  nHitTrt( self ):
   return self.__nHitTrt
   
  def  nHitTrtHighEFractionMax(self): 
   return self.__nHitTrtHighEFractionMax
   
  def  nHitTrtHighEFractionWithOutliersMax(self):
   return self.__nHitTrtHighEFractionWithOutliersMax
   
  def  useSharedHitInfo(self):
   return self.__useSharedHitInfo
   
  def  useTrackQualityInfo(self):
   return self.__useTrackQualityInfo
   
  def  fitChi2OnNdfMax(self):
   return self.__fitChi2OnNdfMax
   
  def  TrtMaxEtaAcceptance(self):
   return self.__TrtMaxEtaAcceptance

  def  MaxChi2PerTrack(self):
   return self.__maxChi2PerTrack 

  def  nHolesPix(self):
   return self.__nHolesPix

  def  chi2CutMethod(self): 
   return self.__chi2CutMethod

  def  enableMultipleVertices(self): 
   return self.__enableMultipleVertices

  def  doMaxTracksCut(self):
    return self.__doMaxTracksCut
     
  def MaxTracks(self):
    return self.__MaxTracks

#-------------------------------------------------------------------------
#  Print method dumping the summary of the current cut configuration
#
  
  def printInfo( self ) :
    print '****** Inner Detector Primary Vertex Reconstruction Cuts ****************************'
    print '*'
    print '* mode is                                           : ', self.__mode
    print '* cut level for primary vertexing is                : ', self.__indetflags.priVtxCutLevel()
    print '*'
    print '* Track Cut Level                                   : ', self.__TrackCutLevel
    print '*'
    print '* min pT                                            : ', self.__minPT, ' MeV'
    print '* Max IP d0                                         : ', self.__IPd0Max
    print '* Max IP Z0                                         : ', self.__IPz0Max
    print '* Max z0                                            : ', self.__z0Max
    print '* Max Sig IP d0                                     : ', self.__sigIPd0Max
    print '* Max Sig IP z0                                     : ', self.__sigIPz0Max
    print '* Max Sig d0                                        : ', self.__d0significanceMax
    print '* Max Sig z0                                        : ', self.__z0significanceMax
    print '* Max eta                                           : ', self.__etaMax
   
#cluster and track quality cuts  
    print '* Use TrackSummary                                  : ', self.__useTrackSummaryInfo
    print '* Min N Innermost Layer Hits                        : ', self.__nHitInnermostLayer
    print '* Min N Pix Hit                                     : ', self.__nHitPix
    print '* Max N Pix Holes                                   : ', self.__nHolesPix
    print '* Min N SCT Hits                                    : ', self.__nHitSct
    print '* Min N Si Hits                                     : ', self.__nHitSi
    print '* Min N TRT Hits                                    : ', self.__nHitTrt
    print '* Min N high threshold hits TRT                     : ', self.__nHitTrtHighEFractionMax
    print '* Min N high threshold hits TRT  (incl. outliers)   : ', self.__nHitTrtHighEFractionWithOutliersMax
    print '* Use shared hit info                               : ', self.__useSharedHitInfo
    print '* Use Track Quality info                            : ', self.__useTrackQualityInfo
    print '* Chi2/ndf of the fit                               : ', self.__fitChi2OnNdfMax
    print '* TRT Max  eta                                      : ', self.__TrtMaxEtaAcceptance
    print '*'
    print '* Chi2 cut method                                   : ', self.__chi2CutMethod 
    print '* Enable Multiple Vertices                          : ', self.__enableMultipleVertices 
    print '*'
    print '************************************************************************************'


#-------------------------------------------------------------------------
# Technical private method to set the InDet flags 
# 
  def __set_indetflags(self):
    from InDetRecExample.InDetJobProperties import InDetFlags
    self.__indetflags = InDetFlags
  

