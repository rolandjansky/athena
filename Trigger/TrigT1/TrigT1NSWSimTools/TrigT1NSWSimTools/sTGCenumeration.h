/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCenumeration_h
#define sTGCenumeration_h

static const float VERY_LARGE               =      999999.
                 , UNDEF                    =  -VERY_LARGE
                 , DEFAULT_VALUE            =        UNDEF
                 ; 

enum selectorType     { NONE                =  -1
                      , ALL                 =  -2
                      , DEFAULT             =  -3
                      , TOO_SMALL           =  -4
                      , TOO_LARGE           =  -5
                      , UNDEFINED           =  -999999
                      } ;

enum STGCModuleType   { STGC_MODULE_1       =  1
                      , STGC_MODULE_2   
                      , STGC_MODULE_3   
                      , STGC_MODULES        =  3
                      } ; 

enum STGCDetectorType { STGC_DETECTOR_0 
                      , STGC_DETECTOR_1    
                      , STGC_DETECTOR_2
                      , STGC_DETECTOR_3
                      , STGC_DETECTORS 
                      } ;

enum PivotConfirmType { PIVOT              
                      , CONFIRM 
                      , STGC_WEDGES 
                      } ;

enum SectorType       { SMALL                
                      , LARGE 
                      , SECTORS_IN_OCTANT
                      , TOY                 =  SECTORS_IN_OCTANT  // 2
                      , SECTOR_TYPES
                      } ;

enum STGCType         { SMALL_PIVOT    
                      , SMALL_CONFIRM      
                      , LARGE_PIVOT       
                      , LARGE_CONFIRM    
                      , TOY_PIVOT           
                      , TOY_CONFIRM                         
                      , STGC_TYPES   
                      } ;

enum LayerType        { NO_LAYER            = -1 
                      , STGC_LAYER_1        =  1         
                      , STGC_LAYER_2         
                      , STGC_LAYER_3          
                      , STGC_LAYER_4        
                      , STGC_LAYERS         =  5
                      } ;

enum MMType           { REGULAR      
                      , CORNERED              
                      , EXTRUDED            
                      , MM_TYPES       
                      } ;

enum MMModule         { MM_MODULE_1         =  1
                      , MM_MODULE_2  
                      , MM_MODULE_3 
                      , MM_MODULE_4  
                      , MM_MODULES          =  4
                      } ;

enum MDTModule        { MDT_MODULE_1         =  1
                      , MDT_MODULE_2  
                      , MDT_MODULE_3 
                      , MDT_MODULE_4  
                      , MDT_MODULES          =  4
                      } ;

// description of the trigger patterns

enum triggerPattern   { TRPAT_NBITS_LAYER =  8 
                      , TRPAT_PHI_BITS    =  0 
                      , TRPAT_R_BITS      =  4 
                      , TRPAT_MASK        =  0x7
                      , TRPAT_DRMIN       =  0 
                      , TRPAT_DRMAX       =  1 
                      , TRPAT_DPHIMIN     =  0 
                      , TRPAT_DPHIMAX     =  1 
                      } ;

#endif
