/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_MSCONSTRAINTTRACKSPROVIDER_H
#define TRKALIGNGENTOOLS_MSCONSTRAINTTRACKSPROVIDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "muonEvent/Muon.h"
#include "TrkAlignInterfaces/ITrackCollectionProvider.h"
#include "TrkFitterUtils/FitterTypes.h"  





class TFile;
class TTree;


namespace Muon{
  class IMuonHitSummaryTool;
}


namespace Trk {
  class IGlobalTrackFitter;

  class MSConstraintTracksProvider : virtual public ITrackCollectionProvider, public AthAlgTool {

  public:
    MSConstraintTracksProvider(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~MSConstraintTracksProvider();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    virtual StatusCode trackCollection(const TrackCollection*& tracks);

    virtual void printSummary();

  private :

    bool combinedMuonSelection(const Analysis::Muon*);
    bool bookNtuple();  
    void initializeNtuple();
    void setNtuple(TFile* ntuple);
    StatusCode fillNtuple();

    ToolHandle<IGlobalTrackFitter>        m_trackFitter;     //!< normal track fitter 
    ToolHandle<Muon::IMuonHitSummaryTool> m_muonHitSummaryTool;
 
    std::string 	  m_inputMuonCollection;       //!< Name of input combined muons collection
    std::string		  m_inputTracksCollection;     //!< Name of input tracks collection
    RunOutlierRemoval     m_runOutlierRemoval;         //!< run outlier removal in the GX2 fitter 
    int      		  m_nRetrievalErrors;          //!< number of retrieval errors at the beginning of the job
    int      		  m_maxRetrievalErrors;        //!< maximum allowed number of retrieval errors at the beginning of the job (-1 for infinite)
    bool		  m_useMSConstraintTrkOnly;
    bool 		  m_doTree;

    double   		  m_minPt;
    int     		  m_minPIXHits;
    int    		  m_minSCTHits;
    int    		  m_minTRTHits;
    double 		  m_maxIDd0;
    double 		  m_maxIDz0;
    double 		  m_minIDPt;
    int     	 	  m_minMDTHits;
    int    		  m_minRPCPhiHits;
    int    		  m_minTGCPhiHits;
    double 		  m_maxMSd0;
    double 		  m_maxMSz0;
    double 		  m_minMSPt;
    double 		  m_maxNumberOfSectors;
    double 		  m_minNumberOfPhiLayers;
    double 		  m_minStationLayers;
    int 		  m_nCBMuonsFromSG;
    int			  m_nCBMuonsHasEXandID;
    int 		  m_nCBMuonsPassSelection;
    int 		  m_nCBMuonsFailedRefit;	
    int   		  m_nCBMuonsSucRefit;

    // ntuple variables
    TFile*   		  m_ntuple;
    TTree*   		  m_tree;
    int			  m_run;
    int			  m_event;
    double 		  m_pID;
    double 		  m_pMS;
    double 		  m_ptID;
    double 		  m_ptMS;
    int 		  m_charge;

    double 	 	  m_combinedEta;
    double 		  m_IDEta;
    double 		  m_combinedPhi;
    double 		  m_IDPhi;

    double 		  m_pID_constrained;
    double 		  m_ptID_constrained;
    double 		  m_IDEta_constrained;
    double 		  m_IDPhi_constrained;
    int 		  m_charge_constrained;

    int 		  m_eBLhits;
    int			  m_nBLhits;

    int			  m_nPIXDS;
    int 		  m_nSCTDS;

    int		   	  m_nPIXH;
    int 		  m_nSCTH;

    int 		  m_nPIXHits;
    int 		  m_nSCTHits;
    int 		  m_nTRTHits;

    int 		  m_sectors;
    int 		  m_phiLayers;
    int 		  m_stationLayers;

    int 		  m_sectorNum;
    int 		  m_phiLayerNum;
    int 		  m_stationLayerNum;

  }; // end class

} // end namespace

#endif // TRKALIGNGENTOOLS_MSCONSTRAINTTRACKSPROVIDER_H
