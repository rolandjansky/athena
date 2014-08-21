/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_TOOLS_IDENTCONTINDEX_H
# define ATHLINKS_TOOLS_IDENTCONTINDEX_H

/** @class IdentContIndex 
 * @brief Identifiable container index to a contained object
 *
 * IdentContIndex is a helper class used in conjunction with
 * IdentContIndexingPolicy to create a compound index to an object in
 * an identifiable container. The index is composed of the hash id of
 * the collection and the index of a object in the collection. An
 * IdentContIndex provides an unsigned int compound, composed of two
 * 16 bit hash id and collection index.
 *
 * @author RD Schaffer
 * $Id: IdentContIndex.h,v 1.2 2007-06-14 22:41:47 ssnyder Exp $
 **/

class IdentContIndex 
{
public:
    /// Constructors
    //@{	
    IdentContIndex();
    IdentContIndex(unsigned short collHash, unsigned short objIndex);
    IdentContIndex(unsigned int hashAndIndex);
    //@}
    /// Accessor to hash, obj index and combined index
    //@{	
    /// collection hash id
    unsigned short collHash()     const;
    /// object index in collection
    unsigned short objIndex()     const;
    /// combined index
    unsigned int   hashAndIndex() const;
    /// check that both fields are set
    bool           isValid()      const;
    //@}
        
    /// Setters of hash, obj index and combined index
    //@{	
    /// set collection hash id
    void setHashAndIndex(unsigned int hashAndIndex);
    /// set object index in collection
    void setCollHash(unsigned short hash);
    /// set combined index
    void setObjIndex(unsigned short index);
    //@}

private:
    unsigned short m_collHash;
    unsigned short m_objIndex;
};



// inline definitions

inline 
IdentContIndex::IdentContIndex() 
        :
	m_collHash(0xFFFF),
        m_objIndex(0xFFFF)
{}

inline 
IdentContIndex::IdentContIndex(unsigned short collHash, unsigned short objIndex)
        :
	m_collHash(collHash),
        m_objIndex(objIndex)
{}

inline 
IdentContIndex::IdentContIndex(unsigned int hashAndIndex)
{
    setHashAndIndex(hashAndIndex);
}


inline 
unsigned short
IdentContIndex::collHash() const
{
    return (m_collHash);
}

inline 
unsigned short
IdentContIndex::objIndex() const
{
    return (m_objIndex);
}

inline 
unsigned int   
IdentContIndex::hashAndIndex() const
{
    // Concatenate hash and index
    unsigned int result = (m_collHash << 16) + m_objIndex;
    return (result);
}
        
inline 
bool
IdentContIndex::isValid()      const
{
    // is valid if both hash and index are not default values
    return (m_collHash != 0xFFFF && m_objIndex != 0xFFFF);
}


inline 
void
IdentContIndex::setHashAndIndex(unsigned int hashAndIndex)
{
    // Extract hash and index
    m_collHash = static_cast<unsigned short> (hashAndIndex >> 16);
    m_objIndex = static_cast<unsigned short> (hashAndIndex & 0xFFFF);
}

inline 
void
IdentContIndex::setCollHash(unsigned short hash)
{
    m_collHash = hash;
}

inline 
void
IdentContIndex::setObjIndex(unsigned short index)
{
    m_objIndex = index;
}

#endif // not ATHLINKS_TOOLS_IDENTCONTINDEX_H
