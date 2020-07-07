/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAINTERFACETOOL_H
#define ANALYSISCALIBRATIONDATAINTERFACETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagCalibration/CalibrationBroker.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceBase.h"
#include "CalibrationDataInterface/ICalibrationDataInterfaceTool.h"
#include "Rtypes.h"
#include <vector>
#include <string>
#include <utility>

class AtlasDetectorID;
class Identifier;
class Jet;

namespace Analysis 
{

  class CalibrationDataVariables;

  /** @class CalibrationDataInterfaceTool 

      This tool provides an interface to flavour tagging performance estimates.

      A separate instance should be used for each different tagging algorithm.
      For each instance, all appropriate jet collections and tagger operating points
      need to be specified.

      The model:
      - b-jets:
      *   data-MC scale factor (factorised 2D function of eta, pt)
      *   MC reference
      *   the product is the data efficiency; alternatively, the scale factor may be used
      - c-jets:
      *   as for b-jets, but with a different MC reference
      - light-flavour jets:
      *   data-MC scale factor (factorised 2D function of eta, pt)
      *   MC reference
      Besides the results, it is also possible to retrieve associated uncertainties. This need not
      be configured, and a choice as to the uncertainty component can be made on a case-by-case
      basis.

      To retrieve results from COOL, a CalibrationBroker instance is used (a separate one
      from the one used for tagger weight distributions). This allows to use the CalibrationBroker's
      channel aliasing (although possibly with different aliases). Of course it is up to the user to
      verify that the aliasing is appropriate!

      DB representation (all residing under a common root folder):
      .../<jet author>/<tagger>/<operating point>/<flavour>/<object name>

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataInterfaceTool :
  public AthAlgTool, virtual public ICalibrationDataInterfaceTool, public CalibrationDataInterfaceBase
    {
    public:
      CalibrationDataInterfaceTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~CalibrationDataInterfaceTool ();
      
      /** allow access to the ICalibrationDataInterfaceTool interface */
      StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /**
	 Main interface methods accessing the flavour tagging performance information.
	 Note that for both of the following, the label is assumed to adhere to the
	 TruthInfo conventions (see package PhysicsAnalysis/JetTagging/JetTagInfo).
      */

      /** efficiency retrieval */
      CalibResult getEfficiency (const Jet& jet, const std::string& label,
				 const std::string& OP, Uncertainty unc = None) const;
      
      /** "MC" efficiency retrieval */
      CalibResult getMCEfficiency (const Jet& jet, const std::string& label,
				   const std::string& OP, Uncertainty unc = None) const;
      
      /** efficiency scale factor retrieval */
      CalibResult getScaleFactor (const Jet& jet, const std::string& label,
				  const std::string& OP, Uncertainty unc = None) const;

    private:

      /** @brief auxiliary function dealing with registration of objects */
      void registerObjects(const std::string& folder, const std::string& OP) const;

      /** @brief auxiliary function to retrieve values of function arguments */
      void retrieveFunctionArguments(const Jet& jet, const std::string& object, Double_t* x) const;

      /** @brief auxiliary function to create the intermediate struct needed for the data layer */
      void makeVariables (const Jet& jet, CalibrationDataVariables& x) const;

      //-------------------- Properties --------------------

      /** @brief calibration curves for b, c, and light-flavour jets */
      std::string m_EffcalibrationBName;
      std::string m_EffcalibrationCName;
      std::string m_EffcalibrationTName;
      std::string m_EffcalibrationLightName;
      std::string m_SFcalibrationBName;
      std::string m_SFcalibrationCName;
      std::string m_SFcalibrationTName;
      std::string m_SFcalibrationLightName;

      /** @brief pointer to the performance broker */
      ToolHandle<CalibrationBroker> m_broker;

    }; 
} // end of namespace

#endif 
