// Dear emacs, this is -*- c++ -*

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file:   BPhysHelper.h
 *  @author: Daniel Scheirich <daniel.scheirich@cern.ch>
 *
 *  @brief:  B-physcis xAOD helpers.
 *
 *  This class provides interface to the basic B-physics augmemtation of
 *  the xAOD::Vertex, i.e.:
 *    - refitted track momenta
 *    - links to muons
 *    - links to other vertices
 *    - links to (some) refitted PV
 *    - transverse distance (and errors)
 *    - impact parameters (and errors)
 *    - pT error
 *    
 *  
 *  In addition, it provides methods that convert covariance matrix from its 
 *  native xAOD form (i.e. vector<float>) into the matrix form represented by 
 *  the TMatrixTSym<double> class
 * 
 *  To access hypothesis-specific augmentation (e.g. invariant mass, lifetime, etc)
 *  use BPhysHypoHelper.
 *
 *  @note
 *  The class caches covariance matrix, refitted track momenta, pointers to muons
 *  and pointers to other vertices in order to speed up access when getter methods
 *  are called repeatedly
 *
 *  Usage example:
 *
 *  @code
 *    void myFunction(xAOD::Vertex* vtx) {
 *      // Let "vtx" be some xAOD::Vertex created by the b-physics software
 *      // We gain access to augmentations through the helper class:
 *      xAOD::BPhysHelper bVtx(vtx);
 *
 *      std::cout << "Refitted pT of the 1st track: ";
 *      if(bVtx.nRefTrks()>0) {
 *        std::cout <<  bVtx.refTrk(0).Pt();
 *      }
 *      std::cout << std::endl;
 *
 *    }
 *  @endcode
 *
 */  

#ifndef XAOD_BPHYSHELPER_H
#define XAOD_BPHYSHELPER_H

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"


#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixTSym.h"

#include <assert.h>


/** Base class for the B-physcis xAOD helpers */
namespace xAOD {

  class BPhysHelper {

    public:
      
      /************************************************************************/
      /** @brief: Main constructor 
       *
       *  The Main constructor.
       *  BPhysHelper does NOT take ownership of the class "b"
       *
       *  @param[in] b   Pointer to the xAOD::Vertex
       */
      
      BPhysHelper(const xAOD::Vertex* b) :
        m_b(b),
        m_covCached(false),
        m_cachedCov(0),
        m_refTracksCached(false),
        m_cachedRefTracks(0),
        m_muonsCached(false),
        m_electronsCached(false),
        m_cachedMuons(0),
        m_cachedElectrons(0),
        m_precedingVerticesCached(false),
        m_cachedPrecedingVertices(0),
        m_cascadeVerticesCached(false),
        m_cachedCascadeVertices(0)
      { 
        assert(m_b!=0); // sanity check: m_b must not be null
      }

      /************************************************************************/
      /** Getter method for the cached xAOD::Vertex 
       *  @returns: cached xAOD::Vertex
       */
      
      const xAOD::Vertex* vtx() const { return m_b; }
      
      /************************************************************************/
      /** @brief: Returns full covariance matrix
       *
       *  Converts native covariance matrix stored as a std::vector<float> into 
       *  a symmetric matrix-form represented by TMatrixTSym<double> class.
       *
       *  @note This method needs to know the number of tracks. If refitted 
       *        tracks are stripped from the verrtex, this method will 
       *        no longer work.
       *
       *  @returns: covariance matrix, 0x0 matrix in case of an error 
       */
      
      const TMatrixTSym<double>& covariance();

      /************************************************************************/
      /** @{ 
       *  Methods providing access to the refitted tracks momenta and related
       *  quantities: 
       */
      
      /** Returns number of stored refitted track momenta
       *  @returns: number of refitted tracks, -1 in case of an error  
       */
      
      int nRefTrks();
      
      /** Returns i-th refitted track 3-momentum
       *  @param[in] index  index of the refitted track 
       *  @returns:  refitted track momentum, (0,0,0) in case of an error 
       */
      
      TVector3 refTrk(const size_t index);

      /** Returns refitted track momenta
       *  @note The std::vector is returned as a reference! It will cease to exist
       *        when BPhysHelper class is destroyed.
       *  @returns:  refitted track, empty vector in case of an error  
       */
      
      const std::vector<TVector3>& refTrks();
      
      /** Returns i-th refitted track as a 4-vector given the mass hypothesis
       *  @param[in] index  index of the refitted track
       *  @param[in] mass   mass hypothesis assigned to the refitted track
       *  @returns:  refitted track 4-momentum, (0,0,0,0) in case of an error 
       */
      
      TLorentzVector refTrk(const size_t index, const float mass);
      
      /** @brief: Returns the original track (charged or neutral) corresponding to 
       *         the i-th refitted track
       *  
       *  In xAOD model, charged and neutral particles are represented by two
       *  distict classes: xAOD::TrackParticle and xAOD::NeutralParticle. Vertex
       *  fitters can use both as input and store charded tracks first (needs checking!!!) in
       *  the list of refitted momenta.
       *
       *  This method returns pointer to the original track corresponding to the i-th
       *  refitted track, i.e. either TrackParticle (if index<nTrackParticles) or
       *  NeutralParticle (if index>=nTrackParticles).
       *
       *  One has to dynamicly cast the returned value to either TrackParticle or 
       *  NeutralParticle. One can use refTrkCharge to determine if the refitted track
       *  is charged or neutral.
       *
       *  @note This method needs access to original TrackParticles (and/or NeutralParticles). 
       *        If the collection(s) of original tracks has(have) been stripped, this method
       *        will return the error value. 
       *
       *  @param[in] index  index of the refitted track
       *  @returns: pointer to the original TrackParticle (if index<nTrackParticles),
       *           pointer to the original NeutralParticle (if index>=nNeutralParticles),
       *           0 in case of an error 
       */
      
      const xAOD::IParticle* refTrkOrigin(const size_t index) const;

      /** Returns perigee 3-momentum of the original track corresponding i-th refitted track 
       *
       *  @note This method needs access to original TrackParticles (and/or NeutralParticles). 
       *        If the collection(s) of original tracks has(have) been stripped, this method
       *        will return the error value. 
       *
       *  @param[in] index    index of the refitted track
       *  @returns:   Original perigee momentum, (0,0,0) in case of an error
       */
      
      TVector3 refTrkOriginP(const size_t index) const;
            
      /** @brief: Returns lorentz vector build from the original track perigee momentum 
       *         and given mass hypothesis
       *
       *  The lorentz vector is created from the perigee parameters of the original track 
       *  (neutral or charged) which corresponds to the i-th refitted track and a given mass
       *  hypothesis
       *
       *  @note This method needs access to original TrackParticles (and/or NeutralParticles). 
       *        If the collection(s) of original tracks has(have) been stripped, this method
       *        will return the error value. 
       *
       *  @param[in] index    index of the refitted track
       *  @param[in] mass     mass hypothesis of the track
       *  @returns:   Original perigee momentum, (0,0,0,0) in case of an error
       */
      
      TLorentzVector refTrkOriginP(const size_t index, const float mass) const;
            
      /** Returns charge of the i-th track 
       *
       *  @note This method needs access to original TrackParticles (and/or NeutralParticles). 
       *        If the collection(s) of original tracks has(have) been stripped, this method
       *        will return the error value. 
       *
       *  @param[in] index    index of the track
       *  @returns:   1. for positive track, -1. for negative, and -9999. on error 
       */
      
      float refTrkCharge(const size_t index) const;
      
      /** Sets refitted track momenta
       *  @param[in] px,py,pz   components of the refitted momenta
       *  @returns: true on success 
       */
      
      bool setRefTrks(const std::vector<float>& px,
                      const std::vector<float>& py,
                      const std::vector<float>& pz);

      /** Sets refitted track momenta
       *  @param[in] refTrks  std::vector of refitted momenta as TVector3
       *  @returns: true on success 
       */
      
      bool setRefTrks(const std::vector<TVector3>& refTrks);

#ifndef XAOD_ANALYSIS

      /** @brief: Sets refitted track momenta
       *
       *  This method uses as input the tracks-at-vertex branch
       *  (std::vector< Trk::VxTrackAtVertex >) in xAOD::Vertex. This method
       *  only works in ATHENA and only for transient xAOD::Vertex instances, which 
       *  still hold the refitted tracks as vxTrackAtVertex branch. Once the vertex 
       *  class is stored in the xAOD file, the vxTrackAtVertex branch is lost.
       *
       *  This method is meant to be called by the vertex fitters immediately
       *  after the vertex is created to convert vxTrackAtVertex branch into 
       *  the persistifiable form.
       *
       *  @returns: true on success 
       */
      
      bool setRefTrks();


#endif // not XAOD_ANALYSIS
      
      /** @} */
      
      /************************************************************************/
      /** @{
       *  Total momentum calculated from refitted tracks
       */
      
      /** @brief: Returns total 3-momentum calculated from the refitted tracks
       *
       *  Returns total 3-momentum calculated from the refitted tracks. 
       *
       *  @note In case that this vertex has some linked cascade vertices, it adds 
       *  the total momentum in the cascade vertices.
       *
       *  @returns: Total momentum, (0,0,0) on error
       *
       */
      
      TVector3 totalP();

      /** Returns total 4-momentum calculated from the refitted tracks given
       *  mass hypotheses for individual tracks
       *
       *  @param[in] masses   invariant mass hypotheses
       *  @returns: Total 4-momentum, (0,0,0,0) on error
       */
      
      TLorentzVector totalP(const std::vector<double>& masses);

      /** Returns pT error
       *
       *  @returns: pT error, -9999999. if augmentation is not available
       */
      
      float ptErr();

      /** Set pT error
       *
       *  @param[in] input value
       *  @returns: true on success
       */
      
      bool setPtErr(const float val);
      
      /** @} */
      
      /************************************************************************/
      /** @{ 
       *  @brief: Methods providing access to the linked muons 
       *  
       *  By default, xAOD::Vertex provides only links to the TrackParticles 
       *  used to build the vertex. Links to muons (e.g. for Jpsi->mu+mu)
       *  are stored using auxiliary branches. 
       */

      /** Returns number of stored muon links
        * @returns: number of muons, -1 in case of an error 
        */
      
      int nMuons();
      int nElectrons();
      
      /** Returns pointer to the i-th linked muon
       *  @param[in] index  index of the linked muon 
       *  @returns: Pointer to the muon, 0 in case of an error 
       */
      
      const xAOD::Muon* muon(const size_t index);
      const xAOD::Electron* electron(const size_t index);
      
      /** Returns linked muons
       *  @returns:  linked muons, empty vector in case of an error 
       */
      
      const std::vector<const xAOD::Muon*>& muons();
      const std::vector<const xAOD::Electron*>& electrons();
      
      /** Set links to muons
       *  @param[in] muons          std::vector of muons to be linked to this vertex
       *  @param[in] muonContainer  collection these muons belong to
       *  @returns: true on success 
       */
      
      bool setMuons(const std::vector<const xAOD::Muon*>& muons, 
                    const xAOD::MuonContainer* muonContainer);
      
      bool setElectrons(const std::vector<const xAOD::Electron*>& electrons, 
                    const xAOD::ElectronContainer* electronContainer);
      /** @} */
      
      /************************************************************************/
      /** @{ 
       *  @brief: Links to preceding vertices
       *  
       *  The B vertices are often build from tracks that have already been 
       *  pre-selected by requirements based on some preceding vertex fit. For
       *  instance, B --> J/psi(mumu)+K vertex is only build using muon tracks that
       *  have been successfully refitted in a J/psi->mumu vertex. 
       *
       *  The following methods enable to retrieve/store links to the vertices that
       *  were preceded this one in the reconstruction flow.
       */

      /** Returns number of preceding vertices
       *
       *  @returns: number of preceding vertices, -1 on error
       */
      
      int nPrecedingVertices();
            
      /** Returns pointer to a preceding vertex
       *
       *  @param[in] index index of the preceding vertex
       *  @returns: pointer to the vertex, 0 on error
       */
      
      const xAOD::Vertex* precedingVertex(const size_t index);

      /** Returns vector of pointers to preceding vertices
       *
       *  @returns: vector of pointers to vertices, empty vector on error
       */
      
      const std::vector<const xAOD::Vertex*>& precedingVertices();
      
      /** Sets links to preceding vertices
       *
       *  @param[in] index   index of the preceding vertex
       *  @returns: pointer to the vertex, 0 on error
       */
      
      bool setPrecedingVertices(const std::vector<const xAOD::Vertex*>& vertices,
                                const xAOD::VertexContainer* vertexContainer);
      
      
      /** @} */
      
      /************************************************************************/
      /** @{ 
       *  @brief: Links to cascade vertices
       *  
       *  Cascade decays, e.g. Bd->Jpsi(mumu)+Ks(pipi), consist of more than one
       *  reconstructed vertex. for instance in case of the Bd decay,
       *  2 vertices are required. Cascade vertices links are used to store 
       *  relations between mother and daughter vertices in the cascade decays.
       */

      /** Returns number of cascade vertices
       *
       *  @returns: number of cascade vertices, -1 on error
       */
      
      int nCascadeVertices();
            
      /** Returns pointer to a cascade vertex
       *
       *  @param[in] index   index of the cascade vertex
       *  @returns: pointer to the vertex, 0 on error
       */
      
      const xAOD::Vertex* cascadeVertex(const size_t index);

      /** Returns vector of pointers to cascade vertices
       *
       *  @returns: vector of pointers to vertices, empty vector on error
       */
      
      const std::vector<const xAOD::Vertex*>& cascadeVertices();
      
      /** Sets links to cascade vertices
       *
       *  @param[in] index index of the cascade vertex
       *  @returns: pointer to the vertex, 0 on error
       */
      
      bool setCascadeVertices(const std::vector<const xAOD::Vertex*>& vertices,
                                const xAOD::VertexContainer* vertexContainer);
      
      /** Returns number of stored refitted tracks INCLUDING those from the linked
       *  cascade vertices
       *
       *  @returns: number of refitted tracks, -1 in case of an error  
       */
      
      int nRefTrksCascade();
      
      /** @} */
      
      /************************************************************************/
     /**
       *  @brief: Enum type of the PV
       * 
       *  This enum type is used to identify type of the PV and quantities calculated
       *  using the PV. For instance, the transverse distance can be calculated wrt
       *  the default PV, the closest PV etc.
       * 
       *  @value: PV_MAX_SUM_PT2  vertex with the largest sum Pt^2 (the default one)
       *  @value: PV_MIN_A0       collision vertex closest in 3D to the particle's trajectory,
       *                          i.e. the vertex with the smalles 3D impact parameter a0
       *  @value: PV_MIN_Z0       collision vertex closest in delta(z0) = pv_z - z0, where z0 
       *                          is the z coordinate of the intesection of the  particle's 
       *                          trajectory with the  beam axis in the Z-rho plane.
       *  @value: PV_MIN_Z0_BA    collision vertex closest in delta(z0) = pv_z - z0, where z0 
       *                          is the z coordinate of the point of closest aproach (in 2D) to the  particle's 
       *                          trajectory with the  beam axis in the Z-rho plane.
       */
      
      enum pv_type {PV_MAX_SUM_PT2, PV_MIN_A0, PV_MIN_Z0, PV_MIN_Z0_BA};
      
      /************************************************************************/
      /** @{ 
       *  Links to the refitted primary vertices
       *  
       *  @todo: In case the refitted vertices are augmented with links to the original
       *         primary vertices, method origPv will resolve the links and return the original PVs.
       */
      
      /** Get the refitted collision vertex of type pv_type
       *  
       *  @param[in] vertexType        type (selection) of the PV (@see: pv_type)
       *  @returns: pointer to the PV, 0 if not stored
       */
      
      const xAOD::Vertex* pv    (const pv_type vertexType  = BPhysHelper::PV_MIN_A0); //!< refitted PV
      const xAOD::Vertex* origPv(const pv_type vertexType = BPhysHelper::PV_MIN_A0);  //!< original PV

      /** Set the original collision vertex of type pv_type
       *  
       *  @param[in] vertexType        type (selection) of the PV (@see: pv_type)
       *  @param[in] pv                pointer to the PV
       *  @param[in] vertexContainer   collection in which the PV is stored
       *  @returns: true on success, false on failure
       */
      bool setOrigPv(const xAOD::Vertex* pv, 
                 const xAOD::VertexContainer* vertexContainer,
                 const pv_type vertexType = BPhysHelper::PV_MIN_A0);
      
      /** Set the refitted collision vertex of type pv_type
       *  
       *  @param[in] vertexType        type (selection) of the PV (@see: pv_type)
       *  @param[in] pv                pointer to the PV
       *  @param[in] vertexContainer   collection in which the PV is stored
       *  @returns: true on success, false on failure
       */
      
      bool setPv(const xAOD::Vertex* pv, 
                 const xAOD::VertexContainer* vertexContainer,
                 const pv_type vertexType = BPhysHelper::PV_MIN_A0);
      
      /**Set the exitCode of the refitter for vertex of type pv_type
      * @param[in] vertexType        type (selection) of the PV (@see: pv_type)
      * @param[in] code              int containing the code
      * @returns: true on success, false on failure      
      */
      bool setRefitPVStatus(int code, const pv_type vertexType = BPhysHelper::PV_MIN_A0);

      /**Get the exitCode of the refitter for vertex of type pv_type
      * @param[in] vertexType      type (selection) of the PV (@see: pv_type)
      * @returns: the code  0=refit not request, 1=completed sucessfully, >1 not refitted for good reasons,
      *   <0 not refitted for bad reasons (see PrimaryVertexRefitter.cxx for more details)
      */
      int RefitPVStatus(const pv_type vertexType = BPhysHelper::PV_MIN_A0);      

      /** @} */

      /************************************************************************/
      /** @{ 
       *  Transverse decay distance and its error
       */
      
      /** Get the transverse decay distance and its error measured between the refitted 
       *  primary vertex of type pv_type and the B vertex
       *  
       *  @param[in] vertexType        type (selection) of the PV (@see: pv_type)
       *  @returns: lxy, -9999999. in case augmentation is not available
       */
      
      float lxy   (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< decay distance
      float lxyErr(const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< its error

      /** Set the transverse decay distance and its error measured between the refitted 
       *  primary vertex of type pv_type and the B vertex
       *  
       *  @param[in] vertexType   type (selection) of the PV (@see: pv_type)
       *  @param[in] val          input value
       *  @returns: true on success
       */
      
      bool setLxy   (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< decay distance
      bool setLxyErr(const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< its error

      float lxyz   (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< decay distance
      float lxyzErr(const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< its error
      bool setLxyz   (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< decay distance
      bool setLxyzErr(const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< its error

      /** @} */
      
      /************************************************************************/
      /** @{ 
       *  Impact parameters and their error
       */
      
      /** Get the 3D, transverse, and longitudinal impact parameters and their error
       *  
       *  @param[in] vertexType        type (selection) of the PV (@see: pv_type)
       *  @returns: impact parameter or its error, -9999999. in case augmentation is not available
       */
      
      float a0     (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< 3D impact parameter
      float a0Err  (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< 3D impact parameter error
      float a0xy   (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< transverse impact parameter
      float a0xyErr(const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< rtansverse impact parameter error
      float z0     (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< longitudinal impact parameter
      float z0Err  (const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< longitudinal impact parameter error

      /** Set the 3D and transverse impact parameters and their error
       *  
       *  @param[in] vertexType   type (selection) of the PV (@see: pv_type)
       *  @param[in] val          input value
       *  @returns: true on success
       */
      
      float setA0     (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< 3D impact parameter
      float setA0Err  (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< 3D impact parameter error
      float setA0xy   (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< transverse impact parameter
      float setA0xyErr(const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< transverse impact parameter error
      float setZ0     (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< longitudinal impact parameter
      float setZ0Err  (const float val, const pv_type vertexType = BPhysHelper::PV_MIN_A0); //!< longitudinal impact parameter error
      
      /** @} */
      
      /************************************************************************/
      /** @{
       *  Return string names for vertex association types.
       */
      
      static const std::string pv_type_str[];
      /** @}  */
      
      /************************************************************************/
      /** @{
       *  Return number of vertex association types (useful for loops).
       */
      
      static const unsigned int n_pv_types;
      /** @}  */
      
      /************************************************************************/
      /************************************************************************/
    protected:

      /** @brief: Cache covariance matrix
       *
       *  To speed up access to covariance matrix, m_cachedCov 
       *  attribute is created first time covariance matrix accessor method is 
       *  called. In subsequent calls, cached matrix is used. 
       *  @returns: true on success   
       */
      
      bool cacheCov();
      
      /** @brief: Cache refitted tracks
       *
       *  To speed up access to refitted track momenta, m_cachedRefTracks 
       *  vector is created first time refitted track accessor methods are 
       *  called. In subsequent calls, cached vector is used. 
       *  @returns: true on success   
       */
      
      bool cacheRefTracks();

      /** @brief: Cache linked muons
       *
       *  To speed up access to linked muons, m_cachedMuons 
       *  vector is created first time linked muon accessor methods are 
       *  called. In subsequent calls, cached vector is used. 
       *  @returns: true on success   
       */
      
      bool cacheMuons();
      bool cacheElectrons();


      /** @brief: Cache preceding vertices
       *
       *  To speed up access to linked preceding vertices, m_cachedPrecedingVertices 
       *  vector is created first time linked preceding vertices accessor methods are 
       *  called. In subsequent calls, cached vector is used. 
       *  @returns: true on success   
       */
      
      bool cachePrecedingVertices();

      /** @brief: Cache cascade vertices
       *
       *  To speed up access to linked cascade vertices, m_cachedCascadeVertices 
       *  vector is created first time linked cascade vertices accessor methods are 
       *  called. In subsequent calls, cached vector is used. 
       *  @returns: true on success   
       */
      
      bool cacheCascadeVertices();

      /************************************************************************/
      /** Cached B decay xAOD vertex */
      const xAOD::Vertex* m_b;

      /************************************************************************/
      /** @{
       *  cached covariance matrix 
       */
      
      bool                m_covCached;
      TMatrixTSym<double> m_cachedCov;
      
      /** @} */

      /************************************************************************/
      /** @{
       *  cached refitted track momenta 
       */
      
      bool                  m_refTracksCached;
      std::vector<TVector3> m_cachedRefTracks;
      
      /** @} */

      /************************************************************************/
      /** @{
       *  cached linked muons 
       */
      
      bool                           m_muonsCached;
      bool                           m_electronsCached;
      std::vector<const xAOD::Muon*> m_cachedMuons;
      std::vector<const xAOD::Electron*> m_cachedElectrons;
      
      /** @} */

      /************************************************************************/
      /** @{
       *  cached linked vertices 
       */
      
      bool                             m_precedingVerticesCached;
      std::vector<const xAOD::Vertex*> m_cachedPrecedingVertices;
      bool                             m_cascadeVerticesCached;
      std::vector<const xAOD::Vertex*> m_cachedCascadeVertices;
      
      /** @} */
      
      /************************************************************************/
      /** @{
       *  Return error values for vector quantities: 
       */
      
      static const std::vector<TVector3>            emptyVectorOfTVector3;
      static const std::vector<const xAOD::Muon*>   emptyVectorOfMuons;
      static const std::vector<const xAOD::Electron*>   emptyVectorOfElectrons;
      static const TMatrixTSym<double>              emptyMatrix;
      static const std::vector<const xAOD::Vertex*> emptyVectorOfVertices;
      /** @}  */
      
  }; // BPhysHelper

} // namespace xAOD

/** Useful CHECK macro
 */
#ifdef ATH_MSG_WARNING
  #define BPHYS_CHECK( EXP ) { if( ! EXP ) ATH_MSG_WARNING ( "Call of \"" << #EXP << "\" failed" ); }
#else
  #define BPHYS_CHECK( EXP ) { if( ! EXP ) std::cout << "Warning in <BPhysHelper::BPHYS_CHECK> Call of \"" << #EXP << "\" failed" << std::endl; }
#endif

#endif // XAOD_BPHYSHELPER_H
