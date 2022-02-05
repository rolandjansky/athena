/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCONDDATA_H
#define CSCCONDDATA_H
/**
        @class CscCondData

        @author lampen@physics.arizona.edu

        @brief Containes information for a single index value for a CSC COOL data object. 

 Every value from the CSC COOL database is kept in a CscCondData object.
 **/


namespace MuonCalib {
  template <class data_type> class CscCondData
  {
    public:
    CscCondData(const data_type & value) : m_value (value) {  }
      void setValue(const data_type & value);
      inline const data_type & getValue() const;
    private:
      data_type m_value; 
  };
 
  
 template <class data_type>  
   inline void  CscCondData<data_type>::setValue(const data_type & value) { 
     m_value = value; 
   }
 template <class data_type>  
  inline const data_type & CscCondData<data_type>::getValue() const {
    return m_value;
  }
}//end namespace MuonCalib
#endif
