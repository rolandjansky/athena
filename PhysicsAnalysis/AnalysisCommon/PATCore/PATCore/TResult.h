/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TRESULT__
#define __TRESULT__

/**
   @class TResult
   @brief Object to encode the result of retrieving efficiencies, scale factors,...

   @author Karsten Koeneke (CERN)
   @date   April 2011

*/

#include <TString.h>
#include <map>
#include <vector>




namespace Root {
  class [[deprecated("do not use for multi-threaded code, use xAOD decorations instead")]] TResult
  {

  public: 
    /** Standard constructor */
    TResult(const char* name="TResult");

    /** Standard copy constructor: DO NOT COPY IT! It is expensive to copy! */
    TResult( const TResult& parent );
  
    /** Standard assignment operator: DO NOT COPY IT! It is expensive to copy! */
    TResult& operator= ( const TResult& rhs );
  
    /** Standard destructor */
    virtual ~TResult();
  
  public:
    /** Overload the double operator; allows for:
        double myScaleFactor = double(myTResult) */
    inline operator double() const
    {
      return getNResults() >= 1 ? m_result[0] : m_defaultResult;
    }


    /** Overload the double operator; allows for:
        std::pair<double,double> myScaleFactorAndUncertainty = std::pair<double,double>(myTResult) */
    inline operator std::pair<double,double>() const
    {
      return getNResults() >= 2 ? std::make_pair( m_result[0], m_result[1] ) : std::make_pair( m_defaultResult, m_defaultResult );
    }


    /** Get the zeroth entry, by convention, this is the efficiency or scale factor or MVA response or... */
    inline double getEfficiency() const { return double(*this); };

    /** Get the zeroth entry, by convention, this is the efficiency or scale factor or MVA response or... */
    inline double getScaleFactor() const { return double(*this); };

    /** Get the zeroth entry, by convention, this is the efficiency or scale factor or MVA response or... */
    inline double getMVAResponse() const { return double(*this); };

    /** Get the first entry, by convention, this is the total uncertainty */
    inline double getTotalUncertainty() const
    {
      return getNResults() >= 2 ? m_result[1] : m_defaultResult;
    }



    /** Get the name of the class instance */
    inline const char* getName() const { return m_name; }


    /** Clear all stored values (not the names and descriptions thought) and assign the default value for each result */
    inline void clear() 
    {
      for ( unsigned int i=0; i < m_result.size(); i++ )
        {
          m_result[i] = m_defaultResult;
        }
    }



    /** Get the number of results defined */
    inline unsigned int getNResults() const { return m_resultMap.size(); }


    /** Add a result; returning the result position */
    int addResult( const TString& resultName, const TString& resultDescription );


    /** Get the position of a result */
    inline unsigned int getResultPosition( const TString& resultName ) const 
     {
      std::map< TString, std::pair< TString, unsigned int > >::const_iterator it = m_resultMap.find(resultName);
      return (it != m_resultMap.end()) ? (it->second).second : 999999;
    }


    /** Get the name of a result, based on the result position (slow, avoid usage)*/
    const TString& getResultName( unsigned int resultPosition ) const;


    /** Get the description of a result, based on the result name */
    inline const TString& getResultDescription( const TString& resultName ) const
    {
      std::map< TString, std::pair< TString, unsigned int > >::const_iterator it = m_resultMap.find(resultName);
      return (it != m_resultMap.end()) ? (it->second).first : m_emptyString;
    }

    /** Get the description of a result, based on the result position */
    const TString& getResultDescription( unsigned int resultPosition ) const;


    /** Get a result, based on the result name (safer) */
    inline double getResult( const TString& resultName ) const
    {
      unsigned int resultPosition = getResultPosition(resultName);
      return getResult(resultPosition);
    }

    /** Get a result, based on the result position (faster) */
    inline double getResult( unsigned int resultPosition ) const
    {
      return m_result[resultPosition];
    }


    /** Get the vector of results (Don't use this as it is slow) */
    inline std::vector<double> getResults() const
    {
      return m_result;
    }


    /** Set a result, based on the result name (safer) */
    inline void setResult( const TString& resultName, double resultResult )
    {
      unsigned int resultPosition = getResultPosition(resultName);
      return setResult( resultPosition, resultResult );
    }

    /** Set a result, based on the result position (faster) */
    inline void setResult( unsigned int resultPosition, double resultResult )
    {
      m_result[resultPosition] = resultResult;
      return;
    }


    /** Set a result, based on the result name (safer) */
    inline void setResultDescription( const TString& resultName, const TString& resultDescription )
    {
      unsigned int resultPosition = getResultPosition(resultName);
      return setResultDescription( resultPosition, resultDescription );
    }

    /** Set a result, based on the result position (faster) */
    void setResultDescription( unsigned int resultPosition, const TString& resultDescription );


#ifdef ROOTCORE
    /** For convenient PROOF support */
    ClassDef(TResult,1);
#endif


    // Protected members
  protected:
    /** The name of the class instance */
    TString m_name;


    // Private members
  private:
    /** The default result value */
    double m_defaultResult;

    /** The results */
    std::vector<double> m_result;

    /** The map for mapping result names to their description and position */
    std::map< TString, std::pair< TString, unsigned int > > m_resultMap;

    /** An empty string as default return for some member methods */
    TString m_emptyString;


  }; // End: class definition

} // End: namespace Root


#endif
