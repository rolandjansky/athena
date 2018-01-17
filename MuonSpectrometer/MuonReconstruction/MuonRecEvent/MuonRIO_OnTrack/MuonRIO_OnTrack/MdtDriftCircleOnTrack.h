/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtDriftCircleOnTrack.h
//   Header file for class MdtDriftCircleOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// /** Class to implement DriftCircle On Track for MDT */
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004 Ketevi A. Assamagan
// adapted from Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef MUONRIO_ONTRACK_MDTDRIFTCIRCLEONTRACK_H
#define MUONRIO_ONTRACK_MDTDRIFTCIRCLEONTRACK_H

// Base classes
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonPrepRawData/MdtPrepData.h"

#include "TrkSurfaces/Surface.h"

#include "TrkEventPrimitives/DriftCircleSide.h"
#include "TrkEventPrimitives/DriftCircleStatus.h"
#include <cassert>

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "AthLinks/ElementLink.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"

typedef ElementLink<Muon::MdtPrepDataContainer> ElementLinkToIDC_MDT_Container;

namespace Trk
{
    class LocalParameters;
    class StraightLineSurface;
}

 namespace Trk 
{
    class ITrkEventCnvTool;
}

namespace Muon
{
    class MuonEventCnvTool;
    class MdtDriftCircleOnTrackCreator;

/** @brief This class represents the corrected MDT measurements, 
    where the corrections include the effects of wire sag etc.*/
class MdtDriftCircleOnTrack :   public Trk::RIO_OnTrack
{
public:    
                        
    friend class  Trk::ITrkEventCnvTool;
    friend class Muon::MuonEventCnvTool;
    friend class Muon::MdtDriftCircleOnTrackCreator;

    /** @brief Default ctor - for use by POOL only. Do not use yourself!*/
    MdtDriftCircleOnTrack();

    MdtDriftCircleOnTrack(const MdtDriftCircleOnTrack &);

    MdtDriftCircleOnTrack &operator=(const MdtDriftCircleOnTrack &);
    MdtDriftCircleOnTrack &operator=(MdtDriftCircleOnTrack &&);



    /** @brief Constructor for both non-sagged and sagged wire (default is non-sagged, since saggedSurface=0).
        Using the constructor implies the sign solution of the MDT is resolved.

        The identifier hash, det element pointer etc are taken from RIO (the MdtPrepData pointer) 
        so it is <b>vital</b> that this pointer is not zero.
        
        @param [in] RIO                  pointer to parent MdtPrepData used to create this RIO_OnTrack
        @param [in] locPos               local position (i.e. drift radius) of the measurement. 
                                         This drift radius includes all corrections 
        @param [in] errDriftRadius       the Amg::MatrixX (should be 1d, and contains the error on the drift radius measurement)
        @param [in] driftTime            drift time used to obtain the drift radius
        @param [in] status               status of drift circle. See Trk::DriftCircleStatus for definitions.
        @param [in] globDir              the predicted/extrapolated track direction (used to calculate global position)
        @param [in] positionAlongWire    this is the extrapolated position along the wire. i.e. it is NOT a measurement, and so should not
                                         be included in the Amg::MatrixX
        @param [in] saggedSurface        pointer to a Trk::StraightLineSurface created at the sagged position of the wire. It is not mandatory
                                         (i.e. if nothing is passed, or a zero pointer then the non-sagged wire is used). If something is passed, 
                                         this object will own it (i.e. it will be deleted by this object)
        @param [in] creationParameters   A bitword containing information about the construction of the ROT. 
                                         See m_rotCreationParameters for details.
        */
    MdtDriftCircleOnTrack( const MdtPrepData*                  RIO,
                           const Trk::LocalParameters&         locPos,
                           const Amg::MatrixX&                 errDriftRadius,
                           const double                        driftTime,
                           const Trk::DriftCircleStatus        status,  
                           const Amg::Vector3D&                globDir,
                           const double                        positionAlongWire,
                           const MuonDriftCircleErrorStrategy& errorStrategy,
                           const Trk::StraightLineSurface*     saggedSurface=0
                           );

    /** @brief Constructor without global direction for both non-sagged and sagged wire (default is non-sagged, since saggedSurface=0).
        This necessarily implies that the DriftCircleStatus is UNDECIDED, since without the GlobalDirection it cannot be worked out.
        In order to have a fully defined MdtDriftCircleOnTrack it is necessary to use the complete constructor (above)
        
        The identifier hash, det element pointer etc are taken from RIO (the MdtPrepData pointer) 
        so it is vital that this pointer is not zero.
        
        @param [in] RIO                  pointer to parent MdtPrepData used to create this RIO_OnTrack
        @param [in] locPos               local position (i.e. drift radius) of the measurement. This drift radius includes
        all corrections 
        @param [in] errDriftRadius       the Amg::MatrixX (should be 1d, and contains the error on the drift radius measurement)
        @param [in] driftTime            drift time used to obtain the drift radius
        @param [in] status               status of drift circle. See Trk::DriftCircleStatus for definitions.
        @param [in] positionAlongWire    this is the extrapolated position along the wire. i.e. it is NOT a measurement, and so should not
                                         be included in the Amg::MatrixX
        @param [in] saggedSurface        pointer to a Trk::StraightLineSurface created at the sagged position of the wire. It is not mandatory
                                         (i.e. if nothing is passed, or a zero pointer then the non-sagged wire is used). If something is passed, 
                                         this object will own it (i.e. it will be deleted by this object)
        @param [in] creationParameters   A bitword containing information about the construction of the ROT. 
                                         See m_rotCreationParameters for details.
        */
    MdtDriftCircleOnTrack( const MdtPrepData*                  RIO,
                           const Trk::LocalParameters&         locPos,
                           const Amg::MatrixX&                 errDriftRadius,
                           const double                        driftTime,
                           const Trk::DriftCircleStatus        status,
                           const double                        positionAlongWire,
                           const MuonDriftCircleErrorStrategy& errorStrategy,
                           const Trk::StraightLineSurface*     saggedSurface=0
                           );


    // Alternate constructor that doesn't dereference the RIO link.
    MdtDriftCircleOnTrack( 
        const ElementLinkToIDC_MDT_Container& RIO, 
        const Trk::LocalParameters&     locPos, 
        const Amg::MatrixX&             errDriftRadius, 
        const Identifier&               id,
        const MuonGM::MdtReadoutElement* detEl,
        const double                    driftTime,
        const Trk::DriftCircleStatus    status,  
        double                          positionAlongWire, 
        float                           localAngle,
        const MuonDriftCircleErrorStrategy& errorStrategy,
        const Trk::StraightLineSurface* surface
                           );

    /** @brief Destructor: */
    virtual ~MdtDriftCircleOnTrack();

    /** @brief Returns the side on which the drift radius is wrt to the track */
    Trk::DriftCircleSide side() const;

     /** @brief Returns the status of the  drift radius calibration. 
        (for more information see the definition of TrkEventPrimitives/DriftCircleStatus)
         - extends Trk::RIO_OnTrack interface*/
    Trk::DriftCircleStatus status() const;

    /** @copydoc Trk::RIO_OnTrack::clone()  */
    virtual MdtDriftCircleOnTrack* clone() const ;

    /** @brief Returns the PrepRawData used to create this corrected measurement */
    virtual const MdtPrepData* prepRawData() const;
    const ElementLinkToIDC_MDT_Container& prepRawDataLink() const;

    /** @brief Returns the hashID of the PRD collection */
    virtual IdentifierHash collectionHash() const;

    /** @brief Returns an invalid hash @todo Remove*/
    virtual IdentifierHash idDE() const { return IdentifierHash(); } 

    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::MdtReadoutElement* detectorElement() const;

    /** @brief Returns the surface on which this measurement was taken. 
        - If hasSaggedSurface()==false, then the surface will be that of the matching Detector Element
        - If hasSaggedSurface()==true, then the surface will be a special surface, representing the sagged position 
        of the wire at the coords of this measurement.*/
    virtual const Trk::Surface& associatedSurface() const;

    /** @brief Returns the global Position. 
    Be aware that this is calculated from the predicted position along 
    the tube, and the drift radius. i.e. it is partly inferred from other data, 
    and so is not a 'true' measurement.*/
    virtual const Amg::Vector3D& globalPosition() const;

    /** @brief Returns a boolean indicating if this object was created with a sagged surface (i.e. if sagging corrections have been done.)
       - If true, it means that this surface was created with a sagged surface, i.e. it has been corrected for wire sag. 
       - If false, then the associatedSurface() method returns the suface of the detector element (i.e. the nominal surface)*/
    bool hasSaggedSurface() const;
    
    /** @brief Returns the value of the drift radius.
    Obviously to use this method you need to cast to a MdtDriftCircleOnTrack if you have a pointer or reference 
    to the base class (Trk::RIO_OnTrack). An alternative is to use:
    @code localParameters().get(Trk::driftRadius) @endcode
    @warning This method assumes that the MdtDriftCircleOnTrack has been filled correctly. There are no checks!
    */
    double driftRadius() const;

    /** @brief Returns the value of the drift time used to obtain the drift radius.
    Obviously to use this method you need to cast to a MdtDriftCircleOnTrack if you have a pointer or reference 
    to the base class (Trk::RIO_OnTrack).
    */
    double driftTime() const;
    
    /** @brief Returns the position along the wire, as determined by the extrapolation used when creating this Trk::RIO_OnTrack.*/
    float positionAlongWire() const;

    /** @brief Returns the local angle, as determined by the extrapolation used when creating this Trk::RIO_OnTrack.*/
    float localAngle() const;
    
    /** @brief Get information about the creation strategy used by Muon::MdtDriftCircleOnTrackCreator when making this object.*/
    const MuonDriftCircleErrorStrategy& errorStrategy() const;
    
    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;
    
    // /////////////////////////////////////////////////////////////////
    // Private data:
    // /////////////////////////////////////////////////////////////////
private:
    /**@brief Sets the local parameters.
    @warning Only intended for use by the Muon::MdtDriftCircleOnTrackCreator*/
    virtual void setLocalParameters( const Trk::LocalParameters& locPos);
    
    /**@brief Sets the DetElement and Trk::PrepRawData pointers after reading from disk.
    @warning Only intended for use by persistency convertors*/
    virtual void setValues(const Trk::TrkDetElementBase*,
                           const Trk::PrepRawData*);
                           
    /** @brief Uses the passed loc3Dframe to calculate and set the global coord of this hit. 
    If there is a sagged wire defined, this will be used for the transformation, otherwise the detector element surface is used*/
    void setGlobalPosition(HepGeom::Point3D<double>& loc3Dframe) const;                  

    //Sets the error strategy, only used by the Muon::MdtDriftCircleOnTrackCreator
    void setErrorStrategy(const MuonDriftCircleErrorStrategy* strategy);

    /**information on the status of the Mdt measurement - see Trk::DriftCircleStatus for definitions*/
    Trk::DriftCircleStatus                      m_status;
    
    /** the pointer to the MdtPrepData object (mutable because it might need to be recreated when reading tracks)*/
    ElementLinkToIDC_MDT_Container              m_rio;

    /** global position of the measurement. */
    mutable const Amg::Vector3D*          m_globalPosition;
    
    /** Surface at postion of sagged wire. This is owned by this object, and will be deleted when it 
    is.*/
    const Trk::StraightLineSurface*             m_saggedSurface;
    
    /** The detector element, associated with this measurement*/
    const MuonGM::MdtReadoutElement*            m_detEl;

    /** This angle is the position of the point of closest approach in cylindrical coordinates, and is needed to construct the global position*/
    mutable float                               m_localAngle;
    
    /** This is the position of the point of closest approach, in the local z coord (i.e. along the wire), and is needed to construct the global position*/
    mutable float                               m_positionAlongWire;

    /** This is the drift time used to obtain the drift radius */
    mutable float                               m_driftTime;
    
    /** Records information about the 'strategy' used by Muon::MdtDriftCircleOnTrackCreator to make this object.*/    
    MuonDriftCircleErrorStrategy                m_errorStrategy;
    
};

// /////////////////////////////////////////////////////////////////
// Inline methods:
// /////////////////////////////////////////////////////////////////:

inline Trk::DriftCircleSide MdtDriftCircleOnTrack::side() const
{
    if (m_status == Trk::UNDECIDED) return Trk::NONE;
    if (localParameters()[Trk::driftRadius] < 0. ) return Trk::LEFT;
    return Trk::RIGHT;
}

inline  Trk::DriftCircleStatus MdtDriftCircleOnTrack::status() const
  { 
    return m_status;
  }


inline MdtDriftCircleOnTrack* MdtDriftCircleOnTrack::clone() const
{
    return new MdtDriftCircleOnTrack(*this);
}

inline const MdtPrepData* MdtDriftCircleOnTrack::prepRawData() const
{
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    //std::cout<<"MdtDriftCircleOnTrack::WARNING invalid PRD"<<std::endl;
    return 0;
}

inline const ElementLinkToIDC_MDT_Container& MdtDriftCircleOnTrack::prepRawDataLink() const
{
  return m_rio;
}

inline IdentifierHash MdtDriftCircleOnTrack::collectionHash() const
{
    const MdtPrepData* prd = prepRawData();
    return prd->collectionHash();
}

inline const MuonGM::MdtReadoutElement* MdtDriftCircleOnTrack::detectorElement() const
{
    return m_detEl;
}

inline const Trk::Surface& MdtDriftCircleOnTrack::associatedSurface() const
{
    assert(0!=detectorElement());
    if ( hasSaggedSurface() ) return *m_saggedSurface;
    return detectorElement()->surface(identify());
}

inline  void MdtDriftCircleOnTrack::setLocalParameters(const Trk::LocalParameters& locParams)
{
    // update base class values
  m_localParams = locParams;
}

inline void MdtDriftCircleOnTrack::setErrorStrategy(const MuonDriftCircleErrorStrategy* strategy)
{
  m_errorStrategy=*strategy;
}

inline  void MdtDriftCircleOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
        const Trk::PrepRawData* /*prd*/)
{
    m_detEl = dynamic_cast< const MuonGM::MdtReadoutElement* >(detEl);
    //m_rio   = dynamic_cast< const MdtPrepData* >(prd);
    assert (m_detEl!=0);
}

inline bool MdtDriftCircleOnTrack::hasSaggedSurface() const
{
    if (m_saggedSurface==0) return false;
    return true;
}


inline double MdtDriftCircleOnTrack::driftRadius() const
{
    return localParameters().get(Trk::driftRadius);
}

inline double MdtDriftCircleOnTrack::driftTime() const
{
  return m_driftTime;
}

inline float MdtDriftCircleOnTrack::positionAlongWire() const
{
    return m_positionAlongWire;
}

inline float MdtDriftCircleOnTrack::localAngle() const
{
    return m_localAngle;
}

inline const MuonDriftCircleErrorStrategy& MdtDriftCircleOnTrack::errorStrategy() const {
    return m_errorStrategy;
}

}

#endif // MUONRIOONTRACK_MUONDRIFTCIRCLEONTRACK_H


