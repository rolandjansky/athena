/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNMODULE_H
#define TRKALIGNEVENT_ALIGNMODULE_H

#include "TrkEventPrimitives/ParamDefs.h"


//#include "TrkDetElementBase/TrkDetElementBase.h"
#include "Identifier/Identifier.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

//#include <algorithm>
#include <vector>
#include <string>

/**
   @file AlignModule.h
   @class AlignModule

   @brief AlignModule is a grouping of TrkDetElementBase objects, grouped according to the type of alignment, groupings being specific to detector type.  
   For example, for MDT alignment:
   1) L1 alignment uses an AlignModule for each endcap and one for the barrel for a total of 3 AlignModule elements in the AlignModuleList.
   2) L3 alignment uses a single MDT chamber for each AlignModule.
   3) L2 and other types not yet implemented for MDT alignment.
   See detector-specific implementation of IGeometryManagerTool::createAlignModuleList() to see how implemented.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 10/14/2008
*/

class MsgStream;
class AlgTool;

namespace Trk {
  
  class TrkDetElementBase;
  class AlignTrack;
  class AlignPar;

  void decomposeTransform(const Amg::Transform3D& transform, double* values);
  
  class AlignModule {
       
  public:
    
    /////////////////////////////////////////////////////////////////////////
    // enums/typedefs
    /////////////////////////////////////////////////////////////////////////
    
    /** enum to keep track of transformation coordinates */
    enum TransformParameters { TransX=0, TransY, TransZ, RotX, RotY, RotZ, BowX, BowY, BowZ,  
             NTransformPar, MaxNPar=10 };
        
    enum DetectorType { unidentified=0,Pixel,SCT,TRT,MDT,CSC,RPC,TGC,NDetectorTypes};    
      
    /** typedefs to contain detector element pointers and transforms */
    typedef std::vector<const TrkDetElementBase*> DetElementCollection;
    typedef std::vector<Identifier>               IdentifierCollection;
    typedef std::vector<Amg::Transform3D>           TransformCollection;

    /////////////////////////////////////////////////////////////////////////
    // constructors/destructor
    /////////////////////////////////////////////////////////////////////////

    /** constructor creates MsgStream with output level of parent tool and AlignModule for name.  Transform is global to align frame transform */
    AlignModule(const AlgTool* algtool, 
                const Amg::Transform3D& globalToAlignXform=Amg::Transform3D::Identity(),
                const std::string& name="");

    /** constructor gets msgSvc from parent and creates MsgStream with name AlignModule.  Transform is global to align frame transform */
    AlignModule(MsgStream* log, 
                const Amg::Transform3D& globalToAlignXform=Amg::Transform3D::Identity(),
                const std::string& name="");
                
    AlignModule() = delete; // don't allow constructor without IMessageSvc
    
    /**forbid copy constructor **/
    AlignModule(const AlignModule & rhs) = delete;
    
    /**forbid assignment **/
    AlignModule & operator=(const AlignModule & rhs) = delete;

    virtual ~AlignModule();

    /////////////////////////////////////////////////////////////////////////
    // public methods
    /////////////////////////////////////////////////////////////////////////

    /** Set and return name of align module (i.e. endcap A, etc.) */
    void  setName(const std::string& name) { m_name=name; }
    const std::string& name() const { return m_name; }

    /** Set and return index of module, used by alignment classes to keep track of order of align module */
    IdentifierHash  identifyHash() const { return m_idHash; }
    void setIdHash(IdentifierHash id) { m_idHash = id; }
    
    /** Set and return identifier of module */
    void setIdentifier(Identifier identifier) { m_identifier = identifier; }
    Identifier identify() const { return m_identifier; }

    /** returns 32-bit compact identifier */
    unsigned int identify32() const { return m_identifier.get_identifier32().get_compact(); }

    /////////////////////////////////////////////////////////////////////////
    // alignment transforms
    /////////////////////////////////////////////////////////////////////////

    /** returns global frame to alignment frame */
    const Amg::Transform3D& globalFrameToAlignFrame() const { return m_globalToAlignFrameTransform ; }

    /** returns global to alignment frame transforms */
    const Amg::RotationMatrix3D& globalToAlignFrameRotation() const { return m_globalToAlignFrameRotation ; }
    const Amg::Vector3D&         globalToAlignFrameTranslation() const { return m_globalToAlignFrameTranslation ; }
    
    /** set global to alignment frame transforms */
    void setGlobalFrameToAlignFrameTransform( const Amg::Transform3D& t ) {
      m_globalToAlignFrameTransform   = t ;
      m_globalToAlignFrameTranslation = t.translation() ;
      m_globalToAlignFrameRotation    = t.rotation() ;
    }

    /** set values for a set of alignment parameters and errors */
    void setAlignParShift(const AlignPar* alignPar) const;

    //
    // methods dealing with detector elements
    //

    /** returns collection of detector elements */
    const DetElementCollection* detElementCollection(AlignModule::DetectorType detType) const { return m_detelements[detType]; } 

    /** returns collection of Identifiers */
    const IdentifierCollection* identifierCollection(AlignModule::DetectorType detType) const { return m_detIdentifiers[detType]; }

    /** returns align module to detector element transforms */
    const std::vector<Amg::Transform3D>* alignModuleToDetElementTransforms(AlignModule::DetectorType detType) { return m_alignModuleToDetElementTransforms[detType]; }

    /** returns AlignModule to DetElement transform for a detector element */
    const Amg::Transform3D *
      alignModuleToDetElementTransform(AlignModule::DetectorType detType, 
                                       const TrkDetElementBase* det,
                                       const Identifier id=Identifier()) const;

    /** used to add a detector element to the align module with a align frame 
        to detector element local frame transform */
    void addDetElement(AlignModule::DetectorType detType,
                       const TrkDetElementBase* det,
                       const Amg::Transform3D &transform,
                       Identifier id=Identifier());
    
    //
    // methods used for numerical derivatives
    // 

    /** Used only for numerical derivatives.  Sets and returns number of 
  chamber shifts used to plot chi2 as a function of alignment parameter. */
    void setNChamberShifts(int nShifts) const { m_nChamberShifts = nShifts; }
    int  nChamberShifts() const { return m_nChamberShifts; }

    /** Used only for numerical derivatives to get chi2 as a function of 
  alignment parameters */
    void     setChi2VAlignParamArray(double** chi2VAlignParam) const { m_chi2VAlignParam = chi2VAlignParam; }
    double** chi2VAlignParamArray() const { return m_chi2VAlignParam; }

    /** sets the values of chamber shifts in array */
    void     setChi2VAlignParamXArray(double** chi2VAlignParamX) const { m_chi2VAlignParamX = chi2VAlignParamX; }
    double** chi2VAlignParamXArray() const { return m_chi2VAlignParamX; }
    
    /** vector of arrays of chamber shifts and chi2, one for each type of TrackState::MeasurementType */
    void     setChi2VAlignParamArrayMeasType(int measType, double** chi2VAlignParam) const { m_chi2VAlignParamMeasType[measType]=chi2VAlignParam; }
    double** chi2VAlignParamArrayMeasType(int measType) const { return m_chi2VAlignParamMeasType[measType]; }
    
    //
    // miscellaneous methods
    //
    
    /** Increments and returns number of tracks passing through align module */
    void addTrack(int ntracks=1) const { m_ntracks+=ntracks; }
    int  nTracks() const { return m_ntracks; }
    void setNTracks(int ntracks) const { m_ntracks=ntracks; }

    /** Increments and returns chi2 of tracks passing through align module */
    void addTrackChi2(double chi2) const { m_trackchi2+=chi2; }
    double trackChi2() const { return m_trackchi2; }
    void setTrackChi2(double chi2) const { m_trackchi2=chi2; }

    /** Increments and returns number of hits in align module */
    void addHit(int nhits=1) const { m_nhits+=nhits; }
    int  nHits() const { return m_nhits; }
    void setNHits(int nhits) const { m_nhits=nhits; }

    /** Increments and returns number of degrees of freedom for tracks 
        passing through align module */
    void addNDoF(int nDoF) const { m_nDoF+=nDoF; }
    int  nDoF() const { return m_nDoF; }
    void setNDoF(int nDoF) const { m_nDoF=nDoF; }

    /** Calculates center of gravity (average center of associated det-elements) */
    static Amg::Vector3D  centerOfGravity(const std::vector<DetElementCollection*>&);
    Amg::Vector3D         centerOfGravity() const;
 
    void shiftSurface(TrkDetElementBase*, Identifier) const {}
    void restoreSurfaces(TrkDetElementBase*) const {}

    /** Calculates Align to Global transform  based on the TrkDetElementBase in the alignmodule **/
    Amg::Transform3D  calculateAlignModuleToGlobal() const;
     
    /** Reset  align module to detector element transforms based on the AlignModuleToGlobal 
        transform and the global to DetElementTransform **/
    void resetAlignModuleToDetElementTransforms();



  protected:

    std::vector<DetElementCollection*> m_detelements;
    std::vector<IdentifierCollection*> m_detIdentifiers;

    // vector of parameters and transforms for individual detectors
    std::vector<std::vector<Amg::Transform3D>* > m_alignModuleToDetElementTransforms;

  private:
    
    
    // private variables
    Amg::Transform3D        m_globalToAlignFrameTransform ;   // transform to go from global frame to alignment frame
    Amg::RotationMatrix3D   m_globalToAlignFrameRotation ;    // cache for speed up
    Amg::Vector3D           m_globalToAlignFrameTranslation ;  //actually a vector not a point
        
    
    // chi2 vs. align parameters
    mutable int      m_nChamberShifts;
    mutable double** m_chi2VAlignParam;
    mutable double** m_chi2VAlignParamX;
    mutable double*** m_chi2VAlignParamMeasType;
    
    std::string    m_name;
    IdentifierHash m_idHash;
    Identifier     m_identifier;
 
    mutable int m_ntracks; // number of tracks going through module
    mutable int m_nhits;   // number of hits in the module

    mutable double m_trackchi2; // chi2 of tracks going through module
    mutable int    m_nDoF;    // number of degrees of freedom for tracks going through module

    MsgStream* m_log;
    
  }; // end class


  /** returns the detector type */
  std::string detTypeStr(AlignModule::DetectorType detType);

  /** overload of << operator for MsgStream for debug output */
  MsgStream& operator << (MsgStream& sl, const AlignModule& alignModule);
 

} // end namespace

#endif // TRKALIGNEVENT_ALIGNMODULE_H
