/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
//   MuonAlignmentUncertTool
//      Helper classes to read the scattering and alignment uncertainties
//      of the muons from the histograms
//////////////////////////////////////////////////////////////////////////////

#ifndef MUONCOMBINEDBASETOOLS_MUONALIGNMENTUNCERTTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONALIGNMENTUNCERTTOOL_H

#include <MuidInterfaces/IMuonAlignmentUncertTool.h>
#include <AthenaBaseComps/AthAlgTool.h>
#include <GaudiKernel/ServiceHandle.h>
#include "GaudiKernel/ToolHandle.h"

#include <TrkDetDescrInterfaces/ITrackingVolumesSvc.h>
#include <TrkTrack/TrackStateOnSurface.h>
#include <TrkTrack/Track.h>
#include <TrkGeometry/TrackingVolume.h>
/// Memory for the smart pointers
#include <memory>
/// Numbers are read from histograms
#include <TH1.h>
#include <string>
#include <map>
#include <vector>
#include <functional>

namespace Muon {
    class MuonAlignmentUncertTool : public AthAlgTool, virtual public IMuonAlignmentUncertTool {
        public:
            
            MuonAlignmentUncertTool(const std::string& type, const std::string& name, const IInterface* parent);
            ~MuonAlignmentUncertTool()=default;

            StatusCode initialize() override;
              
            float get_uncertainty(const Trk::Track* trk) const override;
            
            /// Helper method to retrieve the last measurement of the ID, 
            ///  the first and last scatterer in the calorimeter
            /// and finally to retrieve the first measurement inside the MS
           void get_track_state_measures(const Trk::Track* track, const Trk::TrackStateOnSurface*& id_exit,
                                                    const Trk::TrackStateOnSurface*& calo_entrance,
                                                    const Trk::TrackStateOnSurface*& calo_exit,
                                                    const Trk::TrackStateOnSurface*& ms_entrance) const override;
                                                    
        private:
            /// Helper function to assign the uncertainty from the Axis. 
            /// Assignment is based on the title of the axis
            std::function<int(const Trk::Track* trk)> make_reader(const TAxis* axis) const;
           
           
           
            /// Volume service to 
            ///
            ServiceHandle<Trk::ITrackingVolumesSvc>  m_trackingVolumesSvc{this,"TrackingVolumesSvc",
                                                                               "TrackingVolumesSvc/TrackingVolumesSvc"};
            
           
           Gaudi::Property<std::string> m_in_file{this, "InFile", 
                                    "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root", 
                                    "Location of the calibration file"};
           Gaudi::Property<std::string> m_histo_name{this, "HistoName", "", "Name of the histogram in the file"};
          
            
            /// Histogram to be cached
            std::unique_ptr<TH1> m_histo;
            unsigned int m_dim;
            /// Standard functions used to read the histogram having as input
            /// a Trk::Track. 
            std::function<int(const Trk::Track* trk)> m_x_reader;
            std::function<int(const Trk::Track* trk)> m_y_reader;
            std::function<int(const Trk::Track* trk)> m_z_reader;
            
            
            std::unique_ptr<const Trk::Volume>         m_calorimeterVolume;            
            std::unique_ptr<const Trk::Volume>         m_indetVolume;          
    
    };


}  // end of namespace Rec

#endif  
