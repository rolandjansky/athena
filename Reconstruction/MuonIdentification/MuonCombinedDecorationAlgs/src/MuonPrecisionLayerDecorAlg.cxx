/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrecisionLayerDecorAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadHandle.h" 

namespace{
    static const SG::AuxElement::ConstAccessor<std::vector<std::vector<unsigned int>>> acc_alignEffectChId("alignEffectChId");
    static const SG::AuxElement::ConstAccessor<std::vector<float>> acc_alligSigmaDeltaTrans("alignEffectSigmaDeltaTrans");
}
 MuonPrecisionLayerDecorAlg::MuonPrecisionLayerDecorAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator){}
 
 StatusCode MuonPrecisionLayerDecorAlg::initialize() {
    ATH_CHECK(m_MuonContainer.initialize());
    ATH_CHECK(m_TrackContainer.initialize());
    m_trkAlignReadKey.clear();
    for (const SG::ReadHandleKey<xAOD::TrackParticleContainer>& trk : m_TrackContainer){
        m_trkAlignReadKey.emplace_back(trk.key()+".alignEffectChId");
    }
    ATH_CHECK(m_trkAlignReadKey.initialize());
    
    m_goodPrecLayerKey = m_MuonContainer.key() + ".numberOfGoodPrecisionLayers";
    ATH_CHECK(m_goodPrecLayerKey.initialize());
    m_isGoodSmallKey = m_MuonContainer.key() + ".isSmallGoodSectors";
    ATH_CHECK(m_isGoodSmallKey.initialize());
    m_isEndcapGoodLayersKey = m_MuonContainer.key() + ".isEndcapGoodLayers";   
    ATH_CHECK(m_isEndcapGoodLayersKey.initialize());
    return StatusCode::SUCCESS;
 }

 StatusCode MuonPrecisionLayerDecorAlg::execute(const EventContext& ctx) const {
    SG::ReadHandle<xAOD::MuonContainer> muons{m_MuonContainer, ctx};
    if (!muons.isValid()) {
        ATH_MSG_FATAL("Failed to load track collection "<<m_MuonContainer.fullKey());
        return StatusCode::FAILURE;
    }     
    SG::WriteDecorHandle<xAOD::MuonContainer, uint8_t> acc_precLayers{m_goodPrecLayerKey,ctx};
    SG::WriteDecorHandle<xAOD::MuonContainer, uint8_t> acc_isSmallGood{m_isGoodSmallKey,ctx};
    SG::WriteDecorHandle<xAOD::MuonContainer, uint8_t> acc_isEndcapGood{m_isEndcapGoodLayersKey, ctx};
 
    for (const xAOD::Muon* mu : *muons) {
         // no CT or ST muons
        if (mu->primaryTrackParticle() == mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) continue;  
        // no SA muons w/o ME tracks
        if (mu->primaryTrackParticle() == mu->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)) continue;  
        const xAOD::TrackParticle* ptp = mu->primaryTrackParticle();
        const std::vector<std::vector<unsigned int>>& chIds = acc_alignEffectChId(*ptp);
        const std::vector<float>& alignEffSDT = acc_alligSigmaDeltaTrans(*ptp);

        uint8_t prec = 0;  // all precision layers
        mu->summaryValue(prec, xAOD::numberOfPrecisionLayers);
        const uint8_t nTotPrec = prec;        
        uint8_t nBadPrec{0}, nBadBar{0}, nBadSmall{0}, nBadLarge{0}, nGoodBar{0}, nGoodLarge{0}, nGoodSmall{0};

        std::map<Muon::MuonStationIndex::ChIndex, int> chamberQual;  // 1=good, 2=bad; for choosing large/small
        for (unsigned int i = 0; i < chIds.size(); ++i) {
            for (unsigned int j = 0; j < chIds[i].size(); ++j) {
                Muon::MuonStationIndex::ChIndex currInd = (Muon::MuonStationIndex::ChIndex)chIds[i][j];
                if (alignEffSDT[i] >= 0.5) {
                    if ((chamberQual.count(currInd) && chIds[i].size() > 1) || !chamberQual.count(currInd)) {
                        // either we haven't seen this chamber before, or we have but now
                        // we're in a sub-vector that's not just this chamber
                        chamberQual[currInd] = 2;
                    }
                } else {
                    if ((chamberQual.count(currInd) && chIds[i].size() > 1) || !chamberQual.count(currInd)) {
                    // either we haven't seen this chamber before, or we have but now
                    // we're in a sub-vector that's not just this chamber
                    chamberQual[currInd] = 1;
                    }
                }
            }
        }
        for (const auto& it : chamberQual) {
            const int chnum = it.first;
            const bool is_barrel = chnum < 7;
            const bool is_small  = !(chnum %2);
            if (it.second == 2) {
                ++nBadPrec;            
                nBadBar+= is_barrel;
                nBadSmall+= is_small;
                nBadLarge+= !is_small;               
            } else {
                nGoodBar+= is_barrel;
                nGoodLarge+= !is_small;
                nGoodSmall+= is_small;
            }
        }
        const uint8_t nGoodPrec = nTotPrec - nBadPrec;
        const uint8_t nBadEnd = nBadPrec - nBadBar;
        const uint8_t nGoodEnd = nGoodPrec - nGoodBar;        
        const bool countHits = (nGoodSmall == nGoodLarge) || (nBadSmall == nBadLarge);
        
        uint8_t isSmall = (nGoodPrec >= nBadPrec&& nGoodSmall > nGoodLarge) || (nGoodPrec < nBadPrec&& nBadSmall > nBadLarge);
        const uint8_t isEnd = (nGoodPrec >= nBadPrec&& nGoodEnd > nGoodBar) || (nGoodPrec < nBadPrec && nBadEnd >nBadBar );
        
        static constexpr std::array<xAOD::MuonSummaryType, 4> small_sectors{xAOD::innerSmallHits, xAOD::middleSmallHits, xAOD::outerSmallHits, xAOD::extendedSmallHits}; 
        static constexpr std::array<xAOD::MuonSummaryType, 4> large_sectors{xAOD::innerLargeHits, xAOD::middleLargeHits, xAOD::outerLargeHits, xAOD::extendedLargeHits}; 
       
        if (countHits) {  // decide large-small by counting hits
            auto accumulator = [mu](int val, const xAOD::MuonSummaryType& sum) -> int{
                uint8_t sumval = 0;
                mu->summaryValue(sumval,sum);
                return val +sumval;
            };
            const int nSmallHits = std::accumulate(small_sectors.begin(), small_sectors.end(),0 ,accumulator);
            const int nLargeHits = std::accumulate(large_sectors.begin(), large_sectors.end(),0 ,accumulator);
            isSmall = (nSmallHits > nLargeHits);
        }
        acc_precLayers(*mu) = nGoodPrec;
        acc_isSmallGood(*mu) = isSmall;
        acc_isEndcapGood(*mu) = isEnd;
    }  
    return StatusCode::SUCCESS;
 }
 