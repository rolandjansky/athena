/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalPixelTool.h
 * Implementation of inner detector global hits monitoring tool
 *
 * @author Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 *
 * $Id: InDetGlobalPixelTool.h,v 1.3 2009-04-23 16:55:24 kastanas Exp $
 *
 *********************************************************************************/


#ifndef InDetGlobalPixelTool_H
#define InDetGlobalPixelTool_H

//Local includes
#include "InDetGlobalMotherMonTool.h"
//Standard c++
#include <string>
//Predeclarations
class TH1F;
class TH2F;
class IInterface;
class StatusCode;

///Template monitoring tool derived from InDetGlobalMotherMonTool
class InDetGlobalPixelTool : public InDetGlobalMotherMonTool
{    
   public:
      ///Constructor
      InDetGlobalPixelTool( const std::string & type, 
            const std::string & name,
            const IInterface* parent); 

      ///Virtual destructor
      virtual ~InDetGlobalPixelTool() {}

      ///@name Book, fill and proc histograms
      ///@{

      ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
      virtual StatusCode bookHistograms(); 	

      ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
      virtual StatusCode fillHistograms();

      ///@} 

   private:

      std::string m_detector;

      const TrackCollection *m_tracks;  //container for tracks
      std::string m_TracksName;

      TH1F*   m_Pixel_track_clus_groupsize; 
      TH1F*   m_Pixel_track_cluster_ToT;            
      TH1F*   m_Pixel_track_res_phi;               
      TH1F*   m_Pixel_track_pull_phi;                   
      TH1F*   m_Pixel_track_res_eta;                  
      TH1F*   m_Pixel_track_pull_eta;                   
      TH2F*   m_Pixel_track_ecA_cluster_occupancy;
      TH2F*   m_Pixel_track_ecC_cluster_occupancy;     
      TH2F*   m_Pixel_track_bar_lay0_cluster_occupancy;
      TH2F*   m_Pixel_track_bar_lay1_cluster_occupancy;
      TH2F*   m_Pixel_track_bar_lay2_cluster_occupancy;
      TH1F*   m_Pixel_track_Lvl1A;
      TH1F*   m_Pixel_track_chi2;  
      TH1F*   m_Pixel_track_qOverP;
      TH1F*   m_Pixel_track_d0;   
      TH1F*   m_Pixel_track_z0;   
      TH1F*   m_Pixel_track_phi0;  
      TH1F*   m_Pixel_track_theta; 
      TH1F*   m_Pixel_track_eta;  
};

#endif
