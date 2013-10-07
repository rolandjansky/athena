/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// SiResidualValidationNtupleHelper.h
//   Header file for SiResidualValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef INDET_SIRESIDUALVALIDATIONNTUPLEHELPER_H
#define INDET_SIRESIDUALVALIDATIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "InDetRawData/PixelRDO_Container.h"        // typedef

class TTree;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class ISiliconConditionsSvc;
template <class Svc> class ServiceHandle;
namespace InDetDD{
    class SiDetectorElement;
}
namespace Trk{
    class MeasurementBase;
}


namespace InDet {
    class PixelCluster;

/** @class SiResidualValidationNtupleHelper
    This validation helper tool writes Pixel and SCT specific data into the ntuple
    @author Sebastian.Fleischmann@cern.ch
*/

class SiResidualValidationNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {

    public:
        // standard AlgToolmethods
        SiResidualValidationNtupleHelper(const std::string&,const std::string&,const IInterface*);
        ~SiResidualValidationNtupleHelper();

        // standard Athena methods
        StatusCode initialize();
        StatusCode finalize();

        // fill data
        virtual StatusCode fillMeasurementData (const Trk::MeasurementBase*,
                const Trk::TrackParameters*, const int& detectorType,
                const bool& isOutlier) const;
        virtual StatusCode fillHoleData (const Trk::TrackStateOnSurface& tsos,
                const int& detectorType) const;

        // add items to the ntuple and configure the helper tool:
        // should be called once (per detector type)
        // by the steering tool (Trk::IValidationNtupleTool)
        virtual StatusCode addNtupleItems (
                TTree* tree,
                const int& detectorType ) const;

        // reset ntuple variables
        virtual StatusCode resetVariables (const int& detectorType ) const;

    private:
	// variables
	const PixelID* m_pixelid;                   //!< pixel id helper
	const SCT_ID*  m_sctid;                     //!< SCT id helper
	bool m_WriteDetailedPixelInformation;       //!< jobOption: fill pixel info: ToT, Charge, LVL1A, eta, phi
	std::string m_rdocontainername;  	    //!< jobOption: name of container with RDOs
        mutable const PixelRDO_Container* m_rdocontainer;   //!< container of pixel RDO (used by if pixel detailed information is requested)  


	ServiceHandle< ISiliconConditionsSvc > m_PixelConditionsSvc; // pixel conditions

        // --- ntuple items. Please document here so they can be automatically parsed for doxygen/wiki tables ---
	mutable std::vector<int>*   m_PixDetType; //!< Layer information (0, 1, 2 for barrel, 100, 101, 102 for endcap A side, -100, -101, -102 for endcap C side) it is 0 by default 
	mutable std::vector<int>*   m_PixHasGanged; //!< if the cluster contains ganged pixels UNIT:1:yes
	mutable std::vector<int>*   m_PixIsFake;   //!< if the cluster is marked as fake UNIT:1:yes
	mutable std::vector<int>*   m_PixIsSplit;   //!< if the cluster is marked as split UNIT:1:yes
	mutable std::vector<float>* m_PixSplitProb1;//!< probability to be a single particle cluster
	mutable std::vector<float>* m_PixSplitProb2;//!< probability to be a double particle cluster
	mutable std::vector<int>*   m_PixLVL1A;    //!< lower level1 accept among the pixel of the cluster 
	mutable std::vector<int>*   m_PixToT;      //!< ToTal time over threshold (ToT) of the cluster UNIT:??
	mutable std::vector<float>* m_PixCharge;   //!< total charge of the cluster UNIT:??
	mutable std::vector<float>* m_PixTLorPhi;  //!< tangent of the lorentz angle (local x direction) UNIT:1
	mutable std::vector<float>* m_PixTLorEta;  //!< tangent of the lorentz angle (local y direction - should be 0!) UNIT:1
	mutable std::vector<float>* m_PixBiasVolt; //!< Bias voltage (from conditions service) UNIT:V ?
	mutable std::vector<float>* m_PixTemp;     //!< Temperature (from conditions service) UNIT:K ?
	mutable std::vector<float>* m_PixDepVolt;  //!< Depletion voltage (from conditions service) UNIT:kV?
	mutable std::vector<float>* m_PixDigResPhi; //!< local phi residual obtained using the cluster digital coordinates UNIT:mm
	mutable std::vector<float>* m_PixDigResEta; //!< local eta residual obtained from using the cluster digital coords UNIT:mm
	mutable std::vector<float>* m_PixErrPhi;    //!< PixelCluster position error (local x) UNIT:mm
	mutable std::vector<float>* m_PixErrEta;    //!< PixelCluster position error (local y) UNIT:mm
	mutable std::vector<float>* m_PixEta;       //!< pseudorapidity calculated from module position
	mutable std::vector<float>* m_PixGloX;      //!< global X position of the cluster UNIT:mm
	mutable std::vector<float>* m_PixGloY;      //!< global Y position of the cluster UNIT:mm
	mutable std::vector<float>* m_PixGloZ;      //!< global Z position of the cluster UNIT:mm
	mutable std::vector<float>* m_PixLocX;      //!< local x coordinate of the cluster UNIT:mm
	mutable std::vector<float>* m_PixLocY;      //!< local y coordinate of the cluster UNIT:mm
	mutable std::vector<int>*   m_PixEtaModule; //!< eta index of the module associated to the cluster
	mutable std::vector<int>*   m_PixPhiModule; //!< phi index of the module associated to the cluster
	mutable std::vector<float>* m_PixFirstRow;  //!< lower row of the pixels contained in the cluster UNIT:index
	mutable std::vector<float>* m_PixFirstCol;  //!< lower column of the pixels contained in the cluster UNIT:index
	mutable std::vector<float>* m_PixDeltaPhi;  //!< Pixel cluster size phi, in cell units UNIT:cells
	mutable std::vector<float>* m_PixDeltaEta;  //!< Pixel cluster size eta, in cell units UNIT:cells
	mutable std::vector<int>*   m_PixDeltaRow;  //!< Pixel cluster size phi or row UNIT:mm 
	mutable std::vector<int>*   m_PixDeltaCol;  //!< Pixel cluster size eta or column UNIT:mm
	mutable std::vector<float>* m_PixOmegaPhi;  //!< Pixel charge sharing variable, phi clustering UNIT:1
	mutable std::vector<float>* m_PixOmegaEta;  //!< Pixel charge sharing variable, eta clustering UNIT:1
	mutable std::vector<float>* m_PixTrkAngle;  //!< track incidence angle on the module (rphi plane) in local reference frame UNIT:rad
	mutable std::vector<float>* m_PixTrkThetaI; //!< track incidence angle on the module (eta-normal plane) in local reference frame UNIT:rad
	mutable std::vector<float>* m_PixTrkEta;    //!< track incidence pseudorapidity in local module reference frame
	mutable std::vector<float>* m_PixTrkPt;     //!< track transverse momentum at module
	mutable std::vector<float>* m_PixTrkQ;      //!< charge of the associated track UNIT:+/-1
	mutable std::vector<float>* m_PixTrkClusGroupSize;  //!< total size of the cluster UNIT:cells
	mutable std::vector<std::vector<float> >* m_PixChargeList;//!< Charge of the pixel UNIT:electrons 
	mutable std::vector<std::vector<int> >*   m_PixToTList; //!< Time over Threshold of the pixel
	mutable std::vector<std::vector<int> >*   m_PixLVL1AList; //!< Level 1 accept of the pixel
	mutable std::vector<std::vector<int> >*   m_PixEtaIndex; //!< Cluster position converted into pixel column index (eta direction) UNIT:index
	mutable std::vector<std::vector<int> >*   m_PixPhiIndex; //!< Cluster position converted into pixel row index (phi direction) UNIT:index

        // --- ntuple items for Pixel Holes - modules with hits not on track ---
	mutable std::vector<float>*  m_PixHoleGloX;  //!< global X position of the hole UNIT:mm
	mutable std::vector<float>*  m_PixHoleGloY;  //!< global Y position of the hole UNIT:mm
	mutable std::vector<float>*  m_PixHoleGloZ;  //!< global Z position of the hole UNIT:mm
	mutable std::vector<float>*  m_PixHoleLocX;  //!< local x position of the hole on the module UNIT:mm
	mutable std::vector<float>*  m_PixHoleLocY;  //!< local y position of the hole on the module UNIT:mm
	mutable std::vector<int>*    m_PixHoleEtaModule;//!< eta index of the module containing the hole
	mutable std::vector<int>*    m_PixHolePhiModule; //!< phi index of the module containing the hole
	mutable std::vector<float>*  m_PixHoleLayer; //!< layer index of the module containing the hole
	mutable std::vector<float>*  m_PixHoleECBarrel;      //!< barrel/endcap flag for the hole
	mutable std::vector<int>*    m_PixHoleClNearstSize;  //!< total size of the nearest cluster to the hole UNIT:cells
	mutable std::vector<float>*  m_PixHoleLocXClNearst;  //!< nearest cluster local x UNIT:mm
	mutable std::vector<float>*  m_PixHoleLocYClNearst;  //!< nearest cluster local x UNIT:mm
	mutable std::vector<float>*  m_PixHoleClRowClNearst; //!< row index of the nearest cluster
	mutable std::vector<float>*  m_PixHoleClColClNearst; //!< column index of the nearest cluster
	mutable std::vector<float>*  m_PixHoleGloZNearst;    //!< global z of the nearest cluster
	mutable std::vector<float>*  m_PixHoleDeltaRowNearst;//!< nearest cluster size, in cell units UNIT:cells
	mutable std::vector<float>*  m_PixHoleDeltaColNearst;//!< nearest cluster size, in cell units UNIT:cells

        // --- ntuple items for SCT clusters on track ---
	mutable std::vector<int>*   m_SctDetType;   //!< detector part (0 for barrel, +/-2 for endcaps)
	mutable std::vector<int>*   m_SctLayerDisk; //!< index of layer or disk containing the cluster
	mutable std::vector<int>*   m_SctEtaModule; //!< eta index of module containing the cluster
	mutable std::vector<int>*   m_SctPhiModule; //!< phi index of module containing the cluster
	mutable std::vector<int>*   m_SctSide;      //!< side of module containing the cluster
	mutable std::vector<float>* m_SctTLorPhi;   //!< tangent of the lorentz angle (local x direction) UNIT:1
	mutable std::vector<float>* m_SctTLorEta;   //!< tangent of the lorentz angle (local y direction, should be 0) UNIT:1
	mutable std::vector<float>* m_SctEta;       //!< pseudorapidity calculated from module position
	mutable std::vector<float>* m_SctDeltaPhi;  //!< SCT cluster size in strip units UNIT:strips
	mutable std::vector<float>* m_SctLocX;      //!< Local X of track
        mutable std::vector<float>* m_SctLocY;      //!< Local Y of track
	mutable std::vector<float>* m_SctIncAng;    //!< incidence angle in plane perp. to strip direction
	mutable std::vector<float>* m_SctTrkAngle;  //!< track incidence angle on the module in local reference frame UNIT:rad
	mutable std::vector<float>* m_SctTrkQ;      //!< reconstructed track charge
	mutable std::vector<float>* m_SctErr;       //!< ClusterOnTrack's error in local X coordinate UNIT:mm
	mutable std::vector<float>* m_SctTrkClusGroupSize; //!< SCT total cluster size

	mutable long m_lvl1TriggerType;  //!< level1 trigger stream type UNIT:8-bit

	// methods
	double calcPull( const double residual, const double locMesCov,
			const double locTrkCov ) const;

	Amg::Vector2D GetDigitalPosition(const InDet::PixelCluster*& pix,
			const InDetDD::SiDetectorElement*& EL) const;

	int findAnglesToWaferSurface ( double (&vec)[3], double &sinAlpha, 
				       const InDetDD::SiDetectorElement*& element, double &theta, double &phi ) const; 


};


} // end of namespace

#endif // INDET_SIRESIDUALVALIDATIONNTUPLEHELPER_H
