/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 19.10.2007, AUTHOR: STEFFEN KAISER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_NtupleDisplayToolH
#define MuonCalib_NtupleDisplayToolH

//:::::::::::::::::::::::::::::
//:: CLASS NtupleDisplayTool ::
//:::::::::::::::::::::::::::::

/// \class NtupleDisplayTool
///
/// This class serves as display of events in mdt chambers.  
///
/// \author Steffen.Kaiser@cern.ch
///
/// \date 19.10.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//root 
#include "TEllipse.h"
class TCanvas;
class TControlBar;
class TApplication;

class MdtIdHelper;

// STL //
#include <string>

// MuonCalib
#include "MdtCalibInterfaces/IMdtPatRecFitter.h"

namespace MuonGM{
class MuonDetectorManager;
}
class RegionSelectionSvc;

namespace MuonCalib {

class IIdToFixedIdTool;
class MuonCalibSegment;
class NtupleStationId;
class MuonCalibEvent;

class TubeCircle;

class NtupleDisplayTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{

    public:
        // Constructors //
        NtupleDisplayTool(const std::string& t, const std::string& n, const IInterface* p);
        ///< Default constructor.

        inline ~NtupleDisplayTool(){}
        ///< Destructor

        // Methods //
	
	/** tool initialize */
	StatusCode initialize();
	
	/** tool finalize */
	StatusCode finalize();
	
        StatusCode handleEvent(const MuonCalibEvent & event, int evnt_nr, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
        ///< analysis of the given segment of
        ///< event eventnumber, event contains
        ///< the raw hits
        StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
        ///< analysis at the end of the data
        ///< file
	inline bool KeepSegments() const
		{
		return false;
		}

    private:
            //for retrieving the chamber geometry
            const MdtIdHelper* m_mdtIdHelper;
            const MuonGM::MuonDetectorManager* m_detMgr;
            const MuonCalib::IIdToFixedIdTool *m_id_tool;
            
            
            TApplication *m_root;  //to display graphics within athena
            TControlBar *m_bar;    //the control bar
            
            TCanvas *m_canvas;     //canvas for chamber display
            TCanvas *m_canvas_leg; //canvas for legend
         
      
            std::string m_fitter_name; // name of the track fitter
            unsigned int m_nb_hits; //minimum number of hits in displayed segment  
            double m_road_width;    //road width for pattern recognition
            int m_chosen_event;     //to chose a single event 
            double m_scalefactor;   //scale factor for the size of the display
            double m_adc_cut;       //adc cut to define noise hits

            // calibration region //
	    ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
            // line reconstruction //
            IMdtPatRecFitter *m_qfitter;    
            
            // numbers //
            int m_nb_multilayers;
            int m_nb_layers;
            int m_nb_tubes;

            
            std::vector<int> m_colors;  //vector of colors to be used
                        
            // vectors of tube circles //
            std::vector< std::vector< std::vector<TubeCircle*> > > m_tube_circle;
                //used for identification of the tubes 

            std::vector<TubeCircle*> m_tube_circle_lin;
               //used for drawing and resetting
                        
    };


    class TubeCircle : public TEllipse {

    public:
        
        // Constructors //
        TubeCircle();
        ///< Default constructor.
        TubeCircle(std::vector<int> colors, double m_adc_cut = 50.);
        ///< constructor which sets to vector of colors and the adc cut
        TubeCircle(Amg::Vector3D TubePos, std::vector<int> colors, double m_adc_cut = 50.);
        ///< constructor which sets to vector of colors, the adc cut and the tube position

        void setPosition(Amg::Vector3D TubePos); 
        ///< set the tube position
        void setColors(std::vector<int> colors);
        ///< set the vector of colors to be used (size=5), 
        ///< 0: segment hit, 1: raw hit, 2: drift radius > 14.6, 
        ///< 3: adc < 70, 4: drift time < 0 
        void setAdcCut(double adc_cut);
        ///< set adc cut to define noise hit

        void setUsedFlag(bool flag); ///<set used flag
        bool isUsed(void);           ///<get used flag

        void setSegmentHit(double radius, double time, double adc);
        ///< set segment hit(drift radius, drift time, adc)
        
        bool hasSegmentHit(void);
        ///< return flag if the tube has a segment hit

        void setRawHit(double radius, double time, double adc);
        ///< set raw hit(drift radius, drift time, adc)
        
        bool hasRawHit(void);
        ///< return flag if the tube has a raw hit

        
        void reset(void);
        ///< reset the tube to a clean and empty tube

	int tube, layer, multilayer;
    private:

        void setRadius(double radius, double time, double adc, int hit);
        //set the properties of the hit (drift radius, drift time, adc), 
        //hit = 0: segment hit, = 1: raw hit

        bool m_used_flag;           //is the tube used
        bool m_rawhit_flag;         //has the tube a raw hit
        bool m_segmenthit_flag;     //has the tube a segment hit
        double m_adc_cut;           //adc cut to define noi
        std::vector<int> m_colors;  //vector of colors to be used
        
    };

}

#endif
