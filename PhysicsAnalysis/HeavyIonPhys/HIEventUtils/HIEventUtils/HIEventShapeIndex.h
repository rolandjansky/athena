/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPEINDEX_H
#define HIEVENTUTILS_HIEVENTSHAPEINDEX_H

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "HIEventUtils/HIEventDefs.h"
#include <map>
#include <cmath>
#include <TH2.h>


class HIEventShapeIndex
{

public:
  HIEventShapeIndex();
  ~HIEventShapeIndex();

  //initialize indexing given object or scheme
  //returns total number of bins

  unsigned int setBinning(const TH2* h2, bool asMask);
  unsigned int setBinning(const xAOD::HIEventShapeContainer* shape);
  unsigned int setBinning(HI::BinningScheme scheme);

  void initializeEventShapeContainer(xAOD::HIEventShapeContainer* shape_container, unsigned int num_harmonics) const;
  //can associate ptr to shape container w/ binning
  //not required

  //lookup functions
  unsigned int getIndex(float eta, int layer) const;
  unsigned int getIndexFromBin(unsigned int ebin, int layer) const;
  unsigned int getEtaBin(float eta, int layer) const;
  unsigned int getNumBins() const {return m_size;};
  xAOD::HIEventShape* getShape(float eta, int layer, xAOD::HIEventShapeContainer* shape_container) const;
  const xAOD::HIEventShape* getShape(float eta, int layer, const xAOD::HIEventShapeContainer* shape_container) const;
  //HIEventShapeIndex& operator=(const HIEventShapeIndex& in);
  std::string print() const;
private:
  struct range_index_t{
    float eta_min;
    float eta_max;
    unsigned int index;

    range_index_t(float emin, float emax, unsigned int ii) : eta_min(emin), eta_max(emax),index(ii) {};
    range_index_t()=default;
    bool operator() (float eta) const
    {
      if (eta > this->eta_min && eta < this->eta_max) return true;
      if(eta==this->eta_min) return true;
      return false;
    };
    bool operator< (const range_index_t& rhs)  const {return this->eta_min < rhs.eta_min;};
  };
  std::map<int,std::vector<range_index_t> > m_edges;
  //pointer to container
  //only used to check compatibility of bins, not used as accessor
  const xAOD::HIEventShapeContainer* m_shape_container;
  unsigned int m_size;

  unsigned int getIndex_Internal(float eta, int layer, bool etaIndex) const;
  std::map<int,std::vector<range_index_t> >::const_iterator getLayer(int layer) const;

  inline float roundToTenth(float d) const {return std::floor(d)+std::floor((d-std::floor(d))*10.0+0.5)/10.0;};



};

#endif
