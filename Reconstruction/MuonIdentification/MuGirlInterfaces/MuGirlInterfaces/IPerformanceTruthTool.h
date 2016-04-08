/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPerformanceTruthTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSPERFORMANCETRUTHTOOL_H
#define IMUGIRLNSPERFORMANCETRUTHTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"

class PerigeeParameters;
class TrigMuonEFTrack;
class MuonFeature;

namespace NTuple
{
    class Tuple;
}

namespace MuGirlNS 
{
    class CandidateSummary;

    static const InterfaceID IID_IPerformanceTruthTool("MuGirlNS::IPerformanceTruthTool", 1, 0);
    
    class IPerformanceTruthTool : virtual public IAlgTool
    {
    
    public:
        static const InterfaceID& interfaceID()
        {
            return IID_IPerformanceTruthTool;
        }
    
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

        virtual StatusCode printParticle(int i) = 0;       

        virtual StatusCode getTruthMuons(TruthMuonList& truthMuonList) = 0;
    
        struct TruthTrack
        {
	    int		 PDG_ID;    /** True PDG ID*/
	    int 	 PDG_Mother;/** True Mothe's PDG ID*/
            double       Phi;       /**< True phi */
            double       Eta;       /**< True eta */
	    double 	 Theta;	    /**< True theta */
            double       Pt;        /**< True pT */
	    double       d0;        /**< True d0 */
	    double       z0;        /**< True d0 */
	    double 	 QOverP;    /**< True QOverP */
            int          Charge;    /**< Charge */
            double       DPhi;      /**< phi(Truth) - phi(ID) */
            double       DEta;      /**< eta(Truth) - eta(ID) */
            double       DPt_Pt;    /**< (pT(Truth) - pT(ID)) / (pT(Truth) */
	   
        };
    
        //virtual StatusCode getTruthTrack(const Rec::TrackParticle* pParticle, TruthTrack& truthTrack) = 0;
        virtual StatusCode getTruthTrack(const xAOD::TrackParticle* pParticle, TruthTrack& truthTrack, CandidateSummary* summary) = 0;
	virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple) = 0;
    	//virtual StatusCode fillNTuple(const Rec::TrackParticle* pParticle, CandidateSummary *summary=NULL)=0;
    	virtual StatusCode fillNTuple(CandidateSummary *summary)=0;
    
        virtual StatusCode getTruthTrack(const MuonFeature* muonFeature, TruthTrack& truthTrack, CandidateSummary* summary) = 0;
        virtual StatusCode getTruthTrack(const TrigMuonEFTrack* muonEFTrack, TruthTrack& truthTrack, CandidateSummary* summary) = 0;
    };


} // end of namespace

#endif 
