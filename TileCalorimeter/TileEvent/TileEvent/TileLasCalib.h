/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Marius Cornelis van Woerden*/
/*date Nov/2014*/
/*mvanwoer@cern.ch*/

#ifndef TILELASCALIB_H
#define TILELASCALIB_H

#include <math.h>


  
class TileLasCalib {
  
 public:
  
   TileLasCalib() 
     : m_mean_LG(-99.)
     , m_sigma_LG(-99.)
     , m_mean_HG(-99.)
     , m_sigma_HG(-99.)
     , m_type(-1)
     , m_nevts(0)
     , m_isSet_LG(false)
     , m_isSet_HG(false) { }
    
    ~TileLasCalib() { }
    
    void setCalib(int type, double sumXinQDC, double sumX2inQDC, int nevts, int gain){
      if(nevts > 0 && !isSet(gain) && (gain == LG || gain == HG) ){
        double tmp_mean = sumXinQDC / double(nevts);
        double tmp_sigma = sqrt(sumX2inQDC / nevts - tmp_mean * tmp_mean);
        tmp_mean = 8500. - tmp_mean; // CONVERT QDC TO ADC COUNTS
        if(tmp_mean > 0){
          if(gain == LG){
            m_mean_LG = tmp_mean;
            m_sigma_LG = tmp_sigma;
            m_isSet_LG = true;
          } else { // IF
            m_mean_HG = tmp_mean;
            m_sigma_HG = tmp_sigma;
            m_isSet_HG = true;
          } // ELSE
          m_type = type;
          m_nevts = nevts;
        } // IF
      } // IF
    } // setPed
    
    // MEAN IS CALCULATED FROM SUMX AND N EVENTS
    // <X> = SUMX / N
    // CONVERTED FROM QDC TO ADC BY
    // <X>' = 8500-<X>
    
    double getMean(int gain) const {
      if(gain == LG) return m_mean_LG;
      return m_mean_HG;
    } // getMean
    
    // SIGMA IS CALCULATED USING MEAN <X> AND SUMX^2
    // SIGMA = SQRT(SUMX^2/N - <X>^2)
    
    double getSigma(int gain) const{
      if(gain == LG) return m_sigma_LG;
      return m_sigma_HG;
    } // getSigma
    
    // DATA ACQUISITION TYPE VALUES
    // TYPE       HEX   DEC
    // Pedestal   0x10  16
    // Alpha      0x11  17
    // Led        0x12  18
    // Linearity  0x13  19
    // Laser      0x14  20
    
    // CALIBRATION TYPE VALUES
    // TYPE       HEX   DEC
    // Pedestal   0x00  0
    // Pedestal   0x01  1
    // Led        0x02  2
    // Alpha      0x03  3
    
    int getType() const{
      return m_type;
    } // getType
    
    // NUMBER OF EVENTS USED IN PEDESTAL RUN
    
    int getN() const{
      return m_nevts;
    } // getN
    
    bool isSet(int gain) const{
      if(gain == LG) return m_isSet_LG;
      if(gain == HG) return m_isSet_HG;
      //if(gain != LG && gain != HG) return false;
      return false;
    } // isSet
    
  private:
    
    static const int HG = 1;
    static const int LG = 0;
    double m_mean_LG;
    double m_sigma_LG;
    double m_mean_HG;
    double m_sigma_HG;
    int m_type;
    int m_nevts;
    bool m_isSet_LG;
    bool m_isSet_HG;
    
  };
  

#endif
