/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPROTOMATERIALEFFECTS_H
#define TRKPROTOMATERIALEFFECTS_H

#include <vector>
#include "CxxUtils/checker_macros.h"
#include <memory>
namespace Trk{

  class MaterialEffectsOnTrack;
  class MaterialEffectsBase;
  class EnergyLoss;
  class Surface;
  class MaterialProperties;

  /**
   * @brief class that is similar to MaterialEffectsOnTrack, but has 'set' methods for more flexibility during track fitting

   @author thijs.cornelissen@cern.ch, sebastian.fleischmann@cern.ch
  */

  class ProtoMaterialEffects{

    public:
     ProtoMaterialEffects();
     ProtoMaterialEffects(const MaterialEffectsOnTrack *);
     ProtoMaterialEffects(const ProtoMaterialEffects&);
     ~ProtoMaterialEffects();
     ProtoMaterialEffects& operator=(const ProtoMaterialEffects& rhs);

     void setScatteringAngles(double,double);
     void setScatteringSigmas(double,double);
     double x0() const;
     void setX0(double);
     double deltaPhi() const;
     double measuredDeltaPhi() const;
     void setMeasuredDeltaPhi(double);
     double deltaTheta() const;
     double sigmaDeltaPhi() const;
     double sigmaDeltaTheta() const;
     double deltaE() const;
     void setEloss(EnergyLoss*);
     double sigmaDeltaE() const;
     double sigmaDeltaEPos() const;
     double sigmaDeltaENeg() const;
     double sigmaDeltaEAve() const;
     void setDeltaE(double);
     void setSigmaDeltaE(double);
     void setSigmaDeltaEPos(double);
     void setSigmaDeltaENeg(double);
     double delta_p() const;
     void setdelta_p(double);
     void setKink(bool);
     bool isKink() const;
     void setMeasuredEloss(bool);
     bool isMeasuredEloss() const;
     const Surface *surface() const;
     void setSurface(const Surface*);
     MaterialEffectsBase *makeMEOT ATLAS_NOT_THREAD_SAFE() const;
     std::unique_ptr<MaterialEffectsBase> makeUniqueMEOT() const;
     const MaterialProperties *materialProperties() const;
     void setMaterialProperties(const MaterialProperties *);

    private:
     double m_scatphi;
     double m_scattheta;
     double m_sigmascatphi;
     double m_sigmascattheta;
     double m_x0;
     double m_deltap;
     double m_deltae;
     double m_sigmadeltae;
     double m_sigmadeltaepos;
     double m_sigmadeltaeneg;
     const EnergyLoss *m_eloss;
     const Surface *m_surf;
     const MaterialProperties *m_matprop;
     bool m_iskink;
     bool m_ismeasuredeloss;
     bool m_owneloss; //This the main MT issue in this package
     double m_measscatphi; // fudge to stabilize fit in muon system
     double m_sintheta;
  };

}

#endif //TRKPROTOMATERIALEFFECTS_H
