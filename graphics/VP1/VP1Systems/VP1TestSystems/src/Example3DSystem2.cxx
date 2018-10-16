/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class Example3DSystem2                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TestSystems/Example3DSystem2.h"

#include "StoreGate/StoreGate.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

//_____________________________________________________________________________________
Example3DSystem2::Example3DSystem2()
  : IVP13DSystemSimple("Ex3DSys2",
		       "This is an illustration of a very basic 3D system.\n"
		       "It simply transforms track information found in storegate"
		       " into relevant 3D objects (SoLineSet's).",
		       "Thomas.Kittelmann@cern.ch")
{
}

//_____________________________________________________________________________________
void Example3DSystem2::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  //1) Try to get the collection of (InDet) tracks from storegate:

  //Sanity check:
  if (!sg) {
    message("Error: Got null storegate pointer");
    return;
  }

  const TrackCollection *trackColl;
  std::string trackname="Tracks";
  StatusCode status = sg->retrieve(trackColl, trackname);
  if (status != StatusCode::SUCCESS || !trackColl) {
    message("Error: Could not retrieve track collection (used key="+QString(trackname.c_str())+")");
    return;
  }

  //2) Loop over the tracks and construct a relevant 3D object for each of them (a SoLineSet):

  TrackCollection::const_iterator trackItr, trackItrEnd = trackColl->end();

  for ( trackItr = trackColl->begin() ; trackItr != trackItrEnd; ++trackItr) {
      const Trk::Track *track = (*trackItr);
      const DataVector<const Trk::TrackParameters> *params = track->trackParameters();

      //Just a sanity check:
      if ( !params )
	continue;

      //The list of points on this track should be set in a so-called
      //SoVertexProperty (which one realises by reading the
      //documentation for SoLineSet at http://doc.coin3d.org/Coin/):

      SoVertexProperty *vertices = new SoVertexProperty();

      int iver(0);
      DataVector<const Trk::TrackParameters>::const_iterator it, itE = params->end();
      for (it = params->begin();it!=itE;++it) {
	vertices->vertex.set1Value(iver++,(*it)->position().x(),(*it)->position().y(),(*it)->position().z());
      }

      //Create a set of lines from these vertices:
      SoLineSet * line = new SoLineSet();
      line->numVertices = iver;
      line->vertexProperty = vertices;

      //Add to the tree:
      root->addChild(line);

      //To avoid GUI freeze-ups:
      updateGUI();

  }

}
