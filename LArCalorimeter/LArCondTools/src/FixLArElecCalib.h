/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIXLARELECCALIB_H
#define FIXLARELECCALIB_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include <string>

class  LArEM_ID ;
class  LArHEC_ID;
class  LArFCAL_ID;
class  LArOnlineID;
class  LArOnline_SuperCellID;
class  CaloCell_SuperCell_ID;
class LArOnOffIdMapping;

#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArfSamplMC.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"

// this class is collection of various fixes, applied to LAr conditions,
// when some bug was found.
// should serve mainly as example, in case yet another problem will need a fix
class FixLArElecCalib : public AthAlgorithm
{
 public:
  FixLArElecCalib(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~FixLArElecCalib() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}  
  virtual StatusCode stop() override;  

  StatusCode fix1(const LArOnOffIdMapping *cabling);  
  StatusCode fix2(const LArOnOffIdMapping *cabling);  
  StatusCode fix3(const LArOnOffIdMapping *cabling);  
  StatusCode fix4(const LArOnOffIdMapping *cabling);  
  StatusCode fix5(const LArOnOffIdMapping *cabling);  
  StatusCode fix6(const LArOnOffIdMapping *cabling);  
  StatusCode fix7(const LArOnOffIdMapping *cabling);  
  StatusCode fix8(const LArOnOffIdMapping *cabling);  
  StatusCode fix9(const LArOnOffIdMapping *cabling);  
  StatusCode fix10(const LArOnOffIdMapping *cabling);  
  StatusCode fix11(const LArOnOffIdMapping *cabling);  
  StatusCode fix12(const LArOnOffIdMapping *cabling);  
  StatusCode fix13(const LArOnOffIdMapping *cabling);  
  StatusCode fix14(const LArOnOffIdMapping *cabling);  
  StatusCode fix15();  
  StatusCode fix16();  
  StatusCode fix17(const LArOnOffIdMapping *cabling);  
  StatusCode fix18(const LArCalibLineMapping *clmap);  

 private: 

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "SCCalibLineKey", "LArCalibLineMap", "SG calib line key"};

  StatusCode ReadFile(const std::string& filename, const LArOnOffIdMapping *cabling, bool EM, bool withGain, int nvar );
  StatusCode ReadFileAll(const std::string& filename, const LArOnOffIdMapping *cabling, bool withGain, int nvar );

  StatusCode update_DAC2uA(const std::string& em_filename,const std::string& hec_filename);
  // FIXME: for future round of developments:
  // StatusCode update_uA2MeV(const std::string& em_filename,const std::string& hec_filename);
  //  StatusCode update_AC(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Noise(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Mphys(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Ramp(const std::string& em_filename,const std::string& hec_filename);
  //StatusCode update_Shape(const std::string& em_filename,const std::string& hec_filename);

  template <class T>
  StatusCode update_EM_HEC(const std::string& em_filename,const std::string& hec_filename, const LArOnOffIdMapping *cabling,
                           bool withGain, int nvar);
  template <class T>
  StatusCode update_All(const std::string& filename, const LArOnOffIdMapping *cabling, bool withGain, int nvar);

  void print_object(const std::string& msg, const LArNoiseMC::LArCondObj& obj); 
  void set_object(LArNoiseMC::LArCondObj& obj, const std::vector<float>& v ); 

  void print_object(const std::string& msg, const LArAutoCorrMC::LArCondObj& obj); 
  void set_object(LArAutoCorrMC::LArCondObj& obj, const std::vector<float>& v ); 

  void print_object(const std::string& msg, const LArRampMC::LArCondObj& obj); 
  void set_object(LArRampMC::LArCondObj& obj, const std::vector<float>& v ); 

  //void print_object(const std::string& msg, const LAruA2MeVMC::LArCondObj& obj); 
  //void set_object(LAruA2MeVMC::LArCondObj& obj, std::vector<float>& v ); 

  void print_object(const std::string& msg, const LArShape32MC::LArCondObj& obj); 
  void set_object(LArShape32MC::LArCondObj& obj, const std::vector<float>& v ); 

  void print_object(const std::string& msg, const LArSingleFloatP& obj); 
  void set_object(LArSingleFloatP& obj, const std::vector<float>& v ); 

  //  void print_object(const std::string& msg, const LArDAC2uAMC::LArCondObj& obj); 
  //void set_object(LArDAC2uAMC::LArCondObj& obj, std::vector<float>& v ); 

  StatusCode addMphysOverMcal(const LArOnOffIdMapping *cabling) ; // add new one with default for FCAL

  StatusCode fixDACuAMeV() ; // remove -100000000 


  StatusCode updateEMfSampl(const std::string& filename, const LArOnOffIdMapping *cabling);
  StatusCode updateMinBias(const std::string& filename, const LArOnOffIdMapping *cabling);
  StatusCode updateEM_DACuAMeV(const std::string& filename, const LArOnOffIdMapping *cabling);
  StatusCode updateHADfSampl(const LArOnOffIdMapping *cabling);

  int  m_fixFlag ; 
  std::string m_g4Phys; 
  std::string m_infile; 

  const LArEM_ID* m_em_idhelper;
  const LArHEC_ID* m_hec_idhelper;
  const LArFCAL_ID* m_fcal_idhelper;
  const LArOnlineID* m_online_idhelper;
  const LArEM_SuperCell_ID* m_sem_idhelper;
  const LArHEC_SuperCell_ID* m_shec_idhelper;
  const LArFCAL_SuperCell_ID* m_sfcal_idhelper;
  const LArOnline_SuperCellID* m_sonline_idhelper;
  const CaloCell_SuperCell_ID* m_scell_idhelper;




  typedef std::pair<HWIdentifier,std::vector<float> > ROW_t ;
  typedef std::vector<ROW_t> VROW_t;

  VROW_t m_cache[3] ; 


};

template <class T >
  StatusCode FixLArElecCalib::update_EM_HEC(const std::string& em_filename,const std::string& hec_filename, const LArOnOffIdMapping *cabling, bool withGain, int nvar)
{
   // read in the file

   const T * container_c = nullptr; 
   ATH_CHECK( detStore()->retrieve(container_c) );

   T* container = const_cast<T*>(container_c);
    
   typedef typename T::LArCondObj  CONDOBJ ; 

   for (int i=0; i<2;++i)
     { // EM and HEC
       if( i==0 )
	 {  
	   if ( em_filename == "") continue ; 
	   bool EM=true;
	   ATH_CHECK( ReadFile(em_filename,cabling,EM,withGain,nvar) );
	 }
       if( i==1 )
	 {  
	   if ( hec_filename == "") continue ; 
	   bool EM=false ;
	   ATH_CHECK( ReadFile(hec_filename,cabling,EM,withGain,nvar) );
	 }


       int ngain=1;
       if (withGain) ngain=3;
       for (int igain = 0;igain<ngain;++igain){
	 VROW_t::iterator it = m_cache[igain].begin();
	 VROW_t::iterator it_e = m_cache[igain].end();
     
	 int n=0; 
	 for( ;it!=it_e;++it)
	   {
	     HWIdentifier hid = (*it).first;

	     const CONDOBJ& u = container->get(hid,igain); 

	     if( u.isEmpty() ) 
	       {
		 ATH_MSG_WARNING(" No existing conditions data, adding new one " );
		 CONDOBJ t  ;
		 set_object(t,(*it).second)  ; 
		 container->setPdata(hid,t,igain); 
	       }
	     else
	       {
		 print_object(" Old object = ",u);
		 
		 CONDOBJ& u2 = const_cast<CONDOBJ&>(u); 
		 set_object(u2,(*it).second)  ; 
	       }
		 
	     const CONDOBJ& u3 = container->get(hid,igain); 
	     print_object(" New object = ",u3);
		 
	     ++n;
	   }
	 ATH_MSG_INFO( " done for gain ="<<igain<<" with "<<n<<" objects");

       }// gain
     }// EM HEC

   ATH_MSG_INFO(" done with EM "<<em_filename<<"  and HEC" <<hec_filename );
   return StatusCode::SUCCESS;
}


template <class T >
  StatusCode FixLArElecCalib::update_All(const std::string& filename, const LArOnOffIdMapping *cabling, bool withGain, int nvar)
{
   // read in the file

   const T * container_c = nullptr; 
   ATH_CHECK( detStore()->retrieve(container_c) );

   T* container = const_cast<T*>(container_c);
    
   typedef typename T::LArCondObj  CONDOBJ ; 

   if ( filename == "") { ATH_MSG_ERROR("No input filename "); return StatusCode::FAILURE; }
   ATH_CHECK( ReadFileAll(filename, cabling, withGain,nvar) );

   int ngain=1;
   if (withGain) ngain=3;
   for (int igain = 0;igain<ngain;++igain){
     VROW_t::iterator it = m_cache[igain].begin();
     VROW_t::iterator it_e = m_cache[igain].end();
 
     int n=0; 
     for( ;it!=it_e;++it) {
         HWIdentifier hid = (*it).first;

         if(!hid.is_valid()) {
            ATH_MSG_WARNING(" Not valid hid: 0x" <<hid.getString() );
            continue;
         }

         const CONDOBJ& u = container->get(hid,igain); 

         if( u.isEmpty() ) {
    	 ATH_MSG_WARNING(" Not existing conditions data, adding new one " );
    	 CONDOBJ t  ;
    	 set_object(t,(*it).second)  ; 
    	 container->setPdata(hid,t,igain); 
         } else {
    	 print_object(" Old object = ",u);
    	 
    	 CONDOBJ& u2 = const_cast<CONDOBJ&>(u); 
    	 set_object(u2,(*it).second)  ; 
         }
    	 
         const CONDOBJ& u3 = container->get(hid,igain); 
         print_object(" New object = ",u3);
    	 
         ++n;
     }
     ATH_MSG_INFO( " done for gain ="<<igain<<" with "<<n<<" objects");

   }// gain

   ATH_MSG_INFO(" done with "<<filename );
   return StatusCode::SUCCESS;
}
#endif // FixLArElecCalib
