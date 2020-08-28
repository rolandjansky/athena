/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file MHTPufitFex.cxx
 *
 * Implementation of mhtpufit fex class
 * @author Jon Burr
 *****************************************************************************/

#include "MHTPufitFex.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/PufitUtils.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "xAODBase/IParticleHelpers.h"

namespace
{
  const static SG::AuxElement::ConstAccessor<float> accArea("ActiveArea4vec_pt");
  const static SG::AuxElement::ConstAccessor<float> accDetectorEta("DetectorEta");
} // namespace

namespace HLT
{
  namespace MET
  {
    double MHTPufitFex::getSigma(const SignedKinematics &kin) const
    {
      return m_caloNoise * m_caloNoise + kin.absPt() * m_caloStoch * m_caloStoch;
    }

    MHTPufitFex::MHTPufitFex(const std::string &name, ISvcLocator *pSvcLocator)
        : FexBase(name, pSvcLocator)
    {
    }

    StatusCode MHTPufitFex::initialize()
    {
      CHECK(m_inputJetsKey.initialize());
      if (m_inputJvtKey.key().find(".") == std::string::npos)
        m_inputJvtKey = m_inputJetsKey.key() + "." + m_inputJvtKey.key();
      else if (SG::contKeyFromKey(m_inputJvtKey.key()) != m_inputJvtKey.key())
      {
        ATH_MSG_ERROR("Input JVT key does not match the input jet key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_inputJvtKey.initialize());

      CHECK(m_inputKey.initialize());

      CHECK(m_rhoKey.initialize(m_jetCalibIncludesAreaSub));

      return initializeBase({"UncorrJets"});
    }

    StatusCode MHTPufitFex::fillMET(
        xAOD::TrigMissingET &met,
        const EventContext &context,
        MonGroupBuilder &) const
    {
      using namespace PufitUtils;
      // Retrieve the input
      auto inputJets = SG::makeHandle(m_inputJetsKey, context);
      if (!inputJets.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputJetsKey);
        return StatusCode::FAILURE;
      }
      auto jvtAcc = SG::makeHandle<float>(m_inputJvtKey, context);
      auto inputs = SG::makeHandle(m_inputKey, context);
      if (!inputs.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputKey);
        return StatusCode::FAILURE;
      }

      double rho = 0;
      if (m_jetCalibIncludesAreaSub)
      {
        auto rhoHandle = SG::makeHandle(m_rhoKey, context);
        if (!rhoHandle->getDensity(xAOD::EventShape::Density, rho))
        {
          ATH_MSG_ERROR("EventShape density not available!");
          return StatusCode::FAILURE;
        }
      }

      // Create the grid
      PufitGrid grid(m_maxEta, m_nEtaBins, m_nPhiBins);
      CovarianceSum pileupSum;
      METComponent sum;
      METComponent uncorrSum;

      // Keep track of which clusters/PFOs we have decided are included in a HS jet
      std::set<const xAOD::IParticle *> hardScatterInputs;
      std::vector<SignedKinematics> hardScatterJets;
      std::vector<float> jetAreas;
      for (const xAOD::Jet *ijet : *inputJets)
      {
        float eta = m_useDetectorEta ? accDetectorEta(*ijet) : ijet->eta();
        if (std::abs(eta) > m_centralEta)
        {
          if (ijet->pt() < m_forwardPt)
            continue;
        }
        else
        {
          if (ijet->pt() < m_minPt)
            continue;
          else if (ijet->pt() < m_maxPt && jvtAcc(*ijet) < m_jvtCut)
            continue;
        }
        // If we're here then the jet has passed the selection
        hardScatterJets.push_back(*ijet);
        jetAreas.push_back(accArea(*ijet));
        for (const auto &link : ijet->constituentLinks())
        {
          if (link.isValid())
          {
            const xAOD::IParticle *constituent = *link;
            if (const xAOD::IParticle *original = xAOD::getOriginalObject(*constituent))
              hardScatterInputs.insert(original);
            else
              hardScatterInputs.insert(constituent);
          }
          else
            ATH_MSG_WARNING("Invalid constituent link!");
        }
      } //> end loop over jets

      if (hardScatterJets.size() > 0)
      {
        for (const xAOD::IParticle *ipart : *inputs)
        {
          const xAOD::IParticle *original = xAOD::getOriginalObject(*ipart);
          if (!original)
            original = ipart;
          bool isHS = hardScatterInputs.count(original);
          SignedKinematics kin = SignedKinematics::fromEnergyEtaPhi(
              ipart->e(), ipart->eta(), ipart->phi());
          if (!isHS)
            pileupSum.add(kin, getSigma(kin));

          // Figure out which tower this particle belongs in
          bool outOfRange = false;
          std::size_t idx = grid.getIndex(ipart->eta(), ipart->phi(), outOfRange);
          if (!outOfRange)
          {
            PufitGrid::Tower &tower = grid[idx];
            tower += kin;
            if (isHS)
              // mask a tower that has *any* clusters from HS jets in it
              tower.mask();
          }
        }

        // Calculate the expected PU contributions to each jet
        double towerMean = 0;
        double towerVariance = 0;
        unmaskedMeanAndVariance(grid, towerMean, towerVariance);
        float towerArea = grid.etaWidth() * grid.phiWidth();
        std::vector<double> means;
        std::vector<double> variances;
        means.reserve(jetAreas.size());
        variances.reserve(jetAreas.size());
        for (float area : jetAreas)
        {
          means.push_back(towerMean * area / towerArea);
          variances.push_back(towerVariance * area / towerArea);
        }

        std::vector<SignedKinematics> corrections = pufit(
            pileupSum.sum,
            pileupSum.covariance,
            means,
            variances,
            hardScatterJets,
            m_constraintImportance);

        for (std::size_t ii = 0; ii < hardScatterJets.size(); ++ii)
        {
          SignedKinematics kin = hardScatterJets.at(ii);
          uncorrSum += kin;
          if (m_jetCalibIncludesAreaSub)
            // Add back the energy that was subtracted by the rho sub part of the
            // calibration otherwise pufit will overcount this effect
            kin += SignedKinematics::fromEnergyEtaPhi(
                jetAreas.at(ii) * rho, kin.eta(), kin.phi());
          sum += kin;
          sum += corrections.at(ii);
        }
      }

      uncorrSum.fillMETComponent(0, met);
      sum.fillMET(met);

      return StatusCode::SUCCESS;
    }
  } // namespace MET
} // namespace HLT