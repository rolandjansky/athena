/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGXFMATERIALEFFECTS_H
#define TRKGXFMATERIALEFFECTS_H



namespace Trk{

  class MaterialEffectsOnTrack;
  class MaterialEffectsBase;
  class EnergyLoss;
  class Surface;
  class MaterialProperties;

  /**
   * @brief class that is similar to MaterialEffectsOnTrack, but has 'set' methods for more flexibility during track fitting

   @author thijs.cornelissen@cern.ch
  */

  class GXFMaterialEffects{

    public:
     GXFMaterialEffects();
     GXFMaterialEffects(const MaterialEffectsOnTrack *);
     GXFMaterialEffects(GXFMaterialEffects&);
     ~GXFMaterialEffects();
     GXFMaterialEffects& operator=(GXFMaterialEffects& rhs);

     void setScatteringAngles(double,double);
     void setScatteringSigmas(double,double);
     double x0();
     void setX0(double);
     double deltaPhi();
     double measuredDeltaPhi();
     void setMeasuredDeltaPhi(double);
     double deltaTheta();
     double sigmaDeltaPhi();
     double sigmaDeltaTheta();
     double deltaE();
     void setEloss(EnergyLoss*);
     double sigmaDeltaE();
     double sigmaDeltaEPos();
     double sigmaDeltaENeg();
     double sigmaDeltaEAve();
     void setDeltaE(double);
     void setSigmaDeltaE(double);
     void setSigmaDeltaEPos(double);
     void setSigmaDeltaENeg(double);
     double delta_p();
     void setdelta_p(double);
     void setKink(bool);
     bool isKink();
     void setMeasuredEloss(bool);
     bool isMeasuredEloss();
     const Surface *surface();
     void setSurface(const Surface*);
     MaterialEffectsBase *makeMEOT();
     const MaterialProperties *materialProperties();
     void setMaterialProperties(const MaterialProperties *);
     //std::vector<double> &momentumJacobians();    

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
     bool m_owneloss;
     double m_measscatphi; // fudge to stabilize fit in muon system
     double m_sintheta;
     //std::vector<double> m_pjac;
  };

}

#endif
