/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_DECODEVERSIONKEY_H
#define GEOMODELSVC_DECODEVERSIONKEY_H

#include <string>
class IGeoModelSvc;
class IGeoDbTagSvc;

/// This is a helper class to query the version tags from GeoModelSvc and
/// determine the appropriate tag and node to pass to RDBAccessServer.
/// If a subsystem has an override tag then that tag is used otherwise the 
/// ATLAS tag is used.

class DecodeVersionKey
{
  
 public:
  /// Constructor is passed a pointer to GeoModelSvc plus the node
  /// for which you want the tag. 
  /// Possible nodes are ATLAS, InnerDetector, Pixel, SCT, TRT, LAr, TileCal, or MuonSpectrometer.
  DecodeVersionKey(const IGeoModelSvc *, const std::string & node);
  // Overloading the constructor to allow adiabatic migration of clients away from the IGeoModelSvc
  DecodeVersionKey(const IGeoDbTagSvc *, const std::string & node);
  DecodeVersionKey(const std::string & node);
  
  /// Return version tag 
  const std::string & tag() const;

  /// Return the version node.
  const std::string & node() const;

  /// Return true if CUSTOM is selected.
  bool custom() const;

 private:

  template <class T>
    void defineTag(const T* svc, const std::string & node);

  // utilily for dealing with CUSTOM tags.
  bool getCustomTag(const std::string & inputTag, std::string & outputTag);

  std::string m_tag;
  std::string m_node;
  bool m_custom;

};

#endif // GEOMODELSVC_DECODEVERSIONKEY_H
