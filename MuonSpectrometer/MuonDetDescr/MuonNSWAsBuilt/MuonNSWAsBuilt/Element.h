/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_ELEMENT_H
#define MUONNSWASBUILT_ELEMENT_H

#include <vector>
#include <map>
#include <memory>

#include "MuonNSWAsBuilt/ElementModel.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace NswAsBuilt {

  /**
  * Element: a node in a hierarchy of alignment frames
  *
  * The class Element is the building block for chaining alignment
  * transformations in various coordinate systems.
  *
  * Each Element has attached to it a local coordinate system. The
  * transformation of a point in local coordinates from this Element frame to
  * the higher-level coordinate system is achieved with the method transform.
  * The transform method applies rigid transformation (rotations, translation)
  * and possibly deformations.
  *
  * The Elements are organized in a tree structure, representing the hierarchy
  * of coordinate systems. Traversal of the tree may be achieved using the
  * mother and daughter pointers, or with the method traverseTree, which
  * applies the user-provided function to all the daughers and grand-daughters
  * of this Element.
  *
  * In this context, it is interesting to call the transform methods
  * iteratively on all the mothers. This is achieved by the method
  * transformToFrame.
  *
  * The transformation method is handled by an external class ElementModel,
  * which, by sub-classing, provides the deformation model of this Element. The
  * ElementModel must be set in the constructor.
  *
  * The ElementModel is parameterized by an array of model parameters, whose
  * meaning is known only to the used ElementModel implementation. The model
  * parameters are members of this class, and are provided in two forms: the
  * NOMINAL parameters (meant to represent the nominal geometry), and the
  * CORRECTION parameters (meant to represent the actually measured
  * parameters). The choice of one or the other of the two forms is possible
  * using the ParameterClass argument of the transform method.
  */
   
  class Element {
    public:
      using ParameterVector = ElementModel::ParameterVector;
      using ipar_t = ElementModel::ipar_t;
      using daughterVec_t = std::vector<std::unique_ptr<Element>>;
      using VectorSetRef = ElementModel::VectorSetRef;

     /* ParameterClass: choose NOMINAL or CORRECTION parameters.
      *
      * The CORRECTION parameters are the ones which are actually measured,
      * while the NOMINAL parameters are the ones of the design */
      enum class ParameterClass {
        NOMINAL,
        CORRECTION
      };

     /* Constructors: pass a unique_ptr to the deformation model */
      Element(std::unique_ptr<ElementModel> deformationModel);

      // delete others
      Element() = delete;
      Element(const Element&) = delete;
      Element& operator=(const Element&) = delete;

     /* Getters and setters for the ElementModel parameters */
      double getParameter(ParameterClass iclass, ipar_t ipar) const;
      void setParameter(ParameterClass iclass, ipar_t ipar, double value);
      void setParametersFromMap(ParameterClass iclass, const std::map<std::string, double>& values);

     /* Transforms a set of vectors in local coordinates of this Element's
      * frame to the coordinates of the mother frame */
      void transform(ParameterClass iclass, VectorSetRef local) const;
      
     /* Repeatedly call the transform method using all mothers up to frame.
      * Set frame to nullptr to loop on all mothers. */
      void transformToFrame(ParameterClass iclass, VectorSetRef local, Element* frame) const;

     /* Cache the rigid component of the transform, for quicker access */
      void cacheTransforms();

     /* asapId: optional string identifier (imported from ASAP) */
      void setAsapId(const std::string& asapId) { m_asapId = asapId; }
      const std::string& asapId() const { return m_asapId; }

     /* Getters and setters for the tree structure */
      void addDaughter(std::unique_ptr<Element> daughter);
      Element* mother() const { return m_mother; }

     /* Applies the callback function to this Element and to all its daughters
      * down to the leaves*/
      void traverseTree(std::function<void(Element&)> callback);


    private:

      ParameterVector& parameterVector(ParameterClass iclass);
      const ParameterVector& parameterVector(ParameterClass iclass) const;

      std::unique_ptr<ElementModel> m_deformationModel; // The deformation model
      ParameterVector m_nominalPar;   // The parameter vector for the NOMINAL class
      ParameterVector m_correctedPar; // The parameter vector for the CORRECTION class

      daughterVec_t m_daughters;      // The list of daughter Elements
      Element* m_mother = nullptr;    // The mother Element

      std::string m_asapId;           // Optional string identifier (imported from ASAP)
  };
}

#endif

