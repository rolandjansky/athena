#include "PFTrackMuonCaloTaggingAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthContainers/ConstDataVector.h"

PFTrackMuonCaloTaggingAlg::PFTrackMuonCaloTaggingAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{
}

PFTrackMuonCaloTaggingAlg::~PFTrackMuonCaloTaggingAlg() {}

StatusCode PFTrackMuonCaloTaggingAlg::initialize()
{
  ATH_CHECK(m_inputTracksKey.initialize());
  ATH_CHECK(m_caloExtensionKey.initialize());
  ATH_CHECK(m_inputCellsKey.initialize());
  ATH_CHECK(m_outputTracksKey.initialize());
  ATH_CHECK(m_looseMuonTagTool.retrieve());
  ATH_CHECK(m_tightMuonTagTool.retrieve());
  ATH_CHECK(m_muonScoreTool.retrieve());
  ATH_CHECK(m_depositInCaloTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode PFTrackMuonCaloTaggingAlg::execute(const EventContext &ctx) const
{
  auto inputTracks = SG::makeHandle(m_inputTracksKey, ctx);
  if (!inputTracks.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputTracksKey);
    return StatusCode::FAILURE;
  }
  auto inputCells = SG::makeHandle(m_inputCellsKey, ctx);
  if (!inputCells.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputCellsKey);
    return StatusCode::FAILURE;
  }
  auto inputCaloExtensions = SG::makeHandle(m_caloExtensionKey, ctx);
  if (!inputCaloExtensions.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_caloExtensionKey);
    return StatusCode::FAILURE;
  }
  auto outputHandle = SG::makeHandle(m_outputTracksKey, ctx);

  auto outputTracks = std::make_unique<ConstDataVector<xAOD::TrackParticleContainer>>(SG::VIEW_ELEMENTS);
  for (const xAOD::TrackParticle *itrk : *inputTracks)
  {
    if (itrk->pt() > m_minPt)
    {
      std::vector<DepositInCalo> deposits = m_depositInCaloTool->getDeposits(itrk, inputCells.ptr(), inputCaloExtensions.ptr());
      int looseTag = m_looseMuonTagTool->caloMuonTag(deposits, itrk->eta(), itrk->pt());
      int tightTag = m_tightMuonTagTool->caloMuonTag(deposits, itrk->eta(), itrk->pt());
      float score = m_muonScoreTool->getMuonScore(itrk, inputCells.ptr(), inputCaloExtensions.ptr());
      if (tightTag || (score > m_caloMuonScoreCut && looseTag))
        // This means that the track is tagged as a muon, so don't output it
        continue;
    }
    outputTracks->push_back(itrk);
  }
  ATH_CHECK(outputHandle.put(ctx, std::move(outputTracks)) != nullptr);
  return StatusCode::SUCCESS;
}