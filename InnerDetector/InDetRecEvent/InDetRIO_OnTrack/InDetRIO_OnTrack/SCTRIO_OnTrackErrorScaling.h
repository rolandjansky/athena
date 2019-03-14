/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _SCTRIO_OnTrackErrorScaling_H_
#define _SCTRIO_OnTrackErrorScaling_H_

#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
class SCTRIO_OnTrackErrorScaling : public RIO_OnTrackErrorScaling
{
public:
  virtual CLID clid() const override;

  virtual Amg::MatrixX getScaledCovariance(const Amg::MatrixX& cov_input,
                                           bool is_endcap,
                                           double sinLocalAngle) const;

  enum ESCTErrorScalingRegions {
    kBarrel,
    kEndcap,
    kNParamTypes};

  static const char* const* paramNames() { return s_names; }
  virtual bool postProcess() override;
protected:
  static const char* const s_names[kNParamTypes];
};


CLASS_DEF( SCTRIO_OnTrackErrorScaling ,          14458362, 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(SCTRIO_OnTrackErrorScaling, 6837094, RIO_OnTrackErrorScaling);

#endif
