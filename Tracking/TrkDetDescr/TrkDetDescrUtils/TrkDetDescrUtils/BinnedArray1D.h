/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY1D_H
#define TRKDETDESCRUTILS_BINNEDARRAY1D_H

#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray1D

    1-dimensional binned arry based on a sorting
    given by the BinUtitlity.

   @author Andreas.Salzburger@cern.ch
   */

  template <class T> class BinnedArray1D : public BinnedArray<T> {

    public:
     /**Default Constructor - needed for inherited classes */
     BinnedArray1D():
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(0){}

     /**Constructor with std::vector and a  BinUtility - reference counted, will delete objects at the end,
        if this deletion should be turned off, the boolean deletion should be switched to false
        the global position is given by object */
     BinnedArray1D(const std::vector< std::pair< SharedObject<const T>, Amg::Vector3D > >& tclassvector, BinUtility* bingen) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(bingen)
      {
        // prepare the binned Array
        if (bingen){
          size_t vecsize = tclassvector.size();
          m_array = new std::vector< SharedObject<const T> >(vecsize);
          for (size_t ivec = 0; ivec < vecsize; ++ivec){
            const Amg::Vector3D currentGlobal(((tclassvector[ivec]).second));
            if (bingen->inside(currentGlobal)){
               (*m_array)[bingen->bin(currentGlobal, 0)] = ((tclassvector)[ivec]).first;
            } else 
                throw GaudiException("BinnedArray1D constructor", "Object outside bounds", StatusCode::FAILURE );  
          }
       }
     }

     /**Copy Constructor - copies only pointers !*/
     BinnedArray1D(const BinnedArray1D& barr) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(0)
      {
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          if (m_binUtility) {
           m_array = new std::vector< SharedObject<const T> >(barr.m_array->size());
           for (unsigned int ient=0; ient<barr.m_array->size() ; ++ient){
            (*m_array)[ient] = (*barr.m_array)[ient];
           }
         }
      }
     /**Assignment operator*/
     BinnedArray1D& operator=(const BinnedArray1D& barr)
      {
        if (this != &barr){

          delete m_array;
          delete m_arrayObjects; 
          m_arrayObjects = 0;
          delete m_binUtility;
          // now refill
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          // --------------------------------------------------------------------------
          if (m_binUtility) {
           m_array = new std::vector< SharedObject<const T> >(barr.m_array->size());
           for (unsigned int ient=0; ient<barr.m_array->size(); ++ient){
            (*m_array)[ient] = (*barr.m_array)[ient];
           }
          }


        }
        return *this;
      }
     /** Implicit Constructor */
     BinnedArray1D* clone() const
     { return new BinnedArray1D(*this); }

     /**Virtual Destructor*/
     ~BinnedArray1D()
      {
        delete m_array;
        delete m_arrayObjects;
        delete m_binUtility;
      }

     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined;
      */
     const T* object(const Amg::Vector2D& lp) const
     { if (m_binUtility->inside(lp)) 
         return ((*m_array)[m_binUtility->bin(lp, 0)]).getPtr();
       return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
         it returns 0 if not defined;
      */
     const T* object(const Amg::Vector3D& gp) const
     { return ((*m_array)[m_binUtility->bin(gp, 0)]).getPtr(); }


     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     const T* entryObject(const Amg::Vector3D& gp) const
     { return ((*m_array)[m_binUtility->entry(gp, 0)]).getPtr(); }

     /** Returns the pointer to the templated class object from the BinnedArray
      */
     const T* nextObject(const Amg::Vector3D& gp, const Amg::Vector3D& mom, bool associatedResult=true) const
     {
      // the bins
      size_t bin = associatedResult ? m_binUtility->bin(gp, 0) : m_binUtility->next(gp, mom, 0);
      return ((*m_array)[bin]).getPtr();
     }

     /** Return all objects of the Array */
     const std::vector< const T* >& arrayObjects() const {
      if (!m_arrayObjects){
        m_arrayObjects = new std::vector<const T*>;
        m_arrayObjects->reserve(arrayObjectsNumber());
        for (size_t ill=0; ill< (m_binUtility->bins(0)); ++ill)
              m_arrayObjects->push_back(((*m_array)[ill]).getPtr());
       }
       return (*m_arrayObjects);
     }

     /** Number of Entries in the Array */
     unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

     /** Return the BinUtility*/
     const BinUtility* binUtility() const { return(m_binUtility); }

    private:
     std::vector< SharedObject<const T> >*                     m_array;        //!< vector of pointers to the class T
     mutable std::vector< const T* >*                          m_arrayObjects; //!< forced 1D vector of pointers to class T
     BinUtility*                                               m_binUtility;   //!< binUtility for retrieving and filling the Array

  };


} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY1D_H

