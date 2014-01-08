/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyIlineStore.h"
 
MboyIlineStore::MboyIlineStore(){ m_Active = 0 ; }
MboyIlineStore::~MboyIlineStore(){}

// Reset the Store
void MboyIlineStore::Reset(){MboyIlineSet.clear();}
 
// Print Out the Store
void MboyIlineStore::PrintSummary() const
{

 std::cout << " MboyIlineStore size is " << MboyIlineSet.size() << std::endl;

}
void MboyIlineStore::Print(std::ostream* out) const
{

  int TheSize = MboyIlineSet.size();
  if ( TheSize==0 ){
   *out << " MboyIlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyIlineSet[Index].Print(out);
  }   
  
}
void MboyIlineStore::Print() const
{

  int TheSize = MboyIlineSet.size();
  if ( TheSize==0 ){
   std::cout << " MboyIlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyIlineSet[Index].Print();
  }   
  
}

// Add a MboyIline
void MboyIlineStore::Add(MboyIline ToBeAdded){MboyIlineSet.push_back(ToBeAdded);}
 
// Nber of MboyIline
int MboyIlineStore::NberOfObjects() const{return MboyIlineSet.size();}
 
// Get a Pointer on a MboyIline of Nber MboyIlineNber
const MboyIline* MboyIlineStore::GetMboyIline(int MboyIlineNber) const{
 const MboyIline* ToBeReturned = 0 ;
 if (MboyIlineNber < 0 || MboyIlineNber > NberOfObjects()) return ToBeReturned;
 return &MboyIlineSet[MboyIlineNber];
}
MboyIline* MboyIlineStore::GetMboyIlineForUpdate(int MboyIlineNber){
 MboyIline* ToBeReturned = 0 ;
 if (MboyIlineNber < 0 || MboyIlineNber > NberOfObjects()) return ToBeReturned;
 return &MboyIlineSet[MboyIlineNber];
}

void MboyIlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  MboyIlineStore::GetActive(){return m_Active;}
      
// Sort the Store
void MboyIlineStore::Sort(){

  int TheSize = MboyIlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<MboyIline>  MboyIlineSetTEMP ;
  MboyIlineSetTEMP.clear();
  
  int jtyp_Max = MboyIlineSet[1].Getjtyp() ;
  int jtyp_Min = MboyIlineSet[1].Getjtyp() ;
  int jff_Max  = MboyIlineSet[1].Getjff()  ;
  int jff_Min  = MboyIlineSet[1].Getjff()  ;
  int jzz_Max  = MboyIlineSet[1].Getjzz()  ;
  int jzz_Min  = MboyIlineSet[1].Getjzz()  ;
  int job_Max  = MboyIlineSet[1].Getjob()  ;
  int job_Min  = MboyIlineSet[1].Getjob()  ;
  int jlay_Max = MboyIlineSet[1].Getjlay() ;
  int jlay_Min = MboyIlineSet[1].Getjlay() ;

  for (int Index= 0 ; Index <TheSize ; Index++){
    int jtyp = MboyIlineSet[Index].Getjtyp() ;
    int jff  = MboyIlineSet[Index].Getjff()  ;
    int jzz  = MboyIlineSet[Index].Getjzz()  ;
    int job  = MboyIlineSet[Index].Getjob()  ;
    int jlay = MboyIlineSet[Index].Getjlay() ;
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
    if ( Index != -1 ) MboyIlineSetTEMP.push_back(MboyIlineSet[Index]) ;
  }
  }   
  }   
  }
  }
  
  if (MboyIlineSet.size() !=  MboyIlineSetTEMP.size()){
    std::cout << " MboyIlineStore::Sort "
              << " Probleme MboyIlineSet.size() !=  MboyIlineSetTEMP.size() "
              << std::endl ;
  }
  
  MboyIlineSet.clear();
  TheSize = MboyIlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    MboyIlineSet.push_back(MboyIlineSetTEMP[Index]) ;
  }
     
}
 
int MboyIlineStore::getData(int JTYP,int JFF,int JZZ,int JOB,int JLAY){
  int ToBeReturned = -1;
  
  int TheSize = MboyIlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (MboyIlineSet[Index].Getjtyp() == JTYP){
    if (MboyIlineSet[Index].Getjff()  == JFF ){
    if (MboyIlineSet[Index].Getjzz()  == JZZ ){
    if (MboyIlineSet[Index].Getjob()  == JOB ){
    if (MboyIlineSet[Index].Getjlay() == JLAY){
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
 
