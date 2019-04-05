/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_Process_h
#define TRT_PAI_Process_h

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TRT_PAI_Process/ITRT_PAITool.h"
#include "TRT_PAI_gasMixture.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>
#include <string>

namespace CLHEP {
  class HepRandomEngine;
}

/**
 * @brief The Photon Absorption Ionisation model used for the ionisation
 *        in the TRT.
 *
 * The main reference for this implementation is
 * - V.M.Grishin, V.K.Ermilova, and S.K.Kotelnikov, NIM A307 (1991) 273-278.
 *
 * Other references to the PAI model are
 * - V.C.Ermilova, L.P.Kotenko, and G.I.Merzon, NIM145 (1977) 555-563.
 * - J.H.Cobb, W.W.M.Allison, and J.N.Bunch, NIM133 (1976) 315-323.
 * - W.W.M.Allison and J.H.Cobb, Ann.Rev.Nucl.Part.Sci (1980), 253-98.
 *
 * @note Internally this class uses the original units of the fortran version.
 *       To external world sees CLHEP units.
 *
 * @author Originally implemented in age FORTRAN by Pavel Nevski.
 * Adapted to C++ by T.Kittelmann/Mogens Dam.
 */

class TRT_PAI_Process final : public extends<AthAlgTool, ITRT_PAITool> {

 public:

  /**
   * Not much action. Initializes a few variables
   */
  TRT_PAI_Process( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );
  /**
   * Initialization of the PAI model:
   * -# Construct chemical elements;
   * -# Construct gas components (molecules) from elements;
   * -# Construct gas mixture from gas components;
   * -# Construct "effective gas" from the gas mixture, i.e. calculate
   *    everything necessary for the current gas mixture to do the PAI
   *    simulation
   */
  virtual StatusCode initialize() override final;

  virtual StatusCode finalize() override final;

  /**
   * Get the mean free path in gas (CLHEP units)
   * @param scaledKineticEnergy: The kinetic energy a proton would have
   *        had, if it had same Lorentz gamma factor as the particle in
   *        question
   * @param squaredCharge: Charge squared
   * @return Mean free path
   */
  virtual double GetMeanFreePath(double scaledKineticEnergy,
                         double squaredCharge) const override final;
  /**
   * Get the energy transferred from the charged particle to the gas
   * (CLHEP units). The energy transfer is randomly generated according
   * to the distributions calculated based on the input data in gasdata.h
   *
   * @param scaledKineticEnergy: The kinetic energy a proton would have
   *        had, if it had same Lorentz gamma factor as the particle in
   *        question.
   * @return Random energy transfer consistent with PAI model calculation.
   */
  virtual double GetEnergyTransfer(double scaledKineticEnergy, CLHEP::HepRandomEngine *rndmEngine) const override final;

 private:
  // Constants that define the span and granularity of the tables:
  const unsigned int m_nTabulatedGammaValues{56};
  const double m_gamExpMin{-2.};    // Min Lorentz gamma: 1+10^(m_gamExpMin)
  const double m_gamExpMax{5.};    // Max Lorentz gamma: 1+10^(m_gamExpMax)
  const double m_deltaGamExp{};

  // Arrays to hold tables:
  std::vector<float>                m_en_array;
  std::vector< std::vector<float> > m_fn_array;
  std::vector<float>                m_dndx;
  TRT_PAI_gasMixture *m_trtgas{};


  /**
   * Converting Lorentz gamma to table index (well, double)
   */
  double ScaledEkin2GamVarTab(double scaledKineticEnergy) const;

  std::string m_gasType{"Auto"};

};

#endif
