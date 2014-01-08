/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIDTagRPC_H
#define MbIDTagRPC_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIDTag.h"

  /**
   @class MbIDTagRPC

   This class is for taging a RPC data

  @author samusog@cern.ch
  
  */

class MbIDTagRPC: public MbIDTag{
public:
    MbIDTagRPC();
    virtual ~MbIDTagRPC();
    MbIDTagRPC(const MbIDTagRPC& ToBeCopied);
    MbIDTagRPC& operator=( const MbIDTagRPC& ToBeCopied );

public:
///////////////////////////////////

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Athena DoubletR    Id  */
   void SetSGDoubletR   (const int&	    SGDoubletR	   );
  
   /**Set Athena DoubletZ    Id  */
   void SetSGDoubletZ   (const int&	    SGDoubletZ	   );
  
   /**Set Athena DoubletPhi  Id  */
   void SetSGDoubletPhi (const int&	    SGDoubletPhi   );
 
   /**Set Athena GasGap      Id  */
   void SetSGGasGap     (const int&	    SGGasGap	   );

   /**Set Athena MeasuresPhi Id  */
   void SetSGMeasuresPhi(const int&	    SGMeasuresPhi  );

   /**Set Athena Strip       Id  */
   void SetSGStrip      (const int&	    SGStrip	   );

   /**Set Amdc JSPLI Id */
   void SetJSPLI       (const int& 	   JSPLI       );

   /**Set Amdc JSL   Id */
   void SetJSL	       (const int& 	   JSL	       );

   /**Set Amdc JSZ   Id */
   void SetJSZ	       (const int& 	   JSZ	       );

   /**Set Amdc JSTRI Id */
   void SetJSTRI       (const int& 	   JSTRI       );

   /**Get Athena DoubletR    Id  */
   int	       GetSGDoubletR   () const ; 

   /**Get Athena DoubletZ    Id  */
   int	       GetSGDoubletZ   () const ; 

   /**Get Athena DoubletPhi  Id  */
   int	       GetSGDoubletPhi () const ; 

   /**Get Athena GasGap      Id  */
   int	       GetSGGasGap     () const ; 

   /**Get Athena MeasuresPhi Id  */
   int	       GetSGMeasuresPhi() const ; 

   /**Get Athena Strip       Id  */
   int	       GetSGStrip      () const ; 

   /**Get Amdc JSPLI Id */
   int	       GetJSPLI	       () const ;

   /**Get Amdc JSL   Id */
   int	       GetJSL	       () const ;
 
   /**Get Amdc JSZ   Id */
   int	       GetJSZ	       () const ;

   /**Get Amdc JSTRI Id */
   int	       GetJSTRI	       () const ;

  /**Copy Ids */
  void CopyIds(const MbIDTagRPC* pMbIDTag) ;
  
protected:
///////////////////////////////////

 int m_SGDoubletR    ; //!< Athena DoubletR    Id  
 int m_SGDoubletZ    ; //!< Athena DoubletZ    Id  
 int m_SGDoubletPhi  ; //!< Athena DoubletPhi  Id  
 int m_SGGasGap      ; //!< Athena GasGap      Id  
 int m_SGMeasuresPhi ; //!< Athena MeasuresPhi Id  
 int m_SGStrip       ; //!< Athena Strip       Id  

 int m_JSPLI         ; //!< Amdc JSPLI Id  
 int m_JSL           ; //!< Amdc JSL   Id  
 int m_JSZ           ; //!< Amdc JSZ   Id  
 int m_JSTRI         ; //!< Amdc JSTRI Id  
 
};

#endif
