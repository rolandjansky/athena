/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGISETTINGS_H
#define LUCID_DIGISETTINGS_H

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <map>
#include <string>

class IMessageSvc;
class Algorithm;

class LUCID_DigiSettings {
  
 public:
  
  LUCID_DigiSettings();

  void   Initialize(IMessageSvc* msgSvc);
  
  void   SetDigiParDouble(std::string, double);
  void   SetDigiParInt   (std::string, int);
  double GetDigiParDouble(std::string);
  int    GetDigiParInt   (std::string);
  
  void   OverwriteDigiParProperties(Algorithm*);
  void   OverwriteDigiParValues();
  
  void Print() const;
  
 private:   
  
  void SetDefaultValues();

  IMessageSvc* m_msgSvc;

  int    m_numTubes;
  int    m_qdcChannelsPerPE;
  double m_qdcFedNoiseFactor;
  double m_tdcPmtNoiseFactor;
  double m_tdcFedNoiseFactor;

  double m_parValueNotSetByUserDouble;
  int    m_parValueNotSetByUserInt;

  void DefNewParameterDouble(std::string, std::string, double*, double, double);
  void DefNewParameterInt   (std::string, std::string, int*,    int,    int); 

  struct parDouble {  

    std::string parDescription;
    double* par;
    double  low;
    double  high;   
    double  overwriteVal;
  };
  
  struct parInt {  
    
    std::string parDescription;
    int* par;
    int  low;
    int  high;
    int  overwriteVal; 
  };
  
  std::map <std::string, parDouble> m_doubleMap;
  std::map <std::string, parInt>    m_intMap;
};

#endif
