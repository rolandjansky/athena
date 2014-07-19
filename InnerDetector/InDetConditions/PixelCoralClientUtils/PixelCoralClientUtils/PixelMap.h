/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _PIXELMAP_hh_
#define _PIXELMAP_hh_

#include <vector>
#include <map>

/* class Pixel_t */
/* { */
/*  public: */
/*   Pixel_t(const unsigned int row, const unsigned int column, */
/* 	  const unsigned int status) */
/*     : m_row(row), m_column(column), m_status(status) */
/*     {} */
/*   const unsigned int row() const    { return m_row; } */
/*   const unsigned int column() const { return m_column; } */
/*   const unsigned int status() const { return m_status; } */
/*  private: */
/*   unsigned int m_row; */
/*   unsigned int m_column; */
/*   unsigned int m_status; */
/* }; */

//typedef std::vector< Pixel_t >  PixelMap_t;

typedef std::map< std::pair<int, int>, unsigned int >  PixelMap_t; // mapping 2D index (col,row) to status 

#endif
