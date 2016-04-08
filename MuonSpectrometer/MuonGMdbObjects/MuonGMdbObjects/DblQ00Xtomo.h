/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/Xtomo
 *******************************************************/

 //  author: M Schreyer
 // entered: 2012-09-24
 // comment: Mdt AsBuilt parameters - class to read from DB

#include <string>

#ifndef DBLQ00_XTOMO_H
#define DBLQ00_XTOMO_H

class IRDBQuery;

namespace MuonGM
{
    

class DblQ00Xtomo {

public:
    DblQ00Xtomo();
    ~DblQ00Xtomo();
    DblQ00Xtomo(IRDBQuery* m_xtomo);
//    DblQ00Xtomo(std::string asciiFileName);
    
//    void WriteXtomoToAsciiFile(std::string filename);

    // data members for DblQ00/XTOMO fields
    struct XTOMO {
          int line; // LINE NUMBER
          std::string XTOMOCHBERNAME; 
	  std::string XTOMOSITE;     
	  int XTOMOSITEID;     
	  int XTOMOTIME; 
	  int XTOMOPASSED;     
	  std::string XTOMOSIDE; 
	  int XTOMONBERTUBE1;  
	  int XTOMONBERTUBE2;  
	  int XTOMONBERML;     
	  int XTOMONBERLAYER;  
	  int XTOMOML1STAGG;
	  int XTOMOML2STAGG;
	  float XTOMOD1;
	  int XTOMONMEZ;
	  float XTOMOML1NYTUB;   
	  float XTOMOML1NZTUB;   
	  float XTOMOML1NDELA;   
	  float XTOMOML1NYPIT;   
	  float XTOMOML1NZPIT;   
	  float XTOMOML1PYTUB;   
	  float XTOMOML1PZTUB;   
	  float XTOMOML1PDELA;   
	  float XTOMOML1PYPIT;   
	  float XTOMOML1PZPIT;   
	  float XTOMOML2NYTUB;   
	  float XTOMOML2NZTUB;   
	  float XTOMOML2NDELA;   
	  float XTOMOML2NYPIT;   
	  float XTOMOML2NZPIT;   
	  float XTOMOML2PYTUB;   
	  float XTOMOML2PZTUB;   
	  float XTOMOML2PDELA;   
	  float XTOMOML2PYPIT;   
	  float XTOMOML2PZPIT;
    };

    const XTOMO* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "XTOMO"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "XTOMO"; };

private:
    XTOMO* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Xtomo & operator=(const DblQ00Xtomo &right);
    DblQ00Xtomo(const DblQ00Xtomo&);
};


} // end of MuonGM namespace

#endif // DBLQ00_ASZT_H

