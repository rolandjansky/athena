/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetSubtractorToolBase.h

#ifndef HIJETREC_HIJETSUBTRACTORTOOLBASE_H
#define HIJETREC_HIJETSUBTRACTORTOOLBASE_H

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetSubtractorToolBase
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Abstract base for JetModifiers applying constituent based subtraction.
///
////////////////////////////////////////////////////////////////////////////////

#include "HIJetRec/IHISubtractorTool.h"
#include "AsgTools/AsgTool.h"

class HIJetSubtractorToolBase : public asg::AsgTool, virtual public IHISubtractorTool
{
  ASG_TOOL_CLASS(HIJetSubtractorToolBase, IHISubtractorTool)
public:

  HIJetSubtractorToolBase(const std::string& myname);
  virtual ~HIJetSubtractorToolBase(){};
  inline float minEnergyForMoments() const {return m_EminMoment;};
  inline float minEnergySigForMoments() const {return m_EsigMoment;};
  inline bool usesCells() const {return m_useCells;};


  inline const xAOD::HIEventShapeContainer* getShape() const {return m_shape;};
  inline const HIEventShapeIndex* getIndex() const {return m_index;};
  inline const IHIUEModulatorTool* getModulator() const {return m_modulator;};
  inline void setShape(const xAOD::HIEventShapeContainer* s) {m_shape=s;};
  inline void setIndex(const HIEventShapeIndex* ind) {m_index=ind;};
  inline void setModulator(const IHIUEModulatorTool* mod) {m_modulator=mod;};

  virtual StatusCode configureEvent(const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const IHIUEModulatorTool* modulator);

private:

  Gaudi::Property< float > m_EminMoment { this, "MinimumEnergyForMoments", 50., "> E, cluster given tower coordinates" };
  Gaudi::Property< float > m_EsigMoment { this, "MinimumSignificanceForMoments", 0.1, "if E after subtr / E total < this cut, cluster given tower coordinates" };
  Gaudi::Property< bool > m_updateClusters { this, "UpdateClusters", false, "If true set cluster kinematics to reflect subtraction" };
  Gaudi::Property< bool > m_useCells { this, "UseCells", true, "Boolean switch for cells usage" };

  const xAOD::HIEventShapeContainer* m_shape;
  const HIEventShapeIndex* m_index;
  const IHIUEModulatorTool* m_modulator;

protected:

  inline void setMinEnergyForMoment(float min_E) { m_EminMoment=min_E; };
  inline void setUpdateClusters(bool up) { m_updateClusters=up; };

  inline bool UpdateClusters() const { return m_updateClusters; };

  void setSubtractedEtaPhi(float E, float& eta, float& phi, float eta0, float phi0, float sig) const;

  inline void setUseCells(bool v) {m_useCells=v;};

};

#endif
