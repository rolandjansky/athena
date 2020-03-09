
#ifndef TRIGEGAMMAMATCHINGUTILS_H_
#define TRIGEGAMMAMATCHINGUTILS_H_

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include <iostream>
#include "TrigDecisionTool/TrigDecisionTool.h"


namespace TrigEgammaMatchingUtils{

  /* This class is used to hold the associated te/composite to a trigger.
   * Depend on the rdt configuration, the element will hold the composite pointer
   * for Run3 or the trigger element for Run1/2.
   */
  class Element
  {
    public:
  
      /* Contructors */
      Element();
      Element( std::string );
      Element( std::string, const HLT::TriggerElement *);
      Element( std::string, const xAOD::TrigComposite *);
  
      /* Destructors */
      ~Element(){};
  
      /*! Get the trigger chain name */
      std::string name() const {return m_name;};
  
      /*! check if we have at least one valid pointer */
      bool isValid() const {return (m_te||m_data)?true:false;};
      
      /*! Get the trigger element (Run 1,2) */
      const HLT::TriggerElement* te() const {return m_te;};
      
      /*! Get the trigger composite (Run 3) */
      const xAOD::TrigComposite* data() const {return m_data;};
  
      /*! Set the composite pointer */
      void setPtr( const xAOD::TrigComposite *data ) {m_data=data;};
      
      /*! Set the trigger element pointer */
      void setPtr( const HLT::TriggerElement *te ) {m_te=te;};
  
    private:
  
      /* the trigger chain name */
      std::string m_name;
      /* the trigger element from run 1,2 framework */
      const HLT::TriggerElement *m_te;
      /* the xAOD composite pointer from the storegate (run 3) */
      const xAOD::TrigComposite *m_data;
  
  };
  
  
  
  /* This class is used to hold all edm objects. Its like a standard vector */
  template<class OBJECT > 
  class Container{
    public:
  
      Container(){};
      ~Container(){};
  
      /*! Check if the container is empty */
      bool isValid() const {return m_vec.empty()? false : true;};
      
      /*! Get the standard vector inside */
      std::vector<OBJECT>& cptr() {return m_vec;};
  
      /*! Push back an object to the container */
      void push_back( OBJECT obj) {m_vec.push_back(obj);};
    
      /*! Get the container size */
      size_t size() const {return m_vec.size();};
    
    private:
  
      /*! vector of objects */
      std::vector<OBJECT> m_vec;
  };
  
  
  /* This class us used to hold the edm object pointer and the associated element */
  template<class OBJECT>
  class Feature{
  
    public:
  
      Feature( OBJECT feat , const Element obj ): 
        m_feat(feat), m_obj(obj) {};
      
      ~Feature(){};
  
      OBJECT cptr() {return m_feat;};
      const Element obj() const {return m_obj;};
      bool isValid() const {return (m_feat && m_obj.isValid() )?true:false;};

    private:
  
      OBJECT m_feat;
      const Element m_obj;
  
  };

}



#endif

