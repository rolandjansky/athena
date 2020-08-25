/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrtAtlas.h
//
#ifndef TRKVKALVRTFITTER_VKALVRTATLAS_H
#define TRKVKALVRTFITTER_VKALVRTATLAS_H

// Mag field service
#include  "TrkVKalVrtCore/VKalVrtBMag.h"
#include "MagFieldElements/AtlasFieldCache.h"
// External propagator
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include  "TrkVKalVrtFitter/ITrkVKalVrtFitter.h"


namespace Trk{

class TrkVKalVrtFitter;


//  ATLAS magnetic field access for TrkVKalVrtFitter
//-----------------------------------------------------
  class VKalAtlasMagFld : public Trk::baseMagFld
   {
    public:

       VKalAtlasMagFld();
      ~VKalAtlasMagFld();
       void getMagFld(const double,const double,const double,double&,double&,double&)const;
       void setAtlasField(MagField::AtlasFieldCache *);
       void setAtlasField(const double );
       void setAtlasMagRefFrame( double, double, double );

    private:

       MagField::AtlasFieldCache * m_VKalAthenaField;
       double m_FIXED_ATLAS_FIELD=1.997;
       double m_magFrameX, m_magFrameY, m_magFrameZ ;

   };


class StraightLineSurface;

//  External propagator access for TrkVKalVrtCore
//-----------------------------------------------------
  class VKalExtPropagator : public Trk::basePropagator
   {
    public:

        VKalExtPropagator( TrkVKalVrtFitter* );
        virtual ~VKalExtPropagator();

//
// Propagator from RefStart point  to RefEnd point in local coordinate
//   system. Global coordinates are incapsulated inside function
//
        virtual
        void Propagate(long int trkID, long int Charge,
	               double *ParOld, double *CovOld, double *RefStart,
                       double *RefEnd, double *ParNew, double *CovNew,
                       const IVKalState& istate) const override;
        virtual
        bool checkTarget( double *,
                          const IVKalState& istate) const override;


        void setPropagator(const IExtrapolator* );

        const TrackParameters* myExtrapWithMatUpdate(long int TrkID,
                                                     const TrackParameters *inpPer,
                                                     Amg::Vector3D *endPoint,
                                                     const IVKalState& istate) const;
        const TrackParameters* myExtrapToLine(long int TrkID,
                                                     const TrackParameters *inpPer,
                                                     Amg::Vector3D * endPoint,
                                                     StraightLineSurface  &lineTarget,
                                                     const IVKalState& istate) const;
        const NeutralParameters* myExtrapNeutral( const NeutralParameters *inpPer,
                                                     Amg::Vector3D *endPoint) const;

        const Perigee* myxAODFstPntOnTrk(const xAOD::TrackParticle* xprt) const;

    private:

        const IExtrapolator     *m_extrapolator;       //!< Pointer to Extrapolator AlgTool
        TrkVKalVrtFitter            *m_vkalFitSvc;        //!< Pointer to TrkVKalVrtFitter

        double Protection(double *, const IVKalState& istate) const;


   };

}

#endif
