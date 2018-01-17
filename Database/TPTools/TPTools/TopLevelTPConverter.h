/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPLEVELTPCONVERTER_H
#define TOPLEVELTPCONVERTER_H

/** @file TopLevelTPConverter.h
 *  @brief Defines the most specialized template for all top-level TP converters 
 *  @author Marcin.Nowak@cern.ch
 **/

#include "TopLevelTPCnvBaseP.h"
#include "TPConverter.h"


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

/*
MN: TODO - seems that inheritance from the main converter and also
having it as a data member is redundant - fix?
*/
    
template< class MAIN_CNV, class TL_PERS >
class TopLevelTPConverter
   : public TopLevelTPCnvBaseP< TL_PERS >
   , public TPConverterBase< typename MAIN_CNV::Trans_t, TL_PERS >
{
public:
  typedef typename MAIN_CNV::Trans_t	TRANS;


  // ----  Methods to be implement in the derived converter:

  // this declaration is repeated here for clarity 
  /// @copydoc TopLevelTPCnvBaseP::setPStorage()
  virtual void	setPStorage( TL_PERS* persObj) = 0;

  /** Constructor. 
  Note: The constructor in the derived class should add all local converters
  using AddTPConveter()
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
  

  /** Create transient representation of persObj. 
      NOTE: usually this method will not delete persObj and the user
      needs to do it himself. However, if ownership was somehow passed to
      the converter earlier, then the object will be deleted.
      @param persObj [IN] pointer to the persistent representation
      @param log [IN] message stream for output
      @return TRANS* transient object passed by a pointer
  */
  virtual TRANS*  createTransient(const TL_PERS* persObj, MsgStream &log) {
     setPStorage( const_cast<TL_PERS*>(persObj) ); 
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
  virtual unsigned short	getConverterID() { return 0; }


 // method implementations allowing nesting top level objects
 //- -----------------------------------------------

  /// probably obsolete (wrong?) and pending removal
  virtual TPObjRef virt_toPersistent( const TRANS *trans, MsgStream &log ) {
     m_mainConverter.virt_toPersistent( trans, log );
     return TPObjRef(this->m_pStorageTID, 0);  // 0 -> no index (not a vector)
  }

  /// probably obsolete and pending removal
  virtual TRANS* virt_createTransFromPStore( unsigned index, MsgStream &log ) {
     return m_mainConverter.virt_createTransFromPStore( index, log );
  }

  /// probably obsolete and pending removal
  virtual void	pstoreToTrans( unsigned index, TRANS *trans, MsgStream &log ) {
     m_mainConverter.pstoreToTrans( index, trans, log );
  }

  virtual void persToTrans (const TL_PERS* pers,
                            TRANS* trans,
                            MsgStream& msg)
  {
    setPStorage (const_cast<TL_PERS*> (pers));
    m_mainConverter.pstoreToTrans (0, trans, msg);
  }
  
  virtual void transToPers(const TRANS* trans,
                           TL_PERS* pers,
                           MsgStream& msg)
  {
    this->setTLPersObject( pers );
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
  }

protected:
  MAIN_CNV		m_mainConverter;

};

#endif
