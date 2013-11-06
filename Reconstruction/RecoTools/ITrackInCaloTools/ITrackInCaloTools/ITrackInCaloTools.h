/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRACKINCALOTOOLS_H
#define ITRACKINCALOTOOLS_H
//
#include "GaudiKernel/AlgTool.h"
#include <vector>

class CaloCell;
class P4EEtaPhiM;

namespace Trk { class Track; }
class INavigable4Momentum;
//
static const InterfaceID IID_ITrackInCaloTools("ITrackInCaloTools", 1, 0);

namespace TICT { enum CaloLayer {ps=0,em1=1,em2=2,em3=3,tile1=4,tile2=5,tile3=6,hec0=7,hec1=8,hec2=9,hec3=10}; }
using namespace TICT;

/** @class ITrackExtrapolatorToCalo

    Tool used to calculate calorimetric isolation for muons and muon energy loss
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackInCaloTools
    Implemented in TrackInCaloTools/TrackInCaloTools

    @author Konstantinos.Bachas@cern.ch, Samira.Hassani@cern.ch, Bruno.Lenzi@cern.ch
*/
class ITrackInCaloTools : virtual public IAlgTool
{
  public:

    virtual ~ITrackInCaloTools(){}
    static const InterfaceID&	interfaceID() { return IID_ITrackInCaloTools; };
    
    // Get energy (Et) around the track, subtracting etCore
    virtual double getIsolationEnergy(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get energy for range of layers
    virtual double getMeasuredEnergy(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0; // if isEt = false, get E
    // Energy for several cones
    virtual bool getMeasuredEnergy(const INavigable4Momentum* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0;
    // Get the core energy, corresponding to the energy deposited by a muon track
    virtual double getEcore(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0;
    // Cells around the track per layer, subtracting the ones used for eCore
    virtual std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get the cells used on the calculation of Ecore
    virtual std::vector<const CaloCell*> getCellsForEcore(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get cells around the track for all layers
    virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get cells around the track for all layers, "rectangular cone"
    virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track,const double deta, const double dphi, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get neta x nphi cells around the track for given layer
    virtual std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const INavigable4Momentum *track, CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false) =0;
    // Get cells on track (one per layer)
    virtual std::vector<const CaloCell*> getCellsOnTrack(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)=0;
    // Get all cells crossed by track (can be more than one per layer)
    virtual std::vector<const CaloCell*> getCellsCrossedByTrack(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    // Get the fraction of the path crossed by the track which is covered by the cells collected
    virtual std::vector<double> getWeightsForCellsCrossed(const INavigable4Momentum* track, bool useLooseCut) =0;
    // Get the % of the path crossed inside the given cell. If one or both the extrapolation fails, return 1 if it is the cell on track
    virtual double getPathInsideCell(const INavigable4Momentum* track, const CaloCell *cell) =0;
    // Get sum( Et**EtExp * dR**dRExp )/sum(Et) within a cone of dR    
    virtual double getIsolationWidth(const INavigable4Momentum* track,const double dR, const double EtExp, const double dRExp, bool negRemoval=true, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) = 0;
    
    //-----------------------------------------------------------------------------------------
    // same for Trk::Track
    virtual double getIsolationEnergy(const Trk::Track* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual double getMeasuredEnergy(const Trk::Track* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0;
    virtual bool getMeasuredEnergy(const Trk::Track* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0;
    virtual double getEcore(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt=true) =0;
    virtual std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::Track* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<const CaloCell*> getCellsForEcore(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track*,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track*,const double deta,const double dphi, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::Track *track, CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false) =0;
    virtual std::vector<const CaloCell*> getCellsOnTrack(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) =0;
    virtual std::vector<double> getWeightsForCellsCrossed(const Trk::Track* track, bool useLooseCut) =0;
    virtual double getPathInsideCell(const Trk::Track *track, const CaloCell *cell) =0;
    virtual double getIsolationWidth(const Trk::Track* track,const double dR, const double EtExp, const double dRExp, bool negRemoval=true, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) = 0;

    virtual std::vector<double> getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false) = 0;
    
    // Get the neta X nphi neighbours of the cell
    virtual std::vector<const CaloCell*> getNeighbours(const CaloCell*, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false) =0;
    // Get the cell layer according to the internal convention (CaloLayer)
    virtual int getLayer(const CaloCell *cell) =0;

    virtual void print_EcoreDefinition() =0;
    virtual void setEcoreDefinition(std::vector<std::string> eCoredef) =0;
    virtual double getNoiseThreshold() =0;
    virtual void setNoiseThreshold(double threshold) =0;
    
};
#endif //ITRACKINCALOTOOLS_H
