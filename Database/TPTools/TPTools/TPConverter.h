// dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPCONVERTER_H
#define TPCONVERTER_H 1

/** @file TPConverter.h
 *  @brief This file contains template definitions for Transient/Persistent converters
 *  @author Marcin.Nowak@cern.ch
 *  $Id: TPConverter.h 653708 2015-03-12 15:44:41Z ssnyder $
 **/

#include "AthenaKernel/ITPCnvBase.h"
#include "ITPConverter.h"
#include "TopLevelTPCnvBase.h"

#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <stdexcept>
#include <cassert>


/** @class  ITPConverterFor
    Common base class for all TP converters, specialized for a given transient type.
    Defines methods that refer only to the transient type

    In the case of polymorphic types with a common base type TRANS, all converters for them need to  inherit from ITPConverterFor<TRANS> 
*/
template< class TRANS >
class ITPConverterFor : public ITPConverter
{
public:
  typedef TRANS				TransBase_t;
  typedef TransBase_t			Trans_t;
  typedef ITPConverterFor< TRANS >	PolyCnvBase_t;

  ITPConverterFor() :
        m_pStorageTIDvalue(0), m_topConverter(0), m_topConverterRuntime(0),
        m_wasUsedForReading( false )
  {}
  
  virtual ~ITPConverterFor()  = default;

  /** Find converter for a given C++ type ID, that is or ihnerits from CNV type.
      @param cnv [IN] parameter specifying the converter type
      @param t_info [IN] C++ type id for which a converter is sought
      @param log [IN] output message stream
      @return CNV* pointer to the converter, if found
  */
  template < class CNV >
  CNV *
  converterForType( CNV *cnv, const std::type_info& t_info, MsgStream& log ) {
     ITPConverter *c = m_topConverterRuntime->converterForType( t_info );
     cnv = dynamic_cast< CNV* >( c );
     if( !cnv )
	this->converterNotFound( typeid(CNV), c, t_info.name(), log );
     return cnv;
  }

  /** Find converter for a TP type ID (passed in a TP Ref), that is or ihnerits from CNV type.
      @param cnv [IN] parameter specifying the converter type
      @param ref [IN] TP Ref to an object for which a converter is sought
      @param log [IN] output message stream
      @return CNV* pointer to the converter, if found
  */
  template < class CNV >
  CNV *
  converterForRef( CNV *cnv, const TPObjRef& ref, MsgStream& log ) {
     ITPConverter *c = m_topConverterRuntime->converterForRef( ref );
     cnv = dynamic_cast<CNV*>(c);
     if( !cnv )
        this->converterNotFound( ref.typeID(), c, typeid(CNV).name(), log );
     return cnv;
  }

  /** Persistify bass class of a given object and store the persistent represenation in
      the storage vector of the top-level persistent object. The converter is
      located using the transient type from the CNV  parameter, not
      from the object itself (because we need the base type, not the
      actuall type)
      @param cnv [IN/OUT] type of this parameter decides which
      converter will be used. Once the converter is found, this
      pointer will be set so the search is done only once
      @param transObj [IN] transient object
      @param log [IN] output message stream
      @return TPObjRef TP reference to the persistent representation
      stored in the storage vector of the top-level persistent object
  */
  template < class CNV >
  TPObjRef
  baseToPersistent( CNV **cnv, const typename CNV::Trans_t* transObj, MsgStream& log)  {
     if( !*cnv || (*cnv)->wasUsedForReading() ) {
        // don't trust the converter if it was used for reading, find again
	*cnv = converterForType( *cnv, typeid(typename CNV::Trans_t), log );
	if( !*cnv )  return TPObjRef();
        (*cnv)->clearReadingFlag();
     }
//     return (**cnv).toPersistent_impl(transObj, log); 
     return (**cnv).virt_toPersistent(transObj, log); 
  }

  /** Persistify an object and store the persistent represenation in
      the storage vector of the top-level persistent object.
      The correct converter is located using the actuall object type.
      @param cnv [IN/OUT] pointer to the converter, usually 0 at the
      start. Once the right converter is found, this pointer will be set so the search is done only once
      @param transObj [IN] transient object
      @param log [IN] output message stream
      @return TPObjRef TP reference to the persistent representation
      stored in the storage vector of the top-level persistent object
  */  
  template < class CNV >
  TPObjRef toPersistent( CNV **cnv, const typename CNV::TransBase_t* transObj, MsgStream& log) {
     if( !transObj ) return TPObjRef();
     CNV *temp_cnv_p = 0;
     if( !cnv ) cnv = &temp_cnv_p;
     if( !*cnv || (*cnv)->wasUsedForReading() ) {
        // don't trust the converter if it was used for reading, find again
	*cnv = converterForType( *cnv, typeid(*transObj), log );
	if( !*cnv )  return TPObjRef();
        (*cnv)->clearReadingFlag();
     }
     return (**cnv).virt_toPersistent(transObj, log);
  }

  /** Convert persistent object, stored in the the top-level
   * persistent object and referenced by the TP Ref, to transient
   * representation. An empty transient object to be filled in is
   * provided. If converter is not given, it will be found based on
   * the Ref type.
   @param cnv [IN][OUT] pointer to the converter, usually 0 at the
   start. Once the right converter is found, this pointer will be set
   so the search is done only once
   @param ref [IN] TP Ref to the persistent object to be converted
   @param trans [IN] pointer to the empty transient object
   @param log [IN] output message stream
  */
  template < class CNV, class TRANS_T >
  void	fillTransFromPStore( CNV **cnv, const TPObjRef& ref, TRANS_T *trans, MsgStream &log ) {
    if( ref.isNull() )  return;
    CNV *temp_cnv_p = 0;
    if( !cnv ) cnv = &temp_cnv_p;
    // see if we already have a converter and if it is the right one
    if( !*cnv || (*cnv)->typeID().value() != ref.typeID() ) {
       // we don't - find the right converter for ref.typeID()
       *cnv = converterForRef( *cnv, ref, log );
       if( !*cnv ) return;
       (*cnv)->setReadingFlag();
     }
    (**cnv).pstoreToTrans( ref.index(), trans, log );
  }

  /** Create transient representation of a persistent object, stored
   * in the the top-level persistent object and referenced by the TP Ref.
   * If a TP converter is not specified, it will be found based on
   * the Ref type.
   @param cnv [IN][OUT] pointer to the converter, usually 0 at the
   start. Once the right converter is found, this pointer will be set
   so the search is done only once
   @param ref [IN] TP Ref to the persistent object to be converted
   @param log [IN] output message stream
   @return pointer to the created transient represention
  */
  template < class CNV >
  typename CNV::Trans_t*
  createTransFromPStore( CNV **cnv, const TPObjRef& ref, MsgStream& log) {
     if( ref.isNull() )  return 0;
     CNV *temp_cnv_p = 0;
     if( !cnv ) cnv = &temp_cnv_p;
     // see if we already have a converter and if it is the right one
     if( !*cnv || (*cnv)->typeID().value() != ref.typeID() ) {
        // we don't - find the right converter for ref.typeID()
	*cnv = converterForRef( *cnv, ref, log );
	if( !*cnv ) return 0;
        (*cnv)->setReadingFlag();
     }
     return (**cnv).virt_createTransFromPStore( ref.index(), log );
  }

  // hook to initialize private converters
  virtual void	initPrivateConverters(TopLevelTPCnvBase *) {}
  

  /// @copydoc ITPConverter::topConverter()
  virtual TopLevelTPCnvBase*	topConverter() const {
     return m_topConverter;
  }

  /// @copydoc ITPCnvBase::transientTInfo()
  virtual const std::type_info& transientTInfo() const { return typeid(TRANS); }

  /// @copydoc ITPConverter::transBaseTInfo()
  /// this method is not overwritten in the subclasses like transientTInfo()
  const std::type_info& transBaseTInfo() const { return typeid(TRANS); }

  /// @copydoc ITPConverter::typeID()
  virtual const TPObjRef::typeID_t&	typeID() const { return m_pStorageTID; }

  /// inlined non-virtual version to get the typeID value fast
  unsigned      typeIDvalue() const { return m_pStorageTIDvalue; }

  
  /// @copydoc ITPConverter::setRuntimeTopConverter()
  virtual void	setRuntimeTopConverter( TopLevelTPCnvBase *topConverter ) {
     m_topConverterRuntime = topConverter;
     initPrivateConverters( topConverter );
  }
  
  /// @copydoc ITPConverter::setTopConverter()
  virtual void	setTopConverter( TopLevelTPCnvBase *topConverter,
				 const TPObjRef::typeID_t &TPtypeID )
  {
     m_topConverter = m_topConverterRuntime = topConverter;
     m_pStorageTID = TPtypeID;
     m_pStorageTIDvalue = TPtypeID.value();
     initPrivateConverters( topConverter );
  }


  /* Internal accessor methods for the Read Flag
     Need to be public
  */
  void          setReadingFlag() { m_wasUsedForReading = true; }
  void          clearReadingFlag() { m_wasUsedForReading = false; }
  bool          wasUsedForReading() { return m_wasUsedForReading; }

  
//protected:
  // ------ methods operating on the persistent store of the top level object

  /** Internal interface method that is used to invoke the real conversion
      method (toPersistent_impl) in the derived converter.
      @param trans [IN] transient object
      @param log [IN] output message stream
      @return TPObjRef TP reference to the persistent representation
      stored in the storage vector of the top-level persistent object
  */
  virtual TPObjRef virt_toPersistent( const TransBase_t* trans, MsgStream& log) = 0;

  /** Internal interface method that is used to invoke the real conversion
      method (createTransient) in the derived converter.
      @param index [IN] index of the persistent object in the storage vector
      @param log [IN] output message stream
      @return Transient representation (by pointer)
  */
  virtual TRANS* virt_createTransFromPStore( unsigned index, MsgStream &log ) = 0;

  /** Internal interface method that is used to invoke the real conversion
      method (persToTrans) in the derived converter.
      @param index [IN] index of the persistent object in the storage vector
      @param transObj [IN] transient object to be filled out
      @param log [IN] output message stream
  */
  virtual void	pstoreToTrans( unsigned index, TransBase_t* transObj, MsgStream& log) = 0; 

protected:
  /// TP Ref typeID for the persistent objects this converter is creating
  TPObjRef::typeID_t    m_pStorageTID;

  /// 	m_pStorageTID converted to integer value
  unsigned              m_pStorageTIDvalue;

  /// top level converter that owns this elemental TP converter
  /// it also holds the storage object
  TopLevelTPCnvBase 	*m_topConverter;
  
  /// top level converter "owning" this TP converter at runtime (different from m_topConverter in case the top-level converter and object have extensions)
  TopLevelTPCnvBase 	*m_topConverterRuntime;

  /// flag set when using this converter for reading
  /// triggers search for a new converter before writing, to prevent possible use of old version
  bool                  m_wasUsedForReading;
};




// --------------------------------------------------------------

/** @class  TPAbstractPolyCnvBase
    The most basic TP converter template which is parametrized by
    transient and persistent types.
    Can be used to implement TP converters for /b abstract transient base
    types.  

    TRANS - the transient type
    PERS - the persistent representation
    TRANS_BASE - common transient base type for all transient objects
    belonging to the same inheritance tree. For the root of that tree,
    TRANS == TRANS_BASE
    Note: Converters for polymorphic types in the same inheritance tree must
    have the same template parameter base class type:  TRANS_BASE
*/

template< class TRANS_BASE, class TRANS, class PERS >
class TPAbstractPolyCnvBase
   : public ITPConverterFor< TRANS_BASE > 
{
public:
  typedef TRANS		Trans_t;
  typedef PERS		Pers_t;
  typedef PERS		PersBase_t;  // Obsolete, for backward compat
  
  TPAbstractPolyCnvBase() :
	m_pStorage(0),
	m_curRecLevel(0),
	m_recursive(false),
	m_ignoreRecursion(false)
  {}
  
  virtual ~TPAbstractPolyCnvBase() = default;


  // conversion methods - to be implemented in the concrete converter
  //                      -------------------------------------------
  
  /** Convert persistent representation to transient one. Copies data
      members from persistent object to an existing transient one.
      Needs to be implemented by the developer on the actuall converter. 
      @param persObj [IN] persistent object
      @param transObj [IN] transient object
      @param log [IN] output message stream
  */
   virtual void persToTrans(const PERS* persObj, TRANS* transObj, MsgStream &log) = 0;

  /** Convert transient representation to persistent one. Copies data
      members from transient object to an existing persistent one.
      Needs to be implemented by the developer on the actuall converter. 
      @param transObj [IN] transient object
      @param persObj [IN] persistent object
      @param log [IN] output message stream
  */  
  virtual void transToPers(const TRANS* transObj, PERS* persObj, MsgStream &log) = 0;

  /// @copydoc ITPCnvBase::persToTransUntyped()
  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log)
  {
    persToTrans (reinterpret_cast<const PERS*> (pers),
                 reinterpret_cast<TRANS*> (trans),
                 log);
  }

  /// @copydoc ITPCnvBase::transToPersUntyped()
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log)
  {
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
  virtual PERS* createPersistent(const TRANS* transObj, MsgStream &log);

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

  /// @copydoc ITPCnvBase::persistentTInfo()
  virtual const std::type_info& persistentTInfo() const { return typeid(PERS); }

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
     TRANS* trans_der = dynamic_cast<TRANS*>(trans);
     if (!trans_der) std::abort();
     this->persToTrans( &(*m_pStorage)[index], trans_der, log );
  }

  /** @copydoc ITPConverterFor::virt_toPersistent()
      Here toPersistent_impl is invoked with the dynamic cast of the
      transient type pointer to it's actual type
  */
  virtual TPObjRef virt_toPersistent( const TRANS_BASE *trans, MsgStream &log )  {
     const TRANS* trans_der = dynamic_cast<const TRANS*>(trans);
     if (!trans_der) std::abort();
     return toPersistent_impl( trans_der, log);
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
  void	setPStorage( std::vector< PERS > *storage ) {
     m_pStorage = storage;
     m_curRecLevel = 0;
  }

  /** Tell the converter if it should work in recursive mode
      slower but it can safely handle recursion
  */
  void	setRecursive( bool flag=true ) {
     m_recursive = flag;
  }

  /** Tell the converter to ignore recursion (do not throw errors)
      even when recurion is detected.
      UNSAFE!
      use only if you are sure you preallocated enough persistent storage
  */
  void	ignoreRecursion( bool flag=false ) {
     m_ignoreRecursion = flag;
  }

  /// Reserve 'size' elements for persistent storage
  virtual void	reservePStorage( size_t size ) {
     m_pStorage->reserve( size );
  }

protected:
  /// the address of the storage vector for persistent representations
  std::vector< PERS > 	*m_pStorage;

  /// count recursive invocations, to detect recursion
  int			m_curRecLevel;

  /// if true, work in recursion-safe way (slower)
  bool			m_recursive;

  /// if true, do not throw errors in case of recursion.
  bool			m_ignoreRecursion;
};



// --------------------------------------------------------------

namespace TPCnv {

/** @class  TPCnv::CreateTransient
    Helper class to make specializing the creation of transient objects
    in the TPConverterBase::createTransient() easier.

    Provide specialization before definition of your T/P converter for transient typeTRANS
    The final T/P converter can still overwrite its whole createTransient() method 
*/
   template<class TRANS>
   class CreateTransient  {
      public:
      static std::unique_ptr<TRANS>  create() { return std::make_unique<TRANS>(); }
   };

} // namespace TPCnv



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

template< class TRANS_BASE, class TRANS, class PERS >
class TPPolyCnvBase
   : public TPAbstractPolyCnvBase< TRANS_BASE, TRANS, PERS > 
{
public:
  TPPolyCnvBase() {}
  virtual ~TPPolyCnvBase() = default;


  // Default implementations (usually no need to overwrite)
  // ------------------------------------------------------

  /** Create transient representation of a persistent object.
      Simply creates a new transient object and calls persToTrans()
      @param persObj [IN] persistent object
      @param log [IN] output message stream
      @return the created transient object
  */
  virtual TRANS* createTransient(const PERS* persObj, MsgStream &log);

  /** Internal interface method that is used to invoke the real conversion
      method (createTransient)
      @param index [IN] index of the persistent object in the storage vector
      @param log [IN] output message stream
      @return Created transient object (by pointer)
  */
  virtual TRANS* virt_createTransFromPStore( unsigned index, MsgStream &log )  {
     assert (index < this->m_pStorage->size());
     return createTransient( &(*this->m_pStorage)[index], log );
  }

};



/** @class TPConverterBase
    TP Converter template for a "regular" type.
    Only slighlty specialized (for better performance) version of the
    polymorphic converter template TPPolyCnvBase 
*/
template< class TRANS, class PERS >
class TPConverterBase
   : public TPPolyCnvBase< TRANS, TRANS,  PERS >
{
public:
  TPConverterBase() {}
  virtual ~TPConverterBase() = default;


  // implement these methods without dynamic cast to TRANS

  /// @copydoc TPAbstractPolyCnvBase::virt_toPersistent()
  virtual TPObjRef virt_toPersistent( const TRANS *trans, MsgStream &log ) {
      return this->toPersistent_impl( trans, log);
  }

  /// @copydoc TPAbstractPolyCnvBase::pstoreToTrans()
  virtual void	pstoreToTrans( unsigned index, TRANS *trans, MsgStream &log ) {
     assert (index < this->m_pStorage->size());
     this->persToTrans( &(*this->m_pStorage)[index], trans, log );
  }

};


// --------------------------------------------------------------

/** @class TPPtrVectorCnv
  Converter between:
  transient vector of T* (like DataVector<T>)
  and persistent vector<T>
  where T is NOT a polymorphic type!
  Uses converter CONV for the actual TP conversion
*/
template<class TRANS, class PERS, class CONV>
class TPPtrVectorCnv : public TPConverterBase<TRANS, PERS> {
public:
  TPPtrVectorCnv() : m_elementCnv(0)  {}

  /** Converts vector of PERS::value_type objects to vector of TRANS::value_type objects,
      using converter CONV
      @param persVect [IN] vector of persistent objects
      @param transVect [IN] vector of transient object
      @param log [IN] output message stream
  */
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log) {
     transVect->clear();
     transVect->reserve( persVect->size() );
     // convert vector entries one by one
     for( typename PERS::const_iterator 
	    it   = persVect->begin(),
	    iEnd = persVect->end();
	  it != iEnd;  ++it ) {
 	transVect->push_back( this->createTransFromPStore( &m_elementCnv, *it, log ) );
     }        
  }
  

  /** Converts vector of TRANS::value_type objects to vector of PERS::value_type objects,
      using converter CONV
      @param transVect [IN] vector of transient object
      @param persVect [IN] vector of persistent objects
      @param log [IN] output message stream
  */
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) {
     persVect->clear();
     persVect->reserve( transVect->size() );
     // convert vector entries one by one
     for( typename TRANS::const_iterator 
	    it   = transVect->begin(),
	    iEnd = transVect->end();
	  it != iEnd;  
	  ++it ) {
       persVect->push_back( this->toPersistent( &m_elementCnv, *it, log ) );
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
template<class TRANS, class PERS, class CONV>
class TPPolyVectorCnv : public TPPtrVectorCnv<TRANS, PERS, CONV> {
public:
  /// @copydoc TPPtrVectorCnv::transToPers()
  /// Overwritten method that forces toPersistent() to look up the right converter every time
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) {
     persVect->clear();
     persVect->reserve( transVect->size() );
     // convert vector entries one by one
     for( typename TRANS::const_iterator 
	    it   = transVect->begin(),
	    iEnd = transVect->end();
	  it != iEnd;  ++it ) {
 	persVect->push_back( this->toPersistent( (CONV**)0, *it, log ) );
     }       
  }
};


/*
  Converter between
  transient vector<T> (by value) and persistent vector<TPObjRef>  
*/

template<class TRANS, class PERS, class CONV>
class TPValVectorCnv : public TPConverterBase<TRANS, PERS> {
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



// --------------------------------------------------------------
/*
  Converter between
  transient AthenaHistVector of T* and persistent vector<T>
*/

template<class TRANS, class PERS, class CONV>
class T_AthenaHitsVectorCnv : public TPConverterBase<TRANS, PERS> {
public:
    T_AthenaHitsVectorCnv() {}

  /// persToTrans
  virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log);
  /// transToPers
  virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
};


// --------------------------------------------------------------
/*
  Converter between
  transient AtlasHitsVector of T* and persistent vector<T>
*/

template<class TRANS, class PERS, class CONV>
class T_AtlasHitsVectorCnv : public TPConverterBase<TRANS, PERS> {
public:
    T_AtlasHitsVectorCnv() {}

  /// persToTrans
  virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log);
  /// transToPers
  virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
};


// -------------------------------
//  OLD STYLE vector TP converters
//  TP conversion in-place, not involving top-level objects or TPRefs
// -------------------------------------------------------------------
/*
  Converter between
  transient vector of T* (like DataVector<T>)
  and persistent vector<T>
*/

template<class TRANS, class PERS, class CONV>
class TPCnvVector : public TPConverterBase<TRANS, PERS> {
public:
  TPCnvVector() {}

  /// @copydoc TPPtrVectorCnv::persToTrans()
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log);
  /// @copydoc TPPtrVectorCnv::transToPers()
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log);
};


/*
  Converter between
  transient vector of T and persistent vector<T>
*/

template<class TRANS, class PERS, class CONV>
class TPCnvStdVector : public TPConverterBase<TRANS, PERS> {
public:
  TPCnvStdVector() {}

  /// @copydoc TPPtrVectorCnv::persToTrans()
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log) ;
  /// @copydoc TPPtrVectorCnv::transToPers()
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) ;
};


//---------------------------------------------------------------------------------
/*
  Converter between
  transient IdentifiableContainer<T> and persistent std::vector<T>
  Used by Muon RDO containers 
  This version assumes RDO collection returns an identifier as a unsigned int.
*/

template<class TRANS, class PERS, class CONV>
class TPCnvIDCont : public TPConverterBase<TRANS, PERS> {
public:
  TPCnvIDCont() {}

  /// @copydoc TPPtrVectorCnv::persToTrans()
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log) ;

  /// @copydoc TPPtrVectorCnv::transToPers()
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) ;

  typedef typename TRANS::IDENTIFIABLE COLLECTION_t;
};

//---------------------------------------------------------------------------------
/*
  Converter between
  transient IdentifiableContainer<T> and persistent std::vector<T>
  Used by Muon RDO containers
  This version assumes RDO collection returns an identifier as an Identifier
*/

template<class TRANS, class PERS, class CONV>
class TPCnvIDContFromIdentifier : public TPConverterBase<TRANS, PERS> {
public:
  TPCnvIDContFromIdentifier() {}

  /// @copydoc TPPtrVectorCnv::persToTrans()
  virtual void persToTrans(const PERS* persVect, TRANS* transVect, MsgStream &log) ;

  /// @copydoc TPPtrVectorCnv::transToPers()
  virtual void transToPers(const TRANS* transVect, PERS* persVect, MsgStream &log) ;

  typedef typename TRANS::IDENTIFIABLE COLLECTION_t;
};



#include "TPConverter.icc"
#endif
