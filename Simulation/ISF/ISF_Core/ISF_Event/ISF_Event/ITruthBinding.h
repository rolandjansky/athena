/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthBinding.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENTBASE_ITRUTHBINDING_H
#define ISF_EVENTBASE_ITRUTHBINDING_H

// Barcode includes
#include "BarcodeInterfaces/Barcode.h"

// CLHEP includes
#include "CLHEP/Geometry/Vector3D.h"

namespace ISF {

  /**
   @class ITruthBinding 
   @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
   */

 class ITruthBinding {
   public:
     ITruthBinding() : m_persistent(false) {}

     /** virtual destructor */
     virtual ~ITruthBinding() { }

     virtual ITruthBinding* clone() const = 0;                     

     /** Status if TruthParticle is in use or not (e.g. by McEvent Collection) */
     bool isPersistent() const { return m_persistent; } 

     /** Change TruthParticle use status */
     void setPersistency(bool useStatus) { m_persistent = useStatus; } 

   private:
     bool             m_persistent;
  };

  /**
   @class TruthBindingT

    A simple template for truth forward binding

   @author Andreas.Salzburger -at- cern.ch   
   */

 template  <class T> class TruthBindingT : public ITruthBinding {
    
    public:
        /** constructor with truth particle*/
        TruthBindingT(T& truthParticle) :
            m_tParticle(&truthParticle)
        {}

        /** destructor frees up memory (only if truth binding is not in use, e.g. in truth event)
          @TODO: 1) deleting m_tParticle causes seg-faults inside FastCaloSim code (legacy mode)
                 2) empty destructor *may* cause memory leak ! */
        //virtual ~TruthBindingT() { if (!isPersistent()) delete m_tParticle; }
        virtual ~TruthBindingT() { };

        /** Implicit constructor*/
        TruthBindingT* clone() const { return new TruthBindingT(*m_tParticle); }

        /** Get particle's representation in the MC truth record */
        T& truthParticle() const { return (*m_tParticle);  } 

    private:
        T*               m_tParticle;
                        
  };
  
} // end of namespace

#endif // ISF_EVENTBASE_ITRUTHBINDING_H
