/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_EFFINFO_H
#define MONTECARLOREACTUTILS_EFFINFO_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>



namespace MonteCarloReact {

  class TrigVersion {

  public:
    /* Contructors */
    TrigVersion() : m_min(0), m_max(0) {}

    TrigVersion(const std::string& version) ;

    TrigVersion(const std::string& version_min, const std::string& version_max) ;

    bool contains(const TrigVersion& version) const {
      return (version.m_min >= m_min)  && (m_max < 0 || version.m_min <= m_max)  && 
        (m_max <0 || version.m_max <= m_max)  && (version.m_max <0 || version.m_max >= m_min) ; }

    bool isContainedBy(const TrigVersion& version) const {
      return version.contains(*this); }

    bool operator== (const TrigVersion& version) const {
      return (m_min == version.m_min) &&  (m_max == version.m_max);}

    bool operator!= (const TrigVersion& version) const {
      return (m_min != version.m_min) ||  (m_max != version.m_max);}


  private:
    float m_min ;
    float m_max ;

  } ;

  
  class EffInfo {
  public:
    /* Constructor */
    EffInfo() {;}
    EffInfo(std::string datatype, std::string efftype, std::string effname,
            std::vector<std::string> effvarnames, int effversion, int objectversion,
            std::string objecttype, std::string objectquality, std::string objectalgorithm);

    
    /* Destructor */
    virtual ~EffInfo();

    /* Methods to set and access all the meta data */
    void setDataType( const std::string & type);
  
    std::string getDataType() const;

    void setEffType( const std::string & type);
  
    std::string getEffType() const;
  
    void setEffName( const std::string & name);

    std::string getEffName() const;
  
    void setEffVarNames(const std::vector<std::string>& names );
  
    std::vector<std::string> getEffVarNames() const;
  
    int getEffNVars() const;

    void setEffVersion( int version);
  
    int getEffVersion() const;
  
    void setEffMethod( const std::string & input);

    std::string getEffMethod() const;

    void setEffTagQuality( const std::string & input);

    std::string getEffTagQuality() const;

    //-------- Epoch defenition -------------------------------

    void setRunRangeLow( long lo);

    long getRunRangeLow() const;
  
    void setRunRangeHigh( long hi);

    long getRunRangeHigh() const;
  
    void setDQDefinition( const std::string & input);

    std::string getDQDefinition() const;

    void setDataSetName( const std::string & input);

    std::string getDataSetName() const;
    
    void setTriggerVersion( const std::string & input);

    std::string getTriggerVersion() const;
    
    void setTriggerVersionHigh( const std::string & input);

    std::string getTriggerVersionHigh() const;

    void setTriggerVersionLow( const std::string & input);

    std::string getTriggerVersionLow() const;

    // ------------ Object meta data ------------------------------------

    void setObjType( const std::string & type);
  
    std::string getObjType() const;
  
    void setObjQuality( const std::string & quality);

    std::string getObjQuality() const;  
  
    void setObjVersion( int version);
  
    int getObjVersion() const;
  
    void setObjRelativeTo( const std::string & input);

    std::string getObjRelativeTo() const;

    void setObjAlgorithm( const std::string & algo);
  
    std::string getObjAlgorithm() const;
    
    // ------------ Additional meta data ---------------------------------

    std::string getATLASNoteID() const;
    
    void setATLASNoteID( const std::string & note );
  
    /// several comments line could be added
    void setComments( const std::string & input);

    std::vector <std::string>  getComments() const;


    bool getNParameters( const std::string & id, int & i) const;
    
    // --------------- END of the META DATA  --------------
  
    std::string makeFileName() const;
    
    template<class T> void setEntry( const std::string & id, const T & value);
  
    void setEntry( const std::string & id, const std::vector< std::string > & values);
  
    template< class T> bool addToEntry( const std::string & id, const T & value);
  
    template<class T> bool getEntry( const std::string & id, T & value, int i =0) const;

    bool getEntry( const std::string & id, std::vector< std::string> & vec) const;
  
    template<class type> std::string convertToString(const type & val)  const; 
  
    virtual bool parseInputLine( const std::string & key,
                                 const std::vector< std::string> & line );

    // Return true if :
    // - info is more specific request than than this object 
    // (has more parameters specified) AND
    // - all fields of this object are the same as fields in the info object AND
    // ( TriggerVersion matched to the trigger version in info object OR
    //  triggerVersion inside region TriggerVersionLow -- TriggerVersionHigh)
    bool contains( const EffInfo & info) const ;
    
    // return true if this object has exactly the same number of parameters and
    // all parameters are the same
    // ATLASNote number and Comments parameters  are excluded from the comparison.
    bool operator== (const EffInfo& info) const {return isContainedBy(info) && contains(info);}

    bool operator!= (const EffInfo& info) const {return !isContainedBy(info) || !contains(info);}

    bool isContainedBy( const EffInfo & info) const {return info.contains(*this) ;}

    void clear() {m_map.clear();}
  
  private:
    std::map<std::string, std::vector<std::string> > m_map;
  
    friend std::ostream & operator << ( std::ostream & os, const EffInfo & ei);  
  };

  
  inline std::ostream & operator << ( std::ostream & os, const EffInfo & info)
    {
      for( std::map< std::string, std::vector<std::string> >::const_iterator 
             mapIter = info.m_map.begin();
           mapIter != info.m_map.end();
           ++mapIter) {
        const std::string & key = mapIter->first;
        std::vector< std:: string > vec = mapIter->second;
        os << key << " : ";
        int n = vec.size();
        for( int i = 0; i < n; ++i) {
          os << vec.at(i) << " ";
        }
        os << std::endl;
      }
      return os;  
    }
}
#endif // _EFFINFO_HPP

