/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentPerformanceAlg.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "MuonStationIndex/MuonStationIndex.h"

MuonSegmentPerformanceAlg::MuonSegmentPerformanceAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : 
  AthAlgorithm(name, pSvcLocator),
  m_writeToFile (false),
  m_segmentKey("MuonSegments"),
  m_truthSegmentKey( "MuonTruthSegments"),
  m_nevents(0)
{
  declareProperty("SegmentLocation",      m_segmentKey);
  declareProperty("TruthSegmentLocation", m_truthSegmentKey);
  declareProperty("writeToFile",          m_writeToFile = false);
  declareProperty("FileName",             m_fileName = "MuonSegmentPerformanceAlg.txt" );
}
 
StatusCode MuonSegmentPerformanceAlg::initialize()
{

  // initialize cuts, please make sure the number of bins and the sizes of the cuts + string are always the same
  unsigned int nbins = 3;
  m_nevents = 0;
  m_ntruth.resize(nbins);
  m_nfound.resize(nbins);
  m_nfake.resize(nbins);
  m_nhitCuts = { 3,4,5 };
  m_hitCutString = { "n==3 ", "n==4 ", "n>=5 " };
  for(unsigned int i=0;i<nbins;++i ){ 
    m_ntruth[i].resize(Muon::MuonStationIndex::ChIndexMax,0);
    m_nfound[i].resize(Muon::MuonStationIndex::ChIndexMax,0);
    m_nfake[i].resize(Muon::MuonStationIndex::ChIndexMax,0);
  }

  if(!m_segmentKey.key().empty()) ATH_CHECK(m_segmentKey.initialize());
  if(!m_truthSegmentKey.key().empty()) ATH_CHECK(m_truthSegmentKey.initialize());

  return StatusCode::SUCCESS;
}
 
StatusCode MuonSegmentPerformanceAlg::execute()
{
  const xAOD::MuonSegmentContainer* segments = 0;
  if( !retrieve(m_segmentKey,segments) || !segments ) return StatusCode::SUCCESS;

  const xAOD::MuonSegmentContainer* truthSegments = 0;
  if( !retrieve(m_truthSegmentKey,truthSegments) || !truthSegments ) return StatusCode::SUCCESS;
  std::set<const xAOD::MuonSegment*> matchedSegments;
  ++m_nevents;

  bool missedSegment = false;
  for( const auto& seg : *truthSegments ){
    
    int chIndex = seg->chamberIndex();
    if( chIndex < 0 || chIndex >= Muon::MuonStationIndex::ChIndexMax ){
      ATH_MSG_WARNING("bad index " << chIndex );
      continue;
    }
    unsigned int index = 0;
    if( seg->nPrecisionHits() < 3 ) continue;
    while( seg->nPrecisionHits() > m_nhitCuts[index] && index < m_nhitCuts.size()-1 ) ++index;

    ++m_ntruth[index][chIndex];

    if(index==2) {
      if(Muon::MuonStationIndex::chName(static_cast<Muon::MuonStationIndex::ChIndex>(chIndex))=="CSS") ATH_MSG_WARNING(" CSS with more than 4 layers ");
      if(Muon::MuonStationIndex::chName(static_cast<Muon::MuonStationIndex::ChIndex>(chIndex))=="CSL") ATH_MSG_WARNING(" CSL with more than 4 layers ");
    }
    const ElementLink< xAOD::MuonSegmentContainer >& recoLink = seg->auxdata< ElementLink< xAOD::MuonSegmentContainer > >("recoSegmentLink");
    if( recoLink.isValid() ) {
      ++m_nfound[index][chIndex];
      matchedSegments.insert(*recoLink);
    }else{
      ATH_MSG_DEBUG(" Missing segment in sector " << seg->sector() 
                    << "  " << Muon::MuonStationIndex::chName(static_cast<Muon::MuonStationIndex::ChIndex>(chIndex)) << " eta " << seg->etaIndex()  
                    << " nprec " << seg->nPrecisionHits() << " nphi " << seg->nPhiLayers() << " nTrigEta " << seg->nTrigEtaLayers() );
      missedSegment = true;
    }
  }
  if(missedSegment) ATH_MSG_DEBUG(" Dump Fake segments " );

  for( const auto& seg : *segments ){
    if( matchedSegments.count(seg) ) continue;
    int chIndex = seg->chamberIndex();
    if( chIndex < 0 || chIndex >= Muon::MuonStationIndex::ChIndexMax ){
      ATH_MSG_WARNING("bad index " << chIndex );
      continue;
    }

    unsigned int index = 0;
    if( seg->nPrecisionHits() < 3 ) continue;
    while( seg->nPrecisionHits() > m_nhitCuts[index] && index < m_nhitCuts.size()-1 ) ++index;
    if(missedSegment) ATH_MSG_DEBUG(" Fake segment in sector " << seg->sector() 
                    << "  " << Muon::MuonStationIndex::chName(static_cast<Muon::MuonStationIndex::ChIndex>(chIndex)) << " eta " << seg->etaIndex()  
                    << " nprec " << seg->nPrecisionHits() << " nphi " << seg->nPhiLayers() << " nTrigEta " << seg->nTrigEtaLayers() );

    ++m_nfake[index][chIndex];
  }
  
  return StatusCode::SUCCESS;
  
}

std::string MuonSegmentPerformanceAlg::printRatio( std::string prefix, unsigned int begin, unsigned int end, 
                                            const std::vector<int> reco, const std::vector<int> truth ) const {
  std::ostringstream sout;
  unsigned int width = 9;
  unsigned int precision = 3;
  sout << std::endl << prefix;
  for( unsigned int i=begin;i<end;++i ){
    sout << std::setw(width) << std::setprecision(precision);
    if(  truth[i] == 0 ) sout << " ";
    else                 sout << static_cast<double>(reco[i])/static_cast<double>(truth[i]);
  }
  sout << std::endl << " #Events        " << " ";
  for( unsigned int i=begin;i<end;++i ){
    sout << std::setw(width) << std::setprecision(precision);
    if(  truth[i] == 0 ) sout << " ";
    else sout << static_cast<double>(truth[i]);
  }
  return sout.str();
}
std::string MuonSegmentPerformanceAlg::printRatio( std::string prefix, unsigned int begin, unsigned int end, 
                                            const std::vector<int> reco ) const {
  std::ostringstream sout;
  unsigned int width = 9;
  unsigned int precision = 3;
  sout << std::endl << prefix;
  for( unsigned int i=begin;i<end;++i ){
    sout << std::setw(width) << std::setprecision(precision);
    if(  m_nevents == 0 ) sout << " ";
    else                  sout << static_cast<double>(reco[i])/static_cast<double>(m_nevents);
  }
  return sout.str();
}

StatusCode MuonSegmentPerformanceAlg::finalize() {
  std::ofstream fileOutput;
  std::string outfile = "muonPerformance_segments.txt";
  fileOutput.open(outfile.c_str(), std::ios::trunc);
  std::ostringstream sout;
  sout.precision(4);

  unsigned int width = 9;
  sout << "Segment finding efficiencies barrel" << std::endl;
  sout << " Chambers        ";
  std::string prefix_eff  = " Efficiency ";
  std::string prefix_fake = " Fake rate  ";
  for( unsigned int i=0;i<Muon::MuonStationIndex::BEE;++i ) sout << std::setw(width) << Muon::MuonStationIndex::chName((Muon::MuonStationIndex::ChIndex)i);
  for( unsigned int j=0;j<m_nfound.size();++j ){
    sout << printRatio(prefix_eff+m_hitCutString[j],0,Muon::MuonStationIndex::BEE,m_nfound[j],m_ntruth[j]); 
  }
  sout << std::endl;
  for( unsigned int j=0;j<m_nfound.size();++j ){
    sout << printRatio(prefix_fake+m_hitCutString[j],0,Muon::MuonStationIndex::BEE,m_nfake[j]); 
  }
  sout << std::endl;

  sout << "Segment finding efficiencies endcaps" << std::endl;
  sout << " Chambers        ";
  for( unsigned int i=Muon::MuonStationIndex::BEE;i<Muon::MuonStationIndex::ChIndexMax;++i ) 
    sout << std::setw(width) << Muon::MuonStationIndex::chName((Muon::MuonStationIndex::ChIndex)i);
  for( unsigned int j=0;j<m_nfound.size();++j ){
    sout << printRatio(prefix_eff+m_hitCutString[j],Muon::MuonStationIndex::BEE,Muon::MuonStationIndex::ChIndexMax,m_nfound[j],m_ntruth[j]); 
  }
  sout << std::endl;
  for( unsigned int j=0;j<m_nfound.size();++j ){
    sout << printRatio(prefix_fake+m_hitCutString[j],Muon::MuonStationIndex::BEE,Muon::MuonStationIndex::ChIndexMax,m_nfake[j]); 
  }
  sout << std::endl;
  fileOutput << sout.str() << std::endl;
  fileOutput.close();
  return StatusCode::SUCCESS;
}

bool MuonSegmentPerformanceAlg::retrieve(const SG::ReadHandleKey<xAOD::MuonSegmentContainer>& segments, const xAOD::MuonSegmentContainer*& ptr ) const {
  SG::ReadHandle<xAOD::MuonSegmentContainer> handle(segments);
  if(!handle.isPresent() || !handle.isValid()) { //Cautious isPresent TODO remove once job options are more intelligent
      ATH_MSG_WARNING( "Unable to retrieve " << segments.key() );
      ptr=nullptr;
      return false;
  }
  ptr = handle.cptr();    
  ATH_MSG_DEBUG( "Retrieved " << segments.key() << " size " << ptr->size() );
  return true;
}
 


