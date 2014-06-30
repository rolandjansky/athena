/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   ITrackTools.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Track tools interface.
 *
 * Created in February 2013, based on TrackInCaloTools.
 */

#ifndef ITrackTools_H
#define ITrackTools_H
//
#include "GaudiKernel/AlgTool.h"
#include <vector>
#include "Scintillator.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloID.h"
#include "AthContainers/ConstDataVector.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "TrkParameters/TrackParameters.h"


class CaloCell;
class P4EEtaPhiM;
class CaloCell_ID;

namespace Trk{
    class Track;
} // NAMESPACE
class INavigable4Momentum;
namespace Rec{
class TrackParticle;
}

static const InterfaceID IID_ITrackTools("ITrackTools", 1, 0);

namespace TICT { enum CaloLayer {ps=0,em1=1,em2=2,em3=3,tile1=4,tile2=5,tile3=6,hec0=7,hec1=8,hec2=9,hec3=10}; }
using namespace TICT;
using namespace std;

class ConeDefinition;

//========================================
class ITrackTools:virtual public IAlgTool{
//========================================
    public:

        virtual ~ITrackTools(){}
        static const InterfaceID& interfaceID(){ return IID_ITrackTools; };

        typedef std::vector<const CaloCell*> ListOfCells;

        // METHOD FOR FILTERING ALGORITHM
        virtual void getCellsWithinConeAroundTrack(const Trk::Track* track, 
                                                   const CaloCellContainer* input,
                                                   ConstDataVector<CaloCellContainer>* output, 
                                                   double cone,
                                                   bool includelar) = 0;
        virtual void getCellsWithinConeAroundTrack(const Rec::TrackParticle* track, 
                                                   const CaloCellContainer* input,
                                                   ConstDataVector<CaloCellContainer>* output, 
                                                   double cone,
                                                   bool includelar) = 0;
        virtual std::vector< double > getXYZEtaPhiInCellSampling(const Trk::Track* track, const CaloCell *cell) = 0;
        virtual std::vector< double > getXYZEtaPhiInCellSampling(double exit[], const Trk::Track* track, const CaloCell *cell) = 0;
        virtual std::vector< double > getXYZEtaPhiInCellSampling(const Trk::Track* track, int sampling) = 0;

        // OTHER METHODS
        virtual std::vector< std::pair<double, double> > getEtaPhiPerLayer(const Trk::TrackParameters* track) = 0;
        virtual std::vector< std::pair<double, double> > getEtaPhiPerLayer(const Trk::Track* track) = 0;
        virtual std::vector< std::pair<double, double> > getEtaPhiPerLayer(const INavigable4Momentum* track) = 0;

        virtual std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const Trk::TrackParameters* track) = 0;
        virtual std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const Trk::Track* track) = 0;
        virtual std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const INavigable4Momentum* track) = 0;

        virtual std::vector<const Trk::TrackParameters*> getExit(const Trk::TrackParameters *track) = 0;
        virtual std::vector<const Trk::TrackParameters*> getEntrance(const Trk::TrackParameters *track) = 0;

        virtual std::vector<const Trk::TrackParameters*> getExit(const Trk::Track *track) = 0;
        virtual std::vector<const Trk::TrackParameters*> getEntrance(const Trk::Track *track) = 0;

        virtual std::vector<const Trk::TrackParameters*> getExit(const INavigable4Momentum *track) = 0;
        virtual std::vector<const Trk::TrackParameters*> getEntrance(const INavigable4Momentum *track) = 0;

        virtual const Trk::TrackParameters* getExit(const Trk::TrackParameters *track, int layer) = 0;
        virtual const Trk::TrackParameters* getEntrance(const Trk::TrackParameters *track, int layer) = 0;

        virtual const Trk::TrackParameters* getExit(const Trk::Track *track, int layer) = 0;
        virtual const Trk::TrackParameters* getEntrance(const Trk::Track *track, int layer) = 0;

        virtual const Trk::TrackParameters* getExit(const INavigable4Momentum *track, int layer) = 0;
        virtual const Trk::TrackParameters* getEntrance(const INavigable4Momentum *track, int layer) = 0;

        virtual double getExit(const Trk::TrackParameters *track, int layer, int pos) = 0;
        virtual double getEntrance(const Trk::TrackParameters *track, int layer, int pos) = 0;

        virtual double getExit(const Trk::Track *track, int layer, int pos) = 0;
        virtual double getEntrance(const Trk::Track *track, int layer, int pos) = 0;

        virtual double getExit(const INavigable4Momentum *track, int layer, int pos) = 0;
        virtual double getEntrance(const INavigable4Momentum *track, int layer, int pos) = 0;

        virtual double getPathFromExtra(const CaloCell* cell, std::vector<Trk::TrackParameters*> track_extrapolations) = 0;
        virtual double getPathFromExtra(const Trk::Track* i_trk, const CaloCell* i_cell) = 0;

        //====================
        // INavigable4Momentum
        //====================
        virtual double getIsolationEnergy(const INavigable4Momentum* track, const double dR,
                                  CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut) = 0;
        virtual double getMeasuredEnergy(const INavigable4Momentum* track, const double dR,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                 bool useLooseCut = false, bool isEt = true) = 0;
        virtual bool getMeasuredEnergy(const INavigable4Momentum* track, const std::vector<double>& dR, std::vector<double>& energies,
                               CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false, bool isEt = true) = 0;
        virtual double getEcore(const INavigable4Momentum* track, CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                        bool useLooseCut = false, bool isEt = true) = 0;
        virtual std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const INavigable4Momentum* track,const double dR, 
                                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                                         bool useLooseCut) = 0;
        virtual std::vector<const CaloCell*> getCellsForEcore(const INavigable4Momentum* track,
                                                      CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                      bool useLooseCut = false) = 0;
        virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track, const double dR,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false) = 0;
        virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track, const double deta, const double dphi, 
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false) = 0;

	//virtual std::vector<const CaloCluster*> getClustersAroundTrack(double cone, const INavigable4Momentum* track);
	virtual std::vector<const CaloCluster*> getClustersAroundTrack(double cone, const Rec::TrackParticle* track) = 0;
	virtual bool isClusterAroundTrack(double cone, const CaloCluster* cluster, const Rec::TrackParticle* track) = 0;
	//virtual bool isClusterAroundTrack(double cone, const CaloCluster* cluster, const INavigable4Momentum* track);
        virtual std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const INavigable4Momentum *track,
                                                                CaloLayer layer, int neta, int nphi, int dir_eta = 0, int dir_phi = 0,
                                                                bool useLooseCut = false) = 0;
        virtual std::vector<const CaloCell*> getCellsOnTrack(const INavigable4Momentum* track,
                                                     CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                     bool useLooseCut = false) = 0;
        virtual std::vector<const CaloCell*> getCellsCrossedByTrack(const INavigable4Momentum* track,
                                                            CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                            bool useLooseCut = false) = 0;
        virtual std::vector<double> getWeightsForCellsCrossed(const INavigable4Momentum* track, bool useLooseCut) = 0;
        virtual double getPathInsideCell(const INavigable4Momentum *track, const CaloCell *cell) = 0;

//         virtual double getCBNTActivePathInsideCell(const INavigable4Momentum *track, const CaloCell *cell) = 0;
        virtual double getCBNTPathInsideCell(const Trk::Track *track, const CaloCell *cell) = 0;
//         virtual double getCBNTActivePathInsideCell(const Trk::Track *track, const CaloCell *cell) = 0;
        virtual double getIsolationWidth(const INavigable4Momentum* track,
                                 const double dR, const double EtExp, const double dRExp, bool negRemoval = true,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false) = 0;

        //=======================
        // METHODS FOR Trk::Track
        //=======================
        virtual std::vector<const CaloCell*> getAssociatedCells(const Trk::Track* track, CaloLayer first_layer, CaloLayer last_layer) = 0;
        virtual double getIsolationEnergy(const Trk::Track* track, const double dR,
                                  CaloLayer first_layer, CaloLayer last_layer,
                                  bool useLooseCut) = 0;
        virtual double getMeasuredEnergy(const Trk::Track* track, const double dR,
                                 CaloLayer first_layer = ps,  CaloLayer last_layer = hec3,
                                 bool useLooseCut = false, bool isEt = true) = 0;
        virtual bool getMeasuredEnergy(const Trk::Track* track,const std::vector<double>& dR, std::vector<double>& energies,
                               CaloLayer first_layer = ps,  CaloLayer last_layer = hec3,
                               bool useLooseCut = false, bool isEt = true) = 0;
        virtual double getEcore(const Trk::Track* track,
                        CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                        bool useLooseCut = false, bool isEt = true) = 0;
        virtual std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::Track* track, const double dR,
                                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                                         bool useLooseCut) = 0;
        virtual std::vector<const CaloCell*> getCellsForEcore(const Trk::Track* track,
                                                      CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                      bool useLooseCut = false) = 0;
        virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track, const double dR, 
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3, 
                                                                       bool useLooseCut = false) = 0;
        virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track, const double deta,const double dphi,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false) = 0;
        virtual std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::Track *track,
                                                                CaloLayer layer, int neta, int nphi, int dir_eta = 0, int dir_phi = 0,
                                                                bool useLooseCut = false) = 0;
        virtual std::vector<const CaloCell*> getCellsOnTrack(const Trk::Track* track,
                                                     CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false) = 0;
        virtual std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::Track* track,
                                                             CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                             bool useLooseCut = false) = 0;
        virtual std::vector<double> getWeightsForCellsCrossed(const Trk::Track* track, bool useLooseCut) = 0;
        virtual double getPathInsideCell(const Trk::Track *track, const CaloCell *cell) = 0;
        virtual double getIsolationWidth(const Trk::Track* track,const double dR, const double EtExp, const double dRExp, bool negRemoval = true,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false) = 0;

        //=====================
        // SCINTILLATOR METHODS
        //=====================
//         virtual float getActivePath(const Trk::Track* i_trk, const CaloCell* i_cell) = 0;
//         virtual float getActivePath(const Trk::Track* i_trk, const CaloCell* i_cell, 
//                                     std::vector<const Scintillator*> scintillators, short& N_scint) = 0;
        virtual int retrieveIndex(int sampling, float eta) = 0;
        virtual int retrieveIndex(const CaloCell* cell) = 0;
//         virtual void getScintCoordinates(const CaloCell* cell, std::vector<const Scintillator*>& scintillators) = 0;

        //==================
        // REMAINING METHODS
        //==================
        // GET ISOLATION ENERGY
        virtual std::vector<double> getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR,
                                               CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut) = 0;

//         // MEASURE ENERGY MEASUREMENT
//         virtual std::pair<double,double> measurement(CaloCell_ID::CaloSample sample, const Trk::TrackParameters& parm) = 0;
// 
//         // COMPUTE E, ET, ASSOCIATED CELLS AND CELLS ON TRACK
//         virtual StatusCode getEnergyAndCells(const Trk::TrackParameters* track,
//                                      std::vector<double> conesize,
//                                      std::vector< std::vector<double> > &etot,
//                                      std::vector< std::vector<ListOfCells> > &usedCells, 
//                                      bool useLooseCut = false, bool isEt = true) = 0;

        // UTILITIES
        virtual int getLayer(const CaloCell *cell) = 0;
        virtual void setNoiseThreshold(double threshold) = 0;
        virtual double getNoiseThreshold() = 0;


        //Get Energy from LAr cone; Added by Archil
        virtual double getEnergyLArCone(const Trk::Track* track, double cone)=0;

};
#endif //ITrackTools_H
