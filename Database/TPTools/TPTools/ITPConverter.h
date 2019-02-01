// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_ITPConverter_H
#define ATHENAPOOLCNVSVC_ITPConverter_H 1

/** @file TPTools/ITPConverter.h
 *  @brief Defines the base ITPConverter class interface for all TP
 *  converters.  Also defines TPConverterTypeMap utility class, used
 *  by the top-level converters.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <typeinfo>
#include <typeindex>
#include <string>


class MsgStream;
class TopLevelTPCnvBase;

/** @class ITPConverter
    Base interface class for all TP converters - typeless
*/
class ITPConverter  : public ITPCnvBase
{
protected:
  ITPConverter() {} 
public:
  virtual ~ITPConverter() = default;

  /** Set which top-level converter owns this elemental converter, and
      what TPtypeID was assigned to the persistent objects it produces.
      @param topConverter [IN] the top-level converter owning this converter
      @param TPtypeID [IN] TP type id for persistent objects  (used in
      TP refs)
  */
  virtual void	setTopConverter( TopLevelTPCnvBase *topConverter, 
				 const TPObjRef::typeID_t& TPtypeID ) = 0;
  
  /** Set runtime top-level converter - usually it is the owning TL
   * converter, but in case of extended objects it will be the TL
   * converter of the extended object
   @param topConverter [IN] runtime top-level converter for this converter
   */
  virtual void	setRuntimeTopConverter( TopLevelTPCnvBase *topConverter ) = 0;

  /** return the top-level converter for this elemental TP converter
      @return TopLevelTPCnvBas
  */
  virtual TopLevelTPCnvBase*	topConverter() const { return 0; }

  /** Return TP typeID for persistent objects produced by this converter
      @return TPObjRef::typeID_t&
  */
  virtual const TPObjRef::typeID_t&	typeID() const = 0;

  /** return C++ type id of the common base transient type
      for all converters for a group of polymorphic types
      @return std::type_info&
  */
  virtual const std::type_info& transBaseTInfo() const = 0;

  /// Reserve 'size' elements for persistent storage
  virtual void	reservePStorage( size_t size ) = 0;

  /** method called when the right TP converter was not found during writing
      - useful as a debugging hook, prints a detailed error message
      
      @param converterType [IN] converterType that was requested
      @param c [IN] converter that was actually found (0 if not)
      @param typeName [IN] the C++ type name of the type for which converter was searched for
      @param log [IN] output message stream
  */
  virtual void	converterNotFound( const std::type_info& converterType,
				   ITPConverter *c,
				   const std::string& typeName,
				   MsgStream& log );

  /** method called when the right TP converter was not found during reading
      - useful as a debugging hook, prints a detailed error message
      
      @param typeID [IN] converter ID that was requested
      @param c [IN] converter that was actually found (0 if not)
      @param typeName [IN] the C++ type name of the type for which converter was searched for
      @param log [IN] output message stream
  */
  virtual void	converterNotFound( unsigned typeID,
				   ITPConverter *c,
				   const std::string& typeName,
				   MsgStream& log );
};




//--------------  TPConverterTypeMap<CONVERTER_BASE>  -------------
#include <map>
#include <typeinfo>

/** class TPConverterTypeMap
    Converter registry template: a map from type_info to the appropriate converter.
    Ttemplated for different converter interfaces.
*/
template<class CONVERTER_BASE>
class TPConverterTypeMap {
public:
  typedef 	std::map<const std::type_index, CONVERTER_BASE*>	TId_Map_t;
  typedef	typename TId_Map_t::iterator	iterator;

  TPConverterTypeMap() {}

  /** Find converter for a given C++ type info
      @param objTypeInfo [IN] C++ type info
      @return pointer to the located converter
  */
  CONVERTER_BASE *	findConverter(const std::type_info &objTypeInfo) const;

  /** Add a new converter to the map
      @param converter [IN] the converter to be added
      @param objTypeInfo [IN] C++ type id of the type the converter is for
  */
  void		addConverter(CONVERTER_BASE *converter,
			     const std::type_info &objTypeInfo);

  /// @return map iterator to the beginning of the converter list
  iterator	begin() { return m_cnvRegistry.begin(); }

  /// @return map iterator to the end of the converter list
  iterator	end() { return m_cnvRegistry.end(); }
  
protected:
  TId_Map_t	m_cnvRegistry;	//!< map of the converters, indexed by type_index
};





template<class CONVERTER_BASE>
inline
CONVERTER_BASE *
TPConverterTypeMap<CONVERTER_BASE>
::findConverter(const std::type_info &objTypeInfo) const
{
   typename TId_Map_t::const_iterator	iter = m_cnvRegistry.find( std::type_index(objTypeInfo) );
   return (iter == m_cnvRegistry.end())?
      0 : iter->second; 
}


template<class CONVERTER_BASE>
inline
void
TPConverterTypeMap<CONVERTER_BASE>
::addConverter(CONVERTER_BASE *converter, const std::type_info &objTypeInfo)
{
   m_cnvRegistry[std::type_index(objTypeInfo)] = converter;
}



#endif
