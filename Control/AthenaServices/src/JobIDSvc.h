/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_JOBIDSVC_H
#define ATHENASERVICES_JOBIDSVC_H

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IJobIDSvc.h"
#include <string>

/**
 * @class JobIDSvc
 * @brief a service to distribute unique job identifier
 * @author Yushu Yao <yyao@lbl.gov> - ATLAS Collaboration
 */
class JobIDSvc : public extends<AthService, IJobIDSvc> {
public:
  // Standard Constructor
  JobIDSvc(const std::string& name, ISvcLocator* svc);

  /// Get current JobID
  virtual PJobID_t getJobID() const override;

  /// Get current JobID as string
  virtual std::string toString() const override;

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  //@}

private:
  JobID_t m_JobID{};
};

#endif // ATHENASERVICES_JOBIDSVC_H
