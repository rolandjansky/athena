/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSTAUTOOLDEFINITIONS_H
#define MUGIRLNSSTAUTOOLDEFINITIONS_H

#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include <map>

namespace MuGirlNS 
{
  /** @class StauTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  shikma bressler <sshikma@tx.technion.ac.il>
  */  
    #define dbg std::cout <<"shikma::"<<__FILE__<<" "<<__LINE__<< std::endl;
    
    #define StauBetaDefault 0.
    #define StauBetaOnEdge 0.1
	
    #define StauTTrackOnEdge 0 
    #define StauTTrackDefault 0.

    #define SPEEDOFLIGHT 0.299792458 	//[m/ns]
    #define RPCINVSIGVEL 4.8            //[ns/m]
    #define RPCRESOLUTION 6.6   // time resolution 3.125
    #define MDTTRESOLUTION 5   //  time resolution
    #define TILECALRESOLUTION 2   //  time resolution
    #define TRTRESOLUTION 5.5  //  time resolution
    #define TDC2NS 0.78125 			//1tdc=0.78125ns

    typedef std::vector<const Muon::MdtDriftCircleOnTrack*> MDCOTList;
    typedef std::vector<MDCOTList> MDCOTLists;

    typedef std::vector<std::vector<const Muon::MuonClusterOnTrack*> > TriggerClusters;
    typedef std::vector<const Muon::MuonSegment*> MdtSegments;
    
    enum StauTechnology
    {
        AOD_TECHS, //currently includes MDT+RPC - The RPC can be be removed using the doRPC=False flag
        
        MDT_TECH,
        MDTT_MU_TECH,
        MDTT_SP_TECH,
        RPC_TECH,
        TILECAL_TECH,
	TRT_TECH,
        
        ALL_TECHS, //MDT RPC TILE TRT
        ALLT_TECHS, //MDTT RPC TILE TRT

        MS_TECHS, //MDT RPC
        MST_TECHS, //MDTT RPC

	NORPC_TECHS, //MDT TILE TRT
	NORPCT_TECHS, //MDTT TILE TRT

	NOTRT_TECHS, //MDT RPC TILE
	NOTRTT_TECHS //MDTT RPC TILE
    };

    inline std::string technologyName(StauTechnology eTech)
    {
        switch(eTech)
        {
            case AOD_TECHS: return "AOD";
            case MDT_TECH: return "MDT";
            case MDTT_MU_TECH: return "MDTT MU";
            case MDTT_SP_TECH: return "MDTT SP";
            case RPC_TECH: return "RPC";
            case TILECAL_TECH: return "TileCal";
	    case TRT_TECH: return "TRT";
            
            case ALL_TECHS: return "All";
            case ALLT_TECHS: return "AllT";
            case MS_TECHS: return "MS";
            case MST_TECHS: return "MST";
            case NORPC_TECHS: return "NoRpc";
            case NORPCT_TECHS: return "NoRpcT";
	    case NOTRT_TECHS: return "NoTrt";
	    case NOTRTT_TECHS: return "NoTrtT";
            default: return std::string();
        }
    }

    struct StauHit
    {
        StauTechnology eTech;
        double mToF;
        double x;
        double y;
        double z;
        Identifier id;
        double e;
        double error;
        double shift;
        bool measuresEta;
        StauHit():
            mToF(-1),
            x(0),
            y(0),
            z(0),
            e(-1),
            error(-1),
            shift(0),
            measuresEta(false)
        {}  
        StauHit(StauTechnology tech, double tof, double ix, double iy, double iz, Identifier iid, double ie, double er = -1, double sh = 0, bool eta = false ):
            eTech(tech),
            mToF(tof),
            x(ix),
            y(iy),
            z(iz),
            id(iid),
            e(ie),
            error(er),
            shift(sh),
            measuresEta(eta)
        {}   
    };
    typedef std::vector<StauHit> StauHits;    

    struct Chi2Contribution
    {
        double chi2;
        double dof;
        Chi2Contribution():
            chi2(-1.),
            dof(-1.)
        {}
    };

    struct BetaChi2
    {
        double beta;
        double chi2;
        double dof;
        std::map<StauTechnology,Chi2Contribution> techContribution;
        BetaChi2():
             beta(StauBetaDefault),
             chi2(-1.),
             dof(-1)
        {}
    };

    struct BetaAvg
    {
        double beta;
        double rms;
        BetaAvg():
            beta(StauBetaDefault),
            rms(-1)
        {} 
    };

    struct MdtStepStationData
    {
        double chi2;
        double dof;
        double numHits;
        double numSegs;
        double distance;  
        double dirEta;
        double dirPhi;
     
    };
    
    struct MdtStepData
    {
        double beta;
        double chi2; //combined chi2 (of all the stations)
	double dof; 
	double totNumHits; 
	double hitsInSegments; 
	double numSegs;
	std::vector< MdtStepStationData* >* pStationDataList;
	double tTrack;
    };

    struct MdttStepData
    {
        double beta;
        double chi2;
        double dof;
    };
        
    struct RpcStepStationData
    {
        double chi2;
	double numHits;
// 	double distance;
    };
    
    struct RpcStepData
    {
        double beta;
	double chi2;
        double dof;
	double totNumHits;
	std::vector< RpcStepStationData* >* pStationDataList;
	double tTrack;
    };
    
    struct TgcStepData
    {
        double beta;
        double tTrack;
    };
    
    struct StauTileCalCell
    {
        double x;
	double y;
	double z;
	double error;
	double distance;
	double recoTime;
        double energy; 
	Identifier id;
    };
    typedef std::vector<StauTileCalCell*> StauTileCalCells;
    
    struct TileCalStepData
    {
        double beta;
	double chi2;
        double dof;
	double numOfCells;
	double tTrack;
    };
    
    struct TrtStepData
    {
    	double beta;
    	double chi2;
    	double dof;
    	double tTrack;
	double error;
	int nHits;
	TrtStepData():
	beta(0),
	chi2(0),
        dof(0),
	tTrack(0),
	error(0),
	nHits(0)
	{} 
    };
    
    struct GlobalFitData
    {
        double beta;
	double chi2;
	double dof;
	double hitsOnTrk;
	double outliersOnTrk;
	double p;
	double tTrack;
    };
    
    struct FcnStepData
    {
        double beta;
        double tTrack;
	double chi2;
	bool isBarrel;
	bool isGlobalFit;
	double dof;
	MdtStepData* mdtData;
	RpcStepData* rpcData;
	TgcStepData* tgcData;
	TrtStepData* trtData;
	GlobalFitData* globalFitData;
	TileCalStepData* tileCalData;
	MdttStepData* mdttData;
    };
    
    typedef std::vector< FcnStepData* > FcnStepsData;
    
    struct Range
    {
	unsigned int i_first;
	unsigned int i_last;
	unsigned int i_minChi2;
	double minChi2;
	double minChi2Param;
	int nHits;
	double minChi2_nHits;
	double min;
	double max;
	Range():
	i_first(0),
	i_last(0),
	i_minChi2(0),
	minChi2(-1),
	minChi2Param(0),
	nHits(-1),
	minChi2_nHits(0),
	min(0),
	max(0)
	{}

	void clear()
	{
		i_first = 0;
		i_last = 0;
		i_minChi2 = 0;
		minChi2 = -1;
		minChi2Param = -1;
		nHits = -1;
		minChi2_nHits = 0; 
		min = 0;
		max = 0;
	}

	bool addStepLast(unsigned int step, int hits, double chi2, double param)
	{
		if (nHits == -1 ) // this is a new range
		{
			i_first = step;
			i_last = step;
			this->nHits = hits;
			minChi2 = chi2;
			minChi2Param = param;
			max = param;
			min = param;
		}
		if (this->nHits != hits || step < i_last ) return false;// sanity check failed
		i_last = step;
		max = param;
		if (minChi2 > 0 && minChi2 < chi2) return true;
		minChi2 = chi2;
		minChi2Param = param;
		minChi2_nHits = chi2/(double) nHits;
		i_minChi2 = step;
		return true;
	}

	bool isBetter(Range r2)
	{
		if (r2.nHits == 0) return true; // something is better than nothing
		if (this->size() < 2 ) return false;
		if (r2.size() < 2 ) return true;
		return (this->nHits > r2.nHits);

		double diffChi2 = this->minChi2 - r2.minChi2;
		double extraHits = this->nHits - r2.nHits;
		
		//std::cout << "E123: dc " << diffChi2 << " eh " << extraHits << " dc/eh < 15 " << (diffChi2 / extraHits < 15) 
		// << " mc this: " << this->minChi2 << "mc that: " << r2.minChi2 <<  std::endl;
		return (diffChi2 / extraHits < 15);
		//return (this->nHits > r2.nHits && diffChi2 / extraHits < 15 );
	}
	int size()
	{
		
		return i_last - i_first + 1; 
	}

    };

} // end of namespace

#define dbgE std::cout << "E123: "<< __LINE__ << " " << __FILE__ << std::endl; 
#endif // MUGIRLNSSTAUTOOLDEFINITIONS_H
