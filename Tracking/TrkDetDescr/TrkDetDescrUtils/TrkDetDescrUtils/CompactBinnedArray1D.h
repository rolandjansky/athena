/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompactBinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_COMPACTBINNEDARRAY1D_H
#define TRKDETDESCRUTILS_COMPACTBINNEDARRAY1D_H

#include "TrkDetDescrUtils/CompactBinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinMap.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class CompactBinnedArray1D

    1-dimensional binned array with compact packing.
    Does not take ownership of stored objects.

   @author sarka.todorova@cern.ch
   */

  template <class T> class CompactBinnedArray1D : public CompactBinnedArray<T> {

  public:
    /**Default Constructor - needed for inherited classes */
    CompactBinnedArray1D() throw (GaudiException):
    CompactBinnedArray<T>(),
      m_binUtility(0){}
    
    /**Constructor with std::vector and a BinUtility */
    CompactBinnedArray1D(const std::vector< const T*>& tclassvector,const std::vector<size_t>& indexvector,  BinUtility* bingen) throw (GaudiException) :
      CompactBinnedArray<T>(),
      m_array(indexvector),
      m_arrayObjects(tclassvector),
      m_binUtility(bingen)
      {    
	// size of the index vector must correspond to the number of bins in the BinUtility
         if (indexvector.size() != bingen->bins() ) 
	   throw GaudiException("CompactBinnedArray1D", "dimension of index vector out of bounds:", StatusCode::FAILURE); 
        // maximal index must stay within the range of available objects
        unsigned int iMax = 0;
	for (unsigned int i=0; i<indexvector.size(); i++) if (indexvector[i]>iMax) iMax = indexvector[i];
	if (iMax > tclassvector.size()-1) 
	  throw GaudiException("CompactBinnedArray1D", "runaway index", StatusCode::FAILURE);    
      }

     /**Copy Constructor - copies only pointers !*/
     CompactBinnedArray1D(const CompactBinnedArray1D& barr) throw (GaudiException) :
      CompactBinnedArray<T>(),
      m_binUtility(0)
      {
	m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;

        m_array = barr.m_array;
	m_arrayObjects = barr.m_arrayObjects;
      }
     /**Assignment operator*/
     CompactBinnedArray1D& operator=(const CompactBinnedArray1D& barr)
      {
        if (this != &barr){

          delete m_binUtility;
          // now refill
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          // --------------------------------------------------------------------------
          if (m_binUtility) {
	    m_array = barr.m_array;
	    m_arrayObjects = barr.m_arrayObjects;
          }

        }
        return *this;
      }
     /** Implicit Constructor */
     CompactBinnedArray1D* clone() const
     { return new CompactBinnedArray1D(m_arrayObjects,m_array,m_binUtility->clone()); }

     /**Virtual Destructor*/
     ~CompactBinnedArray1D()
      {
        delete m_binUtility;
      }

     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined;
      */
     const T* object(const Amg::Vector2D& lp) const
     { if (m_binUtility->inside(lp) ) 
         return m_arrayObjects[m_array[m_binUtility->bin(lp, 0)]];
       return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
         it returns 0 if not defined;
      */
     const T* object(const Amg::Vector3D& gp) const
     {
       if (m_binUtility) {
         size_t bin = m_binUtility->bin(gp, 0);
         if (bin<0 || bin>=m_array.size()) {
           return 0;
	 }         
         if (m_array[bin]<0 || m_array[bin]>=m_arrayObjects.size()) {
           return 0;
	 }         
	 return m_arrayObjects[m_array[bin]];
       }
       return 0; 
     }

     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     const T* entryObject(const Amg::Vector3D& gp) const
     { 
       if (m_binUtility)
	 return ( m_arrayObjects[m_array[m_binUtility->entry(gp, 0)]]);
       return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
      */
     const T* nextObject(const Amg::Vector3D& gp, const Amg::Vector3D& mom, bool associatedResult=true) const
     {
       if (!m_binUtility) return 0;
       // the bins
       size_t bin = associatedResult ? m_binUtility->bin(gp, 0) : m_binUtility->next(gp, mom, 0);
       return m_arrayObjects[m_array[bin]];
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
     const BinUtility* layerBinUtility(const Amg::Vector3D& ) const { return(m_binUtility); }

     /** Return the layer bin*/
     size_t layerBin(const Amg::Vector3D& pos) const { return(m_binUtility->bin(pos)); }

    /** access to objects */
     Trk::BinMap<T> binMap(const Amg::Vector3D& gp, const Amg::Vector3D& dir, 
			   float min=-1.e-05, float max=1.e05 ) const{

      Trk::BinPath path = m_binUtility->binPath(gp,dir,min,max);

       std::vector<T*> objects;
       for (unsigned int i=0; i<path.steps.size(); i++)  {
         if (path.steps[i].second>=0 && path.steps[i].second<int(m_array.size())) {
	   objects.push_back(m_arrayObjects[m_array[path.steps[i].second]]);
         } else {
	   objects.push_back(0);
	 }
       }

       Trk::BinMap<T> map(objects,path);
       //map.check();
       return map;
     }

    private:
     const std::vector<size_t >                          m_array;        //!< vector of indices to objects
     const std::vector< const T* >                       m_arrayObjects; //!< objects
     BinUtility*                                         m_binUtility;   //!< binUtility 

  };


} // end of namespace Trk

#endif // TRKDETDESCRUTILS_COMPACTBINNEDARRAY1D_H

