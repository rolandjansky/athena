// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABASECOMPS_FILTEREDALGORTITHM_H
#define ATHENABASECOMPS_FILTEREDALGORTITHM_H 1

// STL include files
#ifndef _CPP_MEMORY
 #include <memory>
#endif
#ifndef _CPP_MAP
 #include <map>
#endif
#ifndef _CPP_SET
 #include <set>
#endif
#ifndef _CPP_VECTOR
 #include <vector>
#endif
#ifndef _CPP_STRING
 #include <string>
#endif

// Required for inheritance
#ifndef GAUDIKERNEL_IDATASELECTOR_H
 #include "GaudiKernel/IDataSelector.h"
#endif
#ifndef ATHENABASECOMPS_ATHALGORITHM_H
 #include "AthenaBaseComps/AthAlgorithm.h"
#endif
//get the CLID typedef
#ifndef GAUDIKERNEL_CLASSID_H
 #include "GaudiKernel/ClassID.h"
#endif
#ifndef GAUDIKERNEL_PROPERTY_H
 #include "GaudiKernel/Property.h"
#endif
#ifndef GAUDIKERNEL_SERVICEHANDLE_H
 #include "GaudiKernel/ServiceHandle.h"
#endif

#include "AthenaKernel/IDecisionSvc.h"

/** @class FilteredAlgorithm
  * @brief algorithm that marks for write data objects in SG
  * 
  * @author srinir@bnl.gov
  * $Id: FilteredAlgorithm.h,v 1.3 2009-01-26 12:48:55 binet Exp $
  */
class FilteredAlgorithm : public AthAlgorithm
{

protected:
  
  /// Vector of names of Algorithms that this stream accepts
  StringArrayProperty      m_acceptNames;
  /// Vector of names of Algorithms that this stream requires
  StringArrayProperty      m_requireNames;
  /// Vector of names of Algorithms that this stream is vetoed by
  StringArrayProperty      m_vetoNames;

  ServiceHandle<IDecisionSvc> m_decSvc;

protected:
  /// Standard algorithm Constructor
  FilteredAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~FilteredAlgorithm();

public:
  /// \name implement IAlgorithm
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();
  //@}

  /// Test whether this event should be output
  bool isEventAccepted( ) const;


  FilteredAlgorithm(); //> not implemented
  FilteredAlgorithm (const FilteredAlgorithm&); //> not implemented
  FilteredAlgorithm& operator= (const FilteredAlgorithm&); //> not implemented
};

#endif // !ATHENABASECOMPS_FILTEREDALGORITHM_H
