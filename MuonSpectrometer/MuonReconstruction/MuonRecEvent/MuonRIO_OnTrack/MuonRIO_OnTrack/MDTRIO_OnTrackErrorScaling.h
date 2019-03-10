/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MDTRIO_OnTrackErrorScaling_H_
#define _MDTRIO_OnTrackErrorScaling_H_

#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "TrkEventPrimitives/ParamDefs.h"

class MDTRIO_OnTrackErrorScaling : public RIO_OnTrackErrorScaling
{
public:
  virtual CLID clid() const override;

  virtual Amg::MatrixX getScaledCovariance(const Amg::MatrixX& cov_input,
                                           bool is_endcap) const;

  enum EMDTErrorScalingRegions {
    kBarrel,
    kEndcap,
    kNParamTypes};

  static const char* const* paramNames() { return s_names; }
  virtual bool postProcess() override;
protected:
  static const char* const s_names[kNParamTypes];
};

CLASS_DEF( MDTRIO_OnTrackErrorScaling ,  100992927, 1 )
CONDCONT_DEF(MDTRIO_OnTrackErrorScaling, 213408417, RIO_OnTrackErrorScaling);
#endif
