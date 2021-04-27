/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeanTrackParsAtFloor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "InDetDNASeparator/MeanTrackParsAtFloor.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

//================ Constructor ===============================================
Trk::MeanTrackParsAtFloor::MeanTrackParsAtFloor()
  :
  nen(0),
  ras(0.0),
  phs(0.0),
  ths(0.0),
  pts(0.0),
  dps(0.0),
  nnf(0),
  dnf(0.0),
  nnb(0),
  dnb(0.0)
{}

void Trk::MeanTrackParsAtFloor::addTrackParsToFloor(const Trk::TrackParameters& tpars,
                                                    const Trk::DNA_MaterialEffects*  internalBrem,
                                                    const Trk::EstimatedBremOnTrack* officialBrem)
{
  // update average using computer-friendly calculation of mean (cf Barlow)
  ++nen;
  ras  += (tpars.position().perp()        - ras)/double(nen);
  phs  += (tpars.parameters()[Trk::phi0]  - phs)/double(nen);
  ths  += (tpars.parameters()[Trk::theta] - ths)/double(nen);
  pts  += (sin(tpars.parameters()[Trk::theta])/tpars.parameters()[Trk::qOverP]
                                          - pts)/double(nen);
  //const Trk::MeasuredTrackParameters* mpars = 
  //  dynamic_cast<const Trk::MeasuredTrackParameters*>(&tpars);
  //if (mpars) {
  const AmgSymMatrix(5)* merr = tpars.covariance();
  if(merr){
    double dpopb = Amg::error(*merr,Trk::qOverP)
      /fabs(tpars.parameters()[Trk::qOverP]);
    dps  += (dpopb - dps)/double(nen);
  }

  if (internalBrem != nullptr) {

    if (internalBrem->foundByForwardFilter()) {
      ++nnf;
      dnf += (internalBrem->addSigmaQoverP()/fabs(tpars.parameters()[Trk::qOverP]) - dnf)
        /double(nnf);
    } else {
      ++nnb;
      dnb += (internalBrem->addSigmaQoverP()/fabs(tpars.parameters()[Trk::qOverP]) - dnb)
        /double(nnb);
    }
  } else if (officialBrem != nullptr) {

    if (officialBrem->searchDirection() == Trk::outside) {
      ++nnf;
      dnf += (sqrt(officialBrem->sigmaQoverPsquared())/fabs(tpars.parameters()[Trk::qOverP])
              - dnf)/double(nnf);
    } else {
      ++nnb;
      dnb += (sqrt(officialBrem->sigmaQoverPsquared())/fabs(tpars.parameters()[Trk::qOverP])
              - dnb)/double(nnb);
    }
  }

  // possibly add history+management info: vector<TP*> or vector<iPos>

}

