/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPHASESTREAMERTOOL_H
#define TBREC_TBPHASESTREAMERTOOL_H

/*! \brief Used for cutting TBPhase on guard region
 *
 * The \c TBPhaseStreamerTool compares the TBPhase value
 * do a user supplied guard value.  If the TBPhase is within
 * the guard value (either at the zero or 25 nsec end)
 * this tool returns status code failure.
 * The \c Guard property is the cut value.
 *
 * \param Guard   - Guard value in TDC counts
 *
 * \Author Rob McPherson
 * \Date   February 10, 2005 - first implementation
 */

#include "TBRec/TBEventStreamerTool.h"

#include <string>
#include <map>
#include <vector>

class TBPhaseStreamerTool : public TBEventStreamerTool
{
 public:

  TBPhaseStreamerTool(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
  virtual ~TBPhaseStreamerTool();

  virtual StatusCode initializeTool();

  virtual StatusCode accept();

 protected:

  float       m_guardValue;
  std::string m_TBPhaseKey;
    
};
#endif // not TBREC_TBPHASESTREAMERTOOL_H
