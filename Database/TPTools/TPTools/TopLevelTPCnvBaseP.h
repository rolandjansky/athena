/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

 
  // ---------------  internals - "non-public" use

  /// @copydoc TopLevelTPCnvBase::hasTLPersObject()
  virtual bool	hasTLPersObject() const {
     return m_tlPersObject;
  }

  /// @copydoc TopLevelTPCnvBase::createTLPersObject()
  virtual void	createTLPersObject() {
     m_tlPersObject = new TL_PERS();
     setPStorage( m_tlPersObject );     
  }

  /// @copydoc TopLevelTPCnvBase::deleteTLPersObject()
  virtual void	deleteTLPersObject() {
     if( m_tlPersObject ) {
	delete m_tlPersObject;
	clearTLPersObject();
     }
  }
  
  /// @return pointer to the persistent object owned by this converter
  TL_PERS*	getTLPersObject() const {
     return m_tlPersObject;
  }

  /// @copydoc TopLevelTPCnvBase::getTLPersObjectAsVoid()
  virtual void*	getTLPersObjectAsVoid() const {
     return getTLPersObject();
  }     

  /// @copydoc TopLevelTPCnvBase::clearTLPersObject()
  virtual void		clearTLPersObject() {
     m_tlPersObject = 0;
  }

  /// @copydoc TopLevelTPCnvBase::setTLPersObject()
  virtual void	setTLPersObject( void *persObj ) {
     if( m_tlPersObject )   delete m_tlPersObject; 
     m_tlPersObject = reinterpret_cast<TL_PERS*>( persObj );
     setPStorage( m_tlPersObject  );
  }


  /// @copydoc TopLevelTPCnvBase::getTokenListVar()
  virtual TPCnvTokenList_p1*	getTokenListVar() { return 0; }

  /// @copydoc TopLevelTPCnvBase::getTokenListVarFrom()
  virtual TPCnvTokenList_p1* 	getTokenListVarFrom( void *persObj ) {
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
