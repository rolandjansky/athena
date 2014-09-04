/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_ANALYSISSELECTION_H
#define ANALYSISUTILS_ANALYSISSELECTION_H

/**
   Utility class to get elements which pass a selection criteria.
   User must provide a class for the selection critaria.
   The class must have a public method "bool isAccepted(Collection::value_type e)".
   
   @author Tadashi Maeno
*/

#include <vector>

namespace AnalysisUtils {

  template <class CRITERIA> struct Selection : public CRITERIA
  {
    
    /** get objects which pass the selection criteria
	@param COLL [in] type of collection
	@param coll [in] pointer to collection
	@param out [out] selected objects
     */
    template <class COLL> void getObjects(const COLL *coll, std::vector<typename COLL::value_type> &out)
    {
      out.clear();

      // loop over all elements
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	// selection
	if (CRITERIA::isAccepted(*it))
	  out.push_back(*it);
    }

    /** get indices of the objects which pass the selection criteria
	@param COLL [in] type of collection
	@param coll [in] pointer to collection
	@param out [out] indices of selected objects
     */
    template <class COLL> void getIndices(const COLL *coll, std::vector<int> &out)
    {
      out.clear();

      // loop over all elements
      int index = 0;
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  // selection
	  if (CRITERIA::isAccepted(*it))
	    out.push_back(index);
	  
	  ++index;
	}
    }
  };

} // end of AnalysisUtils namespace

#endif
