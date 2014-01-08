/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIOObjectStore_H
#define MbIOObjectStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbROTMDT.h"
#include "MboyCoreEvt/MbROTRPC.h"
#include "MboyCoreEvt/MbROTTGC.h"
#include "MboyCoreEvt/MbROTCSC.h"
#include "MboyCoreEvt/MbROTCSCclu.h"

#include "MboyCoreEvt/MbMuSc.h"

#include "MboyCoreEvt/MbHoleMDT.h"
#include "MboyCoreEvt/MbHoleRPC.h"
#include "MboyCoreEvt/MbHoleTGC.h"
#include "MboyCoreEvt/MbHoleCSC.h"
#include "MboyCoreEvt/MbHoleCSCclu.h"

#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"
#include "MboyCoreEvt/MbIDTagCSC.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

  /**
   @class MbIOObjectStore

   This class is for managing collections of digit-recs
   
   A digit is added using the Add method.
   
   It is accessible using a index, LinearNber, going from 0 to NberOfDigi()-1
   and a technology dependent method, e.g. LiIdGetMbROTMDT, 
   which returns 0 if LinearNber does not correspond to the selected technology type.
   
   Internally, digits are stored in vectors (e.g MbROTMDTSet) according to their type
   The method TyTECH, e.g TyIdMDT, for a given LinearNber, returns the rank of the digit
   in these vectors (and -1 if if LinearNber does not correspond to the selected technology type).
   This connection is done using arrays such as LinearIndexToMDTSet. The reverse connection
   is possible thanks to arrays such as LinearIndexMDTSet.
   
  @author samusog@cern.ch
  
  */

class MbIOObjectStore{
public:
    MbIOObjectStore();
    virtual ~MbIOObjectStore();

public:
///////////////////////////////////

   /**Reset the Store */
   void ResetMbIOObjectStore();

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
   void Add(const MbROTMDT&    ToBeAdded);
   void Add(const MbROTRPC&    ToBeAdded);
   void Add(const MbROTTGC&    ToBeAdded);
   void Add(const MbROTCSC&    ToBeAdded);
   void Add(const MbROTCSCclu& ToBeAdded);
   
   void Add(const MbMuSc&      ToBeAdded);
   
   void Add(const MbHoleMDT&      ToBeAdded);
   void Add(const MbHoleRPC&      ToBeAdded);
   void Add(const MbHoleTGC&      ToBeAdded);
   void Add(const MbHoleCSC&      ToBeAdded);
   void Add(const MbHoleCSCclu&   ToBeAdded);

   /**Get number of typed elements */
   int GetNberOfDigiMDT   () const ;
   int GetNberOfDigiRPC   () const ;
   int GetNberOfDigiTGC   () const ;
   int GetNberOfDigiCSC   () const ;
   int GetNberOfDigiCSCclu() const ;
   
   int GetNberOfMbMuSc() const ;
   
   int GetNberOfHoleMDT   () const ;
   int GetNberOfHoleRPC   () const ;
   int GetNberOfHoleTGC   () const ;
   int GetNberOfHoleCSC   () const ;
   int GetNberOfHoleCSCclu() const ;

   /**Get number of elements */
   int GetNberOfElements() const ;
   int GetNberOfDigi()     const ;

   /**Get const pointer of a element */
   const MbIOObject*    LiIdGetMbIOObject  (int LinearNber) const;
   const MbIDTag*       LiIdGetMbIDTag     (int LinearNber) const;
   const MbPOT*         LiIdGetMbPOT       (int LinearNber) const;
   const MbROT*         LiIdGetMbROT       (int LinearNber) const;
   const MbHole*        LiIdGetMbHole      (int LinearNber) const;

   /**Get const pointer of a typed element */
   const MbROTMDT*      LiIdGetMbROTMDT    (int LinearNber) const;
   const MbROTRPC*      LiIdGetMbROTRPC    (int LinearNber) const;
   const MbROTTGC*      LiIdGetMbROTTGC    (int LinearNber) const;
   const MbROTCSC*      LiIdGetMbROTCSC    (int LinearNber) const;
   const MbROTCSCclu*   LiIdGetMbROTCSCclu (int LinearNber) const;

   const MbMuSc*        LiIdGetMbMuSc    (int LinearNber) const;

   const MbHoleMDT*     LiIdGetMbHoleMDT    (int LinearNber) const;
   const MbHoleRPC*     LiIdGetMbHoleRPC    (int LinearNber) const;
   const MbHoleTGC*     LiIdGetMbHoleTGC    (int LinearNber) const;
   const MbHoleCSC*     LiIdGetMbHoleCSC    (int LinearNber) const;
   const MbHoleCSCclu*  LiIdGetMbHoleCSCclu (int LinearNber) const;

   const MbIDTagMDT*    LiIdGetMbIDTagMDT    (int LinearNber) const;
   const MbIDTagRPC*    LiIdGetMbIDTagRPC    (int LinearNber) const;
   const MbIDTagTGC*    LiIdGetMbIDTagTGC    (int LinearNber) const;
   const MbIDTagCSC*    LiIdGetMbIDTagCSC    (int LinearNber) const;
   const MbIDTagCSCclu* LiIdGetMbIDTagCSCclu (int LinearNber) const;

   /**Get number of sorted elements */
   int GetNberOfElsorted() const ;

   /**Get LinearNber Sorted */
   int GetLinearNberSorted(int IdSorted) const ;

   /** Sort */
   void SortElements(
                  double XrefStart,
                  double YrefStart,
                  double ZrefStart 
                 ) ;

   /**Print Sorted elements */
   void PrintSortedElements(std::ostream* out) const ;

protected:
///////////////////////////////////
//Functions

   /**Push back all ToSmthgSet*/
   void PushBackToSmthgSet () ;

   /**Rank in technology dependent element vectors */
   int TyIdMDT       (int LinearNber) const;
   int TyIdRPC       (int LinearNber) const;
   int TyIdTGC       (int LinearNber) const;
   int TyIdCSC       (int LinearNber) const;
   int TyIdCSCclu    (int LinearNber) const;
   
   int TyIdMbMuSc   (int LinearNber) const;
   
   int TyIdHoleMDT   (int LinearNber) const;
   int TyIdHoleRPC   (int LinearNber) const;
   int TyIdHoleTGC   (int LinearNber) const;
   int TyIdHoleCSC   (int LinearNber) const;
   int TyIdHoleCSCclu(int LinearNber) const;

   int IsValidLinearNber(int LinearNber) const;

//Data

   int m_RunNber;   //!< Run number
   int m_EventNber; //!< Event number

   int m_KounterMDT    ;//!< Kounter for MDT   
   int m_KounterRPC    ;//!< Kounter for RPC   
   int m_KounterTGC    ;//!< Kounter for TGC   
   int m_KounterCSC    ;//!< Kounter for CSC   
   int m_KounterCSCclu ;//!< Kounter for CSC Clusters

   int m_KounterMbMuSc    ;//!< Kounter for Mu Sca 

   int m_KounterHoleMDT    ;//!< Kounter for hole MDT   
   int m_KounterHoleRPC    ;//!< Kounter for hole RPC   
   int m_KounterHoleTGC    ;//!< Kounter for hole TGC   
   int m_KounterHoleCSC    ;//!< Kounter for hole CSC   
   int m_KounterHoleCSCclu ;//!< Kounter for hole CSC Clusters

   int m_KounterAll    ;//!< Kounter for all types

   std::vector<MbROTMDT>     MbROTMDTSet;    //!< Storage vector for MDT   
   std::vector<MbROTRPC>     MbROTRPCSet;    //!< Storage vector for RPC   
   std::vector<MbROTTGC>     MbROTTGCSet;    //!< Storage vector for TGC   
   std::vector<MbROTCSC>     MbROTCSCSet;    //!< Storage vector for CSC   
   std::vector<MbROTCSCclu>  MbROTCSCcluSet; //!< Storage vector for CSC Clusters

   std::vector<MbMuSc>     MbMuScSet;    //!< Storage vector for hole MDT   

   std::vector<MbHoleMDT>     MbHoleMDTSet;    //!< Storage vector for hole MDT   
   std::vector<MbHoleRPC>     MbHoleRPCSet;    //!< Storage vector for hole RPC   
   std::vector<MbHoleTGC>     MbHoleTGCSet;    //!< Storage vector for hole TGC   
   std::vector<MbHoleCSC>     MbHoleCSCSet;    //!< Storage vector for hole CSC   
   std::vector<MbHoleCSCclu>  MbHoleCSCcluSet; //!< Storage vector for hole CSC Clusters

   std::vector<int>  LinearIndexMDTSet;    //!< Storage vector for MDT   
   std::vector<int>  LinearIndexRPCSet;    //!< Storage vector for RPC   
   std::vector<int>  LinearIndexTGCSet;    //!< Storage vector for TGC   
   std::vector<int>  LinearIndexCSCSet;    //!< Storage vector for CSC   
   std::vector<int>  LinearIndexCSCcluSet; //!< Storage vector for CSC Clusters

   std::vector<int>  LinearIndexMbMuScSet;    //!< Storage vector for Mu Sca  

   std::vector<int>  LinearIndexHoleMDTSet;    //!< Storage vector for hole MDT   
   std::vector<int>  LinearIndexHoleRPCSet;    //!< Storage vector for hole RPC   
   std::vector<int>  LinearIndexHoleTGCSet;    //!< Storage vector for hole TGC   
   std::vector<int>  LinearIndexHoleCSCSet;    //!< Storage vector for hole CSC   
   std::vector<int>  LinearIndexHoleCSCcluSet; //!< Storage vector for hole CSC Clusters

   std::vector<int>  LinearIndexToMDTSet;    //!< Connection global Id to typed Id for MDT   
   std::vector<int>  LinearIndexToRPCSet;    //!< Connection global Id to typed Id for RPC   
   std::vector<int>  LinearIndexToTGCSet;    //!< Connection global Id to typed Id for TGC   
   std::vector<int>  LinearIndexToCSCSet;    //!< Connection global Id to typed Id for CSC   
   std::vector<int>  LinearIndexToCSCcluSet; //!< Connection global Id to typed Id for CSC Clusters

   std::vector<int>  LinearIndexToMbMuScSet;    //!< Connection global Id to typed Id for Mu Sc  

   std::vector<int>  LinearIndexToHoleMDTSet;    //!< Connection global Id to typed Id for hole MDT   
   std::vector<int>  LinearIndexToHoleRPCSet;    //!< Connection global Id to typed Id for hole RPC   
   std::vector<int>  LinearIndexToHoleTGCSet;    //!< Connection global Id to typed Id for hole TGC   
   std::vector<int>  LinearIndexToHoleCSCSet;    //!< Connection global Id to typed Id for hole CSC   
   std::vector<int>  LinearIndexToHoleCSCcluSet; //!< Connection global Id to typed Id for hole CSC Clusters

   int m_HasBeenSorted; //!< Tells if the digits collections has been sorted
   std::vector<int>  m_LinearIndexSorted;    //!<  Array of sorted Ids   

};


#endif
