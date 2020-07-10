/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CAN_AnalysisResultList_t_hh_
#define _CAN_AnalysisResultList_t_hh_

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/PixResultsException.hh"
#include <map>
#include <vector>
#include <sstream>
#include "PixelMap.h"

namespace CAN {

  class AnalysisResultList_t;

  class AnalysisResultValueNotFound : public CAN::PixResultsException
  {
    friend class AnalysisResultList_t;
  protected:
    AnalysisResultValueNotFound(const std::string &function_name,
				const std::string &message)
      : CAN::PixResultsException("ANAVAL_NOTEXISTING "+function_name+": "+message)
    {}

  };

  class AverageResult_t
  {
  public:
    AverageResult_t(const unsigned int n, const float mean, const float rms, const float lower99, const float upper99)
      : m_n(n)
    {
      m_values[kMean]=mean;
      m_values[kRMS]=rms;
      m_values[kLower99]=lower99;
      m_values[kUpper99]=upper99;
    }

    AverageResult_t()
      : m_n(0)
    { 
      for(unsigned int i=0; i<kNValues; ++i) { 
	m_values[i]=0;
      }
    }

    enum EValues {kMean, kRMS, kLower99, kUpper99, kNValues};
    float value(int i) const {return m_values[i];};
    unsigned int n() const {return m_n;};

  private:
    unsigned int m_n;
    float m_values[kNValues];
  };


  /** Container for analysis results.
   * Can store :
   * <ul>
   *   <li>unsigned ints<li/>
   *   <li>bools<li/>
   *   <li>average value given by : n, meam, rms, lower 99% and upper 99% bounds.<li/>
   * </ul>
   * Each value is assigned to a variable name and a connectivity name. 
   * The values are stored by <tt>analysis_results.addValue<unsigned int>("a_value","ROD_C1_S11",1)<tt>,
   * they are retrieved by <tt>unsignd int a_value = analysis_results.value<unsigned int>("a_value","ROD_C1_S11")<tt>,
   * or one can iteratre over all stored values. The iterators can be obtained by <tt>analysis_results.begin<unsigned int>()</tt>
   * and <tt>analysis_results.end<unsigned int>()</tt>.
   */
  class AnalysisResultList_t {
  public:

//     void addValue(const std::string &name, const float value) {
//       m_floatValues.insert(std::make_pair(name, value));
//     }

  protected:
    template <class T>
    std::map<std::string, std::map<std::string, T> > &getMap();

    template <class T>
    const std::map<std::string, std::map<std::string, T> > &getMap ATLAS_NOT_THREAD_SAFE () const // const_cast is used.
      {return const_cast<AnalysisResultList_t*>(this)->getMap<T>();}

    void throwValueDoesNotExist(const std::string &var_name, const std::string conn_name) const {
      std::stringstream message;
      message << "No value of name " << var_name << " for connectivity object " << conn_name << ".";
      throw AnalysisResultValueNotFound("AnalysisResultList_t::value<T>",message.str());
    }

  public:
    template <class T>
    void insertValue(const std::string &var_name, const std::string &conn_name, const T &value) {
      getMap<T>()[var_name].insert(std::make_pair(conn_name,value));
    }

    template <class T>
    void addValue(const std::string &var_name, const std::string &conn_name, const T &value) {
      getMap<T>()[var_name][conn_name]=value;
    }

    template <class T>
    const T &value(const std::string &var_name, const std::string &conn_name) const {
      typename std::map< std::string, std::map<std::string, T> >::const_iterator iter=getMap<T>().find(var_name);
      if (iter == getMap<T>().end()) {
	throwValueDoesNotExist(var_name,conn_name);
      }
      typename std::map<std::string, T>::const_iterator val_iter=iter->second.find(conn_name);
      if (val_iter == iter->second.end()) {
	throwValueDoesNotExist(var_name,conn_name);
      }
      return val_iter->second;
    }

    template <class T>
    bool has(const std::string &var_name, const std::string &conn_name) const {
      typename std::map< std::string, std::map<std::string, T> >::const_iterator iter=getMap<T>().find(var_name);
      if (iter != getMap<T>().end()) {
	typename std::map<std::string, T>::const_iterator val_iter=iter->second.find(conn_name);
	if (val_iter != iter->second.end()) {
	  return true;
	}
      }
      return false;
    }

    int nPixelMaps() { return m_pixelMap.size(); };

    template <class T>
    typename std::map< std::string, std::map<std::string, T> >::iterator begin()
      { return getMap<T>().begin(); }

    template <class T>
    typename std::map< std::string, std::map<std::string, T> >::const_iterator begin ATLAS_NOT_THREAD_SAFE () const // Thread unsafe getMap method is used.
      { return getMap<T>().begin(); }

    template <class T>
    typename std::map< std::string, std::map<std::string, T> >::const_iterator end ATLAS_NOT_THREAD_SAFE () const // Thread unsafe getMap method is used.
      { return getMap<T>().end(); }

  protected:
    std::map<std::string, std::map<std::string, bool> >            m_boolValues;
    std::map<std::string, std::map<std::string, unsigned int> >    m_uintValues;
    std::map<std::string, std::map<std::string, float> >           m_floatValues; //CLA
    std::map<std::string, std::map<std::string, AverageResult_t> > m_avValues;
    std::map<std::string, std::map<std::string, PixelMap_t> >      m_pixelMap; //CLA
  };


  template <> inline std::map<std::string, std::map<std::string, bool> > &
  AnalysisResultList_t::getMap<bool>() { return AnalysisResultList_t::m_boolValues; }

  template <> inline std::map<std::string, std::map<std::string, unsigned int> > &
  AnalysisResultList_t::getMap<unsigned int>() { return AnalysisResultList_t::m_uintValues; }

  template <> inline std::map<std::string, std::map<std::string, float> > & 
  AnalysisResultList_t::getMap<float>() { return AnalysisResultList_t::m_floatValues; } //CLA

  template <> inline std::map<std::string, std::map<std::string, AverageResult_t> > &
  AnalysisResultList_t::getMap<AverageResult_t>() { return AnalysisResultList_t::m_avValues; }

  template <> inline std::map<std::string, std::map<std::string, PixelMap_t> > & 
  AnalysisResultList_t::getMap<PixelMap_t>() { return AnalysisResultList_t::m_pixelMap; }

  // example:
  //
  //    inline void test() {
  //     AnalysisResultList_t a;
  //     a.addValue<unsigned int>("test","ROD_C1_S11",1);
  //     unsigned int a_value = a.value<unsigned int>("test","ROD_C1_S11");
  //
  //     for (std::map<std::string, std::map<std::string, unsigned int > >::const_iterator iter=a.begin<unsigned int>();
  // 	 iter != a.end<unsigned int>();
  // 	 iter ++) {
  //
  //       for (std::map<std::string, unsigned int >::const_iterator val_iter=iter->second.begin();
  // 	   val_iter != iter->second.end();
  // 	   val_iter ++) {
  // 	      unsigned int b=val_iter->second;
  //       }
  //     }
  //   }

}

#endif
