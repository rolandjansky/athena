/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcIlineStore.h"
 
AmdcIlineStore::AmdcIlineStore(){ m_Active = 0 ; } 
AmdcIlineStore::~AmdcIlineStore(){}

// Reset the Store
void AmdcIlineStore::Reset(){m_AmdcIlineSet.clear();}
 
// Print Out the Store
void AmdcIlineStore::PrintSummary() const
{

 std::cout << " AmdcIlineStore size is " << m_AmdcIlineSet.size() << std::endl;

}
void AmdcIlineStore::Print(std::ostream* out) const
{

  int TheSize = m_AmdcIlineSet.size();
  if ( TheSize==0 ){
   *out << " AmdcIlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   m_AmdcIlineSet[Index].Print(out);
  }   
  
}
void AmdcIlineStore::Print() const
{

  int TheSize = m_AmdcIlineSet.size();
  if ( TheSize==0 ){
   std::cout << " AmdcIlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   m_AmdcIlineSet[Index].Print();
  }   
  
}

// Add a AmdcIline
void AmdcIlineStore::Add(const AmdcIline& ToBeAdded){m_AmdcIlineSet.push_back(ToBeAdded);}
 
// Nber of AmdcIline
int AmdcIlineStore::NberOfObjects() const{return m_AmdcIlineSet.size();}
 
// Get a Pointer on a AmdcIline of Nber AmdcIlineNber
const AmdcIline* AmdcIlineStore::GetAmdcIline(int AmdcIlineNber) const{
 const AmdcIline* ToBeReturned = nullptr ;
 if (AmdcIlineNber < 0 || AmdcIlineNber > NberOfObjects()) return ToBeReturned;
 return &m_AmdcIlineSet[AmdcIlineNber];
}
AmdcIline* AmdcIlineStore::GetAmdcIlineForUpdate(int AmdcIlineNber){
 AmdcIline* ToBeReturned = nullptr ;
 if (AmdcIlineNber < 0 || AmdcIlineNber > NberOfObjects()) return ToBeReturned;
 return &m_AmdcIlineSet[AmdcIlineNber];
}

void AmdcIlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  AmdcIlineStore::GetActive(){return m_Active;}
      
// Sort the Store
void AmdcIlineStore::Sort(){

  int TheSize = m_AmdcIlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<AmdcIline>  AmdcIlineSetTEMP ;
  AmdcIlineSetTEMP.clear();
  
  
  int jtyp_Max = m_AmdcIlineSet[1].Getjtyp() ;
  int jtyp_Min = m_AmdcIlineSet[1].Getjtyp() ;
  int jff_Max  = m_AmdcIlineSet[1].Getjff()  ;
  int jff_Min  = m_AmdcIlineSet[1].Getjff()  ;
  int jzz_Max  = m_AmdcIlineSet[1].Getjzz()  ;
  int jzz_Min  = m_AmdcIlineSet[1].Getjzz()  ;
  int job_Max  = m_AmdcIlineSet[1].Getjob()  ;
  int job_Min  = m_AmdcIlineSet[1].Getjob()  ;
  int jlay_Max = m_AmdcIlineSet[1].Getjlay() ;
  int jlay_Min = m_AmdcIlineSet[1].Getjlay() ;

  for (int Index= 0 ; Index <TheSize ; Index++){
    int jtyp = m_AmdcIlineSet[Index].Getjtyp() ;
    int jff  = m_AmdcIlineSet[Index].Getjff()  ;
    int jzz  = m_AmdcIlineSet[Index].Getjzz()  ;
    int job  = m_AmdcIlineSet[Index].Getjob()  ;
    int jlay = m_AmdcIlineSet[Index].Getjlay() ;
    if ( jtyp > jtyp_Max ) jtyp_Max = jtyp ;
    if ( jtyp < jtyp_Min ) jtyp_Min = jtyp ;
    if ( jff  > jff_Max  ) jff_Max  = jff  ;
    if ( jff  < jff_Min  ) jff_Min  = jff  ;
    if ( jzz  > jzz_Max  ) jzz_Max  = jzz  ;
    if ( jzz  < jzz_Min  ) jzz_Min  = jzz  ;
    if ( job  > job_Max  ) job_Max  = job  ;
    if ( job  < job_Min  ) job_Min  = job  ;
    if ( jlay > jlay_Max ) jlay_Max = jlay ;
    if ( jlay < jlay_Min ) jlay_Min = jlay ;
  }   
  
  for (int JTYP = jtyp_Min ; JTYP <= jtyp_Max ; JTYP++ ){
  for (int JFF  = jff_Min  ; JFF  <= jff_Max  ; JFF++  ){
  for (int JZZ  = jzz_Min  ; JZZ  <= jzz_Max  ; JZZ++  ){
  for (int JOB  = job_Min  ; JOB  <= job_Max  ; JOB++  ){
  for (int JLAY = jlay_Min ; JLAY <= jlay_Min ; JLAY++ ){
    int Index  =getData(JTYP,JFF,JZZ,JOB,JLAY) ;
    if ( Index != -1 ) AmdcIlineSetTEMP.push_back(m_AmdcIlineSet[Index]) ;
  }
  }
  }   
  }   
  }
  
  
  if (m_AmdcIlineSet.size() !=  AmdcIlineSetTEMP.size()){
    std::cout << " AmdcIlineStore::Sort "
              << " Probleme m_AmdcIlineSet.size() !=  AmdcIlineSetTEMP.size() "
              << std::endl ;
  }
  
  m_AmdcIlineSet.clear();
  TheSize = AmdcIlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    m_AmdcIlineSet.push_back(AmdcIlineSetTEMP[Index]) ;
  }
     
}
 
int AmdcIlineStore::getData(int JTYP,int JFF,int JZZ,int JOB,int JLAY){
  int ToBeReturned = -1;
  
  int TheSize = m_AmdcIlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (m_AmdcIlineSet[Index].Getjtyp() == JTYP){
    if (m_AmdcIlineSet[Index].Getjff()  == JFF ){
    if (m_AmdcIlineSet[Index].Getjzz()  == JZZ ){
    if (m_AmdcIlineSet[Index].Getjob()  == JOB ){
    if (m_AmdcIlineSet[Index].Getjlay() == JLAY){
      ToBeReturned = Index ;
      break ;
    }
    }
    }
    }
    }
  }   
  return ToBeReturned ;
}
