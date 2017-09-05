#ifndef TRUTHWEIGHTTOOLS_INDEXRECEIVER_H
#define TRUTHWEIGHTTOOLS_INDEXRECEIVER_H

#include "xAODTruth/TruthMetaData.h"
#include <stdexcept>

class IIndexRetriever
{

public:
  IIndexRetriever(): m_isValid(false) {};
  virtual ~IIndexRetriever() {};
  virtual void update(xAOD::TruthMetaData const *const) = 0;
  virtual void update(const std::vector<std::string> &weightNamesVec) = 0;

  virtual size_t getIndex() = 0;
  //virtual size_t getIndex() {
  //  if (!m_isValid) throw std::runtime_error("Weight name not found in event");
  //  return m_currentIndex;
  //}
  virtual bool isValid() { return m_isValid;}
protected:
  bool m_isValid;
  size_t m_currentIndex;
};

class IndexRetriever : public IIndexRetriever
{
public:
  IndexRetriever(std::string);
  virtual void update(xAOD::TruthMetaData const *const);
  virtual void update(const std::vector<std::string> &weightNamesVec);
  virtual size_t getIndex()
  {
    if (!m_isValid) { throw std::runtime_error("Weight \"" + m_WeightName + "\" not found in event"); }

    return m_currentIndex;
  }
protected:
  IndexRetriever();
  std::string m_WeightName;
};
#endif
