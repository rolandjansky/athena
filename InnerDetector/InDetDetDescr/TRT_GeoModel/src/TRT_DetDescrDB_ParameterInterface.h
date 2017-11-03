/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_GEOMODEL_TRT_DETDESCRDB_PARAMETERINTERFACE_H
#define TRT_GEOMODEL_TRT_DETDESCRDB_PARAMETERINTERFACE_H


#include "TRTParameterInterface.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class IRDBRecord;
class TopLevelPlacements;


#include <map>

class TRT_DetDescrDB_ParameterInterface : public TRTParameterInterface {

public:

   
  // Only allowed constructor
  TRT_DetDescrDB_ParameterInterface(const InDetDD::AthenaComps * athenaComps);

  ~TRT_DetDescrDB_ParameterInterface();

  virtual const HepGeom::Transform3D & partTransform(const std::string & partName) const;
  virtual bool partPresent(const std::string & partName) const;
  virtual const InDetDD::DistortedMaterialManager * distortedMatManager() const;

  const std::string& name() const { 
    static const std::string n("TRT_GeoModel::TRT_DetDescrDB_ParameterInterface"); 
    return n;
  }

  IRDBRecordset_ptr  scalingTable() const {return m_scalingTable;}

  void SetValues();

  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  
private:

  const InDetDD::AthenaComps * m_athenaComps;

  const InDetDD::DistortedMaterialManager * m_distortedMatManager;
  TopLevelPlacements * m_placements;
  IRDBRecordset_ptr m_scalingTable;
};

#endif
