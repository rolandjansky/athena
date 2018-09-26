#ifndef ANNULUSDEBUG_H
#define ANNULUSDEBUG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "TrkTrack/TrackCollection.h"
#include <string>

class AnnulusDebugAlg : public AthAlgorithm
{
  public:
    AnnulusDebugAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~AnnulusDebugAlg(void);

    StatusCode	      initialize(void);
    StatusCode	      execute(void);
    StatusCode	      finalize(void);

  private:
      SG::ReadHandle<TrackCollection>  m_tracks;
};


#endif 




