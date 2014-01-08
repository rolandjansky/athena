/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigiStore_H
#define MuonRecDigiStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigitMDT.h"
#include "MboyCoreEvt/MuonRecDigitRPC.h"
#include "MboyCoreEvt/MuonRecDigitTGC.h"
#include "MboyCoreEvt/MuonRecDigitCSC.h"
#include "MboyCoreEvt/MuonRecDigitCSCClu.h"

  /**
   @class MuonRecDigiStore

   This class is for managing collections of digits
   
   A digit is added using the Add method.
   
   It is accessible using a index, LinearNber, going from 0 to NberOfDigi()-1
   and a technology dependent method, e.g. LiIdGetMuonRecDigitMDT or NonCte_LiIdGetMuonRecDigitCSC, 
   which returns 0 if LinearNber does not correspond to the selected technology type.
   
   Internally, digits are stored in vectors (e.g MuonRecDigitMDTSet) according to their type
   The method TyTECH, e.g TyIdMDT, for a given LinearNber, returns the rank of the digit
   in these vectors (and -1 if if LinearNber does not correspond to the selected technology type).
   This connection is done using arrays such as LinearIndexToMDTSet. The reverse connection
   is possible thanks to arrays such as LinearIndexMDTSet.
   
  @author samusog@cern.ch
  
  */

class MuonRecDigiStore{
public:
    MuonRecDigiStore();
    virtual ~MuonRecDigiStore();

public:
///////////////////////////////////

   /**Reset the Store */
   void ResetMuonRecDigiStore();

   /**Set Run number */
   void SetRunNber(int RunNber);

   /**Set Event number */
   void SetEventNber(int EventNber);

   /**Get Run number */
   int  GetRunNber() const;

   /**Set Event number */
   int  GetEventNber() const;

   /**Print summary */
   void PrintSummary() const;

   /**Print digits data using Amdc Ids */
   void PrintDigits(std::ostream* out) const;

   /**Print digits data using Athena Ids */
   void PrintDigitsAthIds(std::ostream* out) const;

   /**Add typed element */
   void Add(MuonRecDigitMDT& ToBeAdded);
   void Add(MuonRecDigitRPC& ToBeAdded);
   void Add(MuonRecDigitTGC& ToBeAdded);
   void Add(MuonRecDigitCSC& ToBeAdded);
   void Add(MuonRecDigitCSCClu& ToBeAdded);

   /**Get number of typed elements */
   int NberOfDigiMDT() const ;
   int NberOfDigiRPC() const ;
   int NberOfDigiTGC() const ;
   int NberOfDigiCSC() const ;
   int NberOfDigiCSCClu() const ;

   /**Get number of elements */
   int NberOfDigi() const ;

   /**Get const pointer of a typed element */
   const MuonRecDigitMDT*    LiIdGetMuonRecDigitMDT    (int LinearNber) const;
   const MuonRecDigitRPC*    LiIdGetMuonRecDigitRPC    (int LinearNber) const;
   const MuonRecDigitTGC*    LiIdGetMuonRecDigitTGC    (int LinearNber) const;
   const MuonRecDigitCSC*    LiIdGetMuonRecDigitCSC    (int LinearNber) const;
   const MuonRecDigitCSCClu* LiIdGetMuonRecDigitCSCClu (int LinearNber) const;

   /**Get pointer of a typed element */
   MuonRecDigitMDT*    NonCte_LiIdGetMuonRecDigitMDT    (int LinearNber) ;
   MuonRecDigitRPC*    NonCte_LiIdGetMuonRecDigitRPC    (int LinearNber) ;
   MuonRecDigitTGC*    NonCte_LiIdGetMuonRecDigitTGC    (int LinearNber) ;
   MuonRecDigitCSC*    NonCte_LiIdGetMuonRecDigitCSC    (int LinearNber) ;
   MuonRecDigitCSCClu* NonCte_LiIdGetMuonRecDigitCSCClu (int LinearNber) ;

   /**Print checks */
   void PrintChecks() const;

   /**Print summary */
   void PrintDigitsSums(std::ostream* out) const;

   /**Print strings for R vs S function */
   void PrintDigitsRsLine(std::ostream* out) const;

   /**Reserve space */
   void ReserveMuonRecDigiStore(
    const int& DoReserve,
    const int& SpaceForMDT   ,
    const int& SpaceForRPC   ,
    const int& SpaceForTGC   ,
    const int& SpaceForCSC   ,
    const int& SpaceForCSCClu
    );

protected:
///////////////////////////////////
//Functions

   /**Push back all ToSmthgSet*/
   void PushBackToSmthgSet () ;

   /**Rank in technology dependent element vectors */
   int TyIdMDT   (int LinearNber) const;
   int TyIdRPC   (int LinearNber) const;
   int TyIdTGC   (int LinearNber) const;
   int TyIdCSC   (int LinearNber) const;
   int TyIdCSCClu(int LinearNber) const;

//Data

   int m_RunNber;   //!< Run number
   int m_EventNber; //!< Event number

   int m_KounterMDT    ;//!< Kounter for MDT   
   int m_KounterRPC    ;//!< Kounter for RPC   
   int m_KounterTGC    ;//!< Kounter for TGC   
   int m_KounterCSC    ;//!< Kounter for CSC   
   int m_KounterCSCClu ;//!< Kounter for CSC Clusters
   
   int m_KounterAll    ;//!< Kounter for all types

   std::vector<MuonRecDigitMDT>     MuonRecDigitMDTSet;    //!< Storage vector for MDT   
   std::vector<MuonRecDigitRPC>     MuonRecDigitRPCSet;    //!< Storage vector for RPC   
   std::vector<MuonRecDigitTGC>     MuonRecDigitTGCSet;    //!< Storage vector for TGC   
   std::vector<MuonRecDigitCSC>     MuonRecDigitCSCSet;    //!< Storage vector for CSC   
   std::vector<MuonRecDigitCSCClu>  MuonRecDigitCSCCluSet; //!< Storage vector for CSC Clusters

   std::vector<int>  LinearIndexMDTSet;      //!< Connection typed Id to global Id for MDT   
   std::vector<int>  LinearIndexRPCSet;      //!< Connection typed Id to global Id for RPC   
   std::vector<int>  LinearIndexTGCSet;      //!< Connection typed Id to global Id for TGC   
   std::vector<int>  LinearIndexCSCSet;      //!< Connection typed Id to global Id for CSC   
   std::vector<int>  LinearIndexCSCCluSet;   //!< Connection typed Id to global Id for CSC Clusters

   std::vector<int>  LinearIndexToMDTSet;      //!< Connection global Id to typed Id for MDT   
   std::vector<int>  LinearIndexToRPCSet;      //!< Connection global Id to typed Id for RPC   
   std::vector<int>  LinearIndexToTGCSet;      //!< Connection global Id to typed Id for TGC   
   std::vector<int>  LinearIndexToCSCSet;      //!< Connection global Id to typed Id for CSC   
   std::vector<int>  LinearIndexToCSCCluSet;   //!< Connection global Id to typed Id for CSC Clusters

  std::map<int,int> IdentifierToMDTSet;    //!< Connection from Digit Identifier to typed Id for MDT
  std::map<int,int> IdentifierToRPCSet;    //!< Connection from Digit Identifier to typed Id for RPC
  std::map<int,int> IdentifierToTGCSet;    //!< Connection from Digit Identifier to typed Id for TGC
  std::map<int,int> IdentifierToCSCSet;    //!< Connection from Digit Identifier to typed Id for CSC
  std::map<int,int> IdentifierToCSCCluSet; //!< Connection from Digit Identifier to typed Id for CSCClu
};


#endif
