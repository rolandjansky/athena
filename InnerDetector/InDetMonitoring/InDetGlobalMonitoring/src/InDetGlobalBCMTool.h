/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalBCMTool.h
* Implementation of inner detector global hits monitoring tool
*
* @author Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
*
* $Id: InDetGlobalBCMTool.h,v 1.1 2009-01-19 11:07:24 sandaker Exp $
*
*********************************************************************************/


#ifndef InDetGlobalBCMTool_H
#define InDetGlobalBCMTool_H

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
class InDetGlobalBCMTool : public InDetGlobalMotherMonTool
{    
public:
    ///Constructor
    InDetGlobalBCMTool( const std::string & type, 
			 const std::string & name,
			 const IInterface* parent); 
    
    ///Virtual destructor
    virtual ~InDetGlobalBCMTool() {}

    ///@name Book, fill and proc histograms
    ///@{
  
    ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistogramsRecurrent(); 	

    ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
    virtual StatusCode fillHistograms();

    ///@} 
    
private:
    
    std::string m_detector;
    std::string m_histFolder;
    std::string m_vxContainerName;

    /// Example histogram
    TH1F*  m_nExamplePlot;
    
    // Example variables
    int   m_nExampleInt; 
    
    //BCM Side Histograms    
    TH1F *stat_pulse_position[2][2][2]; //station/pulse/gain
    TH1F *stat_pulse_width[2][2][2]; //station/pulse/gain
    TH1F *stat_pulse_position_gen[2][2]; //station/gain
    TH1F *stat_pulse_width_gen[2][2]; //station/gain
    TH1F *stat_lvl1a[2][2]; //station/gain

    //BCM Detector Histogram
    TH1F* det_lvl1a[8][2];//detector/gain
    TH1F* det_pulse_position[8][2][2];//detector/pulse/gain
    TH1F* det_pulse_width[8][2][2];//detector/pulse/gain
    TH1F* det_pulse_position_gen[8][2];//detector/gain
    TH1F* det_pulse_width_gen[8][2];//detector/gain
    TH2F* det_timewalk[8][2];//detector/gain

    // BCM Global Monitoring Histograms
    TH2F* ChannelVsLvl1a;
    TH2F* ChannelVsBCID;
    TH2F* ChannelVsECR;

    TH1F* hits_lvl1a_all;
    TH2F* timewalk_all;
    TH1F* AbortFraction;
    TH1F* AbortFractionROD0;
    TH1F* AbortFractionROD1;
    TH1F* AbortFractionVsBCID;
    TH1F* AbortFractionROD0VsBCID;
    TH1F* AbortFractionROD1VsBCID;
    TH1F* AbortFractionVsECR;
    TH1F* AbortFractionROD0VsECR;
    TH1F* AbortFractionROD1VsECR;
    TH2F* AbortFractionVsLB;

    TH1F* NumberOfEvents;
    TH1F* pulse_position_all[2];
    TH1F* pulse_width_all[2];
    TH1F* hits_lvl1a[2];//gain
    TH1F* hits_bcid[2];//gain
    TH1F* hitdistribution[2];//gain
    TH1F* pulse_position_gen[2];//gain
    TH1F* pulse_width_gen[2];//gain
    TH1F* strange_signals[2];//gain
    TH1F* hitdistribution_pulse[2][2];//pulse/gain
    TH1F* pulse_position[2][2];//pulse/gain
    TH1F* pulse_width[2][2];//pulse/gain
    TH1F* highocc;

    //Deltat Histograms
    TH1F* deltat_vs_hits[2];//gain
    TH1F* deltat_aligned_vs_hits[2];//gain
    TH2F* deltat_vs_bcid[2];//gain
    TH2F* deltat_aligned_vs_bcid[2];//gain
    TH2F* deltat_vs_lb[2];//gain
    TH2F* deltat_aligned_vs_lb[2];//gain
    TH2F* deltat_vs_ecr[2];//gain
    TH2F* deltat_aligned_vs_ecr[2];//gain
    TH2F* deltat_vs_PrVertex[2];//gain
    TH2F* deltat_vs_pixhits[2];//gain
    TH2F* deltat_vs_pixhitsEC[2];//gain
    TH2F* deltat_vs_pixhitsBR[2];//gain
    TH2F* deltat_vs_scthits[2];//gain
    TH2F* deltat_vs_scthitsEC[2];//gain
    TH2F* deltat_vs_scthitsBR[2];//gain
    TH2F* sct_vs_pix_col[2];//gain
    TH2F* sct_vs_pix_bkg[2];//gain

    //LB
    unsigned int m_current_LB;
};

    class deltat_data{
    public: 
      unsigned int bcid;
      unsigned int ecr;
      unsigned int position;
      unsigned int detector;
	unsigned int lvl1a;
      deltat_data();
      bool operator<(const deltat_data &data);
      // deltat_data min(const deltat_data &data);
      
    };

#endif
