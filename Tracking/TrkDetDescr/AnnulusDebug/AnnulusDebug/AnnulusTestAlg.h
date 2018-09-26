#ifndef ANNULUSTEST_H
#define ANNULUSTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class AnnulusTestAlg : public AthAlgorithm
{
  public:
    AnnulusTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~AnnulusTestAlg(void);

    StatusCode	      initialize(void);
    StatusCode	      execute(void);
    StatusCode	      finalize(void);

  private:


    StatusCode testInsideDistances();
    StatusCode visualInsideTolerance();
    StatusCode visualInsideCovariance();
    StatusCode testConsistencyCartesian();
    StatusCode testBoundsOnSurface();

};


#endif 




