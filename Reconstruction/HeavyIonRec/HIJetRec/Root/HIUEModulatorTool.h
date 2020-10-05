/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIJETREC_HIUEMODULATORTOOL_H__
#define __HIJETREC_HIUEMODULATORTOOL_H__

#include "HIJetRec/IHIUEModulatorTool.h"
#include "AsgTools/AsgTool.h"

#include "xAODHIEvent/HIEventShapeContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIUEModulatorTool : public asg::AsgTool, virtual public IHIUEModulatorTool
{
  ASG_TOOL_CLASS(HIUEModulatorTool,IHIUEModulatorTool)
public:

  HIUEModulatorTool(const std::string& myname);

  virtual float getModulation(float phi, const xAOD::HIEventShape* shape = nullptr) const override;
  virtual StatusCode setEventShapeForModulation(const xAOD::HIEventShape* shape) override;
  virtual void setHarmonics(const std::vector<unsigned int>& v) override;
  virtual StatusCode retrieveShape() const override;
  virtual StatusCode initialize() override;

  virtual StatusCode getShape(const xAOD::HIEventShape* & shape) const override;

  static float modulate(const std::vector<unsigned int>& nh_vector, const xAOD::HIEventShape* shape, float phi);

private:
  StatusCode checkQVectorSize(const xAOD::HIEventShape* shape, unsigned int n) const;
  StatusCode checkCompatibility() const;

  //Attempt to move to data handlers
  //std::string m_shape_key;
  //key attribute set to NULL to have consistency with previously set m_shape_key
  SG::ReadHandleKey< xAOD::HIEventShapeContainer > m_shape_key { this, "EventShapeKey", "NULL", "The input HI Event Shape"};

  const xAOD::HIEventShape* m_shape;
  std::vector<unsigned int> m_nh_vector;

  //bool m_do_v2;
  Gaudi::Property< bool > m_do_v2 { this, "DoV2", true, "Do V2 modulation" };
  //bool m_do_v3;
  Gaudi::Property< bool > m_do_v3 { this, "DoV3", true, "Do V3 modulation" };
  //bool m_do_v4;
  Gaudi::Property< bool > m_do_v4 { this, "DoV4", true, "Do V4 modulation" };

};
#endif
