/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPTOPLEVELTPCNVBASE_H
#define TPTOPLEVELTPCNVBASE_H

/** @file TopLevelTPCnvBase.h
 *  @brief Defines the base class for top-level TP converters
 *  @author Marcin.Nowak@cern.ch
 **/

#include <set>
#include <map>

#include "AthenaPoolUtilities/TPObjRef.h"
class TPCnvTokenList_p1;
#include "ITPConverter.h"

/** @class TopLevelTPCnvBase
    @brief Base class for top-level TP converters

    Base (not templated) class for all top-level TP
    converters.
    Provides:
    - handling of elemental TP converters
    - typeless interface to access the persistent object being
    converted by this top-level converter
*/

class TopLevelTPCnvBase
{
public: 
  TopLevelTPCnvBase() : m_typeIDCount(0) { }

  virtual ~TopLevelTPCnvBase() { } 

  // ------------- Methods for elemental TP converter management 
  
  /// Add a TP converter to this top level converter
  /// @param cnv [IN] the TP converter to be added 
  virtual void 	addTPConverter( ITPConverter *cnv );

  /// Add a TP converter to this top level converter but only for
  /// reading. Old converters that read schema evolved objects should
  /// be added only for reading.
  /// @param cnv [IN] the TP converter to be added 
  virtual void 	addTPConverterForReading( ITPConverter *cnv );

private:
  /// add TP converter to the list of converters consulted when
  /// writing - internal method.
  /// @param cnv [IN] the TP converter to be added 
  virtual void 	addTPConverterForWriting( ITPConverter *cnv );

public:
  /// add extending TP converter to this top level converter - to the
  /// list consulted when reading only.
  /// @param cnv [IN] the TP converter to be added 
  virtual void 	addExtTPConverterForReading( ITPConverter *cnv );

  /** copy all extending converters from this top level converter
      to "dest" top level converter
      @param dest [IN] destination converter which will receive all
      elemental TP converters that this one has
  */
  virtual void	addTPConvertersTo( TopLevelTPCnvBase *dest );

  /** copy all extending converters from this top level converter
      to "dest" top level converter - for reading
      @param dest [IN] destination converter which will receive all
      elemental TP converters that this one has
  */
  virtual void	addTPConvertersForReadingTo( TopLevelTPCnvBase *dest );

  /** Find and return a TP converter for a given C++ type info
      @param info [IN] C++ type info reference to the transient type
      @return  ITPConverter* pointer to the TP converter for type info
  */
  ITPConverter*	   converterForType( const std::type_info &info ) const {
     return m_converters.findConverter( info );
  }

  /** Find and return a TP converter for persistent type referenced by
      ref
      @param ref [IN] TP reference of a persistent object
      @return  ITPConverter* pointer to the TP converter for ref
  */
 ITPConverter*	   converterForRef( const TPObjRef &ref ) const {
     convIdMap_t::const_iterator  iter = m_convIdMap.find( ref.typeID() );
     return (iter == m_convIdMap.end())?
	0 : iter->second;
  }


  // ---------------------   methods dealing with the persistent
  // object owned by the topl-level converter

  /// Returns @c true if the converter has a persistent object
  virtual bool	hasTLPersObject() const = 0;

  /// @return pointer to the persistent object owned by this converter as @c void*
  virtual void*	getTLPersObjectAsVoid() = 0;

  /// @return pointer to the persistent object owned by this converter as @c void*
  virtual const void*	getTLPersObjectAsVoid() const = 0;

  /// Create persistent object - the converter keeps it
  virtual void	createTLPersObject() = 0;

  /// Delete the persistent object owned by the converter
  virtual void	deleteTLPersObject() = 0;

  /// Discard the persistent object
  virtual void	clearTLPersObject() = 0;

  /** Take ownership of the persistent object. Also initialize all
      elemental TP converters to use that object. 
   @param persObj [IN] the persistent object passed as @c void*
  */
  virtual void	setTLPersObject(void* persObj) = 0;


  // ----------- methods dealing with the token list

  
  /** Return the list of tokens to the objects extending the
      object owned by this converter.
      This method is needed only in a conveter which has extensions.
      It needs to be implemented in the top-level converter by the
      user
      @return pointer to the token list from the persistent object
  */
  virtual TPCnvTokenList_p1*	getTokenListVar() = 0;

  /** Return the list of tokens to the objects extending the
      object NOT owned by this converter.
      @param persObj [IN] persistent object from which contains list of
      tokens. Passes as void* - if the type is wrong, it may crash
      @return pointer to the token list from persObj
  */
  virtual TPCnvTokenList_p1* 	getTokenListVarFrom( void *persObj ) = 0;
  

  // ----  other methods
 
  /// @return ID of the converter
  virtual unsigned short	getConverterID() = 0;
  
protected:
  /** Remember that elemental TP converters from the given top-level
   * converter are already registered.
   @param cnv [IN] top-level TP converter - source of the elemental TP
   converters
  */
  void	rememberConverter( TopLevelTPCnvBase *cnv )
  {
     m_registeredTLConverters.insert(cnv);
  }

  /** Check if TP converters from a given top-level converter were
   * already registered in this converter.
   @param cnv [IN] top-level converter to check
   @return true if TP converters from cnv were already registered
  */
  bool	hasConvertersFrom( TopLevelTPCnvBase *cnv )
  {
     return m_registeredTLConverters.find(cnv) != m_registeredTLConverters.end();
  }

  
protected:
  
  typedef std::map<unsigned, ITPConverter*>	convIdMap_t;

  /// Map of elemental TP converters indexed by their ID - used for reading
  convIdMap_t				m_convIdMap;

  /// Map of elemental TP converters indexed by transient typeid - used for writing
  TPConverterTypeMap< ITPConverter >	m_converters;

  /// Set top-level converters from which elemental TP converters were
  /// registered
  std::set<TopLevelTPCnvBase*> m_registeredTLConverters;

  /// Counter used to create typed IDs used in the transient Ref
  unsigned				m_typeIDCount;
};


#endif
