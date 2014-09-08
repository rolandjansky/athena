/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParametersCnv_p2.h
// author: edward.moyse@cern.ch
//
//-----------------------------------------------------------------------------

#ifndef TRACK_PARAMETERS_CNV_P2_H
#define TRACK_PARAMETERS_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p2.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h" 
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;

/** New convertor which handles all track parameters in one go - i.e. it replaces all the rest of the classes in the TrkParameters directory.
It does not (currently) write out bounds.*/
class TrackParametersCnv_p2
  : public T_AthenaPoolTPAbstractPolyCnvBase<Trk::TrackParameters, Trk::TrackParameters, Trk::TrackParameters_p2>
  // : public T_AthenaPoolTPCnvBase<Trk::ParametersBase,Trk::TrackParameters_p2>
{
public:
  TrackParametersCnv_p2(bool nosurf = false)
    : m_emConverter(0),
      m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"),
      m_nosurf (nosurf)
  {}

  void persToTrans( const Trk :: TrackParameters_p2 *persObj,
    Trk :: TrackParameters    *transObj,
    MsgStream &log );

  Trk::TrackParameters* createTransient( const Trk::TrackParameters_p2 * persObj, MsgStream& log);
  
  void transToPers( const Trk::TrackParameters    *transObj,
    Trk::TrackParameters_p2 *persObj,
    MsgStream &log );

   virtual  Trk::TrackParameters* virt_createTransFromPStore( unsigned index, MsgStream& log) {
   //FIXME - I'm really not sure if this is correct to have this here. Perhaps I just need to change the T_AthenaXXX baseclass that this cnv inherits from?
   // The method I wanted to call is in  T_AthenaPoolTPPolyCnvBase
   // But T_AthenaPoolTPPolyCnvBase doesn't work because T_AthenaPoolTPConverter.icc:63 tries to instantiate a Trk::ParametersBase, which isn't possible.
    assert (index < this->m_pStorage->size());
    return createTransient( &(*this->m_pStorage)[index], log );
  }
  
  
private:
  AmgSymMatrix(5)*    transErrorMatrix(const Trk :: TrackParameters_p2 *persObj,  MsgStream& log);
  const Trk::Surface* transSurface(    const Trk :: TrackParameters_p2 *persObj,  Trk::Surface::SurfaceType type,     MsgStream& log);
  void                fillPersSurface( const Trk :: TrackParameters    *transObj, Trk :: TrackParameters_p2 *persObj, MsgStream& log);
  void                convertTransCurvilinearToPers(const Trk :: TrackParameters    *transObj, Trk :: TrackParameters_p2 *persObj) const;
  bool                isPersistifiableType(const Trk :: TrackParameters    *transObj) const;
  
  ErrorMatrixCnv_p1 *                        m_emConverter;
  ToolHandle<Trk::IEventCnvSuperTool>        m_eventCnvTool;  
  bool                                       m_nosurf;
};

#endif // TRACK_PARAMETERS_CNV_P2_H
