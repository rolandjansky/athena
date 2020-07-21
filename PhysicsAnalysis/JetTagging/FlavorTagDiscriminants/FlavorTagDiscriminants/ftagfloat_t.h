#ifndef FTAG_FLOAT_T_H
#define FTAG_FLOAT_T_H

/// This is the type that we use for IPRNN, IP3D, and DL1
///
/// It has historically been a double, but it should eventuall be a
/// float. See this ticket
///
/// https://its.cern.ch/jira/browse/ATLASRECTS-5201
///

using ftagfloat_t = double;

#endif
