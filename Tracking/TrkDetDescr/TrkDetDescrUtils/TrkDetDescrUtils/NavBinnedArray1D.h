/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NavBinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H
#define TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class NavBinnedArray1D

Avoiding a map search, the templated BinnedArray class can help
ordereing geometrical objects by providing a dedicated BinUtility.

For use within navigation objects, global coordinates/transform refer to
the position within mother navigation object

@author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
*/

  template <class T> class NavBinnedArray1D : public BinnedArray<T> {

public:
    /**Default Constructor - needed for inherited classes */
    NavBinnedArray1D() :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(),
      m_transf(0){}

    /**Constructor with std::vector and a  BinUtility - reference counted, will delete objects at the end,
    if this deletion should be turned off, the boolean deletion should be switched to false
    the global position is given by pointer and then deleted! */
    NavBinnedArray1D(const std::vector< SharedObject<const T> >& tclassvector, BinUtility* bingen, Amg::Transform3D* transform) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(SharedObject<BinUtility>(bingen)),
      m_transf(transform)
    {
            // prepare the binned Array // simplify as the array is ordered when defined
        if (bingen){
            m_array = new std::vector< SharedObject<const T> >(tclassvector);
        }
    }

    /**Copy Constructor with shift */
    NavBinnedArray1D(const NavBinnedArray1D& barr, std::vector< SharedObject<const T> >* vec, Amg::Transform3D& shift) :
      BinnedArray<T>(),
      m_array(vec),
      m_arrayObjects(0),
      m_binUtility(barr.m_binUtility),
      m_transf(new Amg::Transform3D(shift*(*barr.m_transf)))
    {}

    /**Copy Constructor - copies only pointers !*/
    NavBinnedArray1D(const NavBinnedArray1D& barr) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(barr.m_binUtility),
      m_transf(0)
    {
        if (m_binUtility.getPtr()) {
            m_array = new std::vector< SharedObject<const T> >(m_binUtility.getPtr()->bins(0));
            for (size_t ient=0; ient< m_binUtility.getPtr()->bins(0); ++ient){
                (*m_array)[ient] = (*barr.m_array)[ient];
            }
        }
        m_transf = (barr.m_transf) ? new Amg::Transform3D(*(barr.m_transf)) : 0;
    }

    /**Assignment operator*/
    NavBinnedArray1D& operator=(const NavBinnedArray1D& barr)
    {
        if (this != &barr){
        
            delete m_array;
            delete m_arrayObjects; m_arrayObjects = 0;
            m_binUtility.remRef();
            delete m_transf;
           // now refill
            m_binUtility = barr.m_binUtility;
           // --------------------------------------------------------------------------
            if (m_binUtility.getPtr()) {
                m_array = new std::vector< SharedObject<const T> >(m_binUtility.getPtr()->bins(0));
                for (size_t ient=0; ient< m_binUtility.getPtr()->bins(0); ++ient){
                    (*m_array)[ient] = (*barr.m_array)[ient];
                }
            }
            m_transf = (barr.m_transf) ? new Amg::Transform3D(*barr.m_transf) : 0;
        
        }
        return *this;
    }

     /** Implicit Constructor */
    NavBinnedArray1D* clone() const
    { return new NavBinnedArray1D(*this); }

     /**Virtual Destructor*/
    ~NavBinnedArray1D()
    {
        delete m_array;
        delete m_arrayObjects;
        delete m_transf;
    }

     /** Returns the pointer to the templated class object from the BinnedArray,
     it returns 0 if not defined;
     */
     const T* object(const Amg::Vector2D& lp) const
     {
         if (m_binUtility.getPtr()->inside(lp)) return ((*m_array)[m_binUtility.getPtr()->bin(lp)]).getPtr();
         return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
     it returns 0 if not defined;
     */
     const T* object(const Amg::Vector3D& gp) const
     {
       // transform into navig.coordinates
         const Amg::Vector3D navGP((m_transf->inverse())*gp);
         if (m_binUtility.getPtr()->inside(navGP))
             return ((*m_array)[m_binUtility.getPtr()->bin(navGP)]).getPtr();
         return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     const T* entryObject(const Amg::Vector3D&) const
         { return ((*m_array)[0]).getPtr(); }

     /** Returns the pointer to the templated class object from the BinnedArray
     */
     const T* nextObject(const Amg::Vector3D& gp, const Amg::Vector3D& mom, bool associatedResult=true) const {
     // transform into navig.coordinates
         const Amg::Vector3D navGP((m_transf->inverse())*gp);
         const Amg::Vector3D navMom((m_transf->inverse()).linear()*mom);
     // the bins
         size_t firstBin = m_binUtility.getPtr()->next(navGP, navMom, 0);
     // use the information of the associated result
         if (associatedResult){
             if (firstBin<=m_binUtility.getPtr()->max(0))
                 return ((*m_array)[firstBin]).getPtr();
             else return 0;
         }
     // the associated result was 0 -> set to boundary
         firstBin  = (firstBin < m_binUtility.getPtr()->bins(0)) ? firstBin : m_binUtility.getPtr()->max(0);
         return ((*m_array)[m_binUtility.getPtr()->bin(navGP)]).getPtr();
     }

     /** Return all objects of the Array */
     const std::vector< const T* >& arrayObjects() const {
         if (!m_arrayObjects){
             m_arrayObjects = new std::vector<const T*>;
             for (unsigned int ill=0; ill < m_array->size(); ++ill)
                 m_arrayObjects->push_back( ((*m_array)[ill]).getPtr());
         }
         return (*m_arrayObjects);
     }

     /** Number of Entries in the Array */
     unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

     /** Return the BinUtility*/
     const BinUtility* binUtility() const { return(m_binUtility.getPtr()); }

     /** Return the transform*/
     Amg::Transform3D* transform() const { return(m_transf); }

     /** Reposition */
     void updateTransform( Amg::Transform3D& transform ) const {
         Amg::Transform3D* tr = m_transf;
         m_transf = new Amg::Transform3D(transform* (*m_transf));
         delete tr;
     }

 private:
     std::vector< SharedObject<const T> >*                     m_array;        //!< vector of pointers to the class T
     mutable std::vector< const T* >*                          m_arrayObjects; //!< forced 1D vector of pointers to class T
     SharedObject<BinUtility>                                  m_binUtility;   //!< binUtility for retrieving and filling the Array
     mutable Amg::Transform3D*                                 m_transf;      // !< transform into local navigation coordinates

 };

} // end of namespace Trk

#endif // TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H

