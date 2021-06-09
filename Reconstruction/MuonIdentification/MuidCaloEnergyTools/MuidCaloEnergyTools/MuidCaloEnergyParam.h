/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class MuidCaloEnergyParam
 AlgTool returning the parametrized value for the calorimeter energy
 deposited by a traversing muon. The energy is parametrized according
 to trackParameters given at the calo mid-surface.

  @author Konstantinos.Nikolopoulos@cern.ch, Alan.Poppleton@cern.ch
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOENERGYTOOLS_MUIDCALOENERGYPARAM_H
#define MUIDCALOENERGYTOOLS_MUIDCALOENERGYPARAM_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class CaloEnergy;
namespace Rec {

    class MuidCaloEnergyParam : public AthAlgTool, virtual public IMuidCaloEnergyParam {
    public:
        MuidCaloEnergyParam(const std::string& type, const std::string& name, const IInterface* parent);
        ~MuidCaloEnergyParam(void);  // destructor

        StatusCode initialize();

        // mean muon energy loss with asymmetric errors describing the whole distribution
        CaloEnergy* meanParametrizedEnergy(double trackMomentum, double eta, double phi) const;

        // most probable muon energy loss with asymmetric errors describing the whole distribution
        CaloEnergy* mopParametrizedEnergy(double trackMomentum, double eta, double phi) const;

        // most probable muon energy loss with asymmetric errors describing the peak region
        CaloEnergy* mopPeakEnergy(double trackMomentum, double eta, double phi) const;

        // returns the Mop deposited energy with its error
        CaloEnergy* mopDepositedEnergy(double trackMomentum, double eta, double phi) const;
        // returns the percentage of inert material
        double x0mapInertMaterial(double) const;
        // returns the percentage of em calorimeter material
        double x0mapEmMaterial(double) const;
        // returns the percentage of hec calorimeter material
        double x0mapHecMaterial(double) const;
        // returns the relative depth of a compartment in subcalo
        double caloCompartmentDepth(int, int) const;
        // returns the fraction of the full mop deposition corresponding to the em
        double emMopFraction(double eta) const;
        // fraction of the full mop deposition corresponding to the em preshower + compartment #1
        double em1MopFraction(double eta) const;

    private:
        // private methods

        // local communication for linear eta interpolation (bins and weights)
        struct BinsWeights {
            int etaBin1;
            int etaBin2;
            double etaWeight1;
            double etaWeight2;
        };

        BinsWeights etaFixedBin(double) const;
        BinsWeights etaVariableBin(double) const;
        double meanEnergyLoss(double, double) const;
        std::pair<double, double> meanEnergyLossError(double, double) const;
        double mopEnergyLoss(double, double) const;
        double mopEnergyLossError(double, double) const;
        double symmetricMopEnergyLoss(double, double) const;
        // returns the approx. material percentage of the ith compartment
        double caloCompartmentDepthTile(int) const;
        // returns the approx. material percentage of the ith compartment
        double caloCompartmentDepthLArHEC(int) const;
        // returns the approx. material percentage of the ith compartment
        double caloCompartmentDepthLArEM(int) const;

        // configuration
        bool m_cosmics;
        double m_smoothingFraction;

        // precomputed widths etc for eta binning and smoothing
        double m_binWidth;
        double m_etaOffset;
        double m_inverseWidth;

        // ============ Mean parametrization coefficients ============= //
        double m_meanEnergyLossP0[26]{};
        double m_meanEnergyLossP1[26]{};
        double m_meanEnergyLossP2[26]{};
        double m_meanEnergyLossErrorLeftP0[26]{};
        double m_meanEnergyLossErrorLeftP1[26]{};
        double m_meanEnergyLossErrorRightP0[26]{};
        double m_meanEnergyLossErrorRightP1[26]{};
        // ============ Mop parametrization coefficients ============= //
        double m_mopEnergyLossP0[26]{};
        double m_mopEnergyLossP1[26]{};
        double m_mopEnergyLossP2[26]{};
        double m_mopEnergyLossErrorP0[26]{};
        double m_mopEnergyLossErrorP1[26]{};
        // ============ Symmetric Mop parametrization coefficients ==== //
        double m_mopSymmetricEnergyLossP0[26]{};
        double m_mopSymmetricEnergyLossP1[26]{};
        double m_mopSymmetricEnergyLossP2[26]{};
        // ==================== fractional contribution from em calo === //
        double m_emMopFraction[26]{};
        // double			m_em1MopFraction[26];
        // ==================== Material map =========================== //
        double m_etaGranularity;      // Granularity in eta of material map
        double m_dead_eta[277]{};     // eta of bin
        double m_dead_x01[277]{};     // material in front of the e/m calorimeter
        double m_dead_x02[277]{};     // material in front of hadronic calorimeter
        double m_dead_x0tot[277]{};   // material in front of Muon Spectrometer
        double m_dead_cryo0[277]{};   // Lar front cryostat (included in m_dead_x01)
        double m_dead_cryo1[277]{};   // LarEm back cryostat
        double m_dead_cryo2[277]{};   // LarHec back cryostat
        double m_dead_girder[277]{};  // Tile calorimetergirder structure
    };

}  // namespace Rec

#endif  // MUIDCALOENERGYTOOLS_MUIDCALOENERGYPARAM_H
