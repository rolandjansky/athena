/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPObjRef_H
#define TPObjRef_H

/** @class TPObjRef

    This class is an object reference used in Athena persistent data
    model. It replaces transient pointers and can also replace
    embedding and inheritance. Works only inside a single top-level
    persistent object or group of objects created by a given
    AthenaPool converter. 

    @author Marcin.Nowak@cern.ch 
*/


class TPObjRef {
public:

  /** @class TPObjRef::typeID_t
      
      This structure holds an ID of a persistent type. The
      ID is local to a given top level persistent object and its TL TP
      converter. The type ID consists of 2 numbers: the ID of a
      basic TP converter, and the ID of the top-level TP converter
      that owns this basic converter.
  */ 
  struct typeID_t {
    /** Constructor:
	@param [in] tlCnvID ID of the top-level TP converter
	@param [in] cnvID ID of the basic TP converter
    */
    typeID_t( unsigned short tlCnvID=0, unsigned short cnvID=0 )
	  : m_TLCnvID(tlCnvID), m_cnvID(cnvID) {}

    /// Returns the ID of the top-level TP converter
    unsigned short topLevelCnvID() const { return m_TLCnvID; }

    /// Returns the ID of the basic TP converter
    unsigned short cnvID() const { return m_cnvID; }

    /// Returns the type ID as an integer
    unsigned	value() const { return m_TLCnvID<<16 | m_cnvID; }

    /// Tests if the type is defined
    bool	isNull() const { return !(m_TLCnvID | m_cnvID); }
    
    unsigned short	m_TLCnvID;	//!< ID of the top-level TP converter
    unsigned short	m_cnvID;	//!< ID if the basic TP converter (inside the TL converter)
  };

  /// Default constructor - generates and empty reference (like a NULL pointer)
  TPObjRef() : m_index(0) {}

  /** Constructor with parameters
      @param [in] typeID type ID of the persistent object the reference is pointing to
      @param [in] index position of the object in the storage vector in the top-level persistent object
  */
  TPObjRef( const typeID_t& typeID, int index )
	: m_typeID(typeID),
	  m_index(index)
  { }

  /// Tests if the reference is NULL
  bool isNull() const { return m_typeID.isNull(); }

  /// returns the type ID (as integer) of the referenced object
  unsigned		typeID() const { return m_typeID.value(); }

  /// returns the ID if the top-level converter for the referenced object
  unsigned short	topLevelCnvID() const { return m_typeID.topLevelCnvID(); }

  /// returns the position of the referenced object in the storage vector
  unsigned		index() const  { return m_index; }

protected:
  typeID_t	m_typeID;	//!< the type ID of the referenced object
  unsigned	m_index;	//!< the position of the referenced object in the storage vecto
};


#endif






