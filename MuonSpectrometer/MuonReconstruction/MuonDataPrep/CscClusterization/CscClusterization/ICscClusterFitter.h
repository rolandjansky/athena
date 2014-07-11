/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICscClusterFitter.h

#ifndef ICscClusterFitter_H
#define ICscClusterFitter_H

// David Adams and Woochun Park
// February 2007
//
// Interface for tool to fit a CSC cluster.
//
// Returns the coordinate in the measurement plane, i.e. in the
// wire plane in the direction transverse to the strips.
//
// Also optionally rerurns a map of intermediate values indexed by
// name. It is intended that this list be used to tune the tool
// and provide measures of data quality.

#include <vector>
#include <map>
#include "GaudiKernel/IAlgTool.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "CscClusterization/ICscStripFitter.h"

namespace Muon {
  class CscPrepData;
}


/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscClusterFitter("ICscClusterFitter", 1, 0);

class ICscClusterFitter : virtual public IAlgTool {

public:  // Typedefs.

  // Input list of strips.
  typedef std::vector<Muon::CscStripPrepData*> StripList;

  // List of strip fit results.
  typedef ICscStripFitter::Result StripFit;
  typedef std::vector<StripFit> StripFitList;

  // Time on each strip.
  typedef std::vector<double> ChargeList;

  // Additional data.
  typedef std::vector<std::string> DataNames;
  typedef std::map<std::string, double> DataMap;

public:  // Embedded class.

  // This is a description of a fitted cluster.
  // The status, strip, position and position error shouls always be filled.
  class Result {
  public:
    int fitStatus;           // Fit status: 0 for success
    Muon::CscClusterStatus clusterStatus;   // Cluster status
    Muon::CscTimeStatus timeStatus;   // peak strip status
    unsigned int strip;      // Index of the strip to identify this clustrer
    unsigned int fstrip;     // Index of the first strip to identify this clustrer (for splitCluster)
    unsigned int lstrip;     // Index of the last  strip to identify this clustrer (for splitCluster)
    double position;         // Positon.
    double dposition;        // Error in the position.
    double time;             // Time (ns).
    double dtime;            // Error in the time.
    double charge;           // Charge (e).
    double dcharge;          // Error in charge.
    double width;            // Full width (ns) of the peak, e.g. FWHM, 2sigma, ...
    double qpeak;            // Peak Charge
    double qleft;            // Peak Charge
    double qright;            // Peak Charge

    double time_beforeBPCorr;         // Time (ns) of the peak. without t0 correction
    double time_beforeT0Corr;         // Time (ns) of the peak. without t0 correction
    double charge_beforeBPCorr;         // Time (ns) of the peak. without t0 correction
    
    DataMap dataMap;         // Extra data
    Result(int stat= 0,
           Muon::CscClusterStatus cstat =Muon::CscStatusUndefined,
           Muon::CscTimeStatus tstat =Muon::CscTimeStatusUndefined
           )
      : fitStatus(stat), clusterStatus(cstat), timeStatus(tstat), strip(999), fstrip(999), lstrip(999),
        position(999.), dposition(0.0), time(999.), dtime(0.),
        charge(0.), dcharge(0.), width(0.),
        qpeak(0.), qleft(0.), qright(0.),
        time_beforeBPCorr(-99999.0),
        time_beforeT0Corr(-99999.0),
        charge_beforeBPCorr(0.0)
    { }
  };

public:  // Static methods.

  // Return the interface ID.
  //  static const InterfaceID& interfaceID();
  static const InterfaceID& interfaceID() {  return IID_ICscClusterFitter; }

public:  // Interface methods

  // Return the list of additional data names.
  // Implementation here returns an empty list.
  virtual const DataNames& dataNames() const;

  typedef std::vector<Result> Results;
  // Fit a cluster with unknown direction.
  virtual Results fit(const StripFitList& sfits) const =0;

  // Fit a cluster with specified track direction.
  virtual Results fit(const StripFitList& sfits, double dposdz) const =0;

  virtual double getCorrectedError(const Muon::CscPrepData* pclu, double slope=-999) const =0;

public:

  // Old interface for backward compatibility.
  // This is temporary--please use one of the inteface methods.
  // Fit a cluster.
  // Input:
  //   strips - List of strips in the cluster.
  //   sfits - Results from fits of these strips
  // Output:
  //   pos = Position of the fitted cluster.
  //   dpos = Error in the position.
  //   clustatus = Status indicating if and how the cluster is spoiled.
  //   istrip = Index of the strip that should be used to identify the cluster
  //   time = cluster time
  //   pdmap = Additional data for validation, etc. Null to not fill.
  // Returns 0 for success.
  // For nonzero return, pos and dpos need not be filled but spoil
  // should be assigned a meaningful value.
  /*  int fit(const StripList& strips, const StripFitList& sfits,
          double& pos, double& dpos, Muon::CscClusterStatus& clustatus,
          unsigned int& istrip, double& charge, double& time, DataMap* pdmap =0) const;
  */
};

#endif
