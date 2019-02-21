/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef PATCORE_ACCEPT_INFO_H
#define PATCORE_ACCEPT_INFO_H

/**
   @class AcceptInfo
   @brief Object to describe a series of cuts

   @author Karsten Koeneke (CERN)
   @author Nils Krumnack (Iowa State University)
   @date   April 2011
   @date   Sep 2017

*/

#include <string>
#include <map>
#include <bitset>


namespace asg {
  class AcceptInfo final
  {

  public: 
    /** The number of bits for cuts */
    static const unsigned int NBITS=32;

    /** Standard constructor */
    AcceptInfo(const char* name="AcceptInfo")
      : m_name(name),
        m_cutMap()
    {}
  
  public:

    /** Get the name of the class instance */
    inline const char* getName() const { return m_name.c_str(); };

    /** Get the number of cuts defined */
    inline unsigned int getNCuts() const { return m_cutMap.size(); };

    /** Get a bitmask for all cuts defined */
    inline std::bitset<NBITS> getCutMask() const { return m_cutMask; };


    /** Add a cut; returning the cut position */
    int addCut( const std::string& cutName, const std::string& cutDescription )
    {
      // Make sure that this new cuts doesn't exceed the number of bits available
      if ( m_cutMap.size() >= NBITS )
      {
        return -1;
      }

      // Add the cut to the map
      std::pair< std::string, unsigned int > cutPair = std::make_pair( cutDescription, m_cutMap.size() );
      m_cutMap.insert( std::make_pair( cutName, cutPair ) );

      // Return the position of the newly added cut in the bitmask
      int result = (m_cutMap.size() - 1);
      m_cutMask.set (result);
      return result;
    }


    /** Get the bit position of a cut */
    inline unsigned int getCutPosition( const std::string& cutName ) const 
    {
      auto it = m_cutMap.find(cutName);
      return (it != m_cutMap.end()) ? (it->second).second : 999999;
    }


    /** Get the name of a cut, based on the cut position (slow, avoid usage)*/
    const std::string& getCutName( unsigned int cutPosition ) const;


    /** Get the description of a cut, based on the cut name */
    const std::string& getCutDescription( const std::string& cutName ) const;

    /** Get the description of a cut, based on the cut position */
    const std::string& getCutDescription( unsigned int cutPosition ) const;


    /** Set the result of a cut, based on the cut name (safer) */
    inline void setCutDescription( const std::string& cutName, const std::string& cutDescription )
    {
      unsigned int cutPosition = getCutPosition(cutName);
      return setCutDescription( cutPosition, cutDescription );
    }

    /** Get the result of a cut, based on the cut position (faster) */
    void setCutDescription( unsigned int cutPosition, const std::string& cutDescription );


    // Private members
  private:

    /** The name of the class instance */
    std::string m_name;

    /** The map for mapping cut names to their description and position */
    std::map< std::string, std::pair< std::string, unsigned int > > m_cutMap;

    /** A bitmap that provides a mask that only leaves defined cuts */
    std::bitset<NBITS> m_cutMask;


  }; // End: class definition

} // End: namespace asg


#endif
