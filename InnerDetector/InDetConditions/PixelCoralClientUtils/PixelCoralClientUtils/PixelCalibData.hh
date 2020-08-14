/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibData.hh, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBDATA_HH
#define PIXELCALIBDATA_HH

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

/** @class PixelCalibData 

    The PixelCalibData is a class that designed to hold the calibration data for each module that contains 
    16 FE PixelChipSummaryData. The calibration data consist of threshold, timewalk information and Time over 
    threshold calibration that averages per FE chip. The details can be found at 
    https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Calibrations
    .

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/  

namespace PixelCoralClientUtils {


class PixelCalibData{

  public:
  /** Constructor:*/
  PixelCalibData(); 
  PixelCalibData(unsigned int ident,int nmax); 

  /** default destructor */
  ~PixelCalibData () = default;

  const static int nPixelChipSummaryDataMax = 16; /** maximum number of chips per module */
  /** embedded class PixelChipSummaryData holds all the calibration data for a chip */ 

  class PixelChipSummaryData { 

  public: 
    PixelChipSummaryData();
    ~PixelChipSummaryData() = default;

    /** Methods to access the calibration data */ 
    int getChipID() const { return (int)m_chip; } //<! get the chip id inside a module from 0 to 15 
    int getThreshold(int type) const; //<! get threshold for type=0/1/2 for standard,long and ganged pixels 
    int getThresholdSigma(int type) const; //<! get threshold sigma 
    int getNoise(int type) const; //<! get noise 
    int getTimeWalk(int type) const; //<! get time walk  
    float getQ2TotA(int type) const; //<! get charge to tot conversion constants A with Tot=A(Q+E)/(Q+C)  
    float getQ2TotE(int type) const; //<! get constants E 
    float getQ2TotC(int type) const; //<! get constants C
    float getQ2Tot(int type, float Q) const; //<! conver Q to tot 
    float getTotP1() const; //<! get P1 parameter that parametrized the tot resolution sigma=P1 +p2*tot  
    float getTotP2() const; //<! get P2 parameter for tot resolution 
    float getTotRes(float Q) const; //<! get tot resolution for charge Q

    /** Methods to set the calibration data */
    void setChipID(int ichip); //<! set chip id 
    void setThreshold(int type, int thres, int sigma, int noise, int timewalk); //<! set threshold with inputs as int.  
    void setThreshold(int type, float thres, float sigma, float noise, float timewalk);//<! set threshold with inputs as float.
    void setQ2Tot(int type, float A, float E, float C); //<! set charge to tot conversion constants
    void setTotRes(float p1, float p2); //<! set tot resolution dependent constants 

  // for packed format db, this has to be public -- A.X.
  private:
    char m_chip; //<! 0-15 which chip number 
    char m_threshold[4]; //<! one Word(thres, sigma, Noise,TW): 
    char m_thresholdlong[4]; //<! one Word(thres, sigma, Noise,TW) for long pixel
    char m_thresholdganged[4]; //<! one Word(thres, sigma, Noise,TW) for ganged pixel 
    float  m_q2tot[3]; //<! a,c,e for standard pixel and long 
    float  m_q2totganged[3]; //<! a,b,c for ganged 
    char m_totres[2]; //<! for (p1, p2) each chip    

    // alternative to public chip data -- A.X.
    //friend int PixCoralClient::fillTablePixelCalibData(long long fk, const char* option);
    //friend void PixCoralClient::getCalibrationDataFromDB(const char* option);
  }; 
  
  /** define const iterator defination */ 
  typedef std::vector<PixelChipSummaryData>::const_iterator PixelChipSummaryData_citer;
  int size() const;
  PixelChipSummaryData_citer cbegin() const; 
  PixelChipSummaryData_citer cend() const; 

  /** define non-const iterator defination */
  typedef std::vector<PixelChipSummaryData>::iterator PixelChipSummaryData_iter; 
  PixelChipSummaryData_iter begin(); 
  PixelChipSummaryData_iter end(); 

  /** return pointer to particular module */ 
  PixelChipSummaryData_citer findChipID(int ichip) const; 
  PixelChipSummaryData_iter findChipID(int ichip);

  /** return pointer to the identifier */
  unsigned int getModuleID() const; 
  int getNFEIX() const ;

  /** add a new member to the PixelCalibData */
  void add(const PixelChipSummaryData& idat);
  bool update(const PixelChipSummaryData& idat); 
  void sortv(); 
  void print() const; 
  const PixelChipSummaryData* getPixelChipSummaryData(int circ) const; //<! get the pointer to iith PixelChipSummaryData 

 protected: 
  unsigned int m_ident; //<! module identifier
  int m_nfeix; // number of FEIX chips per module
  std::vector<PixelChipSummaryData> m_vec; //<! data vector contains 0-15 PixelChipSummarydata  per module 
}; 

//CLASS_DEF(PixelCalibData, 27089939, 1)

inline int PixelCalibData::PixelChipSummaryData::getThreshold(int type) const{
  // type=0/1/2 for standard, long and ganged pixels 
   if(type==1){ return m_thresholdlong[0]*30+3500;} 
   else if(type==2){ return m_thresholdganged[0]*30+3500;}
   else{ return m_threshold[0]*30+3500;}
}

inline int PixelCalibData::PixelChipSummaryData::getThresholdSigma(int type) const{ 
  // type=0/1/2 for standard, long and ganged pixels 
  if(type==1){ return m_thresholdlong[1]*3+300;} 
  else if(type==2){ return m_thresholdganged[1]*3+300;}
  else{ return m_threshold[1]*3+300;}
 }

 inline int PixelCalibData::PixelChipSummaryData::getNoise(int type) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==1){ return m_thresholdlong[2]*3+300;} 
   else if(type==2){ return m_thresholdganged[2]*3+300;}
   else{ return m_threshold[2]*3+300;}
 }

 inline int PixelCalibData::PixelChipSummaryData::getTimeWalk(int type) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==1){ return m_thresholdlong[3]*30+5000;} 
   else if(type==2){ return m_thresholdganged[3]*30+5000;}
   else{ return m_threshold[3]*30+5000;}
 }

 inline float PixelCalibData::PixelChipSummaryData::getQ2TotA(int type) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==2){ return m_q2totganged[0];} 
   else{ return m_q2tot[0];}
 }

 inline float PixelCalibData::PixelChipSummaryData::getQ2TotE(int type) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==2){ return m_q2totganged[1];} 
   else{ return m_q2tot[1];}
 }

 inline float PixelCalibData::PixelChipSummaryData::getQ2TotC(int type) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==2){ return m_q2totganged[2];} 
   else{ return m_q2tot[2];}
 }

 inline float PixelCalibData::PixelChipSummaryData::getQ2Tot(int type, float Q) const{ 
   // type=0/1/2 for standard, long and ganged pixels 
   if(type==2){ return m_q2totganged[0]*(Q+m_q2totganged[1])/(Q+m_q2totganged[2]);} 
   else{ return m_q2tot[0]*(Q+m_q2tot[1])/(Q+m_q2tot[2]);}
 }

  inline float PixelCalibData::PixelChipSummaryData::getTotP1() const{ return m_totres[0]* (1/100.);  }

  inline float PixelCalibData::PixelChipSummaryData::getTotP2() const{  return m_totres[1]*(1./1000.); }

 inline float PixelCalibData::PixelChipSummaryData::getTotRes(float Q) const{ 
   float p1 = m_totres[0]*(1/100.0);
   float p2 = m_totres[1]*(1/1000.0);
   return p1+p2*Q; 
 }

  //Set Methods
 inline void  PixelCalibData::PixelChipSummaryData::setChipID(int ichip){
   m_chip = (char)ichip; }

 inline void  PixelCalibData::PixelChipSummaryData::setThreshold(int type, float thres, float sigma, float noise, float timewalk){
   using std::abs;
   int ax0 = ((int)thres-3500)/30;
   if(abs(ax0)>127) ax0 = 127*ax0/abs(ax0);
   int ax1 = ((int)sigma-300)/3;
   if(abs(ax1)>127) ax1 = 127*ax1/abs(ax1);
   int ax2 = ((int)noise-300)/3;
   if(abs(ax2)>127) ax2 = 127*ax2/abs(ax2);
   int ax3 = ((int)timewalk-5000)/30;
   if(abs(ax3)>127) ax3 = 127*ax3/abs(ax3);
                                                                                                                         
   if(type==0) {
     m_threshold[0] = (char)ax0;
     m_threshold[1] = (char)ax1;
     m_threshold[2] = (char)ax2;
     m_threshold[3] = (char)ax3;
   }
   if(type==1){
     m_thresholdlong[0] = (char)ax0;
     m_thresholdlong[1] = (char)ax1;
     m_thresholdlong[2] = (char)ax2;
     m_thresholdlong[3] = (char)ax3;
   }
   if(type==2){
     m_thresholdganged[0] = (char)ax0;
     m_thresholdganged[1] = (char)ax1;
     m_thresholdganged[2] = (char)ax2;
     m_thresholdganged[3] = (char)ax3;
   }
 }                                                                                                                        

 inline void  PixelCalibData::PixelChipSummaryData::setThreshold(int type, int thres, int sigma, int noise, int timewalk){
   using std::abs;
   int ax0 = (thres-3500)/30;
   if(abs(ax0)>127) ax0 = 127*ax0/abs(ax0); 
   int ax1 = (sigma-300)/3; 
   if(abs(ax1)>127) ax1 = 127*ax1/abs(ax1); 
   int ax2 = (noise-300)/3; 
   if(abs(ax2)>127) ax2 = 127*ax2/abs(ax2); 
   int ax3 = (timewalk-5000)/30; 
   if(abs(ax3)>127) ax3 = 127*ax3/abs(ax3);

   if(type==0) {
     m_threshold[0] = (char)ax0;
     m_threshold[1] = (char)ax1; 
     m_threshold[2] = (char)ax2; 
     m_threshold[3] = (char)ax3; 
   }
   if(type==1){
     m_thresholdlong[0] = (char)ax0;
     m_thresholdlong[1] = (char)ax1; 
     m_thresholdlong[2] = (char)ax2; 
     m_thresholdlong[3] = (char)ax3; 
   }
   if(type==2){
     m_thresholdganged[0] = (char)ax0;
     m_thresholdganged[1] = (char)ax1; 
     m_thresholdganged[2] = (char)ax2; 
     m_thresholdganged[3] = (char)ax3; 
   }
 }




inline void PixelCalibData::PixelChipSummaryData::setQ2Tot(int type, float A, float E, float C){
  if(type==0||type==1){
    m_q2tot[0] = A;
    m_q2tot[1] = E; 
    m_q2tot[2] = C; 
  }
  if(type==2){ 
    m_q2totganged[0] = A; 
    m_q2totganged[1] = E; 
    m_q2totganged[2] = C; 
  }
}

inline void PixelCalibData::PixelChipSummaryData::setTotRes(float p1, float p2){

  // careful with rounding -- A.X.
  using std::abs;
  int p1x = (int)roundf(p1*100); 
  if (p1x > 127) p1x = 127;
  else if (p1x < -127) p1x = -127;
  int p2x = (int)roundf(p2*1000); 
  if (p2x > 127) p2x = 127;
  else if (p2x < -127) p2x = -127;

  m_totres[0] = (char)p1x; 
  m_totres[1] = (char)p2x; 
}

  inline bool operator< (const PixelCalibData::PixelChipSummaryData& lhs, 
		       const PixelCalibData::PixelChipSummaryData& rhs){

    return lhs.getChipID()<rhs.getChipID(); 
}

  inline PixelCalibData::PixelCalibData() 
    :   m_ident(-1), m_nfeix(0), m_vec()
{  
  m_vec.reserve(PixelCalibData::nPixelChipSummaryDataMax);
} 

  inline PixelCalibData::PixelCalibData(unsigned int ident,int nmax )
  :
    m_ident(ident), m_nfeix(nmax),m_vec()
  {
   m_vec.reserve(PixelCalibData::nPixelChipSummaryDataMax);
  }
                                                                                
inline void PixelCalibData::add(const PixelChipSummaryData& idat){
   m_vec.push_back(idat);
 }
                                                                                                                
inline  bool PixelCalibData::update(const PixelChipSummaryData& idat){
   PixelChipSummaryData_iter it = findChipID(idat.getChipID());
   if(it==m_vec.end()) return false;
   it->setThreshold(0,idat.getThreshold(0),idat.getThresholdSigma(0),idat.getNoise(0),idat.getTimeWalk(0));
   it->setThreshold(1,idat.getThreshold(1),idat.getThresholdSigma(1),idat.getNoise(1),idat.getTimeWalk(1));
   it->setThreshold(2,idat.getThreshold(2),idat.getThresholdSigma(2),idat.getNoise(2),idat.getTimeWalk(2));
   it->setQ2Tot(0, idat.getQ2TotA(0),idat.getQ2TotE(0),idat.getQ2TotC(0));
   it->setQ2Tot(2, idat.getQ2TotA(2),idat.getQ2TotE(2),idat.getQ2TotC(2));
   it->setTotRes(idat.getTotP1(), idat.getTotP2());
   return true;
 }

inline void PixelCalibData::print() const {
   std::cout<< "PixelCalibData::print  tagname:"<< m_ident<<" vector size "<<m_vec.size()<<std::endl;
   // print out the calibration data for each chip
   int i = 0;
   for(PixelChipSummaryData_citer cit=cbegin(); cit !=cend(); ++cit){
     std::cout<<" Item: "<< i++<<" ChipId: "<<cit->getChipID()
	      <<" TotP1: "<<cit->getTotP1()<<" TotP2: "<<cit->getTotP2()<<std::endl
	      <<" Standard Pixel Threshold: "<<cit->getThreshold(0)<<" sigma: "
	      <<cit->getThresholdSigma(0)<<" noise:"<<cit->getNoise(0)<<" TimeWalk: "
	      <<cit->getTimeWalk(0)<<" Q2TotA: "<<cit->getQ2TotA(0)<<" Q2TotE: "
	      <<cit->getQ2TotE(0)<<" Q2TotC: "<<cit->getQ2TotC(0)<<std::endl
	      <<" Long Pixel Threshold: "<<cit->getThreshold(1)<<" sigma: "
	      <<cit->getThresholdSigma(1)<<" noise:"<<cit->getNoise(1)<<" TimeWalk: "
	      <<cit->getTimeWalk(1)<<" Q2TotA: "<<cit->getQ2TotA(1)<<" Q2TotE: "
	      <<cit->getQ2TotE(1)<<" Q2TotC: "<<cit->getQ2TotC(1)<<std::endl
	      <<" Ganged Pixel Threshold: "<<cit->getThreshold(2)<<" sigma: "
	      <<cit->getThresholdSigma(2)<<" noise:"<<cit->getNoise(2)<<" TimeWalk: "
	      <<cit->getTimeWalk(2)<<" Q2TotA: "<<cit->getQ2TotA(2)<<" Q2TotE: "
	      <<cit->getQ2TotE(2)<<" Q2TotC: "<<cit->getQ2TotC(2)<<std::endl;
   }
 }

inline PixelCalibData::PixelChipSummaryData::PixelChipSummaryData(){
  // initialised to zero for all members 
  m_chip=0; 
  for(int i = 0; i<4; ++i){
    m_threshold[i] = 0; 
    m_thresholdlong[i] = 0;
    m_thresholdganged[i] = 0;
    if(i<3){
      m_q2tot[i] = 0.; 
      m_q2totganged[i] = 0.;
    }
    if(i<2)m_totres[i] = 0.;
  }
}


inline int PixelCalibData::size() const
   { return (int)m_vec.size();}
                                                                                                  
inline PixelCalibData::PixelChipSummaryData_citer PixelCalibData::cbegin() const
{ return m_vec.begin();}

inline PixelCalibData::PixelChipSummaryData_citer PixelCalibData::cend() const
{ return m_vec.end();} 

inline PixelCalibData::PixelChipSummaryData_iter PixelCalibData::begin()
{ return m_vec.begin();}

inline PixelCalibData::PixelChipSummaryData_iter PixelCalibData::end()
{ return m_vec.end();} 

inline PixelCalibData::PixelChipSummaryData_citer 
PixelCalibData::findChipID(int ichip) const{
  PixelCalibData::PixelChipSummaryData obj; 
  obj.setChipID(ichip); 
  PixelCalibData::PixelChipSummaryData_citer rc =std::lower_bound(m_vec.begin(),m_vec.end(),obj);
  if(rc !=m_vec.end()&&rc->getChipID()!=ichip) rc = m_vec.end(); 
  return rc; 
}

inline PixelCalibData::PixelChipSummaryData_iter 
PixelCalibData::findChipID(int ichip) {
  PixelCalibData::PixelChipSummaryData obj; 
  obj.setChipID(ichip); 
  PixelCalibData::PixelChipSummaryData_iter rc =std::lower_bound(m_vec.begin(),m_vec.end(),obj);
  if(rc !=m_vec.end()&&rc->getChipID()!=ichip) rc = m_vec.end(); 
  return rc; 
}

inline unsigned int PixelCalibData::getModuleID() const { return m_ident;} 

inline int PixelCalibData::getNFEIX() const { return m_nfeix;}

inline void PixelCalibData::sortv(){ 
  sort(m_vec.begin(), m_vec.end()); }

inline const PixelCalibData::PixelChipSummaryData* PixelCalibData::getPixelChipSummaryData(int circ) const{ // ith chip 
  if(circ>=(int)m_vec.size())return 0; 
  return &m_vec[circ];
}


}
#endif // PIXELCALIBDATA_HH
