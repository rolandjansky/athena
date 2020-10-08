/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPLEVELTPCONVERTER_H
#define TOPLEVELTPCONVERTER_H

/** @file TopLevelTPConverter.h
 *  @brief Defines the most specialized template for all top-level TP converters 
 *  @author Marcin.Nowak@cern.ch
 **/

#include "TopLevelTPCnvBaseP.h"
#include "TPConverter.h"
#include "CxxUtils/checker_macros.h"


/** @class TopLevelTPConverter
    TopLevelTPConverter is the final and most specialized
    template for all top-level TP converters. It combines the
    top-level converter interface and functionality with the elemental
    TP converter interface, so a top-level converter can look and
    function as a standard TP converter.

    This class uses a concept of the main TP converter, which is the
    first TP converter that needs to be invoked in the conversion
    process. This converter is 'built-in' and shold be added to the
    list of elemental TP converters by invoking addMainTPConverter() 
*/

template< class MAIN_CNV, class TL_PERS >
class TopLevelTPConverter
   : public TopLevelTPCnvBaseP< TL_PERS >
   , public ITPCnvBase
{
public:
  typedef typename MAIN_CNV::Trans_t	TRANS;
  typedef TL_PERS                       PERS;

  // ----  Methods to be implement in the derived converter:

  // this declaration is repeated here for clarity 
  /// @copydoc TopLevelTPCnvBaseP::setPStorage()
  virtual void	setPStorage( TL_PERS* persObj) override = 0;

  /** Constructor. 
  Note: The constructor in the derived class should add all local converters
  using AddTPConverter()
  */
  TopLevelTPConverter() {}


  // ----- default implementations

  /** Create transient representation of the persistent object known
   * to this converter. 
   NOTE: If the persistent object is @b owned by the converter, it
   will be deleted
   @param log [IN] message stream for output
   @return TRANS* transient object passed by a pointer
  */
  TRANS*  createTransient( MsgStream &log ) {
     // 0 is the index of the main and only one top level object
     TRANS *transObj = m_mainConverter.virt_createTransFromPStore( 0, log );
     this->deleteTLPersObject();
     return transObj;
  }
  

  /** Create transient representation of the persistent object known
   * to this converter. 
   NOTE: If the persistent object is @b owned by the converter, it
   will be deleted
   @param key [IN] SG key of the object being converted
   @param log [IN] message stream for output
   @return TRANS* transient object passed by a pointer
  */
  TRANS*  createTransientWithKey( const std::string& key,
                                  MsgStream &log )
  {
     // 0 is the index of the main and only one top level object
    TRANS *transObj = m_mainConverter.virt_createTransFromPStoreWithKey( 0, key, log );
     this->deleteTLPersObject();
     return transObj;
  }
  

  /** Create transient representation of persObj. 
      NOTE: usually this method will not delete persObj and the user
      needs to do it himself. However, if ownership was somehow passed to
      the converter earlier, then the object will be deleted.
      @param persObj [IN] pointer to the persistent representation
      @param log [IN] message stream for output
      @return TRANS* transient object passed by a pointer
  */
  virtual TRANS*  createTransient(const TL_PERS* persObj, MsgStream &log) {
     // FIXME: TPConverter uses the same non-const member m_pStorage
     // for both reading and writing, but we want it to be const
     // in the former case.
     TL_PERS* pers_nc ATLAS_THREAD_SAFE = const_cast<TL_PERS*>(persObj);
     setPStorage( pers_nc ); 
     return createTransient( log );
  }

  /** Create persistent representation of transObj.
      @param transObj [IN] pointer to the transient representation
      @param log [IN] message stream for output
      @return TL_PRES* persistent object passed by a pointer
  */
  virtual TL_PERS*  createPersistent(const TRANS* transObj, MsgStream &log)
  {
     // create a new persistent object (empty)
     this->createTLPersObject();
     // do transToPers conversion, fill in the persistent object
     m_mainConverter.virt_toPersistent(transObj, log);
     // take the persistent object away from the TL converter 
     TL_PERS	*ret = this->getTLPersObject();
     // important to clear so createTransient() will not try to delete it (and crash)
     this->clearTLPersObject();
     return ret;
  }

  /** Create persistent representation of transObj.
      @param transObj [IN] pointer to the transient representation
      @param key [IN] SG key of the object being converted.
      @param log [IN] message stream for output
      @return TL_PRES* persistent object passed by a pointer
  */
  virtual TL_PERS*  createPersistentWithKey(const TRANS* transObj,
                                            const std::string& key,
                                            MsgStream &log)
  {
     // create a new persistent object (empty)
     this->createTLPersObject();
     // do transToPers conversion, fill in the persistent object
     m_mainConverter.virt_toPersistentWithKey(transObj, key, log);
     // take the persistent object away from the TL converter 
     TL_PERS	*ret = this->getTLPersObject();
     // important to clear so createTransient() will not try to delete it (and crash)
     this->clearTLPersObject();
     return ret;
  }

  /// Add the main TP converter to the internal list - should be
  /// called from user-defined constructor
  void	addMainTPConverter() {  this->addTPConverter( &m_mainConverter ); }

  /// Set the persistent storage space for the main TP converter
  // moved to a template to accomodate TPIntegerVectorStorage_pN classes
  // without whole class specialization
  template< typename STORAGE >
  void	setMainCnvPStorage( STORAGE* storage ) {
     m_mainConverter.setPStorage( storage );
  }

  /// Returns this converter's ID.  By default it is 0. Should be
  /// overwritten in extending converters
  /// @return this converter's ID
  virtual unsigned short	getConverterID() override { return 0; }


  //- --------------------------------------------------------------------------
  // implementation of ITPCnvBase interface

  /// @copydoc ITPCnvBase::transientTInfo()
  virtual const std::type_info& transientTInfo() const override { return typeid(TRANS); }

  /// @copydoc ITPCnvBase::persistentTInfo()
  virtual const std::type_info& persistentTInfo() const override { return typeid(PERS); }


  /// @copydoc ITPCnvBase::persToTransUntyped()
  virtual void persToTransUntyped(const void* pers, void* trans, MsgStream& log) override {
     persToTrans( reinterpret_cast<const PERS*>(pers), reinterpret_cast<TRANS*>(trans), log );
  }

  /// @copydoc ITPCnvBase::transToPersUntyped()
  virtual void transToPersUntyped(const void* trans, void* pers, MsgStream& log) override {
     transToPers( reinterpret_cast<const TRANS*>(trans),  reinterpret_cast<PERS*>(pers), log );
  }

  /// @copydoc ITPCnvBase::persToTransWithKeyUntyped()
  virtual void persToTransWithKeyUntyped(const void* pers,
                                         void* trans,
                                         const std::string& /*key*/,
                                         MsgStream& log) override
  {
     persToTrans( reinterpret_cast<const PERS*>(pers), reinterpret_cast<TRANS*>(trans), log );
  }

  /// @copydoc ITPCnvBase::transToPersWithKeyUntyped()
  virtual void transToPersWithKeyUntyped(const void* trans,
                                         void* pers,
                                         const std::string& /*key*/,
                                         MsgStream& log) override
  {
     transToPers( reinterpret_cast<const TRANS*>(trans),  reinterpret_cast<PERS*>(pers), log );
  }

  // ----------------------  methods used by T_TPCnv<> converters
  virtual void persToTrans (const PERS* pers, TRANS* trans, MsgStream& msg) {
    // FIXME: TPConverter uses the same non-const member m_pStorage
    // for both reading and writing, but we want it to be const
    // in the former case.
    TL_PERS* pers_nc ATLAS_THREAD_SAFE = const_cast<TL_PERS*>(pers);
    setPStorage( pers_nc ); 
    m_mainConverter.pstoreToTrans (0, trans, msg);
  }
  
  virtual void transToPers(const TRANS* trans, PERS* pers, MsgStream& msg) {
    this->setTLPersObject( pers );
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
  }

protected:
  MAIN_CNV		m_mainConverter;

};

#endif

