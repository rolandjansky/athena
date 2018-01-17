/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackExtensionTool_DAF
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Concret implementation for base class ITRT_TrackExtensionTool
/////////////////////////////////////////////////////////////////////////////////
// Sebastian Fleischmann
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TRACKEXTENSIONTOOL_DAF_H
#define TRT_TRACKEXTENSIONTOOL_DAF_H

//#include <list>
#include <vector>
#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h" 
//#include "MagFieldInterfaces/IMagFieldSvc.h"
    
#include "TrkParameters/TrackParameters.h"  // typedef
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"  // typedef

#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"

class MsgStream;
class TRT_ID;

namespace MagField {
    class IMagFieldSvc;
}

namespace Trk {
    class Surface;
    //class MagneticFieldProperties;
    class IPropagator;
}

namespace InDet {
    class ITRT_DetElementsRoadMaker;
    class ICompetingTRT_DriftCirclesOnTrackCreator;

/**
@class TRT_TrackExtensionTool_DAF 
The TRT_TrackExtensionTool_DAF produces an extension with Trk::CompetingRIOsOnTrack of
silicon tracks into the TRT.
 
@author Sebastian.Fleischmann@cern.ch
*/

class TRT_TrackExtensionTool_DAF :

    virtual public ITRT_TrackExtensionTool, public AthAlgTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    TRT_TrackExtensionTool_DAF(const std::string&,const std::string&,const IInterface*);
    virtual ~TRT_TrackExtensionTool_DAF();
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    ///////////////////////////////////////////////////////////////////
    // Main methods for track extension to TRT
    ///////////////////////////////////////////////////////////////////

    virtual std::vector<const Trk::MeasurementBase*>& extendTrack(const Trk::Track&);
    virtual std::vector<const Trk::MeasurementBase*>& extendTrack(const Trk::TrackParameters&);
    virtual Trk::Track* newTrack(const Trk::Track&);
    virtual void newEvent();
    ///////////////////////////////////////////////////////////////////
    // TRT seed extension to TRT  
    ///////////////////////////////////////////////////////////////////

    virtual Trk::TrackSegment* findSegment(const Trk::TrackParameters&);

    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    MsgStream&    dump(MsgStream&    out) const;
    std::ostream& dump(std::ostream& out) const;

protected:

    ///////////////////////////////////////////////////////////////////
    // Protected Data
    ///////////////////////////////////////////////////////////////////

    const TRT_DriftCircleContainer*  m_trtcontainer; //!< container of TRT RIOs
    std::string                      m_jo_trtcontainername; //!< jobOption: name of container with TRT RIOs
    double                           m_jo_roadwidth; //!< jobOption: Max width of the road
    bool                             m_jo_simpleExtension; //!< jobOption: do the simple TRT extension by putting all RIOs of one detector element within the road into one Trk::CompetingRIOsOnTrack
    
    double                           m_jo_maxGroupDistance; //!< jobOption: Max distance of the RIO groups in the grouped barrel extension (distance in the x-y-plane)
    double                           m_jo_minGroupDistance; //!< jobOption: Min distance of the RIO groups in the grouped barrel extension (distance in the x-y-plane)

//    static const int                    maxTrackGlobalPositions = 200; //!< array size for global track positions
//    static const int                    maxDetElements = 200; //!< array size for the detElements road
    std::vector <const InDetDD::TRT_BaseElement*>    m_detectorElements; //!< vector to store the detElements
    std::vector <const Trk::TrackParameters*>        m_propagatedTrackParameters; //!< vector to store the propagated track parameters (propagated to the related entry of m_detectorElements)
    
    const Trk::TrackParameters* m_siliconTrkParams; //!< track parameters at the outermost Silicon layer
    
    mutable std::vector<const Trk::MeasurementBase*>  m_measurement; //!< vector of MeasurementBase for the output


    ///////////////////////////////////////////
    // the different tools and their jobOptions
    ///////////////////////////////////////////
    ToolHandle< InDet::ICompetingTRT_DriftCirclesOnTrackCreator >   m_compROTcreator;       //!< the instance of the CompetingTRT_DriftCirclesOnTrackCreator tool
    double                                                          m_jo_annealingFactor;    //!< jobOption: The annealing factor used for Trk::CompetingRIOsOnTrack creation

    ToolHandle< InDet::ITRT_DetElementsRoadMaker >                  m_roadtool;             //!<  instance of the TRT road maker tool

    ToolHandle< Trk::IPropagator >                                  m_propagator;           //!<  the Propagator tool

    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle; 

    std::string                                                     m_fieldmode;          //!< jobOption: Magnetic field mode
    Trk::MagneticFieldProperties                                    m_fieldprop;            //!< Magnetic field properties

    const TRT_ID*                                                   m_trtID;                //!< the TRT ID helper

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////
    
    /** find an element-wise extension (ie. the RIOs in a CompROT belong to one detElement) */
    StatusCode elementWiseExtension(int, int) const;
    /** find a barrel extension with RIOs grouped along the globalPositions of the track */
    StatusCode groupedBarrelExtension(int, int) const;

    //    MsgStream&    dumpConditions(MsgStream   & out) const;
    //    MsgStream&    dumpEvent     (MsgStream   & out) const;
};

//MsgStream&    operator << (MsgStream&   ,const TRT_TrackExtensionTool_DAF&);
//std::ostream& operator << (std::ostream&,const TRT_TrackExtensionTool_DAF&);

} // end of name space

#endif // TRT_TRACKEXTENSIONTOOL_DAF_H

