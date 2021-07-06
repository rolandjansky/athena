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

    Gaudi::Property<float> m_pixelThreshold{
      this,
      "PixelThreshold",
      300,
      "Minimum charge in a cluster required to associate a truth particle to a pixel RDO [e's] -- default set to ~noise level = ~1/10 of threshold"
    };

     Gaudi::Property<float> m_sctThreshold{
      this,
      "SCT_Threshold",
      600,
      "Minimum charge in a cluster required to associate a truth particle to a SCT RDO [e's] -- default set to ~1/10 threshold"
    };
  };


}

#endif/*PRD_MULTITRUTHBUILDER_H*/
