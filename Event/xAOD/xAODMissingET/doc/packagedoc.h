// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/**
@page xAODMissingET_page Data objects for Missing Transverse Momentum reconstruction
 *
 *  @tableofcontents
 *
 *  The event data model provided in this package supports the reconstruction of missing transverse energy in ATLAS, and provides the
 *  data containers to represent the results of this reconstruction.
 * 
@section xAODMissingET_@section met_intro Introduction
 * 
 *  Missing transverse momentum (MET, @f$ E_{\rm T}^{\rm miss}) @f$ reconstruction in ATLAS is actually represented by the three 
 *  kinematic components @f$ (p_{x}^{\rm miss},p_{y}^{\rm miss},\Sigma E_{\rm T}) @f$, which are the missing momentum components
 *  in the transverse plane, and the scalar sum of transverse momenta contributing to MET. In addition to the full event MET, 
 *   MET @a terms represent contributions from 
 *  certain physics and signal objects. In general, all reconstructed hard physics objects like individual particles and particle 
 *  jets contribute their (fully calibrated) kinematics to their specific MET term (e.g., @f$ E_{\rm T}^{\rm miss,electron} @f$,
 *   @f$ E_{\rm T}^{\rm miss,jet} @f$, ...). In addition, signal objects (topological calorimeter cell clusters and reconstructed tracks) 
 *  not used by hard objects are collected into a dedicated soft term MET contribution (SoftTerm). The sum of all terms reconstructed 
 *  within the context of a given MET configuration is then the full event MET.
 * 
 *  @subsection met_intro_scope MET reconstruction scope and basics
 *
 *  The scope of MET reconstruction is to use all hard objects passing dedicated kinematic and topological cuts, and having an acceptable
 *  reconstruction quality indicator, for the individual MET terms. As there is likely signal overlap between these objects, an event signal
 *  ambiguity resolution strategy is applied which is based on tagging common calorimeter signal use and rejecting object contributions if 
 *  the signal they are based on is already used in a previously scheduled MET contribution. After all hard objects are considered this way, 
 *  the residual unused signals are collected into the MET SoftTerm. 
 *
 *  This strategy makes the resulting total MET dependent on the order with which the hard objects are considered. This order is by default
 *  defined by expectations for the reconstruction quality and precision, leading to the sequence (1) electrons, (2) photons, (3) taus,
 *  (4) muons, (5) particle jets, and (6) SoftTerm.    
 *
 *  @subsection met_intro_edm Event data model basics
 *
 *  The MET event data model (EDM) reflects the needs introduced by the MET reconstruction discussed above. It consists of two basic data objects and
 *  their respective containers, in addition to the corresponding auxiliary storage objects. The two basic data objects respectively provide storage of the 
 *  @ref met_intro "kinematic variables" related to a MET term 
 *  (@link xAOD::MissingET_v1 MissingET @endlink object), and storage of information related to the contributions 
 *  to this MET term (@link xAOD::MissingETComponent_v1 MissingETComponent @endlink object). With this approach the kinematic data, characterized by 
 *  basic internally used data types and structures, is clearly separated from the composition data providing detailed information, which typically
 *  requires to provide more support for data management and search capabilities. 
 * 
 *  The @link xAOD::MissingET_v1  MissingET @endlink is considered universal, as it is used to store individual MET terms as well as the final 
 *  full event MET. The objects representing the terms and the full event MET are stored in a container object 
 *  (@link xAOD::MissingETContainer_v1 MissingETContainer @endlink). Standard MET reconstruction tools also create a MET component object for
 *  each MET term, but not for the full event MET, as this means duplication of information. This 
 *  @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object is stored in its own container 
 *  (@link xAOD::MissingETComponentMap_v1 MissingETComponentMap @endlink, one per MET term). 
 * 
@section xAODMissingET_@section met_edm Missing Transverse Momentum Event Data Model
 * 
 *  The MET EDM is tailored to support a dynamic (MET reconstruction in the precision reconstruction and analysis context) and static (mostly 
 *  MET related data retrieval in an analysis context) phase. This means that the data objects in this EDM need to (among others) 
 *  @li provide storage for the variables characterizing a MET term; 
 *  @li support easy and efficient access to these variables for a given term;
 *  @li support searching for a specific MET term and its components in their respective containers;  
 *  @li implement the algebra correctly calculating the kinematic variables in the dynamic phase; 
 *  @li support event signal ambiguity resolution;  
 *  @li provide access to the individual contributions to a given MET term; 
 *  @li support re-reconstruction of the full MET for a given configuration, or of selected MET terms only.
 *
 *  @note The MET EDM does not provide any implementation directly reconstructing a MET term. There is no functionality provided which fills e.g.
 *        the basic data object and the component object at the same time. There is also no implementation provided which e.g. extracts underlying
 *        basic signals like topological calorimeter cell clusters or reconstructed charged tracks from a given physics object. These are tasks which
 *        need to be implemented in the dedicated tools reconstructing a MET term from a list of given objects. On the other hand, the MET EDM provides 
 *        the book-keeping to keep track of use clusters and tracks, with the idea that the corresponding data has to be provided by the tools again 
 *        (used signal objects have to be explicitly set in the book-keeping caches).  
 * 
 *  @subsection met_edm_data MissingET and MissingETContainer
 *
 *  The basic kinematic data object for an individual MET term, any combination of those, and the final (summed) MET is the 
 *  @link xAOD::MissingET_v1 MissingET @endlink object. It provides all functionality and support for the correct algebra applicable for MET 
 *  reconstruction. The relevant stored kinematic variables are @f$ (p_{x}^{\rm miss}, p_{y}^{\rm miss}, \Sigma E_{\rm T}) @f$.
 *
 *  The result of MET reconstruction is represented by three kinematic variables @f$ (p_{x}^{\rm miss},p_{y}^{\rm miss},\Sigma E_{\rm T}) @f$, which are 
 *  constructed as
 *  @anchor met_sum_rule
 *  @f{eqnarray*}{
 *     p_{x}^{\rm miss} & = & - \sum_{i = 1}^{N_{\rm contrib}} p_{x,i} \\ 
 *     p_{y}^{\rm miss} & = & - \sum_{i = 1}^{N_{\rm contrib}} p_{y,i} \\
 *     \Sigma E_{\rm T} & = &   \sum_{i = 1}^{N_{\rm contrib}} p_{{\rm T},i} .
 *  @f}
 *  Here @f$ N_{\rm contrib} @f$ is the number of contributions to the MET term, and 
 *  @f$ p_{x,i} @f$ and @f$ p_{y,i} @f$ are the transverse momentum components of contribution @f$ i @f$, and @f$ p_{{\rm T},i} @f$ is the transverse
 *  momentum of this component. These three MET variables can be accessed, set, and manipulated using methods provided by the 
 *  @link xAOD::MissingET_v1 MissingET @endlink object.  
 * 
 *  In addition, each @link xAOD::MissingET_v1 MissingET @endlink object provides accessors and setters for the name of the MET term it represents, 
 *  and a source indicator (bitmask).
 *
 // *  @subsubsection met_edm_basics_construct Construction
 // *  
 // *  The @link xAOD::MissingET_v1 MissingET @endlink can be instantiated in three principal states. The <i>default state</i>
 // *  is a data object without even a data store, which generally is unusable for reconstruction or data presentation, but is needed for some internal I/O
 // *  and data container instantiation support. The <i>empty state</i> is represented by a MET object with a name and (default or client provided) source
 // *  indicator. The kinematic variables are set to @f$( p_{x}^{\rm miss} = 0, p_{y}^{\rm miss} = 0, \Sigma E_{\rm T} = 0) @f$. The last option is the 
 // *  <i>loaded state</i>, which has the same data content as the empty date, except that the kinematic variables are loaded from a given contributing
 // *  object, or a corresponding set of numbers.     
 // *
 // *  The dynamic phase is supported by @link xAOD::MissingET_v1::add methods @endlink implementing the summing convention given in this 
 // *  @ref met_sum_rule "set of equations". Other operations like re-scaling to the kinematic variables and removing a kinematic contribution 
 // *  from a given physics or signal object are implemented as well.
 // *
 // *  @note It is suggested that clients interact with the @link xAOD::MissingET_v1 MissingET @endlink object using the available 
 // *        methods and operators. This allows maintaining  the consistency of the kinematic data. In addition, the correct sum rules
 // *        are applied.   
 // *  
 // *  @subsubsection met_edm_basics_storage Storage 
 // * 
 // *  @link xAOD::MissingET_v1 MissingET @endlink objects are initially constructed as individual, non-storable data objects - either in the default or 
 // *  one of  the plain-old-data (POD) states (empty or loaded). Independent of the initial state, if the pointer to a 
 // *  @link xAOD::MissingET_v1 MissingET @endlink is stored in a @link xAOD::MissingETContainer_v1 MissingETContainer @endlink, and this container 
 // *  has a valid reference to an auxiliary data store provided  by the @link xAOD::MissingETAuxContainer_v1 MissingETAuxContainer @endlink, the 
 // *  @link xAOD::MissingET_v1 MissingET @endlink object becomes fully useable and storable. If its initial object state is default, the newly attached 
 // *  data store has default content, while for the other two states the (private) data store content is transfered to the auxiliary store. 
 // *
 // *  While there are no implementations enforcing any rules on the sequence of the @link xAOD::MissingET_v1 MissingET @endlink pointers  


// As mentioned above, each of these data objects typically contains the contribution from a set of same type final state objects
//  *  like particles and jets, and is identified as a specific contribution by a name and a source indicator (bit pattern). In general all MET objects from the various
//  *  sources contribute to a given MET reconstruction configuration and their sum is the overall MET for the event. For storage, and to indicated that a given xAOD::MissingET_v1
//  *  object represents a certain MET term in a given MET reconstruction configuration, the individual MET objects 
//  *  are collected into a xAOD::MissingETContainer_v1 object. Typically, the insertion sequence (e.g. the index of an individual MET object) in this container is
//  *  determined by the tool  sequence with which the MET terms are reconstructed, there is no particular enforcement of this rule in the EDM. Safe (random) accessors using the 
//  *  name or the source indicator of the requested MET object are provided by the MET object.

//  *
//  *  The @link xAOD::MissingET_v1 MissingET @endlink object can be instantiated as a plain-old-data (POD) object. To store it, it needs to be
//  *  collected into a @link xAOD::MissingETContainer_v1 MissingETContainer @endlink with an attached auxiliary
//  *  @link xAOD::MissingETAuxContainer_v1 MissingETAuxContainer @endlink store. This auxiliary store provides the underlying storable (and optimized) 
//  *  data structures.
 *
 *  @subsection met_edm_composition Contributions and composition    
 *
 *  The contributions to a given MET term are characterized by (1) a list of contributing objects, (2) a list of the kinematic weights associated with
 *  each contributing object,  and (3) a list of status-words characterizing each of the contributions. The data object storing this information is
 *  @link xAOD::MissingETContribution_v1 MissingETContribution @endlink. This object can be instantiated as POD, and it provides all functionality 
 *  to manipulate the mentioned stores.
 * 
 *  For storage, and to describe a fully reconstructed MET, the @link xAOD::MissingETContribution_v1 MissingETContribution @endlink objects are collected
 *  into a @link xAOD::MissingETCompositionMap_v1 MissingETCompositionMap @endlink object, which together with its auxiliary 
 *  @link xAOD::MissingETAuxCompositionMap_v1 MissingETAuxCompositionMap @endlink store providing the underlying storage structures, can be persistified. 
 *
 *  Most of the functionality connected with the access and management of the @link xAOD::MissingETCompositionMap_v1 MissingETCompositionMap @endlink
 *  is provided by static functions in the @link xAOD::MissingETComposition MissingETComposition @endlink structure. 
 */
