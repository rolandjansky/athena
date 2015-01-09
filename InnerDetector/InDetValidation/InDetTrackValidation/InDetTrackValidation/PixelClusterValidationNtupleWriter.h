/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PixelClusterValidationNtupleWriter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Aldo.Beati
 
#ifndef INDET_PIXELCLUSTERVALIDATIONNTUPLEWRITER_H
#define INDET_PIXELCLUSTERVALIDATIONNTUPLEWRITER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetPrepRawData/PixelClusterContainer.h" // typedef
#include "InDetSimEvent/SiHitCollection.h"          // typedef
#include "InDetRawData/PixelRDO_Container.h"        // typedef
#include "TrkTrack/TrackCollection.h"

#include <vector>
#include <string>

// Constants to deal with diffenrent geometries.
#define ATLAS 0
#define IBL   1
#define SLHC  2
#define SLHC2  3

#define ATLASPITCHY  0.4
#define SLHCPITCHY  0.25
#define IBLPITCHY  0.25
#define SLHC2_PITCHY_A  0.15
#define SLHC2_PITCHY_B  0.25




class PixelID;
class TTree;
class SiHit;
class IPixelOfflineCalibSvc;
class IBeamCondSvc;
class IGeoModelSvc;
class IBLParameterSvc;


namespace InDet {
    //class SiHitCollection;

/** @class Pixel_ClusterValidationNtupleWriter
 
   Validation Algorithm to retrieve a Pixel_ClusterContainer, loop over the entries and fill
   the validation ntuple with RIO specific entries.
   
   @author Sebastian.Fleischmann@cern.ch
//Aldo.Beati
*/

  struct NNinput
  { 
    int sizeX;
    int sizeY;
    std::vector<std::vector<float> > matrixOfToT;
    std::vector<float> vectorOfPitchesY;
    int ClusterPixLayer;
    int ClusterPixBarrelEC;
    float phi;
    float theta;
    float etaModule;
    bool useTrackInfo;
    int columnWeightedPosition;
    int rowWeightedPosition;
  };
  
  
  
class PixelClusterValidationNtupleWriter : public AthAlgorithm {
public:
    /** Standard Athena-Algorithm Constructor */
    PixelClusterValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~PixelClusterValidationNtupleWriter();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    
    const SiHit * 	FindNextHit(const SiHit *, SiHitCollection::const_iterator);
    int		FindAssociatedParticle(int, int, int);
    std::vector<int> FindMultipleAssociatedParticle(int, int, int);
    
private:
    //    NNinput*  createInput(const InDet::PixelCluster , HepPoint3D  , double , double , int , int );

    StatusCode fillClusterVector(const InDet::PixelCluster& cluster, IdentifierHash element_hash );

    StatusCode createInput(const InDet::PixelCluster& pCluster,
                     //    HepPoint3D & beamSpotPosition,
                           Amg::Vector3D & beamSpotPosition,
                         double & tanl,
                         double & pitch_y,
                         int sizeX=7,
                         int sizeY=7);


    const InDet::PixelClusterContainer*  m_riocontainer; //!< container of RIOs
    const SiHitCollection* 	 	 m_hitcontainer; //!< container of HITs
    const PixelRDO_Container* 		 m_rdocontainer; //!< container of RDOs
    std::string mjo_riocontainername;   		 //!< jobOption: name of container with RIOs
    std::string mjo_hitcontainername;   		 //!< jobOption: name of container with HITs
    std::string mjo_rdocontainername;  			 //!< jobOption: name of container with RDOs
    std::string	mjo_trackCollectionName;                 //!< jobOption: name of container with Tracks
    bool mjo_Hits;					 //!< jobOption: fill Pixel Geant4 hits information
    bool mjo_MC;					 //!< jobOption: associate Pixel Geant4 hits to clusters
    bool mjo_NotAssociated;				 //!< jobOption: fill not associated Pixel Geant4 hits info
    bool mjo_WriteDetailedPixelInformation;              //!< jobOption: fill pixel info: ToT, Charge, eta, phi, LVL1A
    bool mjo_NN;			 	   	 //!< jobOption: Write training NN ntuple 
    bool mjo_ToT;			 	   	 //!< jobOption: Write ToT instead of charge to the NN training ntuple 
    bool mjo_onTrack;                                    //!< jobOption: fill only for cluster on track
    mutable const TrackCollection*        m_trackCollection;
    bool                              m_IBLAbsent;
    ServiceHandle<IBLParameterSvc>    m_IBLParameterSvc; 
    ServiceHandle<IPixelOfflineCalibSvc> m_pixelCalib;
    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;

    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    std::string m_ntupleNNTreeName;       //!< jobOption: Ntuple tree name

    const PixelID* m_pixelid;           //   Pixel ID helper
    
    //! Pointer to the NTuple tree
    TTree* m_nt;
    TTree* m_nnt;

    // --- Contans Geometry
    int _geoId;    //!< Geometry ID (see defines in header).
    float _pitchY; //!< default y pitch.
    // Number of layers to flip in SLHC 
    int _slhc_layer_flip_1;
    int _slhc_layer_flip_2;
    int _slhc_layer_flip_3;

    // --- ntuple items ---
    long m_eventNumber;      //!< current event number
    long m_pixClusNum;       //!< number of pixel clusters per event
    long m_nRIOs;

    long m_lvl1TriggerType;  //!< Level1 Trigger type (stream) UNIT:8-bit

    /// Access to GeoModelSvc for tags
    IGeoModelSvc* m_geoModel;   
    
    std::vector<float>*   m_PixClusLocX;      //!< Local X coordinate of the cluster (digital position) UNIT:mm
    std::vector<float>*   m_PixClusLocY;      //!< Local Y coordinate of the cluster (digital position) UNIT:mm
    std::vector<float>*   m_PixClusLocXcentroid; //!< Local X coordinate of the cluster (centroid) UNIT:mm
    std::vector<float>*   m_PixClusLocYcentroid; //!< Local Y coordinate of the cluster (centroid) UNIT:mm
    std::vector<float>*   m_PixClusLocX_LorentzCorrection; //!< Lorentz Correction of the Local X coordinate of the cluster UNIT:mm
    std::vector<float>*   m_PixClusGloX;      //!< Global X of surface centre of the cluster UNIT:mm
    std::vector<float>*   m_PixClusGloY;      //!< Global Y of surface centre of the cluster UNIT:mm
    std::vector<float>*   m_PixClusGloZ;      //!< Global Z of surface centre of the cluster UNIT:mm
    std::vector<float>*   m_PixClusEta;       //!< Global eta position of the cluster 
    std::vector<float>*   m_PixClusPhi;       //!< Global phi position of the cluster 
    std::vector<float>*   m_PixECBarrel;      //!< Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel) UNIT:index
    std::vector<float>*   m_PixClusLayer;     //!< Layer information (0, 1 or 2) UNIT:index
    std::vector<float>*   m_PixEtaModule;     //!< Eta index of the module with cluster UNIT:index
    std::vector<float>*   m_PixPhiModule;     //!< Phi index of the module with cluster UNIT:index
    std::vector<float>*   m_PixClusGroupsize; //!< Number of pixels in the clusters UNIT:cells
    std::vector<float>*   m_PixClusRow;       //!< Row index of the first pixel of the cluster UNIT:index 
    std::vector<float>*   m_PixClusCol;       //!< Cloumn index of the first pixel of the cluster UNIT:index
    std::vector<int>*     m_PixDeltaRow;      //!< Row width of the cluster UNIT:cells
    std::vector<int>*     m_PixDeltaCol;      //!< Column width of the cluster UNIT:cells
    std::vector<float>*   m_PixOmegaPhi;      //!< Charge sharing variable, phi direction 
    std::vector<float>*   m_PixOmegaEta;      //!< Charge sharing variable, eta direction 
    std::vector<int>*     m_PixClusToT;       //!< Time over Threshold of the cluster 
    std::vector<float>*   m_PixClusCharge;    //!< Total charge of the cluster UNIT:electrons
    std::vector<int>*     m_PixClusLvl1;      //!< Level 1 accept of the cluster 
    std::vector<int>*     m_PixClusGanged;    //!< If the cluster contains ganged pixels (1:yes)
    std::vector<int>*     m_PixClusFake;      //!< If the cluster is flagged as fake (1:yes)
    std::vector<int>*     m_PixClusAmbiguous; //!< If the cluster is ambiguous (1:yes)
    std::vector<float>*   m_PixClusLocXana;   //!< Local X coordinate of the cluster (analog position) UNIT:mm
    std::vector<float>*   m_PixClusLocYana;   //!< Local Y coordinate of the cluster (analog position) UNIT:mm
    std::vector<float>*   m_PixClusErrX;      //!< Precise parameterized error on local X coordinate of the cluster UNIT:mm
    std::vector<float>*   m_PixClusErrY;      //!< Precise parameterized error on local Y coordinate of the cluster UNIT:mm
    std::vector<std::vector<int> >* m_PixClusEtaIndex; //!< Individual pixel column index (eta direction) UNIT:index
    std::vector<std::vector<int> >* m_PixClusPhiIndex; //!< Individual pixel row index (phi direction) UNIT:index
    std::vector<std::vector<float> >* m_PixClusChargeList; //!< Charge of the pixel UNIT:electrons 
    std::vector<std::vector<int> >* m_PixClusToTList; //!< Time over Threshold of the pixel
    std::vector<std::vector<int> >* m_PixClusLVL1AList; //!< Level 1 accept of the pixel
    std::vector<int>*     m_PixClusIsSplit;   //!< If the cluster is splitted by NN cluster   (1:yes)
    std::vector<float>*   m_PixClusSplitProb1;//!< Probability of being 1  particle cluster  
    std::vector<float>*   m_PixClusSplitProb2;//!< Probability of being >1 particle cluster


    long m_pixHitsNum;  //!< number of Geant4 hits strings simulated for current event 
    std::vector<float>*	  m_PixHitXstartpos; 	//!< Local X position of the starting point of the Geant4 hit UNIT:mm
    std::vector<float>*	  m_PixHitYstartpos; 	//!< Local Y position of the starting point of the Geant4 hit UNIT:mm
    std::vector<float>*	  m_PixHitZstartpos; 	//!< Local Z position of the starting point of the Geant4 hit UNIT:mm
    std::vector<float>*	  m_PixHitXendpos;   	//!< Local X position of the ending point of the Geant4 hit UNIT:mm
    std::vector<float>*	  m_PixHitYendpos;   	//!< Local Y position of the ending point of the Geant4 hit UNIT:mm
    std::vector<float>*	  m_PixHitZendpos;   	//!< Local Z position of the ending point of the Geant4 hit UNIT:mm
    std::vector<int>*	  m_PixHitBarrelEndcap; //!< Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel) UNIT:index
    std::vector<int>*	  m_PixHitLayerDisk; 	//!< Layer information (0, 1 or 2) UNIT:index
    std::vector<int>*	  m_PixHitEtaModule; 	//!< Eta index of the module with G4 hit UNIT:index
    std::vector<int>*	  m_PixHitPhiModule; 	//!< Phi index of the module with G4 hit UNIT:index
    std::vector<int>*     m_PixHitEtaIndex;  	//!< G4 hit position converted into pixel column index (eta direction) UNIT:index
    std::vector<int>*     m_PixHitPhiIndex;  	//!< G4 hit position converted into pixel row index (phi direction) UNIT:index
    std::vector<float>*   m_PixHitAngle;     	//!< G4 hit incidence angle in phi direction UNIT:rad
    std::vector<float>*   m_PixHitAngle2;    	//!< G4 hit incidence angle in eta direction UNIT:rad
    std::vector<float>*   m_PixHitAngle_Lorcorr;//!< G4 hit corrected incidence angle UNIT:rad
    std::vector<float>*   m_PixHitEnergyLoss;	//!< G4 hit total energy loss UNIT:GeV
    std::vector<float>*   m_PixHitTime;      	//!< G4 hit arrival time UNIT:ns
    std::vector<int>*     m_PixHitPDGParticle; 	//!< G4 hit particle PDG code 
    std::vector<int>*	  m_PixHitFlag;      	//!< G4 hit flag
    std::vector<int>*	    m_PixHitBarcode;      	//!< G4 hit flag
    std::vector<float>*   m_PixHitParticleP;
//    std::vector<int>*     m_PixHitEtaIndex_delta;
//    std::vector<int>*     m_PixHitPhiIndex_delta;

    std::vector<float>*   m_MC_Xpos;           //!< G4 hit associated to cluster local X position UNIT:mm
    std::vector<float>*   m_MC_Ypos;           //!< G4 hit associated to cluster local Y position UNIT:mm
    std::vector<float>*   m_MC_IncidentAngle;  //!< G4 hit associated to cluster incidence angle in phi direction UNIT:rad
    std::vector<float>*   m_MC_IncidentAngle2; //!< G4 hit associated to cluster incidence angle in eta direction UNIT:rad
    std::vector<float>*   m_MC_IncidentAngle_Lorcorr; //!< MC_IncidentAngle corrected for the Lorentz angle UNIT:rad
    std::vector<float>*   m_MC_EnergyLoss;     //!< G4 hit associated to cluster total energy loss UNIT:GeV
    std::vector<float>*   m_MC_Time; 	       //!< G4 hit associated to cluster arrival time UNIT:ns
    std::vector<int>*     m_MC_PDGParticle;    //!< G4 hit associated to cluster particle PDG code 
    std::vector<int>*     m_MC_Flag; 	       //!< G4 hit associated to cluster flag 
    std::vector<int>*     m_MC_Barcode;      //!< G4 hit associated to cluster particle PDG code 
    std::vector<float>*   m_MC_Momentum;      //!< G4 hit associated to cluster particle PDG code 

    std::vector<float>*   m_NotAssociated_Xpos;  	  //!< Local X position of G4 hit not associated to any cluster UNIT:mm
    std::vector<float>*   m_NotAssociated_Ypos; 	  //!< Local Y position of G4 hit not associated to any cluster UNIT:mm
    std::vector<int>*     m_NotAssociated_BarrelEndcap;	  //!< Barrel/EndCap index (-2 for endcapC, 2 for endcapA, 0 for barrel) UNIT:index
    std::vector<int>*     m_NotAssociated_LayerDisk; 	  //!< Layer information (0, 1 or 2) UNIT:index
    std::vector<int>*     m_NotAssociated_EtaModule; 	  //!< Eta index of the module with not associated G4 hit UNIT:index
    std::vector<int>*     m_NotAssociated_PhiModule; 	  //!< Phi index of the module with not associated G4 hit UNIT:index
    std::vector<float>*   m_NotAssociated_IncidentAngle;  //!< Not associated G4 hit incidence angle in phi direction UNIT:rad
    std::vector<float>*   m_NotAssociated_IncidentAngle2; //!< Not associated G4 hit incidence angle in eta direction UNIT:rad
    std::vector<float>*   m_NotAssociated_IncidentAngle_Lorcorr; //!< NotAssociated_IncidentAngle corrected for the Lorentz angle UNIT:rad
    std::vector<float>*	  m_NotAssociated_EnergyLoss; 	  //!< Not associated G4 hit total energy loss UNIT:GeV 
    std::vector<float>*   m_NotAssociated_Time; 	  //!< Not associated G4 hit arrival time UNIT:ns
    std::vector<int>*     m_NotAssociated_PDGParticle; 	  //!< Not associated G4 hit particle PDG code  
    std::vector<int>*     m_NotAssociated_Flag; 	  //!< Not associated G4 hit flag

    std::vector<std::vector<int> >*     m_MultipleAssociated_Index; 	  //!< Not associated G4 hit flag

    //  Variables for NN training
    
    std::vector<int>* m_NN_sizeX; //!< Row width of the cluster UNIT:cells
    std::vector<int>* m_NN_sizeY; //!< Column width of the cluster UNIT:cells
    //   std::vector<std::vector<std::vector<float> > >* m_NN_matrixOfToT;
    std::vector<std::vector<float> >* m_NN_matrixOfToT;
    std::vector<std::vector<float> >* m_NN_vectorOfPitchesY;
    std::vector<int>* m_NN_ClusterPixLayer;
    std::vector<int>* m_NN_ClusterPixBarrelEC;
    std::vector<float>* m_NN_phiBS;
    std::vector<float>* m_NN_thetaBS;
    std::vector<float>* m_NN_etaModule;
    std::vector<bool>* m_NN_useTrackInfo;
    std::vector<int>* m_NN_columnWeightedPosition;
    std::vector<int>* m_NN_rowWeightedPosition;
 
    std::vector<double>* m_NN_localColumnWeightedPosition;
    std::vector<double>* m_NN_localRowWeightedPosition;


    std::vector<std::vector<float> >* m_NN_positionX;
    std::vector<std::vector<float> >* m_NN_positionY;
    std::vector<std::vector<float> >* m_NN_position_idX;
    std::vector<std::vector<float> >* m_NN_position_idY;

    std::vector<std::vector<float> >* m_NN_theta;
    std::vector<std::vector<float> >* m_NN_phi;
    
    
    std::vector<std::vector<int> >* m_NN_pdgid;
    std::vector<std::vector<int> >* m_NN_barcode;

    std::vector<std::vector<float> >* m_NN_momentum;
    std::vector<std::vector<float> >* m_NN_distanceX;
    std::vector<std::vector<float> >* m_NN_distanceY;


    Amg::Vector2D GetDigitalPosition(const InDet::PixelCluster*& pix,
			                  const InDetDD::SiDetectorElement*& EL) const;

   			
};

} // close of namespace

#endif // INDET_PIXELCLUSTERVALIDATIONNTUPLEWRITER_H

