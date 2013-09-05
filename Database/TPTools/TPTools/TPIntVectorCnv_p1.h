/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLTPVECTORCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLTPVECTORCNV_H

#include "AthenaPoolUtilities/TPIntegerVector.h"
#include "TPConverter.h"


template< class TRANS_BASE, class TRANS >
class TPAbstractPolyCnvBase < TRANS_BASE, TRANS, TPIntegerVector_p1 > : public ITPConverterFor< TRANS_BASE > 
{
public:
  typedef TRANS		Trans_t;
  typedef TPIntegerVector_p1	PERS;
  
  TPAbstractPolyCnvBase() :m_pStorage(0), m_curRecLevel(0){}
  
  virtual ~TPAbstractPolyCnvBase() {}


  // conversion methods - to be implemented in the concrete converter
  //                      -------------------------------------------
  
  /** Convert persistent representation to transient one. Copies data
      members from persistent object to an existing transient one.
      Needs to be implemented by the developer on the actuall converter. 
      @param persObj [IN] persistent object
      @param transObj [IN] transient object
      @param log [IN] output message stream
  **/
   virtual void persToTrans(const PERS* persObj, TRANS* transObj, MsgStream &log) = 0;

  /** Convert transient representation to persistent one. Copies data
      members from transient object to an existing persistent one.
      Needs to be implemented by the developer on the actuall converter. 
      @param transObj [IN] transient object
      @param persObj [IN] persistent object
      @param log [IN] output message stream
  **/  
  virtual void transToPers(const TRANS* transObj, PERS* persObj, MsgStream &log) = 0;

  /// @copydoc ITPCnvBase::persToTransUntyped()
  virtual void persToTransUntyped(const void* pers, void* trans,MsgStream& log){
    persToTrans (reinterpret_cast<const PERS*> (pers),
                 reinterpret_cast<TRANS*> (trans),
                 log);
  }

  /// @copydoc ITPCnvBase::transToPersUntyped()
  virtual void transToPersUntyped(const void* trans, void* pers, MsgStream& log){
    transToPers (reinterpret_cast<const TRANS*> (trans),
                 reinterpret_cast<PERS*> (pers),
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
//  virtual PERS* createPersistent(const TRANS* transObj, MsgStream &log);

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
     PERS pers = (*m_pStorage)[index];
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
  void	setPStorage( TPIntegerVectorStorage_p1 *storage ) {
     m_pStorage = storage;
     m_curRecLevel = 0;
  }

  /// Reserve 'size' elements for persistent storage - not much sense in this clas
  virtual void	reservePStorage( size_t size ) {
     m_pStorage->reserve( size );
  }
  
protected:
  /// the address of the storage vector for persistent representations
  TPIntegerVectorStorage_p1 	*m_pStorage;

  /// count recursive invocations, to detect recursion
  int			m_curRecLevel;

  /// if true, work in recursion-safe way (slower)
  bool			m_recursive;
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
    PERS - the persistent representation
    TRANS_BASE - common transient base type for all transient objects
    belonging to the same inheritance tree. For the root of that tree,
    TRANS == TRANS_BASE
    Note: Converters for polymorphic types in the same inheritance tree must
    have the same template parameter base class type:  TRANS_BASE
*/

template< class TRANS_BASE, class TRANS >
class TPPolyCnvBase<  TRANS_BASE, TRANS, TPIntegerVector_p1 >  : public TPAbstractPolyCnvBase< TRANS_BASE, TRANS, TPIntegerVector_p1 > 
{
public:
  TPPolyCnvBase() {}
  virtual ~TPPolyCnvBase() {}

  typedef TPIntegerVector_p1 PERS;

  // Default implementations (usually no need to overwrite)
  // ------------------------------------------------------

  /** Create transient representation of a persistent object.
      Simply creates a new transient object and calls persToTrans()
      @param persObj [IN] persistent object
      @param log [IN] output message stream
      @return the created transient object
  */
  virtual TRANS* createTransient(const PERS* persObj, MsgStream &log) {
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
     PERS pers = (*this->m_pStorage)[index];
     return createTransient( &pers, log );
  }
};



/** @class TPCnvBase
    TP Converter template for a "regular" type.
    Only slighlty specialized (for better performance) version of the
    polymorphic converter template TPPolyCnvBase 
*/
template< class TRANS >
class TPCnvBase< TRANS,  TPIntegerVector_p1 >
   : public TPPolyCnvBase< TRANS, TRANS, TPIntegerVector_p1 >
{
public:
  TPCnvBase() {}
  virtual ~TPCnvBase() {}

  typedef TPIntegerVector_p1 PERS;

  // implement these methods without dynamic cast to TRANS

  /// @copydoc TPAbstractPolyCnvBase::virt_toPersistent()
  virtual TPObjRef virt_toPersistent( const TRANS *trans, MsgStream &log ) {
     return toPersistent_impl( trans, log );
  }

  /// @copydoc TPAbstractPolyCnvBase::pstoreToTrans()
  virtual void	pstoreToTrans( unsigned index, TRANS *trans, MsgStream &log ) {
     assert (index < this->m_pStorage->size());
     PERS pers = (*this->m_pStorage)[index];
     persToTrans( &pers, trans, log );
  }

};




template< class TRANS_BASE, class TRANS >
TPObjRef
TPAbstractPolyCnvBase<TRANS_BASE, TRANS, TPIntegerVector_p1>::
toPersistent_impl( const TRANS *trans, MsgStream &log ){
   // make sure there is a storage object
//   if( !this->topConverter()->hasTLPersObject() ) {
//      this->topConverter()->createTLPersObject();
//   }
   // make a new space in the storage vector
   m_pStorage->grow();

   if( m_curRecLevel > 0 ) {
      throw std::runtime_error(
	 std::string("T/P converter: ") + typeid(*this).name()
	 + " recursive usage not supported! " );
   }
   m_curRecLevel++;
   PERS pers = m_pStorage->back();
   transToPers( trans, &pers, log);
   m_curRecLevel--;
   
   return TPObjRef( this->m_pStorageTID, m_pStorage->size()-1 );
}


#endif



