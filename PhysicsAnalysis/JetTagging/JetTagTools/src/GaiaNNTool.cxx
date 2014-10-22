/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterNNTool.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : January 2008
///
/// Adopted for Gaia by Dan Guest in 2013
///
/// DESCRIPTION:
///
/// This class computes the Neural Network probability for being b-jet, 
/// c-jet or uds-jet for 
/// a single event. The Undergraduate Tagger (TM) is used for training, 
/// C++11 code that avoids ROOT wherever possible.
///
/////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/GaiaNNTool.h"
#include "JetTagTools/AGILEClient.h"

#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/Jet.h"
#include "JetTagTools/JetTagUtils.h"
#include "JetTagCalibration/CalibrationBroker.h"

#include "TObjString.h"

#include <string> 
#include <map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "math.h"

namespace Analysis {
    
    GaiaNNTool::GaiaNNTool(const std::string& name, const std::string& n, const IInterface* p):
        AthAlgTool(name, n,p),
        m_calibrationDirectory("Gaia"),
        m_calibrationSubDirectory("NeuralNetwork"),
        m_calibrationTool("BTagCalibrationBroker"),
        m_min_pt(20*1e3),
        m_max_abs_eta(2.5),
        m_runModus("analysis") 
    {
        
        // python binding
        declareInterface<ITagTool>(this);
        
        // access to XML configuration files for TMVA from COOL:
        declareProperty("calibration_directory", m_calibrationDirectory);
        declareProperty("calibration_subdirectory", m_calibrationSubDirectory);
        declareProperty("calibration_tool", m_calibrationTool);

        // input weights
        declareProperty("inputIP3DSourceName", m_ip3d_infosource = "IP3D");
        declareProperty("inputJFSourceName", m_jftNN_infosource = "JetFitter");
        declareProperty("inputSV1SourceName", m_sv1_infosource = "SVInfoPlus");
        declareProperty("xAODBaseName",      m_xAODBaseName);

        // constants 
        declareProperty("min_pt", m_min_pt);
        declareProperty("max_abs_eta", m_max_abs_eta);
 
        // global configuration
        declareProperty("Runmodus", m_runModus);
    }

    GaiaNNTool::~GaiaNNTool() {
        // delete all the different neural network calibrations
        for ( auto network : m_networks ) {
            delete network.second;
            network.second = 0;
        }         
    }

    StatusCode GaiaNNTool::initialize() {

        // prepare calibraiton tool
        StatusCode sc = m_calibrationTool.retrieve();
        if ( sc.isFailure() ) {
            ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_calibrationTool );
            return sc;
        }
        else {
            ATH_MSG_INFO( "#BTAG# Retrieved: " << m_calibrationTool );
        }

        // load the calibration histograms (i.e. the yaml file with the appropriate weights
        load_calibration_file();
    
        ATH_MSG_INFO(" Initialization of GaiaNNTool successful" );
        return StatusCode::SUCCESS;
    }

    void GaiaNNTool::load_calibration_file() {
        std::string file_name = m_calibrationSubDirectory + "/calib";            // directory in db with calibration file
        m_calibrationTool->registerHistogram(m_calibrationDirectory, file_name); // register the calibration file for later access
    
        ATH_MSG_DEBUG(" #BTAG# Registered NN histograms with directory: " << 
                      m_calibrationDirectory << " and subdirectory " << 
                      m_calibrationSubDirectory);
        
        m_calibrationTool->printStatus();
    }

    StatusCode GaiaNNTool::finalize() { // all taken care of in destructor
        ATH_MSG_INFO(" #BTAG# Finalization of GaiaNNTool successfull" );
        return StatusCode::SUCCESS;
    }

    // ---------------------------------------------------------------------------- tagJet
    StatusCode GaiaNNTool::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag) {

        std::string jetauthor = JetTagUtils::getJetAuthor(jetToTag); // determine the jet's channel
        ATH_MSG_DEBUG("#BTAG# Jet author: " << jetauthor );

        if ( jetauthor.empty() ) {
            ATH_MSG_WARNING(" #BTAG# Hypothesis or jetauthor is empty."
                             " No likelihood value given back. ");
        } 

        // retrieve weights
        double jetpT  = jetToTag.pt();
        double jeteta = jetToTag.eta();
        std::vector<double> weights {0,0,0};

        if ( (jetpT < m_min_pt) || (std::abs(jeteta) > m_max_abs_eta) ) {
            if(m_runModus=="analysis") {
                BTag->setVariable<double>(m_xAODBaseName, "gaia_pu", weights[0] );
                BTag->setVariable<double>(m_xAODBaseName, "gaia_pc", weights[1] );
                BTag->setVariable<double>(m_xAODBaseName, "gaia_pb", weights[2] );
            }
            return StatusCode::SUCCESS;
        }


        
        // check to see if the calibration is cached or not
        cache_calibration(jetauthor);
        
    // Fill Input Information                                                              
    //-------------------------------------------------------------------------------
        var_map inputs;                  // map of input information
        inputs["categ_eta"] = jeteta;
        inputs["categ_pt"]  = jetpT;
        fill_ip3d(inputs, BTag);         // fill IP3D variables
        fill_jetfitter(inputs, BTag);    // fill JetFitter variables
        fill_svp(inputs, BTag);          // fill svp variables
    
    // GAIA Predicitons                                                           
    //-------------------------------------------------------------------------------
        std::map<std::string, double> outputs = m_networks.find(jetauthor)->second->predict(inputs);
    
        ATH_MSG_DEBUG("#BTAG# NN output b: " << outputs["bottom"] << 
                      " c: " << outputs["charm"] << " u: " <<  outputs["light"]);
         
        /** give information to the info class. */
        weights[0] = outputs["light"];
        weights[1] = outputs["charm"];
        weights[2] = outputs["bottom"];
        if(m_runModus=="analysis") {
          BTag->setVariable<double>(m_xAODBaseName, "gaia_pu", weights[0]);
          BTag->setVariable<double>(m_xAODBaseName, "gaia_pc", weights[1]);
          BTag->setVariable<double>(m_xAODBaseName, "gaia_pb", weights[2]);
        }
            
        return StatusCode::SUCCESS;
    }

    void GaiaNNTool::cache_calibration(const std::string& jetauthor) {
        bool update = m_calibrationTool->retrieveHistogram(m_calibrationDirectory, jetauthor, "calib").second;
        
        // check if this network is alread cached, so do nothing
        if ( (!update) && (m_networks.count(jetauthor) == 1) ) {
            return;
        }
        
        // get the yaml calibration
        std::string calib = get_calib_string(jetauthor);
        std::stringstream calib_stream(calib);

        // load it into a new neural net
        std::auto_ptr<agile::network_client> new_net(new agile::network_client);
        new_net->load(calib_stream);
        if (m_networks.count(jetauthor)) {
            delete m_networks[jetauthor];
            m_networks.erase(jetauthor);
        }
        m_networks.insert(std::make_pair(jetauthor, new_net.get()));
        new_net.release();
    }

    std::string GaiaNNTool::get_calib_string(std::string jetauthor, std::string name /* = "calib" */) {
        
        // get the yaml file with the Gaia NN configuration
        std::string file_name = m_calibrationSubDirectory + "/" + name;
        std::pair<TObject*, bool> stringpair = m_calibrationTool->retrieveTObject<TObject>(m_calibrationDirectory, jetauthor, file_name);
        if (stringpair.second == true) {
            m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, jetauthor, name, false);
        }
        TObjString* cal_string = dynamic_cast<TObjString*>(stringpair.first);
        
        if (cal_string == 0){ // catch if no string was found
          std::string fuller_name = m_calibrationDirectory + '/' + 
            jetauthor + '/' + m_calibrationSubDirectory + '/' + name; 
          if (stringpair.first) { 
            fuller_name.append(" [but an object was found]"); 
          }
          throw std::runtime_error("Cannot retrieve string: " + fuller_name);
        } 
        std::string calibration(cal_string->GetString().Data()); 
        return calibration; 
    }

    void GaiaNNTool::fill_jetfitter(var_map& inputs, xAOD::BTagging* BTag) {

        int jf_nvtx     = -1;
        int jf_nvtx1t   = -1;
        int jf_ntrkAtVx = -1;
        int jf_n2tv     = -1;
        float jf_efrc     = -1;
        float jf_mass     = -1;
        float jf_sig3d    = -1;
        float jfvtx_chi2     = -1;
        int   jfvtx_ndof     = -1;
        float jfvtx_phi      = -1;
        float jfvtx_theta    = -1;
        float jfvtx_errphi   = -1;
        float jfvtx_errtheta = -1;
        
        // check if we have vertices
        int jf_nvtx_tmp(0); bool jfitter_ok(false);
        BTag->variable<int>(m_jftNN_infosource, "JetFitter_nVTX", jf_nvtx_tmp);
        if(jf_nvtx_tmp>0) jfitter_ok = true;

        if(jfitter_ok) {
        
            // Get values from the xAOD
            if("JetFitter" == m_jftNN_infosource) { // check if JetFitter is known by the xAOD?
                BTag->taggerInfo(jf_nvtx, xAOD::BTagInfo::JetFitter_nVTX);
                BTag->taggerInfo(jf_nvtx1t, xAOD::BTagInfo::JetFitter_nSingleTracks);
                BTag->taggerInfo(jf_ntrkAtVx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
                BTag->taggerInfo(jf_efrc, xAOD::BTagInfo::JetFitter_energyFraction);
                BTag->taggerInfo(jf_mass, xAOD::BTagInfo::JetFitter_mass);
                BTag->taggerInfo(jf_sig3d, xAOD::BTagInfo::JetFitter_significance3d);
                BTag->taggerInfo(jf_n2tv, xAOD::BTagInfo::JetFitter_N2Tpair);
            }
            else { // get variables explicitely
                BTag->variable<int>(m_jftNN_infosource, "JetFitter_nVTX", jf_nvtx);
                BTag->variable<int>(m_jftNN_infosource, "JetFitter_nSingleTracks", jf_nvtx1t);
                BTag->variable<int>(m_jftNN_infosource, "JetFitter_nTracksAtVtx", jf_ntrkAtVx);
                BTag->variable<float>(m_jftNN_infosource, "JetFitter_energyFraction", jf_efrc);
                BTag->variable<float>(m_jftNN_infosource, "JetFitter_mass", jf_mass);
                BTag->variable<float>(m_jftNN_infosource, "JetFitter_significance3d", jf_sig3d);
                BTag->variable<int>(m_jftNN_infosource, "JetFitter_N2Tpair", jf_n2tv);
            }
 
            // Get information from the PV of jetfitter
            std::vector<float> fittedPosition, fittedCov;
            BTag->variable<std::vector<float> >(m_jftNN_infosource, "fittedPosition", fittedPosition); 
            BTag->variable<std::vector<float> >(m_jftNN_infosource, "fittedCov", fittedCov);           
            double phi, theta, errphi, errtheta;
            enum { jet_phi = 3, jet_theta = 4 };

            // angular distribution
            jfvtx_phi = fittedPosition[jet_phi];
            jfvtx_theta = fittedPosition[jet_theta];
                
            // error calculation
            errphi = fittedCov[jet_phi];
            if ( errphi > 0 ) {
                jfvtx_errphi = sqrt( errphi );
            }
            errtheta = fittedCov[jet_theta];
            if ( errtheta  > 0 ) {
                jfvtx_errtheta = sqrt( errtheta );

            }

            // fit quality variables
            BTag->variable<float>(m_jftNN_infosource, "JetFitter_chi2", jfvtx_chi2);
            BTag->variable<int>(m_jftNN_infosource, "JetFitter_ndof", jfvtx_ndof);
             
        }

        // add variables to input map
        inputs["jf_nvtx"] = jf_nvtx;
        inputs["jf_nvtx1t"] = jf_nvtx1t;   
        inputs["jf_ntrkAtVx"] = jf_ntrkAtVx; 
        inputs["jf_efrc"] = jf_efrc; 
        inputs["jf_mass"] = jf_mass; 
        inputs["jf_sig3d"] = jf_sig3d; 
        inputs["jftwotrackvertex_n"] = jf_n2tv; 
        inputs["jftvtx_phi"] = jfvtx_phi;
        inputs["jftvtx_errphi"] = jfvtx_errphi;
        inputs["jftvtx_theta"] = jfvtx_theta;
        inputs["jftvtx_errtheta"] = jfvtx_errtheta;
        inputs["jftvtx_chi2"] = jfvtx_chi2;
        inputs["jftvtx_ndof"] = jfvtx_ndof;
        
    }

    void GaiaNNTool::fill_ip3d(var_map& inputs, xAOD::BTagging* BTag) {
        // default values 
        int ip3d_ntrk = -1;
        float ip3d_pb = -1;
        float ip3d_pc = -1;
        float ip3d_pu = -1;

        if( m_ip3d_infosource == "IP3D" ) {
            BTag->taggerInfo(ip3d_ntrk, xAOD::BTagInfo::IP3D_ntrk);
            ip3d_pb = BTag->IP3D_pb();
            ip3d_pc = BTag->IP3D_pc();
            ip3d_pu = BTag->IP3D_pu();
        }
        else {
            BTag->variable<int>(m_ip3d_infosource, "IP3D_ntrk", ip3d_ntrk);
            BTag->variable<float>(m_ip3d_infosource, "pb", ip3d_pb);
            BTag->variable<float>(m_ip3d_infosource, "pc", ip3d_pc);
            BTag->variable<float>(m_ip3d_infosource, "pu", ip3d_pu);
        }

        // TODO: enforce ip3d_pu + ip3d_pb + ip3d_pc
        if ( ip3d_pu + ip3d_pb + ip3d_pc != 1. ) {
            ATH_MSG_INFO("#BTAG# : ip3d probabilites don't add up to 1!");
        }

        inputs["ip3d_ntrk"] = ip3d_ntrk;
        inputs["ip3d_pb"]   = ip3d_pb;
        inputs["ip3d_pc"]   = ip3d_pc;
        inputs["ip3d_pu"]   = ip3d_pu; 
    }
    
    void GaiaNNTool::fill_svp(var_map& inputs, xAOD::BTagging* BTag) {
        // default values 
        float svp_chi2   = -1; int   svp_ndof   = -1;
        float svp_cov_xy = -1; float svp_cov_xz = -1; float svp_cov_yz = -1;
        float svp_efrc   = -1;
        float svp_err_x  = -1; float svp_err_y  = -1; float svp_err_z  = -1;
        float svp_mass   = -1;
        int   svp_n2t    = -1;
        int   svp_ntrk   = -1;
        int   svp_ntrkv  = -1;
        float svp_x      = -1; float svp_y      = -1; float svp_z      = -1;
        float svp_norm_dist = -1;
        
        // get vertex information
        bool svp_ok(false);
        std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
        BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices);
        if ( myVertices.size() > 0 && myVertices[0].isValid() ) {
            const xAOD::Vertex* firstVertex = *(myVertices[0]);

            // goodness of fit variables
            float chi2 = firstVertex->chiSquared();
            int   ndof = firstVertex->numberDoF();

            // position variables
            float x    = firstVertex->position().x();
            float y    = firstVertex->position().y();
            float z    = firstVertex->position().z();

            // position variable's associated errors
            const AmgSymMatrix(3)& err = firstVertex->covariancePosition();
            float err_x  = sqrt( err(Trk::x, Trk::x) ); //Amg::error(err, Trk::x);
            float err_y  = sqrt( err(Trk::y, Trk::y) ); //Amg::error(err, Trk::y);
            float err_z  = sqrt( err(Trk::z, Trk::z) ); //Amg::error(err, Trk::z);
            float cov_xy = err(Trk::x, Trk::y);
            float cov_xz = err(Trk::x, Trk::z);
            float cov_yz = err(Trk::y, Trk::z);

            // fill svp variables
            svp_chi2 = chi2;
            svp_ndof = ndof;
            svp_x = x; svp_y = y; svp_z = z;
            svp_err_x = err_x; svp_err_y = err_y; svp_err_z = err_z;
            svp_cov_xy = cov_xy; svp_cov_xz = cov_xz; svp_cov_yz = cov_yz;

            // if we found a vertex, then svp is okay to use
            svp_ok = true;
        }

        if (svp_ok) {
            if (m_sv1_infosource == "SVInfoPlus") {
                BTag->taggerInfo(svp_mass, xAOD::BTagInfo::SV1_masssvx);
                BTag->taggerInfo(svp_efrc, xAOD::BTagInfo::SV1_efracsvx);
                BTag->taggerInfo(svp_n2t, xAOD::BTagInfo::SV1_N2Tpair);
                BTag->taggerInfo(svp_ntrk, xAOD::BTagInfo::SV1_NGTinJet);
                BTag->taggerInfo(svp_ntrkv, xAOD::BTagInfo::SV1_NGTinSvx);
                BTag->taggerInfo(svp_norm_dist, xAOD::BTagInfo::SV1_normdist);
            }
            else {
                BTag->variable<float>(m_sv1_infosource, "masssvx", svp_mass);
                BTag->variable<float>(m_sv1_infosource, "efracsvx", svp_efrc);
                BTag->variable<int>(m_sv1_infosource, "N2Tpair", svp_n2t);
                BTag->variable<int>(m_sv1_infosource, "NGTinJet", svp_ntrk);
                BTag->variable<int>(m_sv1_infosource, "NGTinSvx", svp_ntrkv);
                BTag->variable<float>(m_sv1_infosource, "normdist", svp_norm_dist);
            }
        }

        // fill input map
        inputs["svp_chi2"] = svp_chi2;
        inputs["svp_cov_xy"] = svp_cov_xy;
        inputs["svp_cov_xz"] = svp_cov_xz;
        inputs["svp_cov_yz"] = svp_cov_yz;
        inputs["svp_efrc"] = svp_efrc;
        inputs["svp_err_x"] = svp_err_x;
        inputs["svp_err_y"] = svp_err_y;
        inputs["svp_err_z"] = svp_err_z;
        inputs["svp_mass"]  = svp_mass;
        inputs["svp_n2t"]   = svp_n2t;
        inputs["svp_ndof"]  = svp_ndof;
        inputs["svp_ntrk"]  = svp_ntrk;
        inputs["svp_ntrkv"] = svp_ntrkv;
        inputs["svp_x"] = svp_x;
        inputs["svp_y"] = svp_y;
        inputs["svp_z"] = svp_z;
        inputs["svp_norm_dist"] = svp_norm_dist;
    }

} // end Analysis namespace
