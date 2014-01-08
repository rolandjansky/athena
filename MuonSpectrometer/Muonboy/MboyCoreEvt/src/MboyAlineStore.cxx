/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyAlineStore.h"
 
MboyAlineStore::MboyAlineStore(){ m_Active = 0 ; }
MboyAlineStore::~MboyAlineStore(){}

// Reset the Store
void MboyAlineStore::Reset(){MboyAlineSet.clear();}
 
// Print Out the Store
void MboyAlineStore::PrintSummary() const
{

 std::cout << " MboyAlineStore size is " << MboyAlineSet.size() << std::endl;

}
void MboyAlineStore::Print(std::ostream* out) const
{

  int TheSize = MboyAlineSet.size();
  if ( TheSize==0 ){
   *out << " MboyAlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyAlineSet[Index].Print(out);
  }   
  
}
void MboyAlineStore::Print() const
{

  int TheSize = MboyAlineSet.size();
  if ( TheSize==0 ){
   std::cout << " MboyAlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyAlineSet[Index].Print();
  }   
  
}

// Add a MboyAline
void MboyAlineStore::Add(MboyAline ToBeAdded){MboyAlineSet.push_back(ToBeAdded);}
 
// Nber of MboyAline
int MboyAlineStore::NberOfObjects() const{return MboyAlineSet.size();}
 
// Get a Pointer on a MboyAline of Nber MboyAlineNber
const MboyAline* MboyAlineStore::GetMboyAline(int MboyAlineNber) const{
 const MboyAline* ToBeReturned = 0 ;
 if (MboyAlineNber < 0 || MboyAlineNber > NberOfObjects()) return ToBeReturned;
 return &MboyAlineSet[MboyAlineNber];
}
MboyAline* MboyAlineStore::GetMboyAlineForUpdate(int MboyAlineNber){
 MboyAline* ToBeReturned = 0 ;
 if (MboyAlineNber < 0 || MboyAlineNber > NberOfObjects()) return ToBeReturned;
 return &MboyAlineSet[MboyAlineNber];
}

void MboyAlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  MboyAlineStore::GetActive(){return m_Active;}
      
// Sort the Store
void MboyAlineStore::Sort(){

  int TheSize = MboyAlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<MboyAline>  MboyAlineSetTEMP ;
  MboyAlineSetTEMP.clear();
  
  
  int jtyp_Max = MboyAlineSet[1].Getjtyp() ;
  int jtyp_Min = MboyAlineSet[1].Getjtyp() ;
  int jff_Max  = MboyAlineSet[1].Getjff()  ;
  int jff_Min  = MboyAlineSet[1].Getjff()  ;
  int jzz_Max  = MboyAlineSet[1].Getjzz()  ;
  int jzz_Min  = MboyAlineSet[1].Getjzz()  ;
  int job_Max  = MboyAlineSet[1].Getjob()  ;
  int job_Min  = MboyAlineSet[1].Getjob()  ;

  for (int Index= 0 ; Index <TheSize ; Index++){
    int jtyp = MboyAlineSet[Index].Getjtyp() ;
    int jff  = MboyAlineSet[Index].Getjff()  ;
    int jzz  = MboyAlineSet[Index].Getjzz()  ;
    int job  = MboyAlineSet[Index].Getjob()  ;
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
    if ( Index != -1 )  MboyAlineSetTEMP.push_back(MboyAlineSet[Index]) ;
  }   
  }   
  }
  }
  
  
  if (MboyAlineSet.size() !=  MboyAlineSetTEMP.size()){
    std::cout << " MboyAlineStore::Sort "
              << " Probleme MboyAlineSet.size() !=  MboyAlineSetTEMP.size() "
              << std::endl ;
  }
  
  MboyAlineSet.clear();
  TheSize = MboyAlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    MboyAlineSet.push_back(MboyAlineSetTEMP[Index]) ;
  }
     
}
 
int MboyAlineStore::getData(int JTYP,int JFF,int JZZ,int JOB){
  int ToBeReturned = -1;
  
  int TheSize = MboyAlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (MboyAlineSet[Index].Getjtyp() == JTYP){
    if (MboyAlineSet[Index].Getjff()  == JFF ){
    if (MboyAlineSet[Index].Getjzz()  == JZZ ){
    if (MboyAlineSet[Index].Getjob()  == JOB ){
      ToBeReturned = Index ;
      break ;
    }
    }
    }
    }
  }   
  return ToBeReturned ;
}
 
