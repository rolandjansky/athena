/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetRecToolInterfaces_page InDetRecToolInterfaces
@author Markus.Elsing@cern.ch, Igor.Gavrilenko@cern.ch
@section InDetRecToolInterfaces_InDetRecToolInterfacesIntro Introduction

InDetRecToolInterfaces is a directory which contains set virtual abstract classes which of them inherit
from base class IAlgTool and describe methodes and intefaces needs for different step tracks and vertices reconstruction process.

@section InDetRecToolInterfaces_InDetRecToolInterfacesISiSpacePointsSeedMaker ISiSpacePointsSeedMaker

 This tool should provide search all space points seeds inside full Atlas inner detector
 or only inside some region. Each of this seed is a list space points with different lenght and
 is input for detailed track-finding procedure.    
 
 This tool contains two methods to initialize seed finding process.

   virtial void newEvent ()=0;
   virtual void newRegion
     (const std::vector<IdentifierHash>& pixels ,const std::vector<IdentifierHash>& scts)=0;
      
 This tool contains three methods for finding space points seed with different lenght, with
 two, three or variable number space points. Input information is list estimated position of primary vertices.
 If this list is empty tool should provide search space points without vertices information.
 

   virtual void find2Sp (const std::list<Trk::Vertex>&)=0;
   virtual void find3Sp (const std::list<Trk::Vertex>&)=0;
   virtual void findVSp (const std::list<Trk::Vertex>&)=0;

 This tool contains iterator through seeds collection produced accordingly method find.      

   virtual const SiSpacePointsSeed* next()=0;

@section InDetRecToolInterfaces_InDetRecToolInterfacesISiZvertexMaker ISiZvertexMaker

 This tool should provide fast estimation position all primary vertices along
 beam direction. 

 This tool contains two methods to initialize primary vertices finding process.

    virtual void newEvent()=0;
    virtual void newRegion
      (const std::vector<IdentifierHash>& pixels,const std::vector<IdentifierHash>& scts)=0;

 and one method for vertices finding
 
      virtual const std::list<Trk::Vertex>&  getVertices ()=0;

@section InDetRecToolInterfaces_InDetRecToolInterfacesITRT_TrackExtensionTool ITRT_TrackExtensionTool

 This tool should find best track prolongation in TRT and  contains one method to initiate
 track extention process 

    virtual void newEvent()=0;

 and contains two methods for track extention with diferent input information. 
 Input for prolongation can be Trk::Track or Trk::TrackParameters.
 Output both nethods is vector Trk::MeasurementBase objects.

    virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::Track&) = 0;
    virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::TrackParameters&) = 0;


@section InDetRecToolInterfaces_InDetRecToolInterfacesITRT_DetElementsRoadMake ITRT_DetElementsRoadMake

 This tool should find all TRT detector elements on the trajectory sorted in
 propagation order and contains two method with slightly different output information.
 Output can be simple list pointers to TRT detector elements or list pair
 with pointer to TRT detector element together with track parameters of this tarjectory
 on detector element surface. Input for both method is inital track parameters and
 information about direction.

    virtual void detElementsRoad
      (const Trk::TrackParameters&,Trk::PropDirection, 
       std::list<const InDetDD::TRT_BaseElement*>&)=0;

    virtual void detElementsRoad
      (const Trk::TrackParameters&,Trk::PropDirection, 
       std::list<std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> >&)=0;


@section InDetRecToolInterfaces_InDetRecToolInterfacesISiDetElementsRoadMaker ISiDetElementsRoadMaker

 This tool should find all SCT and Pixels detector elements on the trajectory sorted in
 propagation order and contains three methods with different input information.
 Input for tool can be simple list space points or list global positions or track parameters.
 Output for for all methods is list pointers to SCT and Pixels detetctor elements.

   virtual void detElementsRoad
      (const std::list<const Trk::SpacePoint*>&,
      std::list<const InDetDD::SiDetectorElement*>&)=0;

    virtual void detElementsRoad
      (std::list<Trk::GlobalPosition>&,
       std::list<const InDetDD::SiDetectorElement*>&)=0;
    
    virtual void detElementsRoad
      (const Trk::TrackParameters&,Trk::PropDirection, 
       std::list<const InDetDD::SiDetectorElement*>&)=0;
 

@section InDetRecToolInterfaces_InDetRecToolInterfacesITRT_TrackSegmentsMaker ITRT_TrackSegmentsMaker

 This tool should provide search all TRT track segments inside full Atlas inner detector
 or only inside some region without extra information (Stande alone TRT reconstruction). 
 Each of this track segment is a vector TRT RIOonTrack will use as input for detailed track-finding procedure.    

 This tool contains two methods to initialize track segment finding process

    virtual void newEvent ()=0;
    virtual void newRegion(const std::vector<IdentifierHash>& trt)=0;
 
 contains one method for track segments finding
     
    virtual void find()=0;

 and provide iterator trough track sements collection

    virtual Trk::TrackSegment* next()=0;


@section InDetRecToolInterfaces_InDetRecToolInterfacesISiTrackMaker ISiTrackMaker

 This tool initiate detailed track-finding procedure inside SCT and Pixel detector
 elements and contains one method to initialize track finding process
 
    virtual void newEvent(bool pixel,bool sct)=0;
 
 and one method for track finding. 
 
    virtual const std::list<Trk::Track*>& 
      getTracks(const std::list<const Trk::SpacePoint*>&)=0;

 Input for this method is list space points and output is list pointers to track.


@section InDetRecToolInterfaces_InDetRecToolInterfacesITRT_SeededSpacePointFinder ITRT_SeededSpacePointFinder

 This tool searches for space point seeds in the last two SCT layers close to the TRT.

 It contains one intialization method to load the SCT space points

    virtual void newEvent()=0;

 and one method for space point seed search

    virtual std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*>*> find2Sp (const Trk::TrackParameters& tP)=0;

 that returns a list of space point pairs for the specific track parameters 
 extracted from the TRT track segment used as an input. If only one space 
 point is found then it is included in the pair twice.

 The iterator method over the next event for seed searching

    virtual const SiSpacePointsSeed* next()=0;

 is not implemented.

@section InDetRecToolInterfaces_InDetRecToolInterfacesITRT_SeededTrackFinder ITRT_SeededTrackFinder

 This tool initiates the track-finding procedure inside the SCT and Pixel detector 
 elements using as input the track information provided by track segments reconstructed
 inside the TRT. This tool contains one method to initialize the track finding process

    virtual void newEvent()=0;

 and three methods for track finding.

    virtual const Trk::Track* getTrack(const Trk::Track&) = 0;
    virtual const Trk::Track* getTrack(const Trk::TrackParameters&) = 0;
    virtual const Trk::Track* getTrack(const Trk::TrackSegment&) = 0;
 
 Input for this tool can be Track,TrackSegment or simple track parameters and output
 is pointer to track. Some care is needed when the track parameters are
 passed directly, since they are going to be used by the seed finding tool 
 to determine the area of search inside the TRT. Passing a TRT track segment
 as input is the prefered method, most compatible with the new tracking requirements.


*/
