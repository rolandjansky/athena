/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBDATA__IDENTIFIEDPROFILEHISTOGRAM_H
#define TRT_CALIBDATA__IDENTIFIEDPROFILEHISTOGRAM_H
/********************************************************************

NAME:     IdentifiedProfileHistogram
PACKAGE:  TRT_AlignData

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Mapped Storage/Access of profile-histograms 
          

********************************************************************/

// INCLUDES:

#include <vector>
#include <numeric>
#include <string>
#include <iostream>

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <CLHEP/Matrix/Matrix.h>

#include "TRT_ConditionsData/ArrayStore.h"

template <typename SomeIDTag>
class IdentifiedProfileHistogram {
public:

  // Constructors
  IdentifiedProfileHistogram(const std::string& a_title, const unsigned int& a_numberofbins, 
			     const float& a_loweredge, const float& a_upperedge, 
			     const float& a_lowerlimit = 0.0, const float& a_upperlimit = 0.0);
  IdentifiedProfileHistogram(const std::string& a_title, const unsigned int& a_numberofbins,
			     const float& a_loweredge, const float& a_upperedge, 
			     const std::vector<float>& a_lowerlimit, const std::vector<float>& a_upperlimit );

  // Destructor
  virtual ~IdentifiedProfileHistogram() {}

  // Accessors
  const std::string& title() const;
  unsigned int numberOfBins() const;
  float lowerEdge() const;
  float upperEdge() const;
  float binWidth() const;
  const std::vector<int>& numberOfEntries(const SomeIDTag& ident) const;
  const std::vector<float>& content(const SomeIDTag& ident) const;
  const std::vector<float>& contentSquared(const SomeIDTag& ident) const;
  int numberOfIDs() const;

  // Statistics calculators
  const std::vector<float>& abcissa() const;
  const std::vector<float>& abcissaError() const;
  const std::vector<float>& average(const SomeIDTag& ident) const;
  const std::vector<float>& rms(const SomeIDTag& ident) const;
  const std::vector<float>& averageError(const SomeIDTag& ident) const;
  float globalAverage(const SomeIDTag& ident) const;
  float globalRMS(const SomeIDTag& ident) const;
  float globalAverageError(const SomeIDTag& ident) const;
  int globalNumberOfEntries(const SomeIDTag& ident) const;
  int totalNumberOfEntries() const;

  //modifiers
  bool fillEntry(const SomeIDTag& ident, const float& abcissa, const float& ordinate, const float& weight = 1.0);

private:

  unsigned int m_NumberOfBins;
  float m_LowerEdge;
  float m_BinWidth;
  std::vector<float> m_LowerLimit;
  std::vector<float> m_UpperLimit;
  ArrayStore<SomeIDTag, std::vector<int> >   m_NumberOfEntries;
  ArrayStore<SomeIDTag, std::vector<float> > m_Content;
  ArrayStore<SomeIDTag, std::vector<float> > m_ContentSquared;
  
  // Do we want histograms summed over several IDs?

};

template <typename SomeIDTag>
inline const std::string& IdentifiedProfileHistogram<SomeIDTag>::title() const {return m_NumberOfEntries.tag();}

template <typename SomeIDTag>
inline unsigned int IdentifiedProfileHistogram<SomeIDTag>::numberOfBins() const {return m_NumberOfBins;}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::lowerEdge() const {return m_LowerEdge;}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::upperEdge() const {return m_LowerEdge+float(m_NumberOfBins)*m_BinWidth;}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::binWidth() const {return m_BinWidth;}

template <typename SomeIDTag>
inline const std::vector<int>& IdentifiedProfileHistogram<SomeIDTag>::numberOfEntries(const SomeIDTag& ident) const {return m_NumberOfEntries[ident];}

template <typename SomeIDTag>
inline const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::content(const SomeIDTag& ident) const {return m_Content[ident];}

template <typename SomeIDTag>
inline const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::contentSquared(const SomeIDTag& ident) const {return m_ContentSquared[ident];}

template <typename SomeIDTag>
inline int IdentifiedProfileHistogram<SomeIDTag>::numberOfIDs() const {return m_NumberOfEntries.numberOfIDs();}

template <typename SomeIDTag>
inline int IdentifiedProfileHistogram<SomeIDTag>::totalNumberOfEntries() const
{
  int i=0;
  for (typename ArrayStore<SomeIDTag, std::vector<int> >::map_citr itr=m_NumberOfEntries.cbegin();itr!=m_NumberOfEntries.cend();++itr)
    i=std::accumulate(m_NumberOfEntries[itr->first].begin(),m_NumberOfEntries[itr->first].end(),i);
  return i;
}

template <typename SomeIDTag>
inline int IdentifiedProfileHistogram<SomeIDTag>::globalNumberOfEntries(const SomeIDTag& ident) const
{
  return m_NumberOfEntries.existID(ident) ? 
    std::accumulate(m_NumberOfEntries[ident].begin(),m_NumberOfEntries[ident].end(),0) : 0; 
}

template <typename SomeIDTag>
inline const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::abcissaError() const{
  const std::vector<float> result(m_NumberOfBins, m_BinWidth/sqrt(12.0)); // strictly only valid for weight=CONST 
  return result;
}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::globalAverage(const SomeIDTag& ident) const{
  return globalNumberOfEntries(ident)>0 ? 
    std::accumulate(m_Content[ident].begin(),m_Content[ident].end(),0.0)/(float)globalNumberOfEntries(ident) : 0.0; 
}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::globalRMS(const SomeIDTag& ident) const{
  return globalNumberOfEntries(ident)>0 ? 
    sqrt(std::accumulate(m_ContentSquared[ident].begin(),m_ContentSquared[ident].end(),0.0)/
	 (float)globalNumberOfEntries(ident)-globalAverage(ident)*globalAverage(ident)) : 0.0;
}

template <typename SomeIDTag>
inline float IdentifiedProfileHistogram<SomeIDTag>::globalAverageError(const SomeIDTag& ident) const{
  return globalNumberOfEntries(ident)>0 ? 
    globalRMS(ident)/sqrt((float)globalNumberOfEntries(ident)) : 0.0; 
}

#ifndef TRT_ALIGNDATA__IDENTIFIEDPROFILEHISTOGRAM_ICC
#define TRT_ALIGNDATA__IDENTIFIEDPROFILEHISTOGRAM_ICC

template <typename SomeIDTag>
IdentifiedProfileHistogram<SomeIDTag>::IdentifiedProfileHistogram(const std::string& a_title,
								  const unsigned int& a_numberofbins, 
								  const float& a_loweredge, const float& a_upperedge, 
								  const float& a_lowerlimit, const float& a_upperlimit) : 
  m_NumberOfBins(a_numberofbins),m_LowerEdge(a_loweredge),
  m_BinWidth((a_upperedge-a_loweredge)/(float)a_numberofbins),
  m_LowerLimit(a_numberofbins,a_lowerlimit),m_UpperLimit(a_numberofbins,a_upperlimit),
  m_NumberOfEntries(a_title) {}

template <typename SomeIDTag>
IdentifiedProfileHistogram<SomeIDTag>::IdentifiedProfileHistogram(const std::string& a_title,
								  const unsigned int& a_numberofbins,
								  const float& a_loweredge, const float& a_upperedge, 
								  const std::vector<float>& a_lowerlimit,
								  const std::vector<float>& a_upperlimit ) : 
  m_NumberOfBins(a_numberofbins),m_LowerEdge(a_loweredge),
  m_BinWidth((a_upperedge-a_loweredge)/(float)a_numberofbins), 
  m_LowerLimit(a_lowerlimit), m_UpperLimit(a_upperlimit),m_NumberOfEntries(a_title){
  if (a_lowerlimit.size()!=a_upperlimit.size()&& a_lowerlimit.size()!=a_numberofbins) {
    std::cout<<"Inconsistent vector(s) of limits passed to IdentifiedProfileHistogram: "
	     << a_title << " -- limits ignored..."  <<std::endl;
    m_LowerLimit.clear();
    m_UpperLimit.clear();
    std::fill_n(std::back_inserter(m_LowerLimit), m_NumberOfBins, 0.0);
    std::fill_n(std::back_inserter(m_UpperLimit), m_NumberOfBins, 0.0);
  }
}

template <typename SomeIDTag>
const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::abcissa() const{
  std::vector<float> result(m_NumberOfBins, m_BinWidth);
  result[0]=m_LowerEdge+m_BinWidth/2.0;
  for (int i=1;i<m_NumberOfBins;i++)
    result[i]=result[i-1]+m_BinWidth;
  return result;
}

template <typename SomeIDTag>
const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::average(const SomeIDTag& ident) const{
  std::vector<float> result(m_NumberOfBins, 0.0);
  if (!m_NumberOfEntries.existID(ident))
//    return result();
    return result;
  for (int i=0;i<m_NumberOfBins;i++)
    result[i]= m_NumberOfEntries[ident][i]>0 ? m_Content[ident][i]/(float)m_NumberOfEntries[ident][i] : 0.0;
  return result;
}

template <typename SomeIDTag>
const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::rms(const SomeIDTag& ident) const{
  std::vector<float> result(m_NumberOfBins, 0.0);
  if (!m_NumberOfEntries.existID(ident))
//    return result();
    return result;
  for (int i=0;i<m_NumberOfBins;i++)
    result[i]= m_NumberOfEntries[ident][i]>0 ? sqrt((m_ContentSquared[ident][i]-m_Content[ident][i]*m_Content[ident][i]/(float)m_NumberOfEntries[ident][i])/(float)m_NumberOfEntries[ident][i]) : 0.0;
  return result;
}

template <typename SomeIDTag>
const std::vector<float>& IdentifiedProfileHistogram<SomeIDTag>::averageError(const SomeIDTag& ident) const{
  std::vector<float> result(m_NumberOfBins, 0.0);
  if (!m_NumberOfEntries.existID(ident))
//    return result();
    return result;
  for (int i=0;i<m_NumberOfBins;i++)
   result[i]= m_NumberOfEntries[ident][i]>0 ? sqrt(m_ContentSquared[ident][i]-m_Content[ident][i]*m_Content[ident][i]/(float)m_NumberOfEntries[ident][i])/(float)m_NumberOfEntries[ident][i] : 0.0;
  return result;
}

template <typename SomeIDTag>
bool IdentifiedProfileHistogram<SomeIDTag>::fillEntry(const SomeIDTag& ident, const float& abcissa, const float& ordinate, const float& weight) {
  if (abcissa<m_LowerEdge||abcissa>m_LowerEdge+m_BinWidth*(float)m_NumberOfBins||m_NumberOfBins==0)
    return false;
  int bin=int((abcissa-m_LowerEdge)/m_BinWidth);
  if (m_LowerLimit[bin]<m_UpperLimit[bin]&&(ordinate<m_LowerLimit[bin]||ordinate>m_UpperLimit[bin]))
    return false;
  if (!m_NumberOfEntries.existID(ident)){
    m_NumberOfEntries.push_back(ident,std::vector<int>(m_NumberOfBins,0));
    m_Content.push_back(ident, std::vector<float>(m_NumberOfBins, 0.0));
    m_ContentSquared.push_back(ident, std::vector<float>(m_NumberOfBins, 0.0));
  }
  m_NumberOfEntries[ident][bin]++;
  m_Content[ident][bin]+=weight*ordinate;
  m_ContentSquared[ident][bin]+=weight*ordinate*ordinate;
  return true;
}

#endif // TRT_CALIBDATA__IDENTIFIEDPROFILEHISTOGRAM_ICC

#endif // TRT_CALIBDATA__IDENTIFIEDPROFILEHISTOGRAM_H

