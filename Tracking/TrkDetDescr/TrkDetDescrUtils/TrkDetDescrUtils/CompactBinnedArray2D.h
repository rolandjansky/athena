/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompactBinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_COMPACTBINNEDARRAY2D_H
#define TRKDETDESCRUTILS_COMPACTBINNEDARRAY2D_H

#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class CompactBinnedArray2D

    2-dimensional binned array 

   @author sarka.todorova@cern.ch
   */

  template <class T> class CompactBinnedArray2D : public CompactBinnedArray<T> {

  public:
    /**Default Constructor - needed for inherited classes */
    CompactBinnedArray2D() throw (GaudiException):
    CompactBinnedArray<T>(),
      m_binUtility(0){}
    
    /**Constructor with std::vector and a BinUtility */
    CompactBinnedArray2D(const std::vector< const T*>& tclassvector, const std::vector<std::vector<size_t> >& indexarray,
			 const BinUtility* bingen,  const std::vector< Trk::BinUtility*>& bVec) throw (GaudiException) :
      CompactBinnedArray<T>(),
      m_array(indexarray),
      m_arrayObjects(tclassvector),
      m_binUtility(bingen),
      m_buVec(bVec)
      {    
        // check compatibility
        // size of the index array must correspond to the number of bins in the BinUtility
        if (indexarray.size() != bingen->bins() )
	   throw GaudiException("CompactBinnedArray2D", "dimension of index array out of bounds:", StatusCode::FAILURE); 
        // maximal index must stay within the range of available objects
        size_t iMax = 0;
	for (size_t i=0; i<indexarray.size(); i++) {
	  for (size_t j=0; j<indexarray[i].size(); j++) {
	    if (indexarray[i][j]>iMax) iMax = indexarray[i][j];
	  }
	}
	if (iMax > tclassvector.size()-1)      
	  throw GaudiException("CompactBinnedArray2D", "runaway index", StatusCode::FAILURE); 
      }
  
      /**Copy Constructor - copies only pointers !*/
      CompactBinnedArray2D(const CompactBinnedArray2D& barr) throw (GaudiException) :
      CompactBinnedArray<T>(),
      m_array(),
      m_arrayObjects(),
      m_binUtility(0),
      m_buVec()
      {
	m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
	
        m_array = barr.m_array;
	m_arrayObjects = barr.m_arrayObjects;
        m_buVec = barr.m_buVec;
      }
      /**Assignment operator*/
      CompactBinnedArray2D& operator=(const CompactBinnedArray2D& barr)
      {
        if (this != &barr){
	  
          delete m_binUtility;
          // now refill
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          // --------------------------------------------------------------------------
          if (m_binUtility) {
	    m_array = std::vector< std::vector< size_t> > ((barr.m_array));
	    m_arrayObjects = std::vector< const T*> ((barr.m_arrayObjects));
	    m_buVec = std::vector< Trk::BinUtility*> ((barr.m_buVec));
          }

        }
        return *this;
      }
      /** Implicit Constructor */
      CompactBinnedArray2D* clone() const
      { return new CompactBinnedArray2D(m_arrayObjects,m_array,m_binUtility->clone(),m_buVec); }
      
     /**Virtual Destructor*/
      ~CompactBinnedArray2D()
	{
	  delete m_binUtility;
	  for (unsigned int i=0; i<m_buVec.size(); i++) delete m_buVec[i];
	}

      /** Returns the pointer to the templated class object from the BinnedArray,
	  it returns 0 if not defined;
      */
      const T* object(const Amg::Vector2D& lp) const
      { if (m_binUtility->inside(lp)  ) { 
	  size_t bin = m_binUtility->bin(lp,0);
	  if (m_buVec[bin]->inside(lp) ) 
	    return m_arrayObjects[m_array[bin][m_buVec[bin]->bin(lp,0)]];
	}
	return 0;
      }
      
      /** Returns the pointer to the templated class object from the BinnedArray
	  it returns 0 if not defined;
      */
      const T* object(const Amg::Vector3D& gp) const
      {
	if (m_binUtility) {
	  size_t bin = m_binUtility->bin(gp);
	  size_t index = m_array[bin][m_buVec[bin]->bin(gp,0)];
	  return m_arrayObjects[index];
	}
	return 0; 
      }

     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     const T* entryObject(const Amg::Vector3D& gp) const
     { 
       if (m_binUtility) {
         size_t bin = m_binUtility->bin(gp);
         size_t biNext = m_binUtility->entry(gp,0);
	 return ( m_arrayObjects[m_array[biNext][m_buVec[bin]->entry(gp,0)]] );
       }
       return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
      */
     const T* nextObject(const Amg::Vector3D& gp, const Amg::Vector3D& mom, bool associatedResult=true) const
     {
       if (!m_binUtility ) return 0;
       // search in layer utility only
       size_t bin = m_binUtility->bin(gp, 0);
       size_t binSub = m_buVec[bin]->next(gp,mom, 0);
       if (associatedResult) {
	 return m_arrayObjects[m_array[bin][binSub]];
       }
       // evaluate distance
       std::pair<int,float> ds = m_binUtility->distanceToNext(gp,mom,0);
       std::pair<int,float> dl = m_buVec[bin]->distanceToNext(gp,mom,0);
       // the bins
       if (dl.second<ds.second) return m_arrayObjects[m_array[bin][binSub]];

       bin =  m_binUtility->next(gp, mom, 0);
       Amg::Vector3D probe = gp + (ds.second)*mom.normalized();
       binSub = m_buVec[bin]->bin(probe,0);
       return m_arrayObjects[m_array[bin][binSub]];
     }

     /** Return all objects of the Array */
     const std::vector< const T* >& arrayObjects() const {
       return m_arrayObjects;
     }

     /** Number of Entries in the Array */
     unsigned int arrayObjectsNumber() const { return arrayObjects().size() ; }

     /** Return the BinUtility*/
     const BinUtility* binUtility() const { return(m_binUtility); }

     /** Return the BinUtility*/
     const BinUtility* layerBinUtility(const Amg::Vector3D& pos) const { return( m_buVec[m_binUtility->bin(pos)] ); }

     /** Return the layer bin*/
     size_t layerBin(const Amg::Vector3D& pos) const { return(layerBinUtility(pos)->bin(pos)); }

    /** access to objects */
     Trk::BinMap<T> binMap(const Amg::Vector3D& gp, const Amg::Vector3D& dir,
			   float min=-1.e-05, float max=1.e05 ) const{
       Trk::BinPath pTop = m_binUtility->binPath(gp,dir,min, max);
       float lmin = min; float lmax = max;       
       Trk::BinMap<T> merged(lmin,lmax);
       double shift=0.;
       for (unsigned int ibu = 0; ibu<pTop.steps.size()-1; ibu++) {
         if (pTop.steps[ibu].first>0.) shift = pTop.steps[ibu].first;
         if (pTop.steps[ibu].second<0) continue;
         Trk::BinUtility* bu = m_buVec[pTop.steps[ibu].second];
         Amg::Vector3D probe = gp+shift*dir;
         Trk::BinPath bpath = bu->binPath(probe,dir,pTop.steps[ibu].first-shift,pTop.steps[ibu+1].first-shift);
	 std::vector<T*> bObj;
         for (unsigned int il = 0; il<bpath.steps.size(); il++) {
	   if (bpath.steps[il].second>=0 && bpath.steps[il].second<int(m_array[pTop.steps[ibu].second].size())) {
	     bObj.push_back(m_arrayObjects[m_array[pTop.steps[ibu].second][bpath.steps[il].second]]);
	   } else {
	     bObj.push_back(0);
	   }
	 }
	 Trk::BinMap<T> bmap(bObj,bpath);
         merged.mergeIn(bmap,shift);
       }
       return merged;
     }

    private:
     const std::vector< std::vector<size_t> >                   m_array;        //!< vector of indices to objects
     const std::vector< const T* >                              m_arrayObjects; //!< objects
     const BinUtility*                                          m_binUtility;   //!< binUtility 
     const std::vector< BinUtility* >                           m_buVec;        //!< vector of bin utilities for 2nd dim   
  };


} // end of namespace Trk

#endif // TRKDETDESCRUTILS_COMPACTBINNEDARRAY2D_H

