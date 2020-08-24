/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTConditionsData_TRTCondIdentifier_h
#define TRTConditionsData_TRTCondIdentifier_h

#include <iostream>
#include <string>

namespace TRTCond
{
  // Important note: inside the conditions code, we'll number the
  // barrel/endcap elements differently, in order to ensure that they
  // are labeled from something that starts at 0. an alternative is to
  // introduce an offset here in the identifier. i'll think about
  // that.

  /**
   @class ExpandedIdentifier

     @brief Identifier for TRT detector elements in the conditions code

     @author Wouter Hulsbergen

     Note that this identifier is for use by clients. Inside the code elements
     are numbered by indices that always start at 0.
  */
  class ExpandedIdentifier
  {
  private:
    size_t m_index[5] ;  //!< Index within levels higher than DETECTOR
    size_t m_level ;     //!< Granularity level
  public:

    /** enum to specify the granularity level */
    enum Level { DETECTOR=0,BARRELEC=1, LAYERWHEEL=2, MODULE=3, STRAWLAYER=4, STRAW=5 } ;

    /** default constructor */
    ExpandedIdentifier() : m_level(STRAW) { 
      m_index[0] = m_index[1] = m_index[2] = m_index[3] = m_index[4] = 0 ;}

    /** normal constructor */
    ExpandedIdentifier(int bec, int layerorwheel, int module, int strawlayer, int straw, int l=STRAW) 
      : m_level(l) {
	m_index[0] = becidtoindex(bec) ; // this is where we couild introduce a map
	m_index[1] = layerorwheel ;
	m_index[2] = module ;
	m_index[3] = strawlayer ;
	m_index[4] = straw ;
      }

    // index operator subtracting 1 from the supplied index to skip the 'detector' level
    //const size_t& operator[](size_t l) const { return m_index[l-1] ; }
    //size_t& operator[]( size_t l ) { return m_index[l-1] ; }
    
    /** returns const index at given level. Note that internally there is no indexing of the DETECTOR level */
    const size_t& index(size_t l) const { return m_index[l-1] ; }

   /** returns index at given level */
    size_t& index(size_t l) { return m_index[l-1] ; }

   /** returns level */
    size_t level() const { return m_level ; }
    
    /** returns level name */
    const std::string name(size_t l) const { 
      static const std::string names[] = {"detector","barrelec","layer_or_wheel","module","straw_layer","straw"} ;
      return names[l] ;
    }
    
    /** print the object */
    void print(std::ostream& os = std::cout) const ;

   /** write the object to file */
    void write(std::ostream& os) const ;

   /** read the object from file */
    void read(std::istream& is) ;
    
   /** returns subdetector index given the barrel_ec number */
    size_t becidtoindex(int becid) const { return becid<0 ? becid+2 : becid + 1 ; }

   /** returns barrel_ec number given the subdetector index  */
    int    becindextoid(size_t becindex) const { return becindex<2 ? int(becindex)-2 : becindex-1; }

    /** returns TRT_ID index at given level */
    int    id(size_t l) const { return l==BARRELEC ? becindextoid(index(l)) : index(l) ; }

  } ;

}  

inline std::ostream& operator<<(std::ostream& os, const TRTCond::ExpandedIdentifier& id)
{
  id.write(os) ;
  return os ;
}

inline std::istream& operator>>(std::istream& is, TRTCond::ExpandedIdentifier& id)
{
  id.read(is) ;
  return is ;
}

#endif
