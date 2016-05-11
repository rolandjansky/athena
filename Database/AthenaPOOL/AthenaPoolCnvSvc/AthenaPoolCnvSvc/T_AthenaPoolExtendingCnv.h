/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLEXTENDCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLEXTENDCNV_H

/** @file T_AthenaPoolExtendingCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolExtendingCnv class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "T_AthenaPoolCustomCnv.h"
//#include "TPTools/AthenaConverterTLPExtension.h"
#include "AthenaPoolCnvTPExtension.h"

/** @class T_AthenaPoolExtendingCnv
 *  @brief This templated class extends T_AthenaPoolCustomCnv to provide additional/chaged functionality for AthenaPool converters for top-level TP separated persistent objects that are an extension to some other, top-level TP separated object. 
 **/

template <class TRANS, class PERS>
class T_AthenaPoolExtendingCnv
   : public T_AthenaPoolCustomCnv<TRANS, PERS>,
     public AthenaPoolCnvTPExtension
{
friend class CnvFactory<T_AthenaPoolExtendingCnv<TRANS, PERS> >;

public:
   typedef T_AthenaPoolCustomCnv<TRANS, PERS>   BaseType;

protected:
   // -------------------------------------------------------------
   /// Constructor
  T_AthenaPoolExtendingCnv(ISvcLocator* svcloc);

  // Methods to be implemented in the derived converter class
  // ##########################################################
  
  /// Return the top level TP converter (which is always used for writing)
  virtual TopLevelTPCnvBase*	getTopLevelTPCnv() = 0;
  
  /// Read the persistent object from POOL
  /// @param token [IN] token of the object to read
  virtual void 		readObjectFromPool( const std::string& token ) = 0 ; 
  
protected:
  // redirect to the old API readObjectFromPool()
  virtual void 		readObject( const std::string& token ) { this->readObjectFromPool(token); }
  
  /// Write the persistent object to POOL
  /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
  virtual const Token*	writeObject(const std::string& key);

  /// return the original AthenaPool converter this one was cloned from
  /// if not cloned this returns self
  /// if not registered this returns NULL
  virtual BaseType*	baseAthenaPoolCnv() { return m_originalExtendingCnv; }

  /// remember the original converter this one was cloned from
  virtual void 		wasClonedFrom( AthenaConverterTLPExtension *orig_converter );

  /// tells if this converter needs to be cloned
  /// (true after the converter has been registered once already)
  virtual bool		needsCloning() const { return m_originalExtendingCnv != 0; }

  /// @copydoc T_AthenaPoolCustomCnv::setToken()
  virtual void		setToken(const std::string& token);

  /// @copydoc T_AthenaPoolCustomCnv::poolReadObject()
  /// deprecated - use poolReadObject(TLPCnv)
  template <class P>
  P* 	poolReadObject();
  
  /// @copydoc T_AthenaPoolCustomCnv::poolReadObject(AthenaPoolTopLevelTPCnvBase&)
  template <class P>
  void 	poolReadObject( TopLevelTPCnvBase& tlp_converter );

   
protected:
   //-------------------------------------------------------------------
   // Internal methods
  
  /// Get the name of this converter (anything that identifies it). Used for logging
  /// @return Name of this converter 
  virtual const std::string   name() const;

  
  // hidden away, as they don't make sense for this converter
  /// no-op
  virtual PERS 	*createPersistent( TRANS *) { return 0; }
  virtual TRANS	*createTransient() { return 0; }

  /// pointer to the original Gaudi converter - only this one is registered in the framework and has to be used for all I/O operations
  BaseType*	m_originalExtendingCnv;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.icc"


#endif
