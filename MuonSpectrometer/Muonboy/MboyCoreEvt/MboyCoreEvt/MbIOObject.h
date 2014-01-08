/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIOObject_H
#define MbIOObject_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MbIOObject

   This class is a class to tel nature of the data

  @author samusog@cern.ch
  
  */

class MbIOObject{
public:
    MbIOObject();
    virtual ~MbIOObject();
    MbIOObject( const MbIOObject& aMbIOObject );

public:
///////////////////////////////////

   /**Print the data */
   virtual void Print(std::ostream* out)  const = 0;
   virtual void PrintCout()  const = 0 ;

   /**Get bool Types  */
   bool IsTypePRD () const ;
   bool IsTypeROT () const ;
   bool IsTypeHOLE() const ;
   bool IsTypeMUSC() const ;

   bool IsTechMDT   () const ;
   bool IsTechRPC   () const ;
   bool IsTechTGC   () const ;
   bool IsTechCSC   () const ;
   bool IsTechCSCclu() const ;

   /**Get string Types  */
   std::string GetType() const ;
   std::string GetTech() const ;

   bool IsOutlier   () const ;
   void SetOutlier  () ;

protected:
///////////////////////////////////

 
 bool m_Type_PRD  ;
 bool m_Type_ROT  ;
 bool m_Type_HOLE ;
 bool m_Type_MUSC ;
 
 void ResetType() ; 
 void Type_PRD () ;
 void Type_ROT () ;
 void Type_HOLE() ;
 void Type_MUSC() ;

 bool m_Tech_MDT    ;
 bool m_Tech_RPC    ;
 bool m_Tech_TGC    ;
 bool m_Tech_CSC    ;
 bool m_Tech_CSCclu ;
 
 void ResetTech(); 
 void Tech_MDT   () ;
 void Tech_RPC   () ;
 void Tech_TGC   () ;
 void Tech_CSC   () ;
 void Tech_CSCclu() ;

 bool m_Status_Outlier ;

};


inline bool MbIOObject::IsOutlier  () const { return m_Status_Outlier  ; }
inline void MbIOObject::SetOutlier ()       { m_Status_Outlier = true  ; }

inline bool MbIOObject::IsTypePRD () const { return m_Type_PRD  ; }
inline bool MbIOObject::IsTypeROT () const { return m_Type_ROT  ; }
inline bool MbIOObject::IsTypeHOLE() const { return m_Type_HOLE ; }
inline bool MbIOObject::IsTypeMUSC() const { return m_Type_MUSC ; }

inline bool MbIOObject::IsTechMDT   () const { return  m_Tech_MDT    ; }
inline bool MbIOObject::IsTechRPC   () const { return  m_Tech_RPC    ; }
inline bool MbIOObject::IsTechTGC   () const { return  m_Tech_TGC    ; }
inline bool MbIOObject::IsTechCSC   () const { return  m_Tech_CSC    ; }
inline bool MbIOObject::IsTechCSCclu() const { return  m_Tech_CSCclu ; }

inline void MbIOObject::ResetType() {
 m_Type_PRD  = false ;
 m_Type_ROT  = false ;
 m_Type_HOLE = false ;
 m_Type_MUSC = false ;
}
inline void MbIOObject::Type_PRD () { ResetType() ; m_Type_PRD  = true ; }
inline void MbIOObject::Type_ROT () { ResetType() ; m_Type_ROT  = true ; }
inline void MbIOObject::Type_HOLE() { ResetType() ; m_Type_HOLE = true ; }
inline void MbIOObject::Type_MUSC() { ResetType() ; m_Type_MUSC = true ; }

inline void MbIOObject::ResetTech() {
 m_Tech_MDT    = false ;
 m_Tech_RPC    = false ;
 m_Tech_TGC    = false ;
 m_Tech_CSC    = false ;
 m_Tech_CSCclu = false ;
}
inline void MbIOObject::Tech_MDT   () { ResetTech() ; m_Tech_MDT    = true ; }
inline void MbIOObject::Tech_RPC   () { ResetTech() ; m_Tech_RPC    = true ; }
inline void MbIOObject::Tech_TGC   () { ResetTech() ; m_Tech_TGC    = true ; }
inline void MbIOObject::Tech_CSC   () { ResetTech() ; m_Tech_CSC    = true ; }
inline void MbIOObject::Tech_CSCclu() { ResetTech() ; m_Tech_CSCclu = true ; }

inline std::string MbIOObject::GetType() const { 
 if ( IsTypePRD () ) return "PRD"     ;
 if ( IsTypeROT () ) return "ROT"     ;
 if ( IsTypeHOLE() ) return "HOLE"    ;
 if ( IsTypeMUSC() ) return "MUSC"    ;
 return "XXX"    ;
}
inline std::string MbIOObject::GetTech() const { 
 if ( IsTechMDT   () ) return "MDT"     ;
 if ( IsTechRPC   () ) return "RPC"     ;
 if ( IsTechTGC   () ) return "TGC"     ;
 if ( IsTechCSC   () ) return "CSC"     ;
 if ( IsTechCSCclu() ) return "CSCclu"  ;
 return "XXX"    ;
}

#endif
