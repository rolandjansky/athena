/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIEtData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIETDATA_H
#define HIETDATA_H
#include <vector>
#include "HIGlobal/HICaloUtil.h"

class AtlasDetectorID;
class Identifier;

/** @class   HIEtData
 *   @author Adam Trzupek <adam.Trzupek@ifj.edu.pl>
 *   @author Dominik Derendarz <Dominik.Derendarz@ifj.edu.pl>
 *   @date 26 June 2009.
 *   @brief Data class for storing in ESD and AOD output from reconstrucion.
 */  

class HIEtData 
  {
  public:
     HIEtData(): m_val(0) {}
     HIEtData(int i): m_val(i) {}

   /** depreciated -  will be removed */  
     void val(int i) { m_val = i; }
   /** depreciated - will be removed */  
     int val() const { return m_val; }
 
   /** Set function for m_Et */  
     void   Et(float et) {m_Et = et; }
   /** Get function for m_Et */  
     float  Et() const {return m_Et; }
   /** Set function for m_Et_final */  
     void   EtvsEta(float * et_vs_eta);
   /** Get function for m_Et_final */  
     std::vector <float> EtvsEta() const {return m_Et_final;} 
     ~HIEtData() {};


  private:
   /** m_val is depreciaded will be removed */  
    int m_val;
   /** total uncalibrated transverse energy calculated from all cells */ 
    float m_Et; 

   /** m_Et_final is transverse energy vs eta reconstructed by mc calibration corrections in eta bin 0.2 and eta range from -5 to 5 */ 
    std::vector <float> m_Et_final; 
    
  }; 


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(HIEtData, 8007, 0)

#endif 
