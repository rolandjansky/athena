/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TopLevelTPCnvBaseP_H
#define TopLevelTPCnvBaseP_H

/** @file TopLevelTPCnvBaseP.h
 *  @brief Base class template for Top-Level TP converters, specialized with persistent type
 *  @author Marcin.Nowak@cern.ch
 **/


#include "TopLevelTPCnvBase.h"

/** @class TopLevelTPCnvBaseP
    This class is a more specialized version of
    TopLevelTPCnvBase - it is a template parametrized by the
    actual type of the persistent object the converter will work
    with. It provided implementations for several methods that were
    pure virtual before (because of the lack of specific type there)
*/

template< class TL_PERS >
class TopLevelTPCnvBaseP
   : public TopLevelTPCnvBase
{
public:

  // ----  Methods to be implement in the derived converter:
  
  /** A stub for a method that should be provided by the converter
   * creator.  In that methods all elemental TP converters that belong
   * to this top-level converter need to be assigned to storage
   * vectors in the persistent object.
   @param persObj [IN] persistent object
  */
  virtual void	setPStorage( TL_PERS* persObj) = 0;

  
  TopLevelTPCnvBaseP() : m_tlPersObject(0)  {}

  virtual ~TopLevelTPCnvBaseP()  { deleteTLPersObject(); }

  TopLevelTPCnvBaseP (const TopLevelTPCnvBaseP&) = delete;
  TopLevelTPCnvBaseP& operator= (const TopLevelTPCnvBaseP&) = delete;

 
  // ---------------  internals - "non-public" use

  /// @copydoc TopLevelTPCnvBase::hasTLPersObject()
  virtual bool	hasTLPersObject() const override {
     return m_tlPersObject;
  }

  /// @copydoc TopLevelTPCnvBase::createTLPersObject()
  virtual void	createTLPersObject() override {
     m_tlPersObject = new TL_PERS();
     setPStorage( m_tlPersObject );     
  }

  /// @copydoc TopLevelTPCnvBase::deleteTLPersObject()
  // cppcheck-suppress virtualCallInConstructor
  virtual void	deleteTLPersObject() override {
     if( m_tlPersObject ) {
	delete m_tlPersObject;
	clearTLPersObject();
     }
  }
  
  /// @return pointer to the persistent object owned by this converter
  TL_PERS*	getTLPersObject() {
     return m_tlPersObject;
  }

  /// @return pointer to the persistent object owned by this converter
  const TL_PERS*	getTLPersObject() const {
     return m_tlPersObject;
  }

  /// @release the persistent object from this converter
  TL_PERS*	releaseTLPersObject() {
     TL_PERS	*tmp = m_tlPersObject;
     clearTLPersObject();
     return tmp;
  }

  /// @copydoc TopLevelTPCnvBase::getTLPersObjectAsVoid()
  virtual void*	getTLPersObjectAsVoid() override {
     return getTLPersObject();
  }     

  /// @copydoc TopLevelTPCnvBase::getTLPersObjectAsVoid()
  virtual const void*	getTLPersObjectAsVoid() const override {
     return getTLPersObject();
  }     

  /// @copydoc TopLevelTPCnvBase::clearTLPersObject()
  virtual void		clearTLPersObject() override {
     m_tlPersObject = 0;
  }

  /// @copydoc TopLevelTPCnvBase::setTLPersObject()
  virtual void	setTLPersObject( void *persObj ) override {
     if( m_tlPersObject )   delete m_tlPersObject; 
     m_tlPersObject = reinterpret_cast<TL_PERS*>( persObj );
     setPStorage( m_tlPersObject  );
  }


  /// @copydoc TopLevelTPCnvBase::getTokenListVar()
  virtual TPCnvTokenList_p1*	getTokenListVar() override { return 0; }

  /// @copydoc TopLevelTPCnvBase::getTokenListVarFrom()
  virtual TPCnvTokenList_p1* 	getTokenListVarFrom( void *persObj ) override {
     // avoid setting up the pointer to persistent object
     // (can lead to deleting it twice, if the user deletes too)
     TL_PERS	*tmp = m_tlPersObject;
     m_tlPersObject = reinterpret_cast<TL_PERS*>( persObj );
     TPCnvTokenList_p1	*toklist = getTokenListVar();
     m_tlPersObject = tmp;
     return toklist;
  }

  
protected:
  /// The persistent object this conveter is working on
  TL_PERS*		m_tlPersObject;
  
};

#endif
