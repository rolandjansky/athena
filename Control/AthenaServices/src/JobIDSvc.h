///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_JOBIDSVC_H
# define ATHENASERVICES_JOBIDSVC_H
/** @file JobIDSvc.h
 * @brief a service to distribute unique job identifier
 * @author Yushu Yao <yyao@lbl.gov> - ATLAS Collaboration
 *$Id: JobIDSvc.h,v 1.3 2008-09-23 22:00:47 binet Exp $
 */

#include <string>
#include "AthenaKernel/IJobIDSvc.h"
#include "AthenaBaseComps/AthService.h"


/** @class JobIDSvc 
 * @brief a service to distribute unique job identifier
 *
 * @details 
 */
class JobIDSvc : virtual public IJobIDSvc,
	public AthService {
private:
public:
	/// Get current JobID
	virtual PJobID_t getJobID() const;
	virtual std::string toString() const;

	///dump to MsgStream contents of in memory DB
	void dump() const;

	/// Gaudi Service Implementation
	//@{
	virtual StatusCode initialize();
	virtual StatusCode reinitialize();
	virtual StatusCode finalize();
	virtual StatusCode queryInterface(const InterfaceID& riid,
			void** ppvInterface);
	//@}

	// Standard Constructor
	JobIDSvc(const std::string& name, ISvcLocator* svc);

	// Standard Destructor
	virtual ~JobIDSvc() {
	}
	;

private:

	/// @name Properties
	//@{
	/// "JobID": list of db files with (CLID, class_name) entries. Loaded at init in svc maps
	JobID_t m_JobID;
	//@}
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENASERVICES_JOBIDSVC_H
