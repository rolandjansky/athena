/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GlobReco/Parameterization.h"


namespace AFP {
  Parameterization::Parameterization(std::string_view filename) : m_slopeInitIP{}, m_positionInitIP{} {
    std::cout << "Reading parameterization from file: \"" << filename << "\"" << '\n';

    std::ifstream file;
    file.open(filename.data());
    if(!file.is_open()) {
      std::stringstream errorMsg;
      errorMsg << "Parameterization::load(): \tCould not open parameterization file: \"" << filename << "\"";
      throw std::runtime_error(errorMsg.str().data());
    }

    while (file.peek() == '#') {
      char ch;
      std::string name;
      file >> ch >> name;

      if (name == "m_xPositionInitIP")  { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_positionInitIP[0] = value; }
      if (name == "m_yPositionInitIP")  { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_positionInitIP[1] = value; }
      if (name == "m_zPositionInitIP")  { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_positionInitIP[2] = value; }
      if (name == "m_xSlopeInitIP")     { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_slopeInitIP[0] = value; }
      if (name == "m_ySlopeInitIP")     { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_slopeInitIP[1] = value; }
      if (name == "m_detectorPosition") { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_parametrizationPosition = value; }
      if (name == "m_nominalEnergy")    { double value; file >> value; std::cout << '\t' << name << ": " << value << '\n'; m_energy = value; }

      file.ignore(1000, '\n');
    }

    std::vector<std::unique_ptr<TF1>> poly;

    m_yIsUsed = false;

    for (int iEqu = 0; iEqu < m_numberOfEquations; iEqu++) {
      file.ignore(1000, '\n');
      std::vector<double> coef;

      for (int iPoly = 0; iPoly < m_numberOfPolynomials; iPoly++) {
        int N = 0;
        file >> N;
        double value;

        for (int i = 0; i <= N; i++) {
          if (file.eof()) {
            std::stringstream errorMsg;
            errorMsg << "Parameterization::load(): \tFile \"" << filename
                     << "\" does not contain a correct parameterization";
            throw std::runtime_error(errorMsg.str().data());
          }

          file >> value;
          coef.push_back(value);
        }

        file.ignore(1000, '\n');

        TString name;
        name.Form("pol%d", N);
        auto temp = std::make_unique<TF1>(name, name);
        for (int iCoef = 0; iCoef <= N; ++iCoef) {
          temp->SetParameter(iCoef, coef.at(iCoef));
        }

        // For y-dependent equation check if polynomials are non-zero order
        if (iEqu == 1 or iEqu == 3) {
          m_yIsUsed = m_yIsUsed or not (N == 0 and coef[0] == 0);
        }

        poly.push_back(std::move(temp));
        coef.clear();
      }

      m_equation.push_back(std::make_unique<Equation>(m_energy, std::move(poly)));
      poly.clear();
    }

    std::cout << "Finished reading parameterization: \"" << filename << "\"\n\n";
  }

  double Parameterization::x(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return getEquation(0)->evaluate(x0 + m_positionInitIP.at(0), y0 + m_positionInitIP.at(1), z0 + m_positionInitIP.at(2), sx0 + m_slopeInitIP.at(0), sy0 + m_slopeInitIP.at(1), E);
  }

  double Parameterization::y(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return getEquation(1)->evaluate(x0 + m_positionInitIP.at(0), y0 + m_positionInitIP.at(1), z0 + m_positionInitIP.at(2), sx0 + m_slopeInitIP.at(0), sy0 + m_slopeInitIP.at(1), E);
  }

  double Parameterization::sx(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return getEquation(2)->evaluate(x0 + m_positionInitIP.at(0), y0 + m_positionInitIP.at(1), z0 + m_positionInitIP.at(2), sx0 + m_slopeInitIP.at(0), sy0 + m_slopeInitIP.at(1), E);
  }

  double Parameterization::sy(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return getEquation(3)->evaluate(x0 + m_positionInitIP.at(0), y0 + m_positionInitIP.at(1), z0 + m_positionInitIP.at(2), sx0 + m_slopeInitIP.at(0), sy0 + m_slopeInitIP.at(1), E);
  }

  double Parameterization::Equation::evaluate(double x, double y, double z, double sx, double sy, double E) const {
    const double xi = (m_energy - E)/m_energy;

    const double v0 = getPolynomial(0)->Eval(xi);
    const double v1 = x * getPolynomial(1)->Eval(xi);
    const double v2 = y * getPolynomial(2)->Eval(xi);
    const double v3 = z * getPolynomial(3)->Eval(xi);
    const double v4 = sx * getPolynomial(4)->Eval(xi);
    const double v5 = sy * getPolynomial(5)->Eval(xi);
    const double v6 = z*sx * getPolynomial(6)->Eval(xi);
    const double v7 = z*sy * getPolynomial(7)->Eval(xi);

    return v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7;
  }
}
