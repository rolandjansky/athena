/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDTrackSegmentHelper               //
//                                                            //
//  Description: Class for keeping track of visible tracks    //
//               and segments and their materials, for        //
//               colour-prd-by-XXX modes.                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDTRACKSEGMENTHELPER_H
#define PRDTRACKSEGMENTHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <vector>
#include <map>

class SoMaterial;
class PRDHandleBase;
namespace Trk
{
  class Track;
  class Segment;
  class PrepRawData;
}

class PRDTrackSegmentHelper : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  PRDTrackSegmentHelper( std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >* prd2handle,
			 IVP1System* sys = 0, QObject * parent = 0);
  virtual ~PRDTrackSegmentHelper();

  //This is used to inform the class of changes in visible tracks and segments:
  void visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >&);
  void visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >&);

  SoMaterial * trackMaterial(const Trk::Track*) const;
  SoMaterial * segmentMaterial(const Trk::Segment*) const;
  class TracksAndSegments {
  public:
    inline TracksAndSegments(const std::vector<const Trk::Track*>&,const std::vector<const Trk::Track*>&,const std::vector< const Trk::Segment* >& );
    std::vector<const Trk::Track*> tracks;//tracks using the given prd in the fit
    std::vector<const Trk::Track*> tracks_outliers;//tracks where the given prd is associated as 'outlier'
    std::vector< const Trk::Segment* > segments;//segments using the given prd
  };
  const TracksAndSegments * tracksAndSegments(const Trk::PrepRawData*);//0 means no tracks/segments
private:

  class Imp;
  Imp * m_d;

};

#endif
