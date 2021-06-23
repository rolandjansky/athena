/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file  InDetGlobalSynchMonTool.h
 * Implementation of inner detector global monitoring synch tool
 *
 * @author
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Wouter Husbergen <Wouter.Husbergen@cern.ch> @n 
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 * 
 * $Id: InDetGlobalSynchMonTool.h,v 1.36 2008-12-19 11:02:55 sandaker Exp $
 *
 ****************************************************************************/


#ifndef InDetGlobalSynchMonTool_H
#define InDetGlobalSynchMonTool_H

//Local
#include "InDetGlobalMotherTrigMonTool.h"
//Standard c++
#include <string>

#include "TrkTrack/TrackCollection.h"
#include <map>

//Predeclarations
class IInterface;
class StatusCode;
class TH1I_LW;
class TH2I_LW;
class TProfile_LW;
class TProfile2D;

/** Synch Monitoring Tool
*
*  Concrete monitoring tool derived from InDetGlobalMotherMonTool
*
*@author Anne-Catherine.Le.Bihan@cern.ch
* Heidi Sandaker <Heidi.Sandaker@cern.ch>
* Arshak Tonoyan <Arshak.Tonyoan@cern.ch>
* Thomas Burgess <Thomas.Burgess@cern.ch>  
*/
class InDetGlobalSynchMonTool : public InDetGlobalMotherTrigMonTool
{
public:
    ///Constructor
    InDetGlobalSynchMonTool( const std::string & type, 
			     const std::string & name,
			     const IInterface* parent); 
    
    ///Virtual destructor
    virtual ~InDetGlobalSynchMonTool() {}

    virtual StatusCode initialize();

    ///@name Book & fill (reimplemented from baseclass)
    ///@{
    
    ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistograms() ; 	

    ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
    virtual StatusCode fillHistograms();

    //@} 
  
 private:

    void FillTbinLvl1aTrigger(int x,int y,int count);
    static void fillNTimesFast(TH2I_LW*h,double x,double y, int count);//Could be implemented on the LW hists themselves if the need was there
    
    unsigned int fillTimingHistos(const InDetTimeCollection * collection, 
				  std::vector<TH2I_LW *> & timing, 
				  std::vector<TH2I_LW *> & timing_diff,
				  TProfile_LW * timing_region_overview,
				  TH2I_LW * timing_over,
				  unsigned int (*procFunc)(unsigned int)); 

    static unsigned int getBCID(unsigned int bcid);
    static unsigned int getLVL1ID(unsigned int lvl1id);


    static unsigned int getPixelHits(const PixelRDO_Container * collection);
    unsigned int getSctHits(const SCT_RDO_Container * collection  );

    inline int getDetectorId( int robid )
	{
	    return ( robid & 0xFF0000 ) >> 16;
	};
    
    std::multimap<short, short> m_robMapping;
    std::map<short, short> m_robRegionMapping;
 
    ///@name JobOptions
    ///@{
    /// Given by job options - needed to book histograms       

    const TrackCollection* m_tracks;

    ///SCT n robs
    int m_n_sct_robs_JO;

    ///TRT n robs
    int m_n_trt_robs_JO;

    ///PIX n robs
    int m_n_pix_robs_JO;
    ///@}

    ///@name ROOT Histograms
    ///@{

    ///LVL1ID warning status between different RODS (PIX+SCT+TRT)

    TProfile_LW * m_diff_LVL1ID;
    TH2I_LW * m_diff_Overview_LVL1ID;
//    TH2I_LW * m_diff_PIX_LVL1ID;
//    TH2I_LW * m_diff_SCT_LVL1ID;
//    TH2I_LW * m_diff_TRT_LVL1ID;

    ///BCID warning status between different RODS (PIX+SCT+TRT)
    TProfile_LW * m_diff_BCID;

    ///BCID warning status between SCT and Pixel vs between SCT and TRT
    TH2I_LW * m_diff_Pixel_SCT_TRT_BCID;
    
    ///BCID warning status between the detectors and the ROD's of each detector
    TH2I_LW * m_diff_Overview_BCID;
    
    ///BCID warning status between SCT RODS
    TH2I_LW * m_diff_SCT_BCID;

    ///BCID warning status between TRT RODS
    TH2I_LW * m_diff_TRT_BCID;

    ///BCID warning status beteeen PIX RODS
    ///@note NOT FILLED
    TH2I_LW * m_diff_PIX_BCID; 

    ///BCID difference between SCT and TRT
    TH1I_LW * m_diff_SCT_TRT_BCID;

    ///BCID difference between SCT and PIX
    ///@note NOT FILLED
    TH1I_LW * m_diff_SCT_PIX_BCID;

    ///BCID difference between SCT and TRT vs event number
    TProfile_LW* m_diff_SCT_TRT_BCID_evt;

    ///BCID difference between SCT and PIX
    ///@note NOT FILLED
    TProfile_LW* m_diff_SCT_PIX_BCID_evt;

    std::vector<TH2I_LW *> m_BCID;
    std::vector<TH2I_LW *> m_det_diff_BCID;

    std::vector<TH2I_LW *> m_LVL1ID;
    std::vector<TH2I_LW *> m_det_diff_LVL1ID;

    ///BSID of SCT RODS
    TH2I_LW * m_SCT_BCID;

    ///BCID of TRT RODS
    TH2I_LW * m_TRT_BCID;

    ///BCID of PIX RODS
    ///@note NOT FILLED
    TH2I_LW * m_PIX_BCID;

    /// Number of hits in the sub-detectors and ID in each BCID 
    TProfile * m_Pixel_Hits_BCID;
    TProfile * m_SCT_Hits_BCID;
    TProfile * m_TRT_Hits_BCID;
    TProfile * m_InDet_Hits_BCID;
    TProfile * m_Tracks_BCID;

    ///@}
};

#endif
