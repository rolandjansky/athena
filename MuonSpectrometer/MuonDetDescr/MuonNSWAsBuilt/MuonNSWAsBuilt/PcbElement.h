/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_PCBELEMENT_H
#define MUONNSWASBUILT_PCBELEMENT_H

#include <memory>

#include "MuonNSWAsBuilt/Element.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace NswAsBuilt {

  /**
  * Helper class saving all the needed information to compute strips for a
  * particular PCB
  *
  * This class holds a reference to an Element representing a specific PCB, and
  * the stripConfiguration: the various pitch vectors to compute a particular
  * strip on this PCB.
  *
  * The method getStrip performs the computation of the requested strip in
  * quadruplet coordinates
  */
  class PcbElement {
    public:

      using ParameterClass = Element::ParameterClass;

     /* A reference point along a strip, and the pitchvector to be added to find the neighbouring strips */
      struct stripPoint_t {
        Amg::Vector3D pos{0,0,0};         // a point along the central strip
        Amg::Vector3D pitchvector{0,0,0}; // the pitch vector to add to get the other strips
      };

     /* The strip configuration: three reference points along the central strip of this PCB. */
      struct stripConfiguration_t {
        int centralStripNumber{0};   // the athena strip number of the central strip
        stripPoint_t centerPoint{};  // the center of the central strip
        stripPoint_t leftPoint{};    // the left (X<0) point of the central strip
        stripPoint_t rightPoint{};   // the right(X>0) point of the central strip
      };

     /* Constructor */
      PcbElement(stripConfiguration_t config, std::reference_wrapper<Element> element);

     /* The return type of the getStrip method: three points along the strip, in quadruplet coordinate */
      struct strip_t {
        Amg::Vector3D center{0,0,0};
        Amg::Vector3D left{0,0,0};
        Amg::Vector3D right{0,0,0};
      };

     /* Returns three points (center, left, right) of a given strip, in quadruplet coordinates. */
      strip_t getStrip(ParameterClass iclass, int stripNumber) const;

     /* Returns a point within sx, in [-0.5,+0.5] aroung the strip, parameterized by second coordinate sy, in [-1,+1] */
      Amg::Vector3D getPositionAlongStrip(ParameterClass iclass, int stripNumber, double sx, double sy) const;

    private:
      stripConfiguration_t m_config;
      std::reference_wrapper<Element> m_element; // reference element of a PCB
  };
}

#endif

