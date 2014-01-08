/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AccessorToMbIOObjectStore_H
#define AccessorToMbIOObjectStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIOObjectStore.h"
   
  /**
   @class AccessorToMbIOObjectStore

   This class is providing standard acces to a .MbIOObjectStore

  @author samusog@cern.ch
  
  */

class AccessorToMbIOObjectStore{
public:
   AccessorToMbIOObjectStore();
   AccessorToMbIOObjectStore(
      MbIOObjectStore&  aMbIOObjectStore
   );
   virtual ~AccessorToMbIOObjectStore();

   /**Get number of typed elements */
   int GetNberOfDigiMDT   () const;
   int GetNberOfDigiRPC   () const;
   int GetNberOfDigiTGC   () const;
   int GetNberOfDigiCSC   () const;
   int GetNberOfDigiCSCclu() const;
   
   int GetNberOfMbMuSc() const ;
   
   int GetNberOfHoleMDT   () const ;
   int GetNberOfHoleRPC   () const ;
   int GetNberOfHoleTGC   () const ;
   int GetNberOfHoleCSC   () const ;
   int GetNberOfHoleCSCclu() const ;

   /**Get number of elements */
   int GetNberOfElements  () const;
   int GetNberOfDigi      () const;

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

   /**Print Sorted elements */
   void PrintSortedElements(std::ostream* out) const ;
   
protected:
///////////////////////////////////

   MbIOObjectStore  m_MbIOObjectStore  ; //!< Associated MbIOObjects

};
#endif
