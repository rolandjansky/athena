/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENACONVERTER_TP_EXT_H
#define ATHENACONVERTER_TP_EXT_H

/** @file AthenaConverterTLPExtension.h
 *  @brief Extensions to AthenaConverters used to implement TP separation
 *  @author Marcin.Nowak@cern.ch
 **/

class TopLevelTPCnvBase;
class IConverter;
class Token;
#include <vector>
#include <string>
#include <map>

/** @class AthenaConverterTLPExtension
    @brief This class is used to add functionality to a standard AthenaConverter.

    AthenaConverterTLPExtension does not work standalone and should not
    be used directly. T_AthenaPoolExtendingCnv, which inherits from
    it, is the enhanced AthenaPool converter base inteded to be used
    to implement new Athena converters.

    AthenaConverterTLPExtension provides:
    - interface to access top level TP converter(s) used by associated
    Athena converter
    - management (read/write/delete) of the group of persistent objects
    consisting of the object being extended together with the objects
    that extend it
    - converter "cloning" - creating more than one instance of a given
    type of Athena converter (not supported by Gaudi)

    AthenaConverterTLPExtension defines interface for both extended and
    extending converters - some methods are implemented only in one
    converter type
*/
    
class AthenaConverterTLPExtension
{
public:
  /// Constructor
  AthenaConverterTLPExtension() : m_TLCnvForReading(0) {}

  /// Destructor
  virtual ~AthenaConverterTLPExtension();

 
  // ----  empty method stubs, implemented only in EXTENDED Athena converters

  /** Writes the extending persistent object created by this converter
      Called from the EXTENDED (principal) Athena converter
      Implemented only in EXTENDING Athena converters
      @param key [in] StoreGet object key (in APR used to determine storage container placement)
      Retursn Token for the written object
  */
  virtual const Token*	writeObject( const std::string& /*key*/ ) { return 0; }

  /** Read the extending object
      @param token [IN] Token of the object to read
  */
  virtual void		readObject( const std::string& ) { }

  /** Clone this Athena Converter.
      Returns the cloned instance
  */
  virtual AthenaConverterTLPExtension*  clone() { return 0; }

  /** Remember the original converter that this one was cloned from
      @param converter [IN] the original converter
  */
  virtual void 		wasClonedFrom( AthenaConverterTLPExtension * ) {}

  /** Find out if this converter needs to be cloned
      Returns true if this converter was already registered once
  */
  virtual bool		needsCloning() const { return false; }

  /** Get name of this converter (anything that identifies it)
      @return Name of this converter
   */
  virtual const std::string   name() const { return "Extended Athena TP Converter"; }
  
  
  /// returns the main top-level TP converter
  /// * To be implemented in the actual Converter implementation *
  virtual TopLevelTPCnvBase*    getTopLevelTPCnv() = 0;


// --------------------------------------------------------------------------------
  
  /// Returns the ID of the main top-level TP converter 
  unsigned short        getTPCnvID();

  /// returns the current top-level TP converter. This converter may be
  /// different from the main one in case of schema evolution
  TopLevelTPCnvBase*    getTopLevelTPCnvForReading() { return m_TLCnvForReading; }

  /// Sets top-level TP converter to be used for reading the next object
  /// @param cnv [IN] top-level TP converter to be used for reading the next
  void                  usingTPCnvForReading( TopLevelTPCnvBase &cnv);

  /** Reset to 0 the TL TP converter for reading. 
      Prevents the use of the previous TP converter if none was set before
      reading the next object 
      Called automatically after reading an object.
  */
  void                  resetTPCnvForReading() { m_TLCnvForReading = 0; }


  /// Write out all extending persistent objects
  /// @param baseObj [IN] pointer to the principal persistent object (where list of tokens will be stored)
  /// @param key [IN] StoreGate key (string) - placement hint to generate storage container name
  virtual void          writeExtendingObjects( void *baseObj, const std::string& key );

  /// Read all component persistent objects
  /// @param baseObj [IN] pointer to the principal persistent object
  /// holding the list of tokens for all component persistent objects to read
  virtual void          readExtendingObjects( void *baseObj );

  /// Delete persistent objects held by attached extending converters
  /// (used mainly in case of abort)
  virtual void          deletePersistentObjects();

  /// Register extending converter (that is, another converter that will extent this converter)
  /// and all his elemental converters
  /// @param cnv [IN] pointer to extending Athena converter
  virtual bool          registerExtendingCnv( AthenaConverterTLPExtension *cnv );
  
protected:
  // typeless invocation to handle types #defined in AthenaPoolCnv
  void                  usingTPCnvForReading( void *cnv );
  
protected:
  typedef std::map<unsigned, AthenaConverterTLPExtension*> extCnvMap_t;

  /// map of Athena converters extending this one
  extCnvMap_t	        m_extendingConverters;

  typedef std::map<TopLevelTPCnvBase*, extCnvMap_t*>    extCnvMapMap_t;

  // map of different extCnvMap_t for each of the old version of the TLP converter
  extCnvMapMap_t        m_extCnvMapMap;

  /// additional Top Level TP converter used only for reading
  /// tells which converter is used in case of reading old versions
  /// (as different versions have different converters)
  /// NOT owned byt his class
  TopLevelTPCnvBase     *m_TLCnvForReading;

  /// list of duplicated converters to delete at the end
  /// held in the original converter
  std::vector< AthenaConverterTLPExtension* >	m_clonedExtendingCnvs;
};

#endif







