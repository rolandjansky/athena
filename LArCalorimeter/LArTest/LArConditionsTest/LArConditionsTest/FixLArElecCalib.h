/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIXLARELECCALIB_H
#define FIXLARELECCALIB_H

#include "GaudiKernel/Algorithm.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/StoreGateSvc.h"
#include <string>

class  LArEM_ID ;
class  LArHEC_ID;
class  LArFCAL_ID;
class  LArOnlineID;
class  LArOnline_SuperCellID;
class  LArCablingService;
class  CaloCell_SuperCell_ID;

#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArfSamplMC.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"

class FixLArElecCalib : public Algorithm
{
 public:
  FixLArElecCalib(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~FixLArElecCalib();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

  StatusCode fix1();  
  StatusCode fix2();  
  StatusCode fix3();  
  StatusCode fix4();  
  StatusCode fix5();  
  StatusCode fix6();  
  StatusCode fix7();  
  StatusCode fix8();  
  StatusCode fix9();  
  StatusCode fix10();  
  StatusCode fix11();  
  StatusCode fix12();  
  StatusCode fix13();  

 private: 
  StatusCode ReadFile(const std::string& filename, bool EM, bool withGain, int nvar );

  StatusCode update_DAC2uA(const std::string& em_filename,const std::string& hec_filename);
  // StatusCode update_uA2MeV(const std::string& em_filename,const std::string& hec_filename);
  //  StatusCode update_AC(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Noise(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Mphys(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Ramp(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Shape(const std::string& em_filename,const std::string& hec_filename);

  template <class T>
  StatusCode update_EM_HEC(const std::string& em_filename,const std::string& hec_filename, bool withGain, int nvar);

  void print_object(MsgStream& log, const std::string& msg, const LArNoiseMC::LArCondObj& obj); 
  void set_object(LArNoiseMC::LArCondObj& obj, std::vector<float>& v ); 

  void print_object(MsgStream& log, const std::string& msg, const LArAutoCorrMC::LArCondObj& obj); 
  void set_object(LArAutoCorrMC::LArCondObj& obj, std::vector<float>& v ); 

  void print_object(MsgStream& log, const std::string& msg, const LArRampMC::LArCondObj& obj); 
  void set_object(LArRampMC::LArCondObj& obj, std::vector<float>& v ); 

  //void print_object(MsgStream& log, const std::string& msg, const LAruA2MeVMC::LArCondObj& obj); 
  //void set_object(LAruA2MeVMC::LArCondObj& obj, std::vector<float>& v ); 

  void print_object(MsgStream& log, const std::string& msg, const LArShape32MC::LArCondObj& obj); 
  void set_object(LArShape32MC::LArCondObj& obj, std::vector<float>& v ); 

  void print_object(MsgStream& log, const std::string& msg, const LArSingleFloatP& obj); 
  void set_object(LArSingleFloatP& obj, std::vector<float>& v ); 

  //  void print_object(MsgStream& log, const std::string& msg, const LArDAC2uAMC::LArCondObj& obj); 
  //void set_object(LArDAC2uAMC::LArCondObj& obj, std::vector<float>& v ); 

  StatusCode addMphysOverMcal() ; // add new one with default for FCAL

  StatusCode fixDACuAMeV() ; // remove -100000000 


  StatusCode updateEMfSampl(const std::string& filename);
  StatusCode updateMinBias(const std::string& filename);
  StatusCode updateEM_DACuAMeV(const std::string& filename);
  StatusCode updateHADfSampl();

  int  m_fixFlag ; 
  std::string m_g4Phys; 

  StoreGateSvc * m_detStore;
  const LArEM_ID* m_em_idhelper;
  const LArHEC_ID* m_hec_idhelper;
  const LArFCAL_ID* m_fcal_idhelper;
  const LArOnlineID* m_online_idhelper;
  const LArEM_SuperCell_ID* m_sem_idhelper;
  const LArHEC_SuperCell_ID* m_shec_idhelper;
  const LArFCAL_SuperCell_ID* m_sfcal_idhelper;
  const LArOnline_SuperCellID* m_sonline_idhelper;
  const CaloCell_SuperCell_ID* m_scell_idhelper;
  LArCablingService* m_cablingSvc ;



  typedef std::pair<HWIdentifier,std::vector<float> > ROW ;
  typedef std::vector<ROW> VROW;

  VROW m_cache[3] ; 


};

template <class T >
  StatusCode FixLArElecCalib::update_EM_HEC(const std::string& em_filename,const std::string& hec_filename, bool withGain, int nvar)
{

   // read in the file

   MsgStream  log(messageService(),name());

   const T * container_c; 
   StatusCode sc=m_detStore->retrieve(container_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find Container for "<<em_filename<<" " <<hec_filename <<endreq;
     return sc;
   }

   T* container = const_cast<T*>(container_c);
    
   typedef typename T::LArCondObj  CONDOBJ ; 

   for (int i=0; i<2;++i)
     { // EM and HEC
       if( i==0 )
	 {  
	   if ( em_filename == "") continue ; 
	   bool EM=true;
	   sc = ReadFile(em_filename,EM,withGain,nvar);
	   
	   if(sc!=StatusCode::SUCCESS){ 
	     log<< MSG::ERROR<<" Failed to read file "<<em_filename <<endreq;
	     return sc;
	   }
     
	 }
       if( i==1 )
	 {  
	   if ( hec_filename == "") continue ; 
	   bool EM=false ;
	   sc = ReadFile(hec_filename,EM,withGain,nvar);
	   
	   if(sc!=StatusCode::SUCCESS){ 
	     log<< MSG::ERROR<<" Failed to read file "<<hec_filename <<endreq;
	     return sc;
	   }
     
	 }


       int ngain=1;
       if (withGain) ngain=3;
       for (int igain = 0;igain<ngain;++igain){
	 VROW::iterator it = m_cache[igain].begin();
	 VROW::iterator it_e = m_cache[igain].end();
     
	 int n=0; 
	 for( ;it!=it_e;++it)
	   {
	     HWIdentifier hid = (*it).first;

	     const CONDOBJ& u = container->get(hid,igain); 

	     if( u.isEmpty() ) 
	       {
		 log<<MSG::WARNING<<" No existing conditions data, adding new one " <<endreq;
		 CONDOBJ t  ;
		 set_object(t,(*it).second)  ; 
		 container->setPdata(hid,t,igain); 
	       }
	     else
	       {
		 print_object(log," Old object = ",u);
		 
		 CONDOBJ& u2 = const_cast<CONDOBJ&>(u); 
		 set_object(u2,(*it).second)  ; 
	       }
		 
	     const CONDOBJ& u3 = container->get(hid,igain); 
	     print_object(log," New object = ",u3);
		 
	     ++n;
	   }
	 log<<MSG::INFO<< " done for gain ="<<igain<<" with "<<n<<" objects"<<endreq;

       }// gain
     }// EM HEC

   log<< MSG::INFO<<" done with EM "<<em_filename<<"  and HEC" <<hec_filename <<endreq;
   return StatusCode::SUCCESS;
}


#endif // FixLArElecCalib
