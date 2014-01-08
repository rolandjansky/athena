/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyBlineStore.h"
 
MboyBlineStore::MboyBlineStore(){ m_Active = 0 ; }
MboyBlineStore::~MboyBlineStore(){}

// Reset the Store
void MboyBlineStore::Reset(){MboyBlineSet.clear();}
 
// Print Out the Store
void MboyBlineStore::PrintSummary() const
{

 std::cout << " MboyBlineStore size is " << MboyBlineSet.size() << std::endl;

}
void MboyBlineStore::Print(std::ostream* out) const
{

  int TheSize = MboyBlineSet.size();
  if ( TheSize==0 ){
   *out << " MboyBlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyBlineSet[Index].Print(out);
  }   
  
}
void MboyBlineStore::Print() const
{

  int TheSize = MboyBlineSet.size();
  if ( TheSize==0 ){
   std::cout << " MboyBlineStore is empty " << std::endl;
  }
  for (int Index= 0 ; Index <TheSize ; Index++){
   MboyBlineSet[Index].Print();
  }   
  
}

// Add a MboyBline
void MboyBlineStore::Add(MboyBline ToBeAdded){MboyBlineSet.push_back(ToBeAdded);}
 
// Nber of MboyBline
int MboyBlineStore::NberOfObjects() const{return MboyBlineSet.size();}
 
// Get a Pointer on a MboyBline of Nber MboyBlineNber
const MboyBline* MboyBlineStore::GetMboyBline(int MboyBlineNber) const{
 const MboyBline* ToBeReturned = 0 ;
 if (MboyBlineNber < 0 || MboyBlineNber > NberOfObjects()) return ToBeReturned;
 return &MboyBlineSet[MboyBlineNber];
}

void MboyBlineStore::SetActive(int Active_i){m_Active = Active_i;}
int  MboyBlineStore::GetActive(){return m_Active;}
      
// Sort the Store
void MboyBlineStore::Sort(){

  int TheSize = MboyBlineSet.size();
  if ( TheSize == 0 ) return ;
  
  std::vector<MboyBline>  MboyBlineSetTEMP ;
  MboyBlineSetTEMP.clear();
  
  for (int JTYP=1   ; JTYP <=60 ; JTYP++){
    for (int JFF =1   ; JFF  <=8  ; JFF++ ){
      for (int JZZ =-10 ; JZZ  <=10 ; JZZ++ ){
        int Index  =getData(JTYP,JFF,JZZ) ;
        if ( Index != -1 ){
          MboyBlineSetTEMP.push_back(MboyBlineSet[Index]) ;
        }
      }   
    }   
  }
  
  if (MboyBlineSet.size() !=  MboyBlineSetTEMP.size()){
    std::cout << " MboyBlineStore::Sort "
              << " Probleme MboyBlineSet.size() !=  MboyBlineSetTEMP.size() "
              << std::endl ;
  }
  
  MboyBlineSet.clear();
  TheSize = MboyBlineSetTEMP.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    MboyBlineSet.push_back(MboyBlineSetTEMP[Index]) ;
  }
     
}
 
int MboyBlineStore::getData(int JTYP,int JFF,int JZZ){
  int ToBeReturned = -1;
  
  int TheSize = MboyBlineSet.size();
  for (int Index= 0 ; Index <TheSize ; Index++){
    if (MboyBlineSet[Index].Getjtyp() == JTYP){
      if (MboyBlineSet[Index].Getjff()  == JFF){
        if (MboyBlineSet[Index].Getjzz()  == JZZ){
          ToBeReturned = Index ;
          break ;
        }
      }
    }
  }   
  return ToBeReturned ;
}
 
