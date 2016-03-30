/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTCNVTOOL_H
#define INDETEVENTCNVTOOL_H

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <utility>

class AtlasDetectorID;
class Identifier;
class IdentifierHash;
class IdDictManager;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
  class TRT_DetectorManager;
}

namespace Trk {
  class TrkDetElementBase;
  class PrepRawData;
}

namespace InDet {
/**Helper tool uses to convert InDet objects in generic tracking custom convertor TrkEventAthenaPool.

See "mainpage" for discussion of jobOpts.
*/
class InDetEventCnvTool :  virtual public Trk::ITrkEventCnvTool, public AthAlgTool   {
  public:
  
  enum InDetConcreteType { SCT, Pixel, TRT, Unknown };

  InDetEventCnvTool(const std::string&,const std::string&,const IInterface*);
  
  virtual ~InDetEventCnvTool ();
  
  virtual StatusCode initialize();
  
  virtual void checkRoT( const Trk::RIO_OnTrack& rioOnTrack );
  
  /** use the passed identifier to recreate the detector element and PRD links on the passed RIO_OnTrack
  @param[in] rioOnTrack The RIO_OnTrack we're interested in
  @return  std::pair of the pointers to the two corresponding objects*/
  virtual std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
      getLinks( const Trk::RIO_OnTrack& rioOnTrack    );
      
  /** @copydoc Trk::ITrkEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack* rot)*/    
  virtual void prepareRIO_OnTrack( Trk::RIO_OnTrack* rot);
  
  /** @copydoc Trk::ITrkEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack* rot)*/
  virtual void recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT );
  
  /** Return the detectorElement associated with this Identifier*/
  virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id, const IdentifierHash& idHash);

  /** Return the detectorElement associated with this Identifier*/
  virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id);

  
  private:

  /** use the passed identifier to recreate the pixel cluster link on the passed RIO_OnTrack*/
  virtual Trk::PrepRawData* pixelClusterLink( const Identifier& id,  const IdentifierHash& idHash);
  
  /** use the passed identifier to recreate the SCT cluster link on the passed RIO_OnTrack*/
  virtual Trk::PrepRawData* sctClusterLink( const Identifier& id,  const IdentifierHash& idHash  );
  
  /** use the passed identifier to recreate the TRT Drift circle link on the passed RIO_OnTrack*/
  virtual Trk::PrepRawData* trtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash );
  
  std::string  m_pixMgrLocation;                    //!< Location of sct Manager
  const InDetDD::PixelDetectorManager*  m_pixMgr;   //!< SCT   Detector Manager
  std::string  m_sctMgrLocation;                    //!< Location of sct Manager
  const InDetDD::SCT_DetectorManager*   m_sctMgr;   //!< SCT   Detector Manager
  std::string  m_trtMgrLocation;                    //!< Location of sct Manager
  const InDetDD::TRT_DetectorManager*   m_trtMgr;   //!< TRT   Detector Manager
  bool m_setPrepRawDataLink;                        //!< if true, attempt to recreate link to PRD


//various id helpers
  const AtlasDetectorID     * m_IDHelper; 

  
  // added to check TRT existence (SLHC geo check) 
  const IdDictManager * m_idDictMgr;

  std::string m_pixClusContName;                    //!< location of container of pixel clusters
  std::string m_sctClusContName;                    //!< location of container of sct clusters
  std::string m_trtDriftCircleContName;             //!< location of container of TRT drift circles
};


}
#endif // MOORETOTRACKTOOL_H

