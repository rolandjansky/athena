/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_Propagator_H
#define _TrkVKalVrtCore_Propagator_H
//#include "TrkVKalVrtCore/TrkVKalVrtCore.h"

namespace Trk {

/*    Class for track propagation to any point       */
/*---------------------------------------------------*/

  typedef void (*addrPropagator)(long int ,long int, double*, double*, double*, double*, double*, double* );

  class VKTrack;

  class basePropagator {
  public:
     basePropagator();
     virtual ~basePropagator();
//
// Propagator from RefStart point to RefEnd point
//     ParOld[5] - track perigee parameters with respect to RefStart point
//     ParNew[5] - track perigee parameters with respect to RefEnd point
     virtual void Propagate(long int TrkID, long int Charge, 
                            double *ParOld, double *CovOld, double *RefStart, 
                            double *RefEnd, double *ParNew, double *CovNew) const =0;
     virtual bool checkTarget(double *RefEnd) const =0;
//
   };

  class vkalPropagator {
  public:
     vkalPropagator();
    ~vkalPropagator();


     void Propagate(long int TrkID, long int Charge, 
                    double *ParOld, double *CovOld, double *RefStart, 
                    double *RefEnd, double *ParNew, double *CovNew) const;
     bool checkTarget(double *RefEnd) const;
     void Propagate(VKTrack *trk, double *RefStart, 
                    double *RefEnd, double *ParNew, double *CovNew) const;
     void setPropagator(addrPropagator ); 
     void setPropagator(basePropagator*); 
     void setTypeProp(int); 
  
   private:
     int m_typePropagator;
     addrPropagator   m_functionProp; 
     basePropagator*  m_objectProp;
   };

}
#endif
