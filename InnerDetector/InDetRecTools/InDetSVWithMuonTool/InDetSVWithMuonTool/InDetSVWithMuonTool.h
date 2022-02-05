/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// InDetSVWithMuonTool.h - Description
//
/*

  Tool to find a secondary vertex which includes the provided muon

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch

----------------------------------------------------------*/
#ifndef _InDetSVWithMuonTool_H
#define _InDetSVWithMuonTool_H
// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//
#include  "xAODTracking/TrackParticleContainer.h"
#include  "xAODTracking/VertexFwd.h"
#include  "TrkVertexFitterInterfaces/IVertexFitter.h"
//

//


class TH1D;
class TH2D;
class ITHistSvc;
namespace Trk{
  class TrkVKalVrtFitter;
  class IVKalState;
}



  typedef std::vector<double> dvect;

  
//------------------------------------------------------------------------
namespace InDet {

//------------------------------------------------------------------------
  static const InterfaceID IID_ISVWithMuonFinder("ISVWithMuonFinder", 1, 0);

  class ISVWithMuonFinder : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_ISVWithMuonFinder;}
//---------------------------------------------------------------------------
//Interface itself

      virtual const xAOD::Vertex* findSVwithMuon(const xAOD::Vertex & PrimVrt,
			                         const xAOD::TrackParticle * Muon,
		   	 	                 const std::vector<const xAOD::TrackParticle*> & InpTrk) 
						 const =0;
  };




  class InDetSVWithMuonTool : public AthAlgTool, virtual public ISVWithMuonFinder
  {

  public:
       /* Constructor */
      InDetSVWithMuonTool(const std::string& type, const std::string& name, const IInterface* parent);
       /* Destructor */
      virtual ~InDetSVWithMuonTool();


      StatusCode initialize();
      StatusCode finalize();

    

      const xAOD::Vertex* findSVwithMuon(const xAOD::Vertex & PrimVrt,
			                 const xAOD::TrackParticle * Muon,
		   	 	         const std::vector<const xAOD::TrackParticle*> & InpTrk) const;

//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

    private:

      struct Hists {
        StatusCode book (ITHistSvc& histSvc);
        TH1D* m_hb_muonPt{};
        TH1D* m_hb_massPiPi{};
        TH1D* m_hb_nvrt2{};
        TH1D* m_hb_nseltrk{};
        TH1D* m_hb_totmass{};
        TH1D* m_hb_impact{};
        TH1D* m_hb_impactR{};
        TH1D* m_hb_ntrkjet{};
        TH1D* m_hb_impactZ{};
        TH1D* m_hb_signif3D{};
        TH1D* m_hb_r2d{};
        TH1D* m_hb_r2dc{};
      };
      std::unique_ptr<Hists> m_h;
      Hists& getHists() const;

      double m_w_1{};
      long int m_iflag{};

      SimpleProperty<int>    m_Robustness;
      SimpleProperty<double> m_MassForConstraint;
      std::vector<double>    m_MassInputParticles;



      long int m_CutSctHits{};
      long int m_CutPixelHits{};
      long int m_CutSiHits{};
      long int m_CutBLayHits{};
      long int m_CutSharedHits{};
      double m_CutPt{};
      double m_CutZVrt{};
      double m_CutA0{};
      double m_CutChi2{};
      double m_SecTrkChi2Cut{};
      double m_ConeForTag{};
      double m_Sel2VrtChi2Cut{};
      double m_Sel2VrtSigCut{};
      double m_TrkSigCut{};
      double m_A0TrkErrorCut{};
      double m_ZTrkErrorCut{};

      bool m_FillHist{};

      bool m_existIBL{};

      long int m_RobustFit{};

      double m_Rbeampipe{};
      double m_RlayerB{};
      double m_Rlayer1{};
      double m_Rlayer2{};
      double m_Rlayer3{};


      ToolHandle < Trk::IVertexFitter >       m_fitterSvc;
      Trk::TrkVKalVrtFitter*   m_fitSvc{};
 

      double m_massPi {};
      double m_massP {};
      double m_massE{};
      double m_massB{};

//
//   Private technical functions
//
//
      xAOD::Vertex*  MuonVrtSec(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                const xAOD::Vertex                           & PrimVrt,
                                const xAOD::TrackParticle                    * Muon,
                                std::vector<const xAOD::TrackParticle*>      & ListSecondTracks) const;
//
//


      StatusCode CutTrk(double,double, double , double , double , 
                          long int ,long int ,long int , long int ) const;
      double ConeDist(const AmgVector(5) & , const TLorentzVector & ) const;
//
      int FindMaxAfterFirst( std::vector<double>& Chi2PerTrk) const;

      void  SelGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                const xAOD::Vertex                           & PrimVrt,
	                        const xAOD::TrackParticle                    * Muon,
                                std::vector<const xAOD::TrackParticle*>& SelectedTracks) const;


      double VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& VrtErr,double& Signif ) const;
 

      double FitCommonVrt(std::vector<const xAOD::TrackParticle*>& ListSecondTracks,
                                  const xAOD::Vertex        & PrimVrt,
 	                          const xAOD::TrackParticle * Muon,
	                          Amg::Vector3D             & FitVertex,
                                  std::vector<double>       & ErrorMatrix,
	                          TLorentzVector            & Momentum,
		        std::vector< std::vector<double> >  & TrkAtVrt) const;


      template <class Trk>
      void RemoveEntryInList(std::vector<const Trk*>& , int) const;
      template <class Trk>
      void RemoveDoubleEntries(std::vector<const Trk*>& ) const;

      TLorentzVector TotalMom(const std::vector<const xAOD::TrackParticle*>& InpTrk) const;
      double pTvsDir(const Amg::Vector3D &Dir, const std::vector< double >& InpTrk) const; 



      void GetTrkWithMuonVrt(std::vector<const xAOD::TrackParticle*>  & SelectedTracks,
                        const xAOD::Vertex                       & PrimVrt,
 	                const xAOD::TrackParticle                * Muon,
 	                std::vector<const xAOD::TrackParticle*>  & ListSecondTracks) const;


     StatusCode VKalVrtFitFastBase(const std::vector<const xAOD::TrackParticle*>& listPart,
                                   Amg::Vector3D& Vertex,
                                   Trk::IVKalState& istate) const;


      StatusCode VKalVrtFitBase(const std::vector<const xAOD::TrackParticle*> & listPart,
                                Amg::Vector3D&                 Vertex,
                                TLorentzVector&                Momentum,
                                long int&                      Charge,
                                std::vector<double>&           ErrorMatrix,
                                std::vector<double>&           Chi2PerTrk,
                                std::vector< std::vector<double> >& TrkAtVrt,
                                double& Chi2,
                                Trk::IVKalState& istate,
                                bool ifCovV0) const;
  };


  template <class Trk>
  void InDetSVWithMuonTool::RemoveEntryInList(std::vector<const Trk*>& ListTracks, int Outlier) const
  {
    if(Outlier < 0 ) return;
    if(Outlier >= (int)ListTracks.size() ) return;
    ListTracks.erase( ListTracks.begin()+Outlier);
  }     

  template <class Trk>
  void InDetSVWithMuonTool::RemoveDoubleEntries(std::vector<const Trk*>& ListTracks) const
  {
    typename std::vector<const Trk*>::iterator   TransfEnd;
    sort(ListTracks.begin(),ListTracks.end());
    TransfEnd =  unique(ListTracks.begin(),ListTracks.end());
    ListTracks.erase( TransfEnd, ListTracks.end());
  }     


}  //end namespace

#endif
