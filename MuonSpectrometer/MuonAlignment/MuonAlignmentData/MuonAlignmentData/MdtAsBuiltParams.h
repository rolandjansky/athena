/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_MDTASBUILTPARAMS_H
#define MUONALIGNMENTDATA_MDTASBUILTPARAMS_H

#include "MuonAlignmentData/MuonAlignmentPar.h"

class MdtAsBuiltParams : public MuonAlignmentPar {

 public:

  // Default constructor
  MdtAsBuiltParams();
  // destructor
  ~MdtAsBuiltParams();

  // Full constructor

    // list of parameters from Florian & al. 
    // XTOMOCHBERNAME	XTOMOSITE	XTOMOSITEID	XTOMOTIME	XTOMOPASSED	XTOMOSIDE	XTOMONBERTUBE1	XTOMONBERTUBE2	XTOMONBERML	XTOMONBERLAYER	XTOMOML1NYTUB	XTOMOML1NZTUB	XTOMOML1NDELY	XTOMOML1NDELZ	XTOMOML1NDELA	XTOMOML1NYPIT	XTOMOML1NZPIT	XTOMOML1PYTUB	XTOMOML1PZTUB	XTOMOML1PDELY	XTOMOML1PDELZ	XTOMOML1PDELA	XTOMOML1PYPIT	XTOMOML1PZPIT	XTOMOML2NYTUB	XTOMOML2NZTUB	XTOMOML2NDELY	XTOMOML2NDELZ	XTOMOML2NDELA	XTOMOML2NYPIT	XTOMOML2NZPIT	XTOMOML2PYTUB	XTOMOML2PZTUB	XTOMOML2PDELY	XTOMOML2PDELZ	XTOMOML2PDELA	XTOMOML2PYPIT	XTOMOML2PZPIT 
    void setParameters(std::string XTOMOCHBERNAME,  	
		       std::string XTOMOSITE, 	    
		       int 	   XTOMOSITEID,     	
		       int         XTOMOTIME,	    
		       int         XTOMOPASSED,     	
		       std::string XTOMOSIDE, 	    
		       int         XTOMONBERTUBE1,  
		       int 	   XTOMONBERTUBE2,  
		       int 	   XTOMONBERML,     
		       int	   XTOMONBERLAYER,  
		       float 	   XTOMOML1NYTUB,   
		       float 	   XTOMOML1NZTUB,   
		       float 	   XTOMOML1NDELY,    
		       float 	   XTOMOML1NDELZ,   
		       float 	   XTOMOML1NDELA,   
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
		       float       XTOMOML2PZPITbz);

    void getParameters(std::string& XTOMOCHBERNAME,	
		       std::string& XTOMOSITE, 	
		       int& XTOMOSITEID, 	
		       int& XTOMOTIME,
		       int& XTOMOPASSED, 	
		       std::string& XTOMOSIDE, 
		       int& XTOMONBERTUBE1, 
		       int& XTOMONBERTUBE2, 
		       int& XTOMONBERML, 
		       int& XTOMONBERLAYER, 
		       float& XTOMOML1NYTUB, 
		       float& XTOMOML1NZTUB,
		       float& XTOMOML1NDELY,    
		       float& XTOMOML1NDELZ,
		       float& XTOMOML1NDELA,
		       float& XTOMOML1NYPIT,
		       float& XTOMOML1NZPIT, 
		       float& XTOMOML1PYTUB,
		       float& XTOMOML1PZTUB,    
		       float& XTOMOML1PDELY,    
		       float& XTOMOML1PDELZ,	
		       float& XTOMOML1PDELA,
		       float& XTOMOML1PYPIT, 
		       float& XTOMOML1PZPIT,    
		       float& XTOMOML2NYTUB, 	
		       float& XTOMOML2NZTUB,	
		       float& XTOMOML2NDELY, 
		       float& XTOMOML2NDELZ,	
		       float& XTOMOML2NDELA, 
		       float& XTOMOML2NYPIT,
		       float& XTOMOML2NZPIT,    
		       float& XTOMOML2PYTUB,	
		       float& XTOMOML2PZTUB,
		       float& XTOMOML2PDELY,
		       float& XTOMOML2PDELZ,
		       float& XTOMOML2PDELA,
		       float& XTOMOML2PYPIT,
		       float& XTOMOML2PZPITbz);

private:

    std::string m_XTOMOCHBERNAME;  
    std::string m_XTOMOSITE; 	    
    int 	   m_XTOMOSITEID;     
    int         m_XTOMOTIME;	    
    int         m_XTOMOPASSED;     
    std::string m_XTOMOSIDE; 	    
    int         m_XTOMONBERTUBE1;  
    int 	   m_XTOMONBERTUBE2;  
    int 	   m_XTOMONBERML;     
    int	           m_XTOMONBERLAYER;  
    float 	   m_XTOMOML1NYTUB;   
    float 	   m_XTOMOML1NZTUB;   
    float 	   m_XTOMOML1NDELY;   
    float 	   m_XTOMOML1NDELZ;   
    float 	   m_XTOMOML1NDELA;   
    float       m_XTOMOML1NYPIT;   
    float       m_XTOMOML1NZPIT;   
    float       m_XTOMOML1PYTUB;   
    float       m_XTOMOML1PZTUB;   
    float       m_XTOMOML1PDELY;   
    float       m_XTOMOML1PDELZ;   
    float       m_XTOMOML1PDELA;   
    float       m_XTOMOML1PYPIT;   
    float       m_XTOMOML1PZPIT;   
    float       m_XTOMOML2NYTUB;   
    float       m_XTOMOML2NZTUB;   
    float       m_XTOMOML2NDELY;   
    float       m_XTOMOML2NDELZ;   
    float       m_XTOMOML2NDELA;   
    float       m_XTOMOML2NYPIT;   
    float       m_XTOMOML2NZPIT;   
    float       m_XTOMOML2PYTUB;   
    float       m_XTOMOML2PZTUB;   
    float       m_XTOMOML2PDELY;   
    float       m_XTOMOML2PDELZ;   
    float       m_XTOMOML2PDELA;   
    float       m_XTOMOML2PYPIT;   
    float       m_XTOMOML2PZPITbz;
    
};

#endif  // MUONALIGNMENTDATA_MDTASBUILTPARAMS_H
