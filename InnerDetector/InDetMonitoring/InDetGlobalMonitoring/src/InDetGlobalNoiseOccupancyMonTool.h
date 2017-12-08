/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalNoiseOccupancyMonTool.h
 * Implementation of inner detector global noise occupancy monitoring tool
 *
 * @author
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalNoiseOccupancyMonTool.h,v 1.28 2009-05-13 13:43:47 kastanas Exp $
 *
 ****************************************************************************/


#ifndef InDetGlobalNoiseOccupancyMonTool_H
#define InDetGlobalNoiseOccupancyMonTool_H

//Local
#include "InDetGlobalMotherMonTool.h"
//Standard C++
#include <string>
#include <vector>
#include <list>
using namespace std;

//Pre declarations
class TH1F_LW;
class TH2F_LW;
class TProfile_LW;
class IInterface;
class StatusCode;

/// Monitoring tool derived from InDetGlobalMotherMonTool
/// Tools to verify correlaction in noise occupancy between the three detectors

class InDetGlobalNoiseOccupancyMonTool : public InDetGlobalMotherMonTool{
  
 public:
  /// Constructor
  InDetGlobalNoiseOccupancyMonTool(const string & type, 
				   const string & name,
				   const IInterface* parent); 
  
  /// Virtual destructor
  virtual ~InDetGlobalNoiseOccupancyMonTool() {}
  
  ///@name Book, fill and proc histograms
  ///@{
  ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistogramsRecurrent();
  
  ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
  virtual StatusCode fillHistograms();
  ///@} 

  
 private:
  /** Check if Drift Circle is on Track
   *@param DCId Drift Circle Identifier
   *@return true if DC is on track
   */
  bool isOnTrack(const Identifier &DCId) const;
  
  /** Get triplet id
   *@paraFGem list 
   *@param rdoId
   *@return
   */
  int histIdx( const vector<int> &aList, 
	       const Identifier &rdoId) const;
  
  
  /* 
   * container for holding calculated NO for each event together
   * Added by m 
   */
  struct noise{
    float trt_no;
    float sct_no;
    float pix_no;
  };
  
  /*
   * new methods for calculating the correlation coefficients
   * Added by m
   */
  void correlation_coeffs(vector<struct noise> &no);
  
  float calc_correlation(float *sum_x, float *sum_y, 
			 float *sum_xy, float *sum_x_sq, float *sum_y_sq,
			 int *points);
  
  int GetSCTCounts(int barrel_ec_sel, bool do_checks_flg);
  int GetPixelCounts(int barrel_ec_sel, bool do_checks_flg);

  
  //-------- private member variables --------
  
  const TrackCollection* m_tracks;
  
  /// Geometric description (to be replaced with database entries)    
  float m_nStraws; 
  float m_nStraws_B;
  float m_nStraws_ECA;
  float m_nStraws_ECC;

  float m_nStrips;
  float m_nStrips_B;
  float m_nStrips_ECA;
  float m_nStrips_ECC;

  float m_nPixels;
  float m_nPixels_B;
  float m_nPixels_ECA;
  float m_nPixels_ECC;

  /// pixel histogram max limit
  float m_pixelMax;    
  int m_pixel_nBins;
  
  ///SCT histogram limit
  float m_sctMax;      
  int m_sct_nBins;
  
  ///TRT histogram limit        
  float m_trtMax;
  int m_trt_nBins;
  
  bool m_start;
  bool m_doData;
  
  //--- Combined noise correleations ---------------------------
  
  ///TRT noise occupancy vs SCT noise occupancy
  TH2F_LW * m_SCT_TRT_NO;
  
  ///PIX noise occupancy vs SCT noise occupancy
  TH2F_LW * m_SCT_PIX_NO;
  
  ///PIX noise occupancy vs TRT noise occupancy
  TH2F_LW * m_PIX_TRT_NO;
  
  ///PIX noise occupancy vs event number   
  TH2F_LW * m_PIX_SCT_TRT_NO;
  
  
  
  /* new correlation coefficient histos to combine TRT, 
   * SCT and PIX NO 
   * Added by m
   */
  
  TH2F_LW * m_TRT_SCTvTRT_PIX_10evt;
  TH2F_LW * m_TRT_SCTvSCT_PIX_10evt;
  TH2F_LW * m_TRT_PIXvSCT_PIX_10evt;
  
  //--- Debug histograms Pixel -------------------------
  
  ///PIX noise occupancy vs event number
  TProfile_LW* m_PIX_NO_evt;
  
  ///PIX noise occupancy
  TH1F_LW * m_PIX_NO;  
  TH1F_LW * m_PIX_NO_B;  
  TH1F_LW * m_PIX_NO_ECA;  
  TH1F_LW * m_PIX_NO_ECC;  
  
  ///PIX noise occupancy vs number of PIX segments
  TH2F_LW * m_PIX_NO_nseg;
  
  ///PIX noise occupancy vs number of PIX segments
  TH2F_LW * m_PIX_NO_ntrk;
  
  //--- Debug histograms SCT -------------------------
  
  ///SCT noise occupancy vs event number
  TProfile_LW* m_SCT_NO_evt;
  
  ///SCT noise occupancy
  TH1F_LW * m_SCT_NO;
  TH1F_LW * m_SCT_NO_B;
  TH1F_LW * m_SCT_NO_ECA;
  TH1F_LW * m_SCT_NO_ECC;

    ///SCT noise occupancy vs number of SCT segments"
  TH2F_LW * m_SCT_NO_nseg;
  
  ///SCT noise occupancy vs number of combined tracks
  TH2F_LW * m_SCT_NO_ntrk;
  
  //--- Debug histograms TRT -------------------------
  
  ///TRT noise occupancy vs event number
  TProfile_LW* m_TRT_NO_evt;

  ///TRT noise occupancy vs phi sector (barrel)
  TProfile_LW* m_TRT_NO_BA_phi;
  TProfile_LW* m_TRT_NO_BC_phi;

  ///TRT noise occupancy vs phi sector (ECA)
  TProfile_LW* m_TRT_NO_ECA_phi;

  ///TRT noise occupancy vs phi sector (ECC)
  TProfile_LW* m_TRT_NO_ECC_phi;
  
  ///TRT HL noise occupancy vs event number
  TProfile_LW* m_TRT_HL_NO_evt;
  
  ///TRT noise occupancy
  TH1F_LW * m_TRT_NO; 
  
  ///TRT HL noise occupancy
  TH1F_LW * m_TRT_HL_NO;
  
  ///TRT noise occupancy vs number of TRT segments
  TH2F_LW * m_TRT_NO_nseg;
  
  ///TRT noise occupancy vs number of combined tracks
  TH2F_LW * m_TRT_NO_ntrk;
  
  /// Temporary (to be verified)
  vector<int> m_phiModBTemp;
  vector<int> m_phiModECATemp;
  vector<int> m_phiModECCTemp;
  
  /// list of trt circle identifiers
  list<Identifier> m_dcidOnTrack;
  
  /* 
   * Vector to hold the noise occupancies so they can
   * be saved for calculating correlation coefficients
   * on several points instead of being over-written
   * each time
   * Added by m
   */  
  vector<struct noise> NO;
};

#endif
