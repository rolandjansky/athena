//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_GAIA_NN_TOOL_C
#define BTAGTOOL_GAIA_NN_TOOL_C

/******************************************************
    @class GaiaNNTool
    Package : JetTagTools 
    Created : September 2013
    DESCRIPTION:
    This class computes the Neural Network probability for being b-jet, 
    c-jet or uds-jet for a single event. 
    @authors Dan Guest, Luke de Oliveira
********************************************************/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/ITagTool.h"

#include "xAODBTagging/BTagging.h"

#include <vector>
#include <map>
#include <string>

// class declarations
namespace agile {
    class network_client;
}

namespace Analysis {
    class CalibrationBroker;
}

namespace Analysis {
    
    class GaiaNNTool : public AthAlgTool, virtual public ITagTool
    {
        public:
            GaiaNNTool(const std::string& name, 
                        const std::string& n, 
                        const IInterface*);

            virtual ~GaiaNNTool();
            StatusCode initialize();
            StatusCode finalize();
            void setOrigin(const xAOD::Vertex* priVtx);

            virtual StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);

            void finalizeHistos() {};
        
        // internal typdefs
        private:
            typedef std::map<std::string, agile::network_client*> nn_map;
            typedef std::map<std::string, double> var_map;
        
        // functions
        private:
            
            // load the calibration file from the COOL db
            void load_calibration_file();
            void cache_calibration(const std::string& jetauthor);
            std::string get_calib_string(std::string jetauthor, std::string name="calib");

            // load input variables from xAOD
            void fill_ip3d(var_map& inputs, xAOD::BTagging* BTag);
            void fill_jetfitter(var_map& inputs, xAOD::BTagging* BTag);
            void fill_svp(var_map& inputs, xAOD::BTagging* BTag);
        
        // data members
        private:
            // variables to load the Gaia calibration file
            std::string m_calibrationDirectory;
            std::string m_calibrationSubDirectory;
            ToolHandle<CalibrationBroker> m_calibrationTool; // pointer to calibration in COOL
            std::string m_xAODBaseName;
            
            // container information
            std::string m_ip3d_infosource;
            std::string m_jftNN_infosource;
            std::string m_sv1_infosource;
            
            // map holding neural network calibrations ( jet_author -> NN )
            nn_map m_networks;

            // constants
            double m_min_pt;
            double m_max_abs_eta;
            
            /** This switch is needed to indicate what to do. The algorithm can be run to produce
                reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
                (m_runModus=1) where already made reference histograms are read.*/ 
            std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)
            
            /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
            // this pointer does not need to be deleted in the destructor (because it
            // points to something in storegate)
            const xAOD::Vertex* m_priVtx;
            
    }; // end class
  
    inline void GaiaNNTool::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }


}// end Analysis namespace

#endif



