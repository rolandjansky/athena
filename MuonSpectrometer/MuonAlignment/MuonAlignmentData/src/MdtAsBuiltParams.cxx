/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/MdtAsBuiltParams.h"
 
MdtAsBuiltParams::MdtAsBuiltParams():
  MuonAlignmentPar(),
  m_XTOMOCHBERNAME     ("XXX"), 
  m_XTOMOSITE          ("YYY"), 	  
  m_XTOMOSITEID        (0),    
  m_XTOMOTIME          (0),	  
  m_XTOMOPASSED        (0),    
  m_XTOMOSIDE          ("X"), 	  
  m_XTOMONBERTUBE1     (0), 
  m_XTOMONBERTUBE2     (0), 
  m_XTOMONBERML        (0),    
  m_XTOMONBERLAYER     (0), 
  m_XTOMOML1NYTUB      (0.),  
  m_XTOMOML1NZTUB      (0.),  
  m_XTOMOML1NDELY      (0.),  
  m_XTOMOML1NDELZ      (0.),  
  m_XTOMOML1NDELA      (0.),  
  m_XTOMOML1NYPIT      (0.),  
  m_XTOMOML1NZPIT      (0.),  
  m_XTOMOML1PYTUB      (0.),  
  m_XTOMOML1PZTUB      (0.),  
  m_XTOMOML1PDELY      (0.),  
  m_XTOMOML1PDELZ      (0.),  
  m_XTOMOML1PDELA      (0.),  
  m_XTOMOML1PYPIT      (0.),  
  m_XTOMOML1PZPIT      (0.),  
  m_XTOMOML2NYTUB      (0.),  
  m_XTOMOML2NZTUB      (0.),  
  m_XTOMOML2NDELY      (0.),  
  m_XTOMOML2NDELZ      (0.),  
  m_XTOMOML2NDELA      (0.),  
  m_XTOMOML2NYPIT      (0.),  
  m_XTOMOML2NZPIT      (0.),  
  m_XTOMOML2PYTUB      (0.),  
  m_XTOMOML2PZTUB      (0.),  
  m_XTOMOML2PDELY      (0.),  
  m_XTOMOML2PDELZ      (0.),  
  m_XTOMOML2PDELA      (0.),  
  m_XTOMOML2PYPIT      (0.),  
  m_XTOMOML2PZPITbz    (0.)
{ }

MdtAsBuiltParams::~MdtAsBuiltParams()
{ }


void MdtAsBuiltParams::setParameters(
    std::string XTOMOCHBERNAME,  
    std::string XTOMOSITE, 	    
    int 	XTOMOSITEID,     
    int         XTOMOTIME,	    
    int         XTOMOPASSED,     
    std::string XTOMOSIDE, 	    
    int         XTOMONBERTUBE1,  
    int 	XTOMONBERTUBE2,  
    int         XTOMONBERML,     
    int	        XTOMONBERLAYER,  
    float       XTOMOML1NYTUB,   
    float 	XTOMOML1NZTUB,   
    float 	XTOMOML1NDELY,   
    float 	XTOMOML1NDELZ,   
    float 	XTOMOML1NDELA,   
    float       XTOMOML1NYPIT,   
    float       XTOMOML1NZPIT,   
    float       XTOMOML1PYTUB,   
    float       XTOMOML1PZTUB,   
    float       XTOMOML1PDELY,   
    float       XTOMOML1PDELZ,   
    float       XTOMOML1PDELA,   
    float       XTOMOML1PYPIT,   
    float       XTOMOML1PZPIT,   
    float       XTOMOML2NYTUB,   
    float       XTOMOML2NZTUB,   
    float       XTOMOML2NDELY,   
    float       XTOMOML2NDELZ,   
    float       XTOMOML2NDELA,   
    float       XTOMOML2NYPIT,   
    float       XTOMOML2NZPIT,   
    float       XTOMOML2PYTUB,   
    float       XTOMOML2PZTUB,   
    float       XTOMOML2PDELY,   
    float       XTOMOML2PDELZ,   
    float       XTOMOML2PDELA,   
    float       XTOMOML2PYPIT,   
    float       XTOMOML2PZPITbz)
{
  m_XTOMOCHBERNAME     =XTOMOCHBERNAME     ; 
  m_XTOMOSITE          =XTOMOSITE          ; 	  
  m_XTOMOSITEID        =XTOMOSITEID        ;    
  m_XTOMOTIME          =XTOMOTIME          ;	  
  m_XTOMOPASSED        =XTOMOPASSED        ;    
  m_XTOMOSIDE          =XTOMOSIDE          ; 	  
  m_XTOMONBERTUBE1     =XTOMONBERTUBE1     ; 
  m_XTOMONBERTUBE2     =XTOMONBERTUBE2     ; 
  m_XTOMONBERML        =XTOMONBERML        ;    
  m_XTOMONBERLAYER     =XTOMONBERLAYER     ; 
  m_XTOMOML1NYTUB      =XTOMOML1NYTUB      ;  
  m_XTOMOML1NZTUB      =XTOMOML1NZTUB      ;  
  m_XTOMOML1NDELY      =XTOMOML1NDELY      ;  
  m_XTOMOML1NDELZ      =XTOMOML1NDELZ      ;  
  m_XTOMOML1NDELA      =XTOMOML1NDELA      ;  
  m_XTOMOML1NYPIT      =XTOMOML1NYPIT      ;  
  m_XTOMOML1NZPIT      =XTOMOML1NZPIT      ;  
  m_XTOMOML1PYTUB      =XTOMOML1PYTUB      ;  
  m_XTOMOML1PZTUB      =XTOMOML1PZTUB      ;  
  m_XTOMOML1PDELY      =XTOMOML1PDELY      ;  
  m_XTOMOML1PDELZ      =XTOMOML1PDELZ      ;  
  m_XTOMOML1PDELA      =XTOMOML1PDELA      ;  
  m_XTOMOML1PYPIT      =XTOMOML1PYPIT      ;  
  m_XTOMOML1PZPIT      =XTOMOML1PZPIT      ;  
  m_XTOMOML2NYTUB      =XTOMOML2NYTUB      ;  
  m_XTOMOML2NZTUB      =XTOMOML2NZTUB      ;  
  m_XTOMOML2NDELY      =XTOMOML2NDELY      ;  
  m_XTOMOML2NDELZ      =XTOMOML2NDELZ      ;  
  m_XTOMOML2NDELA      =XTOMOML2NDELA      ;  
  m_XTOMOML2NYPIT      =XTOMOML2NYPIT      ;  
  m_XTOMOML2NZPIT      =XTOMOML2NZPIT      ;  
  m_XTOMOML2PYTUB      =XTOMOML2PYTUB      ;  
  m_XTOMOML2PZTUB      =XTOMOML2PZTUB      ;  
  m_XTOMOML2PDELY      =XTOMOML2PDELY      ;  
  m_XTOMOML2PDELZ      =XTOMOML2PDELZ      ;  
  m_XTOMOML2PDELA      =XTOMOML2PDELA      ;  
  m_XTOMOML2PYPIT      =XTOMOML2PYPIT      ;  
  m_XTOMOML2PZPITbz    =XTOMOML2PZPITbz    ;
}

void MdtAsBuiltParams::getParameters(
    std::string& XTOMOCHBERNAME,  
    std::string& XTOMOSITE, 	    
    int& 	 XTOMOSITEID,     
    int&         XTOMOTIME,	    
    int&         XTOMOPASSED,     
    std::string& XTOMOSIDE, 	    
    int&         XTOMONBERTUBE1,  
    int&  	 XTOMONBERTUBE2,  
    int&         XTOMONBERML,     
    int&         XTOMONBERLAYER,  
    float&       XTOMOML1NYTUB,   
    float& 	XTOMOML1NZTUB,   
    float& 	XTOMOML1NDELY,   
    float& 	XTOMOML1NDELZ,   
    float& 	XTOMOML1NDELA,   
    float&       XTOMOML1NYPIT,   
    float&       XTOMOML1NZPIT,   
    float&       XTOMOML1PYTUB,   
    float&       XTOMOML1PZTUB,   
    float&       XTOMOML1PDELY,   
    float&       XTOMOML1PDELZ,   
    float&       XTOMOML1PDELA,   
    float&       XTOMOML1PYPIT,   
    float&       XTOMOML1PZPIT,   
    float&       XTOMOML2NYTUB,   
    float&       XTOMOML2NZTUB,   
    float&       XTOMOML2NDELY,   
    float&       XTOMOML2NDELZ,   
    float&       XTOMOML2NDELA,   
    float&       XTOMOML2NYPIT,   
    float&       XTOMOML2NZPIT,   
    float&       XTOMOML2PYTUB,   
    float&       XTOMOML2PZTUB,   
    float&       XTOMOML2PDELY,   
    float&       XTOMOML2PDELZ,   
    float&       XTOMOML2PDELA,   
    float&       XTOMOML2PYPIT,   
    float&       XTOMOML2PZPITbz)
{
  XTOMOCHBERNAME     =m_XTOMOCHBERNAME     ; 
  XTOMOSITE          =m_XTOMOSITE          ; 	  
  XTOMOSITEID        =m_XTOMOSITEID        ;    
  XTOMOTIME          =m_XTOMOTIME          ;	  
  XTOMOPASSED        =m_XTOMOPASSED        ;    
  XTOMOSIDE          =m_XTOMOSIDE          ; 	  
  XTOMONBERTUBE1     =m_XTOMONBERTUBE1     ; 
  XTOMONBERTUBE2     =m_XTOMONBERTUBE2     ; 
  XTOMONBERML        =m_XTOMONBERML        ;    
  XTOMONBERLAYER     =m_XTOMONBERLAYER     ; 
  XTOMOML1NYTUB      =m_XTOMOML1NYTUB      ;  
  XTOMOML1NZTUB      =m_XTOMOML1NZTUB      ;  
  XTOMOML1NDELY      =m_XTOMOML1NDELY      ;  
  XTOMOML1NDELZ      =m_XTOMOML1NDELZ      ;  
  XTOMOML1NDELA      =m_XTOMOML1NDELA      ;  
  XTOMOML1NYPIT      =m_XTOMOML1NYPIT      ;  
  XTOMOML1NZPIT      =m_XTOMOML1NZPIT      ;  
  XTOMOML1PYTUB      =m_XTOMOML1PYTUB      ;  
  XTOMOML1PZTUB      =m_XTOMOML1PZTUB      ;  
  XTOMOML1PDELY      =m_XTOMOML1PDELY      ;  
  XTOMOML1PDELZ      =m_XTOMOML1PDELZ      ;  
  XTOMOML1PDELA      =m_XTOMOML1PDELA      ;  
  XTOMOML1PYPIT      =m_XTOMOML1PYPIT      ;  
  XTOMOML1PZPIT      =m_XTOMOML1PZPIT      ;  
  XTOMOML2NYTUB      =m_XTOMOML2NYTUB      ;  
  XTOMOML2NZTUB      =m_XTOMOML2NZTUB      ;  
  XTOMOML2NDELY      =m_XTOMOML2NDELY      ;  
  XTOMOML2NDELZ      =m_XTOMOML2NDELZ      ;  
  XTOMOML2NDELA      =m_XTOMOML2NDELA      ;  
  XTOMOML2NYPIT      =m_XTOMOML2NYPIT      ;  
  XTOMOML2NZPIT      =m_XTOMOML2NZPIT      ;  
  XTOMOML2PYTUB      =m_XTOMOML2PYTUB      ;  
  XTOMOML2PZTUB      =m_XTOMOML2PZTUB      ;  
  XTOMOML2PDELY      =m_XTOMOML2PDELY      ;  
  XTOMOML2PDELZ      =m_XTOMOML2PDELZ      ;  
  XTOMOML2PDELA      =m_XTOMOML2PDELA      ;  
  XTOMOML2PYPIT      =m_XTOMOML2PYPIT      ;  
  XTOMOML2PZPITbz    =m_XTOMOML2PZPITbz    ;
}
				     
