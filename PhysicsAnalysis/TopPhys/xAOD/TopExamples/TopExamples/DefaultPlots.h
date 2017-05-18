/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEFAULTPLOTS_H_
#define DEFAULTPLOTS_H_

namespace top {
class PlotManager;
class Event;

/**
 * @brief Since a few of the analyses might want some 'default' plots we've put
 * the code to make them in this little function.
 *
 * Hopefully the names and axis labels make it clear what they should contain.
 *
 * @param manager This is a little package that lets you store a bunch of
 * histograms and then refer to them by name.
 */
void addPlots(top::PlotManager& manager);

/**
 * @brief Fill the plots made by the addPlots function. This should be run every
 * event.
 *
 * @param topEvent The 'event' which contains electrons, muons, jets, met etc.
 * @param manager The histogram manager that the plots were added to.  It's used
 * to access them via their name, and then fill them.
 * @param eventWeight Maybe you're running on MC simulation and the events are
 * weighted and you want to plot them with weights?
 */
void fillPlots(const top::Event& topEvent, top::PlotManager& manager, double eventWeight);

}

#endif
