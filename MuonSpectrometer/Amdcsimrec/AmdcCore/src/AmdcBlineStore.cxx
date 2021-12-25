/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcBlineStore.h"
 
AmdcBlineStore::AmdcBlineStore(){ m_Active = 0 ; }
AmdcBlineStore::~AmdcBlineStore(){}

// Reset the Store
void AmdcBlineStore::Reset(){m_AmdcBlineSet.clear();}
 
// Print Out the Store
void AmdcBlineStore::PrintSummary() const
{

 std::cout << " AmdcBlineStore size is " << m_AmdcBlineSet.size() << std::endl;

}
void AmdcBlineStore::Print(std::ostream* out) const
{

  int TheSize = m_AmdcBlineSet.size();
  if ( TheSize==0 ){
   *out << " AmdcBlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   m_AmdcBlineSet[Index].Print(out);
  }   
  
}
void AmdcBlineStore::Print() const
{

  int TheSize = m_AmdcBlineSet.size();
  if ( TheSize==0 ){
   std::cout << " AmdcBlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   m_AmdcBlineSet[Index].Print();
  }   
  
}

// Add a AmdcBline
void AmdcBlineStore::Add(const AmdcBline& ToBeAdded){m_AmdcBlineSet.push_back(ToBeAdded);}
 
// Nber of AmdcBline
int AmdcBlineStore::NberOfObjects() const{return m_AmdcBlineSet.size();}
 
// Get a Pointer on a AmdcBline of Nber AmdcBlineNber
const AmdcBline* AmdcBlineStore::GetAmdcBline(int AmdcBlineNber) const{
 const AmdcBline* ToBeReturned = nullptr ;
 if (AmdcBlineNber < 0 || AmdcBlineNber > NberOfObjects()) return ToBeReturned;
 return &m_AmdcBlineSet[AmdcBlineNber];
}
AmdcBline* AmdcBlineStore::GetAmdcBlineForUpdate(int AmdcBlineNber){
 AmdcBline* ToBeReturned = nullptr ;
 if (AmdcBlineNber < 0 || AmdcBlineNber > NberOfObjects()) return ToBeReturned;
 return &m_AmdcBlineSet[AmdcBlineNber];
}

void AmdcBlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  AmdcBlineStore::GetActive(){return m_Active;}

// Sort the Store
void AmdcBlineStore::Sort(){

  int TheSize = m_AmdcBlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<AmdcBline>  AmdcBlineSetTEMP ;
  AmdcBlineSetTEMP.clear();
  
  
  int jtyp_Max = m_AmdcBlineSet[1].Getjtyp() ;
  int jtyp_Min = m_AmdcBlineSet[1].Getjtyp() ;
  int jff_Max  = m_AmdcBlineSet[1].Getjff()  ;
  int jff_Min  = m_AmdcBlineSet[1].Getjff()  ;
  int jzz_Max  = m_AmdcBlineSet[1].Getjzz()  ;
  int jzz_Min  = m_AmdcBlineSet[1].Getjzz()  ;
  int job_Max  = m_AmdcBlineSet[1].Getjob()  ;
  int job_Min  = m_AmdcBlineSet[1].Getjob()  ;

  for (int Index= 0 ; Index <TheSize ; Index++){
    int jtyp = m_AmdcBlineSet[Index].Getjtyp() ;
    int jff  = m_AmdcBlineSet[Index].Getjff()  ;
    int jzz  = m_AmdcBlineSet[Index].Getjzz()  ;
    int job  = m_AmdcBlineSet[Index].Getjob()  ;
    if ( jtyp > jtyp_Max ) jtyp_Max = jtyp ;
    if ( jtyp < jtyp_Min ) jtyp_Min = jtyp ;
    if ( jff  > jff_Max  ) jff_Max  = jff  ;
    if ( jff  < jff_Min  ) jff_Min  = jff  ;
    if ( jzz  > jzz_Max  ) jzz_Max  = jzz  ;
    if ( jzz  < jzz_Min  ) jzz_Min  = jzz  ;
    if ( job  > job_Max  ) job_Max  = job  ;
    if ( job  < job_Min  ) job_Min  = job  ;
  }   
  
  for (int JTYP = jtyp_Min ; JTYP <= jtyp_Max ; JTYP++ ){
  for (int JFF  = jff_Min  ; JFF  <= jff_Max  ; JFF++  ){
  for (int JZZ  = jzz_Min  ; JZZ  <= jzz_Max  ; JZZ++  ){
  for (int JOB  = job_Min  ; JOB  <= job_Max  ; JOB++  ){
    int Index  =getData(JTYP,JFF,JZZ,JOB) ;
    if ( Index != -1 ) AmdcBlineSetTEMP.push_back(m_AmdcBlineSet[Index]) ;
  }
  }   
  }   
  }
  
  
  if (m_AmdcBlineSet.size() !=  AmdcBlineSetTEMP.size()){
    std::cout << " AmdcBlineStore::Sort "
              << " Probleme m_AmdcBlineSet.size() !=  AmdcBlineSetTEMP.size() "
              << std::endl ;
  }
  
  m_AmdcBlineSet.clear();
  TheSize = AmdcBlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    m_AmdcBlineSet.push_back(AmdcBlineSetTEMP[Index]) ;
  }
     
}
 
int AmdcBlineStore::getData(int JTYP,int JFF,int JZZ,int JOB){
  int ToBeReturned = -1;
  
  int TheSize = m_AmdcBlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (m_AmdcBlineSet[Index].Getjtyp() == JTYP){
    if (m_AmdcBlineSet[Index].Getjff()  == JFF ){
    if (m_AmdcBlineSet[Index].Getjzz()  == JZZ ){
    if (m_AmdcBlineSet[Index].Getjob()  == JOB ){
      ToBeReturned = Index ;
      break ;
    }
    }
    }
    }
  }   
  return ToBeReturned ;
}
