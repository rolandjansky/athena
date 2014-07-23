/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IMuonEfficiencyScaleFactors.h
 *
 *  Created on: Apr 8, 2014
 *      Author: goblirsc
 */

#ifndef IMUONEFFICIENCYSCALEFACTORS_H_
#define IMUONEFFICIENCYSCALEFACTORS_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// STL include(s):
#include <string>
#include <vector>
#include <map>

/**
* @mainpage MuonEfficiencyCorrections
*
* This package provides information about the muon efficiency and the corresponding scale factors.
*
* supported <b> user configuration via Properties</b>:
*
* set using the setProperty("NameOfProperty",YourInput) method <br>
* <ul>
* <li><b>"WorkingPoint"</b> -> Takes a string</li> <br>
*  supported working points:
* <ul>
* <li> "CB": Combined Muons - if you only run combined muons </li>
* <li> "CBandST": The Medium+ working point.  </li>
* <li> "HighPt": Special cuts for analyses searching for high mass resonances </li>
* <li> "SMT": The muon selection used for the soft muon tagger </li>
* </ul>
* <li><b>"DataPeriod"</b> -> Takes a string </li><br>
*  supported data periods:
* <ul>
* <li> "2011": 7 TeV pp data </li>
* <li> "2012": 8 TeV pp data  </li>
* <li> "2015": 13 TeV pp data </li>
* </ul>
* <li><b>Optional: "doAudit"</b> -> Activates audit mode  (for debugging) </li><br>
*  supported choices:
* <ul>
* <li> true  </li>
* <li> false (default)  </li>
* </ul>
* <li><i><b>OPTIONAL: "LumiWeights"</b></i></li><br>
* -> Assign custom lumi weights to replace the default (AllGood GRL) ones
*
*    Argument should be an std::map <std::string, double>, where the strings are run or period names
*    and the doubles are the lumi values
*
*    Example:  std::map<std::string, double> my_lumis; <br>
*              my_lumis ["A"] = 201.1; <br>
*              my_lumis ["B"] = 600.4; <br>
*              (...) <br>
*              m_effi_corr.setProperty("LumiWeights",my_lumis); <br>
*
*              or <br>
*              my_lumis ["205123"] = 23.5; <br>
*              my_lumis ["205126"] = 42.1; <br>
*              (...) <br>
*              m_effi_corr.setProperty("LumiWeights",my_lumis); <br>
* </ul>
*
*  <b>example use</b>, for 8 TeV data and Medium+ Muons:
*
*  (when initializing)
*
*  CP::MuonEfficiencyCorrections m_effi_corr ("EfficiencyCorrections"); <br>
*
*  m_effi_corr.setProperty("WorkingPoint",CP::MuonEfficiencyWorkingPoint::CBandST); <br>
*  m_effi_corr.setProperty("DataPeriod","2012"); <br>
*  m_effi_corr.initialize();
*
*
*  (when calibrating your muons)
*
*  // either (decorate the muon with the information)
*
*  CHECK(m_effi_corr.applyEfficiencyScaleFactor(*mu)); <br>
*  float my_SF = mu->auxdata< float >( "EfficiencyScaleFactor" ); <br>
*
*  // or (only obtain the SF, without decorating the muon):
*
*  float my_scale_factor = 0; <br>
*  CHECK(m_effi_corr.getScaleFactor(*mu),my_scale_factor); <br>
*
*
*
* see the README for more details
* @author goblirsc@SPAMNOT_CERN.ch
*
*
*
*/



namespace CP {


   class IMuonEfficiencyScaleFactors : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {

       ///
       /// @class IMuonEfficiencyScaleFactors
       /// @brief Interface class.
       /// Abstract interface class. Provides the user interface for the
       /// MuonEfficiencyScaleFactors class.


      ASG_TOOL_INTERFACE( CP::IMuonEfficiencyScaleFactors )

   public:



      /// initialize the tool. Call once all properties are set up to your liking
     virtual StatusCode initialize(void) = 0;


     /// Retrieve the Scale factor
     virtual CorrectionCode getEfficiencyScaleFactor( const xAOD::Muon& mu,
    	  float& sf ) = 0;
     /// decorate the muon with scale factor information
     virtual CorrectionCode applyEfficiencyScaleFactor( xAOD::Muon& mu ) = 0;


     /// Audit trail functionality

     virtual bool AlreadyApplied(const xAOD::Muon & mu)=0;
     virtual bool AlreadyApplied(xAOD::Muon & mu)=0;


      /// BONUS functionality

      /// A less conservative way of treating the SF stat error.
      /// Here, we generate a set of SF replicas,
      /// each by smearing the SF bin by bin with a gaussian with the width of the SF error in that bin
      /// the SF stat systematic is then the variation of the final observable within the replicas

      virtual CorrectionCode getEfficiencyScaleFactorReplicas( const xAOD::Muon& mu,
                                             std::vector<float> & sf_err ) = 0;
      /// decorate the muon with a set of SF replica weights.
      virtual CorrectionCode applyEfficiencyScaleFactorReplicas( xAOD::Muon& mu, int nreplicas=50 ) = 0;

      /// Obtain the muon efficiency measured using the data
      //  Can for example be used to scale truth-level MC
      virtual CorrectionCode getRecoEfficiency( const xAOD::Muon& mu,
    		  float& eff ) = 0;
      /// decorate a muon with the efficiency information
      virtual CorrectionCode applyRecoEfficiency( xAOD::Muon& mu ) = 0;





   }; // class IMuonEfficiencyTool

} // namespace CP




#endif /* IMUONEFFICIENCYSCALEFACTORS_H_ */
