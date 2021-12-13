/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFP_GLOBRECO_PARAMETRIZATION_H
#define AFP_GLOBRECO_PARAMETRIZATION_H

// STL include(s):
#include <memory>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// ROOT include(s):
#include <TF1.h>

namespace AFP {

  /// Class representing transport parameterisation as four Equation.
  class Parameterization {

    /// Local class for storing parameterization equation
    class Equation {
      public:
        /// Takes nominal energy and vector of polynomials as arguments.
        Equation(double energy, std::vector<std::unique_ptr<TF1>> polynomial) : m_energy(energy), m_polynomial(std::move(polynomial)) {}

        /// Evaluates value of function with given initial positions, slopes and energy.
        double evaluate(double x, double y, double z, double sx, double sy, double E) const;

        /// Returns const raw poiner to polynomial of number iPoly.
        const TF1* getPolynomial(int iPoly) const { return m_polynomial.at(iPoly).get(); };

      private:
        /// Nominal energy of proton
        double m_energy;

        /// Vector of TF1 polynomials
        std::vector<std::unique_ptr<TF1>> m_polynomial;
    };

    public:
      /// Takes filename of parametrization file as argument.
      /// Loads parametrization file.
      Parameterization(std::string_view filename);

      /// Evaluates value of horizontal position equation.
      double x(double x0, double y0, double z0, double sx0, double sy0, double E) const;

      /// Evaluates value of vertical position equation.
      double y(double x0, double y0, double z0, double sx0, double sy0, double E) const;

      /// Evaluates value of horizontal slope equation.
      double sx(double x0, double y0, double z0, double sx0, double sy0, double E) const;

      /// Evaluates value of vertical slope equation.
      double sy(double x0, double y0, double z0, double sx0, double sy0, double E) const;

      /// Returns position for which parametrization was performed.
      double parametrizationPosition() const { return m_parametrizationPosition; }

      /// Returns nominal energy for which parametrization was performed.
      double energy() const { return m_energy; }

      /// Returns const raw pointer to equation of number iEqu
      const Equation* getEquation(int iEqu) const { return m_equation.at(iEqu).get(); }

      /// Returns m_yIsUsed
      bool yIsUsed() const { return m_yIsUsed; }

    private:
      /// Number of polynomials in equation
      static constexpr int m_numberOfPolynomials = 8;

      /// Number of equations - each for one measured value
      static constexpr int m_numberOfEquations = 4;

      /// Crossing angles in rad
      std::array<double, 2> m_slopeInitIP;

      /// Initial vertex postition
      std::array<double, 3> m_positionInitIP;

      /// Nominal beam energy
      double m_energy;

      /// Position for which the parameterisation was performed
      double m_parametrizationPosition;

      /// Vector of equations: 0 - x, 1 - y, 2 - sx, 3 - sy
      std::vector<std::unique_ptr<Equation>> m_equation;

      /// True if y measurements are used in reconstruction
      bool m_yIsUsed;
  };
}

#endif // AFP_GLOBRECO_PARAMETRIZATION_H
