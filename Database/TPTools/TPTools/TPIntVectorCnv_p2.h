/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T_ATHENAPOOLTPINTVECTORCNV_H
#define T_ATHENAPOOLTPINTVECTORCNV_H

/** @file TPIntVectorCnv.h
 *  @brief This file contains TP converter template specializations 
 *  for converters using TPIntegerVector_p2 as persistent representation type
 *  @author Marcin.Nowak@cern.ch
 **/

#include "TPConverter.h"
#include "AthenaPoolUtilities/TPIntegerVector_p2.h"

#include <stdexcept>


template< class TRANS_BASE, class TRANS >
class TPAbstractPolyCnvBase< TRANS_BASE, TRANS, TPIntegerVector_p2 >
   : public ITPConverterFor< TRANS_BASE > 
{
public:
  typedef TRANS			Trans_t;
  typedef TPIntegerVector_p2	Pers_t;
  
  TPAbstractPolyCnvBase() :  m_pStorage(0)
  {}
  
  virtual ~TPAbstractPolyCnvBase() {}


  // conversion methods - to be implemented in the concrete converter
  //                      -------------------------------------------
  
  /** Convert persistent representation to transient one. Copies data
      members from persistent object to an existing transient one.
      Needs to be implemented by the developer on the actuall converter. 
      @param persObj [IN] persistent object
      @param transObj [IN] transient object
      @param log [IN] output message stream
  */
   virtual void persToTrans(const Pers_t* persObj, TRANS* transObj, MsgStream &log) = 0;

  /** Convert transient representation to persistent one. Copies data
      members from transient object to an existing persistent one.
      Needs to be implemented by the developer on the actuall converter. 
      @param transObj [IN] transient object
      @param persObj [IN] persistent object
      @param log [IN] output message stream
  */  
  virtual void transToPers(const TRANS* transObj, Pers_t* persObj, MsgStream &log) = 0;

  /// @copydoc ITPCnvBase::persToTransUntyped()
  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log)
  {
    persToTrans (reinterpret_cast<const Pers_t*> (pers),
                 reinterpret_cast<TRANS*> (trans),
                 log);
  }

  /// @copydoc ITPCnvBase::transToPersUntyped()
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log)
  {
    transToPers (reinterpret_cast<const TRANS*> (trans),
                 reinterpret_cast<Pers_t*> (pers),
                 log);
  }
  
  // Default implementations (usually no need to overwrite)
  // ------------------------------------------------------

  /** Create persistent representation of a transient object.
      Simply creates a new persistent object and calls transToPers()
      @param transObj [IN] transient object
      @param log [IN] output message stream
      @return the created persistent representation
  */
//  virtual Pers_t* createPersistent(const TRANS* transObj, MsgStream &log);

  /** Convert transient object to persistent representation. Stores the
      result in the storage vector of the top-level object and returns
      a TP Ref to it.
      @param trans [IN] transient object
      @param log [IN] output message stream
      @return TP reference to the persistent representation
  */
  TPObjRef toPersistent_impl( const TRANS *trans, MsgStream &log );

  
  /// @copydoc ITPCnvBase::transientTInfo()
  virtual const std::type_info& transientTInfo() const { return typeid(TRANS); }

//protected:
  // various virtual "redirector" methods

  /** Convert persistent representation stored in the storage vector
   * of the top-level object to transient. Internal.
   @param index [IN] index of the persistent representation in the
   storage vector
   @param trans [IN] empty transient object
   @param log [IN] output message stream   
  */
  virtual void	pstoreToTrans( unsigned index, TRANS_BASE *trans, MsgStream &log ) {
     assert (index < m_pStorage->size());
     Pers_t pers = (*m_pStorage)[index];
     persToTrans( &pers, dynamic_cast<TRANS*>(trans), log );
  }

  /** @copydoc ITPConverterFor::virt_toPersistent()
      Here toPersistent_impl is invoked with the dynamic cast of the
      transient type pointer to it's actual type
  */
  virtual TPObjRef virt_toPersistent( const TRANS_BASE *trans, MsgStream &log )  {
     return toPersistent_impl( dynamic_cast<const TRANS*>(trans), log);
  }

  /** This method implements a pure virtual base class method, but
   * should never be called, as abstract type can not be
   * instantiated.  If it is called, it is a TP converter design error
   */
  virtual TRANS* virt_createTransFromPStore( unsigned, MsgStream& ) {     
     throw std::runtime_error(
	std::string("virt_createTransFromPStore() mothod not supported in TP converter for an abstract class: ")
	+ typeid(*this).name() );
     return 0;
  }

public:
  /** Tell this converter which storage vector it should use to store
   * or retrieve persistent representations
   @param storage [IN] the address of the storage vector
   */
  void	setPStorage( TPIntegerVectorStorage_p2 *storage ) {
     m_pStorage = storage;
  }

  /// Reserve 'size' elements for persistent storage - not much sense in this clas
  virtual void	reservePStorage( size_t size ) {
     m_pStorage->reserve( size );
  }
  
protected:
  /// the address of the storage vector for persistent representations
  TPIntegerVectorStorage_p2 	*m_pStorage;
};



// --------------------------------------------------------------

// Base converter template for polymorphic types

// Converters for objects in the same inheritance tree must share the same
// base class type:  TRANS_BASE

/** @class  TPPolyCnvBase
    Base TP converter template  parametrized by transient and persistent types.
    Intended as a base class for TP converters for non-abstract /b
    polymorphic transient types.  

    TRANS - the transient type
    Pers_t - the persistent representation
    TRANS_BASE - common transient base type for all transient objects
    belonging to the same inheritance tree. For the root of that tree,
    TRANS == TRANS_BASE
    Note: Converters for polymorphic types in the same inheritance tree must
    have the same template parameter base class type:  TRANS_BASE
*/

template< class TRANS_BASE, class TRANS >
class TPPolyCnvBase<  TRANS_BASE, TRANS, TPIntegerVector_p2 >
   : public TPAbstractPolyCnvBase< TRANS_BASE, TRANS, TPIntegerVector_p2 > 
{
public:
  TPPolyCnvBase() {}
  virtual ~TPPolyCnvBase() {}

  typedef TPIntegerVector_p2 Pers_t;

  // Default implementations (usually no need to overwrite)
  // ------------------------------------------------------

  /** Create transient representation of a persistent object.
      Simply creates a new transient object and calls persToTrans()
      @param persObj [IN] persistent object
      @param log [IN] output message stream
      @return the created transient object
  */
  virtual TRANS* createTransient(const Pers_t* persObj, MsgStream &log) {
     std::auto_ptr<TRANS>	trans(new TRANS());
     persToTrans(persObj, trans.get(), log);
     return(trans.release());
  }
  
  /** Internal interface method that is used to invoke the real conversion
      method (createTransient)
      @param index [IN] index of the persistent object in the storage vector
      @param log [IN] output message stream
      @return Created transient object (by pointer)
  */
  virtual TRANS* virt_createTransFromPStore( unsigned index, MsgStream &log )  {
     assert (index < this->m_pStorage->size());
     Pers_t pers = (*this->m_pStorage)[index];
     return createTransient( &pers, log );
  }
};



/** @class TPCnvBase
    TP Converter template for a "regular" type.
    Only slighlty specialized (for better performance) version of the
    polymorphic converter template TPPolyCnvBase 
*/
template< class TRANS >
class TPCnvBase< TRANS,  TPIntegerVector_p2 >
   : public TPPolyCnvBase< TRANS, TRANS, TPIntegerVector_p2 >
{
public:
  TPCnvBase() {}
  virtual ~TPCnvBase() {}

  typedef TPIntegerVector_p2 Pers_t;

  // implement these methods without dynamic cast to TRANS

  /// @copydoc TPAbstractPolyCnvBase::virt_toPersistent()
  virtual TPObjRef virt_toPersistent( const TRANS *trans, MsgStream &log ) {
     return toPersistent_impl( trans, log );
  }

  /// @copydoc TPAbstractPolyCnvBase::pstoreToTrans()
  virtual void	pstoreToTrans( unsigned index, TRANS *trans, MsgStream &log ) {
     assert (index < this->m_pStorage->size());
     Pers_t pers = (*this->m_pStorage)[index];
     persToTrans( &pers, trans, log );
  }

};



template< class TRANS_BASE, class TRANS >
TPObjRef
TPAbstractPolyCnvBase<TRANS_BASE, TRANS, TPIntegerVector_p2>::
toPersistent_impl( const TRANS *trans, MsgStream &log )
{
   // make sure there is a storage object
   if( !this->topConverter()->hasTLPersObject() ) {
      this->topConverter()->createTLPersObject();
   }

   unsigned pos =  m_pStorage->size();
   // make a new space in the storage vector
   m_pStorage->grow();

   Pers_t pers = m_pStorage->back();
   transToPers( trans, &pers, log);
   
   return TPObjRef( this->m_pStorageTID, pos );
}







// --------------------------------------------------------------

/** @class TPPtrVectorCnv
  Converter between:
  transient vector of T* (like DataVector<T>)
  and persistent vector<T>
  where T is NOT a polymorphic type!
  Uses converter CONV for the actual TP conversion
*/
template<class TRANS, class CONV>
class TPPtrVectorCnv<TRANS, TPIntegerVector_p2, CONV >
   : public TPCnvBase<TRANS,  TPIntegerVector_p2 > {
public:
  TPPtrVectorCnv() : m_elementCnv(0)  {}

  /** Converts vector of Pers_t::value_type objects to vector of TRANS::value_type objects,
      using converter CONV
      @param persVect [IN] vector of persistent objects
      @param transVect [IN] vector of transient object
      @param log [IN] output message stream
  */
  virtual void persToTrans(const TPIntegerVector_p2 * persVect, TRANS* transVect, MsgStream &log) {
     TPIntegerVector_p2::const_iterator piter = persVect->begin();
     size_t pers_size = persVect->next(piter);
     transVect->clear();
     transVect->reserve( pers_size );
     // convert vector entries one by one
     while( pers_size-- ) {
	transVect->push_back( createTransFromPStore( &m_elementCnv, persVect->next_TPObjRef(piter), log ) );
     }
  }

  /** Converts vector of TRANS::value_type objects to vector of Pers_t::value_type objects,
      using converter CONV
      @param transVect [IN] vector of transient object
      @param persVect [IN] vector of persistent objects
      @param log [IN] output message stream
  */
  virtual void transToPers(const TRANS* transVect, TPIntegerVector_p2* persVect, MsgStream &log) {
     persVect->reserve( 1 + transVect->size()*2 );
     persVect->push_back( transVect->size() );
     // convert vector entries one by one
     for( typename TRANS::const_iterator 
	    it   = transVect->begin(),
	    iEnd = transVect->end();
	  it != iEnd;  
	  ++it ) {
       persVect->push_TPObjRef( toPersistent( &m_elementCnv, *it, log ) );
     }       
  }

protected:
  /// pointer to the TP converter used for vector elements
  CONV 	*m_elementCnv;	
};



// --------------------------------------------------------------
/** @class TPPolyVectorCnv
  Converter between:
  transient vector of T* (like DataVector<T>)
  and persistent vector<T>
  Uses converter CONV for the actual TP conversion.
  T MAY be a POLYMORPHIC type
  (if T is not polymorphic, for performance reasons use TPPtrVectorCnv)
*/
template<class TRANS, class CONV>
class TPPolyVectorCnv<TRANS, TPIntegerVector_p2, CONV >
   : public TPPtrVectorCnv<TRANS, TPIntegerVector_p2, CONV> {
public:
  /// @copydoc TPPtrVectorCnv::transToPers()
  /// Overwritten method that forces toPersistent() to look up the right converter every time
  virtual void transToPers(const TRANS* transVect, TPIntegerVector_p2* persVect, MsgStream &log)
  {
     persVect->reserve( 1 + transVect->size()*2 );
     persVect->push_back( transVect->size() );
     // convert vector entries one by one
     for( typename TRANS::const_iterator 
	    it   = transVect->begin(),
	    iEnd = transVect->end();
	  it != iEnd;  
	  ++it ) {
       persVect->push_TPObjRef( toPersistent( (CONV**)0, *it, log ) );
     }       
  }
};


/*
  Converter between
  transient vector<T> (by value) and persistent vector<TPObjRef>  
*/
/*
template<class TRANS, class PERS, class CONV>
class TPValVectorCnv : public TPCnvBase<TRANS, PERS> {
public:
  TPValVectorCnv()  : m_elementCnv(0) {}

  /// @copydoc TPPtrVectorCnv::persToTrans()
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log) {
     transVect->clear();
     transVect->reserve( persVect->size() );
     // convert vector entries one by one
     typename TRANS::iterator ti = transVect->begin();
     typename PERS::const_iterator pi = persVect->begin(), iEnd = persVect->end();
     for( ; pi != iEnd;  ++pi, ++ti ) {
        fillTransFromPStore( &m_elementCnv, *pi, &*ti, log );
     }
  }
  
  /// @copydoc TPPtrVectorCnv::transToPers()
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) {
     persVect->clear();
     persVect->reserve( transVect->size() );
     // convert vector entries one by one
     for( typename TRANS::const_iterator
             it = transVect->begin(), iEnd = transVect->end();
	  it != iEnd; ++it ) {
        persVect->push_back( toPersistent( &m_elementCnv, &*it, log ) );
     }
  }
  
protected:
  /// pointer to the TP converter used for vector elements
  CONV 	*m_elementCnv;
};

*/

#endif
