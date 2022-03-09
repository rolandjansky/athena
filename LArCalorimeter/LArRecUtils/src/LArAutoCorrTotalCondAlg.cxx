/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAutoCorrTotalCondAlg.h"
#include "LArRawConditions/LArAutoCorrTotal.h"

#include "LArElecCalib/LArConditionsException.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "LArRawConditions/LArAutoCorrComplete.h"

LArAutoCorrTotalCondAlg::LArAutoCorrTotalCondAlg(const std::string &name,
                                                 ISvcLocator *pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator),
      m_LArADC2MeVObjKey("LArADC2MeV"),
      m_LArOnOffIdMappingObjKey("LArOnOffIdMap"),
      m_LArShapeObjKey("LArShapeSym"),
      m_LArAutoCorrObjKey("LArAutoCorrSym"),
      m_LArNoiseObjKey("LArNoiseSym"),
      m_LArPedestalObjKey("LArPedestal"),
      m_LArfSamplObjKey("LArfSamplSym"),
      m_LArMinBiasObjKey("LArMinBiasSym"),
      m_LArAutoCorrTotalObjKey("LArAutoCorrTotal"),
      m_Nminbias(0), m_NoPile(false), m_isMC(true),
      m_isSuperCell(false), m_Nsamples(5),
      m_firstSample(0), m_deltaBunch(1) {
  declareProperty("LArADC2MeVObjKey", m_LArADC2MeVObjKey,
                  "Key to read LArADC2MeV object");
  declareProperty("LArOnOffIdMappingObjKey", m_LArOnOffIdMappingObjKey,
                  "Key to read LArOnOffIdMapping object");
  declareProperty("LArShapeObjKey", m_LArShapeObjKey,
                  "Key to read LArShape object");
  declareProperty("LArAutoCorrObjKey", m_LArAutoCorrObjKey,
                  "Key to read LArAutoCorr object");
  declareProperty("LArNoiseObjKey", m_LArNoiseObjKey,
                  "Key to read LArNoise object");
  declareProperty("LArPedestalObjKey", m_LArPedestalObjKey,
                  "Key to read LArPedestal object");
  declareProperty("LArfSamplObjKey", m_LArfSamplObjKey,
                  "Key to read LArfSampl object");
  declareProperty("LArMinBiasObjKey", m_LArMinBiasObjKey,
                  "Key to read LArMinBias object");
  declareProperty("LArAutoCorrTotalObjKey", m_LArAutoCorrTotalObjKey,
                  "Key to write LArAutoCorrTotal object");
  declareProperty("Nminbias", m_Nminbias);
  declareProperty("NoPile", m_NoPile);
  declareProperty("isMC", m_isMC);
  declareProperty("isSuperCell", m_isSuperCell);
  declareProperty("Nsamples", m_Nsamples, "Max number of samples to use");
  declareProperty(
      "firstSample", m_firstSample,
      "First sample to use for in-time event on the full pulse shape");
  declareProperty("deltaBunch", m_deltaBunch,
                  "Delta between filled bunches in 25 ns units");
}

LArAutoCorrTotalCondAlg::~LArAutoCorrTotalCondAlg() {}

StatusCode LArAutoCorrTotalCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // ReadCondHandle initialization
  ATH_CHECK(m_LArShapeObjKey.initialize());
  ATH_CHECK(m_LArAutoCorrObjKey.initialize());
  ATH_CHECK(m_LArOnOffIdMappingObjKey.initialize());
  ATH_CHECK( m_LArADC2MeVObjKey.initialize());

  ATH_CHECK(m_LArAutoCorrTotalObjKey.initialize());

  m_NoPile = false;
  if (m_Nminbias <= 0)
    m_NoPile = true;

  ATH_CHECK(m_LArNoiseObjKey.initialize(!m_NoPile && m_isMC));
  ATH_CHECK(m_LArPedestalObjKey.initialize(!m_NoPile && !m_isMC));
  ATH_CHECK(m_LArfSamplObjKey.initialize(!m_NoPile));
  ATH_CHECK(m_LArMinBiasObjKey.initialize(!m_NoPile));

  // Number of gains (does this have to be in initialize now b/c of AthenaMT?)
  if (m_isSuperCell) {
    m_nGains = 1;
  } else {
    m_nGains = 3;
  }

  ATH_MSG_DEBUG("settings: m_Nminbias " << m_Nminbias << " m_NoPile " << m_NoPile);
  return StatusCode::SUCCESS;
}

StatusCode LArAutoCorrTotalCondAlg::execute() {

  // WriteHandle setup
  SG::WriteCondHandle<LArAutoCorrTotal> writeHandle(m_LArAutoCorrTotalObjKey);
  // So the following should not be called usually?!
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG(
        "CondHandle "
        << writeHandle.fullKey() << " is already valid.");
    return StatusCode::SUCCESS;
  }

  // Identifier helper
  const LArOnlineID_Base *larOnlineID = nullptr;
  if (m_isSuperCell) {
    const LArOnline_SuperCellID *scidhelper;
    ATH_CHECK(detStore()->retrieve(scidhelper, "LArOnline_SuperCellID"));
    larOnlineID = scidhelper; // cast to base-class
  } else {                    // regular cells
    const LArOnlineID *idhelper;
    ATH_CHECK(detStore()->retrieve(idhelper, "LArOnlineID"));
    larOnlineID = idhelper; // cast to base-class
  }
  // Mapping helper
  const LArOnOffIdMapping *larOnOffIdMapping = nullptr;
  SG::ReadCondHandle<LArOnOffIdMapping> larOnOffIdMappingHdl{
    m_LArOnOffIdMappingObjKey
  };
  larOnOffIdMapping = *larOnOffIdMappingHdl;
  if (larOnOffIdMapping == nullptr) {
    ATH_MSG_ERROR("Failed to retrieve LArOnOffIdMapping object");
  }

  // Get pointers to inputs
  SG::ReadCondHandle<ILArShape> ShapeHdl{ m_LArShapeObjKey };
  // FIXME: should check if handle is properly created and/or check if handle is
  // properly retrieved
  // operator star of a ReadCondHandle returns a const pointer to type T
  const ILArShape *larShape{ *ShapeHdl };
  writeHandle.addDependency(ShapeHdl);

  SG::ReadCondHandle<ILArAutoCorr> AutoCorrHdl{ m_LArAutoCorrObjKey };
  const ILArAutoCorr *larAutoCorr{ *AutoCorrHdl };
  writeHandle.addDependency(AutoCorrHdl);

  SG::ReadCondHandle<LArADC2MeV> ADC2MeVHdl{ m_LArADC2MeVObjKey };
  const LArADC2MeV *larADC2MeV = nullptr;
  larADC2MeV = *ADC2MeVHdl;
  if (larADC2MeV == nullptr) {
    ATH_MSG_ERROR("Failed to retrieve LArADC2MeV object");
  }
  writeHandle.addDependency(ADC2MeVHdl);

  // Consider the determinstic objects
  // checking isMC and NoPile again seems very clumsy. How to check if key has
  // already been initialized?
  const ILArNoise *larNoise = nullptr;
  const ILArPedestal *larPedestal = nullptr;
  const ILArfSampl *larfSampl = nullptr;
  const ILArMinBias *larMinBias = nullptr;

  if (!m_NoPile) {
    if (m_isMC) {
      SG::ReadCondHandle<ILArNoise> NoiseHdl{ m_LArNoiseObjKey };
      larNoise = *NoiseHdl;
      writeHandle.addDependency(NoiseHdl);
    } else {
      SG::ReadCondHandle<ILArPedestal> PedestalHdl{ m_LArPedestalObjKey };
      larPedestal = *PedestalHdl;
      writeHandle.addDependency(PedestalHdl);
    }

    SG::ReadCondHandle<ILArfSampl> fSamplHdl{ m_LArfSamplObjKey };
    larfSampl = *fSamplHdl;
    writeHandle.addDependency(fSamplHdl);

    SG::ReadCondHandle<ILArMinBias> MinBiasHdl{ m_LArMinBiasObjKey };
    larMinBias = *MinBiasHdl;
    writeHandle.addDependency(MinBiasHdl);
  }

  ATH_MSG_INFO("IOV found from intersection for AutoCorrTotal object: "
               << writeHandle.getRange());

  // make output object
  // dimensions: number of gains x number of channel IDs x elements of
  // AutoCorrTotal
  std::unique_ptr<LArAutoCorrTotal> larAutoCorrTotal =
      std::make_unique<LArAutoCorrTotal>(larOnlineID, larOnOffIdMapping, m_nGains);

  std::vector<HWIdentifier>::const_iterator it = larOnlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e = larOnlineID->channel_end();
  int count = 0;
  int count2 = 0;

  for (; it != it_e; ++it) {
    count++;
    const HWIdentifier chid = *it;
    const IdentifierHash hid = larOnlineID->channel_Hash(chid);
    // const unsigned int id32 = chid.get_identifier32().get_compact();

    if (larOnOffIdMapping->isOnlineConnected(chid)) {
        count2++;

      for (size_t igain = 0; igain < m_nGains; igain++) {
        const ILArShape::ShapeRef_t Shape = larShape->Shape(chid, igain);
        const int nsamples_shape = static_cast<int>(Shape.size());

        const ILArAutoCorr::AutoCorrRef_t AC =
            larAutoCorr->autoCorr(chid, igain);

        int nsamples_AC_OFC = AC.size() + 1;

        if (nsamples_AC_OFC > m_Nsamples) {
          nsamples_AC_OFC = m_Nsamples;
        }

        // fix HEC first sample +1 if the firstSample is 0 and nsamples 4
        unsigned int ihecshift = 0;
        if (larOnlineID->isHECchannel(chid) && nsamples_AC_OFC == 4 &&
            m_firstSample == 0) {
          ihecshift = 1;
          // ATH_MSG_DEBUG( "Using firstSample +1 for HEC ChID 0x" << MSG::hex
          // << id << MSG::dec  );
        }

        //:::::::::::::::::::::::::::::::
        // NB:
        // nsamples_shape    = number of samples of the Shape function (e.g 32)
        // nsamples_AC_OFC = size of AC matrix & OFC vector (e.g 5 in Atlas)
        //:::::::::::::::::::::::::::::::
        float fSigma2 = 0.;
        if (!m_NoPile) {
          float SigmaNoise;
          if (m_isMC)
            SigmaNoise = larNoise->noise(chid, igain);
          else {
            float RMSpedestal = larPedestal->pedestalRMS(chid, igain);
            if (RMSpedestal > (1.0 + LArElecCalib::ERRORCODE))
              SigmaNoise = RMSpedestal;
            else
              SigmaNoise = 0.; //(we will have the ERROR message below)
          }
          float fSampl = larfSampl->FSAMPL(chid);
          float MinBiasRMS = larMinBias->minBiasRMS(chid);
          if (fSampl != 0)
            MinBiasRMS /= fSampl;
          const auto polynom_adc2mev =
              larADC2MeV->ADC2MEV(hid, igain);
          float Adc2MeV = 0.;
          if (polynom_adc2mev.size() > 0) {
            Adc2MeV = (polynom_adc2mev)[1];
          }
          if (SigmaNoise != 0 && Adc2MeV != 0)
            fSigma2 = pow(MinBiasRMS / (SigmaNoise * Adc2MeV), 2);

          if (fSampl == 0 || SigmaNoise == 0 || Adc2MeV == 0) {
            if (m_isMC) {
              ATH_MSG_ERROR(larOnlineID->show_to_string(
                                larOnOffIdMapping->cnvToIdentifier(chid))
                            << "fSampl (" << fSampl << "), SigmaNoise ("
                            << SigmaNoise << ") or Adc2MeV (" << Adc2MeV
                            << ") null "
                            << "=> AutoCorrTotal = only AutoCorr elect. part ");
            }
            fSigma2 = 0.;
          }
          // warning: MinBiasRMS is in MeV (at the scale of the hits)
          //         SigmaNoise is in ADC counts
          //  so MinBiasRMS/fScale and SigmaNoise*Adc2MeV are the same scale
          //  (MeV at the scale of the cells)
        } // end if m_NoPile

        // get in vTerms all the possible non trivial N(N-1)/2 terms of the
        // autocorrelation matrix
        int nsize_tot = (nsamples_AC_OFC - 1) * (nsamples_AC_OFC) / 2;

        std::vector<float> vTerms;

        vTerms.resize(2 * nsize_tot + nsamples_AC_OFC, 0.);
        //:::::::::::::::::::::::::::::::

        for (int j1 = 0; j1 < nsamples_AC_OFC - 1; j1++) {
          for (int j2 = j1 + 1; j2 < nsamples_AC_OFC; j2++) {
            int l = abs(j2 - j1) - 1;
            int index =
                j1 * nsamples_AC_OFC - j1 * (j1 + 1) / 2 + j2 - (j1 + 1);
            vTerms[index] = AC[l];
          }
        }

        // 2nd terms :
        for (int j1 = 0; j1 < nsamples_AC_OFC - 1; ++j1) {
          for (int j2 = j1 + 1; j2 < nsamples_AC_OFC; j2++) {
            int index =
                j1 * nsamples_AC_OFC - j1 * (j1 + 1) / 2 + j2 - (j1 + 1);
            float Rij = 0;
            for (int k = 0; k < nsamples_shape; ++k) {
              if ((j2 - j1 + k) >= 0 && (j2 - j1 + k) < nsamples_shape) {
                int ibunch = 0;
                if ((j1 + m_firstSample + ihecshift - k) % m_deltaBunch == 0)
                  ibunch = 1;
                Rij += Shape[k] * Shape[j2 - j1 + k] * ibunch;
              }
            }
            vTerms[nsize_tot + index] = fSigma2 * Rij;
          }
        }

        // 3rd term : RMS of pileup per samples (multiplied by fSigma2)
        for (int j1 = 0; j1 < nsamples_AC_OFC; j1++) {
          float Rms2i = 0;
          for (int k = 0; k < nsamples_shape; ++k) {
            int ibunch = 0;
            if ((j1 + m_firstSample + ihecshift - k) % m_deltaBunch == 0)
              ibunch = 1;
            Rms2i += pow(Shape[k], 2) * ibunch;
          }
          vTerms[2 * nsize_tot + j1] = fSigma2 * Rms2i;
        }

        // storage
        larAutoCorrTotal->set(hid, igain, vTerms);

      } //(loop on gains)

    } else // unconnected
      for (unsigned int igain = 0; igain < m_nGains; igain++) {
        unsigned nsize_tot = (m_Nsamples - 1) * (m_Nsamples) + m_Nsamples;
        std::vector<float> empty(nsize_tot, 0.);
        larAutoCorrTotal->set(hid, igain, empty);
      }
  }

  ATH_MSG_INFO("LArAutoCorrTotal Ncell " << count);
  ATH_MSG_INFO("LArAutoCorrTotal Nconnected " << count2);

  ATH_MSG_INFO("LArAutoCorrTotal record with key" << m_LArAutoCorrTotalObjKey);

  ATH_CHECK(writeHandle.record(std::move(larAutoCorrTotal)));


  return StatusCode::SUCCESS;
}
