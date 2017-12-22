/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// A tool to add a PRD to PRD_MultiTruthCollection.

#ifndef PRD_MULTITRUTHBUILDER_H
#define PRD_MULTITRUTHBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"

class PixelID;

namespace InDet {
  
  class PRD_MultiTruthBuilder : virtual public IPRD_MultiTruthBuilder,
				public AthAlgTool
  {
  public:
    PRD_MultiTruthBuilder(const std::string& type, const std::string& name, const IInterface* parent);

    virtual void addPrepRawDatum(PRD_MultiTruthCollection *prdTruth,
				 const Trk::PrepRawData* prd,
				 const InDetSimDataCollection* simDataMap,
				 bool pixels
				 ) const;

    virtual StatusCode initialize();
    
  private:
    const PixelID *m_idHelperPixel;
  };


}

#endif/*PRD_MULTITRUTHBUILDER_H*/
