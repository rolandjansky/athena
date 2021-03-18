/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/* Dear emacs, this is -*-c++-*- */
#ifndef _ComponentVisitor_H_
#define _ComponentVisitor_H_

#include <vector>
#include <set>
#include <type_traits>
#include <regex>

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Algorithm.h"

namespace xGaudi {
   class ComponentVisitorBase {
   protected:
      static const std::regex s_noFilter;
   };

   /** Helper class to recurs through a component hierarchy e.g. tools, algorithm sequences.
    * T_Component is the component type e.g. IAlgTool, Gaudi::Algorithm, T_TestVisited is a
    * helper class to e.g. test whether a certain component was already visited to support
    * circular dependencies e.g. std::set<IAlgTool *>.
    *
    * The T_TestVisted class must have an insert( const T_Component *) method which returns
    * a pair of which the second element is a boolean. This second, boolean element must be
    * false if the given component has been "inserted" already. An example of such a class
    * is @ref NoVisitTester.
    *
    * There are predefined visitors for Tools and algorithms : @ref ToolVisitor,
    * @ref AlgorithmVisitor.
    */
   template <class T_Component, class T_TestVisited = std::set<T_Component*> >
   class ComponentVisitor : public ComponentVisitorBase {
      struct IVisitor {
         virtual ~IVisitor()                      = default;
         virtual void visit( T_Component* ) const = 0;
      };

   public:
      static std::vector<T_Component *> *getChilds(T_Component *a_component);

      /** Visit the given tools and their child tools.
       * @param tools: list of top level tools to be visited
       * @param visitor: a helper class which will be called for each visited tool
       * @param reject_filter: an optional regular expression to filter out tools by name i.e. they and their child tools
       * @param ignore : a set of tools to ignore e.g. tools which have been visited already
       *
       * usage:
       * @verbatim
       *      auto visitor=[this](const IAlgTool *tool) { this->msg(MSG::DEBUG)  << tool->name() << endmsg; };
       *      ToolVisitor::visit(tools(), visitor);
       * @verbatim
       */
      template <typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<void, Callable, T_Component*>>>
      static T_TestVisited visit( const std::vector<T_Component*>& components,
                                  Callable& callable,
                                  const std::regex& reject_filter = ComponentVisitorBase::s_noFilter,
                                  T_TestVisited &&ignore = T_TestVisited()) {
         class Visitor final : public IVisitor {
            Callable* m_func;

         public:
            Visitor( Callable* f ) : m_func{f} {}
            void visit( T_Component* a_component ) const override { std::invoke( *m_func, a_component ); }
         };
         recursiveVisit( components, Visitor{&callable}, reject_filter, ignore );
         return std::move (ignore);
      }

   private:
      static void             recursiveVisit( const std::vector<T_Component*>& components, IVisitor const& visitor,
                                              const std::regex& reject_filter,
                                              T_TestVisited &visited);
   };

   class NoVisitTester {
   public:
      std::pair<bool,bool> insert(const Gaudi::Algorithm *) { return std::make_pair(true,true); }
   };

   using ToolVisitor      = ComponentVisitor<IAlgTool>;
   using AlgorithmVisitor = ComponentVisitor<Gaudi::Algorithm, NoVisitTester>;
}
#endif
