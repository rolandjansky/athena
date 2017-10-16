/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BeamPipeDetectorFactory_h
#define BeamPipeDetectorFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>

class StoreGateSvc;
class AbsMaterialManager;
class GeoShape;

class BeamPipeDetectorFactory : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  BeamPipeDetectorFactory(StoreGateSvc  *pDetStore,
			  IRDBAccessSvc *pAccess);
  
  // Destructor:
  ~BeamPipeDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const BeamPipeDetectorManager * getDetectorManager() const;
  
  // Set version Tag and Node
  void setTagNode(std::string tag, std::string node);

  void addSections(GeoPhysVol* parent, int region);

 private:  

  // small helper class
  class EnvelopeEntry {
  public:
    EnvelopeEntry() :  m_z(0), m_r(0) {}
    EnvelopeEntry(double z, double r) : m_z(z), m_r(r) {}
    double z() const {return m_z;}
    double r() const {return m_r;}
  private:
    double m_z;
    double m_r;
  };

  class EnvelopeShapes {
  public:
    EnvelopeShapes() : centralShape(0), fwdShape(0) {} 
    GeoShape * centralShape;
    GeoShape * fwdShape;
  };


  EnvelopeShapes makeEnvelope(IRDBRecordset_ptr bpipeEnvelope);
  EnvelopeShapes makeEnvelopeOld(IRDBRecordset_ptr atlasMother);


  // Illegal operations:
  const BeamPipeDetectorFactory & operator=(const BeamPipeDetectorFactory &right);
  BeamPipeDetectorFactory(const BeamPipeDetectorFactory &right);
  
  // The manager:
  BeamPipeDetectorManager     * m_detectorManager;
   
  const AbsMaterialManager * m_materialManager;

  StoreGateSvc             * m_detectorStore;
  IRDBAccessSvc            * m_access;
  std::string              m_versionTag;
  std::string              m_versionNode;

  double m_centralRegionZMax;
};

// Class BeamPipeDetectorFactory 
#endif


