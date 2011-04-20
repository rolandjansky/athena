/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_RESINFO_H
#define MONTECARLOREACTUTILS_RESINFO_H
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>

namespace MonteCarloReact {

  
  class ResInfo {
  public:
  
    ResInfo() {;}
    ResInfo(std::string datatype, std::string restype, std::string resname,
            std::vector<std::string> resvarnames, std::vector<std::string> shiftparameters,
            int effversion, int objectversion,
            std::string objecttype, std::string objectquality, std::string objectalgorithm);

    
    virtual ~ResInfo();

    // ------------ Resolution meta data ---------------------------------

    void setDataType( const std::string & type);
  
    std::string getDataType() const;
    
    void setResType( const std::string & type);
  
    std::string getResType() const;
  
    void setResName( const std::string & name);

    std::string getResName() const;
  
    void setResVarNames(const std::vector <std::string> & names );
  
    std::vector < std::string> getResVarNames() const;

    void setShiftParameters(const std::vector <std::string> & names );
  
    std::vector < std::string> getShiftParameters() const;
    
    int getResNVars() const;

    int getResNParas() const;
  
    void setResVersion( int version);
  
    int getResVersion() const;
  
    void setResMethod( const std::string & input);

    std::string getResMethod() const;
    
    void setResTagQuality( const std::string & input);

    std::string getResTagQuality() const;

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
  
    void setComments( const std::string & input);

    std::vector <std::string>  getComments() const;

    // --------------- META DATA FOR FUNCTIONS ------------

    void setResFuncNames(const std::vector <std::string> & names );

    std::vector <double> getResFuncBegin() const;
    
    std::vector < std::string> getResFuncNames() const;

    void setResFuncBegin(const std::vector <double> & begin );

    void setResFuncEnd(const std::vector <double> & end );
  
    std::vector <double> getResFuncEnd() const;
  
    int getResNFunc() const;

    // --------------- END of the META DATA  --------------
  
    std::string makeFileName() const;

    template<class T> void setEntry( const std::string & id, const T & value);

    template<class T> void setEntry( const std::string & id,const std::vector< T > & value);
  
    template< class T> bool addToEntry( const std::string & id, const T & value);
  
    template<class T> bool getEntry( const std::string & id, T & value, int i =0) const;

    template<class T> bool getEntry( const std::string & id,std::vector< T > & value) const;  

    bool getEntry( const std::string & id, std::vector< std::string> & vec) const;
  
    bool getNParameters( const std::string & id, int & i) const;
  
    template<class type> std::string convertToString(const type & val)  const;
  
    virtual bool parseInputLine( const std::string & key, const std::vector< std::string> & line );
  
    bool contains( const ResInfo & info) const ;

    bool isContainedBy( const ResInfo & info) const {return info.contains(*this) ;}

    /// return true if this object has exactly the same number of parameters and all parameters are the same
    /// ATLASNote number and Comments parameters  are excluded from the comparison.
    bool operator== (const ResInfo& info) const {return isContainedBy(info) && contains(info);}

    bool operator!= (const ResInfo& info) const {return !isContainedBy(info) || !contains(info);}

    void clear() {m_map.clear();}
  
  private:
    std::map<std::string, std::vector<std::string> > m_map;
  
    friend std::ostream & operator << ( std::ostream & os, const ResInfo & ei);  
  };


  inline std::ostream & operator << ( std::ostream & os, const ResInfo & info)
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
#endif // 

