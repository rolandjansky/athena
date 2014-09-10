/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTXAODBphysMonTool.h
 *   Class declaration for HLTXAODBphysMonTool
 *
 *
 *
 *    @author name (uid@cern.ch)
 *
 *
 */

#ifndef HLTXAODBPHYSMONTOOL_H
#define HLTXAODBPHYSMONTOOL_H

// C++
#include <string>
#include <vector>
#include <map>

class TLorentzVector;
class TVector3;

// TrigHLTMonitoring interface
#include "TrigHLTMonitoring/IHLTMonTool.h"

namespace Trig { class chainGroup; }
namespace BPhys { class BCompositeParticleBuilder; }
namespace Analysis { class PrimaryVertexRefitter;}

///Concrete monitoring tool derived from MonitorToolBase
class HLTXAODBphysMonTool : public IHLTMonTool
{
    
public:
    HLTXAODBphysMonTool(   const std::string & type,
                    const std::string & name,
                    const IInterface* parent);
    virtual ~HLTXAODBphysMonTool();
    
    StatusCode init() ;
    StatusCode book() ;
    StatusCode fill() ;
    StatusCode test() ;
    StatusCode testTrig(const std::string& trigItem);
    StatusCode JpsiFinder() ;
    StatusCode Triggers() ;
    StatusCode L2Tracks() ;
    StatusCode EFTracks() ;
    StatusCode L2DsPhiPiMon() ;
    StatusCode L2TopoDiMuMon() ;
    StatusCode L2TopoDiMuXMon() ;
    StatusCode L2DiMuMon() ;
    StatusCode L2DiMuXMon() ;
    StatusCode L2BJpsieeMon() ;
    StatusCode L2BJpsimumu();
    StatusCode EFDsPhiPiMon() ;
    StatusCode EFTopoDiMuMon() ;
    StatusCode EFDiMuMon() ;
    StatusCode EFBJpsimumu();
    StatusCode CompareDiMuL2EF();
    StatusCode CompareTopoDiMuL2EF();
    StatusCode proc(); // called by procHistograms
    
private:
    //James'
    void buildHist( const std::string & trigname,const std::string & prefix="TrigBphys");
    void fillHist(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,const std::string & prefix="TrigBphys");
    
    //   std::vector<TH1F *> m_HistoVectorBphys;
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;
    // The following methods will eventually be implemented in new "V0Tools". For now,
    // we will keep them here.
    TVector3       trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const;
    TLorentzVector track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    TVector3       origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
    TLorentzVector origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    double         invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const;
    double         massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
    double         trackCharge(const xAOD::Vertex * vxCandidate, int i) const;
    Amg::MatrixX*  convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const;
    double m_muonMass;
    
    std::vector<std::string> m_TrigNames;
    std::vector<bool> m_TrigNames1D;
    std::vector<std::string> m_bphysItems; //! Bphysics chains to test
    
    std::string m_JpsiCandidatesKey;
    
    int L1passed;
    int L2passed;
    int EFpassed;
    
    int L1DiMu;
    int L2DiMu;
    int EFDiMu;
	
    //std::vector<std::string> TrigName;
    //std::vector<bool> TrigName1D;
    
    std::vector<std::string> EffNum;
    std::vector<std::string> EffDenom;
    std::map<std::string, TH1*> EffHistMap;
    
    const Trig::ChainGroup *m_all;
    const Trig::ChainGroup *m_allL1;
    const Trig::ChainGroup *m_allL2;
    const Trig::ChainGroup *m_allEF;
    
    // For converting to BPhys objects
    BPhys::BCompositeParticleBuilder* m_bcpBuilder;
    // Primary vertex refitting
    Analysis::PrimaryVertexRefitter* m_pvRefitter;
    
    // Method for managing the histogram divisions
    void divide(TH1 *num, TH1 *den, TH1 *quo);
    

    
};

#endif
