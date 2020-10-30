/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsSubset.h
 *
 * @brief This file defines the template class used for I/O of
 * conditions data
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArConditionsSubset.h,v 1.3 2007-11-30 16:24:05 wlampl Exp $
 */

#ifndef LARRAWCONDITIONS_LARCONDITIONSSUBSET_H
#define LARRAWCONDITIONS_LARCONDITIONSSUBSET_H
/**
 * @class LArConditionsSubset
 *
 * @brief template class for use for I/O of conditions data
 *
 * This class contains conditions objects of type T. There are two
 * ways that conditions objects may be stored:
 *
 * 1) as a vector of FEB ID, vector<T> pairs where T is the conditions
 *    data for each channel in a FEB of a particular ID. 
 * 2) as a vector of Channel ID, T pairs, where T is the conditions
 *    data for a particular channel id 
 *
 * The first way is labelled "subset" and is how the bulk of
 * conditions data is stored. It is efficient in access and
 * storage. 
 * The second way is labelled "channel set" and is used for
 * storing a correction set for individual channels.
 *
 * LArConditionsContainer uses this class to store conditions in
 * different groupings where each group is a different
 * LArConditionsSubset. And as well, the "correction set" is another
 * LArConditionsSubset. 
 *
 * For some types of payload, we want to allow for alternate storage
 * layouts, to save memory.  This is allowed by the
 * LArConditionsSubsetTraits class; using this, one can substitute
 * alternate classes for the types contained in the subset.
 * (This is used for LArShapeP2/LArOFCP1, to collect all the data
 * together to reduce overhead; the contained types then become
 * proxies that access that data.)
**/

#include <vector>
#include <map>
#include <algorithm>


/**
 * @brief Traits class giving the types to use for the objects
 *        contained within the subset.
 *
 * This is the default traits class; this declares that we store
 * directly a vector<pair<FebId, vector<T> > >.
 */
template <class T>
class LArConditionsSubsetTraits
{
public:
  typedef unsigned int                                 FebId; 

  typedef T&                                           Reference;
  typedef const T&                                     ConstReference;
  typedef T*                                           Pointer;
  typedef const T*                                     ConstPointer;
  typedef typename std::vector<T>                      ChannelVector; 
  typedef const typename std::vector<T>                ConstChannelVector; 
  typedef ChannelVector*                               ChannelVectorPointer;
  typedef typename std::pair<FebId, ChannelVector>     FebPair;
  typedef FebPair&                                     FebPairReference;
  typedef typename std::vector<FebPair>                SubsetVector;

  static const T& empty()
  {
    static const T dum {};
    return dum;
  }


  /**
   * @brief Helper used by LArConditionsSubset::assign.
   * @param otherBeg Start of the range to copy.
   * @param otherEnd End of the range to copy.
   * @param to The subset to which to copy.
   * @param copier Helper to copy a single payload object.
   */
  template <class OTHERIT, class COPIER>
  static void copySubset (OTHERIT otherBeg,
                          OTHERIT otherEnd,
                          SubsetVector& to,
                          COPIER copier);
};


template <class T>
template <class OTHERIT, class COPIER>
void LArConditionsSubsetTraits<T>::copySubset (OTHERIT otherBeg,
                                               OTHERIT otherEnd,
                                               SubsetVector& to,
                                               COPIER copier)
{
  to.reserve (std::distance (otherBeg, otherEnd));
  for (; otherBeg != otherEnd; ++otherBeg) {
    ChannelVector v (otherBeg->second.size());
    for (size_t i = 0; i < otherBeg->second.size(); i++) {
      copier (otherBeg->second[i], v[i]);
    }
    to.emplace_back (otherBeg->first, std::move (v));
  }
}


template<class T> 
class LArConditionsSubset
{
public: 

    ///  Public typedefs for FEB id and channel vector - Subset
    typedef          LArConditionsSubsetTraits<T>        Traits;
    typedef typename Traits::FebId                       FebId;
    typedef typename Traits::ChannelVector               ChannelVector;
    typedef typename Traits::ConstChannelVector          ConstChannelVector;
    typedef typename Traits::FebPair                     FebPair;
    typedef typename Traits::FebPairReference            FebPairReference;
    typedef typename Traits::SubsetVector                SubsetVector;
    typedef typename SubsetVector::const_iterator        ConstSubsetIt;
    typedef typename SubsetVector::iterator              SubsetIt;
    typedef typename Traits::Reference                   Reference;
    typedef typename Traits::ConstReference              ConstReference;
    typedef typename Traits::Pointer                     Pointer;
    typedef typename Traits::ConstPointer                ConstPointer;

    ///  Public typedefs for channel id and T vector - CorrectionSet
    typedef T             Payload;
    typedef unsigned int  ChannelId; 
    typedef typename std::pair<ChannelId, T>             CorrectionPair;
    typedef typename std::vector<CorrectionPair>         CorrectionVec;
    typedef typename CorrectionVec::const_iterator       ConstCorrectionVecIt;
    typedef typename CorrectionVec::iterator             CorrectionVecIt;
    typedef typename CorrectionVec::size_type            size_type;

    /// Default constructor
    LArConditionsSubset( );

    /// Constructor for corrections - only need gain
    LArConditionsSubset(unsigned int gain);

    /// Constructor with initializing set of FEB ids
    LArConditionsSubset(const std::vector<FebId>&  ids, unsigned int gain);


    /// Copy from another subset object.
    /// COPIER is a functional to copy the payload, with signature
    ///  (const T&, T*)
    template <class U, class COPIER>
    void assign (const LArConditionsSubset<U>& other,
                 COPIER copier);
    

    /// destructor
    virtual ~LArConditionsSubset();


    /// Access to a channel vector of a given FEB
    ConstSubsetIt           findChannelVector(FebId  febId) const;

    /// Access to a channel vector of a given FEB
    SubsetIt                findChannelVector(FebId  febId);

    /// Iterators over subset
    ConstSubsetIt           subsetBegin() const;
    ConstSubsetIt           subsetEnd()  const;
    SubsetIt                subsetBegin();
    SubsetIt                subsetEnd();

    /// Size of subset
    size_type               subsetSize() const;
    

    /// Access to a conditions object for a given channel id -
    /// searches channel set ONLY
    ConstCorrectionVecIt    findConditionsObj(ChannelId  id) const;

    /// Iterators over channel set
    ConstCorrectionVecIt    correctionVecBegin() const;
    ConstCorrectionVecIt    correctionVecEnd()   const;

    /// Size of channel set
    size_type               correctionVecSize() const;
    

    /// Access to gain
    unsigned int            gain() const; 

    /// Access to the COOL channel number
    unsigned int            channel() const;

    /// Type of grouping - defined in LArConditionsContainerBase.h
    unsigned int            groupingType() const;

    /// Number of conditions objects in this subset
    unsigned int            nConditions() const;

    /// Initialize with set of FEB ids
    void                    initialize (const std::vector<FebId>&  ids, 
					unsigned int gain);

    /// Reallocate to match size actually used.
    void shrink_to_fit();


    /// set gain
    void                    setGain(unsigned int gain); 

    /// set the COOL channel number
    void                    setChannel(unsigned int channel);

    /// set the type of grouping - defined in LArConditionsContainerBase.h
    void                    setGroupingType(unsigned int type);

    /// Insert a new channel id / T pair correction. If new channel id
    /// is the same as an existing one, the new T replaces the old T
    void                    insertCorrection (ChannelId id, const T& cond);

    /// Insert a group of corrections.
    /// They must be properly sorted.
    void                    insertCorrections (CorrectionVec&& corrs);

    //Get NCHANNELPERFEB
    unsigned                channelVectorSize() const;

    /// Fill map from vector
    // This is also called from a ROOT read rule after an object is read.
    // (Nominally private, but needs to be public to be callable
    // from a ROOT read rule.)
    void                    fillMap(); 

private:
    /// Size of channel vector
    enum Subset_size {
	NCHANNELPERFEB = 128
    };

    class PairSort 
    {
    public:
	bool operator () (const CorrectionPair& x, const CorrectionPair& y) {
	    return (x.first < y.first);
	}
    };

    typedef typename std::map<FebId, unsigned int>  SubsetMap;

    SubsetMap               m_subsetMap;
    SubsetVector            m_subset;
    unsigned int            m_gain; 
    unsigned int            m_channel;
    unsigned int            m_groupingType;
    CorrectionVec           m_correctionVec;
};


// INLINE FUNCTIONS

template<class T> 
inline 
void
LArConditionsSubset<T>::fillMap()
{
    // Fill map from subset
    m_subsetMap.clear();

//     std::cout << "fillMap: subset size, map size "
// 	      << m_subset.size() << " " << m_subsetMap.size()
// 	      << std::endl; 

    for (unsigned int i = 0; i < m_subset.size(); ++i) {
	m_subsetMap[m_subset[i].first] = i;
    }

//     std::cout << "fillMap: subset size, map size "
// 	      << m_subset.size() << " " << m_subsetMap.size()
// 	      << std::endl; 
}


template<class T> 
inline 
LArConditionsSubset<T>::LArConditionsSubset( )
	:
	m_gain(0),
	m_channel(0),
	m_groupingType(0)
{}

template<class T> 
inline 
LArConditionsSubset<T>::LArConditionsSubset(unsigned int gain)
	:
	m_gain(gain),
	m_channel(0),
	m_groupingType(0)
{}

template<class T>
inline 
LArConditionsSubset<T>::LArConditionsSubset(const std::vector<FebId>&  ids,
					    unsigned int gain)
	:
	m_subset(ids.size()),
	m_gain(gain),
	m_channel(0),
	m_groupingType(0)

{

//     std::cout << "LArConditionsSubset: ids size, gain "
// 	      << ids.size() << " " << gain
// 	      << std::endl; 

    // Loop over fed id list, insert id and resize channel vector
    for (unsigned int i = 0; i < ids.size(); ++i) {
	m_subset[i].first = ids[i];
	// NOTE: we move the resize of the channel vector for each feb
	// to the non-const find method below. This allows to keep the
	// overall subset size to a minimum for non-full subsets..
	//m_subset[i].second.resize(NCHANNELPERFEB);
    }
    // Fill map for future lookups
    fillMap();
}


template<class T>
template <class U, class COPIER>
void LArConditionsSubset<T>::assign (const LArConditionsSubset<U>& other,
                                     COPIER copier)
{
  m_subsetMap.clear();
  m_gain = other.gain();
  m_channel = other.channel();
  m_groupingType = other.groupingType();

  Traits::copySubset (other.subsetBegin(), other.subsetEnd(), m_subset, copier);

  size_t corrsz = other.correctionVecSize();
  auto otherCorr = other.correctionVecBegin();
  m_correctionVec.resize (corrsz);
  for (size_t i = 0; i < corrsz; i++) {
    m_correctionVec[i].first = otherCorr[i].first;
    copier (otherCorr[i].second, m_correctionVec[i].second);
  }

  fillMap();
}


template<class T>
inline 
LArConditionsSubset<T>::~LArConditionsSubset()
{}


template<class T>
inline 
typename LArConditionsSubset<T>::SubsetIt
LArConditionsSubset<T>::findChannelVector(FebId  febID) 
{

//     std::cout << "findChannelVector:  febid, size "
// 	      << febID << " " << m_subsetMap.size()
// 	      << std::endl; 


    typename SubsetMap::const_iterator  it = m_subsetMap.find(febID);
    if (it != m_subsetMap.end()) {
	unsigned int index = (*it).second;
	if (index < m_subset.size()) {
	    // check that channel vector has been resized already
	    if (m_subset[index].second.size() == 0) {
		m_subset[index].second.resize(NCHANNELPERFEB);
	    }
	    return (m_subset.begin() + index);
	}
    }
    return (m_subset.end());
}


template<class T>
inline 
typename LArConditionsSubset<T>::ConstSubsetIt
LArConditionsSubset<T>::findChannelVector(FebId  febID) const
{
    typename SubsetMap::const_iterator  it = m_subsetMap.find(febID);
    if (it != m_subsetMap.end()) {
	unsigned int index = (*it).second;
	if (index < m_subset.size()) {
	    return (m_subset.begin() + index);
	}
    }
    return (m_subset.end());
}


template<class T>
inline 
typename LArConditionsSubset<T>::ConstSubsetIt        
LArConditionsSubset<T>::subsetBegin() const
{
    return (m_subset.begin());
}

template<class T>
inline 
typename LArConditionsSubset<T>::ConstSubsetIt        
LArConditionsSubset<T>::subsetEnd()  const
{
    return (m_subset.end());
}

template<class T>
inline 
typename LArConditionsSubset<T>::SubsetIt             
LArConditionsSubset<T>::subsetBegin() 
{
    return (m_subset.begin());
}

template<class T>
inline 
typename LArConditionsSubset<T>::SubsetIt             
LArConditionsSubset<T>::subsetEnd()  
{
    return (m_subset.end());
}

template<class T>
inline 
typename LArConditionsSubset<T>::size_type
LArConditionsSubset<T>::subsetSize() const
{
    return (m_subset.size());
}

template<class T>
typename LArConditionsSubset<T>::ConstCorrectionVecIt    
LArConditionsSubset<T>::findConditionsObj(ChannelId  id) const
{
    ConstCorrectionVecIt result =  (std::lower_bound(m_correctionVec.begin(), 
						     m_correctionVec.end(), 
						     CorrectionPair(id, T()),
						     PairSort()));
    if ( m_correctionVec.end() != result && id != result->first ) {
	result = m_correctionVec.end();
    }
    return (result);
}

template<class T>
typename LArConditionsSubset<T>::ConstCorrectionVecIt
LArConditionsSubset<T>::correctionVecBegin() const
{
    return (m_correctionVec.begin());
}

template<class T>
typename LArConditionsSubset<T>::ConstCorrectionVecIt       
LArConditionsSubset<T>::correctionVecEnd()   const
{
    return (m_correctionVec.end());
}



template<class T>
typename LArConditionsSubset<T>::size_type
LArConditionsSubset<T>::correctionVecSize() const
{
    return (m_correctionVec.size());
}
    
    
template<class T>
inline 
unsigned int                  
LArConditionsSubset<T>::gain() const
{
    return (m_gain);
}

template<class T>
inline 
unsigned int                  
LArConditionsSubset<T>::channel() const
{
    return (m_channel);
}


template<class T>
inline 
unsigned int                  
LArConditionsSubset<T>::groupingType() const
{
    return (m_groupingType);
}

template<class T>
inline 
unsigned int                  
LArConditionsSubset<T>::nConditions() const
{
    unsigned int tot = 0;
    for (unsigned int i = 0; i < m_subset.size(); ++i) {
	tot += m_subset[i].second.size();
    }
    return (tot);
}



template<class T>
inline 
void                 
LArConditionsSubset<T>::initialize (const std::vector<FebId>&  ids, unsigned int gain)
{
    m_correctionVec.clear();

    // resize the subset
    m_subset.resize(ids.size());
    m_gain = gain;
    
    // Loop over fed id list, insert id and resize channel vector
    for (unsigned int i = 0; i < ids.size(); ++i) {
	m_subset[i].first = ids[i];
	m_subset[i].second.resize(NCHANNELPERFEB);
    }

    // Fill map for future lookups
    fillMap();
}

template<class T>
inline 
void                 
LArConditionsSubset<T>::shrink_to_fit()
{
  m_subset.shrink_to_fit();
}


template<class T>
inline 
void                 
LArConditionsSubset<T>::setGain(unsigned int gain)
{
    m_gain = gain;
}

template<class T>
inline 
void                 
LArConditionsSubset<T>::setChannel(unsigned int channel)
{
    m_channel = channel;
}

template<class T>
inline 
void                 
LArConditionsSubset<T>::setGroupingType(unsigned int type)
{
    m_groupingType = type;
}

template<class T>
inline 
void 
LArConditionsSubset<T>::insertCorrection (ChannelId id, const T& cond)
{
    m_correctionVec.push_back(CorrectionPair(id,cond));
    std::sort(m_correctionVec.begin(), m_correctionVec.end(), PairSort());
}


template<class T>
inline 
void 
LArConditionsSubset<T>::insertCorrections (CorrectionVec&& corrs)
{
  if (m_correctionVec.empty()) {
    m_correctionVec = std::move (corrs);
  }
  else {
    m_correctionVec.insert (m_correctionVec.end(), corrs.begin(), corrs.end());
    std::sort(m_correctionVec.begin(), m_correctionVec.end(), PairSort());
  }
}


template<class T>
inline 
unsigned
LArConditionsSubset<T>::channelVectorSize() const
{
  return NCHANNELPERFEB;
}



#endif // LARRAWCONDITIONS_LARCONDITIONSSUBSET_H
