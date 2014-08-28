/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUhggFitter.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////

/*
 *
 * Performs direction fit for a photon pair assumed to come from a H->gg decay
 * Suggested usage:
 *
 *    IPAUhggFitter* m_PAUhggFitter ;
 *    StatusCode sc = toolSvc->retrieveTool( "PAUhggFitter", m_PAUhggFitter );
 * 
 * Assign clusters associated to the two gammas (here "gamma" can come from PhotonCollection 
 * or ElectronCollection); first get clusters from Photon::cluster() or Electron::cluster() 
 * methods,  e.g.:
 *
 *    m_PAUhggFitter->SetEtas( photon1.cluster() , photon2.cluster() ) ; 
 *
 * This also resets all conversion and primary vertex information.
 *
 * Assign primary vertex (if known) with its sigma: if it is unknown, a dummy vertex at Z=0
 * with sigma=56 mm will be used
 *
 *    m_PAUhggFitter->SetPrimVertexInfo(Zvtx,sigmaZvtx) ;
 *
 * If i-th photon (i=1,2) has a conversion associated, pass it to PAUhggFitter: its vertex
 * position will be used as extra point in the fit
 *    
 *    m_PAUhggFitter->SetConv( i , conv ) ;
 *
 * If i-th photon (i=1,2) has a track associated (converted, but vertex not reconstructed)
 * pass it to PAUhggFitter: its perigee position will be used as extra point in the fit
 *    
 *    m_PAUhggFitter->SetTrack( i , trk ) ;
 *
 * Finally call the fit:
 *
 *    float RecoZCalo, RecoZresCalo, RecoEta1Calo, RecoEta2Calo ;
 *    bool status = m_PAUhggFitter->FitPAU( RecoZCalo, RecoZresCalo, RecoEta1Calo, RecoEta2Calo ) ;
 *    
 * (status==false if the fit failed)
 *
 */


#ifndef HGGFITTER_IHGGFITTER_H
#define HGGFITTER_IHGGFITTER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkParameters/TrackParameters.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
//#include "VxVertex/VxCandidate.h"
//#include "CaloEvent/CaloCluster.h"

static const InterfaceID IID_IPAUhggFitter("IPAUhggFitter", 1, 0);

class IPAUhggFitter : virtual public IAlgTool {
  
 public:
  /** Virtual destructor */
  virtual ~IPAUhggFitter(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUhggFitter; };
 
  // public methods:
 
  virtual void SetEtas(const CaloCluster* clus1,const CaloCluster* clus2) =0;
  virtual void SetPrimVertexInfo(float Zinput,float DZinput) =0;
  virtual void SetConversion(unsigned gamma_index, const Trk::VxCandidate* conv) =0;
  virtual void SetTrack(unsigned gamma_index, const Rec::TrackParticle* trk) =0;

  virtual bool FitPAU(float& Zrec, float& ZrecRes, float& eta1rec, float& eta2rec) =0;
  virtual bool getEtaSinglePhoton(float& eta1,float& eta2) const =0;
  virtual bool gimeRZ(int layer, float eta, float& R, float& Z) const =0;
  virtual bool inEMBarrel( float eta ) const =0;
  virtual float EtaFromVertex(int layer, float eta, float Zvtx) const =0;

  enum pointType { PS, Strips, Middle, PrimVtx, Conv } ;

  // old methods...
  virtual void SetEtas(std::vector<float>, std::vector<float>) =0;
  virtual void SetPrimVertexInfo(bool usevtx,float Zinput,float DZinput) =0;
  virtual void SetConvInfo(bool usecv,bool conv1,float rcv1,float zcv1,bool conv2,float rcv2,float zcv2) =0;

};

#endif // HGGFITTER_IHGGFITTER_H
