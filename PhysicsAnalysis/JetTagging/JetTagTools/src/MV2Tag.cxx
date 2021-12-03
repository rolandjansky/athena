/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackParticle.h"

#include "JetTagTools/MV2Tag.h"

#include "xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "AthenaKernel/Units.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <math.h>       /* hypot */


using Athena::Units::GeV;

namespace Analysis {

  /**
      @class MV2Tag
      BDT-based tagger combining more than 20 upper stream b-tagging variables
      @author K.Mochizuki, L.Vacavant, M.Ughetto
      modified May 2016 J. Shlomi
  */

  MV2Tag::MV2Tag(const std::string& name, const std::string& n, const IInterface* p):
    base_class(name, n,p),
    m_runModus("analysis") {

    // force MV2 to always use a calibration derived from MV2CalibAlias jet collection
    declareProperty("forceMV2CalibrationAlias", m_forceMV2CalibrationAlias = true);
    declareProperty("MV2CalibAlias", m_MV2CalibAlias = "AntiKt4TopoEM");

    // global configuration:
    declareProperty("Runmodus", m_runModus);
    //declareProperty("DecorateMvaInputs", m_decorateBTaggingObj=false);

    declareProperty("xAODBaseName",      m_xAODBaseName);//"MV2c20" or etc.

    // which calibration folder to use
    declareProperty("taggerNameBase", m_taggerNameBase = "MV2");
    declareProperty("taggerName", m_taggerName = "MV2");
    declareProperty("decTagName", m_decTagName = "MV2_inputs");

    declareProperty("defaultvals", m_defaultvals );
    declareProperty("MVTMvariableNames", m_MVTM_name_translations );


  }


  MV2Tag::~MV2Tag() {

  }


  StatusCode MV2Tag::initialize() {

    m_disableAlgo=false;
    m_warnCounter=0;

    m_treeName = "BDT";
    m_varStrName = "variables";

    // prepare readKey for calibration data:
    ATH_CHECK(m_readKey.initialize());

    for (const auto& p : m_MVTM_name_translations) {
      m_MVTM_name_backtrans[p.second] = p.first;
    }

    //m_egammaBDTs.clear();
    return StatusCode::SUCCESS;
  }


  StatusCode MV2Tag::finalize() {
    ATH_MSG_DEBUG("#BTAG# Finalizing MV2.");
    return StatusCode::SUCCESS;
  }

  void MV2Tag::assignProbability(xAOD::BTagging *BTag,
         const std::map<std::string, double> &inputs,
         const std::string& assigned_jet_author) const
  {
     /*
     * #index for this function
     * #1: Preparation of MVA instance using MVAUtils BDT
     * #2: set input variables from MultivariateTagManager inputs map
     * #3: Calcuation of MVA output variable(s)
     * #4: Fill MVA output variable(s) into xAOD
     */

    // #0: Check if disabled or not
    if ( m_disableAlgo ) {
      if (m_warnCounter<20) {
        ATH_MSG_WARNING("#BTAG# .... algorithm was disabled due to mis-configuration problems: no output will be written ... PLEASE WATCH OUT" );
        m_warnCounter++;
      }
    }

    // #1: Preparation of MVA instance using egammaBDT
    /* jet author: */
    std::string author(assigned_jet_author);
    ATH_MSG_DEBUG("#BTAG# Jet author for MV2: " << author);

    if (m_forceMV2CalibrationAlias) {
      author = m_MV2CalibAlias;
    }

    //Retrieval of Calibration Condition Data objects
    SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);

    std::string alias = readCdo->getChannelAlias(author);

    //Retrieve BDT from cond object
    MVAUtils::BDT *bdt(nullptr);
    ATH_MSG_DEBUG("#BTAG# Getting MVAUtils::BDT for "<<m_taggerNameBase);
    bdt = readCdo->retrieveBdt(m_taggerNameBase,author);
    if (!bdt) {
      ATH_MSG_WARNING("#BTAG# No BDT for " << m_taggerNameBase<<" exists in the condition object.. Disabling algorithm.");
      m_disableAlgo=true;
      return;
    }
 
    //Retrieve input variables of BDT from cond object
    std::vector<std::string> inputVars = readCdo->retrieveInputVars(m_taggerNameBase,author, m_taggerNameBase+"Calib/"+m_varStrName);
    std::vector<float> vars = CreateVariables (inputs, inputVars);
    ATH_MSG_DEBUG("#BTAG# nConfigVar"<<vars.size());

    if ( inputVars.size()!=vars.size() ) {
      ATH_MSG_WARNING("#BTAG# Number of expected variables for MVA: "<< vars.size() << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
      m_disableAlgo=true;
      return;
    }


    // #3: Calculation of MVA output variable(s)
    /* compute MV2: */
    double mv2 = -10.;  double mv2m_pb=-10., mv2m_pu=-10., mv2m_pc=-10.;

    if (m_taggerNameBase.find("MV2c")!=std::string::npos) mv2= GetClassResponse(vars, bdt);//this gives back double
      else { //if it is MV2m
        std::vector<float> outputs= GetMulticlassResponse(vars, bdt);//this gives back float
      	//vector size is checked in the function above
      	mv2m_pb=outputs[0]; mv2m_pu=outputs[1]; mv2m_pc=outputs[2] ;
      }

    if (m_taggerNameBase.find("MV2c")!=std::string::npos) ATH_MSG_DEBUG("#BTAG# MV2 weight: " << mv2<<", "<<alias<<", "<<author);
    else ATH_MSG_DEBUG("#BTAG# MV2 pb, pu, pc= " << mv2m_pb<<"\t"<<mv2m_pu<<"\t"<<mv2m_pc<<", "<<alias<<", "<<author);

    // #4: Fill MVA output variable(s) into xAOD
    /** give information to the info class. */
    if(m_runModus=="analysis") {

      if (m_taggerNameBase.find("MV2c")!=std::string::npos) {
        //MV2cXX
        BTag->setVariable<float>(m_xAODBaseName, "discriminant", mv2);
      }else {
        //MV2m
        BTag->setVariable<float>(m_xAODBaseName, "pb", mv2m_pb);
        BTag->setVariable<float>(m_xAODBaseName, "pu", mv2m_pu);
        BTag->setVariable<float>(m_xAODBaseName, "pc", mv2m_pc);
      }
    }

  }//end assign probability

  float MV2Tag::d0sgn_wrtJet(const TLorentzVector& jet, const TLorentzVector& trk, float d0sig) const {
    const double dPhi = jet.DeltaPhi(trk);
    const float d0_sign = sin(dPhi) * d0sig;

    if (std::fabs(d0_sign) < 1e-4)
      return 1.0;

    const float res = d0_sign / std::fabs(d0_sign);
    return res;
  }

  float MV2Tag::z0sgn_wrtJet(float trackTheta, float trackZ0, float jetEta) const {
    const float trackEta = -std::log(std::tan(trackTheta/2.));
    const float zs = (jetEta - trackEta)*trackZ0;
    return (zs>=0. ? 1. : -1.);
  }


  std::vector<float>
  MV2Tag::CreateVariables (const std::map<std::string, double> &inputs,
                           const std::vector<std::string> inputVars) const
  {
    std::vector<float> vals;
    vals.reserve (inputVars.size());

    for (const std::string& varName : inputVars) {
      std::string inputName = varName;
      auto it = m_MVTM_name_backtrans.find (varName);
      if (it != m_MVTM_name_backtrans.end()) inputName = it->second;
      auto it2 = inputs.find (inputName);
      if (it2 == inputs.end()) {
      }
      else {
        double val = it2->second;
        if (std::isnan (val)) {
          val = m_defaultvals.at (varName);
        }
        vals.push_back (val);
      }
    }
    return vals;
  }



}//end namespace Analysis
