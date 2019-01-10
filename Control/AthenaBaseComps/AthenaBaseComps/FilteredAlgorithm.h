// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABASECOMPS_FILTEREDALGORTITHM_H
#define ATHENABASECOMPS_FILTEREDALGORTITHM_H 1

// STL include files
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>

// Required for inheritance
#include "GaudiKernel/IDataSelector.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

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
