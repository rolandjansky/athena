/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIDTagTGC_H
#define MbIDTagTGC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIDTag.h"

  /**
   @class MbIDTagTGC

   This class is for taging a TGC data

  @author samusog@cern.ch
  
  */

class MbIDTagTGC: public MbIDTag{
public:
    MbIDTagTGC();
    virtual ~MbIDTagTGC();
    MbIDTagTGC(const MbIDTagTGC& ToBeCopied);
    MbIDTagTGC& operator=( const MbIDTagTGC& ToBeCopied );
    
public:
///////////////////////////////////

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Athena GasGap  Id  */
   void SetSGGasGap     (const int&	    SGGasGap     );

   /**Set Athena IsStrip Id  */
   void SetSGIsStrip    (const int&	    SGIsStrip    );

   /**Set Athena Channel Id  */
   void SetSGChannel    (const int&	    SGChannel    );

   /**Set Amdc JSPLI Id  */
   void SetJSPLI       (const int& 	   JSPLI       );

   /**Set Amdc JSL   Id  */
   void SetJSL	       (const int& 	   JSL	       );

   /**Set Amdc JSZ   Id  */
   void SetJSZ	       (const int& 	   JSZ	       );

   /**Set Amdc JSTRI Id  */
   void SetJSTRI       (const int& 	   JSTRI       );

   /**Get Athena GasGap  Id  */
   int	       GetSGGasGap     () const ;

   /**Get Athena IsStrip Id  */
   int	       GetSGIsStrip    () const ;

   /**Get Athena Channel Id  */
   int	       GetSGChannel    () const ;

   /**Get Amdc JSPLI Id  */
   int	       GetJSPLI	       () const ;

   /**Get Amdc JSL   Id  */
   int	       GetJSL	       () const ;

   /**Get Amdc JSZ   Id  */
   int	       GetJSZ	       () const ;

   /**Get Amdc JSTRI Id  */
   int	       GetJSTRI	       () const ;

  /**Copy Ids */
  void CopyIds(const MbIDTagTGC* pMbIDTag) ;
  
protected:
///////////////////////////////////

 int m_SGGasGap  ; //!< Athena GasGap  Id 
 int m_SGIsStrip ; //!< Athena IsStrip Id 
 int m_SGChannel ; //!< Athena Channel Id 
 
 int m_JSPLI     ; //!< Amdc JSPLI Id 
 int m_JSL       ; //!< Amdc JSL   Id 
 int m_JSZ       ; //!< Amdc JSZ   Id 
 int m_JSTRI     ; //!< Amdc JSTRI Id 

};

#endif
