/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalMotherMonTool.h
 * Implementation of inner detector global monitoring mother tool
 *
 * @author Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalMotherMonTool.h,v 1.49 2009-04-23 16:55:20 kastanas Exp $
 *
 ****************************************************************************/

#ifndef InDetGlobalMotherMonTool_H
#define InDetGlobalMotherMonTool_H

#define ETA_MAX 2.5
//Local includes
//#include "InDetGlobalManagedMonitorToolBase.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//Detector Managers
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

//Tracking
#include "TrkTrack/TrackCollection.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "CommissionEvent/ComTime.h"
#include "TProfile.h"

#include "LWHists/LWHist.h"

//Standard C++
#include <vector>
#include <string>

//Predeclarations
namespace Trk {
    class Track;
    class IPropagator;
    class ITrackSummaryTool;
	class ITrackHoleSearchTool;
	class TrackStateOnSurface;
}

class TH1;
class TH1F;
class TH1I;
class TH2F;
class TH2I;
class TProfile;
class TProfile2D;
class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile_LW;

/// This tool derives from InDetGlobalMonitorToolBase
/// The Mother tool interfaces to all subtools
class InDetGlobalMotherMonTool : public ManagedMonitorToolBase
{

public:
    ///Constructor    
    InDetGlobalMotherMonTool( const std::string & type, 
			      const std::string & name,
			      const IInterface* parent); 
      
    ///Destructor
    virtual ~InDetGlobalMotherMonTool() {}

    ///@name histos Book, fill and process histograms 
    ///(reimplemented in subclasses)
    ///@{

    /** Book Histograms
     *
     *@param isNewEvensBlock       true if starting new events block
     *@param isNewLumiBlock true if starting new lumi block
     *@param isNewRun       true if starting new run 
     *
     *@return Status 
     */
    // virtual StatusCode bookHistograms( bool isNewEventsBlock, 
    // 				       bool isNewLumiBlock, 
    // 				       bool isNewRun ); 

    // /** Process Histograms
    //  *
    //  *@param isNewRun       true if starting new  
    //  *@param isNewLumiBlock true if starting new lumi block
    //  *@param isNewRun       true if starting new run 
    //  *
    //  *@return Status 
    // */
    // virtual StatusCode procHistograms( bool isEndOfEventsBlock,
    // 				       bool isEndOfLumiBlock,
    // 				       bool isEndOfRun );
    // /** Fill Histograms
    //  *@return Status
    //  */ 
    // virtual StatusCode fillHistograms();
    // ///@} 
    
    ///@name Register histograms
    ///@{
    
    /** Register single histogram
     *
     *@param theGroup  Monitoring group
     *@param h1        Histogram 
     *
     *@return Status
     */
    StatusCode registerHist (
	MonGroup & theGroup, 
	TH1* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH1F* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH1I* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH2F* h2);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH2I* h2);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TProfile* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH1I_LW* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH2I_LW* h2);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH1F_LW* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TH2F_LW* h2);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TProfile_LW* h1);

    StatusCode registerHist (
	MonGroup & theGroup, 
	TProfile2D* h2);

    StatusCode registerHist(
	MonGroup theGroup,
	TH1* h1,
	TH1* original_hist);

    template <class histClass>
    StatusCode registerManHist ( 
	histClass * & target, std::string path, Interval_t interval,
	std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi );
	    target->GetXaxis()->SetTitle( xlabel.c_str() );
	    target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return regHist( target, path, interval );
	}

    
    template <class histClass>
    StatusCode registerHistI ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = histClass::create( name.c_str(), title.c_str(), nbinsx, xlow, xhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );

	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}

    template <class histClass>
    StatusCode registerHistI ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	int nbinsy, double ylow, double yhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = histClass::create( name.c_str(), title.c_str(), 
					nbinsx, xlow, xhi,
					nbinsy, ylow, yhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );
	    
	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    
    template <class histClass>
    StatusCode registerHistIR ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );

	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    
    template <class histClass>
    StatusCode registerHistIR ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	int nbinsy, double ylow, double yhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi,
				    nbinsy, ylow, yhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );
	    
	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    


    /** Make and register new 1d histogram and store in vector
     *
     *@param  theGroup Monitoring group
     *@param  name     Histogram name
     *@param  title    Histogram title
     *@param  h        vector of histograms
     *@param  bins     number of bins
     *@param  low      lower edge  
     *@param  high     upper edge
     *
     *@return Status
     */
    template <class histClass>
    StatusCode registerVHistLW(MonGroup & theGroup, 
			     const std::string& name, 
			     const std::string& title, 
			     std::vector<histClass *>& h, 
			     const int bins, 
			     const float low,
			     const float high)
	{
	    histClass * h1 = histClass::create( name.c_str(),
						title.c_str(),
						bins,
						low, high );
	    StatusCode rc = theGroup.regHist(h1);
	    if (rc != StatusCode::SUCCESS) {  	
		if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" 
							      << name <<endreq;
	    }    
	    h.push_back(h1);
	    return rc ;
	}
    

    template <class histClass>
    StatusCode registerVHist(MonGroup & theGroup, 
			     const std::string& name, 
			     const std::string& title, 
			     std::vector<histClass *>& h, 
			     const int bins, 
			     const float low,
			     const float high)
	{
	    histClass * h1 = new histClass( name.c_str(),
					    title.c_str(),
					    bins,
					    low, high );
	    StatusCode rc = theGroup.regHist(h1);
	    if (rc != StatusCode::SUCCESS) {  	
		if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" 
							      << name <<endreq;
	    }    
	    h.push_back(h1);
	    return rc ;
	}
    
    
    
    /** Make and register new 2d histogram and store in vector
     *
     *@param  theGroup Monitoring group
     *@param  name     Histogram name
     *@param  title    Histogram title
     *@param  h        vector of histograms
     *@param  xbins    number of x-bins
     *@param  xlow     lower x-edge  
     *@param  xhigh    upper x-edge
     *@param  ybins    number of y-bins
     *@param  ylow     lower y-edge  
     *@param  yhigh    upper y-edge
     *
     *@return Status
     */
    template <class histClass2D>
    StatusCode registerVHistLW( MonGroup & theGroup,
			      const std::string& name, 
			      const std::string& title, 
			      std::vector<histClass2D *>& h,
			      const int xbins, 
			      const float xlow,
			      const float xhigh,
			      const int ybins, 
			      const float ylow,
			      const float yhigh )
	{
	    histClass2D * h2 = histClass2D::create( name.c_str(),
						    title.c_str(),
						    xbins,
						    xlow, xhigh,
						    ybins,
						    ylow,yhigh );
	    
	    StatusCode rc = theGroup.regHist(h2);
	    if (rc != StatusCode::SUCCESS) {  
		if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram : " 
							      << name <<endreq;
	    } 
	    
	    h.push_back(h2); 
	    return rc ;
	}

    template <class histClass2D>
    StatusCode registerVHist( MonGroup & theGroup,
			      const std::string& name, 
			      const std::string& title, 
			      std::vector<histClass2D *>& h,
			      const int xbins, 
			      const float xlow,
			      const float xhigh,
			      const int ybins, 
			      const float ylow,
			      const float yhigh )
	{
	    histClass2D * h2 = new histClass2D( name.c_str(),
						title.c_str(),
						xbins,
						xlow, xhigh,
						ybins,
						ylow,yhigh );
	    
	    StatusCode rc = theGroup.regHist(h2);
	    if (rc != StatusCode::SUCCESS) {  
		if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram : " 
							      << name <<endreq;
	    } 
	    
	    h.push_back(h2); 
	    return rc ;
	}
    
    
    ///@} 



    /** Check if vector contains value
     *
     * @param ivector vector to check
     * @param totest  value to test for
     * 
     *@return -1 if not found, totest if found
     */
    inline int vecContains(
	const std::vector<int> &ivector, 
	const int &totest) const
	{	   
	    return (find(ivector.begin(),
			 ivector.end(),
			 totest) 
		    == ivector.end())? -1 : totest;
	}

    /**  Set the adresses of track collections to those initialized by InDetGlobalManager in all sub-tools.
     *
     * @param combined_tracks Collection of combined tracks
     * @param pix_tracks Collection of Pixel segments
     * @param sct_tracks  Collection of SCT segments
     * @param trt_tracks Collection of TRT segments
     *
     * @return void
     */

    inline virtual void  fillTracks(
	const TrackCollection* sct_tracks, 
	const TrackCollection* trt_tracks, 
	const TrackCollection* combined_tracks,
	const TrackCollection* pix_tracks) 
	{ 
	    m_sct_tracks = sct_tracks; 
	    m_trt_tracks = trt_tracks; 
	    m_combined_tracks = combined_tracks; 
	    m_pix_tracks = pix_tracks;
	}
   
    /// Filling the detector managers
    inline virtual void  fillDetectorManagers( const TRT_ID *trtID, 
					       const SCT_ID *sctID, 
					       const PixelID *pixelID ) 
      {    
	m_trtID  = trtID; 
	m_sctID  = sctID; 
	m_pixelID  = pixelID;
      }
    
    /// RDO containers
    inline virtual void  fillRDOContainers(const BCM_RDO_Container* bcmRdoContainer,
					   const PixelRDO_Container* pixRdoContainer,
					   const SCT_RDO_Container * sctRdoContainer,
					   const InDet::TRT_DriftCircleContainer* driftCircleContainer) 
      { 
	m_BCM_RDO = bcmRdoContainer;
	m_pixRdoContainer = pixRdoContainer;
	m_sctRdoContainer = sctRdoContainer;
	m_driftCircleContainer = driftCircleContainer;
      }

    /// Timing collections (BCID, LVL1ID, TRT event phase)
    inline virtual void  fillTimeContainers(const InDetTimeCollection* pixLVL1IDContainer,
					    const InDetTimeCollection* sctLVL1IDContainer,
					    const InDetTimeCollection* trtLVL1IDContainer,
					    const InDetTimeCollection* pixel_BCIDColl,
					    const InDetTimeCollection* sct_BCIDColl,
					    const InDetTimeCollection* trt_BCIDColl,
					    const ComTime* trtPhase)
      {
        m_pixLVL1IDContainer = pixLVL1IDContainer;
        m_sctLVL1IDContainer = sctLVL1IDContainer;
        m_trtLVL1IDContainer = trtLVL1IDContainer;
	m_pixBCIDContainer = pixel_BCIDColl;
	m_sctBCIDContainer = sct_BCIDColl;
        m_trtBCIDContainer = trt_BCIDColl;
	m_trtPhase = trtPhase;
      }

    inline virtual void  retrieveTools()
      {
	if (m_trkSummaryTool.retrieve().isFailure() ) {
	  if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "Could not retrieve the track summary tool!" << endreq;
	}
      }

    
    template <class Hist>
    inline void ExpandLBHist(Hist * newHist, Hist * origHist)
	{
	    unsigned int nbins = origHist->GetNbinsX();
	    for (unsigned ix = 1; ix <= nbins; ++ix) 
	    {
		double content = origHist->GetBinContent(ix);
		if (content!=0) {
		    newHist->SetBinContent(ix, content);
		}
	    }
	}
    
    inline void ExpandLBHist(TProfile * newHist, TProfile * origHist)
	{
	    newHist->Add(origHist);
	}


    
    /*    /// Extrapolator
    inline  void  fillExtrapolationTools(
	Trk::IPropagator* propagator, 
	Trk::IExtrapolator* extrapolator) 
	{ 
	    m_propagator = propagator; 
	    m_extrapolator = extrapolator; 
	    }      */
    
protected:
  
    //... To be instanciated by job option for each MonTool (to be reviewed)

    ///True if offline mode, false if online mode
    bool m_isOnline;

    ///True if this is a cosmics run, false for all other types
    bool m_isCosmicsRun;

    ///Track geometry: up, low or all
    std::string m_geo;
    
    ///Check rate used in histograms
    int m_checkRate; 


    ///Track limit
    int m_trackMax;      

    ///Track bin number
    int m_trackBin;

    ///d0 limits
    int m_d0Max;
    
    ///z0 limits
    int m_z0Max;

    ///Number of bins in eta histograms
    int m_nBinsEta;

    ///Number of bins in phi histograms
    int m_nBinsPhi;

    ///Number of events per which some histograms are filled
    int m_perEvent;

    // Geometric layout

    //------ To be instanciated by InDetGlobalMontherMonTool constructor
    int m_evt;


    ///
    int m_LayersB;

    ///
    int m_LayersEC;

    ///@name Track collections (to be reviewed)
    ///@{
    const TrackCollection *m_sct_tracks;
    const TrackCollection *m_trt_tracks;
    const TrackCollection *m_pix_tracks; 
    const TrackCollection *m_combined_tracks;
 
    ///@}

    ///@name Detector managesr
    ///{@

    /// the TRT ID helper
    const TRT_ID *m_trtID;
    
    /// the SCT ID helper 
    const SCT_ID *m_sctID;  

    /// the Pixel ID helper 
    const PixelID *m_pixelID;
    
    ///@}

    ///@ Driftcircles
    typedef InDet::TRT_DriftCircleContainer TRT_DriftCircleContainer;
    typedef InDet::TRT_DriftCircleCollection TRT_DriftCircleCollection;
    typedef InDet::TRT_DriftCircle TRT_DriftCircle;

    const BCM_RDO_Container* m_BCM_RDO;

    const PixelRDO_Container* m_pixRdoContainer;

    const SCT_RDO_Container * m_sctRdoContainer;

    /// the TRT drift circle container
    const TRT_DriftCircleContainer* m_driftCircleContainer;   

    const InDetTimeCollection * m_pixLVL1IDContainer;
    const InDetTimeCollection * m_sctLVL1IDContainer;
    const InDetTimeCollection * m_trtLVL1IDContainer;
    const InDetTimeCollection * m_pixBCIDContainer;
    const InDetTimeCollection * m_sctBCIDContainer;
    const InDetTimeCollection * m_trtBCIDContainer;
    const ComTime * m_trtPhase;

    const ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;


private:

    
};

#endif
