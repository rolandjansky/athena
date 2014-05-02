//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARRAWCONDITIONS_LARCONDITIONSVSCALIBDB_H
#define LARRAWCONDITIONS_LARCONDITIONSVSCALIBDB_H

//class LArCablingService ;

/** Template class for condition objects organized vs calibration line
 *
 * @author M. Fanti
 * @version  0-0-1 ,  7/02/06
 *
 * History:
 *  - 
 */

template < class T >
class LArConditionsVsCalibDB 
{
  
public:

  static const unsigned nChannelsBoard = 128 ;   // channels per calibration board

  typedef unsigned int                            CB_Id ;     // this refers to Calib Board [feedthrough,slot]
  typedef std::vector<T>                          T_vector ;  // vector of 128 T's
  typedef typename std::pair<CB_Id,T_vector>      CB_Tvec_pair ;
  typedef typename std::vector<CB_Tvec_pair>      CB_vector ;
  typedef typename CB_vector::const_iterator      Const_CB_It ;
  typedef typename CB_vector::iterator            CB_It ;
  
  LArConditionsVsCalibDB() {} ;
  virtual ~LArConditionsVsCalibDB() {} ;

  Const_CB_It   find(CB_Id id)  const ;  // access conditions for given CB_Id
  Const_CB_It   begin()         const ;  // calib board begin iterator
  Const_CB_It   end()           const ;  // calib board end iterator
  size_t        size()          const ;  // number of calib boards
  void          insert(CB_Id id) ;       // insert new calib board

protected:

  CB_vector m_cb_vect ;

//   bool operator< (const CB_Tvec_pair& x, const CB_Tvec_pair& y) 
//   { return (x.first < y.first); } ;

  class PairSort {
  public:
    bool operator () (const CB_Tvec_pair& x, const CB_Tvec_pair& y) 
    { return (x.first < y.first); }
    bool operator () (const CB_Tvec_pair& x, const unsigned int& id) 
    { return (x.first < id); }
    bool operator () (const unsigned int& id, const CB_Tvec_pair& y) 
    { return (id < y.first); }
  };

} ;

// calib board query/navigation:

template < class T > 
inline typename LArConditionsVsCalibDB<T>::Const_CB_It 
LArConditionsVsCalibDB<T>::find(CB_Id id) const {
  Const_CB_It result = std::lower_bound(m_cb_vect.begin(), 
					m_cb_vect.end(),
					id, PairSort()) ;
  if ( result != m_cb_vect.end() && (*result).first == id )  return (result) ;
  return m_cb_vect.end() ;
}

template < class T > 
inline typename LArConditionsVsCalibDB<T>::Const_CB_It 
LArConditionsVsCalibDB<T>::begin() const {
  return m_cb_vect.begin() ;
}
template < class T > 
inline typename LArConditionsVsCalibDB<T>::Const_CB_It 
LArConditionsVsCalibDB<T>::end() const {
  return m_cb_vect.end() ;
}
template < class T > 
inline size_t LArConditionsVsCalibDB<T>::size() const {
  return m_cb_vect.size() ;
}

// insert new slot and sort
template < class T > 
inline void LArConditionsVsCalibDB<T>::insert(CB_Id id) {
  m_cb_vect.push_back( CB_Tvec_pair( id,T_vector( nChannelsBoard ) ) ) ;
  std::sort( m_cb_vect.begin() , m_cb_vect.end() , PairSort() ) ;
}


#endif


