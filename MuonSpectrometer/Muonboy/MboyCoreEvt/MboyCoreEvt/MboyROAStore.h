/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyROAStore_H
#define MboyROAStore_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MboyROA.h"

  /**
   @class MboyROAStore

  @author samusog@cern.ch
  
  */

class MboyROAStore{
public:
   MboyROAStore();
   virtual ~MboyROAStore();

public:
///////////////////////////////////
 
   /**Reset the Store */
   void Reset();

   /**Add a MboyROA */
   void AddROA(MboyROA& ToBeAdded);
   void AddROA(
            const double& ROAThe ,
            const double& ROAPhi ,
            const double& ROAR3d 
           );
   void AddROA(
            const double& Point_x ,
            const double& Point_y ,
            const double& Point_z ,
            const double& Direc_x ,
            const double& Direc_y ,
            const double& Direc_z ,
            const double& DeltPhi ,
            const double& DeltEta  
           );
   void AddROA(
            const double& Point_x ,
            const double& Point_y ,
            const double& Point_z ,
            const double& Direc_x ,
            const double& Direc_y ,
            const double& Direc_z 
           );

   /**Get number of MboyROAs */
   int NberOfObjects() const;

   /**Get a MboyROA */
   const MboyROA* GetMboyROA(int MboyROANber) const;
   
private:
///////////////////////////////////
//Data

   std::vector<MboyROA>  MboyROASet ; //!< Storage vector 

};
#endif
