/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// 
// for some details on weighting event using the SF, see
// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopCommonObjects#B_tagging
// http://indico.cern.ch/getFile.py/access?contribId=4&resId=0&materialId=slides&confId=106205
// http://cdsweb.cern.ch/record/1269912
// 

#ifndef SUSYTOOLS_ANALYSISCALIBRATIONDATAINTERFACEBASE_H
#define SUSYTOOLS_ANALYSISCALIBRATIONDATAINTERFACEBASE_H

#include "Rtypes.h"
#include "TMatrixDSym.h"
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

class TF1;

namespace SUSY 
{

  const double CalibZERO = 1.e-6;
  const double dummyValue = -1.;
  typedef std::pair<double,double> CalibResult;
  // The following result is returned upon error
  const CalibResult dummyResult(dummyValue,dummyValue);
  //  const CalibResult dummyResult(std::make_pair(dummyValue,dummyValue));

  /** @class CalibrationDataInterfaceBase 

      This class provides some common functionality for other classes wishing to
      access Flavour Tagging performance estimates.

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataInterfaceBase : public TNamed {
    public:
      CalibrationDataInterfaceBase();

       /** default destructor */
      virtual ~CalibrationDataInterfaceBase() = 0;
      
      /**
	 Main interface methods accessing the flavour tagging performance information.
	 Note that for both of the following, the label is assumed to adhere to the
	 TruthInfo conventions (see package PhysicsAnalysis/JetTagging/JetTagInfo).
      */

      const std::string& EffCalibrationName(const std::string& flavour) const;
      void setEffCalibrationNames(const std::map<std::string, std::string>& names);
      
      const std::string& SFCalibrationName(const std::string& flavour) const;
      void setSFCalibrationNames(const std::map<std::string, std::string>& names);
      
      /** @brief known variable types that can be used as function arguments */
      enum variableType { kEta, kAbsEta, kPt };

      /** @brief retrieve the variable types for the given object */
      const std::vector<int>& functionArguments(const std::string& object) const;

      /** @brief determine whether a given object has been 'registered'. Note that IOV
	  information is not available here (contrary to COOL access) and hence the only
	  thing that needs to be done is to carry out the computation of function arguments */
      bool existFunctionArguments(const std::string& object) const;

#ifndef SUSY_ATHENA
   		ClassDef(CalibrationDataInterfaceBase,1) 
#endif

    private:

      /** @brief this maps to the possible function arguments below (avoid re-computation for each call) */
      mutable std::map<std::string, std::vector<int> > m_functionArguments;

      /** @brief this simply collects the per-flavour properties.
	  NB there ought to be no need for the 'mutable' qualifier here... */
      mutable std::map<std::string, std::string> m_calibrationEffNames;
      mutable std::map<std::string, std::string> m_calibrationSFNames;
      
      /** @brief utility function for computation of statistical uncertainty */
      double parametricVariance(TF1* fct, const TMatrixDSym* cov, const Double_t* x) const;

    protected:

      /** @brief auxiliary function for string concatenation */
      std::string getBasename (const std::string& OP, const std::string& flavour,
			       const std::string& extra = "", bool SF = true) const;

      /** @ brief common code computing final results from functions and variables */
      CalibResult getResults (const Double_t* x, const Double_t* xsyst,
			      TF1* fct, TF1* fctsys,
			      const TMatrixDSym* cov) const;

      /** @brief auxiliary function to determine function arguments from named object title */
      bool computeFunctionArguments(const std::string& object, const char* ctitle) const;

      /** @brief tagging algorithm name */
      std::string m_taggerName;

      /** @brief operating points: these need to be declared, so they have to be known a priori */
      std::vector<std::string> m_operatingPoints;

    }; 
} // end of namespace

#endif 
