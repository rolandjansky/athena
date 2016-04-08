/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNMENT_COLLECTIONPROCESSOR_IH
#define INDETALIGNMENT_COLLECTIONPROCESSOR_IH

//////////////////////////////////////////////////////////////////////////
// ================================================
// CollectionProcessor
// ================================================
//
// ICollectionProcessor.h
// Generic interface for alignment algtools which process a track
// collection and return another one (e.g. refits, with or without vertexing,
// track selection ...)
//
// Namespace InDetAlignment

#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

namespace Trk {
  class Track;
}

namespace InDetAlignment {

  //static const InterfaceID IID_InDetAlignment_ICollectionProcessor("ICollectionProcessor",1,0);

  class ICollectionProcessor: virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID();

    /** does necessary configuration to the tool */
    //virtual void configure() = 0;
    virtual StatusCode configure() = 0;

    /** main processing of track collection
        current policy: if a track cannot be processed (e.g. refit fails), a zero pointer is inserted in the track collection
        instead (thus, the length of the track collection stays always the same) */
    virtual const DataVector<Trk::Track>* processTrackCollection(const DataVector<Trk::Track>* trks) = 0;
    
  };
  
  inline const InterfaceID& ICollectionProcessor::interfaceID()
    {
      static InterfaceID s_myID("ICollectionProcessor", 1 , 0);
      return s_myID;
      //return IID_InDetAlignment_ICollectionProcessor;
    }

} //namespace

#endif // INDETALIGNMENT_COLLECTIONPROCESSOR_IH
