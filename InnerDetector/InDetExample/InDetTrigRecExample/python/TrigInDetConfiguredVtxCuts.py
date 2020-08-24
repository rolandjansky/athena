#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function

import AthenaCommon.SystemOfUnits as Units



class ConfiguredTrigVtxCuts():
#	"""docstring for ConfiguredTrigVtxCuts"""
    def __init__(self, mode = ""):
        self.__mode = mode

        # Cut level for the track selection tool.
        self.__TrackCutLevel             = "NoCut"

        # this is cutLevel = 1
        self.__minPT               = 400. * Units.MeV
        self.__IPd0Max             = 4. # = 4. for useBeamConstraint()==true, = 10. for useBeamConstraint()==false
        self.__IPz0Max             = 1000.
        self.__z0Max               = 1000.
        self.__sigIPd0Max          = 5.
        self.__sigIPz0Max          = 10.
        self.__d0significanceMax   = -1.
        self.__z0significanceMax   = -1.
        self.__etaMax		       = 9999.
   
        #cluster and track quality cuts   
        self.__useTrackSummaryInfo       = True
        self.__nHitInnermostLayer        = 0
        self.__nHitPix                   = 1
        self.__nHolesPix                 = 1
        self.__nHitSct                   = 4
        self.__nHitSi                    = 6
        self.__nHitTrt                   = 0
        self.__nHitTrtHighEFractionMax   = 1
        self.__nHitTrtHighEFractionWithOutliersMax = 1
        self.__useSharedHitInfo          = False
        self.__useTrackQualityInfo       = False 
        self.__fitChi2OnNdfMax           = 3.5
        self.__TrtMaxEtaAcceptance       = 1.9
   
         #cuts ONLY relevant to InDetPriVxFinder + Full or Fast or similar
        self.__chi2CutMethod             = 2
        self.__enableMultipleVertices    = True  

        self.__doMaxTracksCut            = True
        self.__MaxTracks                 = 3000  #Not applied  anyway if above false

#        self.__indetflags = InDetFlags  #TODO: implement or find InDetJobProperties

    def mode( self ) :
        return self.__mode
  
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
     
    def z0significanceMax( self ) : 
        return self.__z0significanceMax
   
    def etaMax( self ) :  
        return self.__etaMax
  
    def useTrackSummaryInfo( self ) :  
        return self.__useTrackSummaryInfo
     
    def nHitInnermostLayer( self ) :  
        return self.__nHitInnermostLayer
     
    def nHitPix( self ):
        return self.__nHitPix
     
    def nHitSct( self ):
        return self.__nHitSct
     
    def nHitSi( self ):
        return self.__nHitSi
     
    def nHitTrt( self ):
        return self.__nHitTrt
     
    def nHitTrtHighEFractionMax(self): 
        return self.__nHitTrtHighEFractionMax
     
    def nHitTrtHighEFractionWithOutliersMax(self):
        return self.__nHitTrtHighEFractionWithOutliersMax
     
    def useSharedHitInfo(self):
        return self.__useSharedHitInfo
     
    def useTrackQualityInfo(self):
        return self.__useTrackQualityInfo
     
    def fitChi2OnNdfMax(self):
        return self.__fitChi2OnNdfMax
     
    def TrtMaxEtaAcceptance(self):
        return self.__TrtMaxEtaAcceptance
  
    def MaxChi2PerTrack(self):
        return self.__maxChi2PerTrack 
  
    def nHolesPix(self):
        return self.__nHolesPix
  
    def chi2CutMethod(self): 
        return self.__chi2CutMethod
  
    def enableMultipleVertices(self): 
        return self.__enableMultipleVertices
  
    def doMaxTracksCut(self):
        return self.__doMaxTracksCut

    def MaxTracks(self):
        return self.__MaxTracks

    def printInfo( self ) :
        print('****** Inner Detector Primary Vertex Reconstruction Cuts ****************************')
        print('*')
        print('* mode is                                           : ' + self.__mode)
        #print('* cut level for primary vertexing is                : ' + self.__indetflags.priVtxCutLevel())
        print('*')
        print('* Track Cut Level                                   : ' + self.__TrackCutLevel)
        print('*')
        print('* min pT                                            : ' + str(self.__minPT) + ' MeV')
        print('* Max IP d0                                         : ' + str(self.__IPd0Max))
        print('* Max IP Z0                                         : ' + str(self.__IPz0Max))
        print('* Max z0                                            : ' + str(self.__z0Max))
        print('* Max Sig IP d0                                     : ' + str(self.__sigIPd0Max))
        print('* Max Sig IP z0                                     : ' + str(self.__sigIPz0Max))
        print('* Max Sig d0                                        : ' + str(self.__d0significanceMax))
        print('* Max Sig z0                                        : ' + str(self.__z0significanceMax))
        print('* Max eta                                           : ' + str(self.__etaMax))
   
#cluster and track quality cuts  
        print('* Use TrackSummary                                  : ' + str(self.__useTrackSummaryInfo))
        print('* Min N Innermost Layer Hits                        : ' + str(self.__nHitInnermostLayer))
        print('* Min N Pix Hit                                     : ' + str(self.__nHitPix))
        print('* Max N Pix Holes                                   : ' + str(self.__nHolesPix))
        print('* Min N SCT Hits                                    : ' + str(self.__nHitSct))
        print('* Min N Si Hits                                     : ' + str(self.__nHitSi))
        print('* Min N TRT Hits                                    : ' + str(self.__nHitTrt))
        print('* Min N high threshold hits TRT                     : ' + str(self.__nHitTrtHighEFractionMax))
        print('* Min N high threshold hits TRT  (incl. outliers)   : ' + str(self.__nHitTrtHighEFractionWithOutliersMax))
        print('* Use shared hit info                               : ' + str(self.__useSharedHitInfo))
        print('* Use Track Quality info                            : ' + str(self.__useTrackQualityInfo))
        print('* Chi2/ndf of the fit                               : ' + str(self.__fitChi2OnNdfMax))
        print('* TRT Max  eta                                      : ' + str(self.__TrtMaxEtaAcceptance))
        print('*')
        print('* Chi2 cut method                                   : ' + str(self.__chi2CutMethod ))
        print('* Enable Multiple Vertices                          : ' + str(self.__enableMultipleVertices) )
        print('*')
        print('***********************************************************************************')

