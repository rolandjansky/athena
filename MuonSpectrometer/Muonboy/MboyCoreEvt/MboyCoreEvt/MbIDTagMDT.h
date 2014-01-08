/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIDTagMDT_H
#define MbIDTagMDT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIDTag.h"

  /**
   @class MbIDTagMDT

   This class is for taging a MDT data

  @author samusog@cern.ch
  
  */

class MbIDTagMDT: public MbIDTag{
public:
    MbIDTagMDT();
    virtual ~MbIDTagMDT();
    MbIDTagMDT(const MbIDTagMDT& ToBeCopied);
    MbIDTagMDT& operator=( const MbIDTagMDT& ToBeCopied );

public:
///////////////////////////////////

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Athena Multilayer Id  */
   void SetSGMultilayer (const int&         SGMultilayer   );

   /**Set Athena TubeLayer  Id  */
   void SetSGTubeLayer  (const int&         SGTubeLayer    );

   /**Set Athena Tube       Id  */
   void SetSGTube       (const int&         SGTube         );

   /**Set Amdc JSL   Id  */
   void SetJSL	        (const int&         JSL            );

   /**Set Amdc JTUBE Id  */
   void SetJTUBE	(const int&         JTUBE          );

   /**Get Athena Multilayer Id  */
   int	       GetSGMultilayer   () const ; 

   /**Get Athena TubeLayer  Id  */
   int	       GetSGTubeLayer	 () const ; 

   /**Get Athena Tube       Id  */
   int	       GetSGTube	 () const ; 

   /**Get Amdc JSL   Id  */
   int         GetJSL		 () const ;

   /**Get Amdc JTUBE Id  */
   int         GetJTUBE 	 () const ;

  /**Copy Ids */
  void CopyIds(const MbIDTagMDT* pMbIDTag) ;
  
protected:
///////////////////////////////////

 int m_SGMultilayer ; //!< Athena Multilayer Id  
 int m_SGTubeLayer  ; //!< Athena TubeLayer  Id  
 int m_SGTube       ; //!< Athena Tube       Id  

 int m_JSL          ; //!< Amdc JSL   Id 
 int m_JTUBE        ; //!< Amdc JTUBE Id 

};

#endif
