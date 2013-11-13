/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialMapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_MATERIALMAPPER_H
#define TRKDETDESCRTOOLS_MATERIALMAPPER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkDetDescrInterfaces/IMaterialMapper.h"
// ROOT
#include "TTree.h"
#include "TString.h"

#define TRKDETDESCRTOOLS_MAXSTEPS 8000
#define TRKDETDESCRTOOLS_MAXLAYERHITS 5000

class TTree;

namespace Trk {

    class TrackingVolume;
    class Layer;


    /** @struct VolumeTreeObject
    -> simple helper struct for filling the Volume objects
    */

    struct VolumeTreeObject {

        float eta;
        float phi;
        float path;
        float pathInX0;
        float pathInL0;
        float A;
        float Z;
        float rho;
        float measure;

        TTree* tree;

        VolumeTreeObject(TString name, TString title)
        : eta(0),
          phi(0.),
          path(0.),
          pathInX0(0.),
          pathInL0(0.),
          A(0.),
          Z(0.),
          rho(0.),
          measure(0.),
          tree(new TTree(name,title))
        {
            tree->Branch("Eta",       &eta,        "eta/F");
            tree->Branch("Phi",       &phi,        "phi/F");
            tree->Branch("Path",      &path,       "path/F");
            tree->Branch("PathInX0",  &pathInX0,   "pathX0/F");
            tree->Branch("PathInL0",  &pathInL0,   "pathL0/F");
            tree->Branch("A",         &A,          "a/F");
            tree->Branch("Z",         &Z,          "z/F");
            tree->Branch("Rho",       &rho,        "rho/F");
            tree->Branch("Measure",   &measure,    "measure/F");
        }
    };


    struct LayerTreeObject {

        // values for the full histo
        float densedHitX;
        float densedHitY;
        float densedHitZ;
        float densedHitR;
        float densedHitPhi;
        float densedHitTheta;
        int   densedHits;

        float correctionFactor;
        float pathInX0;
        float pathInL0;
        float rho;

        int   layerHits;
        float hitPositionX[TRKDETDESCRTOOLS_MAXLAYERHITS];
        float hitPositionY[TRKDETDESCRTOOLS_MAXLAYERHITS];
        float hitPositionZ[TRKDETDESCRTOOLS_MAXLAYERHITS];
        float hitPositionR[TRKDETDESCRTOOLS_MAXLAYERHITS];
        float hitPositionEta[TRKDETDESCRTOOLS_MAXLAYERHITS];

        TTree* tree;

        LayerTreeObject(TString name, TString title)
        : densedHitX(0.),
          densedHitY(0.),
          densedHitZ(0.),
          densedHitR(0.),
          densedHitPhi(0.),
          densedHitTheta(0.),
          densedHits(0),
          correctionFactor(0.),
          pathInX0(0.),
          rho(0.),
          layerHits(0),
          tree(new TTree(name,title))
        {
            tree->Branch("DensedHits",       &densedHits,        "densedHits/I");
            tree->Branch("DensedHitX",       &densedHitX,        "densedHitX/F");
            tree->Branch("DensedHitY",       &densedHitY,        "densedHitY/F");
            tree->Branch("DensedHitZ",       &densedHitZ,        "densedHitZ/F");
            tree->Branch("DensedHitR",       &densedHitR,        "densedHitR/F");
            tree->Branch("DensedHitPhi",     &densedHitPhi,      "densedHitPhi/F");
            tree->Branch("DensedHitTheta",   &densedHitTheta,    "densedHitTheta/F");

            tree->Branch("CorrFactor",       &correctionFactor,  "corrF/F");
            tree->Branch("PathInX0",         &pathInX0,          "pathInX0/F");
            tree->Branch("PathInL0",         &pathInL0,          "pathInL0/F");
            tree->Branch("Rho",              &rho,               "rho/F");

            tree->Branch("Hits",             &layerHits,         "layerhits/I");
            tree->Branch("HitPositionX",     &hitPositionX,      "layerhitsX[layerhits]/F");
            tree->Branch("HitPositionY",     &hitPositionY,      "layerhitsY[layerhits]/F");
            tree->Branch("HitPositionZ",     &hitPositionZ,      "layerhitsZ[layerhits]/F");
            tree->Branch("HitPositionR",     &hitPositionR,      "layerhitsR[layerhits]/F");
            tree->Branch("HitPositionEta",   &hitPositionEta,    "layerhitsR[layerhits]/F");
        }
    };


    /** @struct ReferenceSurfaceTree
    -> simple helper struct for reference surfaces
    */
    struct SurfaceTreeObject {

        float loc1;
        float loc2;
        float eta;
        float pathInX0;
        float pathInL0;
        float correctionFactor;
        float rho;

        int    surfaceHits;
        TTree* tree;

        SurfaceTreeObject(TString name, TString title)
        : loc1(0.),
          loc2(0.),
          eta(0.),
          pathInX0(0.),
          correctionFactor(0.),
          rho(0.),
          surfaceHits(0),
          tree(new TTree(name,title))
        {
            tree->Branch("Local1",           &loc1,              "local1/F");
            tree->Branch("Local2",           &loc2,              "local2/F");
            tree->Branch("Eta",              &eta,               "eta/F");
            tree->Branch("CorrFactor",       &correctionFactor,  "corrF/F");
            tree->Branch("PathInX0",         &pathInX0,          "pathInX0/F");
            tree->Branch("PathInL0",         &pathInL0,          "pathInL0/F");
            tree->Branch("Rho",              &rho,               "rho/F");
        }
    };


    /** @class MaterialMapper

    Implementation of the class MaterialMapper.
    The correct material mapping can only be done with stiff tracks as the
    finalizeEvent method requires an eta,phi bin!

    @author Andreas.Salzburger@cern.ch

     */

    class MaterialMapper : public AthAlgTool, virtual public IMaterialMapper {

      public:

        /** AlgTool like constructor */
        MaterialMapper(const std::string&,
                       const std::string&,
                       const IInterface*);

        /**Virtual destructor*/
        virtual ~MaterialMapper();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();

        /** Record navigation hit : 
            - navigation hit position       
         */
        void record(const Amg::Vector3D& position, int type = 0) const;

        /** Record material hit :
         -> For pure Material validation only pathInX0, x0, A, Z, rho, can be filled,
         -> For Material effects validation sigmaPhi, sigmaTheta, delta p and sigmaP
         can be filled as well
         */
        void recordMaterialHit(const AssociatedMaterial& amhit, const Amg::Vector3D& projectedPosition) const;

        /** Record material hit - if various hits per uniform layer are recorded */
        void recordLayerHit(const AssociatedMaterial& amhit) const;

        /** Record material hit - possibility to overwrite the single layer hit records */
        void recordFullLayerHit(const AssociatedMaterial& amhit) const;

        /** Record material hit on a surface*/
        void recordSurfaceHit(const Amg::Vector2D& locpos,
                              const AssociatedMaterial& amhit) const;

        /** FinalizeEvent */
        void finalizeEvent(double eta, double phi) const;

      private:

        /** Finalize the Volume Steps */
        void finalizeVolumeHits(bool mapped=true) const;

        /** Finalize the SingleLayer Steps */
        void finalizeLayerHits() const;

        /** Finalize the SingleLayer Steps */
        void finalizeSurfaceHits() const;

        /** Validation : book the Tree */
        void bookValidationTree();

        /** find (&&,||) create a VolumeTreObject */
        VolumeTreeObject* volumeTreeObject(const Layer* lay=0, const TrackingVolume* tvol=0) const;

        /** find (&&,||) create the LayerTreeObject  */
        LayerTreeObject* layerTreeObject(const Layer& lay) const;

        /** find (&&,||) create the LayerTreeObject  */
        SurfaceTreeObject* surfaceTreeObject(const Layer& lay) const;



        int                   m_materialAssociationType;
        mutable double        m_eta;
        mutable double        m_phi;
        mutable bool          m_recordDone;

        // (1) ------------------------- Tree name of the total material statistics
        unsigned int          m_maxMappingEvents;
        mutable unsigned int  m_processedEvents;
        
        std::string           m_totalMaterialTree;

        // (2) ------------------------  Ntuple output of material effects validation
        bool                  m_validationMode;
        TTree*                m_validationTree;            //!< The validation tree
        std::string           m_validationTreeName;        //!< validation tree name - to be accessed by this from root
        std::string           m_validationTreeDescription; //!< validation tree description - second argument in TTree
        std::string           m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out


        // Spatial information
        mutable int           m_materialSteps;                                     //!< number of update positions
        mutable float         m_averageEta;                                         //!< total path in x0 in these events
        mutable float         m_averagePhi;                                         //!< total path in x0 in these events lost
        mutable float         m_mappedPathInX0;                                    //!< total path in x0 in these events
        mutable float         m_unmappedPathInX0;                                  //!< total path in x0 in these events lost
        mutable int           m_mapped[TRKDETDESCRTOOLS_MAXSTEPS];                 //!< mapped or not mapped
        mutable float         m_materialStepPathInX0[TRKDETDESCRTOOLS_MAXSTEPS];   //!< x position of the material recording
        mutable float         m_materialStepPositionX[TRKDETDESCRTOOLS_MAXSTEPS];  //!< x position of the material recording
        mutable float         m_materialStepPositionY[TRKDETDESCRTOOLS_MAXSTEPS];  //!< y position of the material recording
        mutable float         m_materialStepPositionZ[TRKDETDESCRTOOLS_MAXSTEPS];  //!< z position of the material recording
        mutable float         m_materialStepPositionR[TRKDETDESCRTOOLS_MAXSTEPS];  //!< r value of the material recording
        mutable float         m_materialProjPositionX[TRKDETDESCRTOOLS_MAXSTEPS];  //!< x position of the material recording
        mutable float         m_materialProjPositionY[TRKDETDESCRTOOLS_MAXSTEPS];  //!< y position of the material recording
        mutable float         m_materialProjPositionZ[TRKDETDESCRTOOLS_MAXSTEPS];  //!< z position of the material recording
        mutable float         m_materialProjPositionR[TRKDETDESCRTOOLS_MAXSTEPS];  //!< r value of the material recording

        bool                  m_navigationValidation;
        
        mutable int           m_navigationSteps;
        mutable float         m_navigationStepPositionX[TRKDETDESCRTOOLS_MAXSTEPS];  //!< x position of a navigation step
        mutable float         m_navigationStepPositionY[TRKDETDESCRTOOLS_MAXSTEPS];  //!< y position of a navigation step
        mutable float         m_navigationStepPositionZ[TRKDETDESCRTOOLS_MAXSTEPS];  //!< z position of a navigation step
        mutable float         m_navigationStepPositionR[TRKDETDESCRTOOLS_MAXSTEPS];  //!< r value of a navigation step

        // Per Volume Validation
        bool                  m_volumeValidation;
        std::string           m_volumeTreePrefix;
        mutable std::map< const Trk::TrackingVolume*, VolumeTreeObject* > m_volumeTrees;
        mutable std::map< const Trk::TrackingVolume*, VolumeTreeObject* > m_volumeTreesUnmapped;

        // Per Layer Validation
        bool                  m_layerValidation;
        std::string           m_layerTreePrefix;
        mutable std::map< const Trk::Layer*, LayerTreeObject* >           m_layerTrees;

        // Per Surface Validation
        bool                  m_surfaceValidation;
        std::string           m_surfaceTreePrefix;
        mutable std::map< const Trk::Layer*, SurfaceTreeObject* >         m_surfaceTrees;

        // tolerance parameters
        double                m_etaTolerance;
        double                m_phiTolerance;
        bool                  m_externalEtaPhi;

    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_MATERIALMAPPER_H

