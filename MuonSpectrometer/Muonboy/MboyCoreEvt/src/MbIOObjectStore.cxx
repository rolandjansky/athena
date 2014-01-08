/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIOObjectStore.h"

MbIOObjectStore::MbIOObjectStore(){

  m_RunNber   = 0 ;
  m_EventNber = 0 ;

  m_KounterMDT    = 0 ;
  m_KounterRPC    = 0 ;
  m_KounterTGC    = 0 ;
  m_KounterCSC    = 0 ;
  m_KounterCSCclu = 0 ;
  
  m_KounterMbMuSc    = 0 ;
  
  m_KounterHoleMDT    = 0 ;
  m_KounterHoleRPC    = 0 ;
  m_KounterHoleTGC    = 0 ;
  m_KounterHoleCSC    = 0 ;
  m_KounterHoleCSCclu = 0 ;
  
  m_KounterAll    = 0 ;
  
  m_HasBeenSorted = 0 ;

}

MbIOObjectStore::~MbIOObjectStore(){}

// Reset the Store
void MbIOObjectStore::ResetMbIOObjectStore(){

//
  MbROTMDTSet.clear();
  MbROTRPCSet.clear();
  MbROTTGCSet.clear();
  MbROTCSCSet.clear();
  MbROTCSCcluSet.clear();

  MbMuScSet.clear();

  MbHoleMDTSet.clear();
  MbHoleRPCSet.clear();
  MbHoleTGCSet.clear();
  MbHoleCSCSet.clear();
  MbHoleCSCcluSet.clear();


//
  LinearIndexMDTSet.clear();
  LinearIndexRPCSet.clear();
  LinearIndexTGCSet.clear();
  LinearIndexCSCSet.clear();
  LinearIndexCSCcluSet.clear();

  LinearIndexMbMuScSet.clear();

  LinearIndexHoleMDTSet.clear();
  LinearIndexHoleRPCSet.clear();
  LinearIndexHoleTGCSet.clear();
  LinearIndexHoleCSCSet.clear();
  LinearIndexHoleCSCcluSet.clear();


//
  LinearIndexToMDTSet.clear();
  LinearIndexToRPCSet.clear();
  LinearIndexToTGCSet.clear();
  LinearIndexToCSCSet.clear();
  LinearIndexToCSCcluSet.clear();

  LinearIndexToMbMuScSet.clear();

  LinearIndexToHoleMDTSet.clear();
  LinearIndexToHoleRPCSet.clear();
  LinearIndexToHoleTGCSet.clear();
  LinearIndexToHoleCSCSet.clear();
  LinearIndexToHoleCSCcluSet.clear();


  m_KounterMDT    = 0 ;
  m_KounterRPC    = 0 ;
  m_KounterTGC    = 0 ;
  m_KounterCSC    = 0 ;
  m_KounterCSCclu = 0 ;
  
  m_KounterMbMuSc    = 0 ;
  
  m_KounterHoleMDT    = 0 ;
  m_KounterHoleRPC    = 0 ;
  m_KounterHoleTGC    = 0 ;
  m_KounterHoleCSC    = 0 ;
  m_KounterHoleCSCclu = 0 ;
  
  m_KounterAll    = 0 ;

}

// Set
void MbIOObjectStore::SetRunNber(int RunNber){m_RunNber = RunNber;}
void MbIOObjectStore::SetEventNber(int EventNber){m_EventNber = EventNber;}

// Get
int MbIOObjectStore::GetRunNber() const{return m_RunNber;}
int MbIOObjectStore::GetEventNber() const{return m_EventNber;}

// Print Out the Store
void MbIOObjectStore::PrintSummary() const{

//*Print out Total Nbers of Digis
   std::cout << "*  For the Event of Nber:"
             << m_EventNber
	     << std::endl;
   int TotalNberOfDigiMDT          = GetNberOfDigiMDT();
   int TotalNberOfDigiRPC          = GetNberOfDigiRPC();
   int TotalNberOfDigiTGC          = GetNberOfDigiTGC();
   int TotalNberOfDigiCSC          = GetNberOfDigiCSC();
   int TotalNberOfDigiCSCclu       = GetNberOfDigiCSCclu();
   
   int TotalNberOfMbMuSc = GetNberOfMbMuSc();
   
   int TotalNberOfHoleMDT          = GetNberOfHoleMDT();
   int TotalNberOfHoleRPC          = GetNberOfHoleRPC();
   int TotalNberOfHoleTGC          = GetNberOfHoleTGC();
   int TotalNberOfHoleCSC          = GetNberOfHoleCSC();
   int TotalNberOfHoleCSCclu       = GetNberOfHoleCSCclu();
   
   int TotalNber =
   + TotalNberOfDigiMDT
   + TotalNberOfDigiRPC
   + TotalNberOfDigiTGC
   + TotalNberOfDigiCSC
   + TotalNberOfDigiCSCclu 
   + TotalNberOfMbMuSc 
   + TotalNberOfHoleMDT
   + TotalNberOfHoleRPC
   + TotalNberOfHoleTGC
   + TotalNberOfHoleCSC
   + TotalNberOfHoleCSCclu ;
   
   std::cout << "*  The Nbers of elements in the MbIOObjectStore are:  " << std::endl;
   std::cout << "*   All elements " <<std::setw(5) << TotalNber << std::endl;
   std::cout << "*   MDT Digis    " <<std::setw(5) << TotalNberOfDigiMDT          << std::endl;
   std::cout << "*   RPC Digis    " <<std::setw(5) << TotalNberOfDigiRPC          << std::endl;
   std::cout << "*   TGC Digis    " <<std::setw(5) << TotalNberOfDigiTGC          << std::endl;
   std::cout << "*   CSC Digis    " <<std::setw(5) << TotalNberOfDigiCSC          << std::endl;
   std::cout << "*   Cluster CSC  " <<std::setw(5) << TotalNberOfDigiCSCclu       << std::endl;
   
   std::cout << "*   Mu Sca       " <<std::setw(5) << TotalNberOfMbMuSc << std::endl;
   
   std::cout << "*   MDT Holes    " <<std::setw(5) << TotalNberOfHoleMDT          << std::endl;
   std::cout << "*   RPC Holes    " <<std::setw(5) << TotalNberOfHoleRPC          << std::endl;
   std::cout << "*   TGC Holes    " <<std::setw(5) << TotalNberOfHoleTGC          << std::endl;
   std::cout << "*   CSC Holes    " <<std::setw(5) << TotalNberOfHoleCSC          << std::endl;
   std::cout << "*   Hole Cl CSC  " <<std::setw(5) << TotalNberOfHoleCSCclu       << std::endl;

}
void MbIOObjectStore::PrintDigits(std::ostream* out) const{

   int TotalNberOfElements = GetNberOfElements();
   
//*Loop on MDT digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTMDT* pMbIdedObj = LiIdGetMbROTMDT(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on RPC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTRPC* pMbIdedObj = LiIdGetMbROTRPC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on TGC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTTGC* pMbIdedObj = LiIdGetMbROTTGC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTCSC* pMbIdedObj = LiIdGetMbROTCSC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSCclu digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTCSCclu* pMbIdedObj = LiIdGetMbROTCSCclu(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on MDT holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleMDT* pMbIdedObj = LiIdGetMbHoleMDT(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on RPC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleRPC* pMbIdedObj = LiIdGetMbHoleRPC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on TGC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleTGC* pMbIdedObj = LiIdGetMbHoleTGC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleCSC* pMbIdedObj = LiIdGetMbHoleCSC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSCclu holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleCSCclu* pMbIdedObj = LiIdGetMbHoleCSCclu(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAmdIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on Mu Sca
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbMuSc* pMbIdedObj = LiIdGetMbMuSc(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->Print(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

}
void MbIOObjectStore::PrintDigitsAthIds(std::ostream* out) const{

   int TotalNberOfElements = GetNberOfElements();
   
//*Loop on MDT digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTMDT* pMbIdedObj = LiIdGetMbROTMDT(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on RPC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTRPC* pMbIdedObj = LiIdGetMbROTRPC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on TGC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTTGC* pMbIdedObj = LiIdGetMbROTTGC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSC digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTCSC* pMbIdedObj = LiIdGetMbROTCSC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSCclu digis
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbROTCSCclu* pMbIdedObj = LiIdGetMbROTCSCclu(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on MDT holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleMDT* pMbIdedObj = LiIdGetMbHoleMDT(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on RPC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleRPC* pMbIdedObj = LiIdGetMbHoleRPC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on TGC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleTGC* pMbIdedObj = LiIdGetMbHoleTGC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSC holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleCSC* pMbIdedObj = LiIdGetMbHoleCSC(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on CSCclu holes
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbHoleCSCclu* pMbIdedObj = LiIdGetMbHoleCSCclu(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->PrintAthIds(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

//*Loop on Mu Sca
   for (int ItemNber=0; ItemNber<TotalNberOfElements ; ItemNber++){
     const MbMuSc* pMbIdedObj = LiIdGetMbMuSc(ItemNber);
     if (pMbIdedObj){ pMbIdedObj->Print(out); *out << std::setw(8)<< ItemNber << std::endl; }
   }

}

// Print sorted digits
void MbIOObjectStore::PrintSortedElements(std::ostream* out) const{

  int TotalNberOfElements=GetNberOfElements(); 
  int TotalNberOfElementsSorted = GetNberOfElsorted(); 
  *out
      << " TotalNberOfElements       " << std::setw(8) << TotalNberOfElements
      << " TotalNberOfElementsSorted " << std::setw(8) << TotalNberOfElementsSorted
      << std::endl;
  for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
    int LinearId = GetLinearNberSorted(iSorted);
    const MbPOT* pMbPOT = LiIdGetMbPOT    (LinearId);
    if (!pMbPOT) {       
      *out
         << " ARGGGH! pMbPOT = 0 for iSorted "
         << std::setw(5)<< iSorted  
         << std::endl;
    }else{
      double Xdca = pMbPOT->GetXdca() ;
      double Ydca = pMbPOT->GetYdca() ;
      double Zdca = pMbPOT->GetZdca() ;
      double DistanceToIP = sqrt ( 
                                  Xdca*Xdca 
                                  + Ydca*Ydca
                                  + Zdca*Zdca
                                  );
      size_t save_prec = out->precision();

      *out << std::setw(12)<<std::setprecision(4)<< DistanceToIP << std::endl ;

      out->precision(save_prec);
      
      *out 
         << std::setw(5)<< iSorted  ;
      pMbPOT->Print(out);
    }
  }
}

// Add typed element
void MbIOObjectStore::Add(const MbROTMDT& ToBeAdded){

  MbROTMDTSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToMDTSet.pop_back();
  LinearIndexToMDTSet.push_back(m_KounterMDT);
  LinearIndexMDTSet.push_back(m_KounterAll);

  m_KounterMDT = m_KounterMDT + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbROTRPC& ToBeAdded){

  MbROTRPCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToRPCSet.pop_back();
  LinearIndexToRPCSet.push_back(m_KounterRPC);
  LinearIndexRPCSet.push_back(m_KounterAll);

  m_KounterRPC = m_KounterRPC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbROTTGC& ToBeAdded){

  MbROTTGCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToTGCSet.pop_back();
  LinearIndexToTGCSet.push_back(m_KounterTGC);
  LinearIndexTGCSet.push_back(m_KounterAll);

  m_KounterTGC = m_KounterTGC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbROTCSC& ToBeAdded){

  MbROTCSCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToCSCSet.pop_back();
  LinearIndexToCSCSet.push_back(m_KounterCSC);
  LinearIndexCSCSet.push_back(m_KounterAll);

  m_KounterCSC = m_KounterCSC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbROTCSCclu& ToBeAdded){

  MbROTCSCcluSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToCSCcluSet.pop_back();
  LinearIndexToCSCcluSet.push_back(m_KounterCSCclu);
  LinearIndexCSCcluSet.push_back(m_KounterAll);

  m_KounterCSCclu = m_KounterCSCclu + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}




void MbIOObjectStore::Add(const MbMuSc& ToBeAdded){

  MbMuScSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToMbMuScSet.pop_back();
  LinearIndexToMbMuScSet.push_back(m_KounterMbMuSc);
  LinearIndexMbMuScSet.push_back(m_KounterAll);

  m_KounterMbMuSc = m_KounterMbMuSc + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}


void MbIOObjectStore::Add(const MbHoleMDT& ToBeAdded){

  MbHoleMDTSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToHoleMDTSet.pop_back();
  LinearIndexToHoleMDTSet.push_back(m_KounterHoleMDT);
  LinearIndexHoleMDTSet.push_back(m_KounterAll);

  m_KounterHoleMDT = m_KounterHoleMDT + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbHoleRPC& ToBeAdded){

  MbHoleRPCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToHoleRPCSet.pop_back();
  LinearIndexToHoleRPCSet.push_back(m_KounterHoleRPC);
  LinearIndexHoleRPCSet.push_back(m_KounterAll);

  m_KounterHoleRPC = m_KounterHoleRPC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbHoleTGC& ToBeAdded){

  MbHoleTGCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToHoleTGCSet.pop_back();
  LinearIndexToHoleTGCSet.push_back(m_KounterHoleTGC);
  LinearIndexHoleTGCSet.push_back(m_KounterAll);

  m_KounterHoleTGC = m_KounterHoleTGC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbHoleCSC& ToBeAdded){

  MbHoleCSCSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToHoleCSCSet.pop_back();
  LinearIndexToHoleCSCSet.push_back(m_KounterHoleCSC);
  LinearIndexHoleCSCSet.push_back(m_KounterAll);

  m_KounterHoleCSC = m_KounterHoleCSC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}
void MbIOObjectStore::Add(const MbHoleCSCclu& ToBeAdded){

  MbHoleCSCcluSet.push_back(ToBeAdded); 

  PushBackToSmthgSet();
  
  LinearIndexToHoleCSCcluSet.pop_back();
  LinearIndexToHoleCSCcluSet.push_back(m_KounterHoleCSCclu);
  LinearIndexHoleCSCcluSet.push_back(m_KounterAll);

  m_KounterHoleCSCclu = m_KounterHoleCSCclu + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}



// Nber of typed elements
int MbIOObjectStore::GetNberOfDigiMDT          () const{return m_KounterMDT              ;}
int MbIOObjectStore::GetNberOfDigiRPC          () const{return m_KounterRPC              ;}
int MbIOObjectStore::GetNberOfDigiTGC          () const{return m_KounterTGC              ;}
int MbIOObjectStore::GetNberOfDigiCSC          () const{return m_KounterCSC              ;}
int MbIOObjectStore::GetNberOfDigiCSCclu       () const{return m_KounterCSCclu           ;}

int MbIOObjectStore::GetNberOfMbMuSc () const{return m_KounterMbMuSc ;}

int MbIOObjectStore::GetNberOfHoleMDT          () const{return m_KounterHoleMDT          ;}
int MbIOObjectStore::GetNberOfHoleRPC          () const{return m_KounterHoleRPC          ;}
int MbIOObjectStore::GetNberOfHoleTGC          () const{return m_KounterHoleTGC          ;}
int MbIOObjectStore::GetNberOfHoleCSC          () const{return m_KounterHoleCSC          ;}
int MbIOObjectStore::GetNberOfHoleCSCclu       () const{return m_KounterHoleCSCclu       ;}

// Nber of elements
int MbIOObjectStore::GetNberOfElements() const{return m_KounterAll;}
int MbIOObjectStore::GetNberOfDigi()     const{
int TOBERETURNED = GetNberOfDigiMDT   ()
                 + GetNberOfDigiRPC   ()
                 + GetNberOfDigiTGC   ()
                 + GetNberOfDigiCSC   ()
                 + GetNberOfDigiCSCclu();
return TOBERETURNED;
}


// Get const pointer of a element
const MbIOObject* MbIOObjectStore::LiIdGetMbIOObject(int LinearNber) const{

  const MbPOT*    pMbPOT    = LiIdGetMbPOT    (LinearNber); if ( pMbPOT    ) return pMbPOT    ;

  return 0;

}
const MbPOT* MbIOObjectStore::LiIdGetMbPOT(int LinearNber) const{

  const MbROT*            pMbROT            = LiIdGetMbROT            (LinearNber); if ( pMbROT            ) return pMbROT            ;
  const MbHole*           pMbHole           = LiIdGetMbHole           (LinearNber); if ( pMbHole           ) return pMbHole           ;
  const MbMuSc*           pMbMuSc           = LiIdGetMbMuSc           (LinearNber); if ( pMbMuSc           ) return pMbMuSc ;

  return 0;

}
const MbIDTag* MbIOObjectStore::LiIdGetMbIDTag(int LinearNber) const{

  const MbIDTagMDT*    pMbIDTagMDT    = LiIdGetMbIDTagMDT    (LinearNber); if ( pMbIDTagMDT    ) return pMbIDTagMDT    ;
  const MbIDTagRPC*    pMbIDTagRPC    = LiIdGetMbIDTagRPC    (LinearNber); if ( pMbIDTagRPC    ) return pMbIDTagRPC    ;
  const MbIDTagTGC*    pMbIDTagTGC    = LiIdGetMbIDTagTGC    (LinearNber); if ( pMbIDTagTGC    ) return pMbIDTagTGC    ;
  const MbIDTagCSCclu* pMbIDTagCSCclu = LiIdGetMbIDTagCSCclu (LinearNber); if ( pMbIDTagCSCclu ) return pMbIDTagCSCclu ;
  const MbIDTagCSC*    pMbIDTagCSC    = LiIdGetMbIDTagCSC    (LinearNber); if ( pMbIDTagCSC    ) return pMbIDTagCSC    ;

  return 0;

}
const MbROT* MbIOObjectStore::LiIdGetMbROT(int LinearNber) const{

  const MbROTMDT*    pMbROTMDT    = LiIdGetMbROTMDT    (LinearNber); if ( pMbROTMDT    ) return pMbROTMDT    ;
  const MbROTRPC*    pMbROTRPC    = LiIdGetMbROTRPC    (LinearNber); if ( pMbROTRPC    ) return pMbROTRPC    ;
  const MbROTTGC*    pMbROTTGC    = LiIdGetMbROTTGC    (LinearNber); if ( pMbROTTGC    ) return pMbROTTGC    ;
  const MbROTCSCclu* pMbROTCSCclu = LiIdGetMbROTCSCclu (LinearNber); if ( pMbROTCSCclu ) return pMbROTCSCclu ;
  const MbROTCSC*    pMbROTCSC    = LiIdGetMbROTCSC    (LinearNber); if ( pMbROTCSC    ) return pMbROTCSC    ;
  
  return 0;

}
const MbHole* MbIOObjectStore::LiIdGetMbHole(int LinearNber) const{

  const MbHoleMDT*    pMbHoleMDT    = LiIdGetMbHoleMDT    (LinearNber); if ( pMbHoleMDT    ) return pMbHoleMDT    ;
  const MbHoleRPC*    pMbHoleRPC    = LiIdGetMbHoleRPC    (LinearNber); if ( pMbHoleRPC    ) return pMbHoleRPC    ;
  const MbHoleTGC*    pMbHoleTGC    = LiIdGetMbHoleTGC    (LinearNber); if ( pMbHoleTGC    ) return pMbHoleTGC    ;
  const MbHoleCSCclu* pMbHoleCSCclu = LiIdGetMbHoleCSCclu (LinearNber); if ( pMbHoleCSCclu ) return pMbHoleCSCclu ;
  const MbHoleCSC*    pMbHoleCSC    = LiIdGetMbHoleCSC    (LinearNber); if ( pMbHoleCSC    ) return pMbHoleCSC    ;
  
  return 0;

}


// Get const pointer of a typed element
const MbROTMDT*         MbIOObjectStore::LiIdGetMbROTMDT        (int LinearNber) const{ if (TyIdMDT(LinearNber)              >= 0) return &MbROTMDTSet          [TyIdMDT(LinearNber)             ] ; return 0; }
const MbROTRPC*         MbIOObjectStore::LiIdGetMbROTRPC        (int LinearNber) const{ if (TyIdRPC(LinearNber)              >= 0) return &MbROTRPCSet          [TyIdRPC(LinearNber)             ] ; return 0; }
const MbROTTGC*         MbIOObjectStore::LiIdGetMbROTTGC        (int LinearNber) const{ if (TyIdTGC(LinearNber)              >= 0) return &MbROTTGCSet          [TyIdTGC(LinearNber)             ] ; return 0; }
const MbROTCSC*         MbIOObjectStore::LiIdGetMbROTCSC        (int LinearNber) const{ if (TyIdCSC(LinearNber)              >= 0) return &MbROTCSCSet          [TyIdCSC(LinearNber)             ] ; return 0; }
const MbROTCSCclu*      MbIOObjectStore::LiIdGetMbROTCSCclu     (int LinearNber) const{ if (TyIdCSCclu(LinearNber)           >= 0) return &MbROTCSCcluSet       [TyIdCSCclu(LinearNber)          ] ; return 0; }

const MbMuSc*           MbIOObjectStore::LiIdGetMbMuSc          (int LinearNber) const{ if (TyIdMbMuSc(LinearNber)           >= 0) return &MbMuScSet            [TyIdMbMuSc(LinearNber)          ] ; return 0; }

const MbHoleMDT*        MbIOObjectStore::LiIdGetMbHoleMDT       (int LinearNber) const{ if (TyIdHoleMDT(LinearNber)          >= 0) return &MbHoleMDTSet         [TyIdHoleMDT(LinearNber)         ] ; return 0; }
const MbHoleRPC*        MbIOObjectStore::LiIdGetMbHoleRPC       (int LinearNber) const{ if (TyIdHoleRPC(LinearNber)          >= 0) return &MbHoleRPCSet         [TyIdHoleRPC(LinearNber)         ] ; return 0; }
const MbHoleTGC*        MbIOObjectStore::LiIdGetMbHoleTGC       (int LinearNber) const{ if (TyIdHoleTGC(LinearNber)          >= 0) return &MbHoleTGCSet         [TyIdHoleTGC(LinearNber)         ] ; return 0; }
const MbHoleCSC*        MbIOObjectStore::LiIdGetMbHoleCSC       (int LinearNber) const{ if (TyIdHoleCSC(LinearNber)          >= 0) return &MbHoleCSCSet         [TyIdHoleCSC(LinearNber)         ] ; return 0; }
const MbHoleCSCclu*     MbIOObjectStore::LiIdGetMbHoleCSCclu    (int LinearNber) const{ if (TyIdHoleCSCclu(LinearNber)       >= 0) return &MbHoleCSCcluSet      [TyIdHoleCSCclu(LinearNber)      ] ; return 0; }



const MbIDTagMDT*    MbIOObjectStore::LiIdGetMbIDTagMDT   (int LinearNber) const{ const MbIDTagMDT*    pMbIDTag = LiIdGetMbROTMDT   (LinearNber); if (pMbIDTag) return pMbIDTag; pMbIDTag = LiIdGetMbHoleMDT   (LinearNber); if (pMbIDTag) return pMbIDTag; return 0;}
const MbIDTagRPC*    MbIOObjectStore::LiIdGetMbIDTagRPC   (int LinearNber) const{ const MbIDTagRPC*    pMbIDTag = LiIdGetMbROTRPC   (LinearNber); if (pMbIDTag) return pMbIDTag; pMbIDTag = LiIdGetMbHoleRPC   (LinearNber); if (pMbIDTag) return pMbIDTag; return 0;}
const MbIDTagTGC*    MbIOObjectStore::LiIdGetMbIDTagTGC   (int LinearNber) const{ const MbIDTagTGC*    pMbIDTag = LiIdGetMbROTTGC   (LinearNber); if (pMbIDTag) return pMbIDTag; pMbIDTag = LiIdGetMbHoleTGC   (LinearNber); if (pMbIDTag) return pMbIDTag; return 0;}
const MbIDTagCSC*    MbIOObjectStore::LiIdGetMbIDTagCSC   (int LinearNber) const{ const MbIDTagCSC*    pMbIDTag = LiIdGetMbROTCSC   (LinearNber); if (pMbIDTag) return pMbIDTag; pMbIDTag = LiIdGetMbHoleCSC   (LinearNber); if (pMbIDTag) return pMbIDTag; return 0;}
const MbIDTagCSCclu* MbIOObjectStore::LiIdGetMbIDTagCSCclu(int LinearNber) const{ const MbIDTagCSCclu* pMbIDTag = LiIdGetMbROTCSCclu(LinearNber); if (pMbIDTag) return pMbIDTag; pMbIDTag = LiIdGetMbHoleCSCclu(LinearNber); if (pMbIDTag) return pMbIDTag; return 0;}


// Get Typed Digi Nber from a Linear Digi Nber
int MbIOObjectStore::TyIdMDT             (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToMDTSet             [LinearNber] >= 0) return LinearIndexToMDTSet             [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdRPC             (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToRPCSet             [LinearNber] >= 0) return LinearIndexToRPCSet             [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdTGC             (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToTGCSet             [LinearNber] >= 0) return LinearIndexToTGCSet             [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdCSC             (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToCSCSet             [LinearNber] >= 0) return LinearIndexToCSCSet             [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdCSCclu          (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToCSCcluSet          [LinearNber] >= 0) return LinearIndexToCSCcluSet          [LinearNber] ; return -1;}

int MbIOObjectStore::TyIdMbMuSc          (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToMbMuScSet          [LinearNber] >= 0) return LinearIndexToMbMuScSet          [LinearNber] ; return -1;}

int MbIOObjectStore::TyIdHoleMDT         (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToHoleMDTSet         [LinearNber] >= 0) return LinearIndexToHoleMDTSet         [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdHoleRPC         (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToHoleRPCSet         [LinearNber] >= 0) return LinearIndexToHoleRPCSet         [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdHoleTGC         (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToHoleTGCSet         [LinearNber] >= 0) return LinearIndexToHoleTGCSet         [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdHoleCSC         (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToHoleCSCSet         [LinearNber] >= 0) return LinearIndexToHoleCSCSet         [LinearNber] ; return -1;}
int MbIOObjectStore::TyIdHoleCSCclu      (int LinearNber) const{ if (IsValidLinearNber(LinearNber) < 0 ) return -1; if (LinearIndexToHoleCSCcluSet      [LinearNber] >= 0) return LinearIndexToHoleCSCcluSet      [LinearNber] ; return -1;}

int MbIOObjectStore::IsValidLinearNber(int LinearNber) const{ if (LinearNber < 0 || LinearNber >= GetNberOfElements()) return -1;  return 1;}



//Sort elements
int MbIOObjectStore::GetNberOfElsorted() const{ return m_LinearIndexSorted.size() ; }
int MbIOObjectStore::GetLinearNberSorted(int IdSorted) const{
 if (IdSorted < 0 || IdSorted >= GetNberOfElsorted()) return -1;
 if ( m_HasBeenSorted == 0 ) return IdSorted ;
 return m_LinearIndexSorted[IdSorted] ;
}
void MbIOObjectStore::SortElements(
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart
){
  m_LinearIndexSorted.clear();
  m_HasBeenSorted = 1 ;
  
  int    iNew = 1 ;
  int    IndexKeep = 0 ;
  double XrefStartNew = 0. ;
  double YrefStartNew = 0. ;
  double ZrefStartNew = 0. ;
  double DistanceToRefMAX =-99999999. ; 
  
  int TotalNberOfElements=GetNberOfElements(); 

  std::vector<double> PointXdca ;
  std::vector<double> PointYdca ;
  std::vector<double> PointZdca ;
  std::vector<int>    PointInde ;
  
  for (int Index=0; Index<TotalNberOfElements; Index++){

    const MbPOT* pMbPOT = LiIdGetMbPOT    (Index);

    if (pMbPOT) {
      double Xdca = 0. ;
      double Ydca = 0. ;
      double Zdca = 0. ;
      Xdca = pMbPOT->GetXdca() ;
      Ydca = pMbPOT->GetYdca() ;
      Zdca = pMbPOT->GetZdca() ;
      PointXdca.push_back(Xdca);
      PointYdca.push_back(Ydca);
      PointZdca.push_back(Zdca);
      PointInde.push_back(Index);
    }else{
      std::cout << "ARGGGH from MbIOObjectStore::SortElements :  pMbPOT null  " << std::endl;
    }
  }
    
  int TotalNberOfElementsTOBESORTED = PointXdca.size();
  for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
    double Xdca = PointXdca[Index] ;
    double Ydca = PointYdca[Index] ;
    double Zdca = PointZdca[Index] ;

    double DistanceToZero= sqrt ( 
                                   (XrefStart-Xdca)*(XrefStart-Xdca)
                                 + (YrefStart-Ydca)*(YrefStart-Ydca)
                                 + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                );
    if ( DistanceToZero > DistanceToRefMAX )DistanceToRefMAX = DistanceToZero;
    
    int Index2Start = Index + 1 ;
    for (int Index2=Index2Start; Index2<TotalNberOfElementsTOBESORTED; Index2++){
      double Xdca2 = PointXdca[Index2] ;
      double Ydca2 = PointYdca[Index2] ;
      double Zdca2 = PointZdca[Index2] ;
      
      double DistanceInter= sqrt ( 
                                    (Xdca2-Xdca)*(Xdca2-Xdca)
                                  + (Ydca2-Ydca)*(Ydca2-Ydca)
                                  + (Zdca2-Zdca)*(Zdca2-Zdca)
                                  );
      if ( DistanceInter > DistanceToRefMAX )DistanceToRefMAX = DistanceInter;
      
    }
    
  }
    
  DistanceToRefMAX = 2. * DistanceToRefMAX ;
    
  double DistanceToRefmin = DistanceToRefMAX ; 
  
  while ( iNew == 1 ){
    iNew = 0 ; 
    for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
      int IndexTrue = PointInde[Index];
      int IsExcluded = 0 ;
      int TotalNberOfElementsSorted = GetNberOfElsorted() ;
      for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
        if ( m_LinearIndexSorted[iSorted] == IndexTrue ) IsExcluded = 1 ;
        if ( m_LinearIndexSorted[iSorted] == IndexTrue ) break ;
      }
      if ( IsExcluded == 0 ){
        iNew = 1 ;
        
        double Xdca = PointXdca[Index] ;
        double Ydca = PointYdca[Index] ;
        double Zdca = PointZdca[Index] ;
        double DistanceToRef = sqrt ( 
                                      (XrefStart-Xdca)*(XrefStart-Xdca)
                                    + (YrefStart-Ydca)*(YrefStart-Ydca)
                                    + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                    );
        if ( DistanceToRef < DistanceToRefmin ){
          DistanceToRefmin = DistanceToRef ;
          IndexKeep = IndexTrue ;
          XrefStartNew = Xdca ;
          YrefStartNew = Ydca ;
          ZrefStartNew = Zdca ;
        }
      }
    }
    if ( iNew == 1 ){
      m_LinearIndexSorted.push_back(IndexKeep) ;
      XrefStart = XrefStartNew ;
      YrefStart = YrefStartNew ;
      ZrefStart = ZrefStartNew ;
      DistanceToRefmin = DistanceToRefMAX ; 
    }
  }
  
}


void MbIOObjectStore::PushBackToSmthgSet(){
  int DummyEntry = - 1 ;

  LinearIndexToMDTSet.push_back    (DummyEntry);
  LinearIndexToRPCSet.push_back    (DummyEntry);
  LinearIndexToTGCSet.push_back    (DummyEntry);
  LinearIndexToCSCSet.push_back    (DummyEntry);
  LinearIndexToCSCcluSet.push_back (DummyEntry);

  LinearIndexToMbMuScSet.push_back    (DummyEntry);

  LinearIndexToHoleMDTSet.push_back    (DummyEntry);
  LinearIndexToHoleRPCSet.push_back    (DummyEntry);
  LinearIndexToHoleTGCSet.push_back    (DummyEntry);
  LinearIndexToHoleCSCSet.push_back    (DummyEntry);
  LinearIndexToHoleCSCcluSet.push_back (DummyEntry);
}
