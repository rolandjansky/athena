/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerTranslatorToolSimple_H
#define TriggerTranslatorToolSimple_H

#include <string>
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/ITriggerTranslatorTool.h"

#include "GaudiKernel/ToolHandle.h"

class IInterface;


/**
 * Provides functionality for users to implement
 * and save histograms, ntuples, and summary data, while allowing a central
 * manager to coordinate and configure many TriggerTranslatorToolSimple instances.
 *
 * $Id: TriggerTranslatorToolSimple.h,v 1.21 2009-05-08 09:52:58 sschaetz Exp $
 *
 * @author Michael Wilson, CERN, January 2007
 */

class TriggerTranslatorToolSimple : public AthAlgTool, virtual public ITriggerTranslatorTool
{
   public:

      TriggerTranslatorToolSimple( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~TriggerTranslatorToolSimple();

      virtual StatusCode initialize() override;

      virtual const std::vector<std::string> translate(const std::string&) const override;




   private:
      std::map<std::string, std::string> m_trigmap_property;
      std::map<std::string, std::vector<std::string>> m_trigmap;


};

#endif
