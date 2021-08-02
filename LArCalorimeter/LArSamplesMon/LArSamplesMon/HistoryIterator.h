/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::HistoryIterator
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_HistoryIterator_H
#define LArSamples_HistoryIterator_H

namespace LArSamples {
  
  class Interface;
  class History;
  
  class HistoryIterator  {
  
    public:
   
      /** @brief Constructor  */
      HistoryIterator(const Interface& interface, unsigned int pos = 0, double eMin = -1, double adcMaxMin = -1);
      
      virtual ~HistoryIterator() { }
      
      const History* history() const;
      unsigned int pos() const { return m_pos; }
      
      bool isValid() const;
      
      const History* operator*() const { return history(); } 
      HistoryIterator* operator++(); 
      const History* next(); 

      bool operator==(unsigned int otherPos) const { return pos() == otherPos; }
      
     private:
       
       const Interface* m_interface;
       unsigned int m_pos;
              
       double m_eMin, m_adcMaxMin;
  };
}
  
#endif
