/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef PATCORE_TACCEPT_H
#define PATCORE_TACCEPT_H

/**
   @class TAccept
   @brief Object to encode the result of several cuts

   @author Karsten Koeneke (CERN)
   @date   April 2011

*/

#include <TString.h>
#include <map>
#include <bitset>


namespace Root {
  // this ought to be deprecated, but we have so many clients, that we
  // are not doing it yet.
  class [[deprecated("please use TAcceptInfo+TAcceptData for multi-threaded code")]] TAccept
  {

  public: 
    /** The number of bits for cuts */
    static const unsigned int NBITS=32;

    /** Standard constructor */
    TAccept(const char* name="TAccept");

    /** Standard copy constructor: DO NOT COPY IT! It is expensive to copy! */
    TAccept( const TAccept& parent );
  
    /** Standard assignment operator: DO NOT COPY IT! It is expensive to copy! */
    TAccept& operator= ( const TAccept& rhs );
  
    /** Standard destructor */
    virtual ~TAccept();
  
  public:
    /** Overload the boolean cast; allows for: if(myTAccept) */
    inline operator bool() const
    {
      return (m_accept.count() == m_cutMap.size()) ? true : false ;
    };


    /** Get the name of the class instance */
    inline const char* getName() const { return m_name; };


    /** Clear all bits */
    inline void clear() { m_accept.reset(); }

    /** Set all bits to true */
    inline void clearPositive() 
    {
      for ( unsigned int i=0; i<m_cutMap.size(); ++i ) m_accept[i] = true;
    }


    /** Get the number of cuts defined */
    inline unsigned int getNCuts() const { return m_cutMap.size(); };


    /** Add a cut; returning the cut position */
    int addCut( const TString& cutName, const TString& cutDescription );


    /** Get the bit position of a cut */
    inline unsigned int getCutPosition( const TString& cutName ) const 
    {
      std::map< TString, std::pair< TString, unsigned int > >::const_iterator it = m_cutMap.find(cutName);
      return (it != m_cutMap.end()) ? (it->second).second : 999999;
    }


    /** Get the name of a cut, based on the cut position (slow, avoid usage)*/
    const TString& getCutName( unsigned int cutPosition ) const;


    /** Get the description of a cut, based on the cut name */
    inline const TString& getCutDescription( const TString& cutName ) const
    {
      std::map< TString, std::pair< TString, unsigned int > >::const_iterator it = m_cutMap.find(cutName);
      return (it != m_cutMap.end()) ? (it->second).first : m_emptyString;
    }

    /** Get the description of a cut, based on the cut position */
    const TString& getCutDescription( unsigned int cutPosition ) const;


    /** Get the result of a cut, based on the cut name (safer) */
    inline bool getCutResult( const TString& cutName ) const
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
      std::bitset<NBITS> inverted;
      // Iterate over the "active" bits and invert them.
      for ( std::size_t i=0; i<m_cutMap.size(); ++i )
      {
        inverted[i] = !(m_accept.test(i));
      }
      return inverted;
    }

    /** Get an inverted bitset as an unsigned integer of the cut result. 
        This is IsEM-style, i.e, 0 means that all cuts are passed */
    inline unsigned int getCutResultInverted() const
    {
      std::bitset<NBITS> inverted;
      // Iterate over the "active" bits and invert them.
      for ( std::size_t i=0; i<m_cutMap.size(); ++i )
      {
        inverted[i] = !(m_accept.test(i));
      }
      return inverted.to_ulong();
    }



    /** Set the result of a cut, based on the cut name (safer) */
    inline void setCutResult( const TString& cutName, bool cutResult )
    {
      unsigned int cutPosition = getCutPosition(cutName);
      return setCutResult( cutPosition, cutResult );
    }

    /** Get the result of a cut, based on the cut position (faster) */
    inline void setCutResult( unsigned int cutPosition, bool cutResult )
    {
      m_accept[cutPosition] = cutResult;
      return;
    }


    /** Set the result of a cut, based on the cut name (safer) */
    inline void setCutDescription( const TString& cutName, const TString& cutDescription )
    {
      unsigned int cutPosition = getCutPosition(cutName);
      return setCutDescription( cutPosition, cutDescription );
    }

    /** Get the result of a cut, based on the cut position (faster) */
    void setCutDescription( unsigned int cutPosition, const TString& cutDescription );


#ifdef ROOTCORE
    /** For convenient PROOF support */
    ClassDef(TAccept,1);
#endif


    // Protected members
  protected:
    /** The name of the class instance */
    TString m_name;


    // Private members
  private:
    /** The cut bits */
    std::bitset<NBITS> m_accept;

    /** The map for mapping cut names to their description and position */
    std::map< TString, std::pair< TString, unsigned int > > m_cutMap;

    /** An empty string as default return for some member methods */
    TString m_emptyString;


  }; // End: class definition

} // End: namespace Root


#endif
