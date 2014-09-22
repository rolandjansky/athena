/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcAlineStore.h"
 
AmdcAlineStore::AmdcAlineStore(){ m_Active = 0 ; }
AmdcAlineStore::~AmdcAlineStore(){}

// Reset the Store
void AmdcAlineStore::Reset(){AmdcAlineSet.clear();}
 
// Print Out the Store
void AmdcAlineStore::PrintSummary() const
{

 std::cout << " AmdcAlineStore size is " << AmdcAlineSet.size() << std::endl;

}
void AmdcAlineStore::Print(std::ostream* out) const
{

  int TheSize = AmdcAlineSet.size();
  if ( TheSize==0 ){
   *out << " AmdcAlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   AmdcAlineSet[Index].Print(out);
  }   
  
}
void AmdcAlineStore::Print() const
{

  int TheSize = AmdcAlineSet.size();
  if ( TheSize==0 ){
   std::cout << " AmdcAlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   AmdcAlineSet[Index].Print();
  }   
  
}

// Add a AmdcAline
void AmdcAlineStore::Add(AmdcAline ToBeAdded){AmdcAlineSet.push_back(ToBeAdded);}
 
// Nber of AmdcAline
int AmdcAlineStore::NberOfObjects() const{return AmdcAlineSet.size();}
 
// Get a Pointer on a AmdcAline of Nber AmdcAlineNber
const AmdcAline* AmdcAlineStore::GetAmdcAline(int AmdcAlineNber) const{
 const AmdcAline* ToBeReturned = 0 ;
 if (AmdcAlineNber < 0 || AmdcAlineNber > NberOfObjects()) return ToBeReturned;
 return &AmdcAlineSet[AmdcAlineNber];
}
AmdcAline* AmdcAlineStore::GetAmdcAlineForUpdate(int AmdcAlineNber){
 AmdcAline* ToBeReturned = 0 ;
 if (AmdcAlineNber < 0 || AmdcAlineNber > NberOfObjects()) return ToBeReturned;
 return &AmdcAlineSet[AmdcAlineNber];
}

void AmdcAlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  AmdcAlineStore::GetActive(){return m_Active;}
      
// Sort the Store
void AmdcAlineStore::Sort(){

  int TheSize = AmdcAlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<AmdcAline>  AmdcAlineSetTEMP ;
  AmdcAlineSetTEMP.clear();
  
  
  int jtyp_Max = AmdcAlineSet[1].Getjtyp() ;
  int jtyp_Min = AmdcAlineSet[1].Getjtyp() ;
  int jff_Max  = AmdcAlineSet[1].Getjff()  ;
  int jff_Min  = AmdcAlineSet[1].Getjff()  ;
  int jzz_Max  = AmdcAlineSet[1].Getjzz()  ;
  int jzz_Min  = AmdcAlineSet[1].Getjzz()  ;
  int job_Max  = AmdcAlineSet[1].Getjob()  ;
  int job_Min  = AmdcAlineSet[1].Getjob()  ;

  for (int Index= 0 ; Index <TheSize ; Index++){
    int jtyp = AmdcAlineSet[Index].Getjtyp() ;
    int jff  = AmdcAlineSet[Index].Getjff()  ;
    int jzz  = AmdcAlineSet[Index].Getjzz()  ;
    int job  = AmdcAlineSet[Index].Getjob()  ;
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
    if ( Index != -1 ) AmdcAlineSetTEMP.push_back(AmdcAlineSet[Index]) ;
  }
  }   
  }   
  }
  
  
  if (AmdcAlineSet.size() !=  AmdcAlineSetTEMP.size()){
    std::cout << " AmdcAlineStore::Sort "
              << " Probleme AmdcAlineSet.size() !=  AmdcAlineSetTEMP.size() "
              << std::endl ;
  }
  
  AmdcAlineSet.clear();
  TheSize = AmdcAlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    AmdcAlineSet.push_back(AmdcAlineSetTEMP[Index]) ;
  }
     
}
 
int AmdcAlineStore::getData(int JTYP,int JFF,int JZZ,int JOB){
  int ToBeReturned = -1;
  
  int TheSize = AmdcAlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (AmdcAlineSet[Index].Getjtyp() == JTYP){
    if (AmdcAlineSet[Index].Getjff()  == JFF ){
    if (AmdcAlineSet[Index].Getjzz()  == JZZ ){
    if (AmdcAlineSet[Index].Getjob()  == JOB ){
      ToBeReturned = Index ;
      break ;
    }
    }
    }
    }
  }   
  return ToBeReturned ;
}
