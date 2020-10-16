/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPACPOINTPROVIDER_H 
#define ISPACPOINTPROVIDER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

class TrigSiSpacePoint;
class IRoiDescriptor;

typedef std::vector< TrigSiSpacePoint* >   spacePointVec;

static const InterfaceID IID_ISpacePointProvider("ISpacePointProvider", 1 , 0); 


class ISpacePointProvider: virtual public IAlgTool
{
 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_ISpacePointProvider;
  }

  virtual spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true, std::map<Identifier, std::vector<long int> >  *clustermap=nullptr) = 0;

  virtual StatusCode fillCollections(const IRoiDescriptor&, bool,bool,
				     std::vector<int>&, std::vector<int>&) = 0;
  virtual StatusCode fillCollections(bool,bool,
				     std::vector<int>&, std::vector<int>&) = 0;

  virtual const std::vector<int>* fillPixelDataErrors() = 0;
  virtual const std::vector<int>* fillSCT_DataErrors() = 0;

  virtual double getBStoClustersTimingPixel()  const= 0;
  virtual double getBStoClustersTimingSCT() const = 0;
  virtual double getSPformationTimingPixel() const = 0;
  virtual double getSPformationTimingSCT() const = 0;
  virtual double getRegSelTiming() const = 0;
  virtual double getTotalTiming() const = 0;
};

#endif // ISPACPOINTPROVIDER_H
