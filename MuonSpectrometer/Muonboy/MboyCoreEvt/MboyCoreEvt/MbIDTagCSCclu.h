/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbIDTagCSCclu_H
#define MbIDTagCSCclu_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIDTag.h"

  /**
   @class MbIDTagCSCclu

   This class is for taging a CSC cluster data

  @author samusog@cern.ch
  
  */

class MbIDTagCSCclu: public MbIDTag{
public:
    MbIDTagCSCclu();
    virtual ~MbIDTagCSCclu();
    MbIDTagCSCclu(const MbIDTagCSCclu& ToBeCopied);
    MbIDTagCSCclu& operator=( const MbIDTagCSCclu& ToBeCopied );

public:
///////////////////////////////////

   /**Print the data using Amdc Ids*/
   virtual void PrintAmdIds(std::ostream* out) const;

   /**Print the data using Athena Ids*/
   virtual void PrintAthIds(std::ostream* out) const;

   /**Set Athena ChamberLayer Id  */
   void SetSGChamberLayer(const int&         SGChamberLayer);

   /**Set Athena WireLayer    Id  */
   void SetSGWireLayer   (const int&         SGWireLayer   );

   /**Set Athena MeasuresPhi  Id  */
   void SetSGMeasuresPhi (const int&         SGMeasuresPhi );

   /**Set Athena Strip        Id  */
   void SetSGStrip       (const int&         SGStrip       );

   /**Set Amdc JSL    Id  */
   void SetJSL            (const int&        JSL            );

   /**Set Amdc JSTRI  Id  */
   void SetJSTRI          (const int&        JSTRI          );

   /**Set Amdc SZFLAG Id  */
   void SetSZFLAG         (const double&     SZFLAG         );

   /**Get Athena ChamberLayer Id  */
   int	       GetSGChamberLayer() const ;

   /**Get Athena WireLayer    Id  */
   int	       GetSGWireLayer	() const ;

   /**Get Athena MeasuresPhi  Id  */
   int	       GetSGMeasuresPhi () const ;

   /**Get Athena Strip        Id  */
   int	       GetSGStrip	() const ;

   /**Get Amdc JSL    Id  */
   int	       GetJSL	      () const ;

   /**Get Amdc JSTRI  Id  */
   int	       GetJSTRI	      () const ;

   /**Get Amdc SZFLAG Id  */
   double      GetSZFLAG      () const ;

  /**Copy Ids */
  void CopyIds(const MbIDTagCSCclu* pMbIDTag) ;
  
protected:
///////////////////////////////////

 int    m_SGChamberLayer; //!< Athena ChamberLayer Id  
 int    m_SGWireLayer   ; //!< Athena WireLayer    Id  
 int    m_SGMeasuresPhi ; //!< Athena MeasuresPhi  Id  
 int    m_SGStrip       ; //!< Athena Strip        Id  

 int    m_JSL           ; //!< Amdc JSL    Id 
 int    m_JSTRI         ; //!< Amdc JSTRI  Id  
 double m_SZFLAG        ; //!< Amdc SZFLAG Id  

};

#endif
