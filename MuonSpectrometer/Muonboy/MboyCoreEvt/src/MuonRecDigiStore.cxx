/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigiStore.h"

using namespace std;

MuonRecDigiStore::MuonRecDigiStore(){

  m_RunNber   = 0 ;
  m_EventNber = 0 ;
  
  m_KounterMDT    = 0 ;
  m_KounterRPC    = 0 ;
  m_KounterTGC    = 0 ;
  m_KounterCSC    = 0 ;
  m_KounterCSCClu = 0 ;
  
  m_KounterAll    = 0 ;

}

MuonRecDigiStore::~MuonRecDigiStore(){}

// Reset the Store
void MuonRecDigiStore::ResetMuonRecDigiStore(){

  MuonRecDigitMDTSet.clear();
  MuonRecDigitRPCSet.clear();
  MuonRecDigitTGCSet.clear();
  MuonRecDigitCSCSet.clear();
  MuonRecDigitCSCCluSet.clear();

  LinearIndexMDTSet.clear();
  LinearIndexRPCSet.clear();
  LinearIndexTGCSet.clear();
  LinearIndexCSCSet.clear();
  LinearIndexCSCCluSet.clear();

  LinearIndexToMDTSet.clear();
  LinearIndexToRPCSet.clear();
  LinearIndexToTGCSet.clear();
  LinearIndexToCSCSet.clear();
  LinearIndexToCSCCluSet.clear();

  IdentifierToMDTSet.clear();
  IdentifierToRPCSet.clear();
  IdentifierToTGCSet.clear();
  IdentifierToCSCSet.clear();
  IdentifierToCSCCluSet.clear();

  m_KounterMDT    = 0 ;
  m_KounterRPC    = 0 ;
  m_KounterTGC    = 0 ;
  m_KounterCSC    = 0 ;
  m_KounterCSCClu    = 0 ;
  m_KounterAll    = 0 ;

}

// Set
void MuonRecDigiStore::SetRunNber(int RunNber){m_RunNber = RunNber;}
void MuonRecDigiStore::SetEventNber(int EventNber){m_EventNber = EventNber;}

// Get
int MuonRecDigiStore::GetRunNber() const{return m_RunNber;}
int MuonRecDigiStore::GetEventNber() const{return m_EventNber;}

// Print Out the Store
void MuonRecDigiStore::PrintSummary() const{

//*Print out Total Nbers of Digis
   std::cout << "*  For the Event of Nber:"
             << m_EventNber
	     << std::endl;
   int TotalNberOfDigiMDT = NberOfDigiMDT();
   int TotalNberOfDigiRPC = NberOfDigiRPC();
   int TotalNberOfDigiTGC = NberOfDigiTGC();
   int TotalNberOfDigiCSC = NberOfDigiCSC();
   int TotalNberOfDigiCSCClu = NberOfDigiCSCClu();
   int TotalNber =
   + TotalNberOfDigiMDT
   + TotalNberOfDigiRPC
   + TotalNberOfDigiTGC
   + TotalNberOfDigiCSC
   + TotalNberOfDigiCSCClu;
   std::cout << "*  The Nbers of Digis in the MuonRecDigiStore are:  " << std::endl;
   std::cout << "*   All Digis                 " <<std::setw(5) << TotalNber << std::endl;
   std::cout << "*   MDT Digis                 " <<std::setw(5) << TotalNberOfDigiMDT << std::endl;
   std::cout << "*   RPC Digis                 " <<std::setw(5) << TotalNberOfDigiRPC << std::endl;
   std::cout << "*   TGC Digis                 " <<std::setw(5) << TotalNberOfDigiTGC << std::endl;
   std::cout << "*   CSC Digis                 " <<std::setw(5) << TotalNberOfDigiCSC << std::endl;
   std::cout << "*   CSC Clusters              " <<std::setw(5) << TotalNberOfDigiCSCClu << std::endl;

}
void MuonRecDigiStore::PrintDigits(std::ostream* out) const{

   int TotalNberOfDigi = NberOfDigi();

//*Loop on MDT digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitMDT* pMuonRecDigitMDT =  LiIdGetMuonRecDigitMDT(ItemNber);
     if (pMuonRecDigitMDT){
       pMuonRecDigitMDT->PrintAmdIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << " " << std::setw(8)<< pMuonRecDigitMDT->GetStatus()
                    << std::endl;
     }
   }

//*Loop on RPC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitRPC* pMuonRecDigitRPC =  LiIdGetMuonRecDigitRPC(ItemNber);
     if (pMuonRecDigitRPC){
       pMuonRecDigitRPC->PrintAmdIds(out);

       size_t save_prec = out->precision();

       *out
                    << std::setw(8)<< ItemNber
		    << " " <<std::setw(10)<<std::setprecision(3)<< pMuonRecDigitRPC->GetRpcTime()
                    << std::endl;

       out->precision(save_prec);
     }
   }

//*Loop on TGC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitTGC* pMuonRecDigitTGC =  LiIdGetMuonRecDigitTGC(ItemNber);
     if (pMuonRecDigitTGC){
       pMuonRecDigitTGC->PrintAmdIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << std::endl;
     }
   }

//*Loop on CSC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitCSC* pMuonRecDigitCSC =  LiIdGetMuonRecDigitCSC(ItemNber);
     if (pMuonRecDigitCSC){
       pMuonRecDigitCSC->PrintAmdIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << std::endl;
     }
   }


//*Loop on CSC clusters
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitCSCClu* pMuonRecDigitCSCClu =  LiIdGetMuonRecDigitCSCClu(ItemNber);
     if (pMuonRecDigitCSCClu){
       pMuonRecDigitCSCClu->PrintAmdIds(out);
       size_t save_prec = out->precision();

       *out
                    << std::setw(8)<< ItemNber
		    << " " << std::setw(14)<<std::setprecision(3)<< pMuonRecDigitCSCClu->GetTime()
		    << " " << std::setw(14)<<std::setprecision(3)<< pMuonRecDigitCSCClu->GetDepositedCharge()
                    << " " << std::setw(8)<< pMuonRecDigitCSCClu->GetStatus()
                    << " " << std::setw(8)<< pMuonRecDigitCSCClu->GetTimeStatus()
                    << std::endl;
 
       out->precision(save_prec);
    }
   }

}
void MuonRecDigiStore::PrintDigitsAthIds(std::ostream* out) const{

   int TotalNberOfDigi = NberOfDigi();

//*Loop on MDT digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitMDT* pMuonRecDigitMDT =  LiIdGetMuonRecDigitMDT(ItemNber);
     if (pMuonRecDigitMDT){
       pMuonRecDigitMDT->PrintAthIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << " " << std::setw(8)<< pMuonRecDigitMDT->GetStatus()
                    << std::endl;
     }
   }

//*Loop on RPC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitRPC* pMuonRecDigitRPC =  LiIdGetMuonRecDigitRPC(ItemNber);
     if (pMuonRecDigitRPC){
       pMuonRecDigitRPC->PrintAthIds(out);
       size_t save_prec = out->precision();

       *out
                    << std::setw(8)<< ItemNber
		    << " " <<std::setw(10)<<std::setprecision(3)<< pMuonRecDigitRPC->GetRpcTime()
                    << std::endl;

       out->precision(save_prec);
     }
   }

//*Loop on TGC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitTGC* pMuonRecDigitTGC =  LiIdGetMuonRecDigitTGC(ItemNber);
     if (pMuonRecDigitTGC){
       pMuonRecDigitTGC->PrintAthIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << std::endl;
     }
   }

//*Loop on CSC digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitCSC* pMuonRecDigitCSC =  LiIdGetMuonRecDigitCSC(ItemNber);
     if (pMuonRecDigitCSC){
       pMuonRecDigitCSC->PrintAthIds(out);
       *out
                    << std::setw(8)<< ItemNber
                    << std::endl;
     }
   }

//*Loop on CSC Clusters
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitCSCClu* pMuonRecDigitCSCClu =  LiIdGetMuonRecDigitCSCClu(ItemNber);
     if (pMuonRecDigitCSCClu){
       pMuonRecDigitCSCClu->PrintAthIds(out);
       size_t save_prec = out->precision();

       *out
                    << std::setw(8)<< ItemNber
		    << " " << std::setw(14)<<std::setprecision(3)<< pMuonRecDigitCSCClu->GetTime()
		    << " " << std::setw(14)<<std::setprecision(3)<< pMuonRecDigitCSCClu->GetDepositedCharge()
                    << " " << std::setw(8)<< pMuonRecDigitCSCClu->GetStatus()
                    << " " << std::setw(8)<< pMuonRecDigitCSCClu->GetTimeStatus()
                    << std::endl;

       out->precision(save_prec);
     }
   }

}

// Add a Typed Digi
void MuonRecDigiStore::Add(MuonRecDigitMDT& ToBeAdded){

  MuonRecDigitMDTSet.push_back(ToBeAdded); 

  int identifier = ToBeAdded.GetIdentifier ();
  if (identifier != -1) IdentifierToMDTSet[identifier] = m_KounterMDT;

  PushBackToSmthgSet();
  
  LinearIndexToMDTSet.pop_back();
  LinearIndexToMDTSet.push_back(m_KounterMDT);
  LinearIndexMDTSet.push_back(m_KounterAll);

  m_KounterMDT = m_KounterMDT + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}

void MuonRecDigiStore::Add(MuonRecDigitRPC& ToBeAdded){

  MuonRecDigitRPCSet.push_back(ToBeAdded); 

  int identifier = ToBeAdded.GetIdentifier ();
  if (identifier != -1) IdentifierToRPCSet[identifier] = m_KounterRPC;

  PushBackToSmthgSet();
  
  LinearIndexToRPCSet.pop_back();
  LinearIndexToRPCSet.push_back(m_KounterRPC);
  LinearIndexRPCSet.push_back(m_KounterAll);

  m_KounterRPC = m_KounterRPC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}

void MuonRecDigiStore::Add(MuonRecDigitTGC& ToBeAdded){

  MuonRecDigitTGCSet.push_back(ToBeAdded); 

  int identifier = ToBeAdded.GetIdentifier ();
  if (identifier != -1) IdentifierToTGCSet[identifier] = m_KounterTGC;

  PushBackToSmthgSet();
  
  LinearIndexToTGCSet.pop_back();
  LinearIndexToTGCSet.push_back(m_KounterTGC);
  LinearIndexTGCSet.push_back(m_KounterAll);

  m_KounterTGC = m_KounterTGC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}

void MuonRecDigiStore::Add(MuonRecDigitCSC& ToBeAdded){

  MuonRecDigitCSCSet.push_back(ToBeAdded); 

  int identifier = ToBeAdded.GetIdentifier ();
  if (identifier != -1) IdentifierToCSCSet[identifier] = m_KounterCSC;

  PushBackToSmthgSet();
  
  LinearIndexToCSCSet.pop_back();
  LinearIndexToCSCSet.push_back(m_KounterCSC);
  LinearIndexCSCSet.push_back(m_KounterAll);

  m_KounterCSC = m_KounterCSC + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}

void MuonRecDigiStore::Add(MuonRecDigitCSCClu& ToBeAdded){

  MuonRecDigitCSCCluSet.push_back(ToBeAdded); 

  int identifier = ToBeAdded.GetIdentifier ();
  if (identifier != -1) IdentifierToCSCCluSet[identifier] = m_KounterCSCClu;

  PushBackToSmthgSet();
  
  LinearIndexToCSCCluSet.pop_back();
  LinearIndexToCSCCluSet.push_back(m_KounterCSCClu);
  LinearIndexCSCCluSet.push_back(m_KounterAll);

  m_KounterCSCClu = m_KounterCSCClu + 1 ;
  m_KounterAll = m_KounterAll + 1 ;

}

// Nber of Typed Digis
int MuonRecDigiStore::NberOfDigiMDT() const{return m_KounterMDT;}
int MuonRecDigiStore::NberOfDigiRPC() const{return m_KounterRPC;}
int MuonRecDigiStore::NberOfDigiTGC() const{return m_KounterTGC;}
int MuonRecDigiStore::NberOfDigiCSC() const{return m_KounterCSC;}
int MuonRecDigiStore::NberOfDigiCSCClu() const{return m_KounterCSCClu;}
// Nber of Digis
int MuonRecDigiStore::NberOfDigi() const{return m_KounterAll;}


// Get a Pointer on a  Digi of Linear Digi Nber
const MuonRecDigitMDT* MuonRecDigiStore::LiIdGetMuonRecDigitMDT(int LinearNber) const{
 if (TyIdMDT(LinearNber)    >= 0) return &MuonRecDigitMDTSet   [TyIdMDT(LinearNber)   ] ;
 return 0;
}
const MuonRecDigitRPC* MuonRecDigiStore::LiIdGetMuonRecDigitRPC(int LinearNber) const{
 if (TyIdRPC(LinearNber)    >= 0) return &MuonRecDigitRPCSet   [TyIdRPC(LinearNber)   ] ;
 return 0;
}
const MuonRecDigitTGC* MuonRecDigiStore::LiIdGetMuonRecDigitTGC(int LinearNber) const{
 if (TyIdTGC(LinearNber)    >= 0) return &MuonRecDigitTGCSet   [TyIdTGC(LinearNber)   ] ;
 return 0;
}
const MuonRecDigitCSC* MuonRecDigiStore::LiIdGetMuonRecDigitCSC(int LinearNber) const{
 if (TyIdCSC(LinearNber) >= 0) return &MuonRecDigitCSCSet[TyIdCSC(LinearNber)] ;
 return 0;
}
const MuonRecDigitCSCClu* MuonRecDigiStore::LiIdGetMuonRecDigitCSCClu(int LinearNber) const{
 if (TyIdCSCClu(LinearNber) >= 0) return &MuonRecDigitCSCCluSet[TyIdCSCClu(LinearNber)] ;
 return 0;
}

// Get a Pointer on a  Digi of Linear Digi Nber
MuonRecDigitMDT* MuonRecDigiStore::NonCte_LiIdGetMuonRecDigitMDT(int LinearNber) {
 if (TyIdMDT(LinearNber)    >= 0) return &MuonRecDigitMDTSet   [TyIdMDT(LinearNber)   ] ;
 return 0;
}
MuonRecDigitRPC* MuonRecDigiStore::NonCte_LiIdGetMuonRecDigitRPC(int LinearNber) {
 if (TyIdRPC(LinearNber)    >= 0) return &MuonRecDigitRPCSet   [TyIdRPC(LinearNber)   ] ;
 return 0;
}
MuonRecDigitTGC* MuonRecDigiStore::NonCte_LiIdGetMuonRecDigitTGC(int LinearNber) {
 if (TyIdTGC(LinearNber)    >= 0) return &MuonRecDigitTGCSet   [TyIdTGC(LinearNber)   ] ;
 return 0;
}
MuonRecDigitCSC* MuonRecDigiStore::NonCte_LiIdGetMuonRecDigitCSC(int LinearNber) {
 if (TyIdCSC(LinearNber) >= 0) return &MuonRecDigitCSCSet[TyIdCSC(LinearNber)] ;
 return 0;
}
MuonRecDigitCSCClu* MuonRecDigiStore::NonCte_LiIdGetMuonRecDigitCSCClu(int LinearNber) {
 if (TyIdCSCClu(LinearNber) >= 0) return &MuonRecDigitCSCCluSet[TyIdCSCClu(LinearNber)] ;
 return 0;
}

void MuonRecDigiStore::PrintChecks() const{

  int DigiMDTNber = 0 ;
  int DigiRPCNber = 0 ;
  int DigiTGCNber = 0 ;
  int DigiCSCNber = 0 ;
  int DigiCSCCluNber = 0 ;

//*Loop on MDT digis
   std::cout << "* MDT " << std::endl;
   int TotalNberOfDigiMDT = NberOfDigiMDT();
   for (int ItemNber=0; ItemNber<TotalNberOfDigiMDT ; ItemNber++){
     DigiMDTNber = TyIdMDT( LinearIndexMDTSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << "  Linear   " << LinearIndexMDTSet[ItemNber]
               << "    " << ItemNber - DigiMDTNber
	       << std::endl;
   }

//*Loop on RPC digis
   std::cout << "* RPC " << std::endl;
   int TotalNberOfDigiRPC = NberOfDigiRPC();
   for (int ItemNber=0; ItemNber<TotalNberOfDigiRPC ; ItemNber++){
     DigiRPCNber = TyIdRPC( LinearIndexRPCSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << "  Linear   " << LinearIndexRPCSet[ItemNber]
               << "    " << ItemNber - DigiRPCNber
	       << std::endl;
   }

//*Loop on TGC digis
   std::cout << "* TGC " << std::endl;
   int TotalNberOfDigiTGC = NberOfDigiTGC();
   for (int ItemNber=0; ItemNber<TotalNberOfDigiTGC ; ItemNber++){
     DigiTGCNber = TyIdTGC( LinearIndexTGCSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << "  Linear   " << LinearIndexTGCSet[ItemNber]
               << "    " << ItemNber - DigiTGCNber
	       << std::endl;
   }

//*Loop on CSC digis
   std::cout << "* CSC " << std::endl;
   int TotalNberOfDigiCSC = NberOfDigiCSC();
   for (int ItemNber=0; ItemNber<TotalNberOfDigiCSC ; ItemNber++){
     DigiCSCNber = TyIdCSC( LinearIndexCSCSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << "  Linear   " << LinearIndexCSCSet[ItemNber]
               << "    " << ItemNber - DigiCSCNber
	       << std::endl;
   }

//*Loop on CSC Clusters
   std::cout << "* CLU " << std::endl;
   int TotalNberOfDigiCSCClu = NberOfDigiCSCClu();
   for (int ItemNber=0; ItemNber<TotalNberOfDigiCSCClu ; ItemNber++){
     DigiCSCCluNber = TyIdCSCClu( LinearIndexCSCCluSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << "  Linear   " << LinearIndexCSCCluSet[ItemNber]
               << "    " << ItemNber - DigiCSCCluNber
	       << std::endl;
   }

//*Loop on digis
   std::cout << "* ALL " << std::endl;
   int TotalNberOfDigi = NberOfDigi();
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     DigiMDTNber    = TyIdMDT( LinearIndexMDTSet[ItemNber] );
     DigiRPCNber    = TyIdRPC( LinearIndexRPCSet[ItemNber] );
     DigiTGCNber    = TyIdTGC( LinearIndexTGCSet[ItemNber] );
     DigiCSCNber    = TyIdCSC( LinearIndexCSCSet[ItemNber] );
     DigiCSCCluNber    = TyIdCSCClu( LinearIndexCSCCluSet[ItemNber] );
     std::cout << "* ItemNber " << ItemNber
               << " " << DigiMDTNber    
               << " " << DigiRPCNber    
               << " " << DigiTGCNber    
               << " " << DigiCSCNber 
               << " " << DigiCSCCluNber 
	       << std::endl;
   }

}

void MuonRecDigiStore::PrintDigitsSums(std::ostream* out) const{

  int TotalNberOfDigiMDT = NberOfDigiMDT();
  int TotalNberOfDigiRPC = NberOfDigiRPC();
  int TotalNberOfDigiTGC = NberOfDigiTGC();
  int TotalNberOfDigiCSC = NberOfDigiCSC();
  int TotalNberOfDigiCSCClu = NberOfDigiCSCClu();
  *out  <<"HIT "
  	<<TotalNberOfDigiMDT << " "
  	<<TotalNberOfDigiRPC << " "
  	<<TotalNberOfDigiTGC << " "
  	<<TotalNberOfDigiCSC << " "
  	<<TotalNberOfDigiCSCClu << " "
  	<<std::endl;

}

void MuonRecDigiStore::PrintDigitsRsLine(std::ostream* out) const{

   int TotalNberOfDigi = NberOfDigi();

//*Loop on MDT digis
   for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){
     const MuonRecDigitMDT* pMuonRecDigitMDT =  LiIdGetMuonRecDigitMDT(ItemNber);
     if (pMuonRecDigitMDT){
     std::string RsLine = pMuonRecDigitMDT->GetRsLine();
       *out
                    << RsLine
                    << std::endl;
     }
   }

}

// Reserve 
void MuonRecDigiStore::ReserveMuonRecDigiStore(
 const int& DoReserve,
 const int& SpaceForMDT   ,
 const int& SpaceForRPC   ,
 const int& SpaceForTGC   ,
 const int& SpaceForCSC   ,
 const int& SpaceForCSCClu
 ){

  int SpaceAll =
             + SpaceForMDT   
             + SpaceForRPC   
             + SpaceForTGC   
             + SpaceForCSC
             + SpaceForCSCClu;

  int CapacityAnteMuonRecDigitMDTSet     = MuonRecDigitMDTSet.capacity();
  int CapacityAnteMuonRecDigitRPCSet     = MuonRecDigitRPCSet.capacity();
  int CapacityAnteMuonRecDigitTGCSet     = MuonRecDigitTGCSet.capacity();
  int CapacityAnteMuonRecDigitCSCSet     = MuonRecDigitCSCSet.capacity();
  int CapacityAnteMuonRecDigitCSCCluSet     = MuonRecDigitCSCCluSet.capacity();
  int CapacityAnteLinearIndexMDTSet      = LinearIndexMDTSet.capacity();
  int CapacityAnteLinearIndexRPCSet      = LinearIndexRPCSet.capacity();
  int CapacityAnteLinearIndexTGCSet      = LinearIndexTGCSet.capacity();
  int CapacityAnteLinearIndexCSCSet      = LinearIndexCSCSet.capacity();
  int CapacityAnteLinearIndexCSCCluSet      = LinearIndexCSCCluSet.capacity();
  int CapacityAnteLinearIndexToMDTSet    = LinearIndexToMDTSet.capacity();
  int CapacityAnteLinearIndexToRPCSet    = LinearIndexToRPCSet.capacity();
  int CapacityAnteLinearIndexToTGCSet    = LinearIndexToTGCSet.capacity();
  int CapacityAnteLinearIndexToCSCSet    = LinearIndexToCSCSet.capacity();
  int CapacityAnteLinearIndexToCSCCluSet    = LinearIndexToCSCCluSet.capacity();

  MuonRecDigitMDTSet.reserve(SpaceForMDT);
  MuonRecDigitRPCSet.reserve(SpaceForRPC);
  MuonRecDigitTGCSet.reserve(SpaceForTGC);
  MuonRecDigitCSCSet.reserve(SpaceForCSC);
  MuonRecDigitCSCCluSet.reserve(SpaceForCSCClu);

  LinearIndexMDTSet.reserve(SpaceForMDT);
  LinearIndexRPCSet.reserve(SpaceForRPC);
  LinearIndexTGCSet.reserve(SpaceForTGC);
  LinearIndexCSCSet.reserve(SpaceForCSC);
  LinearIndexCSCCluSet.reserve(SpaceForCSCClu);

  LinearIndexToMDTSet.reserve(SpaceAll);
  LinearIndexToRPCSet.reserve(SpaceAll);
  LinearIndexToTGCSet.reserve(SpaceAll);
  LinearIndexToCSCSet.reserve(SpaceAll);
  LinearIndexToCSCCluSet.reserve(SpaceAll);

  int CapacityPostMuonRecDigitMDTSet     = MuonRecDigitMDTSet.capacity();
  int CapacityPostMuonRecDigitRPCSet     = MuonRecDigitRPCSet.capacity();
  int CapacityPostMuonRecDigitTGCSet     = MuonRecDigitTGCSet.capacity();
  int CapacityPostMuonRecDigitCSCSet     = MuonRecDigitCSCSet.capacity();
  int CapacityPostMuonRecDigitCSCCluSet     = MuonRecDigitCSCCluSet.capacity();
  int CapacityPostLinearIndexMDTSet      = LinearIndexMDTSet.capacity();
  int CapacityPostLinearIndexRPCSet      = LinearIndexRPCSet.capacity();
  int CapacityPostLinearIndexTGCSet      = LinearIndexTGCSet.capacity();
  int CapacityPostLinearIndexCSCSet      = LinearIndexCSCSet.capacity();
  int CapacityPostLinearIndexCSCCluSet      = LinearIndexCSCCluSet.capacity();
  int CapacityPostLinearIndexToMDTSet    = LinearIndexToMDTSet.capacity();
  int CapacityPostLinearIndexToRPCSet    = LinearIndexToRPCSet.capacity();
  int CapacityPostLinearIndexToTGCSet    = LinearIndexToTGCSet.capacity();
  int CapacityPostLinearIndexToCSCSet    = LinearIndexToCSCSet.capacity();
  int CapacityPostLinearIndexToCSCCluSet    = LinearIndexToCSCCluSet.capacity();

 if ( DoReserve == 2 ){
  std::cout << " SpaceForMDT    " << SpaceForMDT    << std::endl;
  std::cout << " SpaceForRPC    " << SpaceForRPC    << std::endl;
  std::cout << " SpaceForTGC    " << SpaceForTGC    << std::endl;
  std::cout << " SpaceForCSC    " << SpaceForCSC    << std::endl;
  std::cout << " SpaceForCSCClu    " << SpaceForCSCClu    << std::endl;
  std::cout << " SpaceAll       " << SpaceAll       << std::endl;
  std::cout << " MuonRecDigitMDTSet      " << CapacityAnteMuonRecDigitMDTSet     << "  " << CapacityPostMuonRecDigitMDTSet     << std::endl;
  std::cout << " MuonRecDigitRPCSet      " << CapacityAnteMuonRecDigitRPCSet     << "  " << CapacityPostMuonRecDigitRPCSet     << std::endl;
  std::cout << " MuonRecDigitTGCSet      " << CapacityAnteMuonRecDigitTGCSet     << "  " << CapacityPostMuonRecDigitTGCSet     << std::endl;
  std::cout << " MuonRecDigitCSCSet      " << CapacityAnteMuonRecDigitCSCSet     << "  " << CapacityPostMuonRecDigitCSCSet     << std::endl;
  std::cout << " MuonRecDigitCSCCluSet      " << CapacityAnteMuonRecDigitCSCCluSet     << "  " << CapacityPostMuonRecDigitCSCCluSet     << std::endl;
  std::cout << " LinearIndexMDTSet       " << CapacityAnteLinearIndexMDTSet      << "  " << CapacityPostLinearIndexMDTSet      << std::endl;
  std::cout << " LinearIndexRPCSet       " << CapacityAnteLinearIndexRPCSet      << "  " << CapacityPostLinearIndexRPCSet      << std::endl;
  std::cout << " LinearIndexTGCSet       " << CapacityAnteLinearIndexTGCSet      << "  " << CapacityPostLinearIndexTGCSet      << std::endl;
  std::cout << " LinearIndexCSCSet       " << CapacityAnteLinearIndexCSCSet      << "  " << CapacityPostLinearIndexCSCSet      << std::endl;
  std::cout << " LinearIndexCSCCluSet       " << CapacityAnteLinearIndexCSCCluSet      << "  " << CapacityPostLinearIndexCSCCluSet      << std::endl;
  std::cout << " LinearIndexToMDTSet     " << CapacityAnteLinearIndexToMDTSet    << "  " << CapacityPostLinearIndexToMDTSet    << std::endl;
  std::cout << " LinearIndexToRPCSet     " << CapacityAnteLinearIndexToRPCSet    << "  " << CapacityPostLinearIndexToRPCSet    << std::endl;
  std::cout << " LinearIndexToTGCSet     " << CapacityAnteLinearIndexToTGCSet    << "  " << CapacityPostLinearIndexToTGCSet    << std::endl;
  std::cout << " LinearIndexToCSCSet     " << CapacityAnteLinearIndexToCSCSet    << "  " << CapacityPostLinearIndexToCSCSet    << std::endl;
  std::cout << " LinearIndexToCSCCluSet     " << CapacityAnteLinearIndexToCSCCluSet    << "  " << CapacityPostLinearIndexToCSCCluSet    << std::endl;
 }

}

// Get Typed Digi Nber from a Linear Digi Nber

// HACKED by PFG 2008-07-02: the arguments of the following functions
// may be either a LinearNber or the identifier of a digit, as
// returned by MuonRecDigit::GetIdentifier. The TyId functions
// determine if it is of the first or second type based on the value.

// TODO: this is not so clean as MuonRecDigit::GetIdentifier can in
// principle return a value compatible with a LinearNber. Suggest
// that the code get rid of LinearNber completely?

int MuonRecDigiStore::TyIdMDT(int id) const{
  if (id < 0 || id >= NberOfDigi()) {
    // CASE 1: id is inconsistent with a LinearNber. Check in the
    // map to see if it is an identifier
    map<int,int>::const_iterator it = IdentifierToMDTSet.find(id);
    if (it != IdentifierToMDTSet.end()) return it->second;
  } else {
    // CASE 2: id is consistent with a LinearNber. Check in
    // LinearIndex arrays to see if it exists
    if (LinearIndexToMDTSet[id] >= 0) return LinearIndexToMDTSet[id] ;
  }
  // CASE 3: if neither case 1 or 2 have returned, return that we
  // dont know anything about id.
  return -1;
}
int MuonRecDigiStore::TyIdRPC(int id) const{
  if (id < 0 || id >= NberOfDigi()) {
    // CASE 1: id is inconsistent with a LinearNber. Check in the
    // map to see if it is an identifier
    map<int,int>::const_iterator it = IdentifierToRPCSet.find(id);
    if (it != IdentifierToRPCSet.end()) return it->second;
  } else {
    // CASE 2: id is consistent with a LinearNber. Check in
    // LinearIndex arrays to see if it exists
    if (LinearIndexToRPCSet[id] >= 0) return LinearIndexToRPCSet[id] ;
  }
  // CASE 3: if neither case 1 or 2 have returned, return that we
  // dont know anything about id.
  return -1;
}
int MuonRecDigiStore::TyIdTGC(int id) const{
  if (id < 0 || id >= NberOfDigi()) {
    // CASE 1: id is inconsistent with a LinearNber. Check in the
    // map to see if it is an identifier
    map<int,int>::const_iterator it = IdentifierToTGCSet.find(id);
    if (it != IdentifierToTGCSet.end()) return it->second;
  } else {
    // CASE 2: id is consistent with a LinearNber. Check in
    // LinearIndex arrays to see if it exists
    if (LinearIndexToTGCSet[id] >= 0) return LinearIndexToTGCSet[id] ;
  }
  // CASE 3: if neither case 1 or 2 have returned, return that we
  // dont know anything about id.
  return -1;
}
int MuonRecDigiStore::TyIdCSC(int id) const{
  if (id < 0 || id >= NberOfDigi()) {
    // CASE 1: id is inconsistent with a LinearNber. Check in the
    // map to see if it is an identifier
    map<int,int>::const_iterator it = IdentifierToCSCSet.find(id);
    if (it != IdentifierToCSCSet.end()) return it->second;
  } else {
    // CASE 2: id is consistent with a LinearNber. Check in
    // LinearIndex arrays to see if it exists
    if (LinearIndexToCSCSet[id] >= 0) return LinearIndexToCSCSet[id] ;
  }
  // CASE 3: if neither case 1 or 2 have returned, return that we
  // dont know anything about id.
  return -1;
}
int MuonRecDigiStore::TyIdCSCClu(int id) const{
  if (id < 0 || id >= NberOfDigi()) {
    // CASE 1: id is inconsistent with a LinearNber. Check in the
    // map to see if it is an identifier
    map<int,int>::const_iterator it = IdentifierToCSCCluSet.find(id);
    if (it != IdentifierToCSCCluSet.end()) return it->second;
  } else {
    // CASE 2: id is consistent with a LinearNber. Check in
    // LinearIndex arrays to see if it exists
    if (LinearIndexToCSCCluSet[id] >= 0) return LinearIndexToCSCCluSet[id] ;
  }
  // CASE 3: if neither case 1 or 2 have returned, return that we
  // dont know anything about id.
  return -1;
}

void MuonRecDigiStore::PushBackToSmthgSet(){
  int DummyEntry = - 1 ;

  LinearIndexToMDTSet.push_back    (DummyEntry);
  LinearIndexToRPCSet.push_back    (DummyEntry);
  LinearIndexToTGCSet.push_back    (DummyEntry);
  LinearIndexToCSCSet.push_back    (DummyEntry);
  LinearIndexToCSCCluSet.push_back (DummyEntry);

}
