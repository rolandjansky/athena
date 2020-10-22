/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H
#define SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17  updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

// Athena
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "PathResolver/PathResolver.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Random number
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat 
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Units/SystemOfUnits.h"

// C++ Standard Library
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

class SCT_InducedChargeModel {

 public:
  enum EFieldModel {FlatDiodeModel=0, FEMsolutions=1, UniformE=2};
  enum TransportStep {NTransportSteps=100};
  enum FEMNums {NRamoPoints=81, NEFieldPoints=17, NDepthPoints=115};
  enum InducedStrips {StartStrip=-2, EndStrip=+2, Offset=-StartStrip, NStrips=EndStrip+Offset+1};

  struct SCT_InducedChargeModelData {
    float m_VD; // full depletion voltage [Volt] negative for type-P
    float m_VB; // applied bias voltage [Volt]
    float m_T; // temperature
    float m_depletion_depth;
    Amg::Vector3D m_magneticField;
    EFieldModel m_EFieldModel;
    const InDetDD::SiDetectorElement* m_element;
    std::array<std::array<double, NDepthPoints>, NEFieldPoints> m_ExValue;
    std::array<std::array<double, NDepthPoints>, NEFieldPoints> m_EyValue;

    SCT_InducedChargeModelData(const float vdepl,
                               const float vbias,
                               const InDetDD::SiDetectorElement* element,
                               const Amg::Vector3D& magneticField, // in kTesla
                               const float bulk_depth,
                               const EFieldModel model,
                               const ServiceHandle<ISiliconConditionsSvc> siConditionsSvc) {
      m_VD = vdepl; // full depletion voltage [Volt] negative for type-P
      m_VB = vbias; // applied bias voltage [Volt]
      m_element = element;
      m_magneticField = magneticField;

      //------------ find delepletion deph for model=0 and 1 -------------
      m_depletion_depth = bulk_depth;
      // for type N (before type inversion)
      if (m_VD >= 0.) {
        if (m_VB < m_VD) m_depletion_depth = std::sqrt(m_VB/m_VD) * bulk_depth;
      } else {
        // for type P
        if (m_VB <= std::abs(m_VD)) m_depletion_depth = 0.;
      }

      m_EFieldModel = model;
      m_T = siConditionsSvc->temperature(m_element->identifyHash()) + Gaudi::Units::STP_Temperature;
    }
  };

  SCT_InducedChargeModel(size_t maxHash, EFieldModel model=FEMsolutions);

  SCT_InducedChargeModelData*
    setWaferData(const float vdepl,
                 const float vbias,
                 const InDetDD::SiDetectorElement* element,
                 const ServiceHandle<MagField::IMagFieldSvc> magFieldSvc,
                 const ServiceHandle<ISiliconConditionsSvc> siConditionsSvc) const;
  void setRandomEngine(CLHEP::HepRandomEngine* rndmEngine) { m_rndmEngine = rndmEngine; };

  void setEField(SCT_InducedChargeModelData& data) const;

  void transport(const SCT_InducedChargeModelData& data,
                 const bool isElectron,
                 const double x0, const double y0,
                 double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                 const IdentifierHash hashId,
                 const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const;
  void holeTransport(const SCT_InducedChargeModelData& data,
                     const double x0, const double y0,
                     double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                     const IdentifierHash hashId,
                     const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const;
  void electronTransport(const SCT_InducedChargeModelData& data,
                         const double x0, const double y0,
                         double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                         const IdentifierHash hashId,
                         const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const;

 private:
 
  void loadICMParameters();

  bool getVxVyD(const SCT_InducedChargeModelData& data,
                const bool isElectron,
                const double x, const double y, double& vx, double& vy, double& D,
                const IdentifierHash hashId,
                const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const;
  double induced(const SCT_InducedChargeModelData& data,
                 const int istrip, const double x, const double y) const;
  void getEField(const SCT_InducedChargeModelData& data,
                 const double x, const double y, double& Ex, double& Ey) const;

  size_t getFEMIndex(SCT_InducedChargeModelData& data) const;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl(MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

  //-------- parameters for e, h transport --------------------------------
  static const double s_kB; // [m^2*kg/s^2/K]
  static const double s_e; // [Coulomb]

  // Private message stream member
  mutable Athena::MsgStreamMember m_msg;
  
  CLHEP::HepRandomEngine* m_rndmEngine; //!< Random number generation engine 

  //------parameters given externally by jobOptions ------------------
  EFieldModel m_EFieldModel; // 0 (flat diode model), 1 (FEM solusions), 2 (uniform E)
  double m_transportTimeStep = 0.50; // one step side in time [nsec]
  double m_transportTimeMax = m_transportTimeStep*NTransportSteps; // maximun tracing time [nsec]

  //------parameters mostly fixed but can be changed externally  ------------
  double m_bulk_depth =  0.0285; // in [cm]
  double m_strip_pitch = 0.0080; // in [cm]
  double m_y_origin_min = 0.;

  //---------- arrays of FEM analysis -----------------------------------
  // Storages for Ramo potential and Electric field.
  // In strip pitch directions :
  //  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
  //  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
  // In sensor depth directions (for both potential and Electric field):
  //  114 divisions (115 points) with 2.5 nm intervals for 285 um.

  std::vector<std::array<std::array<double, NDepthPoints>, NRamoPoints>> m_PotentialValue;
  std::vector<std::array<std::array<double, NDepthPoints>, NEFieldPoints>> m_ExValue;
  std::vector<std::array<std::array<double, NDepthPoints>, NEFieldPoints>> m_EyValue;

  // Cache of SCT_InducedChargeModelData for each wafer.
  // Assuming wafer parameters do not change during a job.
  // This should be almost always satisfied.
  mutable std::vector<std::unique_ptr<SCT_InducedChargeModelData>> m_data;
  // To protect concurrent access to m_data
  mutable std::mutex m_mutex;

  static const std::vector<float> s_VFD0;
  static const std::vector<std::string> s_VFD0str;
};

#endif // SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H
