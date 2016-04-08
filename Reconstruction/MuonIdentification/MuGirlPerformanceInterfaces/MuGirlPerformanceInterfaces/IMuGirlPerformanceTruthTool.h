/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuGirlPerformanceTruthTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSMUGIRLPERFORMANCETRUTHTOOL_H
#define IMUGIRLNSMUGIRLPERFORMANCETRUTHTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Rec
{
    class TrackParticle;
}

namespace MuGirlNS 
{
    static const InterfaceID IID_IMuGirlPerformanceTruthTool("MuGirlNS::IMuGirlPerformanceTruthTool", 1, 0);
    
    class IMuGirlPerformanceTruthTool : virtual public IAlgTool
    {
    public:
        static const InterfaceID& interfaceID( ) ;
    
        struct TruthMuon
        {
            int type;       /**< 0 = TruthEvent, 1 = TrackRecord */
            double phi;
            double eta;
            double perp;
            int pdgCode;
            int parentPdgCode;
            
            TruthMuon(int type, double phi, double eta, double perp, int pdgCode, int parentPdgCode) :
            type(type), phi(phi), eta(eta), perp(perp), pdgCode(pdgCode), parentPdgCode(parentPdgCode)
            {
            }
        };
        typedef std::vector<TruthMuon*> TruthMuonList;
    
        virtual StatusCode getTruthMuons(TruthMuonList& truthMuonList) = 0;
    
        struct TruthTrack
        {
            double       Phi;       /**< True phi */
            double       Eta;       /**< True eta */
            double       Pt;        /**< True pT */
            int          Charge;    /**< Charge */
            double       DPhi;      /**< phi(Truth) - phi(ID) */
            double       DEta;      /**< eta(Truth) - eta(ID) */
            double       DPt_Pt;    /**< (pT(Truth) - pT(ID)) / (pT(Truth) */
        };
    
        virtual StatusCode getTruthTrack(const Rec::TrackParticle* pParticle, TruthTrack& truthTrack) = 0;
    
    };
    
    inline const InterfaceID& MuGirlNS::IMuGirlPerformanceTruthTool::interfaceID()
    {
        return IID_IMuGirlPerformanceTruthTool;
    }

} // end of namespace

#endif 
