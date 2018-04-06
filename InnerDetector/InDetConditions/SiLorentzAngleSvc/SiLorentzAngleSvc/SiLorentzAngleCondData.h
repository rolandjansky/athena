/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiLorentzAngleCondData.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef SiLorentzAngleCondData_h
#define SiLorentzAngleCondData_h

#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

/**
 * @class SiLorentzAngleCondData
 * Concrete class for Lorentz angle conditions data
 * for each detector element.
**/

class SiLorentzAngleCondData {
public:
  // Constructor
  SiLorentzAngleCondData();
  // Destructor
  virtual ~SiLorentzAngleCondData() = default;

  /** Reszie the data members */
  void resize(const unsigned int size);

  /** Get the Lorentz shift correction in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  double getLorentzShift(const IdentifierHash& elementHash) const;

  /** Get the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  double getLorentzShiftEta(const IdentifierHash& elementHash) const;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  double getTanLorentzAngle(const IdentifierHash& elementHash) const;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  double getTanLorentzAngleEta(const IdentifierHash& elementHash) const;

  /** Get bias voltage */
  double getBiasVoltage(const IdentifierHash& elementHash) const;

  /** Get temperature */
  double getTemperature(const IdentifierHash& elementHash) const;

  /** Get depletion voltage */
  double getDepletionVoltage(const IdentifierHash& elementHash) const;

  /** Get correction factor */
  double getCorrectionFactor() const;

  /** Set the Lorentz shift correction in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  void setLorentzShift(const IdentifierHash& elementHash, const double lorentzShift);

  /** Set the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  void setLorentzShiftEta(const IdentifierHash& elementHash, const double lorentzShiftEta);

  /** Set tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  void setTanLorentzAngle(const IdentifierHash& elementHash, const double tanLorentzAngle);

  /** Set tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  void setTanLorentzAngleEta(const IdentifierHash& elementHash, const double tanLorentzAngleEta);

  /** Set bias voltage */
  void setBiasVoltage(const IdentifierHash& elementHash, const double biasVoltage);

  /** Set temperature */
  void setTemperature(const IdentifierHash& elementHash, const double temperature);

  /** Set depletion voltage */
  void setDepletionVoltage(const IdentifierHash& elementHash, const double depletionVoltage);

  /** Set correction factor */
  void setCorrectionFactor(const double correctionFactor);

private:
  std::vector<double> m_lorentzShift;
  std::vector<double> m_lorentzShiftEta;
  std::vector<double> m_tanLorentzAngle;
  std::vector<double> m_tanLorentzAngleEta;
  std::vector<double> m_monitorBiasVoltage;
  std::vector<double> m_monitorTemperature;
  std::vector<double> m_monitorDepletionVoltage;
  double m_correctionFactor;

  static const double s_invalidValue;
};

CLASS_DEF(SiLorentzAngleCondData, 91712959, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(SiLorentzAngleCondData, 150548945);

#endif // SiLorentzAngleCondData_h
