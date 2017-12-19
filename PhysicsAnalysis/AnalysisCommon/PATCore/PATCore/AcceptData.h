/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef PATCORE_ACCEPT_DATA_H
#define PATCORE_ACCEPT_DATA_H

/**
   @class AcceptData
   @brief Object to encode the result of several cuts

   @author Karsten Koeneke (CERN)
   @author Nils Krumnack (Iowa State University)
   @date   April 2011
   @date   September 2017

*/

#include <string>
#include <map>
#include <bitset>
#include <PATCore/AcceptInfo.h>


namespace asg {
  class AcceptData final
  {

  public: 
    /** The number of bits for cuts */
    static const unsigned int NBITS=AcceptInfo::NBITS;

    /** Standard constructor */
    inline explicit AcceptData(const AcceptInfo* val_info)
      : m_info (val_info)
    {}
  
  public:
    /** Overload the boolean cast; allows for: if(myAcceptData) */
    explicit inline operator bool() const
    {
      return m_accept == m_info->getCutMask();
    };


    /** Get the name of the class instance */
    inline const char* getName() const { return m_info->getName(); };


    /** Clear all bits */
    inline void clear() { m_accept.reset(); }

    /** Set all bits to true */
    inline void clearPositive() 
    {
      m_accept = m_info->getCutMask();
    }


    /** Get the number of cuts defined */
    inline unsigned int getNCuts() const { return m_info->getNCuts(); };

    /** Get a bitmask for all cuts defined */
    inline std::bitset<NBITS> getCutMask() const { return m_info->getCutMask(); };


    /** Get the bit position of a cut */
    inline unsigned int getCutPosition( const std::string& cutName ) const 
    {
      return m_info->getCutPosition(cutName);
    }


    /** Get the name of a cut, based on the cut position (slow, avoid usage)*/
    const std::string& getCutName( unsigned int cutPosition ) const
    {
      return m_info->getCutName (cutPosition);
    };


    /** Get the description of a cut, based on the cut name */
    inline const std::string& getCutDescription( const std::string& cutName ) const
    {
      return m_info->getCutDescription (cutName);
    }

    /** Get the description of a cut, based on the cut position */
    const std::string& getCutDescription( unsigned int cutPosition ) const
    {
      return m_info->getCutDescription (cutPosition);
    }


    /** Get the result of a cut, based on the cut name (safer) */
    inline bool getCutResult( const std::string& cutName ) const
    {
      unsigned int cutPosition = getCutPosition(cutName);
      return getCutResult(cutPosition);
    }

    /** Get the result of a cut, based on the cut position (faster) */
    inline bool getCutResult( unsigned int cutPosition ) const
    {
      return m_accept[cutPosition];
    }


    /** Get the cut result bitset */
    inline std::bitset<NBITS> getCutResultBitSet() const
    {
      return m_accept;
    }

    /** Get an inverted bitset of the cut result. 
        This is IsEM-style, i.e, 0 means the cut is passed */
    inline std::bitset<NBITS> getCutResultInvertedBitSet() const
    {
      return m_accept ^ m_info->getCutMask();
    }

    /** Get an inverted bitset as an unsigned integer of the cut result. 
        This is IsEM-style, i.e, 0 means that all cuts are passed */
    inline unsigned int getCutResultInverted() const
    {
      return getCutResultInvertedBitSet().to_ulong();
    }



    /** Set the result of a cut, based on the cut name (safer) */
    inline void setCutResult( const std::string& cutName, bool cutResult )
    {
      unsigned int cutPosition = getCutPosition(cutName);
      return setCutResult( cutPosition, cutResult );
    }

    /** Get the result of a cut, based on the cut position (faster) */
    inline void setCutResult( unsigned int cutPosition, bool cutResult )
    {
      m_accept[cutPosition] = cutResult;
    }


    // Private members
  private:
    /** the info structure describing us */
    const AcceptInfo *m_info = nullptr;

    /** The cut bits */
    std::bitset<NBITS> m_accept;

  }; // End: class definition

} // End: namespace asg


#endif
