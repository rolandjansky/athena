/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrtAtlas.h
//
#ifndef _TrkVKalVrtFitter_VKalVrtAtlas_H
#define _TrkVKalVrtFitter_VKalVrtAtlas_H

// Mag field service
#include  "MagFieldInterfaces/IMagFieldSvc.h"
#include  "TrkVKalVrtCore/VKalVrtBMag.h"
// External propagator
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkExInterfaces/IExtrapolator.h"

namespace MagField{
   class IMagFieldSvc;
}

namespace Trk{

//  ATLAS magnetic field access for TrkVKalVrtFitter
//-----------------------------------------------------
  class VKalAtlasMagFld : public Trk::baseMagFld 
   {
    public:

       VKalAtlasMagFld();
      ~VKalAtlasMagFld();
       void getMagFld(const double,const double,const double,double&,double&,double&);
       void setAtlasMag(MagField::IMagFieldSvc *);
       void setAtlasMag(const double );
       void setAtlasMagRefFrame( double, double, double );

    private:
    
       MagField::IMagFieldSvc*  m_VKalAthenaField;
       double m_FIXED_ATLAS_FIELD;
       const double m_mm;
       double m_magFrameX, m_magFrameY, m_magFrameZ ;
       double m_saveXpos, m_saveYpos, m_saveZpos;
       double m_saveBX, m_saveBY, m_saveBZ;   
       Amg::Vector3D m_Point;
       

   };


class StraightLineSurface;

//  External propagator access for TrkVKalVrtCore
//-----------------------------------------------------
  class VKalExtPropagator : public Trk::basePropagator 
   {
    public:

        VKalExtPropagator( TrkVKalVrtFitter* );
       ~VKalExtPropagator();

//
// Propagator from RefStart point  to RefEnd point in local coordinate
//   system. Global coordinates are incapsulated inside function
//
        void Propagate(long int trkID, long int Charge, 
	               double *ParOld, double *CovOld, double *RefStart, 
                       double *RefEnd, double *ParNew, double *CovNew) const;
        bool checkTarget( double *) const;


        void setPropagator(const IExtrapolator* );

        const TrackParameters* myExtrapWithMatUpdate(long int TrkID, 
                                                     const TrackParameters *inpPer,
                                                     Amg::Vector3D *endPoint) const;
        const TrackParameters* myExtrapToLine(long int TrkID, 
                                                     const TrackParameters *inpPer,
                                                     Amg::Vector3D * endPoint,
                                                     StraightLineSurface  &lineTarget) const;
        const NeutralParameters* myExtrapNeutral( const NeutralParameters *inpPer,
                                                     Amg::Vector3D *endPoint) const;

        const Perigee* myxAODFstPntOnTrk(const xAOD::TrackParticle* xprt) const;
	
    private:
    
        const IExtrapolator     *m_extrapolator;       //!< Pointer to Extrapolator AlgTool
        TrkVKalVrtFitter            *m_vkalFitSvc;        //!< Pointer to TrkVKalVrtFitter

        double Protection(double *) const;


   };

}

#endif
