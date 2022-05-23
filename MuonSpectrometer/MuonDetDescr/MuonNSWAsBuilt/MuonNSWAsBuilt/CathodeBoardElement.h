/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_CATHODEBOARDELEMENT_H
#define MUONNSWASBUILT_CATHODEBOARDELEMENT_H

#include "MuonNSWAsBuilt/Element.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <memory>

namespace NswAsBuilt {

  /**
   * Helper class saving all the needed information to compute strips for a
   * particular cathode board
   *
   * This class holds a reference to an Element representing a specific cathode board, and
   * the stripConfiguration: the various pitch vectors to compute a particular
   * strip on this cathode board.
   *
   * The method getStgcStrip performs the computation of the requested strip in
   * quadruplet coordinates
   *
   */
  class CathodeBoardElement {
    public:
      using ParameterClass = Element::ParameterClass;
      /* In the sTGC strip (cathode) boards due to the staggering, the first/last strip could be a half 
      * or a full strip depending on the strip_board_type. Therefore first and last strip are handled 
      * separately from second or any i-strip   
      */
      struct stgcStripPoint_t {
        Amg::Vector3D pos;         // a point along the strip
        Amg::Vector3D pitch; // the pitch vector to add in order to get the other strips
      };

      struct stgcStripConfiguration_t {
        int lastStripNumber;            // the strip number of the last strip i.e the number of strips
        stgcStripPoint_t fCenterPoint;  // the center of the first strip
        stgcStripPoint_t fLeftPoint;    // the left (X<0) point of the first strip
        stgcStripPoint_t fRightPoint;   // the right(X>0) point of the first strip
        stgcStripPoint_t sCenterPoint;  // the center of the second strip
        stgcStripPoint_t sLeftPoint;    // the left (X<0) point of the second strip
        stgcStripPoint_t sRightPoint;   // the right(X>0) point of the second strip
        stgcStripPoint_t lCenterPoint;  // the center of the last strip
        stgcStripPoint_t lLeftPoint;    // the left (X<0) point of the last strip
        stgcStripPoint_t lRightPoint;   // the right(X>0) point of the last strip

      };

      /**
       * Constructor
       */
      CathodeBoardElement(stgcStripConfiguration_t config, std::reference_wrapper<Element> element);

      /**
       * The return type of the getStrip method: three points along the strip,
       * in quadruplet coordinate
       */
      struct stgcStrip_t {
        Amg::Vector3D center;
        Amg::Vector3D left;
        Amg::Vector3D right;
      };

      /**
       * Returns three points (center, left, right) of a given strip, in
       * quadruplet coordinates.
       */
      stgcStrip_t getStgcStrip(ParameterClass iclass, int stripNumber) const;

      /**
       * Returns a point on the strip, parameterized by second coordinate s, in
       * [-1,+1]
       *
       */
      Amg::Vector3D getPositionAlongStgcStrip(ParameterClass iclass, int stripNumber, double s) const;

    private:
      stgcStripConfiguration_t m_config_stgc;
      std::reference_wrapper<Element> m_element_stgc; // reference element of a PCB
  };
}

#endif

